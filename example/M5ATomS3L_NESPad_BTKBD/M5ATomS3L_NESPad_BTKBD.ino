//#include <M5Unified.h>
#include <FastLED.h>
#include <BleKeyboard.h>
#include <NESpad.h>

/********************
 * LED Setting
 ********************/
#define NUM_LEDS 1
// ATOM
//#define LED_DATA_PIN 27
// ATOMS3
#define LED_DATA_PIN 35
CRGB leds[NUM_LEDS];

/********************
 * BLE Keyboard settings
 ********************/
//BleKeyboard bleKeyboard("8BitDo Zero 2 gamepad");
BleKeyboard bleKeyboard("NESPadS3L BLE_KBD");

/* HOVERAir X1 Key Assign
https://chrisduke.tv/how-to-use-a-remote-control-with-the-hoverair-x1
C = Fly left: btnU, NES_LEFT
D = Fly right: btnD, NES_RIGHT
F = Fly forward: btnR, NES_UP
E = Fly backwards: btnL, NES_DOWN
I = Ascend/up: btnY, NES_A || NES_UP
G = Descend/down: btnA, NES_A || NES_DOWN
H = Rotate/yaw left: btnX, NES_B
J = Rotate/yaw right: btn_B, NES_A
M = Gimbal down: btnRR, NES_B || NES_DOWN
K = Gimbal up: btnLL, NES_B || NES_UP
N = Start recording: btnSelect, NES_SELECT
O = : btnStart, NES_START
*/

/********************
 * NESPad parameters
 ********************/
// ATOM
//#define nesStrobe 22
//#define nesClock 19
//#define nesData 23
// ATOMS3
#define nesStrobe 5
#define nesClock 6
#define nesData 7

NESpad nintendo = NESpad(nesStrobe,nesClock,nesData);
byte state = 0;
byte oldstate = 0;

void setup() {
//  auto cfg = M5.config();
//  M5.begin(cfg);
  Serial.begin(115200);

  FastLED.addLeds<WS2811, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  // LEDを黒にする
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(10);

  // Nim-BLE Keyboard開始
  Serial.println("NESPad BLE Keyboad");
  bleKeyboard.begin();
}

void loop() {
  // ホストに接続されたら処理を開始
  if(bleKeyboard.isConnected()) {
    // キーの状態を取得
    state = nintendo.buttons();
    Serial.printf("key State: 0x%x\r\n", state);
  
    if(state == 0){
      // キーが押されていないときはキーをリリースする
      bleKeyboard.releaseAll();
      delay(10);
      //LEDを青にする
      leds[0] = CRGB::Blue;
      FastLED.show();
      delay(10);
    }else{
      // キーが変更になったときはキーをリリースする
      if(state != oldstate){
        bleKeyboard.releaseAll();
        delay(10);
      }
      // キーが押されていたらLEDを緑にする
      leds[0] = CRGB::Green;
      FastLED.show();
      delay(10);

      // ボタンの判定
      if(state == NES_A){
        Serial.println("NES_A");
        bleKeyboard.press('j'); // Rotate/yaw right
      }else if(state == NES_B){
        Serial.println("NES_B");
        bleKeyboard.press('h'); // Rotate/yaw left
      }else if(state == NES_SELECT){
        Serial.println("NES_SELECT");
        bleKeyboard.press('n'); // Start recording
      }else if(state == NES_START){
        Serial.println("NES_START");
        bleKeyboard.press('o'); // no action
      }else if(state == NES_UP){
        Serial.println("NES_UP");
        bleKeyboard.press('f'); // Fly forward
      }else if(state == NES_DOWN){
        Serial.println("NES_DOWN");
        bleKeyboard.press('e'); // Fly backwards
      }else if(state == NES_LEFT){
        Serial.println("NES_LEFT");
        bleKeyboard.press('c'); // Fly left
      }else if(state == NES_RIGHT){
        Serial.println("NES_RIGHT");
        bleKeyboard.press('d'); // Fly right
      }else if(state == (NES_A + NES_UP)){
        Serial.println("NES_A + NES_UP");
        bleKeyboard.press('i'); // Ascend/up
      }else if(state == (NES_A + NES_DOWN)){
        Serial.println("NES_A + NES_DOWN");
        bleKeyboard.press('g'); // Descend/down
      }else if(state == (NES_B + NES_UP)){
        Serial.println("NES_B + NES_UP");
        bleKeyboard.press('k'); // Gimbal up
      }else if(state == (NES_B + NES_DOWN)){
        Serial.println("NES_B + NES_DOWN");
        bleKeyboard.press('m'); // Gimbal down
      }else{
        Serial.println("Unknown");
      }
      oldstate = state;

    }
    delay(100);
  }else{
    // 未接続時はLEDを赤-青で点滅
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(500);
    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(500);
  }
}
