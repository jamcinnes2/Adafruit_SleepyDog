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

/**************************************************************************/
/*!
    @brief  Re-enables clock sources and generators after waking from sleep,
   must be called by user code.
*/
/**************************************************************************/
void WatchdogRP2350::ResumeFromSleep() { sleep_power_up(); }

/**************************************************************************/
/*!
    @brief  Sets a callback function to be called upon wake from sleep. Only one
   callback can be set at a time.
    @param  cb
            Pointer to the user-defined callback function.
*/
/**************************************************************************/
void WatchdogRP2350::SetWakeCb(WakeCb cb) { _cb_wake = cb; }

/**************************************************************************/
/*!
    @brief  Gets the duration of the previous sleep cycle, in milliseconds.
    @return The sleep duration in milliseconds.
*/
/**************************************************************************/
long WatchdogRP2350::GetSleepDuration() {
  // Obtain the current time from the AON timer
  struct timespec ts_sleep_end;
  aon_timer_get_time(&ts_sleep_end);
  // Compare timespecs to get sleep duration
  long sleep_duration_ms =
      (ts_sleep_end.tv_sec - _ts_sleep_start.tv_sec) * 1000 +
      (ts_sleep_end.tv_nsec - _ts_sleep_start.tv_nsec) / 1000000;
  return sleep_duration_ms;
}

/**************************************************************************/
/*!
    @brief  Puts the RP2350 into Sleep State (6.5.2) for a specified
            period of time, uses the AON timer to wake the device.
    @param  max_period_ms
            Desired sleep period, in milliseconds.
*/
/**************************************************************************/
void WatchdogRP2350::GoToSleepUntil(int max_period_ms) {
  if (max_period_ms < 0)
    return;

  // Configure the AON timer
  static bool aon_timer_started = false;
  if (!aon_timer_started) {
    struct timespec ts_init = {.tv_sec = 1723124088, .tv_nsec = 0};
    aon_timer_start(&ts_init);
    aon_timer_started = true;
  }

  // Set the crystal oscillator as the dormant clock source, UART will be
  // reconfigured from here This is only really necessary before sending the
  // pico into dormancy but running from xosc while asleep saves power
  sleep_run_from_xosc();

  // Get the time from the aon timer and set our alarm time
  struct timespec ts;
  aon_timer_get_time(&ts);
  ts.tv_sec += max_period_ms / 1000;

  // Store the sleep start time
  // TODO: Optimize by storing only once before sleep cycles if multiple sleeps
  // are done
  aon_timer_get_time(&_ts_sleep_start);

  // Go to sleep
  sleep_goto_sleep_until(&ts, _cb_wake);
}

#endif // ARDUINO_ARCH_RP2350
