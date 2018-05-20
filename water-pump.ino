/*
  WaterPumpController

  Turns on and off a water pump by checking system's pressure level/

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

// constants won't change. They're used here to set pin numbers:
const int inputPressureSensorPin = A0;     // the number of the pushbutton pin
const int outputLedPin = 13;
//const int outputPowerPin = 2;
const int pressureSensorMinLevelTreshold =  85; // Минимальный уровень сигнала с датчика давления, сигнализирующий о подключении
const int probeCount =  10; // Количество замеров с датчика, для усреднения значения
const int minPowerOnPressureValue = 160;
const int maxPowerOffPressureValue = 280;
const int powerOnDelay = 2000;
const int powerOffDelay = 5000;
const int powerOffNoSensorDelay = 2000;



// variables will change:
int sensorOutputLevel, clearLevel = 0;         // variable for getting current pressure level
int i = 1;
int sensorOutputLevelProbeSum;
bool engineState = false;

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT); //outputPowerPin
  //digitalWrite(2, HIGH); //outputPowerPin
  pinMode(inputPressureSensorPin, INPUT);
  Serial.begin(9600);
}



void loop() {
  
  Serial.print("\n");
  sensorOutputLevelProbeSum = 0;
  i = 0;
  sensorOutputLevel = analogRead(inputPressureSensorPin);


  if (sensorOutputLevel >= pressureSensorMinLevelTreshold) {

    while (i < probeCount) {
      sensorOutputLevel = analogRead(inputPressureSensorPin);
      sensorOutputLevelProbeSum += sensorOutputLevel;
      i += 1;
    }

    digitalWrite(LED_BUILTIN, HIGH);
    clearLevel = round(sensorOutputLevelProbeSum / (probeCount * 10)) * 10;
    //Serial.print(" sensorOutputLevelProbeSum =");
    //Serial.print(sensorOutputLevelProbeSum);
    Serial.print(" \t sensor = ");
    Serial.print(clearLevel);
    //Serial.print("\t level = ");
    //Serial.print(sensorOutputLevel);


    if (clearLevel < minPowerOnPressureValue) {
      digitalWrite(2, HIGH);
      engineState = true;
      Serial.print(" \tLow pressure detected. Engine state is - ");
      Serial.print(engineState);
      delay(powerOnDelay);
    } else {
      if (clearLevel > maxPowerOffPressureValue) {
        engineState = false;
        Serial.print(" \tOverPressure detected. Engine state is - ");
        Serial.print(engineState);
        digitalWrite(2, LOW);
        delay(powerOffDelay);

      }
      if ((clearLevel > minPowerOnPressureValue) and (clearLevel < maxPowerOffPressureValue)) {
        Serial.print(" \tIt's good pressure! Waiting... Engine state is - ");
        Serial.print(engineState);
      }
    }
  }

  else {
    digitalWrite(2, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print(" \t No music at all =(");
    Serial.print("\t level = ");
    Serial.print(sensorOutputLevel);
    Serial.print(" \t ");
    //Serial.print(outputPowerPin);
    delay(powerOffNoSensorDelay);
  }
}
