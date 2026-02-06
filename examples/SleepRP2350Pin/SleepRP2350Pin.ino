// Adafruit Watchdog Library - RP2350 Pin/GPIO Sleep Example
//
// Simple example for using the Adafruit Watchdog Library to put the RP2350 to
// sleep until a GPIO pin changes state.
//
// Brent Rubell for Adafruit Industries
//

#include <Adafruit_SleepyDog.h>
#include <Arduino.h>
#define WAKE_PIN 0 // GPIO pin to wake on, change as needed for your use case
static bool awake;

// This function will be called when the device wakes from sleep
// You can add any custom behavior you want here
void cbWake() {
  // Show we're awake again
  digitalWrite(LED_BUILTIN, HIGH);
  awake = true;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  // while(!Serial);
  Serial.println("Adafruit Watchdog Library - RP2350 Sleep Pin/GPIO Demo!");
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

  // Sleep until GPIO WAKE_PIN goes HIGH on a rising edge
  Watchdog.GoToSleepUntilPin(WAKE_PIN, true, true);

  // Sleep until GPIO WAKE_PIN goes HIGH on a falling edge
  // Watchdog.GoToSleepUntilPin(WAKE_PIN, false, true);

  // Sleep until GPIO WAKE_PIN goes LOW on a rising edge
  // Watchdog.GoToSleepUntilPin(WAKE_PIN, true, false);

  // Sleep until GPIO WAKE_PIN goes LOW on a falling edge
  // Watchdog.GoToSleepUntilPin(WAKE_PIN, false, false);

  // Make sure we don't wake
  while (!awake) {
    Serial.println("Should be sleeping here...");
  }

  // Re-enable clocks, generators, USB and resume execution
  // NOTE: This MUST be called to properly resume from sleep!
  Watchdog.ResumeFromSleep();

  Serial.println("I'm awake now!");
  Serial.print("Slept for approximately ");
  long sleep_duration = Watchdog.GetSleepDuration();
  Serial.print(sleep_duration);
  Serial.println(" milliseconds.");
}
