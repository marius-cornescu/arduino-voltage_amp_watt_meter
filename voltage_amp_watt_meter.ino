/*
  PIN CONNECTIONS
  -------------------------------
  A0 - A7 - analog input pins
  -------------------------------
  D1 -> RX
  D0 -> TX
  -------------------------------
Sketch uses 4994 bytes (16%) of program storage space. Maximum is 30720 bytes.
Global variables use 83 bytes (4%) of dynamic memory, leaving 1965 bytes for local variables. Maximum is 2048 bytes.
  -------------------------------
*/
//= DEFINES ========================================================================================
//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DEBUG
#define DEBUG_V
//#define UseCOMMPro

//= INCLUDES =======================================================================================
#include "Common.h"
//#include "CommCommon.h"
#include <LiquidCrystal.h>

//= CONSTANTS ======================================================================================
const byte LED_INDICATOR_PIN = LED_BUILTIN;  // choose the pin for the LED // D13
//------------------------------------------------
const byte MEASUREMENT_ITERATIONS = 200;
//
//= VARIABLES ======================================================================================
// initialize the interface pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
int rawVolt, rawAmpere;
long volt, current, power, ah;
// the 8 arrays that form each segment of the custom numbers
unsigned long msec = 0;
float time = 0.0;
int sample = 0;
float totalCharge = 0.0;
float averageAmps = 0.0;
float ampSeconds = 0.0;
long ampHours = 0;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void setup() {
#ifdef DEBUG
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) { ; }
#endif
  debugPrintln(F("START-UP >>>>>>>>>>>>>>>"));
  //..............................
  // initialize digital pin LED_INDICATOR_PIN as an output.
  pinMode(LED_INDICATOR_PIN, OUTPUT);
  //
  //comm_Setup();
  //
  lcd.begin(16, 2);
  //
  delay(1000 * TIME_TICK);
  //..............................
  debugPrintln(F("START-UP <<<<<<<<<<<<<<<"));
}
//**************************************************************************************************
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
void loop() {
  //
  digitalWrite(LED_INDICATOR_PIN, HIGH);
  //
  _readVoltageAmpsPower();
  //
  _printVoltageAmpsPower();
  _displayVoltageAmpsPower();
  //
  //comm_ActOnNewDataToSend();
  //
  digitalWrite(LED_INDICATOR_PIN, LOW);
  //
  delay(200 * TIME_TICK);
}
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//==================================================================================================
void _readVoltageAmpsPower() {
  for (int i = 0; i < MEASUREMENT_ITERATIONS; i++) {
    rawAmpere = analogRead(A1);
    rawVolt = analogRead(A0);
    volt = volt + rawVolt;
    current = current + rawAmpere;
    delay(1);
  }

  current = (current / MEASUREMENT_ITERATIONS - 514);
  if (current < 1) current = 0;
  current = current * 15;
  volt = volt / 30;
  power = (volt * current) / 1000;
  //--------------------
  sample = sample + 1;

  msec = millis();

  time = (float)msec / 1000.0;
  totalCharge = totalCharge + (current);
  averageAmps = totalCharge / sample;
  ampSeconds = averageAmps * time;
  ampHours = ampSeconds / 3600;
}
//==================================================================================================
void _displayVoltageAmpsPower() {
  int displayValue = 0;

  lcd.setCursor(6, 0);
  lcd.print("V");
  displayValue = volt % 10;
  lcd.setCursor(5, 0);
  lcd.print(displayValue);
  displayValue = (volt / 10) % 10;
  lcd.setCursor(4, 0);
  lcd.print(displayValue);
  lcd.setCursor(3, 0);
  lcd.print(".");
  displayValue = (volt / 100) % 10;
  lcd.setCursor(2, 0);
  lcd.print(displayValue);
  displayValue = (volt / 1000) % 10;
  lcd.setCursor(1, 0);
  if (volt > 999) lcd.print(displayValue);
  else lcd.print(" ");

  lcd.setCursor(14, 0);
  lcd.print("A");

  displayValue = current % 10;
  lcd.setCursor(13, 0);
  lcd.print(displayValue);
  displayValue = (current / 10) % 10;
  lcd.setCursor(12, 0);
  lcd.print(displayValue);
  lcd.setCursor(11, 0);
  lcd.print(".");
  displayValue = (current / 100) % 10;
  lcd.setCursor(10, 0);
  lcd.print(displayValue);


  lcd.setCursor(6, 1);
  lcd.print("W");
  displayValue = power % 10;
  lcd.setCursor(5, 1);
  lcd.print(displayValue);
  displayValue = (power / 10) % 10;
  lcd.setCursor(3, 1);
  lcd.print(displayValue);
  lcd.setCursor(4, 1);
  lcd.print(".");
  displayValue = (power / 100) % 10;
  lcd.setCursor(2, 1);
  if (power > 99) lcd.print(displayValue);
  else lcd.print(" ");
  displayValue = (power / 1000) % 10;

  lcd.setCursor(1, 1);
  if (power > 999) lcd.print(displayValue);
  else lcd.print(" ");
  //------------
  lcd.setCursor(14, 1);
  lcd.print("AH");

  displayValue = ampHours % 10;
  lcd.setCursor(13, 1);
  lcd.print(displayValue);
  displayValue = (ampHours / 10) % 10;
  lcd.setCursor(12, 1);
  lcd.print(displayValue);
  lcd.setCursor(11, 1);
  lcd.print(".");
  displayValue = (ampHours / 100) % 10;
  lcd.setCursor(10, 1);
  lcd.print(displayValue);
  displayValue = (ampHours / 1000) % 10;
  lcd.setCursor(9, 1);
  lcd.print(displayValue);
}
//==================================================================================================
void _printVoltageAmpsPower() {
#ifdef DEBUG_V
  debugPrint(volt);
  debugPrint("  ");
  debugPrint(current);
  debugPrint("  ");
  debugPrint(power);

  debugPrint("  ");
  debugPrint(ampHours);
  debugPrint("  ");
  debugPrintln(time);
  debugPrintln("---------------------------------------");
#endif
}
//==================================================================================================