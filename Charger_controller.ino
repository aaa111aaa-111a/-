#include <Arduino_CAN.h>
#include <stdlib.h>
#include "Config.h"

int8_t msg_current[8];  //CANのmsgを格納する変数
double msg_temp[8];
double msg_cell_volt[108];

double real_current_value;  //復元した値を格納する変数
double real_temp_value[5];
double real_cell_volt_value[95];

unsigned char temp_data[8];
///////////////////////////////////////////////

void decode_current_from_can(int8_t msg_current[]){ //電流の単位はA

 int32_t v = ((int32_t)(msg_current[0] & 0xFF) << 24) |
             ((int32_t)(msg_current[1] & 0xFF) << 16) |
             ((int32_t)(msg_current[2] & 0xFF) << 8) |
             ((int32_t)(msg_current[3] & 0xFF));
 double real_current_value = v / 100.0; //これが本当の電流の値A

}

void decode_temp_from_can(double msg_temp[]){
 uint16_t temp_value_tmp = 0;

    // temp_value[0]
    temp_value_tmp = ((uint16_t)msg_temp[0] << 2) | ((uint16_t)msg_temp[1] & 0xC0) >> 6;
    real_temp_value[0] = temp_value_tmp / 10.0;

    // temp_value[1]
    temp_value_tmp = ((((uint16_t)(msg_temp[1])) & 0x3F) << 4) | (((uint16_t)msg_temp[2]) & 0xF0) >> 4;
    real_temp_value[1] = temp_value_tmp / 10.0;

    // temp_value[2]
    temp_value_tmp = (((uint16_t)(msg_temp[2]) & 0x0F) << 6) | ((uint16_t)msg_temp[3] & 0xFC) >> 2; //0xFCは11111100
    real_temp_value[2] = temp_value_tmp / 10.0;

    // temp_value[3]
    temp_value_tmp = ((((uint16_t)(msg_temp[3])) & 0x03) << 8) | (uint16_t)msg_temp[4];
    real_temp_value[3] = temp_value_tmp / 10.0;

    // temp_value[4]
    temp_value_tmp = ((uint16_t)msg_temp[5]) << 2 | (((uint16_t)msg_temp[6]) & 0xC0) >> 6;
    real_temp_value[4] = temp_value_tmp / 10.0;

}





void decode_voltage_from_can(uint16_t msg_id, const unsigned char CAN_tx_data[8], uint16_t cell_data[5]) {

  for (int j = 0; j < TOTAL_IC * 3; j++) {
      cell_data[0] = (((uint16_t)CAN_tx_data[0] << 4) | ((uint16_t)CAN_tx_data[1] >> 4)) * 20; //単位は0.1mV
      cell_data[1] = ((((uint16_t)CAN_tx_data[1] & 0x0F) << 8) | (uint16_t)CAN_tx_data[2]) * 20;
      cell_data[2] = (((uint16_t)CAN_tx_data[3] << 4) | ((uint16_t)CAN_tx_data[4] >> 4)) * 20;
      cell_data[3] = ((((uint16_t)CAN_tx_data[4] & 0x0F) << 8) | (uint16_t)CAN_tx_data[5]) * 20;
      cell_data[4] = (((uint16_t)CAN_tx_data[6] << 4) | ((uint16_t)CAN_tx_data[7] >> 4)) * 20;
    

}




void setup(){
  Serial.begin(115200);
  while (!Serial) { }

  if (!CAN.begin(CanBitRate::BR_500k))
  {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }
  Serial.println("Can reading is ok");
  //ピンモード入れる
}

void loop(){
  if (CAN.available()) {
    CanMsg const msg = CAN.read(); //msgがメッセージ本体

    if (msg.id == 0x180) {  //0x180は電流
     for(int i=0; i<8; i++){
      msg_current[i] = msg.data[i];
     }

    decode_current_from_can(msg_current);
    }

    else if ((msg.id >= 0x120) && (msg.id < 0x120 + TOTAL_IC)) { //0x120は温度
      for (int i = 0; i < 8; i++) {
        msg_temp[i] = msg.data[i];
      }
      decode_temp_from_can(msg_temp);
    }

    else if (msg.id >= 0x60 && msg.id < 0x60 + TOTAL_IC * 3) { //6*3の18個 //セル電圧

      for (int i = 0; i < 8; i++) {
        msg_cell_volt[i] = msg.data[i]; //msgに乗っているデータにアクセスしたいときは.data
      }
      decode_voltage_from_can(msg.id, voltage_data, cell_data);
    }


  }


  Serial.println("Current is " + msg);
  Serial.println("Temp from " + j +"is" + msg);
}
