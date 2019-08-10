#include "ResetDetector.h"

ResetDetector::ResetDetector(uint32_t timeoutMs, uint32_t eepromSector) :
  timeoutMs(timeoutMs),
  waitingForDoubleReset(true) {
    setValuableResetReasons({REASON_DEFAULT_RST, REASON_EXT_SYS_RST});
    eeprom.begin((eepromSector + 1) * SPI_FLASH_SEC_SIZE);
}

ResetDetector::~ResetDetector() {
  eeprom.end();
}

ResetDetector& ResetDetector::setValuableResetReasons(std::initializer_list<rst_reason> reasons) {
  isLastResetReasonValuable = std::end(reasons) != std::find(
    std::begin(reasons), std::end(reasons), system_get_rst_info()->reason);
  return *this;
}

uint8_t ResetDetector::readResetCount() {
  uint8_t const *eepromBegin = eeprom.getConstDataPtr();
  uint8_t const *eepromEnd = eepromBegin + SPI_FLASH_SEC_SIZE;
  uint8_t const *resetCount = nullptr;
  for (uint8_t const *cell = eepromBegin; cell < eepromEnd; ++cell) {
    if (*cell == 0xFF) {
      continue;
    }
    if (resetCount != nullptr) {
      return 0;
    }
    resetCount = cell;
  }
  return resetCount == nullptr ? 0 : *resetCount;
}

void ResetDetector::writeResetCount(uint8_t resetCount) {
  uint8_t *eepromBegin = eeprom.getDataPtr();
  uint8_t *eepromEnd = eepromBegin + SPI_FLASH_SEC_SIZE;
  uint8_t *resetCountCellPtr = nullptr;
  for (uint8_t *cell = eepromBegin; cell < eepromEnd; ++cell) {
    if (*cell == 0xFF) {
      continue;
    }
    if (resetCountCellPtr == nullptr) {
      resetCountCellPtr = cell;
    }
    *cell = 0xFF;
  }
  if (resetCountCellPtr == nullptr || ++resetCountCellPtr >= eepromEnd) {
    resetCountCellPtr = eepromBegin;
  }
  *resetCountCellPtr = resetCount;
  eeprom.commit();
}

uint8_t ResetDetector::detectResetCount() {
  uint8_t resetCount = readResetCount();
  if (isLastResetReasonValuable) {
    writeResetCount(++resetCount);
  }
  return resetCount;
}

bool ResetDetector::handle() {
  if (!waitingForDoubleReset || !isLastResetReasonValuable) {
    return false;
  }
  if (millis() > timeoutMs) {
    waitingForDoubleReset = false;
    writeResetCount(0);
  }
  return true;
}

void ResetDetector::finishMonitoring() {
  while (handle()) {
    yield();
  }
}

uint8_t ResetDetector::execute() {
  uint8_t resetCount = detectResetCount();
  finishMonitoring();
  return resetCount;
}

uint8_t ResetDetector::execute(uint32_t timeoutMs, uint32_t eepromSector) {
  return ResetDetector(timeoutMs, eepromSector).execute();
}