// #include "SoftWire.h"
// #include "D6T_OMRON.h"

// #define MAX_SENSOR 1

// D6T_Sensor sensors[MAX_SENSOR];

// void setup() {
//   Serial.begin(115200);

//   // Initialize each sensor
//   D6T_init(&sensors[0], 6, 7); // Sensor 1
// }

// void loop() {
//   for (int i = 0; i < MAX_SENSOR; ++i) {
//     D6T temp_val = D6T_readall(&sensors[i]);
    
//     if (temp_val.PEC == 0) { // Kiểm tra dữ liệu hợp lệ
//       String sensorData = "";
//       for (int j = 0; j < 16; ++j) {
//         if (j > 0) {
//           sensorData += ","; // Thêm dấu phẩy giữa các giá trị
//         }
//         sensorData += String(temp_val.pix_temp[j], 2); // In ra giá trị với 2 chữ số thập phân
//       }
//       Serial.println(sensorData);
//     } else {
//       Serial.println("Dữ liệu không hợp lệ");
//     }
    
//     // delay(1000); // Dừng 1 giây
//   }
// }


/************************************************************************ */

// #include <Wire.h>
#include "SoftWire.h"
#include "SMH_01B01_01.h"

#define MAX_SENSOR 1

// Define SDA and SCL pins for each sensor
uint8_t sda_pins[MAX_SENSOR] = {7};
uint8_t scl_pins[MAX_SENSOR] = {6};

SMH_01B01_01 sensors[MAX_SENSOR];

// Define the I2C address and the number of bytes to read
#define I2C_DEVICE_ADDRESS 0x0A 
#define NUM_BYTES 128

void setup() {
  Serial.begin(115200);

  // Initialize all sensors
  SMH_init_all_sensor(sda_pins, scl_pins, MAX_SENSOR);
}

void loop() {
  String sensorData = "";

  // Đọc dữ liệu từ cảm biến đầu tiên
  sensors[0] = SMH_readall(0);

  if (sensors[0].PEC) { // Kiểm tra dữ liệu hợp lệ
    // Duyệt qua tất cả 64 giá trị pixel và áp dụng calib
    for (int j = 0; j < 64; ++j) {
      float raw_temp = sensors[0].pix_temp[j] / 10.0; // Giá trị thô từ pixel
      float calib_temp;

      // Áp dụng hệ số calib theo giá trị của raw_temp
      float a, b;
      if (raw_temp < 50.0) {
        a = 1.0; b = 0.0;
      } else if (raw_temp < 58.8) {
        a = 3.664; b = -48.505;
      } else if (raw_temp < 69.3) {
        a = 3.075; b = -14.077;
      } else {
        a = 2.878; b = -1.088;
      }
      calib_temp = raw_temp * a + b; // Tính giá trị calib

      // Nối giá trị calib vào chuỗi sensorData
      if (j > 0) {
        sensorData += ","; // Thêm dấu phẩy giữa các giá trị
      }
      sensorData += String(calib_temp, 2); // Định dạng 2 chữ số thập phân
    }
    Serial.println(sensorData); // In toàn bộ dữ liệu calib của cảm biến
  } else {
    Serial.println("Dữ liệu không hợp lệ");
  }

  delay(100);
}



