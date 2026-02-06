// Adafruit Watchdog Library RP2350 Sleep Example
//
// Brent Rubell for Adafruit Industries
// MIT License
//
#include <Adafruit_SleepyDog.h>
#include <Arduino.h>

static bool awake;

// This function will be called when the device wakes from sleep
// You can add any custom behavior you want here
void cbWake() {
  // Re-enable clock sources and generators
  // Note: This MUST be called to properly resume from sleep
  digitalWrite(LED_BUILTIN, HIGH); // Show we're awake again
  awake = true;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  // while(!Serial);
  Serial.println("Adafruit Watchdog Library - RP2350 Sleep Demo!");
  Serial.println();

  // When the device wakes back up, we'll call this callback function
  // to indicate that we've resumed from sleep.
  Watchdog.SetWakeCb(cbWake);
}

void loop() {
  Serial.println("Going to sleep in 5 seconds...");
  delay(5000);

  // Enter Sleep mode for 5 seconds
  awake = false;
  digitalWrite(LED_BUILTIN, LOW);
  Watchdog.GoToSleepUntil(5000);

  // Make sure we don't wake
  while (!awake) {
    Serial.println("Should be sleeping here...");
  }

  Watchdog.ResumeFromSleep();

// Try to reattach USB connection on "native USB" boards (connection is
// lost on sleep). Host will also need to reattach to the Serial monitor.
// Seems not entirely reliable, hence the LED indicator fallback.
#if defined(USBCON) && !defined(USE_TINYUSB)
  USBDevice.attach();
#endif

  Serial.println("I'm awake now!");
  Serial.print("Slept for approximately ");
  long sleep_duration = Watchdog.GetSleepDuration();
  Serial.print(sleep_duration);
  Serial.println(" milliseconds.");
}
