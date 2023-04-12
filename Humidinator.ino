// Humidinator
// By Riley Knybel

// Uses an SSD1306 64*128 i2c OLED display
// to show temperature and relative humidity
// from a DHT 11 sensor 
// with graphs :)

#include <Arduino.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <DFRobot_DHT11.h>

#ifdef u8g2_HAVE_HW_I2C
#include <Wire.h>
#endif

//u8g2_SSD1306_128X64_NONAME_HW_I2C u8g2(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ u8g2_PIN_NONE);   // OLEDs without Reset of the Display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // ESP32 Thing, HW I2C with pin remapping

DFRobot_DHT11 DHT;
#define DHT11_PIN 10

// Logging variables
// These arrays hold the last 24 hours of temperature
// and humidity data points to show on screen.
int tempGraph[48];
int humiGraph[48];
int data_points = 48;
int mins_count = 0;
int samp_int = 30; //sample interval

void setup(void)
{
  
  u8g2.begin();
  u8g2.setFont(u8g2_font_inb24_mf);
  
}

void loop(void)
{
  //DHT11 sensor read
  DHT.read(DHT11_PIN);

  //log
  mins_count ++;
  if(mins_count >= samp_int) {
    push(tempGraph, DHT.temperature, data_points);
    push(humiGraph, DHT.humidity, data_points);
    mins_count = 0;
  }

  //screen update
  u8g2.clearBuffer();

  //temperature text
  u8g2.setCursor(0, 24);
  u8g2.print(c_to_f(DHT.temperature));
  u8g2.print(char(176));

  //temperature graph
  drawGraph(tempGraph, 80, 24, data_points);

  //humidity text
  u8g2.setCursor(64, 64);
  u8g2.print(DHT.humidity);
  u8g2.print("%");

  //humidity graph
  drawGraph(humiGraph, 0, 63, data_points);
  
  u8g2.sendBuffer();
  
  delay(59900); // 59900ms for screen refresh (just a guess)
}

int c_to_f(int c) {
  float deg_c = (float) c;
  return (int) (deg_c*1.8) +32;
}

void push (int * grapharr, int newval, int arrsize){
  //move everything back
  for (int i = 1; i < arrsize; i++){
    grapharr[i-1] = grapharr[i];
  }
  //place new value
  grapharr[arrsize-1] = newval;
}

void drawGraph(int * grapharr, int x, int y, int arrsize) {\
  for (int i = 0; i < arrsize; i++){
    if(i%2) //dashed bottom line
      u8g2.drawPixel(x + i, y);
    if( grapharr[i] != 0) //data line
      u8g2.drawPixel(x+i, map(grapharr[i], 0,100,0,-24) + y); //the font is 24 pixels so
    if(i%2) //dashed top line
      u8g2.drawPixel(x + i, y-24);
  }
}

// only push is needed, the graph
// arrays are write only
