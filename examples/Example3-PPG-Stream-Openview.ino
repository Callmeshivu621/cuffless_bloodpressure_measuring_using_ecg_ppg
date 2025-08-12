/*
  ProtoCentral MAX86150 Interface Demo
  Wiring (Breakout to Arduino):
    - 5V   -> 5V
    - GND  -> GND
    - SDA  -> A4 (or SDA)
    - SCL  -> A5 (or SCL)
    - INT  -> Not used
*/

#include <Wire.h>
#include "max86150.h"

// Packet markers and constants
#define PKT_START_BYTE1   0x0A
#define PKT_START_BYTE2   0xFA
#define PKT_TYPE_DATA     0x02
#define PKT_STOP_BYTE     0x0B
#define SENSOR_DATA_LEN   6
#define FILL_ZERO         0

// Buffers for packet structure
volatile char packetPayload[16];
const char packetFooter[2] = {FILL_ZERO, PKT_STOP_BYTE};
const char packetHeader[5] = {PKT_START_BYTE1, PKT_START_BYTE2, SENSOR_DATA_LEN, FILL_ZERO, PKT_TYPE_DATA};

// Sensor object
MAX86150 ppgEcgSensor;

// Data storage variables
uint16_t irReading;
uint16_t redReading;
int16_t  ecgReading;

// Function: transmit collected samples over UART
void transmitSensorPacket() {
  
  packetPayload[0] = ecgReading;
  packetPayload[1] = ecgReading >> 8;

  packetPayload[2] = irReading;
  packetPayload[3] = irReading >> 8;

  packetPayload[4] = redReading;
  packetPayload[5] = redReading >> 8;

  // Send header bytes
  for (int i = 0; i < 5; i++) {
    Serial.write(packetHeader[i]);
  }

  // Send actual sensor data
  for (int i = 0; i < SENSOR_DATA_LEN; i++) {
    Serial.write(packetPayload[i]);
  }

  // Send footer bytes
  for (int i = 0; i < 2; i++) {
    Serial.write(packetFooter[i]);
  }
}

void initializeSystem() {
    Serial.begin(57600);
    Serial.println("MAX86150 PPG + ECG Stream (OpenView)");

    // Initialize the MAX86150 sensor
    if (!ppgEcgSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("Sensor not found. Check connections and power.");
        while (1); // Halt
    }

    Serial.println(ppgEcgSensor.readPartID());

    // Configure sensor with default settings
    ppgEcgSensor.setup();
}

void mainProcess() {
    if (ppgEcgSensor.check() > 0) {
        irReading  = (uint16_t)(ppgEcgSensor.getFIFOIR()   >> 2);
        redReading = (uint16_t)(ppgEcgSensor.getFIFORed()  >> 2);
        ecgReading = (int16_t) (ppgEcgSensor.getFIFOECG()  >> 2);

        transmitSensorPacket();
    }
}

// Arduino entry points
void setup() {
    initializeSystem();
}

void loop() {
    mainProcess();
}
