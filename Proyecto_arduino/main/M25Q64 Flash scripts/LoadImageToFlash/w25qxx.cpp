#include "w25qxx.h"

SPISettings W25Q64xx_SPISettings(SPI_PERIPHERAL_DEFAULT_SPEED, MSBFIRST, SPI_MODE0, DATA_SIZE_8BIT);
W25Q64xx g_flash(W25Q64_CS_PIN);

W25Q64xx::W25Q64xx(uint8_t slaveSelect) :
m_selectPin{slaveSelect}, m_statusW25Qxx{W25QXX_OK}, m_deviceID{INCOMPATIBLE_MODULE} { }

void W25Q64xx::reset(void) {
  this->writeRegister((W25QXX_ENABLE_RESET << 8) | W25QXX_RESET);
}

void W25Q64xx::writeRegister(uint8_t data) {
  SSEL_ACTIVE(this->m_selectPin);
  delayMicroseconds(1);
  SPI.transfer(data);
  SSEL_IDLE(this->m_selectPin);
}

void W25Q64xx::writeRegister16(uint16_t data) {
  SSEL_ACTIVE(this->m_selectPin);
  delayMicroseconds(1);
  // SPI.transfer16(data); // NOTE: Uses 32 bytes more than the instructions below
  SPI.transfer(data >> 8);
  SPI.transfer(data & 0xFF);
  SSEL_IDLE(this->m_selectPin);
}

W25QXX_ID_t W25Q64xx::readID(void) {
  uint8_t result[3];
  SSEL_ACTIVE(this->m_selectPin);
  delayMicroseconds(1);
  SPI.transfer(W25QXX_READ_ID);
  SPI.dmaTransfer(nullptr, result, 3);
  SSEL_IDLE(this->m_selectPin);
  uint32_t deviceID = (uint32_t)((result[0] << 16) | (result[1] << 8) | result[2]);
  if (deviceID == (uint32_t)(W25Q64xV_IQ)) return W25Q64xV_IQ;
  else if (deviceID == (uint32_t)(W25Q64xV_IM)) return W25Q64xV_IM;
  return INCOMPATIBLE_MODULE;
}

uint32_t W25Q64xx::getID(void) {
  uint8_t result[3];
  SSEL_ACTIVE(this->m_selectPin);
  delayMicroseconds(1);
  SPI.transfer(W25QXX_READ_ID);
  SPI.dmaTransfer(nullptr, result, 3);
  SSEL_IDLE(this->m_selectPin);
  uint32_t deviceID = (uint32_t)((result[0] << 16) | (result[1] << 8) | result[2]);
  return deviceID;
}

/* TODO: It doesn't work fine
void W25Q64xx::flashBusy(void) {
  uint8_t result[1];
  SSEL_ACTIVE(this->m_selectPin);
  delayMicroseconds(1);
  SPI.transfer(W25QXX_READ_FST_STATUS_REGISTER);
  SPI.dmaTransfer(nullptr, result, 1);
  SSEL_IDLE(this->m_selectPin);
  while (result[0] & 0x01);
}
*/

bool W25Q64xx::checkWEL(void) {
  uint8_t result[1];
  SSEL_ACTIVE(this->m_selectPin);
  delayMicroseconds(1);
  SPI.transfer(W25QXX_READ_FST_STATUS_REGISTER);
  SPI.dmaTransfer(nullptr, result, 1);
  SSEL_IDLE(this->m_selectPin);
  return ((result[0] >> 1) & 0x01);
}

W25Qxx_result_t W25Q64xx::writeEnable(void) {
  this->writeRegister(W25QXX_ENABLE_WRITE);
  delayMicroseconds(1);
  this->m_statusW25Qxx = (this->checkWEL() ? W25QXX_OK : W25QXX_LATCHED_INVALID);
  return this->getStatus(); 
}

W25Qxx_result_t W25Q64xx::writeDisable(void) {
  this->writeRegister(W25QXX_DISABLE_WRITE);
  delayMicroseconds(1);
  this->m_statusW25Qxx = (this->checkWEL() ? W25QXX_LATCHED_INVALID : W25QXX_OK);
  return this->getStatus(); 
}

bool W25Q64xx::acquire(uint8_t data[], size_t numBytes) {
  if (!data) {
    this->m_statusW25Qxx = W25QXX_UPDATE_ERR;
    return EXIT_FAILURE;
  } else SPI.dmaTransfer(nullptr, data, numBytes);
  return EXIT_SUCCESS;
}

bool W25Q64xx::transmit(uint8_t data[], size_t numBytes) {
  if (!data) {
    this->m_statusW25Qxx = W25QXX_UPDATE_ERR;
    return EXIT_FAILURE;
  } else SPI.dmaTransfer(data, nullptr, numBytes);
  return EXIT_SUCCESS;
}

void W25Q64xx::begin(void) {
  pinMode(this->m_selectPin, OUTPUT);
  SSEL_IDLE(this->m_selectPin);
  spiTransaction(W25Q64xx_SPISettings, []() { });
  this->reset();
  delayMicroseconds(50); // The device will take approximately tRST=30us to reset
  this->m_deviceID = this->readID();
}

W25Qxx_result_t W25Q64xx::erase(uint16_t numSectorBlock, eraseType_t eraseSize) {
  if (eraseSize == ENTIRE_CHIP) {
    if (this->writeEnable()) return this->getStatus();
    this->writeRegister(W25QXX_ERASE_ENTIRE_CHIP);
    delay(W25QXX_MAX_CHIP_ERASE_TIME); // The device will take approximately tCE=100s to erase the entire chip
    if (this->writeDisable()) return this->getStatus();
  } else {
    if (eraseSize == BLOCK_64K_BYTES && numSectorBlock > W25QXX_MAX_ERASE_SIZE_TO_BLOCK_64K_QTY - 1) {
      this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
      return this->getStatus();
    } else if (eraseSize == BLOCK_32K_BYTES && numSectorBlock > W25QXX_MAX_ERASE_SIZE_TO_BLOCK_32K_QTY - 1) {
      this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
      return this->getStatus();
    } else if (eraseSize == SECTOR_4K_BYTES && numSectorBlock > W25QXX_MAX_ERASE_SIZE_TO_BLOCK_4K_QTY - 1) {
      this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
      return this->getStatus();
    }

    uint8_t tData[W25QXX_COMMAND_QY_FRAME_LENGTH];
    uint32_t sectorBlockPosition = numSectorBlock * 16 * W25QXX_BYTES_PER_PAGE;
    if (eraseSize == BLOCK_64K_BYTES) {
      tData[0] = W25QXX_ERASE_BLOCK_64K_BYTES;
      sectorBlockPosition *= 16;
    } else if (eraseSize == BLOCK_32K_BYTES) {
      tData[0] = W25QXX_ERASE_BLOCK_32K_BYTES;
      sectorBlockPosition *= 8;
    } else tData[0] = W25QXX_ERASE_BLOCK_4K_BYTES;
    tData[1] = ((sectorBlockPosition >> 16) & 0xFF);
    tData[2] = ((sectorBlockPosition >> 8) & 0xFF);
    tData[3] = (sectorBlockPosition & 0xFF);
    if (this->writeEnable()) return this->getStatus();
    SSEL_ACTIVE(this->m_selectPin);
    delayMicroseconds(1);
    this->transmit(tData, sizeof(tData));
    SSEL_IDLE(this->m_selectPin);
    if (eraseSize == BLOCK_64K_BYTES) delay(W25QXX_MAX_64KB_BLOCK_ERASE_TIME); // The device will take approximately tBE2=2000ms to erase 64K-Bytes starting from sectorBlockPosition
    else if (eraseSize == BLOCK_32K_BYTES) delay(W25QXX_MAX_32KB_BLOCK_ERASE_TIME); // The device will take approximately tBE1=1600ms to erase 32K-Bytes starting from sectorBlockPosition
    else delay(W25QXX_MAX_SECTOR_ERASE_TIME); // The device will take approximately tSE=400ms to erase 4K-Bytes starting from sectorBlockPosition
    // this->flashBusy();
    if (this->writeDisable()) return this->getStatus();
  }
  return this->getStatus();
}

template <typename T> W25Qxx_result_t W25Q64xx::writeClean(T* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage) {
  if (this->m_deviceID == INCOMPATIBLE_MODULE) {
    this->m_statusW25Qxx = W25QXX_INCOMPATIBLE_MODULE;
    return this->getStatus();
  } else this->m_statusW25Qxx = W25QXX_OK;

  uint32_t memAddr = (startPage * W25QXX_BYTES_PER_PAGE) + position;
  if (startPage > W25QXX_PAGES_QTY - 1) {
    this->m_statusW25Qxx = W25QXX_PAGE_BLOCK_INVALID;
    return this->getStatus();
  } else if (memAddr > (W25QXX_BYTES_QTY - (BYTES_SIZE_TO_TYPE_32 * size)) - 1 && (modifier == INT32 || modifier == UINT32 || modifier == FLOAT)) {
    this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
		return this->getStatus();
	} else if (memAddr > (W25QXX_BYTES_QTY - (BYTES_SIZE_TO_TYPE_16 * size)) - 1 && (modifier == INT16 || modifier == UINT16)) {
    this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
		return this->getStatus();
	} else if (memAddr > (W25QXX_BYTES_QTY - (BYTES_SIZE_TO_TYPE_8 * size)) - 1) {
		this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
		return this->getStatus();
  }

  uint16_t endPage = startPage + ((size + position - 1) / W25QXX_BYTES_PER_PAGE);
  uint16_t numPages = (endPage - startPage) + 1;
  uint16_t startSectorInstance = startPage / 16;
  uint16_t numSectors = (endPage / 16 - startSectorInstance) + 1;
//  uint8_t accBlocks64 = numSectors / 16;
//  uint8_t accBlocks32 = (numSectorsBlocks / 8) - (2 * accBlocks64);
//  uint8_t accSectors = (numSectorsBlocks - (8 * accBlocks32)) - (16 * accBlocks64);

  for (size_t idx = 0; idx < numSectors; idx++) 
    if (this->erase(startSectorInstance++, SECTOR_4K_BYTES)) return this->getStatus();

  uint8_t tData[W25QXX_COMMAND_QY_FRAME_LENGTH + W25QXX_BYTES_PER_PAGE];
  uint32_t dataPosition = 0;
  size_t sizeInstance = size;
  uint8_t positionInstance = position;
  uint16_t startPageInstance = startPage;
  for (size_t idx = 0; idx < numPages; idx++) {
    uint16_t bytesRemaining = ((sizeInstance + positionInstance < W25QXX_BYTES_PER_PAGE) ? sizeInstance : (W25QXX_BYTES_PER_PAGE - positionInstance));
    uint16_t bytesToSend = bytesRemaining + W25QXX_COMMAND_QY_FRAME_LENGTH;
    memAddr = (startPageInstance * W25QXX_BYTES_PER_PAGE) + positionInstance;
    if (this->writeEnable()) return this->getStatus();
    tData[0] = W25QXX_PAGE_PROGRAM;
    tData[1] = ((memAddr >> 16) & 0xFF);
    tData[2] = ((memAddr >> 8) & 0xFF);
    tData[3] = (memAddr & 0xFF);
    if (modifier == INT32 || modifier == UINT32 || modifier == FLOAT && !(size % BYTES_SIZE_TO_TYPE_32)) {
      for (size_t jdx = 0; jdx < (bytesRemaining / BYTES_SIZE_TO_TYPE_32); jdx++) {
        tData[(jdx * BYTES_SIZE_TO_TYPE_32) + W25QXX_COMMAND_QY_FRAME_LENGTH] = ((uint32_t)(data[jdx + dataPosition]) >> 24);
        tData[(jdx * BYTES_SIZE_TO_TYPE_32) + W25QXX_COMMAND_QY_FRAME_LENGTH + 1] = (((uint32_t)(data[jdx + dataPosition]) >> 16) & 0xFF);
        tData[(jdx * BYTES_SIZE_TO_TYPE_32) + W25QXX_COMMAND_QY_FRAME_LENGTH + 2] = (((uint32_t)(data[jdx + dataPosition]) >> 8) & 0xFF);
        tData[(jdx * BYTES_SIZE_TO_TYPE_32) + W25QXX_COMMAND_QY_FRAME_LENGTH + 3] = ((uint32_t)(data[jdx + dataPosition]) & 0xFF);
      }
      dataPosition += (bytesRemaining / BYTES_SIZE_TO_TYPE_32);
    } else if (modifier == INT16 || modifier == UINT16 && !(size % BYTES_SIZE_TO_TYPE_16)) {
      for (size_t jdx = 0; jdx < (bytesRemaining / BYTES_SIZE_TO_TYPE_16); jdx++) {
        tData[(jdx * BYTES_SIZE_TO_TYPE_16) + W25QXX_COMMAND_QY_FRAME_LENGTH] = ((uint16_t)(data[jdx + dataPosition]) >> 8);
        tData[(jdx * BYTES_SIZE_TO_TYPE_16) + W25QXX_COMMAND_QY_FRAME_LENGTH + 1] = ((uint16_t)(data[jdx + dataPosition]) & 0xFF);
      }
      dataPosition += (bytesRemaining / BYTES_SIZE_TO_TYPE_16);
    } else if (modifier == INT8 || modifier == UINT8 || modifier == CHAR && !(size % BYTES_SIZE_TO_TYPE_8)) {
      for (size_t jdx = 0; jdx < (bytesRemaining / BYTES_SIZE_TO_TYPE_8); jdx++)
        tData[(jdx * BYTES_SIZE_TO_TYPE_8) + W25QXX_COMMAND_QY_FRAME_LENGTH] = (uint8_t)(data[jdx + dataPosition]);
      dataPosition += (bytesRemaining / BYTES_SIZE_TO_TYPE_8);
    } else {
      if (this->writeDisable()) return this->getStatus();
      this->m_statusW25Qxx = W25QXX_INCORRECT_MODIFIER;
      return this->getStatus();
    }
    SSEL_ACTIVE(this->m_selectPin);
    delayMicroseconds(1);
    this->transmit(tData, bytesToSend);
    SSEL_IDLE(this->m_selectPin);
    delay(W25QXX_MAX_PAGE_PROGRAM_TIME); // The device will take approximately tPP=3ms to write any page
    startPageInstance++;
    sizeInstance -= bytesRemaining;
    positionInstance = 0;
    if (this->writeDisable()) return this->getStatus();
  }
  return this->getStatus();
}

template W25Qxx_result_t W25Q64xx::writeClean<float>(float* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<int32_t>(int32_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<uint32_t>(uint32_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<int16_t>(int16_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<uint16_t>(uint16_t* data, modifierType_t modifier, size_t size, uint8_t position , uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<unsigned short const>(unsigned short const* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<int8_t>(int8_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<char>(char* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::writeClean<uint8_t>(uint8_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);

/* TODO: Check this functions. It's recommend to debbug each code instruction.
template <typename T> W25Qxx_result_t W25Q64xx::write(T* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage) {
  uint16_t endSector = (startPage + ((size + position - 1) / W25QXX_BYTES_PER_PAGE)) / 16;
  uint16_t startSectorInstance = startPage / 16;
  uint16_t numSectors = (endSector - startSectorInstance) + 1;
  uint8_t previousData[16 * W25QXX_BYTES_PER_PAGE];
  uint16_t sectorOffset = ((startPage % 16) * W25QXX_BYTES_PER_PAGE) + position;
  uint32_t dataIdx = 0;

  size_t sizeInstance = size;
  for (size_t idx = 0; idx < numSectors; idx++) {
    uint32_t startPageInstance = startSectorInstance * 16;
    uint16_t bytesRemaining = ((sizeInstance + sectorOffset < (16 * W25QXX_BYTES_PER_PAGE)) ? sizeInstance : ((16 * W25QXX_BYTES_PER_PAGE) - sectorOffset));
    if (this->read(previousData, UINT8, sizeof(previousData), 0, startPageInstance)) return this->getStatus();
    if (modifier == INT32 || modifier == UINT32 || modifier == FLOAT && !(size % BYTES_SIZE_TO_TYPE_32)) {
      for (size_t jdx = 0; jdx < (bytesRemaining / BYTES_SIZE_TO_TYPE_32); jdx++) {
        previousData[(jdx * BYTES_SIZE_TO_TYPE_32) + sectorOffset] = ((uint32_t)(data[jdx + dataIdx]) >> 24);
        previousData[(jdx * BYTES_SIZE_TO_TYPE_32) + sectorOffset + 1] = (((uint32_t)(data[jdx + dataIdx]) >> 16) & 0xFF);
        previousData[(jdx * BYTES_SIZE_TO_TYPE_32) + sectorOffset + 2] = (((uint32_t)(data[jdx + dataIdx]) >> 8) & 0xFF);
        previousData[(jdx * BYTES_SIZE_TO_TYPE_32) + sectorOffset + 3] = ((uint32_t)(data[jdx + dataIdx]) & 0xFF);
      }
      dataIdx += (bytesRemaining / BYTES_SIZE_TO_TYPE_32);
    } else if (modifier == INT16 || modifier == UINT16 && !(size % BYTES_SIZE_TO_TYPE_16)) {
      for (size_t jdx = 0; jdx < (bytesRemaining / BYTES_SIZE_TO_TYPE_16); jdx++) {
        previousData[(jdx * BYTES_SIZE_TO_TYPE_16) + sectorOffset] = ((uint16_t)(data[jdx + dataIdx]) >> 8);
        previousData[(jdx * BYTES_SIZE_TO_TYPE_16) + sectorOffset + 1] = ((uint16_t)(data[jdx + dataIdx]) & 0xFF);
      }
      dataIdx += (bytesRemaining / BYTES_SIZE_TO_TYPE_16);
    } else if (modifier == INT8 || modifier == UINT8 || modifier == CHAR && !(size % BYTES_SIZE_TO_TYPE_8)) {
      for (size_t jdx = 0; jdx < (bytesRemaining / BYTES_SIZE_TO_TYPE_8); jdx++)
        previousData[(jdx * BYTES_SIZE_TO_TYPE_8) + sectorOffset] = (uint8_t)(data[jdx + dataIdx]);
      dataIdx += (bytesRemaining / BYTES_SIZE_TO_TYPE_8);
    } else {
      if (this->writeDisable()) return this->getStatus();
      this->m_statusW25Qxx = W25QXX_INCORRECT_MODIFIER;
      return this->getStatus();
    }
    if (this->write(previousData, UINT8, sizeof(previousData), 0, startPageInstance)) return this->getStatus();
    startSectorInstance++;
    sizeInstance -= bytesRemaining;
    sectorOffset = 0;
  }
  return this->getStatus();  
}

template W25Qxx_result_t W25Q64xx::write<float>(float* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<int32_t>(int32_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<uint32_t>(uint32_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<int16_t>(int16_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<uint16_t>(uint16_t* data, modifierType_t modifier, size_t size, uint8_t position , uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<unsigned short const>(unsigned short const* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<int8_t>(int8_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<char>(char* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
template W25Qxx_result_t W25Q64xx::write<uint8_t>(uint8_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage);
*/

template <typename T> W25Qxx_result_t W25Q64xx::read(T* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading) {
  if (this->m_deviceID == INCOMPATIBLE_MODULE) {
    this->m_statusW25Qxx = W25QXX_INCOMPATIBLE_MODULE;
    return this->getStatus();
  } else this->m_statusW25Qxx = W25QXX_OK;

  uint32_t memAddr = (startPage * W25QXX_BYTES_PER_PAGE) + position;
  if (startPage > W25QXX_PAGES_QTY - 1) {
    this->m_statusW25Qxx = W25QXX_PAGE_BLOCK_INVALID;
    return this->getStatus();
  } else if (memAddr > (W25QXX_BYTES_QTY - (BYTES_SIZE_TO_TYPE_32 * size)) - 1 && (modifier == INT32 || modifier == UINT32 || modifier == FLOAT)) {
		this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
		return this->getStatus();
	} else if (memAddr > (W25QXX_BYTES_QTY - (BYTES_SIZE_TO_TYPE_16 * size)) - 1 && (modifier == INT16 || modifier == UINT16)) {
		this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
		return this->getStatus();
	} else if (memAddr > (W25QXX_BYTES_QTY - (BYTES_SIZE_TO_TYPE_8 * size)) - 1) {
		this->m_statusW25Qxx = W25QXX_OVERFLOW_INVALID;
		return this->getStatus();
  }

#if (__STM32F1__)
  if (((modifier == INT16 || modifier == UINT16 || modifier == INT32 || modifier == UINT32 || modifier == FLOAT) && size * 3 > MAX_BYTES_PROCCESING_BY_STM32F1)) {
    this->m_statusW25Qxx = STM32Fx_SRAM_EXCEPTION;
    return this->getStatus();
  } else if (((modifier == CHAR || modifier == INT8) && size * 2 > MAX_BYTES_PROCCESING_BY_STM32F1)) {
    this->m_statusW25Qxx = STM32Fx_SRAM_EXCEPTION;
    return this->getStatus();
  } else if (modifier == UINT8 && size > MAX_BYTES_PROCCESING_BY_STM32F1) {
    this->m_statusW25Qxx = STM32Fx_SRAM_EXCEPTION;
    return this->getStatus();
  }
#elif (__STM32F4__) || defined(STM32F446xx)
  if (((modifier == INT16 || modifier == UINT16 || modifier == INT32 || modifier == UINT32 || modifier == FLOAT) && size * 3 > MAX_BYTES_PROCCESING_BY_STM32F4)) {
    this->m_statusW25Qxx = STM32Fx_SRAM_EXCEPTION;
    return this->getStatus();
  } else if (((modifier == CHAR || modifier == INT8) && size * 2 > MAX_BYTES_PROCCESING_BY_STM32F4)) {
    this->m_statusW25Qxx = STM32Fx_SRAM_EXCEPTION;
    return this->getStatus();
  } else if (modifier == UINT8 && size > MAX_BYTES_PROCCESING_BY_STM32F4) {
    this->m_statusW25Qxx = STM32Fx_SRAM_EXCEPTION;
    return this->getStatus();
  }
#endif

  uint8_t tData[W25QXX_COMMAND_QY_FRAME_LENGTH + 1];
  tData[0] = (fastReading ? W25QXX_ENABLE_FAST_READ : W25QXX_ENABLE_READ);
  tData[1] = ((memAddr >> 16) & 0xFF);
  tData[2] = ((memAddr >> 8) & 0xFF);
  tData[3] = (memAddr & 0xFF);
  tData[4] = 0; // Dummy clock
  
  SSEL_ACTIVE(this->m_selectPin);
  delayMicroseconds(1);
  this->transmit(tData, sizeof(tData) - (fastReading ? 0 : 1));
	if (modifier == INT32 || modifier == UINT32 || modifier == FLOAT && !(size % BYTES_SIZE_TO_TYPE_32)) {
    uint8_t regs[size];
    bytesReg32_ut regsUnion[size / BYTES_SIZE_TO_TYPE_32];
    size_t sizeInstance = size;
    if (this->acquire(regs, size)) SSEL_IDLE(this->m_selectPin);
    else {
      SSEL_IDLE(this->m_selectPin);
      for (size_t idx = 0; sizeInstance; idx++) {
        regsUnion[idx].UInt8[3] = regs[idx * BYTES_SIZE_TO_TYPE_32];
        regsUnion[idx].UInt8[2] = regs[(idx * BYTES_SIZE_TO_TYPE_32) + 1];
        regsUnion[idx].UInt8[1] = regs[(idx * BYTES_SIZE_TO_TYPE_32) + 2];
        regsUnion[idx].UInt8[0] = regs[(idx * BYTES_SIZE_TO_TYPE_32) + 3];
        if (modifier == FLOAT) data[idx] = regsUnion[idx].FloatType;
        if (modifier == UINT32) data[idx] = regsUnion[idx].UInt32;
        else data[idx] = (int32_t)(regsUnion[idx].UInt32);
        sizeInstance -= BYTES_SIZE_TO_TYPE_32;
      }
    } 
	} else if (modifier == INT16 || modifier == UINT16 && !(size % BYTES_SIZE_TO_TYPE_16)) {
    uint8_t regs[size];
    bytesReg16_ut regsUnion[size / BYTES_SIZE_TO_TYPE_16];
    size_t sizeInstance = size;
    if (this->acquire(regs, size)) SSEL_IDLE(this->m_selectPin);
    else {
      SSEL_IDLE(this->m_selectPin);
      for (size_t idx = 0; sizeInstance; idx++) {
        regsUnion[idx].UInt8[1] = regs[idx * BYTES_SIZE_TO_TYPE_16];
        regsUnion[idx].UInt8[0] = regs[(idx * BYTES_SIZE_TO_TYPE_16) + 1];
        if (modifier == UINT16) data[idx] = regsUnion[idx].UInt16;
        else data[idx] = (int16_t)(regsUnion[idx].UInt16);
        sizeInstance -= BYTES_SIZE_TO_TYPE_16;
      }
    }
  } else if (modifier == INT8 || modifier == UINT8 || modifier == CHAR && !(size % BYTES_SIZE_TO_TYPE_8)) {
		if (modifier == INT8 || modifier == CHAR) {
      uint8_t regs[size];
      size_t sizeInstance = size;
      if (this->acquire(regs, size)) SSEL_IDLE(this->m_selectPin);
      else {
        SSEL_IDLE(this->m_selectPin);
        for (size_t idx = 0; sizeInstance; idx++) {
          if (modifier == INT8) data[idx] = (int8_t)(regs[idx]);
          else data[idx] = (char)(regs[idx]);
          sizeInstance -= BYTES_SIZE_TO_TYPE_8;
        }
      }
    } else {
      if (this->acquire(static_cast<uint8_t*>(static_cast<void*>(data)), size)) SSEL_IDLE(this->m_selectPin);
      else SSEL_IDLE(this->m_selectPin);
    }
	} else {
    SSEL_IDLE(this->m_selectPin);
    this->m_statusW25Qxx = W25QXX_INCORRECT_MODIFIER;
  }
  return this->getStatus();
}

template W25Qxx_result_t W25Q64xx::read<float>(float* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);
template W25Qxx_result_t W25Q64xx::read<int32_t>(int32_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);
template W25Qxx_result_t W25Q64xx::read<uint32_t>(uint32_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);
template W25Qxx_result_t W25Q64xx::read<int16_t>(int16_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);
template W25Qxx_result_t W25Q64xx::read<uint16_t>(uint16_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);
template W25Qxx_result_t W25Q64xx::read<int8_t>(int8_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);
template W25Qxx_result_t W25Q64xx::read<char>(char* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);
template W25Qxx_result_t W25Q64xx::read<uint8_t>(uint8_t* data, modifierType_t modifier, size_t size, uint8_t position, uint16_t startPage, bool fastReading);

W25Qxx_result_t W25Q64xx::powerManagement(bool flashPower) {
  return this->m_statusW25Qxx;
}

W25Qxx_result_t W25Q64xx::getStatus(void) const {
  return this->m_statusW25Qxx;
}

W25Q64xx::~W25Q64xx() { }