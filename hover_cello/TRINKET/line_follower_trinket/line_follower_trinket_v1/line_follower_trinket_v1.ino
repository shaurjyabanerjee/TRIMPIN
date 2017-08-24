//Hover Cello Trinket Firmware Ver. 0

//Steps in programming an Adafruit Pro Trinket 5V

// 1. Select the "Pro Trinket 5V/16Mhz (USB)" from Tools -> Board Menu
// 2. Select the "USBtinyISP" from Tools -> Programmer 
// 3. Make sure the Trinket is in bootloader mode by pressing the button
// 4. Upload your code within 10 seconds of pressing the button

//PWM frequency raised to 31372.55 Hz for silent motor operation
//Bitwise Style motor control logic

//Shaurjya Banerjee 2017

#include "Wire.h"
#include "sensorbar.h"

//Set pins for Motor 1
int enA = 9;
int in1 = 3;
int in2 = 4;
//Set pins for Motor 2
int enB = 10;
int in3 = 5;
int in4 = 6;

//Set pins for Motor Speed trimmers
int trim_pin1 = 2;
int trim_pin2 = 3;

//Set pin for line color switch
int color_pin = 12;

//Set pin for speed switch
int switch_pin = 13;

//Variables to hold motor speed offsets
int motorA_trim = 0;
int motorB_trim = 0;

//Delay variable for zero degree turning
int zero_time = 0;

//Variables for max and min adaptive zero time
int zt_max = 800;
int zt_min = 300;

//Variable for average motor speed
int avg_speed = 0;

//Variable for boost ammount
int boost_amt = 35;

//Variables to hold speed and color states
int speed_state = 0;
int color_state = 0;

//Variable to hold motor speed
int motor_speeds [] = {0, 0};

//Set I2C address as per hardware jumpers on SparkFun sensor bar
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

  //Setting output pinmode for motor pins
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //Setting input pinmode for control pins
  pinMode(switch_pin, INPUT);
  pinMode(color_pin, INPUT);

  delay(200);

  //Read color pin to determine line color
  color_state = digitalRead(color_pin);

  //Dark line on light floor
  if (color_state == LOW)
  {
    mySensorBar.clearInvertBits();
    Serial.println("Dark Line on Light Floor");
  }

  //Light line on dark floor
  else if (color_state == HIGH)
  {
    mySensorBar.setInvertBits();
    Serial.println("Light Line on Dark Floor");
  }

  //To enable IR only during reads (lower current consumption)
  //mySensorBar.setBarStrobe();
  
  //To enable IR all the time (easier to get visual feedback)
  mySensorBar.clearBarStrobe();
    
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
   //d32  == 00100000
   //d64  == 01000000
   //d128 == 10000000
   else if ( ((temp & 32) == 32) || ((temp & 64) == 64) || ((temp & 128) == 128) )
   {
      drive_motorA(1);
      drive_motorB(0);
      delay(zero_time);
   }
  
   //Now for the states where we correct left
   //d4 == 00000100
   //d2 == 00000010
   //d1 == 00000001
   else if ( ((temp & 4) == 4) || ((temp & 2) == 2) || ((temp & 1) == 1) )
   {
      drive_motorA(0);
      drive_motorB(1);
      delay(zero_time);
   }
}

//Function to set speeds using PWM on enable pins of L298
//Accepts 2 position integer array []
void set_speed (int spd [])
{
      analogWrite(enA, spd[0]);
      analogWrite(enB, spd[1]);
}

//Fucntion to read, scale and limit values from motor speed trimmers
void read_trimmers()
{
   //Read values from analog inputs
   motorA_trim = analogRead(trim_pin1);
   motorB_trim = analogRead(trim_pin2);

   //Divide values by 4 to get 8 bit integers
   //Constrain top speed to 200 
   //to protect L298 from bi-directional surge current
   motor_speeds[0] = constrain( (motorA_trim/4), 0, 200 );
   motor_speeds[1] = constrain( (motorB_trim/4), 0, 200 );

   //Calculate average speed for adaptive zero_time
   avg_speed = ((motor_speeds[0] + motor_speeds[1])/2);

   //Scale and constrain zero time based on motor speed
   zero_time = map(avg_speed, 150, 200, zt_max, zt_min);
   zero_time = constrain(zero_time, zt_min, zt_max);

   read_switch();
}

//Function to read pins to set global motor speed state
void read_switch()
{
  speed_state = digitalRead(switch_pin);

  //High speed Mode
  if (speed_state == HIGH)
  {
     //Offset the trimmer values up by 50
     motor_speeds[0] = motor_speeds[0] + boost_amt;
     motor_speeds[1] = motor_speeds[1] + boost_amt;

     //Turn off adaptive zero_time for high speed operation
     zero_time = 450;
     
     set_speed(motor_speeds);
  }
  
  //Low speed Mode
  else if (speed_state == LOW)
  {
    set_speed(motor_speeds);
  }
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
