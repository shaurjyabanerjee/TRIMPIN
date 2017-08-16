//SparkFun Line Follower Array Test 2
//Optimizing for direct drive BLDC motors
//Bitwise Style

//Shaurjya Banerjee 2017

#include "Wire.h"
#include "sensorbar.h"

//Set motor start/stop pins 
int motor1_pin = 2;
int motor2_pin = 4;

//Set I2C address as per hardware jumpers
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
  
  //Setting output pinmode for motor start/stop
  pinMode(motor1_pin, OUTPUT);
  pinMode(motor2_pin, OUTPUT);

  //Set default state for motor enable pins
  digitalWrite(motor1_pin, HIGH);
  digitalWrite(motor2_pin, HIGH);

  //Wait 5 seconds on startup
  delay (5000);
  
}

void loop()
{
  //Get the data from the sensor bar and load it into the class members
  uint8_t rawValue = mySensorBar.getRaw();

  //call line follower function
  follow_line(rawValue);
}

//Function to print binary values using a serial buffer (for debugging)
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
 
   //Debug expressions
   //bitwise_int_print(temp & 255);

   //If the robot is off the ground or if the robot sees no line
   //d255 == 11111111
   if ( (temp & 255) == 255 || (temp & 255) == 0 )
   {
      Serial.println("robot is off the ground");
      digitalWrite(motor1_pin, HIGH);  //Turn off left motor
      digitalWrite(motor2_pin, HIGH);  //Turn off right motor
   }
  
   //First lets set the centered state, with both motors running
   //d16 == 00010000
   //d8  == 00001000
   else if ( ((temp & 16) == 16) || ((temp & 8) == 8) )
   {
       //Serial.println("centered");
       digitalWrite(motor1_pin, LOW);  //Turn on left motor
       digitalWrite(motor2_pin, LOW);  //Turn on right motor
   }
     
   //Now for the states where we correct right
   //d32 == 00100000
   //d64 == 01000000
   else if ( ((temp & 32) == 32) || ((temp & 64) == 64) )
   {
      //Serial.println("correcting right");
      digitalWrite(motor1_pin, HIGH);  //Turn on left motor
      digitalWrite(motor2_pin, LOW);  //Turn on right motor
   }
  
   //Now for the states where we correct left
   //d4 == 00000100
   //d2 == 00000010
   else if ( ((temp & 4) == 4)||((temp & 2) == 2) )
   {
      //Serial.println("correcting left");
      digitalWrite(motor1_pin, LOW);  //Turn on left motor
      digitalWrite(motor2_pin, HIGH);  //Turn on right motor
   }

   //Explicitly ignore edge cases
   //d128 == 10000000 
   //d1   == 00000001
   else if ( ((temp & 128) == 128)||((temp & 1) == 1) )
   {
      //Serial.println("edge cases");
      digitalWrite(motor1_pin, HIGH);  //Turn on left motor
      digitalWrite(motor2_pin, HIGH);  //Turn on right motor
   }
}

//Function to check extreme left and right for performance markers
void check_markers (int raw)
{
    //Check extreme left for performance marker
    if ( (raw & 128) == 128 )
    {
       //Serial.println("left performance marker");
    }

    //Check extreme right for performance marker
    if ( (raw & 1) == 1 )
    {
       //Serial.println("right performance marker");
    }
}

