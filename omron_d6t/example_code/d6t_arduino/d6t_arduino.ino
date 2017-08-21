//I2C Library
#include <Wire.h>

#define D6T_ID 0x0A    //I2C address for the D6T
#define D6T_CMD 0x4C   //Command to get information

//The D6T will return 35 bytes of data to be processed
//There variables will store them
int ReadBuffer[35];     //D6T Buffer
float ptat;             //reference temparature inside sensor
float tdata[16];        //temporary temperature data for 16 pixels (4x4)

void setup() 
{
  Wire.begin();
  Serial.begin(9600);
  delay(500);  

}

void loop() 
{
  int i;
  //Requesting data from D6T
  Wire.beginTransmission(D6T_ID);
  Wire.write(D6T_CMD);
  Wire.endTransmission();

  //Getting the data and parsing it
  Wire.requestFrom(D6T_ID, 35);

  //Putting this data into a memory/buffer
  for (i =0; i<35; i++)
  {
    ReadBuffer[i] = Wire.read();
  }

  //Processing thedata into Celcius
  //Byte 0-1 = Reference Temp
  //Byte 2-33 = Temperature Data
  //Byte 34 = Packet Check Error
  
  ptat = (ReadBuffer[0]+(ReadBuffer[1]*256))*0.1;  //Reference temp
  
  //Temperature Data
  for (i =0; i<16; i++)
  {
    tdata[i] = (ReadBuffer[(i*2+2)]+(ReadBuffer[(i*2+3)]*256))*0.1;
  }

  //Display somewhere
  float tempF;
  
  //Print Reference Temp in Farenheit
  //First, check if there is data
  if ( ((tdata[0]*9.0/5.0) + 32.0)>0 )
  {
    for (i = 0; i<16; i++)
    {
      tempF = (tdata[i]*9.0/5.0) + 32.0;

      //Now to send the information over Serial
      Serial.print(tempF);
      Serial.print(',');
    }
    
    //Line breaks for serial parsing
    Serial.print((ptat*9.0/5.0)+32.0);
    Serial.print(',');
    Serial.println();
  }
}
