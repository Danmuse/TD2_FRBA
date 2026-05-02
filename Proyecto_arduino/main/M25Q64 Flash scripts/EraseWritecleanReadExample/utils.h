#ifndef _UTILS_H_
#define _UTILS_H_

#include <Arduino.h>
#include "hardwareConfig.h"

#ifndef EXIT_SUCCESS
//! @hideinitializer Indicates successful execution of a C or C++ function.
#define EXIT_SUCCESS 0 //!< <pre><strong>Value:</strong> 0
#endif

#ifndef EXIT_FAILURE
//! @hideinitializer Indicates unsuccessful execution of a C or C++ function.
#define EXIT_FAILURE 1 //!< <pre><strong>Value:</strong> 1
#endif

#define BYTES_SIZE_TO_TYPE_8	1
#define BYTES_SIZE_TO_TYPE_16 2
#define BYTES_SIZE_TO_TYPE_32	4

#define SSEL_IDLE(x) digitalWrite(x, HIGH)
#define SSEL_ACTIVE(x) digitalWrite(x, LOW)

typedef union {
  float FloatType;
  uint32_t UInt32;
  uint16_t UInt16[2];
  uint8_t UInt8[4];
} bytesReg32_ut;

typedef union {
  uint16_t UInt16;
  uint8_t UInt8[2];
} bytesReg16_ut;

//! @brief <b>flag_t</b> enumeration indicates whether a certain states or conditions in the program.
typedef enum {
	FALSE = 0, //!< Indicates that a condition is <tt>false</tt>.
	TRUE  = 1  //!< Indicates that a condition is <tt>true</tt>.
} flag_t;

static bool isNumberExt(uint8_t character);
static void reverse(char *str, byte length);
static uint8_t intToStr(int32_t num, char *str, int decimals);
static char *cftoa(float num, char *str, uint8_t decimals);
static char *citoa(int32_t num, char *str, uint8_t base);

static bool isNumberExt(uint8_t character) {
  return isdigit(character) || character=='-' || character=='+' || character=='.' || character==' ';
}

static void reverse(char *str, byte length) {
  uint8_t start = 0, end = length - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    end--; start++;
  }
}

static uint8_t intToStr(int32_t num, char *str, int decimals) { 
  uint8_t idx = 0;
  if (num == 0) str[idx++] = '0';
  else {
    while (num) { 
      str[idx++] = (num % 10) + '0'; 
      num /= 10; 
    }
  }
  while (idx < decimals) str[idx++] = '0'; 
  reverse(str, idx); 
  str[idx] = '\0'; 
  return idx; 
}

static char *cftoa(float num, char *str, uint8_t decimals) {
  if (num < 0) {
    *str++ = '-';
    num *= -1;
  }
  int32_t integerPart = (int32_t)num; // Extract integer part 
  float floatPart = num - (float)integerPart; // Extract floating part  
  uint8_t idx = intToStr(integerPart, str, 0); // Convert integer part to string
  if (decimals != 0) { // Check for display option after point 
    str[idx++] = '.';
    floatPart *= pow(10, decimals); // Get the value of fraction part upto given number of points after dot.
    intToStr((int32_t)round(floatPart), str + idx, decimals); 
  }
  return str;
}

static char *citoa(int32_t num, char *str, uint8_t base) {
  byte idx = 0; 
  bool isNegative = false;
  if (num == 0) {
    str[idx++] = '0';
    str[idx] = '\0';
    return str;
  }
  if (num < 0 && base == 10) {
    isNegative = true;
    num = -num;
  }
  if (base == 10) idx = intToStr(num, str, 0);
  else {
    while (num != 0) {
      byte rem = num % base;
      str[idx++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
      num = num / base;
    }
    str[idx] = '\0';
    reverse(str, idx);
  }
  if (isNegative) {
    for (byte jdx = idx; jdx > 0; jdx--) 
      str[jdx] = str[jdx - 1];
    str[0] = '-';
    str[++idx] = '\0';
  }
  return str;
}

#endif // _UTILS_H_
