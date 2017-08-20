//Tester for speed balancing on DC motor pairs
//Upload this firmware and observe the hoverboard's trajectory
//Staright line trajectory means both motors are balanced
//Deviation means one of the motors is running slower,
//And that a motor speed offset needs to be calculated

//Firmware has not been tested yet!

//Shaurjya Banerjee 2017

//Set pins for Motor 1
int enA = 10;
int in1 = 9;
int in2 = 8;
//Set pins for Motor 2
int enB = 5;
int in3 = 7;
int in4 = 6;

//Set individual motor speeds here
int motor_speeds [] = {255, 255};

//Set test length here
int test_length = 10000;

void setup() 
{
  Serial.begin(9600);
  
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

  //Wait two seconds after powerup
  delay (2000);
}

void loop() 
{

   set_speed(motor_speeds);
   
   drive_motorA(0);
   drive_motorB(0);
   
   delay(test_length);
   
   stop_motorA();
   stop_motorB();
}

//Function to set speeds using PWM on enable pins
void set_speed (int spd [])
{
      analogWrite(enA, spd[0]);
      analogWrite(enB, spd[1]);
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

