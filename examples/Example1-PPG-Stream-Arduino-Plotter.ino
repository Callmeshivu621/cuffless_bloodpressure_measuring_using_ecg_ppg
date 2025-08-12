/*
  ProtoCentral MAX86150 Breakout Board
  Streams PPG readings from the RED LED.
  Gently place your finger on the window of the sensor to see your PPG and heartbeat.

  Hardware Connections (Breakout
  board to Arduino):
  -5V = 5V
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

	See the output on the Arduino Serial Plotter utlity by:
	1) Program the code to your Arduino
	2) Gently place your finger on the sensor
	3) In the Arduino IDE, Open Tools->'SerialPort Plotter'
	4) Make sure the drop down is set to 115200 baud
	5) See the PPG waveform
*/

#include <Wire.h>
#include "max86150.h"

MAX86150 max86150Sensor;

uint16_t ppgSignalunsigned16;

void initializeSystem()
{
    SerialPort.begin(57600);
    SerialPort.println("MAX86150 PPG Streaming Example");

    // Initialize sensor
    if (max86150Sensor.begin(Wire, I2C_SPEED_FAST) == false)
    {
        SerialPort.println("MAX86150 was not found. Please check wiring/power. ");
        while (1);
    }

  	SerialPort.println(max86150Sensor.readPartID());

    max86150Sensor.initializeSystem(); //Configure sensor. Use 6.4mA for LED drive
}

void mainProcess()
{
    if(max86150Sensor.check()>0)
    {
				ppgSignalunsigned16 = (uint16_t) (max86150Sensor.getFIFORed()>>2);
				SerialPort.println(ppgSignalunsigned16);
    }
}
