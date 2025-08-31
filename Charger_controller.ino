#include <Arduino_CAN.h>
#include <stdlib.h>

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


void decode_real_voltage_value(){



}



void setup(){
  Serial.begin(115200);
  while (!Serial) { }

  if (!CAN.begin(CanBitRate::BR_250k)) //エラー防止のため自宅テスト環境は250k似設定。
  {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }
  Serial.println("Can reading is ok");


}

void loop(){
  if (CAN.available()) {
    CanMsg const msg = CAN.read();
    if (msg.id == 0x180){
    Serial.println(msg);
    

    int value = hexCharToDecimal(current_bytes[i]);
    if (value < 0) {
      Serial.println("エラー: 不正な16進文字があります");
      return;
    }
    decimalValue = decimalValue * 16 + value;
  }

  Serial.print("16進数: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(current_bytes[i]);
  }
  Serial.print(" -> 10進数: ");
  Serial.println(decimalValue);

  //Serial.println(value);


    }
  }
}

// if (msg.id == 0x180){
  //  Serial.println(msg);
   // }
