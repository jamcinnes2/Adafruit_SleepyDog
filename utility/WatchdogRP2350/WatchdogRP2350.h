/*!
 * @file WatchdogRP2350.h
 *
 * Support for RP2040 Hardware Watchdog Timer API
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Brent Rubell for Adafruit Industries.
 *
 * MIT License, all text here must be included in any redistribution.
 *
 */
#ifndef WATCHDOGRP2350_H_
#define WATCHDOGRP2350_H_

#include <hardware/watchdog.h>
#include <pico/time.h>
#include "pico_sleep/include/pico/sleep.h"
#include "pico/aon_timer.h"

// Callback type for wake-from-sleep functions
typedef void (*WakeCb)(void);

/**************************************************************************/
/*!
    @brief  Class that contains functions for interacting with the
            RP2350's hardware watchdog timer and low-power sleep functionality.
*/
/**************************************************************************/
class WatchdogRP2350 {
public:
  WatchdogRP2350() : _wdto(-1){};
  // Watchdog API
  int enable(int maxPeriodMS = 0);
  void disable()
      __attribute__((error("RP2350 WDT cannot be disabled once enabled")));
  void reset();
  // Low-power ("sleep") mode API
  int sleep(int maxPeriodMS = 0); // Basic sleep function, handled by software timer
  int SleepAonTimer(int max_period_ms = 0);
  void SetWakeCb(WakeCb cb);
  void ResumeFromSleep();

private:
  int _wdto;
  WakeCb _cb_wake = nullptr;
};

#endif // WATCHDOGRP2350_H_