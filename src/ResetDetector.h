#ifndef ResetDetector_H__
#define ResetDetector_H__

#include <Arduino.h>

class ResetDetector {
  uint32_t timeoutMs;
  uint32_t memoryOffset;
  bool waitingForDoubleReset;
public:
  ResetDetector(uint32_t timeoutMs, uint32_t memoryOffset = 0);

  uint8_t readResetCount();

  void writeResetCount(uint8_t resetCount);

  // Should be called on the first line of setup method
  uint8_t detectResetCount();

  // Could be used instead of blocking finishMonitoring if timeoutMs is too big
  bool handle();
  
  // Should be called before leaving setup method
  void finishMonitoring();

  static uint8_t execute(uint32_t timeoutMs, uint32_t memoryOffset = 0);
};
#endif // ResetDetector_H__
