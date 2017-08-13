//SparkFun Line Follower Array Test 2
//Line follower with LED indicators in place of motors
//Doing this using the direction metric provided by the SparkFun Library

//Shaurjya Banerjee 2017

#include "Wire.h"
#include "sensorbar.h"

// Uncomment one of the four lines to match your SX1509's address
//  pin selects. SX1509 breakout defaults to [0:0] (0x3E).
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
  
  //For this demo, the IR will only be turned on during reads.
  //mySensorBar.setBarStrobe();
  
  //Other option: Command to run all the time
  mySensorBar.clearBarStrobe();

  //Default dark on light
  mySensorBar.clearInvertBits();
  //Other option: light line on dark
  //mySensorBar.setInvertBits();
  
  //Don't forget to call .begin() to get the bar ready.  This configures HW.
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
  
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  
}

void loop()
{ 
   int dir = mySensorBar.getPosition();  
   int den = mySensorBar.getDensity();
   
   //Check if robot is on the ground
   if (den != 8 && den !=0)
   {
     //Centered position, drive both motors
     if (dir == 0)
     {
        Serial.println("Centered");
        digitalWrite(2, HIGH);
        digitalWrite(4,HIGH);
     }
     //Correct course to the right
     else if (dir < 0)
     {
        Serial.println("Correcting Right");
        digitalWrite(2, LOW);  //Do not power left motor
        digitalWrite(4, HIGH); //Only power right motor 
     }
     //Correct course to the left
     else if (dir > 0)
     {
        Serial.println("Correcting Left");
        digitalWrite(2, HIGH);  //Only power left motor
        digitalWrite(4, LOW);   //Do not power right motor 
     }
   }

   //If the robot is on the ground but detects no line,
   //Both motors shut off
   else if (den == 0)
   {
      Serial.println("No Line Detected");
      digitalWrite(2, LOW);   //Do not power either motor
      digitalWrite(4, LOW);   //Do not power either motor
   }
   
   else 
   {
      //Turn off both motors
      Serial.println("Robot is off the Ground!");
      digitalWrite(2, LOW);
      digitalWrite(4, LOW); 
   }
  
  //Wait
  delay(666);
}

