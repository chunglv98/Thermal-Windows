#include "SMH_01B01_01.h"
#include "Arduino.h"



#define MAX_SENSOR 6

// Define global arrays for SoftWire instances and buffers
SoftWire i2c[MAX_SENSOR] = {
    SoftWire(0, 0),
    SoftWire(0, 0),
    SoftWire(0, 0),
    SoftWire(0, 0),
    SoftWire(0, 0),
    SoftWire(0, 0)
};

uint8_t TxBuffer[MAX_SENSOR][128];
uint8_t RxBuffer[MAX_SENSOR][128];

void SMH_init(uint8_t _sda, uint8_t _scl)
{
    i2c[0].setSda(_sda);
    i2c[0].setScl(_scl);
    i2c[0].setTxBuffer(TxBuffer[0], 128);
    i2c[0].setRxBuffer(RxBuffer[0], 128);
    // i2c[0].begin();
    // i2c[0].end();
}

void SMH_init_all_sensor(uint8_t sda[], uint8_t scl[], size_t num_i2c)
{
    for (size_t i = 0; i < num_i2c; ++i)
    {
        i2c[i].setSda(sda[i]);
        i2c[i].setScl(scl[i]);
        i2c[i].setTxBuffer(TxBuffer[i], 128);
        i2c[i].setRxBuffer(RxBuffer[i], 128);
        // i2c[i].begin();
        // i2c[i].end();
        i2c[i].begin();
        delay(1000);

    i2c[i].beginTransmission(SMH_ADDR);
    i2c[i].write(0xF7);
    i2c[i].write(0x9A);
    i2c[i].endTransmission();

    // i2c[i].beginTransmission(SMH_ADDR);
    // // i2c[i].write(0xF7);
    // i2c[i].write(0x8A);
    // i2c[i].endTransmission();
    
    // i2c[i].beginTransmission(SMH_ADDR);
    // i2c[i].write(0xF1); // Command for setting gain
    // i2c[i].write(0x03); // Gain value for 2000
    // i2c[i].endTransmission();
    }
}

SMH_01B01_01 SMH_readall(size_t sensor_index)
{
    SMH_01B01_01 sen;

    i2c[sensor_index].begin();

    i2c[sensor_index].beginTransmission(SMH_ADDR);
    i2c[sensor_index].write(READ_CMD);
    i2c[sensor_index].endTransmission();

    i2c[sensor_index].requestFrom(SMH_ADDR, 128);

    memset(sen.pix_temp, 0, 64);
    for (uint8_t i = 0; i < 64; i++)
    {
        sen.pix_temp[i] = i2c[sensor_index].read() | i2c[sensor_index].read() << 8;
    }

    i2c[sensor_index].sclLow();
    i2c[sensor_index].sdaLow();

    sen.max_temp = sen.pix_temp[0];
    sen.max_X = 0;
    sen.max_Y = 0;
    for (uint8_t i = 0; i < 64; i++)
    {
        if(sen.max_temp < sen.pix_temp[i])
        {
            sen.max_temp = (float) sen.pix_temp[i];
            sen.max_X = i / 8;
            sen.max_Y = i % 8;
        }
    }
    sen.max_temp = sen.max_temp / 10.00;

    if((sen.max_temp > 20) && (sen.max_temp < 300))
    {
        sen.PEC = true;
    }
    else
    {
        sen.max_temp = 10000;
        sen.PEC = false;
    }

    return sen;
}
