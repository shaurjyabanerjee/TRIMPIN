//SparkFun Line Follower Array Test 2
//Using LED indicators instead of motors
//Bitwise Style

//Shaurjya Banerjee 2017

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
  
  //Setting output pinmode for LED indicators
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  
}

void loop()
{
  //Get the data from the sensor bar and load it into the class members
  uint8_t rawValue = mySensorBar.getRaw();
  
  follow_line(rawValue);
  
  delay(50);
}

//Function to print binary values using a serial buffer
void bitwise_int_print(int raw)
{
  Serial.print("SensorBar Readout : ");
  for( int i = 7; i >= 0; i-- )
  {
    Serial.print((raw >> i) & 0x01);
  }
  Serial.println(" "); 
}

//Function to follow a line 
void follow_line(int raw)
{
   int temp = raw;
   
   //Remove the last bit on the extreme left and right
   //126d == 01111110
   temp = temp & 126;
   
   //Remove the last two bits on the extreme left and right
   //60d == 00111100
   //temp = temp & 60;
   
   bitwise_int_print(temp & 24);
  
   //First lets set the centered state, with both motors running
   if (temp & 24 == 24)
   {
       Serial.println("centered!!!");
   }
     
  
  
  
}


