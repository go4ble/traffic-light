#include <SoftwareSerial.h>
#include <SmartThings.h>

#define PIN_THING_RX  3
#define PIN_THING_TX  2

#define PIN_LIGHT_RED     7
#define PIN_LIGHT_YELLOW  6
#define PIN_LIGHT_GREEN   5

#define MESSAGE_RED_ON      "red on"
#define MESSAGE_RED_OFF     "red off"
#define MESSAGE_YELLOW_ON   "yellow on"
#define MESSAGE_YELLOW_OFF  "yellow off"
#define MESSAGE_GREEN_ON    "green on"
#define MESSAGE_GREEN_OFF   "green off"
#define MESSAGE_REFRESH     "refresh"
#define MESSAGE_AUTO_LEFT   "auto left"
#define MESSAGE_OFF         "off"


bool debugging = true;
     // redLight,
     // yellowLight,
     // greenLight,
     // toggleLights;

#define NUM_READINGS  10
int readings[NUM_READINGS] = { 0 };
int readIndex = 0,
    total = 0,
    distance = 0;

enum {
  autoLeft,
  autoRight,
  autoStop,
  autoOff,
} autoMode;

SmartThingsCallout_t messageCallout;
SmartThings thingShield(PIN_THING_RX, PIN_THING_TX, messageCallout);

void refresh() {
  if (bitRead(PORTD, PIN_LIGHT_RED) == LOW) {
    thingShield.send(MESSAGE_RED_OFF);
  } else {
    thingShield.send(MESSAGE_RED_ON);
  }

  if (bitRead(PORTD, PIN_LIGHT_YELLOW) == LOW) {
    thingShield.send(MESSAGE_YELLOW_OFF);
  } else {
    thingShield.send(MESSAGE_YELLOW_ON);
  }

  if (bitRead(PORTD, PIN_LIGHT_GREEN) == LOW) {
    thingShield.send(MESSAGE_GREEN_OFF);
  } else {
    thingShield.send(MESSAGE_GREEN_ON);
  }

  thingShield.send(MESSAGE_REFRESH);
}

void messageCallout(String message) {
  if (debugging) {
    Serial.print("Message: '");
    Serial.print(message);
    Serial.println("'");
  }

  if (!message.length()) {
    // refresh();
  } else if (message.equals(MESSAGE_RED_OFF)) {
    digitalWrite(PIN_LIGHT_RED, LOW);
    thingShield.send(MESSAGE_RED_OFF);
  } else if (message.equals(MESSAGE_RED_ON)) {
    digitalWrite(PIN_LIGHT_RED, HIGH);
    thingShield.send(MESSAGE_RED_ON);
  } else if (message.equals(MESSAGE_YELLOW_OFF)) {
    digitalWrite(PIN_LIGHT_YELLOW, LOW);
    thingShield.send(MESSAGE_YELLOW_OFF);
  } else if (message.equals(MESSAGE_YELLOW_ON)) {
    digitalWrite(PIN_LIGHT_YELLOW, HIGH);
    thingShield.send(MESSAGE_YELLOW_ON);
  } else if (message.equals(MESSAGE_GREEN_OFF)) {
    digitalWrite(PIN_LIGHT_GREEN, LOW);
    thingShield.send(MESSAGE_GREEN_OFF);
  } else if (message.equals(MESSAGE_GREEN_ON)) {
    digitalWrite(PIN_LIGHT_GREEN, HIGH);
    thingShield.send(MESSAGE_GREEN_ON);
  } else if (message.equals(MESSAGE_AUTO_LEFT)) {
    autoMode = autoLeft;
  } else if (message.equals(MESSAGE_OFF)) {
    autoMode = autoOff;
    digitalWrite(PIN_LIGHT_RED, LOW);
    digitalWrite(PIN_LIGHT_YELLOW, LOW);
    digitalWrite(PIN_LIGHT_GREEN, LOW);
    thingShield.send(MESSAGE_OFF);
  } else if (message.equals("refresh")) {
    refresh();
  } else if (debugging) {
    Serial.println("UNKNOWN MESSAGE!");
  }
}

void setup() {
  if (debugging) {
    Serial.begin(9600);
    Serial.println("setup..");
  }

  pinMode(PIN_LIGHT_RED,    OUTPUT);
  pinMode(PIN_LIGHT_YELLOW, OUTPUT);
  pinMode(PIN_LIGHT_GREEN,  OUTPUT);

  digitalWrite(PIN_LIGHT_RED,    LOW);
  digitalWrite(PIN_LIGHT_YELLOW, LOW);
  digitalWrite(PIN_LIGHT_GREEN,  LOW);

  refresh();

  // TODO temp
  // pinMode(12, OUTPUT);
  // digitalWrite(12, HIGH);
  // autoMode = autoLeft;
  autoMode = autoStop;
}

void loop() {
  thingShield.run();

  if (autoMode == autoLeft || autoMode == autoRight) {
    total = total - readings[readIndex];
    readings[readIndex] = analogRead(autoMode == autoLeft ? A0 : A1);
    total = total + readings[readIndex];
    readIndex += 1;
    if (readIndex >= NUM_READINGS) readIndex = 0;
    distance = map(total / NUM_READINGS, 0, 1023, 0, 510);
    if (distance >= 120) {
      digitalWrite(PIN_LIGHT_GREEN, HIGH);
      digitalWrite(PIN_LIGHT_YELLOW, LOW);
      digitalWrite(PIN_LIGHT_RED, LOW);
      // thingShield.send(MESSAGE_RED_ON);  // TODO message scheme for multiple lights
    } else if (distance >= 60) {
      digitalWrite(PIN_LIGHT_GREEN, LOW);
      digitalWrite(PIN_LIGHT_YELLOW, HIGH);
      digitalWrite(PIN_LIGHT_RED, LOW);
    } else {
      digitalWrite(PIN_LIGHT_GREEN, LOW);
      digitalWrite(PIN_LIGHT_YELLOW, LOW);
      digitalWrite(PIN_LIGHT_RED, HIGH);
    }
  } else if (autoMode == autoStop) {
    digitalWrite(PIN_LIGHT_GREEN, LOW);
    digitalWrite(PIN_LIGHT_YELLOW, LOW);
    digitalWrite(PIN_LIGHT_RED, millis() / 1000 % 2 ? HIGH : LOW);
  }

  delay(100);
}
