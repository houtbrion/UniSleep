#include "UniSleep.h"

//#define PIN_MASK 0b100000000000000  // ESP32でEXT1割り込みを使う場合の設定 ピン番号 IO14 
#define PIN_MASK 0 

/*
 * 機種毎の設定
 */
// Mega
#if CPU_ARCH==AVR_ARCH /* AVR */
#define PIN_NUMBER 18 // スイッチのピン番号
#define WIRELESS_STATUS 0
#define MAX_DURATION 0 // sleep duration in ms.
#endif /* CPU_ARCH==AVR_ARCH */

// MKR WiFi 1010
#if CPU_ARCH==SAMD_ARCH /* MKR, Zero */
#define PIN_NUMBER 5  // スイッチのピン番号
#define MAX_DURATION 10000 // sleep duration in ms.
#define WIRELESS_STATUS 0
#endif /* CPU_ARCH==SAMD_ARCH */

// Due
#if CPU_ARCH==SAM_ARCH /* Due */
#define PIN_NUMBER 5  // スイッチのピン番号
#define MAX_DURATION 10000 // sleep duration in ms.
#define WIRELESS_STATUS 0
#endif /* CPU_ARCH==SAM_ARCH */

// ESP32
#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
#define PIN_NUMBER GPIO_NUM_14  // EXT0割り込みを使う場合のピン番号 IO14
#define MAX_DURATION 10000000 // スリープ時間 (マイクロ秒)
#define WIRELESS_STATUS 0
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

// ESP8266
#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
#define MAX_DURATION 10000000 // sleep duration in ms.
#define WIRELESS_STATUS WAKE_RF_DEFAULT
//#define WIRELESS_STATUS WAKE_RFCAL
//#define WIRELESS_STATUS WAKE_NO_RFCAL
//#define WIRELESS_STATUS WAKE_RF_DISABLED
#define PIN_NUMBER 0  // スイッチのピン番号
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */


/*
 * グローバル変数定義
 */
UniSleep helper;
int cpuArch=CPU_ARCH;
uint64_t pinMask=PIN_MASK;

/*
 * コンソールでsleepのモードを選択する機能
 *   ESP8266は，スリープモードが1つしかないため，ここは不要．
 *   使えるモードだけを選択可能にするため，ifdefで切り替え．
 *   
 *   注意 : SAMDアーキの場合，Idle modeはSerial通信している時点で
 *   sleepから抜けてしまうため，スリープの確認は困難．
 */

int SelectSleepMode(void){
  if (cpuArch==XTENSA_LX106_ARCH) return WIRELESS_STATUS;
  while(true){
    Serial.println("==== Sleep mode Selection ====");
    Serial.println("Please input sleep mode character.");
    if (helper.checkSleepMode(MODE_IDLE)) {
      Serial.println("Idle mode       : \"i\" ");
    }
    if (helper.checkSleepMode(MODE_ADC)) {
      Serial.println("Adc  mode       : \"a\" ");
    }
    if (helper.checkSleepMode(MODE_PWR_SAVE)) {
      Serial.println("power Save mode : \"s\" ");
    }
    if (helper.checkSleepMode(MODE_STANDBY)) {
      Serial.println("standBy mode    : \"b\" ");
    }
    if (helper.checkSleepMode(MODE_PWR_DOWN)) {
      Serial.println("power Down mode : \"d\" ");
    }
    Serial.print  ("mode character => ");
    char val;
    while(true){
      if (Serial.available() > 0) {
        val=Serial.read();
        if ((val == '\n')||(val== '\r')) continue;
        break;
      }
    }
    Serial.println(val);
    
    int mode;
    switch(val){
      case 'i' : {
        if (helper.checkSleepMode(MODE_IDLE)) {
          mode=MODE_IDLE;
        } else {
          mode=_UNSUPPORTED_MODE_;
        }
        break;
      }
      case 'a' : {
        if (helper.checkSleepMode(MODE_ADC)) {
          mode=MODE_ADC;
        } else {
          mode=_UNSUPPORTED_MODE_;
        }
        break;
      }
      case 's' : {
        if (helper.checkSleepMode(MODE_PWR_SAVE)) {
          mode=MODE_PWR_SAVE;
        } else {
          mode=_UNSUPPORTED_MODE_;
        }
        break;
      }
      case 'b' : {
        if (helper.checkSleepMode(MODE_STANDBY)) {
          mode=MODE_STANDBY;
        } else {
          mode=_UNSUPPORTED_MODE_;
        }
        break;
      }
      case 'd' : {
        if (helper.checkSleepMode(MODE_PWR_DOWN)) {
          mode=MODE_PWR_DOWN;
        } else {
          mode=_UNSUPPORTED_MODE_;
        }
        break;
      }
      default:mode=_UNSUPPORTED_MODE_;
    }
    if (mode!=_UNSUPPORTED_MODE_) return mode;
    Serial.println("Error: unknown mode character.");
    Serial.println("");
  }
}

/*
 * スリープから起きる際に，PINの操作を使う機種の場合の割り込み設定(ESP8266以外)
 */
void setupInterrupt() {
  if (cpuArch==XTENSA_LX106_ARCH) return;
  pinMode( PIN_NUMBER, INPUT_PULLUP);    // 後の項で使用
  helper.ClearInterruptFlag();
  if (cpuArch==XTENSA_LX6_ARCH) { /* ESP32 */
    if (pinMask!=0) {
      int val = helper.SetInterrupt(pinMask, ESP_EXT1_WAKEUP_ALL_LOW);
      if (!val) {
        Serial.print("set interrupt fail, error = ");
        Serial.println(val);
      }
    } else {
      int val = helper.SetInterrupt(PIN_NUMBER,(uint8_t) 0, HIGH);
      if (!val) {
        Serial.print("set interrupt fail, error = ");
        Serial.println(val);
      }
    }
  } else {
    int val = helper.SetInterrupt(PIN_NUMBER,(uint8_t) 0, FALLING);
    if (!val) {
      Serial.print("set interrupt fail, error = ");
      Serial.println(val);
    }
  }
}

String uint64ToString(uint64_t val){
  uint64_t maskHigh = 0x0000000011111111;
  uint64_t maskLow  = 0x1111111100000000;
  uint32_t high = (uint32_t)  val & maskHigh;
  uint32_t low  = (uint32_t) (val & maskLow)>>32;
  String result = String(high)+String(low);
  return result;
}

/*
 * スリープから目覚めた場合に，理由を出力する関数(タイマは除く)
 *   ESP8266はタイマ以外はリセットしかないため，これは使わない．
 */
void outputIntInfo(void){
  if(cpuArch==XTENSA_LX106_ARCH) return;
  int32_t reason=helper.WakeUpReason();
  switch(reason){
    case ESP_SLEEP_WAKEUP_EXT0            : Serial.println("外部割り込み(RTC_IO)で起動"); break;
    case ESP_SLEEP_WAKEUP_EXT1            : Serial.print("外部割り込み(RTC_CNTL)で起動 IO=");Serial.println(uint64ToString(helper.Ext1WakeupStatus())); break;
    case ESP_SLEEP_WAKEUP_TIMER           : Serial.println("タイマー割り込みで起動"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD        : Serial.print("タッチ割り込みで起動 PAD=");Serial.println(helper.TouchpadWakeupStatus()); break;
    case ESP_SLEEP_WAKEUP_ULP             : Serial.println("ULPプログラムで起動"); break;
    case ESP_SLEEP_WAKEUP_GPIO            : Serial.println("ライトスリープからGPIO割り込みで起動"); break;
    case ESP_SLEEP_WAKEUP_UART            : Serial.println("ライトスリープからUART割り込みで起動"); break;
    case ESP_SLEEP_WAKEUP_WIFI            : Serial.println("ライトスリープからWiFiパケット受信で起動"); break;
    case ESP_SLEEP_WAKEUP_BT              : Serial.println("ライトスリープからBlueToothパケット受信で起動"); break;
    case ESP_SLEEP_WAKEUP_UNDEFINED       : Serial.println("deep sleepから割り込みなどなしに起きた");break;
    case ESP_SLEEP_WAKEUP_COCPU           : Serial.println("co-CPUの割り込みにより、wakeup");break;
    case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG : Serial.println("co-CPUのクラッシュにより、wakeup");break;
    case UNI_SLEEP_UNSUPPORTED_FUNCTION   : Serial.println("スリープからの復帰は未サポートのはず?"); break;
    default                               : Serial.print("wakeup reason : 0b");Serial.println((reason>>8),BIN);
  }
  helper.ClearInterruptFlag();
  Serial.println("clear interrupt flag ... done.");
}

/*
 * Serialに書き出した内容を完全にフラッシュする
 */
void SerialFlush(void) {
  Serial.flush();
  if (cpuArch==SAMD_ARCH) Serial.end();
}

/*
 * 初期化
 */
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // シリアルポートの準備ができるのを待つ
  }
  Serial.println("setup start.");
  setupInterrupt();
  Serial.println("setup done.");
}

/*
 * メインループ
 */
void loop() {
  Serial.println("start of loop()");
  int mode = SelectSleepMode();

  SerialFlush();

  int sleepMode = helper.SetSleepMode(mode, MAX_DURATION);
  if ( sleepMode != UNI_SLEEP_SUCCESS ) { /* AVRの場合は、スリープ時間は無意味 */
    Serial.print("Set sleep mode failure, val = ");Serial.println(sleepMode);
    SerialFlush();
    return;
  }
  if (!helper.Sleep()) {
    Serial.println("fail to sleep.");
    Serial.println("end of loop()");
    Serial.flush();
    return;
  }
  delay(1000);

  outputIntInfo();

  Serial.println("end of loop()");
  Serial.flush();
}
