
# UniSleepライブラリ

Arduinoおよびその互換機等でCPUを止めてsleepさせるためのライブラリ．

既存のライブラリがCPUアーキテクチャ毎にAPIが違いすぎる上，各アーキテクチャも
使い方などの情報がまとまっているところが見当たらないなど不明な点が多すぎたため，
極力同じようなI/Fでsleep機能を使えるようにするためのライブラリとなっています．

現状のサポート状況は以下の表の通りです。(Arduino DueはSAMアーキなのでサポートしていません)

|CPUアーキ|機種例|サポート状況|備考|
|---|---|---|---|
|AVR|Uno, Megaなど|△|ADC関係は未検証．|
|SAM|Due|×|情報不足のため未実装．|
|SAMD|MKR系列やZero|△|idleモードがうまく動かない．|
|ESP32|ESP32 dev module等|▲|割り込み発生源を個別に有効化，無効化する機能が未実装．EXT1は動作確認できていない．|
|ESP8266|SparkFun esp8266 Thingなど|不明|手持ちの環境で動作検証ができていない．|

現状は，Grove Buttonを使って，割り込みを発生させて起こすのと，時間で自動的に復帰する機能しか検証できていません．
また，表にも書きましたが，開発用PCとesp8266のシリアル接続が不調でesp8266は動作検証できていません．

大部分のCPUアーキの機種では，ADCによってsleepから起こす機能もありますが，使い方がわからないため実装や検証ができていません．
SAMDアーキテクチャは，既存のライブラリ(ArduinoLowPower.h)にADC機能があるためそれを取り込んでいますが，
ドキュメントやソースから類推して実装しただけであるため，動くかどうかも未知数です．

また，本ライブラリは網羅的な試験もできていないので，動作の保証はできない状況です．
持っている装置も少なく，時間も限られているため趣味の範囲では難しいです．

SAM(Arduino Due)もサポートしたいのですが，情報が少なすぎて困ってます．

## 基本的な使い方
### インストール
本ライブラリは内部で「[detectArduinoHardwareライブラリ](https://github.com/houtbrion/detectArduinoHardware)」と「[ArduinoLowPower](https://github.com/arduino-libraries/ArduinoLowPower)」を利用しているため，本ライブラリと「[detectArduinoHardwareライブラリ](https://github.com/houtbrion/detectArduinoHardware)」と「[ArduinoLowPower](https://github.com/arduino-libraries/ArduinoLowPower)」をインストールしてください．

### 使い方

## CPUアーキテクチャによる違いを吸収する方法

### CPUアーキテクチャに依存してしまう部分を``#ifdef``なしにする方法
CPUアーキ毎に，sleepに関するAPIや持っている機能が違いすぎるため，完全には統一できていません．そのため，あるCPUで存在する機能が別のCPUでは存在しない場合，そのAPI用の関数はCPUアーキテクチャ毎に内部で処理を変更しており，該当機能が存在しないCPUでは，返り値として「UNI_SLEEP_UNSUPPORTED_FUNCTION」を返すようになっています．

また，「[detectArduinoHardwareライブラリ](https://github.com/houtbrion/detectArduinoHardware)」の機能を利用することで，以下のように機種毎の違いを``#ifdef``ではなく，プログラムのif文などの制御構造で切り替えることができます．

```
int cpuArch=CPU_ARCH; //CPUアーキテクチャを識別するグローバル変数を定義
(中略)

if (cpuArch==XTENSA_LX106_ARCH) { // ESP8266の場合の処理
    (中略)
}
```

各CPUアーキテクチャに割り当てられている定数は以下の表の通りです．
|CPUアーキテクチャ|定数|
|---|---|
|AVR|AVR_ARCH|
|SAMD|SAMD_ARCH|
|ESP32|XTENSA_LX6_ARCH|
|ESP8266|XTENSA_LX106_ARCH|

### スリープモードの利用可否の判定

各CPUアーキテクチャでサポートする消費電力の動作モードを以下の5種類に分類しています．

|スリープモードを示す定数|AVRの場合の分類|
|---|---|
| MODE_IDLE     | SLEEP_MODE_IDLE     |
| MODE_ADC      | SLEEP_MODE_ADC      |
| MODE_PWR_SAVE | SLEEP_MODE_PWR_SAVE |
| MODE_STANDBY  | SLEEP_MODE_STANDBY  |
| MODE_PWR_DOWN | SLEEP_MODE_PWR_DOWN |

AVRの場合はすべて該当するモードが存在するため，``UniSleep.h``で以下のように定義されています．
```
#define MODE_IDLE     SLEEP_MODE_IDLE
#define MODE_ADC      SLEEP_MODE_ADC
#define MODE_PWR_SAVE SLEEP_MODE_PWR_SAVE
#define MODE_STANDBY  SLEEP_MODE_STANDBY
#define MODE_PWR_DOWN SLEEP_MODE_PWR_DOWN
```

一方，ESP32はスタンバイと消費電力が非常に低い動作モードの2種類しか存在しないため，以下のような定義となっています．
```
#define MODE_IDLE     _UNSUPPORTED_MODE_
#define MODE_ADC      _UNSUPPORTED_MODE_
#define MODE_PWR_SAVE _UNSUPPORTED_MODE_
#define MODE_STANDBY  4
#define MODE_PWR_DOWN 5
```

上の定義を用いて，``#if スリープモードを示す定数!= _UNSUPPORTED_MODE_``とやってもいいのですが，
本ライブラリに含まれるメンバ関数「``bool checkSleepMode(スリープモードを示す定数)``」で，あるスリープモードが
利用可能か否かを判定することができます．添付のサンプルプログラムでは，以下のような使い方をしています．
```
if (helper.checkSleepMode(MODE_IDLE)) {
    MODE_IDLEが利用可能な場合の処理をここに含める
}
```

### MKR系列のボードのUSBシリアルインターフェイスの挙動

SAMDアーキテクチャで動作確認に利用したのが，MKR WiFi1010ですが，
sleep時のシリアルの動きが特殊なため，Sleep前後のSerialへのI/Oは
一度Serialをクローズ(``Serial.end()``)しないとうまく動かないかと思います．

そのため，添付のサンプルプログラムは以下のような関数を準備して，sleep前後で
``Serial.end()``を呼び出すようなコードになっています．
```
/*
 * Serialに書き出した内容を完全にフラッシュする関数
 */
void SerialFlush(void) {
  Serial.flush();
  if (cpuArch==SAMD_ARCH) Serial.end();
}
```
詳細は，添付のサンプルプログラムを参照してください．


# 各種API

## 基本的なAPIの種類

どのCPUアーキテクチャでも，sleepは以下のような手順で進める必要があります．
- (1) クラスオブジェクト生成(本ライブラリ独自の機能)
- (2) sleepモードの選択とパラメータ設定(割り込み関数や自動復帰時間など)
- (3) sleepの準備(I/Oのflushなど)
- (4) sleep
- (5) 復帰理由の確認
- (6) sleepの後始末(I/Oのflushなど)

本ライブラリのAPIは上の6つのうち，(3)と(6)以外は全て対応するものがあります．

### 割り込みでsleep状態から復帰するためのcallback関数(AVRとSAMDの注意事項)
sleep状態から復帰するために，割り込みを用いる場合，ESP32は組み込みの割り込み関数が用意されていますが，AVRやSAMDの標準では準備されていません．
これを補うために，本ライブラリで8個callback関数を用意しています．割り込み発生源が8個までの場合は，プログラマがcallback関数を準備する必要はありませんし，そのため，どのcallback関数が呼び出されたか(どの割り込みが発生したか)をプログラムを書いて判定する手間が軽減されています．

詳細は，``int SetInterrupt(uint32_t, uint8_t num, uint32_t)``と``int UnSetInterrupt(uint32_t)``，``int32_t WakeUpReason(void)``のところを参照してください．

## クラスオブジェクト生成
無引数でクラスオブジェクトを生成してください．サンプルプログラムでは，グローバル変数として本ライブラリのクラスオブジェクトを作成し，その変数のメンバ関数を呼び出す形で各種の機能を利用しています．
```
/*
 * グローバル変数定義
 */
UniSleep helper;
```

## sleep状態への移行
```
bool Sleep(void);
```
このメンバ関数はスリープ状態へ移行させるもので，実際には以下のような使い方をします．
```
/*
 * グローバル変数定義
 */
UniSleep helper;
：
(各種設定は省略)
：
if (helper.Sleep()) {
  スリープからの復帰後の後始末
} else {
  スリープに失敗した場合の処理
}
```


## sleepモードの選択
- ``int SetSleepMode(int mode, uint64_t time)``
- ``int SetSleepMode(int mode)``
- ``int SetSleepMode(uint64_t duration)``
- ``int SetSleepMode(int mode, uint64_t duration, int num, bool ulp, bool pad, bool gpio, bool wifi)``

### ``int SetSleepMode(int mode, uint64_t time)``
引数は，sleep時のCPUの状態と，sleepを時間経過で自動で復帰する場合の期間(esp8266を除く)．第1引数は以下の5つのうち，CPUアーキでサポートしているもの(_UNSUPPORTED_MODE_ではないもの)．
- MODE_IDLE
- MODE_ADC
- MODE_PWR_SAVE
- MODE_STANDBY
- MODE_PWR_DOWN

esp8266では，第1引数がCPUスリープ中のWiFi回路の状態を示す以下の4種類のうちの1つを指定．
- WAKE_RF_DEFAULT
- WAKE_RFCAL
- WAKE_NO_RFCAL
- WAKE_RF_DISABLED

| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_UNSUPPORTED_MODE | 第1引数(``mode``)の値がサポート対象外 |
| UNI_SLEEP_UNSUPPORTED_DURATION | 第2引数(``time``)の値がサポート対象外 |

#### 注意
- AVRは時間での自動復帰機能が存在しないため，第2引数は0でないといけない
- SAMDアーキテクチャの場合は，第2引数はミリ秒単位で32ビット以下の値でなければならない
- ESP8266では，modeはwifiのモードを与える
- ESP8266では，第2引数はマイクロ秒単位の値

### ``int SetSleepMode(int mode)``

引数は，sleep時のCPUの状態で以下の5つのうち，CPUアーキでサポートしているもの(_UNSUPPORTED_MODE_ではないもの)．
- MODE_IDLE
- MODE_ADC
- MODE_PWR_SAVE
- MODE_STANDBY
- MODE_PWR_DOWN

| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_UNSUPPORTED_MODE | 第1引数(``mode``)の値がサポート対象外 |

本メンバ関数がサポートするのは，ESP8266以外の全て．また，ESP8266には該当機能が存在しないため，本関数が呼び出された場合は``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返す．

### ``int SetSleepMode(uint64_t duration)``
esp8266ではスリープ時間を設定し，``UNI_SLEEP_SUCCESS``を返す．esp8266以外のCPUでは該当機能が存在しないため，``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返し何もしない．

### ``int SetSleepMode(int mode, uint64_t duration, int num, bool ulp, bool pad, bool gpio, bool wifi)``
ESP32以外は該当機能が存在しないため，本関数が呼び出された場合は何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返す．

第1引数は，sleep時のCPUの状態で以下の2つ．
- MODE_STANDBY
- MODE_PWR_DOWN

第2引数はスリープ時間(マイクロ秒単位)．第3引数はUARTでスリープから復帰する場合のUART番号．また，第4,5,6,7引数はそれぞれ以下のデバイスでのスリープからの復帰を行うか否かのフラグ．
- ulp
- touch pad
- gpio
- wifi



## 割り込みによるsleepからの復帰設定

### ``int SetInterrupt(uint32_t pin, void (*userFunc)(void), uint32_t mode)``

- 第1引数 : 外部の回路を接続したピンの番号
- 第2引数 : callback関数
- 第3引数 : ピンの状態(HIGH,LOW,RISINGなど)


UNI_SLEEP_SUCCESS
UNI_SLEEP_NULL_CALLBACK
UNI_SLEEP_ILLEGAL_PIN_NUMBER
UNI_SLEEP_UNSUPPORTED_MODE

| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_UNSUPPORTED_MODE | ``mode``の値がサポート対象外 |
| UNI_SLEEP_ILLEGAL_PIN_NUMBER | 割り込みに対応していないピン番号 |
| UNI_SLEEP_NULL_CALLBACK | callback関数がNULLポインタの場合 |

#### ピン番号について
ピン番号のチェックは``NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)``で行っており，割り込みに対応するピンでもsleepからの復帰には使えないものもあるため，注意してください([参考URL](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/))．

#### ESP32とesp2866の場合
ESP32とesp2866は該当機能がないため，なにもせず``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返す．


### ``int UniSleep::SetInterrupt(uint32_t pin, uint8_t num, uint32_t mode)``

- 第1引数 : 外部の回路を接続したピンの番号
- 第2引数 : 組み込みcallback関数の番号
- 第3引数 : ピンの状態(HIGH,LOW,RISINGなど)

標準の開発環境では，AVR, SAMDに対してsleepから復帰する場合に対応したcallback関数を標準で用意することはしていない．一方，ESP32はスリープからの復帰に対応するさまざまなI/Oに対して独自のcallback関数は定義できず，標準のAPIでどのi/oがスリープからの復帰の理由かを取得する仕組みになっている．
しかし，プログラマが独自にcallback関数を定義するよりESP32のような手法の方が使い勝手が良い場合が多い．

そのため，本ライブラリではAVRとSAMDアーキテクチャ用にESPと似た仕組みを用意しました．
このメンバ関数を呼び出すことで，ピン番号と組み込みのcallback関数を結びつけます．なお，組み込みのcallback関数は8個です．


| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_UNSUPPORTED_MODE | ``mode``の値がサポート対象外 |
| UNI_SLEEP_ILLEGAL_PIN_NUMBER | 割り込みに対応していないピン番号 |
| UNI_SLEEP_CALLBACK_ALREADY_IN_USE | 指定されたcallback関数はすでに他のピンに割当て済み |
| UNI_SLEEP_ILLEGAL_CALLBACK_NUMBER | 存在しないcallback関数の番号を指定した |

#### esp8266の場合
該当機能が存在しないため，なにもせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返す．

#### ESP32の場合
内部の動作としては，第2引数は無視して``int SetInterrupt(gpio_num_t pin, int level)``を呼び出しているため，そちらをご覧ください．


### ``int SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level)``
ESP32用の関数で，それ以外のCPUの場合は何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．ESP32の場合，
``esp_err_t esp_sleep_enable_ext1_wakeup(uint64_t mask, esp_sleep_ext1_wakeup_mode_tmode)``を呼び出しているため，各引数の意味については
[参考URL](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html)をご覧ください．

| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_UNSUPPORTED_MODE | ``mode``の値がサポート対象外 |
| UNI_SLEEP_FAILURE | ``esp_sleep_enable_ext1_wakeup()``の実行結果が失敗であった |

### ``int SetInterrupt(uint32_t pin,  void (*userFunc)(void), adc_interrupt mode, uint16_t lo, uint16_t hi)``
本関数はSAMD用のもので，SAMD以外のCPUでは何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．
SAMDの場合は内部で[ArduinoLowPower.h](https://github.com/arduino-libraries/ArduinoLowPower)の``void attachAdcInterrupt(uint32_t pin, voidFuncPtr callback, adc_interrupt mode, uint16_t lo, uint16_t hi)``を呼び出しています．

[公式サイト](https://github.com/arduino-libraries/ArduinoLowPower)にも``attachAdcInterrupt()``のドキュメントはないため，詳細情報は
[ソースコード](https://github.com/arduino-libraries/ArduinoLowPower/blob/master/src/samd/ArduinoLowPower.cpp)を読んでください．

### ``int SetInterrupt(gpio_num_t pin, int level)``
ESP32用の関数で，ESP32以外のCPUでは何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．ESP32では``esp_sleep_is_valid_wakeup_gpio(pin)``でピン番号が不正でないかを確認し，``level``の値のチェックをした上で，``esp_sleep_enable_ext0_wakeup(pin, level)``を呼び出しています．

詳細についは，[公式ガイド](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html)を参照してください．

| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_UNSUPPORTED_MODE | ``mode``の値がサポート対象外 |
| UNI_SLEEP_FAILURE | pin番号がおかしいか，``esp_sleep_enable_ext0_wakeup(pin, level)``の呼び出しに失敗した |

### ``int UnSetInterrupt(void)``
本関数はSAMDアーキテクチャ専用のもので，内部で[ArduinoLowPower.h](https://github.com/arduino-libraries/ArduinoLowPower)の``void detachAdcInterrupt()``を呼び出しています．[ArduinoLowPower.h](https://github.com/arduino-libraries/ArduinoLowPower)の``detachAdcInterrupt()``はvoidなので実行の成功か否かにかかわらず，「``UNI_SLEEP_SUCCESS``」を返します．

SAMD以外のCPUでは，何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．

### ``int UnSetInterrupt(uint32_t pin)``

AVRとSAMDアーキテクチャのCPUでは，``digitalPinToInterrupt(pin)``でピン番号を確認した上で，``detachInterrupt()``で割り込みの対象から外します．また，引数で与えられたピンが内部の組み込みcallback関数に割り当てられている場合は，内部で管理している「使っている/使っていない」フラグをクリアします．返り値は以下の表の通りです．

| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_ILLEGAL_PIN_NUMBER | 割り込みに対応していないピン番号 |

ESP32では，第2引数を[``esp_sleep_source_t``](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html#_CPPv418esp_sleep_source_t)と解釈して，``esp_sleep_disable_wakeup_source()``を呼び出しています．

また，返り値は以下の表の通りです．
| 返り値 | 意味 |
|---|---|
| UNI_SLEEP_SUCCESS | 成功 |
| UNI_SLEEP_FAILURE | ``esp_sleep_disable_wakeup_source()``の呼び出しに失敗した |

esp8266では，該当機能が存在しないため，何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．


### ``void ClearInterruptFlag(void)``
AVRとSAMDアーキテクチャの場合に，内蔵しているcallback関数を使っているか否かを管理するフラグをクリアします．ESP32とesp8266は何もしません．

### ``int32_t WakeUpReason(void)``
ESP32の場合は``esp_sleep_get_wakeup_cause()``を呼び出し，その結果を返り値として返却します．AVRとSAMDの場合，内蔵しているcallback関数8個のうちのどれか利用されたかを参照して，bit列(使われたcallback関数に対応するbitが1)に変換し，さらに8bit左シフトした上で返り値として返却します．esp8266では，該当する機能がないため何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．

この動作をする理由は，全てのCPUアーキテクチャを1つのプログラムで取り扱えるようにするためです．下は添付のサンプルプログラムの例ですが，esp8266の場合はなにもせずに終了し，それ以外のアーキテクチャの場合は32bitの整数で返り値を取得します．

esp32の場合はその値がそのまま``esp_sleep_get_wakeup_cause()``の返り値となるので，case文で割り込みの情報を出力します．AVRとSAMDの場合は，defaultのところに該当し，8ビット右シフトした値を2進数の値として出力しています．

```
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
```

### ``uint64_t Ext1WakeupStatus(void)``

本関数はESP32専用の機能で，ESP32以外のアーキテクチャでは，何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．ESP32では内部で
``uint64_t esp_sleep_get_ext1_wakeup_status()``を呼び出して，その返り値をそのまま返しています．

大まかには，ext1割り込みでスリープから復帰した場合に，どのピン番号が原因で復帰したかを教えてくれます．
詳しい動作については[公式リファレンス](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html#_CPPv432esp_sleep_get_ext1_wakeup_statusv)を参照してください．

### ``int TouchpadWakeupStatus(void)``
本関数はESP32専用の機能で，ESP32以外のアーキテクチャでは，何もせずに``UNI_SLEEP_UNSUPPORTED_FUNCTION``を返します．ESP32では内部で
``uint64_t esp_sleep_get_touchpad_wakeup_status()``を呼び出して，その返り値をそのまま返しています．

大まかには，touchpad操作でスリープから復帰した場合に，復帰した原因を教えてくれます．
詳しい動作については[公式リファレンス](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html#_CPPv436esp_sleep_get_touchpad_wakeup_statusv)


### ``bool checkSleepMode(uint64_t mode)``


以下の5種類のスリープモードが利用可能か否かを判断するための関数になります．

|スリープモードを示す定数|
|---|
| MODE_IDLE     |
| MODE_ADC      |
| MODE_PWR_SAVE |
| MODE_STANDBY  |
| MODE_PWR_DOWN |

もし，引数``mode``で与えられたスリープモードが``_UNSUPPORTED_MODE_``だった場合は``false``を返し，そうでない場合は``true``を返します．
