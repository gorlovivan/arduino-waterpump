/*
  WaterPumpController
  Turns on and off a water pump by checking system's pressure level/
  ver. 0.2rc1

  Release notes:
  * full code refactoring

  created 2018
  by VanZa <http://www.example.org>

  Примерное соответствие уровня сигнала и давления в системе:
  bar (level)
  1.0 (164)
  1.4 (190)
  1.8 (216)
  2.0 (230)
  2.4 (255)
  2.8 (280)
  3.0 (292)
  3.4 (320)
  3.8 (344)
  4.0 (360)

*/

// CONSTANTS:
const int inputPressureSensorPin = A0;     // the number of the pushbutton pin
// const int outputLedPin = 13;
const int pressureSensorMinLevelTreshold =  85; // Минимальный уровень сигнала с датчика давления, сигнализирующий о подключении
const int probeCount =  10; // Количество замеров с датчика, для усреднения значения
const int minPowerOnPressureValue = 160;
const int maxPowerOffPressureValue = 220;
const int powerOnDelay = 2000;
const int powerOffDelay = 5000;
const int powerOffNoSensorDelay = 2000;



// VARs:
int sensorOutputLevel, clearLevel = 0;         // variable for getting current pressure level
int i = 1;
int sensorOutputLevelProbeSum;
bool engineState = false;
int currentPressure = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT); //outputPowerPin
  pinMode(inputPressureSensorPin, INPUT);
  Serial.begin(9600);
}

void indicateLed(int pressure) {
  if (pressure >= pressureSensorMinLevelTreshold) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (pressure < pressureSensorMinLevelTreshold) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

int getCurrentPressure (int checks) {
  int i;
  int sval = 0;
  for (i = 0; i < checks; i++) {
    sval = sval + analogRead(inputPressureSensorPin);    // sensor on analog pin
  }
  sval = round(sval / (checks * 10)) * 10;    // average of checks count
  return sval;
}

void controlEngine(int pressure) {
  if (pressure >= pressureSensorMinLevelTreshold) {
    if (pressure < minPowerOnPressureValue) {
      digitalWrite(2, HIGH);
      engineState = true;
      delay(powerOnDelay);
    }
    if (pressure >= maxPowerOffPressureValue) {
      digitalWrite(2, LOW);
      engineState = false;
      delay(powerOffDelay);
    }
  }
  else {
    digitalWrite(2, LOW);
    engineState = false;
    delay(powerOffNoSensorDelay);
  }
}

String consolePressureOutput (int pressure) {
  String out;
  if (pressure < pressureSensorMinLevelTreshold) {
    out = String("WARN: Low pressure sensor level. Bad music, i'll wait a bit!\n") + String("\t Engine state is - ") + String(engineState);
  }
  else if ((pressure >= pressureSensorMinLevelTreshold) and (pressure < minPowerOnPressureValue)) {
    out = String("INFO: Low pressure detected. It's current level = ") + String(pressure) + String(". Starting waterpump engine\n \t Engine state is - ") + String(engineState);
  }
  else if ((pressure >= minPowerOnPressureValue) and (engineState)) {
    out = String("INFO: Engine is running and rising pressure. It's current level = ") + String(pressure) + String("\n \t Engine state is - ") + String(engineState);
  }
  else if ((pressure >= minPowerOnPressureValue) and (!engineState)) {
    out = String("INFO: Engine stopped with normal pressure. It's current level = ") + String(pressure) + String("\n \t Engine state is - ") + String(engineState);
  }
  return out;
}

void loop() {

  currentPressure = getCurrentPressure(probeCount);
  indicateLed(currentPressure);
  controlEngine(currentPressure);
  Serial.println(consolePressureOutput(currentPressure));
  
}
