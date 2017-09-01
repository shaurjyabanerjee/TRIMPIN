//Visualizer for the Adafriut AMG88

//Shaurjya Banerjee - 2017

import processing.serial.*;
Serial serial;
boolean jsonDataStarted = false; 
String jsondata = "[0]";
String jsondata_temp = "[0]";
JSONArray json;

int drawingMode;

void setup()
{
  size(640, 640, P3D);

  translate(width/2, height/2, 0);
  rotateX(0.5);
  rotateY(0.5);
  //rotateZ(0.5);

  frameRate(60);

  fill(63, 127, 255);

  textSize(8);
  stroke(255);
  println(Serial.list());
  String arduinoPort = Serial.list()[1];
  serial = new Serial(this, arduinoPort, 115200);
}


void draw()
{
  background(128);
  
  if (!jsonDataStarted)
  {
    jsondata = jsondata_temp;
  }
  
  float max, min, dataRange;

  translate(width/2, height/2);    //立体の中心を画面中央に移動   
  rotateY(radians(mouseX-width/2)/5);
  rotateX(radians(mouseY-height/2)/5);

  json = JSONArray.parse(jsondata);
  //print(json);

  max = json.getFloat(0);
  min = json.getFloat(0);
  for (int i=0; i<json.size(); i++)
  {
    if (max < json.getFloat(i)) max = json.getFloat(i);
    if (min > json.getFloat(i)) min = json.getFloat(i);
  }
  dataRange = max-min;

  for (int i=0; i<json.size()/8; i++)
  {
    for (int l=0; l<json.size()/8; l++)
    {
      pushMatrix();
      // translate(50*l -200, 200-50*i, 0);
      translate(200 - 50*l, 200-50*i, 0);
      colorMode(HSB, 1.0);
      switch(drawingMode)
      {
      case 1: 
        fill(0.75 - ((json.getFloat(i*8+l)-min)/dataRange)*0.75, 1.0, 1.0);
        break;
      case 2:
        fill(0.75 - ((json.getFloat(i*8+l)+20)/120)*0.75, 1.0, 1.0);
        break;
      default:
        fill(0.75 - ((json.getFloat(i*8+l)-20)/20)*0.75, 1.0, 1.0);
        break;
      }
      box(50, 50, ((json.getFloat(i*8+l)-min)/dataRange)*50);
      fill(0, 0, 0);
      text(json.getFloat(i*8+l), -10, -10, ((json.getFloat(i*8+l)-min)/dataRange)*25 + 1);
      colorMode(RGB, 100);
      popMatrix();
    }
  }
  
}

void serialEvent(Serial p)
{
  char temp = (char)p.read();
  if (temp == '[')
  {
    jsonDataStarted = true;
    jsondata_temp = "";
  }
  if (jsonDataStarted)
  {
    jsondata_temp += String.valueOf(temp);
    if (temp == ']')
    {
      jsonDataStarted = false;
    }
  }
}

void keyPressed()
{
  if (key == BACKSPACE)
  {
    drawingMode++;
    if (drawingMode > 2) drawingMode =0;
  }
}