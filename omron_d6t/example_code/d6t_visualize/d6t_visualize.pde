//Visualize Data from Omron D6T
//Shaurjya Banerjee 2017

import processing.serial.*;

//Serial Information
String portName = "/dev/cu.usbmodemFD121"; //Serial port name
Serial port;   //Main port object
int BaudRate = 9600;
String inString;  //String that will be processed

//Temparature Variables
float[]tdata = new float [17];  //Ref and temp data go here

//Misc Variables
String buff;
PFont font;
float colorGreen;
int lbTemp = 60;
int hbTemp = 90;

void setup ()
{
   size(640, 640);
   
   port = new Serial(this, portName, BaudRate);
   port.bufferUntil('\n');
   
   font = loadFont("SansSerif.vlw");
   textFont(font, 40);
}

void draw ()
{
  background (0,0,0);
  
  for (int i =0; i<16; i++)
  {
     colorGreen = map(tdata[i], lbTemp, hbTemp, 255, 0);
     fill (255, colorGreen, 0);
     
     //Create rectangles
     rect(( i%4 ) * 160, floor(i/4)*160, 160, 160);
     
     if (tdata[i]<5)
     {
        fill(255); 
     }
     
     else
     {
        fill(0); 
     }
     
     //Center text at 80 x 80
     textAlign(CENTER, CENTER);
     textSize(40);
     text(str(tdata[i]),(i%4)*160+80, floor(i/4)*160+80);
     textSize(20);
     
     text("Relataive Temperature "+str(tdata[16]),140,10);
  }
}

void serialEvent(Serial port)
{
   inString = port.readString();
   tdata = float(split(inString, ','));
}