#include "SoftWire.h"

#define SMH_ADDR 0x0A
#define READ_CMD 0x02
#define CONFIG_CMD 0xF7

// typedef enum
// {
//     SMH_OK,
//     SMH_ERR
// } SMH_STATUS;
typedef struct
{
    uint16_t pix_temp[64];
    float max_temp;
    uint8_t max_X;
    uint8_t max_Y;
    bool PEC;
} SMH_01B01_01;

void SMH_init(uint8_t _sda, uint8_t _scl);
SMH_01B01_01 SMH_readall(size_t sensor_index);
void SMH_init_all_sensor(uint8_t sda[], uint8_t scl[], size_t num_i2c);