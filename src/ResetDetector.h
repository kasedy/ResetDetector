#ifndef ResetDetector_H__
#define ResetDetector_H__

#include <Arduino.h>

class ResetDetector {
  uint32_t timeoutMs;
  uint32_t memoryOffset;
  bool waitingForDoubleReset;
public:
  ResetDetector(uint32_t timeoutMs, uint32_t memoryOffset = 0);

  /**
   * Detscts the cause of device reset and increase the counter. Should be 
   * called on the first line of setup method and only once.
   */
  uint8_t detectResetCount();

  /**
   * Blocks until monitoring time interval is up. 
   */ 
  void finishMonitoring();

  /**
   * Ensures that reset counter set to zero. Returns whether monitoring time
   * interval is up. Could be used inside a loop instead of blocking 
   * finishMonitoring if timeoutMs is too big.
   */
  bool handle();

  /**
   * Starts detection and waits timeoutMs. Returns how many times reset button
   * was clicked.
   */
  static uint8_t execute(uint32_t timeoutMs, uint32_t memoryOffset = 0);
private:
  uint8_t readResetCount();
  void writeResetCount(uint8_t resetCount);
};
#endif // ResetDetector_H__
