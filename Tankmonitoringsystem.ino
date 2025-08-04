#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo pump;

const int levelSensor = A0;
const int tempSensor = A1;
const int redLED = 8;
const int yellowLED = 9;
const int greenLED = 7;
const int buzzer = 6;
const int overrideButton = 13;

void setup() {
  lcd.begin(16, 2);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(overrideButton, INPUT_PULLUP);
  pump.attach(10);
  lcd.print("Tank Controller");
  delay(2000);
  lcd.clear();
}

void loop() {
  int levelRaw = analogRead(levelSensor);
  int tempRaw = analogRead(tempSensor);

  float level = map(levelRaw, 0, 1023, 0, 100);  // Percent
  float voltage = tempRaw * (5.0 / 1023.0);
  float tempC = (voltage - 0.5) * 100;  // TMP36 conversion

  lcd.setCursor(0, 0);
  lcd.print("Lvl:");
  lcd.print(level);
  lcd.print("% T:");
  lcd.print(tempC, 1);
  lcd.print("C ");

  // Default: All alerts off
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(buzzer, LOW);

  bool overfill = false;
  bool underfill = false;
  bool highTemp = false;

  // --- Level Checks ---
  if (level < 20) {
    underfill = true;
    digitalWrite(yellowLED, HIGH);
    digitalWrite(buzzer, HIGH);
  } else if (level > 80) {
    overfill = true;
    digitalWrite(redLED, HIGH);
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(greenLED, HIGH);
  }

  // --- Temp Check ---
  if (tempC > 45.0) {
    highTemp = true;
    digitalWrite(redLED, HIGH);
    digitalWrite(buzzer, HIGH);
  }

  // --- Display Status ---
  lcd.setCursor(0, 1);
  if (overfill) lcd.print("ALERT: OVERFILL ");
  else if (underfill) lcd.print("ALERT: LOW LVL ");
  else if (highTemp) lcd.print("ALERT: TEMP HIGH");
  else lcd.print("Status: Normal   ");

  // --- Pump Logic ---
  bool overridePressed = digitalRead(overrideButton) == LOW;
  if (overridePressed || overfill || highTemp) {
    pump.write(0); // OFF
  } else if (level < 30) {
    pump.write(90); // ON
  } else {
    pump.write(0); // OFF
  }

  delay(500);
}
