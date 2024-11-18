#include "D6T_OMRON.h"
#include "SoftWire.h"

SoftWire i2c_omron(NULL, NULL);

uint8_t TxBuffer_omron[64];
uint8_t RxBuffer_omron[64];

void D6T_init(D6T_Sensor *sensor, uint8_t _sda, uint8_t _scl)
{
    sensor->sda = _sda;
    sensor->scl = _scl;
    i2c_omron.setSda(sensor->sda);
    i2c_omron.setScl(sensor->scl);
    i2c_omron.setTxBuffer(TxBuffer_omron, 64);
    i2c_omron.setRxBuffer(RxBuffer_omron, 64);
    i2c_omron.begin();
}

uint8_t calc_crc(uint8_t data)
{
    uint8_t temp;
    for (int i = 0; i < 8; i++)
    {
        temp = data;
        data <<= 1;
        if (temp & 0x80)
        {
            data ^= 0x07;
        }
    }
    return data;
}

bool D6T_checkCRC(uint8_t *data, uint8_t len)
{
    uint8_t crc = calc_crc((D6T_ADDRESS << 1) | 1);
    for (uint8_t i = 0; i < len - 1; i++)
    {
        crc = calc_crc(*(data + i) ^ crc);
    }
    return (crc == *(data + len - 1));
}

D6T D6T_readall(D6T_Sensor *sensor)
{
    D6T sen;
    uint8_t buflen;
    uint8_t read_buf[35];
    bool valid_data = true;

    // Đọc dữ liệu nhiều lần để kiểm tra tính hợp lệ
    const int retry_count = 3;
    for (int retry = 0; retry < retry_count; ++retry)
    {
        i2c_omron.setSda(sensor->sda);
        i2c_omron.setScl(sensor->scl);
        
        i2c_omron.beginTransmission(D6T_ADDRESS);
        i2c_omron.write(D6T_CMD);
        i2c_omron.endTransmission();
        delay(50); // Tăng thời gian chờ để đảm bảo cảm biến kịp phản hồi

        buflen = i2c_omron.requestFrom(D6T_ADDRESS, 35);

        memset(read_buf, 0, sizeof(read_buf));

        for (uint8_t i = 0; i < buflen; i++)
        {
            read_buf[i] = i2c_omron.read();
        }

        if (D6T_checkCRC(read_buf, 35))
        {
            sen.PTAT_temp = (256 * read_buf[1] + read_buf[0]) / 10.0;
            sen.pix_temp[0] = (256 * read_buf[3] + read_buf[2]) / 10.0;
            sen.min_temp = sen.pix_temp[0];
            sen.max_temp = sen.pix_temp[0];
            sen.average_temp = sen.pix_temp[0];

            for (uint8_t i = 1; i < 16; i++)
            {
                sen.pix_temp[i] = (256 * read_buf[2 * i + 3] + read_buf[2 * i + 2]) / 10.0;
                if (sen.pix_temp[i] == 0)
                {
                    valid_data = false;
                    break;
                }
                if (sen.max_temp < sen.pix_temp[i])
                {
                    sen.max_temp = sen.pix_temp[i];
                    sen.max_X = i / 4;
                    sen.max_Y = i % 4;
                }
                if (sen.min_temp > sen.pix_temp[i])
                {
                    sen.min_temp = sen.pix_temp[i];
                }
                sen.average_temp += sen.pix_temp[i];
            }

            if (valid_data)
            {
                sen.average_temp /= 16.0;
                if (sen.average_temp > 10)
                {
                    sen.PEC = 0; // no error
                    sensor->data = sen;
                    return sen;
                }
            }
        }

        delay(50); // Thời gian chờ giữa các lần thử
    }

    sen.PEC = 2; // temperature wrong or invalid data
    memset(&sen, 0, sizeof(D6T)); // Clear all sensor data
    sensor->data = sen;
    return sen;
}
