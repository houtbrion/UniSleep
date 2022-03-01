#ifndef __UNI_SLEEP_H__
#define __UNI_SLEEP_H__

#include "detectArduinoHardware.h"

#define _UNSUPPORTED_MODE_ -1

#define UNI_SLEEP_SUCCESS 1
#define UNI_SLEEP_FAILURE 0
#define UNI_SLEEP_UNSUPPORTED_MODE     -1
#define UNI_SLEEP_UNSUPPORTED_FUNCTION -2
#define UNI_SLEEP_UNSUPPORTED_DURATION -3
#define UNI_SLEEP_ILLEGAL_PIN_NUMBER   -4
#define UNI_SLEEP_NULL_CALLBACK        -5
#define UNI_SLEEP_ILLEGAL_CALLBACK_NUMBER -6
#define UNI_SLEEP_CALLBACK_ALREADY_IN_USE -7

#define UNI_SLEEP_INTERRUPT_NUM 8

#if CPU_ARCH!=XTENSA_LX6_ARCH
typedef enum {
ESP_SLEEP_WAKEUP_EXT0,
ESP_SLEEP_WAKEUP_EXT1,
ESP_SLEEP_WAKEUP_TIMER,
ESP_SLEEP_WAKEUP_TOUCHPAD,
ESP_SLEEP_WAKEUP_ULP,
ESP_SLEEP_WAKEUP_GPIO,
ESP_SLEEP_WAKEUP_UART,
ESP_SLEEP_WAKEUP_WIFI,
ESP_SLEEP_WAKEUP_BT,
ESP_SLEEP_WAKEUP_UNDEFINED,
ESP_SLEEP_WAKEUP_ALL,
ESP_SLEEP_WAKEUP_COCPU,
ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG
} esp_sleep_source_t;
//} sleepType;

typedef enum {
  ESP_EXT1_WAKEUP_ALL_LOW,
  ESP_EXT1_WAKEUP_ANY_HIGH
} esp_sleep_ext1_wakeup_mode_t;

typedef enum {
    GPIO_NUM_0 = 0,     /*!< GPIO0, input and output */
    GPIO_NUM_1 = 1,     /*!< GPIO1, input and output */
    GPIO_NUM_2 = 2,     /*!< GPIO2, input and output */
    GPIO_NUM_3 = 3,     /*!< GPIO3, input and output */
    GPIO_NUM_4 = 4,     /*!< GPIO4, input and output */
    GPIO_NUM_5 = 5,     /*!< GPIO5, input and output */
    GPIO_NUM_6 = 6,     /*!< GPIO6, input and output */
    GPIO_NUM_7 = 7,     /*!< GPIO7, input and output */
    GPIO_NUM_8 = 8,     /*!< GPIO8, input and output */
    GPIO_NUM_9 = 9,     /*!< GPIO9, input and output */
    GPIO_NUM_10 = 10,   /*!< GPIO10, input and output */
    GPIO_NUM_11 = 11,   /*!< GPIO11, input and output */
    GPIO_NUM_12 = 12,   /*!< GPIO12, input and output */
    GPIO_NUM_13 = 13,   /*!< GPIO13, input and output */
    GPIO_NUM_14 = 14,   /*!< GPIO14, input and output */
    GPIO_NUM_15 = 15,   /*!< GPIO15, input and output */
    GPIO_NUM_16 = 16,   /*!< GPIO16, input and output */
    GPIO_NUM_17 = 17,   /*!< GPIO17, input and output */
    GPIO_NUM_18 = 18,   /*!< GPIO18, input and output */
    GPIO_NUM_19 = 19,   /*!< GPIO19, input and output */

    GPIO_NUM_21 = 21,   /*!< GPIO21, input and output */
    GPIO_NUM_22 = 22,   /*!< GPIO22, input and output */
    GPIO_NUM_23 = 23,   /*!< GPIO23, input and output */

    GPIO_NUM_25 = 25,   /*!< GPIO25, input and output */
    GPIO_NUM_26 = 26,   /*!< GPIO26, input and output */
    GPIO_NUM_27 = 27,   /*!< GPIO27, input and output */

    GPIO_NUM_32 = 32,   /*!< GPIO32, input and output */
    GPIO_NUM_33 = 33,   /*!< GPIO32, input and output */
    GPIO_NUM_34 = 34,   /*!< GPIO34, input mode only */
    GPIO_NUM_35 = 35,   /*!< GPIO35, input mode only */
    GPIO_NUM_36 = 36,   /*!< GPIO36, input mode only */
    GPIO_NUM_37 = 37,   /*!< GPIO37, input mode only */
    GPIO_NUM_38 = 38,   /*!< GPIO38, input mode only */
    GPIO_NUM_39 = 39,   /*!< GPIO39, input mode only */
    GPIO_NUM_MAX = 40,
} gpio_num_t;

#endif /* CPU_ARCH!=XTENSA_LX6_ARCH */

#if CPU_ARCH!=SAMD_ARCH /* MKR, Zero */
enum adc_interrupt
{
	ADC_INT_BETWEEN,
	ADC_INT_OUTSIDE,
	ADC_INT_ABOVE_MIN,
	ADC_INT_BELOW_MAX,
};
#endif /* CPU_ARCH!=SAMD_ARCH */

#if CPU_ARCH==AVR_ARCH /* AVR */
#include <avr/sleep.h>
#include <avr/power.h>
#define MODE_IDLE     SLEEP_MODE_IDLE
#define MODE_ADC      SLEEP_MODE_ADC
#define MODE_PWR_SAVE SLEEP_MODE_PWR_SAVE
#define MODE_STANDBY  SLEEP_MODE_STANDBY
#define MODE_PWR_DOWN SLEEP_MODE_PWR_DOWN
#endif /* CPU_ARCH==AVR_ARCH */

#if CPU_ARCH==SAMD_ARCH /* MKR, Zero */
#include <ArduinoLowPower.h>
#define MODE_IDLE     1  /* idle() in ArduinoLowPower.h */
#define MODE_ADC      _UNSUPPORTED_MODE_
#ifdef BOARD_HAS_COMPANION_CHIP
#define MODE_PWR_SAVE 3  /* companionSleep() */
#else /* BOARD_HAS_COMPANION_CHIP */
#define MODE_PWR_SAVE _UNSUPPORTED_MODE_
#endif /* BOARD_HAS_COMPANION_CHIP */
#define MODE_STANDBY  4  /* sleep(void)      */
#define MODE_PWR_DOWN 5  /* deepSleep(void)  */
#endif /* CPU_ARCH==SAMD_ARCH */

#if CPU_ARCH==SAM_ARCH /* Due */
#endif /* CPU_ARCH==SAM_ARCH */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
#define MODE_IDLE     _UNSUPPORTED_MODE_
#define MODE_ADC      _UNSUPPORTED_MODE_
#define MODE_PWR_SAVE _UNSUPPORTED_MODE_
#define MODE_STANDBY  4
#define MODE_PWR_DOWN 5
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */

#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
#define MODE_IDLE     _UNSUPPORTED_MODE_
#define MODE_ADC      _UNSUPPORTED_MODE_
#define MODE_PWR_SAVE _UNSUPPORTED_MODE_
#define MODE_STANDBY  _UNSUPPORTED_MODE_
#define MODE_PWR_DOWN 5
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */

/*
 * 
 */
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
void ExIntCallBack0(void);
void ExIntCallBack1(void);
void ExIntCallBack2(void);
void ExIntCallBack3(void);
void ExIntCallBack4(void);
void ExIntCallBack5(void);
void ExIntCallBack6(void);
void ExIntCallBack7(void);
extern volatile bool uniSleepIntFlag[UNI_SLEEP_INTERRUPT_NUM];
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */


class UniSleep {
  public:
    UniSleep(void);
    bool Sleep(void);
    int SetSleepMode(int, uint64_t);
    int SetSleepMode(int);
    int SetSleepMode(uint64_t);
    int SetSleepMode(int , uint64_t, int , bool , bool , bool , bool);
#if CPU_ARCH==AVR_ARCH
    int SetInterrupt(gpio_num_t pin, int level);
    int SetInterrupt(uint32_t, void (*userFunc)(void), uint32_t);
    int SetInterrupt(uint32_t, uint8_t num, uint32_t);
    int SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level);
    int SetInterrupt(uint32_t , void (*userFunc)(void) , adc_interrupt , uint16_t , uint16_t );
    int UnSetInterrupt(void);
    int UnSetInterrupt(uint32_t);
#endif /* CPU_ARCH==AVR_ARCH */
#if CPU_ARCH==SAMD_ARCH
    int SetInterrupt(gpio_num_t pin, int level);
    int SetInterrupt(uint32_t , voidFuncPtr, irq_mode mode);
    int SetInterrupt(uint32_t , uint8_t num, irq_mode mode);
    int SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level);
    int SetInterrupt(uint32_t , voidFuncPtr , adc_interrupt , uint16_t , uint16_t );
    int UnSetInterrupt(void);
    int UnSetInterrupt(uint32_t);
#endif /* CPU_ARCH==SAMD_ARCH */
#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
    int SetInterrupt(gpio_num_t pin, int level);
    int SetInterrupt(gpio_num_t pin, int num, int level);
    int SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level);
    int SetInterrupt(uint32_t, void (*userFunc)(void), uint32_t);
    int SetInterrupt(uint32_t , void (*userFunc)(void) , adc_interrupt , uint16_t , uint16_t );
    int UnSetInterrupt(void);
    int UnSetInterrupt(esp_sleep_source_t source);
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */
#if CPU_ARCH==XTENSA_LX106_ARCH /* ESP8266 */
    int SetInterrupt(gpio_num_t pin, int level);
    int SetInterrupt(uint32_t, void (*userFunc)(void), uint32_t);
    int SetInterrupt(uint32_t, uint8_t num, uint32_t);
    int SetInterrupt(uint64_t mask, esp_sleep_ext1_wakeup_mode_t level);
    int SetInterrupt(uint32_t , void (*userFunc)(void) , adc_interrupt , uint16_t , uint16_t );
    int UnSetInterrupt(void);
    int UnSetInterrupt(uint32_t);
#endif /* CPU_ARCH==XTENSA_LX106_ARCH */
    int32_t WakeUpReason(void);
    void ClearInterruptFlag(void);
    uint64_t Ext1WakeupStatus(void);
    int TouchpadWakeupStatus(void);
    bool checkSleepMode(uint64_t mode);

  private:
    int _sleep_mode;
#if (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) /* esp8266とesp32のどちらでもない */
    int32_t intPinNum[UNI_SLEEP_INTERRUPT_NUM];
#endif /* (CPU_ARCH!=XTENSA_LX106_ARCH) && (CPU_ARCH!=XTENSA_LX6_ARCH) */

#if (CPU_ARCH==XTENSA_LX106_ARCH) || (CPU_ARCH==XTENSA_LX6_ARCH) /* esp8266とesp32のいずれか */
    uint64_t _sleepDuration;
#else /* (CPU_ARCH==XTENSA_LX106_ARCH) || (CPU_ARCH==XTENSA_LX6_ARCH) */
    uint32_t _sleepDuration;
#endif /* (CPU_ARCH==XTENSA_LX106_ARCH) || (CPU_ARCH==XTENSA_LX6_ARCH) */

#if CPU_ARCH==XTENSA_LX6_ARCH /* ESP32 */
    int uart_num;
    bool ulp_wakeup;
    bool touchpad_wakeup;
    bool gpio_wakeup;
    bool wifi_wakeup;
#endif /* CPU_ARCH==XTENSA_LX6_ARCH */
};


#endif /* __UNI_SLEEP_H__ */
