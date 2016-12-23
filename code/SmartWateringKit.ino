/*
 *author: Derron form LinkSprite
 *mail: delong.qi@linksprite.com 
 */

#include <SoftwareSerial.h>
#include<LinkSpriteIO.h>
#include "DHT.h"

#define DHTTYPE DHT22
const int LCDdelay=10;
const int txPin = 14;
const int DHTPIN = 4;
const int soilPin = 16;
const int lightPin = 0;
const int switch_key = 15;
String deviceID = "xxxxxxxxxx";
String apikey = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

LinkSpriteIO linksprite(deviceID,apikey);
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial LCD = SoftwareSerial(0, txPin);

void lcdPosition(int row, int col) 
{
  LCD.write(0xFE);   //command flag
  LCD.write((col + row*64 + 128));    //position 
  delay(LCDdelay);
}

void clearLCD()
{
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}

void backlightOn() 
{ 
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  delay(LCDdelay);
}

void backlightOff()
{  
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
  delay(LCDdelay);
}

void serCommand()
{   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}

int get_light()
{
   float data;
   int val;
   val = analogRead(lightPin);
   data = val / 10.24;
   return (int)data;
}

void setup()
{
  linksprite.begin();
  pinMode(soilPin,INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(switch_key, OUTPUT);
  Serial.begin(115200);
  LCD.begin(9600);
  backlightOn() ;
  clearLCD();
  dht.begin();
  linksprite.update("water","0");
  lcdPosition(0,0);
  LCD.print("AutoWaterflower");
  delay(4000);
  clearLCD();
}
 
void loop()
{
     char tmp[10];
     char hum[10];
     char light[10];
     int light_val = 0;
     int soil_val;
     int water;
     int count=0;
     while(1)
     {
       count ++;
       if(count > 1000)
          count = 0;
       float h = dht.readHumidity();
       float t = dht.readTemperature();
       float f = dht.readTemperature(true);
       light_val = get_light();
       soil_val = digitalRead(soilPin);
       float hif = dht.computeHeatIndex(f, h);
       float hic = dht.computeHeatIndex(t, h, false);
       itoa(t,tmp,10);
       itoa(h,hum,10);
       itoa(light_val,light,10);
       if(count % 2 == 1)
       {
         clearLCD();
         lcdPosition(0,0);
         LCD.print("Humidity:");
         LCD.print(h);
         lcdPosition(1,0);
         LCD.print("Temperat:");
         LCD.print(t);
         lcdPosition(1,0);
       }
       else
       {
          clearLCD();
          lcdPosition(0,0);
          LCD.print("Light(%):");
          LCD.print(light_val);
          lcdPosition(1,0);
          LCD.print("SoilMois:");
          if(soil_val == 1)
              LCD.print("Dry");
           else
              LCD.print("Wet"); 
       }
       Serial.println("Soil:");
       Serial.println(soil_val);
       Serial.println("Hum:");
       Serial.println(h);
       Serial.println("Tem:");
       Serial.println(t);
       Serial.println("Light:");
       Serial.println(light_val);
       if(soil_val == 1)
       {
          linksprite.update("Soil_Mois","Wet");
       }
       else
          linksprite.update("Soil_Mois","Dry");
       linksprite.update("temperature",tmp);
       linksprite.update("humidity",hum);
       linksprite.update("light(%)",light);
       water = linksprite.query("water");
       if(water == 1)
       {
          digitalWrite(switch_key,HIGH);
          delay(100);
          digitalWrite(switch_key,LOW);
       }
       else
          digitalWrite(switch_key,LOW);
     }
}