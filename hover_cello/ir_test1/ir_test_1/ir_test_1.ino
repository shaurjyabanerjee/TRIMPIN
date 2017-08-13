//SPARKFUN LINE FOLLOWER ARRAY TEST 1

#include "Wire.h"
#include "sensorbar.h"


//

const uint8_t SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
//const byte SX1509_ADDRESS = 0x3F;  // SX1509 I2C address (01)
//const byte SX1509_ADDRESS = 0x70;  // SX1509 I2C address (10)
//const byte SX1509_ADDRESS = 0x71;  // SX1509 I2C address (11)

SensorBar mySensorBar(SX1509_ADDRESS);

void setup()
{
  Serial.begin(9600);  // start serial for output
  Serial.println("Program started.");
  Serial.println();
  
  //To enable IR only during reads
  //mySensorBar.setBarStrobe();
  
  //To enable IR all the time (easier to get visual feedback)
  mySensorBar.clearBarStrobe();

  //Default dark on light
  mySensorBar.clearInvertBits();
  
  //Light line on dark
  //mySensorBar.setInvertBits();
  
  //.begin() gets the bar ready.  This configures HW.
  uint8_t returnStatus = mySensorBar.begin();
  if(returnStatus)
  {
	  Serial.println("sx1509 IC communication OK");
  }
  else
  {
	  Serial.println("sx1509 IC communication FAILED!");
	  while(1);
  }
  Serial.println();
  
}

void loop()
{
  //Get the data from the sensor bar and load it into the class members
  uint8_t rawValue = mySensorBar.getRaw();
  
  //Print the binary value to the serial buffer.
  Serial.print("Bin value of input: ");
  for( int i = 7; i >= 0; i-- )
  {
    Serial.print((rawValue >> i) & 0x01);
  }
  Serial.println("b");

  
  //Print the position
  Serial.print("Position (-127 to 127): ");
  Serial.println(mySensorBar.getPosition());
  
  //Wait 2/3 of a second
  delay(666);

}



