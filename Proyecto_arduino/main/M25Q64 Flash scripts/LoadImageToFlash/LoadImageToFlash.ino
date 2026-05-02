#include <SoftSerialSTM32.h>
#include "w25qxx.h"
#include "instr.h"
#include "induc.h"
#include "qmeter.h"
#include "sig.h"
#include "mBack.h"
// #include "mFrame.h"
#include "mFrameEntire.h"
#include "header.h"

/*
#define rxPin PB13
#define txPin PB12
// Set up a new SoftwareSerial object
SoftSerialSTM32 mySerial = SoftSerialSTM32(rxPin, txPin);
*/

inline void lDelay(uint32_t delay) { 
  uint32_t i, j;
  j = delay * 7300;
  for (i = 0; i < j; i++) {
    asm volatile(
      "nop \n\t"
     ::);
  } 
}

char *parseStringToCharPointer(const char *string) {
  // char result[strlen(string) + 1];
  // mySerial.write(strlen(string));
  for (size_t idx = 0; idx < strlen(string); idx++) {
    // result[idx] = string[idx];
    Serial.write(string[idx]);
    lDelay(10);
  }
  // result[strlen(string)] = '\0';
  return nullptr;
}

// #define TFT_LINES_HEIGHT_PER_FRAGMENT_IMAGE 68.26666
// #define TFT_LINES_HEIGHT_FOURTH_FRAGMENT_IMAGE 35.2
// #define TFT_LINES_HEIGHT_SECOND_FRAGMENT_LAST_IMAGE 51.73333
#define FRAGMENTS_IMAGE_BYTES_LENGTH (16384 * 2) // (TFT_LINES_HEIGHT_PER_FRAGMENT_IMAGE * 240 * BYTES_SIZE_TO_TYPE_16) // 32768 Bytes
#define FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH (8448 * 2) // (TFT_LINES_HEIGHT_FOURTH_FRAGMENT_IMAGE * 240 * BYTES_SIZE_TO_TYPE_16) // 16896 Bytes
// #define THIRD_FRAGMENT_LAST_IMAGE_BYTES_LENGTH (12416 * 2) // (TFT_LINES_HEIGHT_SECOND_FRAGMENT_LAST_IMAGE * 240 * BYTES_SIZE_TO_TYPE_16) // 24832 Bytes
#define TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE 8 // 8 [FRAGMENTS_IMAGE_BYTES_LENGTH / 4096]
#define TOTAL_SECTORS_TO_PROCESS_FOURTH_FRAGMENT_IMAGE 5 // 4.125 [FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH / 4096]
#define TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE (4 * TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE) // (3 * TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE + TOTAL_SECTORS_TO_PROCESS_FOURTH_FRAGMENT_IMAGE)

#define TFT_LINES_HEIGHT_PER_QUARTER_IMAGE 60
#define QUARTER_IMAGE_BYTES_LENGTH (TFT_LINES_HEIGHT_PER_QUARTER_IMAGE * 240 * BYTES_SIZE_TO_TYPE_16) // 28800 Bytes

#define MAX_PAGE_TO_PROCESS_ANY_IMAGE (32768 / 512 - 1)

// typedef enum {
#define INSTR_IMG 0           //   INSTR_IMG = 0,
#define INDUC_IMG 1           //   INDUC_IMG = 1,
#define QMETER_IMG 2          //   QMETER_IMG = 2,
#define SIG_IMG 3             //   SIG_IMG = 3,
#define MBACK_IMG 4           //   MBACK_IMG = 4,
#define MFRAME_ENTIRE_IMG 5   //   MFRAME_ENTIRE_IMG = 5,
// NOTE: The following two images need to be written to the EEPROM. Pay attention to the lines commented below.
// #define MFRAME_IMG MAX_PAGE_TO_PROCESS_ANY_IMAGE  //   MFRAME_IMG = MAX_PAGE_TO_PROCESS_ANY_IMAGE
#define HEADER_IMG MAX_PAGE_TO_PROCESS_ANY_IMAGE  //   HEADER_IMG = MAX_PAGE_TO_PROCESS_ANY_IMAGE
// } flashImages_t;

/*** SETTING PARAMETERS SECTION *** SETTING PARAMETERS SECTION *** SETTING PARAMETERS SECTION *** SETTING PARAMETERS SECTION ***/
/*******************************************************************************************************************************/
#define NUM_IMAGE INSTR_IMG // Maximum value must be (32768 [W25QXX_PAGES_QTY] / 512 [TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE] - 1)
// 0 -> Write the flash memory
// 1 -> Read PAGES_TO_READ through the flash memory
#define WRITE_OR_READ 0
#define QUARTER_IMAGE_SELECTED 1 // It could be 0, 1, 2 or 3
/*******************************************************************************************************************************/
/*** SETTING PARAMETERS SECTION *** SETTING PARAMETERS SECTION *** SETTING PARAMETERS SECTION *** SETTING PARAMETERS SECTION ***/

void setup() {
  // Define pin modes for TX and RX
  // pinMode(rxPin, INPUT);
  // pinMode(txPin, OUTPUT);
  // pinMode(TFT_ENABLEMENT, OUTPUT);
  // digitalWrite(TFT_ENABLEMENT, HIGH);
  Serial.begin(9600);
  delay(1000);
  g_flash.begin();
  Serial.println(g_flash.getID());
  
#if (WRITE_OR_READ == 0)

#if (QUARTER_IMAGE_SELECTED == 0)
  // NOTE: Indicate the corresponding image fragment in the first parameter of the function.

#if (NUM_IMAGE == INSTR_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(instrument_ST7789_FST_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == INDUC_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(inductometer_ST7789_FST_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == QMETER_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(q_meter_ST7789_FST_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == SIG_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(signals_generator_ST7789_FST_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MBACK_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuBackground_ST7789_FST_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
// #elif (NUM_IMAGE == MFRAME_IMG)
//   W25Qxx_result_t statusWrite = g_flash.writeClean(menuFrame_ST7789_FST_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MFRAME_ENTIRE_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuFrameEntire_ST7789_FST_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#endif

#elif (QUARTER_IMAGE_SELECTED == 1)
  // NOTE: Indicate the corresponding image fragment in the first parameter of the function.

#if (NUM_IMAGE == INSTR_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(instrument_ST7789_SND_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == INDUC_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(inductometer_ST7789_SND_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == QMETER_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(q_meter_ST7789_SND_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == SIG_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(signals_generator_ST7789_SND_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MBACK_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuBackground_ST7789_SND_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
// #elif (NUM_IMAGE == MFRAME_IMG)
//   W25Qxx_result_t statusWrite = g_flash.writeClean(menuFrame_ST7789_SND_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MFRAME_ENTIRE_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuFrameEntire_ST7789_SND_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#endif

#elif (QUARTER_IMAGE_SELECTED == 2)
  // NOTE: Indicate the corresponding image fragment in the first parameter of the function.

#if (NUM_IMAGE == INSTR_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(instrument_ST7789_TRD_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == INDUC_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(inductometer_ST7789_TRD_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == QMETER_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(q_meter_ST7789_TRD_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == SIG_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(signals_generator_ST7789_TRD_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MBACK_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuBackground_ST7789_TRD_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MFRAME_ENTIRE_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuFrameEntire_ST7789_TRD_QUARTER, W25Q64xx::UINT16, FRAGMENTS_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == HEADER_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(header_ST7789_QUARTER, W25Q64xx::UINT16, HEADER_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#endif

#elif (QUARTER_IMAGE_SELECTED == 3)
  // NOTE: Indicate the corresponding image fragment in the first parameter of the function.

#if (NUM_IMAGE == INSTR_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(instrument_ST7789_FTH_QUARTER, W25Q64xx::UINT16, FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == INDUC_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(inductometer_ST7789_FTH_QUARTER, W25Q64xx::UINT16, FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == QMETER_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(q_meter_ST7789_FTH_QUARTER, W25Q64xx::UINT16, FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == SIG_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(signals_generator_ST7789_FTH_QUARTER, W25Q64xx::UINT16, FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MBACK_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuBackground_ST7789_FTH_QUARTER, W25Q64xx::UINT16, FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#elif (NUM_IMAGE == MFRAME_ENTIRE_IMG)
  W25Qxx_result_t statusWrite = g_flash.writeClean(menuFrameEntire_ST7789_FTH_QUARTER, W25Q64xx::UINT16, FOURTH_FRAGMENT_IMAGE_BYTES_LENGTH, 0, QUARTER_IMAGE_SELECTED * (TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE * 16) + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
#endif

#endif

  if (statusWrite != W25QXX_OK) {
    lDelay(10);
    parseStringToCharPointer("CRASH!!");
    lDelay(10);
    Serial.print(' ');
    lDelay(10);
    Serial.print(statusWrite);
    exit(0);
  }

  lDelay(10);
  parseStringToCharPointer("READY");
  lDelay(10);
  parseStringToCharPointer("\n");

#elif (WRITE_OR_READ != 0)
// It is recommended not to modify the "BUFFER_IDX_FRAGMENT" macro so as not to slow down or affect the representation of the image.
// If the value of "BUFFER_IDX_FRAGMENT" is 6, 14400 bytes will be stored, that is, 72% of the SRAM memory occupied for the STM32F103x family of microcontrollers.
#define BUFFER_IDX_FRAGMENT 6

  lDelay(10);
  parseStringToCharPointer("READING");
  lDelay(10);
  parseStringToCharPointer("\n");
  
  uint16_t offsetCorrection = 0;
  uint16_t bufferRead[(QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT];
  for (size_t bufferIdxFragment = 0; bufferIdxFragment < 240 / (((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / 240); bufferIdxFragment++) {
    spiTransaction(W25Q64xx_SPISettings, []() { });
    // The following offset it's associated with the writing method that was used for this implementation.
    // Since each image is stored in four-part fragments. Therefore, each fragment has 28800 Bytes (28800 x 4 = 115200 Bytes)
    // that are stored in 4 contiguous sectors of 4096 Bytes. The expression used is the following:
    // ((28800 / (16 * 256) + 1) - (28800 / (16 * 256))) * (16 * 256) = (8.0312 - 7.0312) * 4096 = 3968
    // NOTE: It would be possible to use less space in flash memory by modifying the writing method, to store each fragment contiguously.
    // if (!(((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragment) % QUARTER_IMAGE_BYTES_LENGTH) && bufferIdxFragment) offsetCorrection += 3968;
    W25Qxx_result_t statusRead = g_flash.read(bufferRead, W25Q64xx::UINT16, QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragment + offsetCorrection) % W25QXX_BYTES_PER_PAGE, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragment + offsetCorrection) / W25QXX_BYTES_PER_PAGE + NUM_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
    if (statusRead != W25QXX_OK) {
      lDelay(10);
      parseStringToCharPointer("CRASH!!");
      exit(0);
    }
    for (size_t idx = 0; idx < (QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT; idx++) {
      if (bufferRead[idx]) Serial.print(bufferRead[idx]);
      else parseStringToCharPointer("0");
      lDelay(10);
      Serial.print('.');
      lDelay(10);
      if (!(idx % (W25QXX_BYTES_PER_PAGE / BYTES_SIZE_TO_TYPE_16))) parseStringToCharPointer("\n");
    }
  }

#endif

}

void loop() { }