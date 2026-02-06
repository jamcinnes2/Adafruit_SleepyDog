// Adafruit Watchdog Library RP2350 Timer Sleep Example
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
  Serial.println("Adafruit Watchdog Library - RP2350 Sleep Timer Demo!");
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
  // Enter sleep state (6.5.2 in RP2350 Datasheet)
  Watchdog.GoToSleepUntil(5000);
  // Uncomment the line below (and comment the line above) to enter Dormant
  // State (6.5.3 in RP2350 Datasheet) instead of the Sleep State above
  // Watchdog.GoToSleepUntil(5000, true);

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
