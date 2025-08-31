#include <Arduino_CAN.h>
#include <stdlib.h>
#include <Config.h>
void decode_real_current_value(){ //real_current_valueを返します。

 int32_t v = ((int32_t)(CAN_tx_data[0] & 0xFF) << 24) |
            ((int32_t)(CAN_tx_data[1] & 0xFF) << 16) |
            ((int32_t)(CAN_tx_data[2] & 0xFF) << 8) |
            ((int32_t)(CAN_tx_data[3] & 0xFF));
 double real_current_value = v / 100.0;

}

void decode_real_temp_value(){ //配列temp_valueを返します。

    uint16_t temp_value_tmp;

    // temp_value[0]
    temp_value_tmp = ((uint16_t)CAN_tx_data[0] << 2) | ((CAN_tx_data[1] & 0xC0) >> 6);
    temp_value[0] = temp_value_tmp / 10.0;

    // temp_value[1]
    temp_value_tmp = ((uint16_t)(CAN_tx_data[1] & 0x3F) << 4) | ((CAN_tx_data[2] & 0xF0) >> 4);
    temp_value[1] = temp_value_tmp / 10.0;

    // temp_value[2]
    temp_value_tmp = ((uint16_t)(CAN_tx_data[2] & 0x0F) << 6) | ((CAN_tx_data[3] & 0xFC) >> 2); //0xFCは11111100
    temp_value[2] = temp_value_tmp / 10.0;

    // temp_value[3]
    temp_value_tmp = ((uint16_t)(CAN_tx_data[3] & 0x03) << 8) | CAN_tx_data[4];
    temp_value[3] = temp_value_tmp / 10.0;

    // temp_value[4]
    temp_value_tmp = ((uint16_t)CAN_tx_data[5] << 2) | ((CAN_tx_data[6] & 0xC0) >> 6);
    temp_value[4] = temp_value_tmp / 10.0;

}


void decode_real_voltage_value(uint16_t msg_id, const unsigned char CAN_tx_data[8], uint16_t cell_data[5]) {
  for (int j = 0; j < TOTAL_IC * 3; j++) {
    if (msg_id == 0x60 + j) {
      cell_data[0] = ((CAN_tx_data[0] << 4) | (CAN_tx_data[1] >> 4)) * 20;
      cell_data[1] = (((CAN_tx_data[1] & 0x0F) << 8) | CAN_tx_data[2]) * 20;
      cell_data[2] = ((CAN_tx_data[3] << 4) | (CAN_tx_data[4] >> 4)) * 20;
      cell_data[3] = (((CAN_tx_data[4] & 0x0F) << 8) | CAN_tx_data[5]) * 20;
      cell_data[4] = ((CAN_tx_data[6] << 4) | (CAN_tx_data[7] >> 4)) * 20;
      break;    // 見つけたらループを抜ける（不要な反復を減らす）
    }
  }
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
float temp_msg; //loopで使う
float current_msg;

}

void loop(){
  if (CAN.available()) {
    CanMsg const msg = CAN.read();
   if (msg.id >= 0x60 && msg.id < 0x60 + TOTAL_IC * 3) {
      // 電圧用バッファにコピー
      unsigned char voltage_data[8];
      for (int i = 0; i < 8; i++) {
        voltage_data[i] = msg.data[i];
      }
      decode_real_voltage_value(msg.id, voltage_data, cell_data);
      
    }

else if (msg.id == 0x180) {
      // 電流用バッファにコピー
      unsigned char current_data[8];
      for (int i = 0; i < 8; i++) {
        current_data[i] = msg.data[i];
      }
      memcpy(CAN_tx_data, current_data, 8); // 既存のdecode_real_current_valueで使うバッファ
      decode_real_current_value();
    }
    else if (msg.id >= 0x120 && msg.id < 0x120 + TOTAL_IC) {
      // 温度用バッファにコピー
      unsigned char temp_data[8];
      for (int i = 0; i < 8; i++) {
        temp_data[i] = msg.data[i];
      }
      memcpy(CAN_tx_data, temp_data, 8);  // 既存のdecode_real_temp_valueで使うバッファ
      decode_real_temp_value();
    }
  }
 
  Serial.println("Current is " + msg);
  Serial.println("Temp from " + j +"is" + msg);
}

// if (msg.id == 0x180){
  //  Serial.println(msg);
   // }
