#ifndef _HARDWARE_CONFIG_H_
#define _HARDWARE_CONFIG_H_

#if (__STM32F1__)
#define TFT_ENABLEMENT PB10
#define TFT_DC PA0
#define TFT_RST PA1
#define TFT_CS PA2
#define W25Q64_CS_PIN PA4
#else
// define ANALOG_VRMS_RLC A0
#define TFT_RST 9
#define TFT_DC 8
#define TFT_CS 7
#define W25Q64_CS_PIN 10
#endif

#endif // _HARDWARE_CONFIG_H_
