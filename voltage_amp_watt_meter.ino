/*
  PIN CONNECTIONS
  -------------------------------
  A0 - raw voltage analog pin
  A1 - raw ampers analog pin
  -------------------------------
  ACS712 - 30A
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
#define RAW_VOLT_PIN A0
#define RAW_AMPS_PIN A1

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DEBUG
//#define DEBUG_V
//#define UseCOMMPro

//= INCLUDES =======================================================================================
#include "Common.h"
//#include "CommCommon.h"
#include <LiquidCrystal.h>

//= CONSTANTS ======================================================================================
const byte LED_INDICATOR_PIN = LED_BUILTIN;  // choose the pin for the LED // D13
//------------------------------------------------
const float OPERATING_VOLTAGE = 5.0;

const byte MEASUREMENT_ITERATIONS = 200;
const float MEASUREMENT_RESOLUTION = 1024.0; // Nano = 10 bits

const byte ACS_SENSITIVITY = 66; // x05B (5A) = 185 || x20A = 100 || x30A = 66
const int AMPS_CORECTION = 514;
const byte AMPS_FACTOR = 15;
//
//= VARIABLES ======================================================================================
// initialize the interface pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
int rawSupply, rawVolt, rawAmpere;
long supply, volt_mV, current, power, ah;
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
  pinMode(RAW_VOLT_PIN, INPUT);
  pinMode(RAW_AMPS_PIN, INPUT);
  //
  analogReference(INTERNAL);
  //
  //comm_Setup();
  //
  lcd.begin(16, 2);
  //
  delay(100 * TIME_TICK);
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
  delay(50 * TIME_TICK);
}
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//==================================================================================================
void _readVoltageAmpsPower() {
  for (int i = 0; i < MEASUREMENT_ITERATIONS; i++) {
    rawAmpere = analogRead(RAW_AMPS_PIN);
    rawVolt = analogRead(RAW_VOLT_PIN);
    rawSupply = _readVcc();
    volt_mV = volt_mV + rawVolt;
    current = current + rawAmpere;
    supply = supply + rawSupply;
    delay(1);
  }

  supply = (supply / MEASUREMENT_ITERATIONS);
  debugPrint("AVG supply = ");debugPrintln(supply);
  volt_mV = supply / 10;


  current = (current / MEASUREMENT_ITERATIONS);
  debugPrint("AVG amp = ");debugPrintln(current);
  power = current;

  float avg_volt = (current * supply)/MEASUREMENT_RESOLUTION;
  debugPrint("AVG volt = ");debugPrintln(avg_volt);
  ampHours = avg_volt;


  current = (OPERATING_VOLTAGE/2 - avg_volt) / (ACS_SENSITIVITY/1000.0); // (2.5 - (AvgAcs * (5.0 / 1024.0)) ) / 0.185;
  //if (current < 1) current = 0;
}
//==================================================================================================
void _readVoltageAmpsPower2() {
  for (int i = 0; i < MEASUREMENT_ITERATIONS; i++) {
    rawAmpere = analogRead(A1);
    rawVolt = analogRead(A0);
    volt_mV = volt_mV + rawVolt;
    current = current + rawAmpere;
    delay(1);
  }

  current = (current / MEASUREMENT_ITERATIONS - 514);
  if (current < 1) current = 0;
  current = current * 15;
  volt_mV = volt_mV / 30;
  power = (volt_mV * current) / 1000;
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
  //--------------------------------------
  lcd.setCursor(6, 0);
  lcd.print("V");

  displayValue = volt_mV % 10;
  lcd.setCursor(5, 0);
  lcd.print(displayValue);
  displayValue = (volt_mV / 10) % 10;
  lcd.setCursor(4, 0);
  lcd.print(displayValue);
  lcd.setCursor(3, 0);
  lcd.print(".");
  displayValue = (volt_mV / 100) % 10;
  lcd.setCursor(2, 0);
  lcd.print(displayValue);
  displayValue = (volt_mV / 1000) % 10;
  lcd.setCursor(1, 0);
  if (volt > 999) lcd.print(displayValue);
  else lcd.print(" ");
  //--------------------------------------
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
  //--------------------------------------
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
  //--------------------------------------
  lcd.setCursor(14, 1);
  lcd.print("Ah");

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
  //--------------------------------------
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
//==================================================================================================
float _computeVoltage(unsigned int raw_analog_value, int unit) {
  float raw_voltage = raw_analog_value * OPERATING_VOLTAGE / 1024.0;
  // read correct supply voltage
  long raw_vcc = _readVcc();
  float supply_voltage = raw_vcc * (unit / 1000.0);
  float corrected_voltage = supply_voltage / OPERATING_VOLTAGE * raw_voltage;
  supply = round(supply_voltage);

#ifdef DEBUG_VCC
  Serial.print("Vcc = ");
  Serial.print(supply_voltage);
  Serial.print(" V | ");
#endif

  return corrected_voltage;
}
//==================================================================================================
//==================================================================================================
long _readVcc() {
  long result;
  // Read 1.1V reference against AVcc
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delay(2);             // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);  // Convert
  while (bit_is_set(ADCSRA, ADSC))
    ;
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result;  // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
  return result;
}
//==================================================================================================