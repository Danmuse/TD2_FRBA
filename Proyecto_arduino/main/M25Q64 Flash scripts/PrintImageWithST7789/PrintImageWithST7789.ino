#include <SoftSerialSTM32.h>
#include "w25qxx.h"
#include "ILI9341.h"

#define rxPin PB13
#define txPin PB12
// Set up a new SoftwareSerial object
SoftSerialSTM32 mySerial = SoftSerialSTM32(rxPin, txPin);

#define BUFFER_IDX_FRAGMENT 6

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
    mySerial.write(string[idx]);
    lDelay(10);
  }
  // result[strlen(string)] = '\0';
  return nullptr;
}

// IMPLEMENTATION: Reading (4 * 128) pages and then print the bytes into the TFT.
// This process it'd be repeat at least 4 times to complete the 512 pages read (equal to 131072 Bytes)
// 512 pages is equal to ((16 * 256 * 8) Bytes [32 KB Block size] * 4 [Image's fragments]) / 256 Bytes [Bytes per page].
// NOTE: It takes 73 mS to process and print a (240 x 240) pixel image when SPI clock is setting to 36 MHz. When using an SD card, it takes 452 mS

#define TFT_LINES_HEIGHT_PER_QUARTER_IMAGE 60
#define QUARTER_IMAGE_BYTES_LENGTH (TFT_LINES_HEIGHT_PER_QUARTER_IMAGE * SCR_WD * BYTES_SIZE_TO_TYPE_16) // 28800 Bytes
#define TOTAL_SECTORS_TO_PROCESS_QUARTER_IMAGE 8 // (7.0312 + 1) \equalTo 8 x 4096 Bytes = 32768
#define TOTAL_FRAGMENTS_TO_PROCESS_ENTIRE_IMAGE (SCR_HT / (((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / SCR_HT))

#define TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE 8 // 8 [(16384 * 2) / 4096]
#define TOTAL_SECTORS_TO_PROCESS_FOURTH_FRAGMENT_IMAGE 5 // 4.125 [(8448 * 2) / 4096]
#define TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE (4 * TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE) // (3 * TOTAL_SECTORS_TO_PROCESS_FRAGMENTS_IMAGE + TOTAL_SECTORS_TO_PROCESS_FOURTH_FRAGMENT_IMAGE)

#define MAX_PAGE_TO_PROCESS_ANY_IMAGE (32768 / 512 - 1)

typedef enum {
  INSTR_IMG = 0,
  INDUC_IMG = 1,
  QMETER_IMG = 2,
  SIG_IMG = 3,
  MBACK_IMG = 4,
  MFRAME_IMG = MAX_PAGE_TO_PROCESS_ANY_IMAGE
} flashImages_t;

void printFromFlash(uint16_t numImage) {
  // It is recommended not to modify the "BUFFER_IDX_FRAGMENT" macro so as not to slow down or affect the representation of the image.
  // If the value of "BUFFER_IDX_FRAGMENT" is 6, 14400 bytes will be stored, that is, 72% of the SRAM memory occupied for the STM32F103x family of microcontrollers.
  W25Qxx_result_t flashStatus = g_flash.getStatus();

  if (flashStatus) {
    drawGradientFault(ERR_SD_NOT_FOUND);
    delay(1000);
  }

  g_tft.fillRect(0, 0, SCR_WD, SCR_HT, DEFAULT_BACKGROUND_COLOR);
  if (flashStatus) return;

  uint32_t timeElapsed = millis();
  uint16_t bufferRead[(QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT];
  uint8_t bufferIdxFragmentInstance = 0;
  for (uint8_t bufferIdxFragment = 0; bufferIdxFragment < TOTAL_FRAGMENTS_TO_PROCESS_ENTIRE_IMAGE; bufferIdxFragment++) {
    bufferIdxFragmentInstance = bufferIdxFragment; /* (bufferIdxFragment < TOTAL_FRAGMENTS_TO_PROCESS_ENTIRE_IMAGE / 2 ? bufferIdxFragment * 2 : (bufferIdxFragment - TOTAL_FRAGMENTS_TO_PROCESS_ENTIRE_IMAGE / 2) * 2 + 1); */
    spiTransaction(W25Q64xx_SPISettings, []() { });
    W25Qxx_result_t statusRead = g_flash.read(bufferRead, W25Q64xx::UINT16, QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragmentInstance) % W25QXX_BYTES_PER_PAGE, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragmentInstance) / W25QXX_BYTES_PER_PAGE + numImage * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
    if (statusRead != W25QXX_OK) {
      lDelay(10);
      parseStringToCharPointer("CRASH!!");
      exit(0);
    }
    spiTransaction(ILI9341_SPISettings, []() { });
    g_tft.drawImage(0, bufferIdxFragmentInstance * (((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / SCR_HT), SCR_WD, ((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / SCR_HT, bufferRead);
  }
  uint32_t result = millis() - timeElapsed;
  // Serial.print("Time elapsed: ");
  // Serial.println(result);
  parseStringToCharPointer("TimeElapsed:"); lDelay(10);
  mySerial.print(result); lDelay(10);
  parseStringToCharPointer("\n");
}

void setup() {
  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);
  delay(1000);
  g_flash.begin();
  
  g_font.init(customRect, SCR_WD, SCR_HT);
  g_tft.begin();
  g_tft.clearScreen();
}

void loop() {
  printFromFlash(0);
  delay(5000);
}