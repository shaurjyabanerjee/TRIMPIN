//SparkFun Line Follower Array Test 2
//Using LED indicators instead of motors
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
   //d126 == 01111110
   temp = temp & 126;
   
   //Or remove the last two bits on the extreme left and right
   //(Dont do this just now, it wont work)
   //d60 == 00111100
   //temp = temp & 60;

   //Debug expressions
   //bitwise_int_print(temp & 255);

   //If the robot is off the ground
   //d126 == 01111110
   if ( (temp & 126) == 126 )
   {
      Serial.println("robot is off the ground");
      digitalWrite(motor1_pin, LOW);  //Turn off left motor
      digitalWrite(motor2_pin, LOW);  //Turn off right motor
   }
  
   //First lets set the centered state, with both motors running
   //d24  == 00011000
   else if ( (temp & 24) == 24 )
   {
       Serial.println("centered");
       digitalWrite(motor1_pin, HIGH);  //Turn on left motor
       digitalWrite(motor2_pin, HIGH);  //Turn on right motor
   }
     
   //Now for the states where we correct right
   //d16 == 00010000
   //d32 == 00100000
   //d64 == 01000000
   else if ( ((temp & 16) == 16)||((temp & 32) == 32)||((temp & 64) == 64) )
   {
      Serial.println("correcting right");
      digitalWrite(motor1_pin, LOW);  //Turn on left motor
      digitalWrite(motor2_pin, HIGH);  //Turn on right motor
   }
  
   //Now for the states where we correct left
   //d8 == 00001000
   //d4 == 00000100
   //d2 == 00000010
   else if ( ((temp & 8) == 8)||((temp & 4) == 4)||((temp & 2) == 2) )
   {
      Serial.println("correcting left");
      digitalWrite(motor1_pin, HIGH);  //Turn on left motor
      digitalWrite(motor2_pin, LOW);  //Turn on right motor
   }
}

//Function to check extreme left and right for performance markers
void check_markers (int raw)
{
    //Check extreme left for performance marker
    if ( (raw & 128) == 128 )
    {
       Serial.println("left performance marker");
    }

    //Check extreme right for performance marker
    if ( (raw & 128) == 128 )
    {
       Serial.println("right performance marker");
    }
}

