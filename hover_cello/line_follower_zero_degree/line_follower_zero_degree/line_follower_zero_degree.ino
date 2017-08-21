//SparkFun Line Follower Array Test
//Written to work with DC motors and the L298 driver
//Now with speed offset trimmers per motor
//PWM frequency raised to 31372.55 Hz for silent motor operation
//Bitwise Style

//Shaurjya Banerjee 2017

#include "Wire.h"
#include "sensorbar.h"

//Set pins for Motor 1
int enA = 10;
int in1 = 8;
int in2 = 7;
//Set pins for Motor 2
int enB = 9;
int in3 = 6;
int in4 = 5;

//Set pins for Motor Speed trimmers
int trim_pin1 = 1;
int trim_pin2 = 2;

//Variables to hold motor speed offsets
int motorA_trim = 0;
int motorB_trim = 0;

//Delay variable for zero degree turning and average speed
int zero_time = 0;
int avg_speed = 0;

//Variable to hold motor speed
int motor_speeds [] = {0, 0};

//Set I2C address as per hardware jumpers
const uint8_t SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
//const byte SX1509_ADDRESS = 0x3F;  // SX1509 I2C address (01)
//const byte SX1509_ADDRESS = 0x70;  // SX1509 I2C address (10)
//const byte SX1509_ADDRESS = 0x71;  // SX1509 I2C address (11)

SensorBar mySensorBar(SX1509_ADDRESS);

void setup()
{
  Serial.begin(9600);  // start serial for output

  //Set PWM frequency to 31372.55 Hz for timer1 (phase-correct PWM)
  //This puts the PWM frequency outside audio rate for silent motor operation
  TCCR1B = (TCCR1B & 0b11111000) | 0x01;
  
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
  
  //.begin() gets the bar ready
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
  
  //Setting output pinmode for motor pins
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //Setting default pin states to low on startup
  digitalWrite(enA, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(enB, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
 
  //Wait 5 seconds on startup
  delay (5000);
}

void loop()
{
  //Get the data from the sensor bar and load it into the class members
  uint8_t rawValue = mySensorBar.getRaw();

  read_trimmers();
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

   //If the robot is off the ground or if the robot sees no line
   //d255 == 11111111
   //d0   == 00000000
   if ( (temp & 255) == 255 || (temp & 255) == 0 )
   {
      stop_motorA();
      stop_motorB();
   }
  
   //First lets set the centered state, with both motors running
   //d16 == 00010000
   //d8  == 00001000
   else if ( ((temp & 16) == 16) && ((temp & 8) == 8) )
   {
       drive_motorA(0);
       drive_motorB(0);
   }
     
   //Now for the states where we correct right
   //d32 == 00100000
   //d64 == 01000000
   else if ( ((temp & 32) == 32) || ((temp & 64) == 64) )
   {
      drive_motorA(1);
      drive_motorB(0);
      delay(zero_time);
   }
  
   //Now for the states where we correct left
   //d4 == 00000100
   //d2 == 00000010
   else if ( ((temp & 4) == 4)||((temp & 2) == 2) )
   {
      drive_motorA(0);
      drive_motorB(1);
      delay(zero_time);
   }

   //Explicitly ignore edge cases
   //d128 == 10000000 
   //d1   == 00000001
   else if ( ((temp & 128) == 128)||((temp & 1) == 1) )
   {  
      stop_motorA();
      stop_motorB();
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

//Function to set speeds using PWM on enable pins
void set_speed (int spd [])
{
      analogWrite(enA, spd[0]);
      analogWrite(enB, spd[1]);
}

void read_trimmers()
{
   motorA_trim = analogRead(trim_pin1);
   motorB_trim = analogRead(trim_pin2);

   motor_speeds[0] = constrain( (motorA_trim/4), 0, 200 );
   motor_speeds[1] = constrain( (motorB_trim/4), 0, 200 );

   avg_speed = ((motor_speeds[0] + motor_speeds[1])/2);

   zero_time = map(avg_speed, 150, 200, 1000, 450);
   zero_time = constrain(zero_time, 450, 1000);
   
   set_speed(motor_speeds);
}

//Function to drive motor A
//Use 0 or 1 for int dir
void drive_motorA (int dir)
{   
    //Drive one direction
    if (dir == 0)
    {
       digitalWrite(in1, HIGH);
       digitalWrite(in2, LOW);
    }
    //Drive the other direction
    else if (dir == 1)
    {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }
    //Error case
    else 
    {
      Serial.println("Use 0 or 1 for Direction!");
    }
}

//Function to stop motor A
void stop_motorA ()
{
   digitalWrite(in1, LOW);
   digitalWrite(in2, LOW);
}

//Function to drive motor B
//Use 0 or 1 for int dir
//Use 0 to 255 for int spd 
void drive_motorB (int dir)
{   
    //Drive one direction
    if (dir == 0)
    {
       digitalWrite(in3, HIGH);
       digitalWrite(in4, LOW);
    }
    //Drive the other direction
    else if (dir == 1)
    {
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
    }
    //Error case
    else 
    {
      Serial.println("Use 0 or 1 for Direction!");
    }
}

//Function to stop motor B
void stop_motorB ()
{
   digitalWrite(in3, LOW);
   digitalWrite(in4, LOW);
}

