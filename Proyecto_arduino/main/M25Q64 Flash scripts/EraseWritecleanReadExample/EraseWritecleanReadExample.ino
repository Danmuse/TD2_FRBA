#include <SoftSerialSTM32.h>
#include "w25qxx.h"

#define rxPin PB13
#define txPin PB12
// Set up a new SoftwareSerial object
SoftSerialSTM32 mySerial = SoftSerialSTM32(rxPin, txPin);

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

// NOTE: Consider to read 56.25 pages and then print the bytes into the TFT.
// This process it'd be repeat at least 8 times to complete the 450 pages read (equal to 115200 Bytes)

#define CONTENT_BUFFER_LENGTH 120
uint16_t buffer[CONTENT_BUFFER_LENGTH];
uint16_t bufferRead[500];

void setup() {
  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(2400);
  delay(1000);

  for (size_t idx = 0; idx < CONTENT_BUFFER_LENGTH; idx++) buffer[idx] = idx + 1;
  g_flash.begin();
  
  /*
  W25Qxx_result_t status = g_flash.erase(0, W25Q64xx::SECTOR_4K_BYTES);
  if (status != W25QXX_OK) {
    lDelay(10);
    parseStringToCharPointer("CRASH!!");
    lDelay(10);
    mySerial.print(' ');
    lDelay(10);
    mySerial.print(status);
    exit(0);
  }
  */
  W25Qxx_result_t statusWrite = g_flash.writeClean(buffer, W25Q64xx::UINT16, sizeof(buffer), 0, 0); // , false);
  if (statusWrite != W25QXX_OK) {
    lDelay(10);
    parseStringToCharPointer("CRASH!!");
    lDelay(10);
    mySerial.print(' ');
    lDelay(10);
    mySerial.print(statusWrite);
    exit(0);
  }
  
  W25Qxx_result_t statusRead = g_flash.read(bufferRead, W25Q64xx::UINT16, sizeof(bufferRead));
  if (statusRead != W25QXX_OK) {
    lDelay(10);
    parseStringToCharPointer("CRASH!!");
    lDelay(10);
    mySerial.print(' ');
    lDelay(10);
    mySerial.print(statusRead);
    exit(0);
  }

  for (size_t idx = 0; idx < sizeof(bufferRead); idx++) {
    if (bufferRead[idx]) mySerial.print(bufferRead[idx]);
    else parseStringToCharPointer("0");
    lDelay(10);
    mySerial.print('.');
    lDelay(10);
  }
  parseStringToCharPointer("\n");
}

void loop() { }