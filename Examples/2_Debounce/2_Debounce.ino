////////////////////////////////////////////////////////////////////////////////
// DigitalIO demo
// Copyright (c) Dan Truong
//
// A straightforward library to use digital switches on Arduinos
////////////////////////////////////////////////////////////////////////////////
// Debounce example for Arduino Uno
//
// Debouncing is removing the glitches caused by the switch hardware contact
// being imperfect and temporarilly having imperfect noisy contacts.
//
// This demo runs 3 loops. Test your sensor in all 3 modes to see how it is
// reported and decide which mode better suits your sensor. To visualize the
// detection open the Serial Console window (menu: Tools -> Serial Monitor).
// Alernatively you can look at the built in LED on the Arduino Uno. It is
// controlled by the "led" variable and will turn on when the button is pressed.
//
// * readLoop
//   A straightforward read of the pin. If the button used is perfect you will
//   get a single message stating if the button is up or down.
//   If the button has noise you'll get multiple readings when you press the it
//
// * keypressLoop (best for on/off buttons)
//   A debounced read of the pin. If the button used is perfect you will get a
//   single message just like readLoop. However you should also only get a single
//   message if the button has noise. If DIGITAL_SWITCH_DEBUG is set to 2, you
//   also get debug glitch warnings indicating that the debouncing worked.
//   Debouncing fails if it detects multiple button transitions for every single
//   button manipulation you do.
//
// * knockLoop (best for knock sensors)
//   A debounced detection of a transition on the pin. This mode is useful for
//   knock sensors which act like a brief press of a button. Often these will
//   have a lot of noise (for example it can be a spring that bounces a lot).
//   If the switch is perfect, you'll detect one transition for each change of
//   state of a button. If it's a knock sensor it will detect a brief motion
//   even if it always report the pin value at default.
//
// Keyes board's usual wiring:
// - ---> Gnd
// + ---> 5V (optional, since there's already a pullup on Pin 7)
// S ---> Pin 7 
//
////////////////////////////////////////////////////////////////////////////////

// These macros can be used to change the behavior of the DigitalIO library
//#define DIGITAL_PIN_DEBUG 2
//#define DIGITAL_PIN_DEBOUNCE_DELAY 200

#include <DigitalIO.hpp>

// Assumes your sensor is on pin 7, and by default the input is HIGH (5V) when
// it is off and LOW (ground) when on. The sensor can be a simple button or
// even a wire that you short to the ground to turn on.
digitalIo<7, HIGH> sensor;
digitalIo<13, LOW> led;

// For AVR platforms using the port directly saves memory and is faster
// Search for a diagram of your board to find the mapping between pin number
// and port name and bit.
//digitalIoAvr<D, 7, HIGH> sensor;
//digitalIoAvr<B, 5, LOW> led;

void setup() {
  // Initialize the serial console to see the output of the switch
  Serial.begin(9600);
  delay(500);
  Serial.println("Starting!");
  led.outputMode();
}

int32_t iters = 500000;

void loop() {
  // The main loop calls the 3 different ways to monitor the sensor pin
  iters++;
  if (iters <= 500000) {
    if (iters == 1) {
     Serial.println("\nreadLoop");
    }
    readLoop();
  } else if (iters <= 1000000) {
    if (iters == 500000+1) {
     Serial.println("\n\nkeypressLoop");
    }
    keypressLoop();
  } else if (iters <= 1500000) {
    if (iters == 1000000+1) {
      Serial.println("\nknockLoop");
    }
    knockLoop();
  } else {
    iters = 0;
    delay(100);
  }
}

void keypressLoop() {
  // Debounce will detect if the switch had a transition and report it only once
  switch(sensor.flipped())
  {
    case 1:
      Serial.println("Button is On!");
      led.turnOn();
      break;
    case -1:
      Serial.println("Button is off!");
      led.turnOff();
      break;
    case 0:
      break;
  }
}

void readLoop() {
  // Print the current state (with no debounce) but don't flood the screen when value doesn't change
  static bool wasAlreadySet = false;

  if (sensor.isOn()) {
    if (wasAlreadySet == false) {
      Serial.print("On ");
      led.turnOn();
      wasAlreadySet = true;
    }
  } else {
    if (wasAlreadySet == true) {
      Serial.print("Off ");
      led.turnOff();
      wasAlreadySet = false;
    }
  }
}


void knockLoop() {
  // Print the current resting state when a knock sensor was triggered
  if (sensor.triggered()) {
    led.toggle();
    if (sensor.lastValue() == HIGH) {
      Serial.println("Knock detected, stable is Off");
    } else {
      Serial.println("Knock detected, stable is On");
    }
  }
}
