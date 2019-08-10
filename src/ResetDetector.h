#ifndef ResetDetector_H__
#define ResetDetector_H__

#include <Arduino.h>
#include <user_interface.h>
#include <EEPROM.h>

class ResetDetector {
  uint32_t timeoutMs;
  bool waitingForDoubleReset;
  bool isLastResetReasonValuable;
  EEPROMClass eeprom;
public:
  ResetDetector(uint32_t timeoutMs, uint32_t eepromSector = 0);
  ~ResetDetector();

  /**
   * Resets with the reason different to provide are ignored.
   */
  ResetDetector& setValuableResetReasons(std::initializer_list<rst_reason> reasons);

  /**
   * Starts blocking detection. Returns how many times microcontroller was reseted.
   */
  uint8_t execute();

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
  static uint8_t execute(uint32_t timeoutMs, uint32_t eepromSector = 0);
private:
  uint8_t readResetCount();
  void writeResetCount(uint8_t resetCount);
};
#endif // ResetDetector_H__
