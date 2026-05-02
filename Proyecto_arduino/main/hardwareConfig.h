#ifndef _HARDWARE_CONFIG_H_
#define _HARDWARE_CONFIG_H_

#if (__STM32F1__)

#define GPIO_TFT_ENABLEMENT PB10
#define GPIO_TFT_DC         PA0
#define GPIO_TFT_RST        PA1
#define GPIO_TFT_CS         PA2

#define GPIO_AD9833_FSYNC   PA3
#define GPIO_ANALOG_DDS     PB0
#define GPIO_ANALOG_VCLAMP  PB1

#define GPIO_W25Q64_CS_PIN  PA4

#define GPIO_KY040_CLK_PIN  PC13
#define GPIO_KY040_DT_PIN   PC14
#define GPIO_KY040_SW_PIN   PC15

// #define GPIO_BAT_CHARGING   PA12 // Indicates that the battery is charging
// #define GPIO_BAT_FULL       PA13 // Indicates that the battery is full (100%)
// #define GPIO_BAT_SEMI_FULL  PA14 // Indicates that the battery is semi-full (75%)
// #define GPIO_BAT_SEMI_EMPTY PA15 // Indicates that the battery is semi-empty (50%)

#define GPIO_BOOSTER_ENABLEMENT PA15 // PA11 /* HACKED!! */
#define GPIO_MPY634_ENABLEMENT  PB11
#define GPIO_OPA564_ENABLEMENT  PB12

#define GPIO_FSTCAP_ENABLEMENT  PB13
#define GPIO_SNDCAP_ENABLEMENT  PB14
#define GPIO_TRDCAP_ENABLEMENT  PB15
#define GPIO_FTHCAP_ENABLEMENT  PA8

#endif

#endif // _HARDWARE_CONFIG_H_
