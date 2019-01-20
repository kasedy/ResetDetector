#include "ResetDetector.h"

#include <user_interface.h>

#define RESET_DETECTOR_MEMORY_SET_FLAG 0xDEADBEEF


ResetDetector::ResetDetector(uint32_t timeoutMs, uint32_t memoryOffset) :
  timeoutMs(timeoutMs),
  memoryOffset(memoryOffset),
  waitingForDoubleReset(true) {
}

uint8_t ResetDetector::readResetCount() {
  uint32_t flag;
  ESP.rtcUserMemoryRead(memoryOffset, &flag, sizeof(flag));
  if (flag != RESET_DETECTOR_MEMORY_SET_FLAG) {
    return 0;
  }
  uint32_t resetCount;
  ESP.rtcUserMemoryRead(memoryOffset + 1, &resetCount, sizeof(resetCount));
  if (resetCount > 0x000000FF) {
    return 0;
  }
  return (uint8_t) resetCount;
}

void ResetDetector::writeResetCount(uint8_t resetCount) {
  uint32_t flag = RESET_DETECTOR_MEMORY_SET_FLAG;
  ESP.rtcUserMemoryWrite(memoryOffset, &flag, sizeof(flag));
  uint32_t resetCount4Byte = resetCount;
  ESP.rtcUserMemoryWrite(memoryOffset + 1, &resetCount4Byte, sizeof(resetCount4Byte));
}

uint8_t ResetDetector::detectResetCount() {
  uint8_t resetCount = readResetCount();
  if (system_get_rst_info()->reason == REASON_EXT_SYS_RST) {
    writeResetCount(++resetCount);
  }
  return resetCount;
}

bool ResetDetector::handle() {
  if (!waitingForDoubleReset || system_get_rst_info()->reason != REASON_EXT_SYS_RST) {
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

uint8_t ResetDetector::execute(uint32_t timeoutMs, uint32_t memoryOffset) {
  ResetDetector resetDetector(timeoutMs, memoryOffset);
  uint8_t resetCount = resetDetector.detectResetCount();
  resetDetector.finishMonitoring();
  return resetCount;
}