/*
Ver.0.0.2  
2018/05/25
add fish_temp > 28 c then fan open
 
 
Ver0.0.1
08:0-20:00 Open FAN
LCD display Time & Temperature & FAN1 FAN2 states.
Arduino Nano

SimpleTimer library for Arduino  //timer
https://github.com/jfturcot/SimpleTimer

Dallas_Temperature_Control_Library //DS1820
https://github.com/milesburton/Arduino-Temperature-Control-Library

LiquidCrystal_I2C Library  //2004 I2C LCD
https://github.com/marcoschwartz/LiquidCrystal_I2C

DS1307 Sample Code (Chinese)
http://lolikitty.pixnet.net/blog/post/145656536


*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleTimer.h>

#define ONE_WIRE_BUS 14  //ds1820 pin

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 20, 4); //i2c address,20 character,4line

SimpleTimer timer;

const byte DS1307_I2C_ADDRESS = 0x68; // DS1307 (I2C) 地址address
const byte NubberOfFields = 7; // DS1307 (I2C) 資料範圍 Data range

int y; // 年year
byte m, d, w, h, mi, s; // 月 month/日 date/週week day/時 hour/分 minute/秒sec

const int lcd_switch=3;
const int fan1_switch=12;
const int fan2_switch=11;

const int relay1=5;
const int relay2=7;

int lcd_backlight;
int fan1;
int fan2;


float fish_tempL=30.0;
float fish_tempS=27.0;
float fish_temp1;

void setup(void)
{
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  Wire.begin();
  lcd.init();
  lcd.backlight();
 // Serial.begin(9600);
  //Serial.println("Temperature Sensor");
  // 初始化 init
  sensors.begin();
  //setTime(18,5,27,7,20,25,30);

  lcd.setCursor(4, 0); //set lcd,character 4, line 0
  lcd.print("/"); //print / at character 4 line 0
  lcd.setCursor(7, 0);
  lcd.print("/");
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(5, 1);
  lcd.print(":");
}

void loop(void)
{
  timer.run();

  //Sensor conversion temperature.
  sensors.requestTemperatures();

  // 取得溫度讀數（攝氏）並輸出， Get  temperature(Celsius).
  // 參數0代表匯流排上第0個1-Wire裝置  0 is One-Wire bus first Sensor.
  //Serial.println(sensors.getTempCByIndex(0));
  timer.setInterval(1000, getTime); // 取得時間
  timer.setInterval(1000, digitalClockDisplay); // 顯示時間

  lcd.setCursor(9, 1);
  lcd.print("Temp: ");
  lcd.print(sensors.getTempCByIndex(0));  //display temperature.
  fish_temp1=sensors.getTempCByIndex(0);
  
  lcd.setCursor(0, 3);
  lcd.print("FAN1 OFF");    
  lcd.setCursor(9, 3);
  lcd.print("FAN2 OFF");  
  
  lcd_backlight=digitalRead(lcd_switch);
  if(lcd_backlight==HIGH)  //open or close LCD backlight.
    lcd.backlight();
  else
    lcd.noBacklight();

  fan1=digitalRead(fan1_switch);
  fan2=digitalRead(fan2_switch); 

  if((h>=8)||(h<=22))  //8:00-20:00 open fan
  {
    if(fan1==HIGH)
    {
      digitalWrite(relay1,LOW); //relay is LOW working
      lcd.setCursor(0, 3);
      lcd.print("FAN1 ON ");
     // Serial.println("FAN1 0");
    }
    else
    {   
        if(fish_temp1>fish_tempL)
        {
            digitalWrite(relay1,LOW); //relay is LOW working
            lcd.setCursor(0, 3);
            lcd.print("FAN1 ON ");
           // Serial.println("FAN1 0");

        }
        if(fish_temp1<fish_tempS)
        {
            digitalWrite(relay1,HIGH);
            lcd.setCursor(0, 3);
            lcd.print("FAN1 OFF");
            //Serial.println("FAN1 1");
        }
    }
  }
  else
  {
    digitalWrite(relay1,HIGH);
    lcd.setCursor(0, 3);
    lcd.print("FAN1 OFF");    
   // Serial.println("FAN1 2");
  }
  if((h>=8)||(h<=20))
  {
    if(fan2==HIGH)
    {
      digitalWrite(relay2,LOW);
      lcd.setCursor(9, 3);
      lcd.print("FAN2 ON ");
    }
    else
    {
        if(fish_temp1>fish_tempL)
        {
            digitalWrite(relay2,LOW);
            lcd.setCursor(9, 3);
            lcd.print("FAN2 ON ");
          
        }
      if(fish_temp1<fish_tempS)
      { 
            digitalWrite(relay2,HIGH);
            lcd.setCursor(9, 3);
            lcd.print("FAN2 OFF");      
      }
    }
  }
  else
  {
    digitalWrite(relay2,HIGH );
    lcd.setCursor(9, 3);
    lcd.print("FAN2 OFF");
  }  

  delay(120);
}


// BCD 轉 DEC
byte bcdTodec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

// DEC 轉 BCD
byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

// 設定時間
void setTime(byte y, byte m, byte d, byte w, byte h, byte mi, byte s) {
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(s));
  Wire.write(decToBcd(mi));
  Wire.write(decToBcd(h));
  Wire.write(decToBcd(w));
  Wire.write(decToBcd(d));
  Wire.write(decToBcd(m));
  Wire.write(decToBcd(y));
  Wire.endTransmission();
}

// 取得時間
void getTime() {
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_I2C_ADDRESS, NubberOfFields);

  s = bcdTodec(Wire.read() & 0x7f);
  mi = bcdTodec(Wire.read());
  h = bcdTodec(Wire.read() & 0x7f);
  w = bcdTodec(Wire.read());
  d = bcdTodec(Wire.read());
  m = bcdTodec(Wire.read());
  y = bcdTodec(Wire.read()) + 2000;
}

// 顯示時間
void digitalClockDisplay() {
 /* Serial.print(y);
  Serial.print("/");
  Serial.print(m);
  Serial.print("/");
  Serial.print(d);
  Serial.print(" ( ");
  Serial.print(w);
  Serial.print(" ) ");
  Serial.print(h);
  Serial.print(":");
  Serial.print(mi);
  Serial.print(":");
  Serial.println(s);*/
  lcd.setCursor(0, 0);
  lcd.print(y);

  lcd.setCursor(5, 0);
  if (m < 10)
  {
    lcd.print("0");
  }  
  lcd.print(m);

  lcd.setCursor(8, 0);
  if (d < 10)
  {
    lcd.print("0");
  }  
  lcd.print(d);

  lcd.setCursor(13, 0);
  lcd.print("Week: ");
  lcd.print(w);
  
  lcd.setCursor(0, 1);
  if (h < 10)
  {
    lcd.print("0");
  }
  lcd.print(h);

  lcd.setCursor(3, 1);
  if (mi < 10)
  {
    lcd.print("0");
  }
  lcd.print(mi);

  lcd.setCursor(6, 1);
  if (s < 10)
  {
    lcd.print("0");
  }
  lcd.print(s);


}