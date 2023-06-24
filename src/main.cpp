#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include "RCSwitch.h"

#define TX_PIN 4
#define ON_SWITCH 2
#define OFF_SWITCH 3
#define TX_ENABLE 10

RCSwitch myRemote = RCSwitch();
bool wakeForOn;

void onDetected() {
  wakeForOn = true;
}

void offDetected() {
  wakeForOn = false;
}

void enableRadio() {
  digitalWrite(TX_ENABLE, LOW);
  delay(100);
}

void disableRadio() {
  delay(100);
  digitalWrite(TX_ENABLE, HIGH);
}

void remoteOn() {
  Serial.println("Remote on");
  enableRadio();
  myRemote.send("100111101011011000011100");
  disableRadio();
}

void remoteOff() {
  Serial.println("Remote off");
  enableRadio();
  myRemote.send("100111101011011000010100");
  disableRadio();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting");

  ADCSRA = 0;
  Serial.println("ADC disabled");

  myRemote.setProtocol(1);
  myRemote.setPulseLength(188);
  myRemote.setRepeatTransmit(12);
  myRemote.enableTransmit(TX_PIN);
  Serial.println("Remote initialised");

  pinMode(ON_SWITCH, INPUT_PULLUP);
  pinMode(OFF_SWITCH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ON_SWITCH), onDetected, FALLING);
  attachInterrupt(digitalPinToInterrupt(OFF_SWITCH), offDetected, FALLING);
  Serial.println("Switches initialised");

  pinMode(TX_ENABLE, OUTPUT);
  digitalWrite(TX_ENABLE, HIGH);
  Serial.println("Enable initialised");
}

void loop() {
  power_all_disable();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  noInterrupts ();           // timed sequence follows
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);  // turn on brown-out enable select
  MCUCR = bit (BODS);        // this must be done within 4 clock cycles of above
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();              // sleep within 3 clock cycles of above
  // BLOCKS
  
  power_all_enable();

  if (wakeForOn) {
    remoteOn();
  } else {
    remoteOff();
  }

  delay(500);
}