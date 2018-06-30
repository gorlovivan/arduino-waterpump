/*
  WaterPumpController
  Turns on and off a water pump by checking system's pressure level/
  
  ver. 0.3rc2

  Release notes:
  + Automatic calibration to determine maxPowerOffPressureValue

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
const int inputPressureSensorPin = A0;            // the number of the pressure sensor pin
const int pressureSensorMinLevelTreshold =  85;   // Минимальный уровень сигнала с датчика давления, сигнализирующий о подключении
const int probeCount =  10;                       // Количество замеров с датчика, для усреднения значения
const int minPowerOnPressureValue = 160;          // Минимальный нормальный уровень давления
const int maxPressureValueChecksDelay = 5000;     // Интервал замеров давления для определения скорости наращивания давления
const int minPressureIncSpdFct = 2;               // Минимальная скорость наращивания давления
const int powerOnDelay = 2000;                    // Задержка "петли" при включенном двигателе насоса
const int powerOffDelay = 5000;                   // Задержка "петли" при выключенном двигателе насоса
const int powerOffNoSensorDelay = 2000;           // Задержка "петли" при отсутстии подключенного сенсора давления




// VARs:
// int sensorOutputLevel, clearLevel = 0;         // variable for getting current pressure level
bool engineState = false;                         // Состояние двигателя (включен/выключен)
int maxPowerOffPressureValue = 210;               // Максимальный нормальный уровень давления
int currentPressure = 0;                          // Переменная для текущего уровня давления

void setup() {
  int mpopv = 0;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT); //outputPowerPin
  pinMode(inputPressureSensorPin, INPUT);
  Serial.begin(9600);
  Serial.println("INITIALIZATION STARTED.\nmaxPowerOffPressureValue = " + String(maxPowerOffPressureValue) + "\n");
  mpopv = getMaxPressure(maxPressureValueChecksDelay);
  if (maxPowerOffPressureValue < mpopv) {
    maxPowerOffPressureValue = mpopv;
    Serial.println("Max pressure value updated. Now it's equal to " + String(maxPowerOffPressureValue));
  }
  else {
    Serial.println("Max pressure value is not updated. Now it's equal to default " + String(maxPowerOffPressureValue) + "\n");
  };
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
    sval = sval + analogRead(inputPressureSensorPin);
  }
  sval = round(sval / (checks * 2)) * 2;
  return sval;
}

int getMaxPressure (int delayValue) {
  int diff = minPressureIncSpdFct;
  int preValue, value = 0;
  String printout;

  Serial.println("Calibration started... Kicking engine on and waiting for 5sec. \n");
  digitalWrite(2, HIGH);
  engineState = true;
  delay(5000);

  while (diff >= minPressureIncSpdFct) {
    preValue = getCurrentPressure(probeCount);
    Serial.print("Getting MaxPressure. === preValue = " + String(preValue) + " === | ===");
    delay(delayValue);
    value = getCurrentPressure(probeCount);
    diff = value - preValue;
    Serial.println(" Value = " + String(value) + "\n");
  }

  return value;
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
    out = String("WARN: Low pressure sensor level. Bad music, i'll wait a bit!\n === Pressure level is - ") + String(pressure) + String(" === | === Engine state is - ") + String(engineState) + " ===";
  }
  else if ((pressure >= pressureSensorMinLevelTreshold) and (pressure < minPowerOnPressureValue)) {
    out = String("INFO: Low pressure detected. Waterpump engine started.\n === Pressure level is - ") + String(pressure) + String(" === | === Engine state is - ") + String(engineState) + " ===";
  }
  else if ((pressure >= minPowerOnPressureValue) and (engineState)) {
    out = String("INFO: Engine is running and rising pressure.\n === Pressure level is - ") + String(pressure) + String(" === | === Engine state is - ") + String(engineState) + " ===";
  }
  else if ((pressure >= minPowerOnPressureValue) and (!engineState)) {
    out = String("INFO: Engine stopped with normal pressure.\n === Pressure level is - ") + String(pressure) + String(" === | === Engine state is - ") + String(engineState) + " ===";
  }
  return out;
}

void loop() {

  currentPressure = getCurrentPressure(probeCount);
  indicateLed(currentPressure);
  controlEngine(currentPressure);
  Serial.println(consolePressureOutput(currentPressure));

}
