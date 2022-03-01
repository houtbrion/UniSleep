
#include "UniSleep.h"


#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
volatile bool uniSleepIntFlag[UNI_SLEEP_INTERRUPT_NUM];
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */

UniSleep::UniSleep(void) {
#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
  _sleep_mode=RF_DEFAULT;
#else /* CPU_ARCH==XTENSA_LX106_ARCH */
  _sleep_mode=_UNSUPPORTED_MODE_;
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */
#if CPU_ARCH!=AVR_ARCH /* AVR */
  _sleepDuration=0;
#endif /* CPU_ARCH!=AVR_ARCH */
#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
  uart_num=-1;
  ulp_wakeup=false;
  touchpad_wakeup=false;
  gpio_wakeup=false;
  wifi_wakeup=false;
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  for (int i=0; i < UNI_SLEEP_INTERRUPT_NUM ; i++) {
    uniSleepIntFlag[i]=false;
    this->intPinNum[i]=-1;
  }
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}


#if (CPU_ARCH==SAMD_ARCH) || (CPU_ARCH==XTENSA_LX6_ARCH) /* MKR, Zero  or ESP32 */
int UniSleep::SetSleepMode(int mode, uint64_t time) {
#if CPU_ARCH==SAMD_ARCH
  if (time>0xFFFFFFFF) return UNI_SLEEP_UNSUPPORTED_DURATION;
#endif /* CPU_ARCH==SAMD_ARCH */
  uint32_t duration=(uint32_t) time;
  if (mode == _UNSUPPORTED_MODE_) return UNI_SLEEP_UNSUPPORTED_MODE;
  switch(mode) {
#if MODE_IDLE     != _UNSUPPORTED_MODE_
    case MODE_IDLE:
#endif
#if MODE_ADC      != _UNSUPPORTED_MODE_
    case MODE_ADC:
#endif
#if MODE_PWR_SAVE != _UNSUPPORTED_MODE_
    case MODE_PWR_SAVE:
#endif
#if MODE_STANDBY  != _UNSUPPORTED_MODE_
    case MODE_STANDBY:
#endif
#if MODE_PWR_DOWN != _UNSUPPORTED_MODE_
    case MODE_PWR_DOWN:
#endif
      _sleep_mode=mode;
      break;
    default:
      return UNI_SLEEP_UNSUPPORTED_MODE;
  }
  _sleepDuration=duration;
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==SAMD_ARCH || CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==AVR_ARCH
int UniSleep::SetSleepMode(int mode, uint64_t duration) {
  if (0!=duration) return UNI_SLEEP_UNSUPPORTED_DURATION;
  if (mode == _UNSUPPORTED_MODE_) return UNI_SLEEP_UNSUPPORTED_MODE;
  switch(mode) {
#if MODE_IDLE     != _UNSUPPORTED_MODE_
    case MODE_IDLE:
#endif
#if MODE_ADC      != _UNSUPPORTED_MODE_
    case MODE_ADC:
#endif
#if MODE_PWR_SAVE != _UNSUPPORTED_MODE_
    case MODE_PWR_SAVE:
#endif
#if MODE_STANDBY  != _UNSUPPORTED_MODE_
    case MODE_STANDBY:
#endif
#if MODE_PWR_DOWN != _UNSUPPORTED_MODE_
    case MODE_PWR_DOWN:
#endif
      _sleep_mode=mode;
      break;
    default:
      return UNI_SLEEP_UNSUPPORTED_MODE;
  }
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::SetSleepMode(int mode, uint64_t duration){
  switch(mode){
    case WAKE_RF_DEFAULT:
    case WAKE_RFCAL:
    case WAKE_NO_RFCAL:
    case WAKE_RF_DISABLED:
      break;
    default: return UNI_SLEEP_UNSUPPORTED_MODE;
  }
  _sleepDuration=duration;
  _sleep_mode=mode;
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH!=XTENSA_LX106_ARCH /* ESP8266以外 */
int UniSleep::SetSleepMode(int mode) {
  if (mode == UNI_SLEEP_UNSUPPORTED_MODE) return UNI_SLEEP_UNSUPPORTED_MODE;
  switch(mode) {
#if MODE_IDLE     != _UNSUPPORTED_MODE_
    case MODE_IDLE:
#endif
#if MODE_ADC      != _UNSUPPORTED_MODE_
    case MODE_ADC:
#endif
#if MODE_PWR_SAVE != _UNSUPPORTED_MODE_
    case MODE_PWR_SAVE:
#endif
#if MODE_STANDBY  != _UNSUPPORTED_MODE_
    case MODE_STANDBY:
#endif
#if MODE_PWR_DOWN != _UNSUPPORTED_MODE_
    case MODE_PWR_DOWN:
#endif
      _sleep_mode=mode;
      break;
    default:
      return UNI_SLEEP_UNSUPPORTED_MODE;
  }
#if (CPU_ARCH==SAMD_ARCH) || (CPU_ARCH==XTENSA_LX6_ARCH) /* MKR, Zero  or ESP32 */
  _sleepDuration=0;
#endif /* CPU_ARCH==SAMD_ARCH || CPU_ARCH==XTENSA_LX6_ARCH */
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH!=XTENSA_LX106_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266以外 */
int UniSleep::SetSleepMode(int mode) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH!=XTENSA_LX106_ARCH /* ESP8266以外 */
int UniSleep::SetSleepMode(uint64_t duration){
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH!=XTENSA_LX106_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::SetSleepMode(uint64_t duration){
  _sleepDuration=duration;
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::SetSleepMode(int mode, uint64_t duration, int num, bool ulp, bool pad, bool gpio, bool wifi) {
  if (mode == _UNSUPPORTED_MODE_) return UNI_SLEEP_UNSUPPORTED_MODE;
  switch(mode) {
#if MODE_IDLE     != _UNSUPPORTED_MODE_
    case MODE_IDLE:
#endif
#if MODE_ADC      != _UNSUPPORTED_MODE_
    case MODE_ADC:
#endif
#if MODE_PWR_SAVE != _UNSUPPORTED_MODE_
    case MODE_PWR_SAVE:
#endif
#if MODE_STANDBY  != _UNSUPPORTED_MODE_
    case MODE_STANDBY:
#endif
#if MODE_PWR_DOWN != _UNSUPPORTED_MODE_
    case MODE_PWR_DOWN:
#endif
      _sleep_mode=mode;
      break;
    default:
      return UNI_SLEEP_UNSUPPORTED_MODE;
  }
  if ((_sleep_mode!=MODE_PWR_DOWN) && (gpio)) {
    _sleep_mode=_UNSUPPORTED_MODE_;
    return UNI_SLEEP_UNSUPPORTED_MODE;
  }
  if ((_sleep_mode!=MODE_PWR_DOWN) && (num>=0)) {
    _sleep_mode=_UNSUPPORTED_MODE_;
    return UNI_SLEEP_UNSUPPORTED_MODE;
  }
  if ((_sleep_mode!=MODE_PWR_DOWN) && (wifi)) {
    _sleep_mode=_UNSUPPORTED_MODE_;
    return UNI_SLEEP_UNSUPPORTED_MODE;
  }
  uart_num=num;
  ulp_wakeup=ulp;
  touchpad_wakeup=pad;
  gpio_wakeup=gpio;
  wifi_wakeup=wifi;
  _sleepDuration=duration;
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH!=XTENSA_LX6_ARCH /* ESP32以外 */
int UniSleep::SetSleepMode(int mode, uint64_t duration, int num, bool ulp, bool pad, bool gpio, bool wifi) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH!=XTENSA_LX6_ARCH */

bool UniSleep::Sleep(void) {
#if CPU_ARCH==AVR_ARCH
  delay(100);

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_twi_disable();

  set_sleep_mode(_sleep_mode);
  cli();

  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();

  power_all_enable();
  return true;
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH /* MKR, Zero */
  switch(_sleep_mode) {
    case MODE_IDLE:
      if (_sleepDuration==0) {
        LowPower.idle();
      } else {
        LowPower.idle(_sleepDuration);
      }
      break;
#ifdef BOARD_HAS_COMPANION_CHIP
    case MODE_PWR_SAVE:
      LowPower.companionSleep();
#endif /* BOARD_HAS_COMPANION_CHIP */
    case MODE_STANDBY:
      if (_sleepDuration==0) {
        LowPower.sleep();
      } else {
        LowPower.sleep(_sleepDuration);
      }
      break;
    case MODE_PWR_DOWN:
      if (_sleepDuration==0) {
        LowPower.deepSleep();
      } else {
        LowPower.deepSleep(_sleepDuration);
      }
  }
  return true;
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
  esp_err_t rst;
  if (ulp_wakeup) {
    rst=esp_sleep_enable_ulp_wakeup();
    if ((rst!=ESP_ERR_NOT_SUPPORTED) && (rst!=ESP_OK)) return false;
  }
  if (touchpad_wakeup) {
    rst=esp_sleep_enable_touchpad_wakeup();
    if ((rst!=ESP_ERR_NOT_SUPPORTED) && (rst!=ESP_OK)) return false;
  }
  if (gpio_wakeup) {
    rst=esp_sleep_enable_gpio_wakeup();
    if ((rst!=ESP_ERR_NOT_SUPPORTED) && (rst!=ESP_OK)) return false;
  }
  if (uart_num>=0) {
    rst=esp_sleep_enable_uart_wakeup(uart_num);
    if ((rst!=ESP_ERR_NOT_SUPPORTED) && (rst!=ESP_OK)) return false;
  }
  if (wifi_wakeup) {
    rst=esp_sleep_enable_wifi_wakeup();
    if ((rst!=ESP_ERR_NOT_SUPPORTED) && (rst!=ESP_OK)) return false;
  } else {
    rst=esp_sleep_disable_wifi_wakeup();
    if ((rst!=ESP_ERR_NOT_SUPPORTED) && (rst!=ESP_OK)) return false;
  }
  switch(_sleep_mode) {
    case MODE_STANDBY:
      if (_sleepDuration==0) {
        esp_light_sleep_start();
      } else {
        esp_sleep_enable_timer_wakeup(_sleepDuration);
        esp_light_sleep_start();
      }
      break;
    case MODE_PWR_DOWN:
      if (_sleepDuration==0) {
        esp_deep_sleep_start();
      } else {
        esp_sleep_enable_timer_wakeup(_sleepDuration);
        esp_deep_sleep_start();
      }
    default: return false;
  }
  return true;
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */
#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
  if (_sleep_mode==_UNSUPPORTED_MODE_){
    ESP.deepSleep(_sleepDuration);
  } else {
    ESP.deepSleep(_sleepDuration,(RFMode) _sleep_mode);
  }
  return true;
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */
}

#if CPU_ARCH==AVR_ARCH
int UniSleep::SetInterrupt(uint32_t pin, void (*userFunc)(void), uint32_t mode) {
  if (userFunc==NULL) return UNI_SLEEP_NULL_CALLBACK;
  if (NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)) return UNI_SLEEP_ILLEGAL_PIN_NUMBER;
  if ((LOW!=mode) && (CHANGE!=mode) && (RISING!=mode) && (FALLING!=mode)) return UNI_SLEEP_UNSUPPORTED_MODE;
  attachInterrupt(digitalPinToInterrupt(pin), userFunc, mode);
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH
int UniSleep::SetInterrupt(uint32_t pin, void (*userFunc)(void), irq_mode mode) {
  if (userFunc==NULL) return UNI_SLEEP_NULL_CALLBACK;
  if (NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)) return UNI_SLEEP_ILLEGAL_PIN_NUMBER;
  if ((LOW!=mode) && (CHANGE!=mode) && (RISING!=mode) && (FALLING!=mode) && (HIGH!=mode)) return UNI_SLEEP_UNSUPPORTED_MODE;
  LowPower.attachInterruptWakeup(pin,  userFunc, mode);
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::SetInterrupt(uint32_t pin, void (*userFunc)(void), uint32_t mode) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::SetInterrupt(uint32_t pin, void (*userFunc)(void), uint32_t mode) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH==AVR_ARCH
int UniSleep::SetInterrupt(uint32_t pin, uint8_t num, uint32_t mode) {
  if (NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)) return UNI_SLEEP_ILLEGAL_PIN_NUMBER;
  if (num >= UNI_SLEEP_INTERRUPT_NUM ) return UNI_SLEEP_ILLEGAL_CALLBACK_NUMBER;
  if (this->intPinNum[num]!=-1) return UNI_SLEEP_CALLBACK_ALREADY_IN_USE;
  if ((LOW!=mode) && (CHANGE!=mode) && (RISING!=mode) && (FALLING!=mode)) return UNI_SLEEP_UNSUPPORTED_MODE;
  this->intPinNum[num]=pin;
  switch(num){
    case 0: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack0, mode);break;
    case 1: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack1, mode);break;
    case 2: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack2, mode);break;
    case 3: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack3, mode);break;
    case 4: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack4, mode);break;
    case 5: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack5, mode);break;
    case 6: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack6, mode);break;
    case 7: attachInterrupt(digitalPinToInterrupt(pin), ExIntCallBack7, mode);break;
  }
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH
int UniSleep::SetInterrupt(uint32_t pin, uint8_t num, irq_mode mode) {
  if (NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)) return UNI_SLEEP_ILLEGAL_PIN_NUMBER;
  if (num >= UNI_SLEEP_INTERRUPT_NUM ) return UNI_SLEEP_ILLEGAL_CALLBACK_NUMBER;
  if (this->intPinNum[num]!=-1) return UNI_SLEEP_CALLBACK_ALREADY_IN_USE;
  if ((LOW!=mode) && (CHANGE!=mode) && (RISING!=mode) && (FALLING!=mode) && (HIGH!=mode)) return UNI_SLEEP_UNSUPPORTED_MODE;
  this->intPinNum[num]=pin;
  switch(num){
    case 0:LowPower.attachInterruptWakeup(pin,  ExIntCallBack0, mode);break;
    case 1:LowPower.attachInterruptWakeup(pin,  ExIntCallBack1, mode);break;
    case 2:LowPower.attachInterruptWakeup(pin,  ExIntCallBack2, mode);break;
    case 3:LowPower.attachInterruptWakeup(pin,  ExIntCallBack3, mode);break;
    case 4:LowPower.attachInterruptWakeup(pin,  ExIntCallBack4, mode);break;
    case 5:LowPower.attachInterruptWakeup(pin,  ExIntCallBack5, mode);break;
    case 6:LowPower.attachInterruptWakeup(pin,  ExIntCallBack6, mode);break;
    case 7:LowPower.attachInterruptWakeup(pin,  ExIntCallBack7, mode);break;
  }
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::SetInterrupt(gpio_num_t pin, int num, int level) {
  return SetInterrupt(pin, level);
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::SetInterrupt(uint32_t pin, uint8_t num, uint32_t mode) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH==AVR_ARCH
int UniSleep::SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH
int UniSleep::SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level) {
  if ((level!=ESP_EXT1_WAKEUP_ALL_LOW) && (level!=ESP_EXT1_WAKEUP_ANY_HIGH)) return UNI_SLEEP_UNSUPPORTED_MODE;
  if (ESP_OK == esp_sleep_enable_ext1_wakeup(mask, level)) return UNI_SLEEP_SUCCESS;
  return UNI_SLEEP_FAILURE;
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH==AVR_ARCH
int UniSleep::SetInterrupt(uint32_t pin,  void (*userFunc)(void), adc_interrupt mode, uint16_t lo, uint16_t hi) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH
int UniSleep::SetInterrupt(uint32_t pin, voidFuncPtr callback, adc_interrupt mode, uint16_t lo, uint16_t hi) {
  //if (NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)) return UNI_SLEEP_ILLEGAL_PIN_NUMBER;
  LowPower.attachAdcInterrupt( pin,  callback,  mode,  lo,  hi);
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::SetInterrupt(uint32_t pin, void (*userFunc)(void) , adc_interrupt mode, uint16_t lo, uint16_t hi) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::SetInterrupt(uint32_t pin,  void (*userFunc)(void), adc_interrupt mode, uint16_t lo, uint16_t hi) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH==AVR_ARCH
int UniSleep::SetInterrupt(gpio_num_t pin, int level) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH
int UniSleep::SetInterrupt(gpio_num_t pin, int level) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::SetInterrupt(gpio_num_t pin, int level) {
  if ((level!=HIGH) && (level!=LOW)) return UNI_SLEEP_UNSUPPORTED_MODE;
  if (esp_sleep_is_valid_wakeup_gpio(pin)) {
    if (ESP_OK == esp_sleep_enable_ext0_wakeup(pin, level)) return UNI_SLEEP_SUCCESS;
  };
  return UNI_SLEEP_FAILURE;
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::SetInterrupt(gpio_num_t pin, int level) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH==AVR_ARCH
int UniSleep::UnSetInterrupt(void) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH
int UniSleep::UnSetInterrupt(void) {
  LowPower.detachAdcInterrupt();
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::UnSetInterrupt(void) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::UnSetInterrupt(void) {
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

#if CPU_ARCH==AVR_ARCH
int UniSleep::UnSetInterrupt(uint32_t pin){
  if (NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)) return UNI_SLEEP_ILLEGAL_PIN_NUMBER;
  detachInterrupt(digitalPinToInterrupt(pin));
  for (int i=0; i< UNI_SLEEP_INTERRUPT_NUM;i++) {
    if (this->intPinNum[i]==pin) {
      this->intPinNum[i]=-1;
      uniSleepIntFlag[i]=false;
      break;
    }
  }
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH
int UniSleep::UnSetInterrupt(uint32_t pin){
  if (NOT_AN_INTERRUPT==digitalPinToInterrupt(pin)) return UNI_SLEEP_ILLEGAL_PIN_NUMBER;
  detachInterrupt(digitalPinToInterrupt(pin));
  for (int i=0; i< UNI_SLEEP_INTERRUPT_NUM;i++) {
    if (this->intPinNum[i]==pin) {
      this->intPinNum[i]=-1;
      uniSleepIntFlag[i]=false;
      break;
    }
  }
  return UNI_SLEEP_SUCCESS;
}
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
int UniSleep::UnSetInterrupt(esp_sleep_source_t source) {
  if (ESP_OK==esp_sleep_disable_wakeup_source(source)) return UNI_SLEEP_SUCCESS;
  return UNI_SLEEP_FAILURE;
}
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
int UniSleep::UnSetInterrupt(uint32_t pin){
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
}
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

void UniSleep::ClearInterruptFlag(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  for (int i=0;i<UNI_SLEEP_INTERRUPT_NUM;i++){
    uniSleepIntFlag[i]=false;
  }
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}

int32_t UniSleep::WakeUpReason(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uint8_t rst=0;
  for (int i=0; i < UNI_SLEEP_INTERRUPT_NUM ; i++) {
    uint8_t mask= 1;
    if (i!=0) mask = mask << i;
    if (uniSleepIntFlag[i]){
      rst = rst | mask;
      uniSleepIntFlag[i]=false;
    }
  }
  int32_t retVal=256*rst;
  return retVal;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
  return (uint32_t) esp_sleep_get_wakeup_cause();
#else /* CPU_ARCH==XTENSA_LX6_ARCH (ESP32) */
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
#endif /* CPU_ARCH==XTENSA_LX6_ARCH (ESP32) */
}

uint64_t UniSleep::Ext1WakeupStatus(void) {
#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
  return esp_sleep_get_ext1_wakeup_status();
#else /* CPU_ARCH==XTENSA_LX6_ARCH ESP32 */
  return 0;
#endif /* CPU_ARCH==XTENSA_LX6_ARCH ESP32 */
}

int UniSleep::TouchpadWakeupStatus(void) {
#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
  return (int) esp_sleep_get_touchpad_wakeup_status();
#else /* CPU_ARCH==XTENSA_LX6_ARCH ESP32 */
  return UNI_SLEEP_UNSUPPORTED_FUNCTION;
#endif /* CPU_ARCH==XTENSA_LX6_ARCH ESP32 */
}

bool UniSleep::checkSleepMode(uint64_t mode) {
  if (mode==_UNSUPPORTED_MODE_) return false;
  return true;
}

void ExIntCallBack0(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[0]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}
void ExIntCallBack1(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[1]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}
void ExIntCallBack2(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[2]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}
void ExIntCallBack3(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[3]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}
void ExIntCallBack4(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[4]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}
void ExIntCallBack5(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[5]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}
void ExIntCallBack6(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[6]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}
void ExIntCallBack7(void){
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
  uniSleepIntFlag[7]=true;
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */
}

