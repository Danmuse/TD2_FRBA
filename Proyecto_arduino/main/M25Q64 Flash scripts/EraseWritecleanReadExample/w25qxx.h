#ifndef _W25QXX_H
#define _W25QXX_H

#include "comm.h"

#define W25QXX_ENABLE_RESET 0x66
#define W25QXX_RESET 0x99
#define W25QXX_READ_ID 0x9F
#define W25QXX_READ_FST_STATUS_REGISTER 0x05
#define W25QXX_ENABLE_READ 0x03
#define W25QXX_ENABLE_FAST_READ 0x0B
#define W25QXX_ENABLE_WRITE 0x06
#define W25QXX_DISABLE_WRITE 0x04
#define W25QXX_ERASE_ENTIRE_CHIP 0xC7
#define W25QXX_ERASE_BLOCK_4K_BYTES 0x20
#define W25QXX_ERASE_BLOCK_32K_BYTES 0x52
#define W25QXX_ERASE_BLOCK_64K_BYTES 0xD8
#define W25QXX_PAGE_PROGRAM 0x02

#define W25QXX_BYTES_PER_PAGE 256
#define W25QXX_BLOCKS_QTY 128 // W25Q64JV model (64M-bits)
#define W25QXX_PAGES_QTY (W25QXX_BLOCKS_QTY * 16 * 16) // Equal to 32768 pages
#define W25QXX_BYTES_QTY (W25QXX_PAGES_QTY * W25QXX_BYTES_PER_PAGE) // Equal to 8388608 Bytes
#define W25QXX_MAX_ERASE_SIZE_TO_BLOCK_4K_QTY 2048
#define W25QXX_MAX_ERASE_SIZE_TO_BLOCK_32K_QTY 256
#define W25QXX_MAX_ERASE_SIZE_TO_BLOCK_64K_QTY 128
#define W25QXX_COMMAND_QY_FRAME_LENGTH 4

#define W25QXX_MAX_PAGE_PROGRAM_TIME 3
#define W25QXX_MAX_SECTOR_ERASE_TIME 400
#define W25QXX_MAX_32KB_BLOCK_ERASE_TIME 1600
#define W25QXX_MAX_64KB_BLOCK_ERASE_TIME 2000
#define W25QXX_MAX_CHIP_ERASE_TIME 100000

#define MAX_BYTES_PROCCESING_BY_STM32F1 14400 // 72% of the 20 Kbyte SRAM memory storage
#define MAX_BYTES_PROCCESING_BY_STM32F4 92160 // 72% of the 128 Kbyte SRAM memory storage

typedef enum {
	W25Q64xV_IQ = 0xEF4017,
  W25Q64xV_IM = 0xEF7017,
  INCOMPATIBLE_MODULE = 0
} W25QXX_ID_t;

//! @brief <b>W25Qxx_result_t</b> enumeration reports all possible errors, conditions, warnings, and states in which the W25Qxx memory operations can be found.
typedef enum {
	W25QXX_OK,					        //!< Successful operation.
	W25QXX_UPDATE_ERR,			    //!< No valid data has been acquired or transmitted via SPI communication.
	W25QXX_PAGE_BLOCK_INVALID,	//!< Exceeds the limit of allowed pages or blocks.
	W25QXX_OVERFLOW_INVALID,	  //!< Exceeds the limit of allowed bytes into the desired page or block.
	W25QXX_INCORRECT_MODIFIER,	//!< The method parameter does not match the supported modifiers.
  W25QXX_LATCHED_INVALID,     //!< Error configuring erase/write protection register.
  W25QXX_INCOMPATIBLE_MODULE, //!< This library does not recognize the module.
  STM32Fx_SRAM_EXCEPTION      //!< The buffer size for receiving or transmitting exceeds 72% (14400 bytes) of the 20 Kbyte SRAM memory storage.
} W25Qxx_result_t;

class W25Q64xx {
  private:
    uint8_t m_selectPin;
    W25Qxx_result_t m_statusW25Qxx;
    W25QXX_ID_t m_deviceID;

    void reset(void);
    void writeRegister(uint8_t data);
    void writeRegister16(uint16_t data);

    W25QXX_ID_t readID(void);
    // void flashBusy(void);
    bool checkWEL(void);
    W25Qxx_result_t writeEnable(void);
    W25Qxx_result_t writeDisable(void);
    bool acquire(uint8_t data[], size_t numBytes);
	  bool transmit(uint8_t data[], size_t numBytes);
  public:
    enum modifierType_t { CHAR, UINT8, INT8, UINT16, INT16, UINT32, INT32, FLOAT };
    enum eraseType_t { SECTOR_4K_BYTES, BLOCK_32K_BYTES, BLOCK_64K_BYTES, ENTIRE_CHIP };

    W25Q64xx(uint8_t slaveSelect);
    void begin(void);
    W25Qxx_result_t erase(uint16_t numSectorBlock, eraseType_t eraseSize);
	  template <typename T> W25Qxx_result_t writeClean(T* data, modifierType_t modifier, size_t size, uint8_t position = 0, uint16_t startPage = 0);
    // template <typename T> W25Qxx_result_t write(T* data, modifierType_t modifier, size_t size, uint8_t position = 0, uint16_t startPage = 0);
    template <typename T> W25Qxx_result_t read(T* data, modifierType_t modifier, size_t size, uint8_t position = 0, uint16_t startPage = 0, bool fastReading = true);
    W25Qxx_result_t powerManagement(bool flashPower = true);

    W25Qxx_result_t getStatus(void) const;
    virtual ~W25Q64xx();
};

extern SPISettings W25Q64xx_SPISettings;
extern W25Q64xx g_flash;

#endif // _W25QXX_H