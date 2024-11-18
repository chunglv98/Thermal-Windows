#ifndef D6T_OMRON_H
#define D6T_OMRON_H

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#define D6T_ADDRESS 0x0A  // Địa chỉ I2C của cảm biến D6T
#define D6T_CMD 0x4C      // Lệnh để đọc dữ liệu từ cảm biến

typedef struct
{
    float PTAT_temp;
    float pix_temp[16];
    float min_temp;
    float max_temp;
    float average_temp;
    uint8_t max_X;
    uint8_t max_Y;
    uint8_t PEC;
} D6T;

typedef struct
{
    uint8_t sda;
    uint8_t scl;
    D6T data;
} D6T_Sensor;

void D6T_init(D6T_Sensor *sensor, uint8_t _sda, uint8_t _scl);
D6T D6T_readall(D6T_Sensor *sensor);

#endif
