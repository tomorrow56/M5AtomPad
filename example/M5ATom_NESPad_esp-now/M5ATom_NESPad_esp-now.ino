/********************
 * About ESP-NOW, I referred to the following article
 * https://lang-ship.com/blog/work/m5stickc-esp-now-1/
 * 
 * NESPad Arduino Library
 * Original
 *  https://code.google.com/archive/p/nespad/downloads
 * Forked on GiuHub
 *  https://github.com/joshmarinacci/nespad-arduino
 ********************/

#include <M5Atom.h>

/********************
 * NESPad parameters
 ********************/
#define nesStrobe 22
#define nesClock 19
#define nesData 23
NESpad nintendo = NESpad(nesStrobe,nesClock,nesData);
byte state = 0;

/********************
 * ESPNow settings
 ********************/
esp_now_peer_info_t slave;
int modeNES = 0;  //0:Multicast, 1:Specifid MAC address
esp_err_t result;
boolean sending = false;

// Transmit Callback Function
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  //M5.dis.drawpix(0, 0x00FF00);  //LED GREEN
  //delay(100);
  //M5.dis.drawpix(0, 0x0000FF);  //LED BLUE
}

// Receive Callback Function
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.printf("Last Packet Recv from: %s\n", macStr);
  Serial.printf("Last Packet Recv Data(%d): ", data_len);
  for ( int i = 0 ; i < data_len ; i++ ) {
    Serial.print(data[i]);
    Serial.print(" ");
  }
  Serial.println("");

  M5.dis.drawpix(0, 0x800000);  //LED RED
  delay(100);
  M5.dis.drawpix(0, 0x000080);  //LED BLUE
}

void setup() {
  // M5.begin(SerialEnable, I2CEnable, DisplayEnable);
  M5.begin(true, false, true);

  M5.dis.drawpix(0, 0x000080);  //LED BLUE
  delay(100);

  pinMode(39, INPUT);
  if(digitalRead(39) == LOW){
    if(modeNES == 0){
      modeNES = 1;
    }else if(modeNES == 1){
      modeNES = 0;     
    }
  }

  Serial.println("NESPad + ESP-NOW");
  Serial.printf("NES mode: %d\n", modeNES);

  // ESP-NOW Initialize
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }

  memset(&slave, 0, sizeof(slave));
  if(modeNES == 0){
    // MultiCast
    for (int i = 0; i < 6; ++i) {
      slave.peer_addr[i] = (uint8_t)0xff;
    }
  }else if(modeNES == 1){
    // Specified MAC Address
    M5.dis.drawpix(0, 0x000080);  //LED BLUE
    // Change the following variables to your target MAC Address
    slave.peer_addr[0] = (uint8_t)0x10;
    slave.peer_addr[1] = (uint8_t)0x20;
    slave.peer_addr[2] = (uint8_t)0x30;
    slave.peer_addr[3] = (uint8_t)0x40;
    slave.peer_addr[4] = (uint8_t)0x50;
    slave.peer_addr[5] = (uint8_t)0x60;
  }else{
    // MultiCast
    for (int i = 0; i < 6; ++i) {
      slave.peer_addr[i] = (uint8_t)0xff;
    }
  }

  esp_err_t addStatus = esp_now_add_peer(&slave);
  if (addStatus == ESP_OK) {
    // Pair success
    Serial.println("Pair success");
  }

  // ESP-NOW Callback Registration
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  state = nintendo.buttons();
  uint8_t data[1] = {0};

  if(state == 0){
    if(modeNES == 0){
      M5.dis.drawpix(0, 0x000080);  //LED BLUE
    }else if(modeNES == 1){
      M5.dis.drawpix(0, 0x008000);  //LED GREEN
    }else{
      M5.dis.drawpix(0, 0x008080);  //LED CYAN
    }
    if(sending){
      data[0] = 0;
      // Send twice in case of missed transmissions
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      delay(50);
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("STOP");
      sending = false;
    }
  }else{
    M5.dis.drawpix(0, 0xFF0000);  //LED RED
    sending = true;
     if(state & NES_A){
      data[0] = 1;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_A");
    }else if(state & NES_B){
      data[0] = 2;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_B");
    }else if(state & NES_SELECT){
      data[0] = 3;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_SELECT");
    }else if(state & NES_START){
      data[0] = 4;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_START");
    }else if(state & NES_UP){
      data[0] = 5;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_UP");
    }else if(state & NES_DOWN){
      data[0] = 6;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_DOWN");
    }else if(state & NES_LEFT){
      data[0] = 7;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_LEFT");
    }else if(state & NES_RIGHT){
      data[0] = 8;
      result = esp_now_send(slave.peer_addr, data, sizeof(data));
      Serial.println("NES_RIGHT");
    }else{
      Serial.println("Unknown");
    }
    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      Serial.println("Peer not found.");
    } else {
      Serial.println("Not sure what happened");
    }    
  }
  delay(100);
}
