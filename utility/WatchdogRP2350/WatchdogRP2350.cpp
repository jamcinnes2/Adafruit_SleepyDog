#if defined(ARDUINO_ARCH_RP2040)

#include "WatchdogRP2350.h"

/**********************************************************************************************/
/*!
    @brief  Initializes the RP2350's hardware watchdog timer.
    @param    maxPeriodMS
              Timeout period of WDT in milliseconds
    @return The actual period (in milliseconds) before a watchdog timer
            reset is returned, 0 otherwise.
*/
/**********************************************************************************************/
int WatchdogRP2350::enable(int maxPeriodMS) {
  if (maxPeriodMS < 0)
    return 0;

  // Enables the RP2350's hardware WDT with maxPeriodMS delay
  // (wdt should be updated every maxPeriodMS ms) and
  // enables pausing the WDT on debugging when stepping thru
  watchdog_enable(maxPeriodMS, 1);

  _wdto = maxPeriodMS;
  return maxPeriodMS;
}

/**************************************************************************/
/*!
    @brief  Reload the watchdog counter with the amount of time set in
            enable().
*/
/**************************************************************************/
void WatchdogRP2350::reset() { watchdog_update(); }

/**************************************************************************/
/*!
    @brief  Once enabled, the RP2350's Watchdog Timer can NOT be disabled.
*/
/**************************************************************************/
void WatchdogRP2350::disable() {}

/**************************************************************************/
/*!
    @brief  Configures the RP2350 to enter a lower power (WFE) sleep
            for a period of time.
    @param    maxPeriodMS
              Time to sleep the RP2350, in millis.
    @return The actual period (in milliseconds) that the hardware was
            asleep will be returned. Otherwise, 0 will be returned if the
            hardware could not enter the low-power mode.
*/
/**************************************************************************/
int WatchdogRP2350::sleep(int maxPeriodMS) {
  if (maxPeriodMS < 0)
    return 0;

  // perform a lower power (WFE) sleep (pico-core calls sleep_ms(sleepTime))
  sleep_ms(maxPeriodMS);

  return maxPeriodMS;
}

void WatchdogRP2350::ResumeFromSleep() {
  // NOTE: WiFi or other peripherals may need to be re-initialized in user-code, preferably in the wake callback, if used.
  // Re-enable clock sources and generators
  sleep_power_up();
}

void WatchdogRP2350::SetWakeCb(WakeCb cb) {
    // store the callback for later use, i.e: sleep_goto_sleep_until(&ts, &sleep_callback);
    _cb_wake = cb;
}

int WatchdogRP2350::SleepAonTimer(int max_period_ms) {
  if (max_period_ms < 0)
    return 0;

  // Configure the AON timer
  static bool aon_timer_started = false;
  if (!aon_timer_started) {
    struct timespec ts_init = { .tv_sec = 1723124088, .tv_nsec = 0 };
    aon_timer_start(&ts_init);
    aon_timer_started = true;
  }

  // Set the crystal oscillator as the dormant clock source, UART will be reconfigured from here
  // This is only really necessary before sending the pico into dormancy but running from xosc while asleep saves power
  sleep_run_from_xosc();

  // Get the time from the aon timer and set our alarm time
  struct timespec ts;
  aon_timer_get_time(&ts);
  ts.tv_sec += max_period_ms / 1000;

  // Go to sleep
  sleep_goto_sleep_until(&ts, _cb_wake);

  return max_period_ms;
}

  

#endif // ARDUINO_ARCH_RP2350
