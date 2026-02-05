# 1 "/var/folders/ff/dmzflvf52tq9kzvt6g8jglxw0000gn/T/tmp3toyevjj"
#include <Arduino.h>
# 1 "/Users/brentrubell/Documents/Arduino/libraries/Adafruit_SleepyDog/examples/SleepRP2350/SleepRP2350.ino"





#include <Arduino.h>
#include <Adafruit_SleepyDog.h>

static bool awake;
void cbWake();
void setup();
void loop();
#line 13 "/Users/brentrubell/Documents/Arduino/libraries/Adafruit_SleepyDog/examples/SleepRP2350/SleepRP2350.ino"
void cbWake() {


    digitalWrite(LED_BUILTIN, HIGH);
    awake = true;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  while(!Serial);
  Serial.println("Adafruit Watchdog Library - RP2350 Sleep Demo!");
  Serial.println();



  Watchdog.SetWakeCb(cbWake);
}

void loop() {
  Serial.println("Going to sleep in 5 seconds...");
  delay(5000);


  awake = false;
  digitalWrite(LED_BUILTIN, LOW);
  int sleep_ms = Watchdog.SleepAonTimer(5000);


    while (!awake) {
        Serial.println("Should be sleeping here...");
    }

  Watchdog.ResumeFromSleep();




  #if defined(USBCON) && !defined(USE_TINYUSB)
  USBDevice.attach();
  #endif

  Serial.print("I'm awake now!");
}