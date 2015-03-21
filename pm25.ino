#include "U8glib.h"
#include "DHT.h"

U8GLIB_ST7920_128X64 u8g(3, 9, 8, U8G_PIN_NONE);  
#define         GP2Y10_POWER          (2) //4号脚
#define         GP2Y10_PIN            (0)  //23
#define         DHT11_PIN             (13)   //19
#define         Backlight_Switch      (6)   //12
#define         Backlight_PIN         (1)    //3
#define         VOC_PIN               (4)  //24

int PM25 = 0;
int AQI = 0;
int Backlight_status=1;
float time=0;
float nexttime=0;
float time_status=1;

static unsigned char jia[] U8G_PROGMEM = {
  0x00,0x20,0x7F,0xF0,0x44,0x20,0x44,0x20,
  0x7F,0xE0,0x44,0x20,0x44,0x20,0x7F,0xE0,
  0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00
};//汉字甲
static unsigned char wen[] U8G_PROGMEM = {
  0x4F,0xC0,0x28,0x40,0x0F,0xC0,0x88,0x40,
  0x4F,0xC0,0x28,0x40,0x40,0x00,0xDF,0xE0,
  0x54,0xA0,0x54,0xA0,0x54,0xA0,0x7F,0xF0
};//汉字温
static unsigned char du[] U8G_PROGMEM = {
  0x02,0x00,0x3F,0xE0,0x24,0x80,0x3F,0xE0,
  0x24,0x80,0x27,0x80,0x20,0x00,0x2F,0x80,
  0x25,0x00,0x42,0x00,0x8D,0x80,0xB0,0x70
};//汉字"度"
static unsigned char quan[] U8G_PROGMEM = {
  0x01,0x40,0xFB,0xF0,0x21,0x40,0xF8,0x80,
  0xA9,0x40,0xAE,0x30,0xDB,0xE0,0x88,0x80,
  0xFB,0xE0,0x88,0x80,0xF8,0x80,0x8F,0xF0
};
//汉字"醛"
static unsigned char shi[] U8G_PROGMEM = {
  0x4F,0xE0,0x28,0x20,0x0F,0xE0,0x88,0x20,
  0x4F,0xE0,0x28,0x20,0x42,0x90,0xD2,0xA0,
  0x4A,0xC0,0x4A,0x80,0x42,0x80,0x7F,0xF0
};
//汉字"湿"
static unsigned char feng[] U8G_PROGMEM = {
  0x20,0x80,0xAA,0x80,0x72,0x40,0x22,0x40,
  0xFC,0x30,0x27,0xE0,0x71,0x20,0xA9,0x20,
  0xA1,0x20,0x22,0x20,0x22,0xA0,0x24,0x40
};
  //汉字"粉"
static unsigned char chen[] U8G_PROGMEM = {
  0x04,0x00,0x04,0x00,0x14,0x80,0x24,0x60,
  0x44,0x20,0x80,0x00,0x04,0x40,0x7F,0xE0,
  0x04,0x00,0x04,0x00,0x04,0x20,0xFF,0xF0
};
//汉字"尘"
static unsigned char duc[] U8G_PROGMEM = {
  0x00,0x00,0x67,0x00,0x68,0xC0,0x10,0x40,
  0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,
  0x10,0x40,0x08,0x80,0x07,0x00,0x00,0x00
};
//符号"度C"

DHT dht;
void setup()
{ 
  pinMode(GP2Y10_POWER, OUTPUT); 
  pinMode(DHT11_PIN, INPUT);
  pinMode(Backlight_Switch, INPUT);
  pinMode(Backlight_PIN, OUTPUT);
  pinMode(VOC_PIN, INPUT);
  dht.setup(DHT11_PIN);
  digitalWrite(Backlight_PIN,HIGH);
}

void loop()
{
  nexttime=millis();
  if(digitalRead(Backlight_Switch)==LOW)
  {
    switch(  Backlight_status){
      case 1:
      Backlight_status=0;
      digitalWrite(Backlight_PIN,LOW);
      break;
      case 0:
      Backlight_status=1;
      digitalWrite(Backlight_PIN,HIGH);
      break;
    }
  }
  PM25=Pm25(GP2Y10_POWER,GP2Y10_PIN);
  //AQI=AQI_EXCHANGE(PM25);
  int TEMP=dht.getTemperature(); 
  int HUMI = dht.getHumidity();
  float VOC=voc(VOC_PIN);

  if (nexttime-time>3000 || time_status==1){
    u8g.firstPage();
    do{
      draw(PM25,VOC,TEMP,HUMI);
      time=millis();
      nexttime=millis();
      time_status=0;
    }
    while(u8g.nextPage());
  } 

  else{
    nexttime=millis();
    time_status=0;
  }
}

void draw(int pm25,float voc,int temp,int humi) {

  u8g.drawRFrame(26,0,67,15,3); 
  u8g.drawRFrame(26,16,67,15,3);
  u8g.drawRFrame(26,32,67,15,3);
  u8g.drawRFrame(26,48,67,15,3);

  u8g.drawVLine(41,0,15);
  u8g.drawVLine(58,0,15);
  u8g.drawVLine(75,0,15);

  u8g.drawVLine(37,16,15);
  u8g.drawVLine(79,16,15);

  u8g.drawVLine(47,32,15);
  u8g.drawVLine(62,32,15);

  u8g.drawVLine(55,48,15);
  u8g.drawVLine(61,48,15);
 
  int width=int(pm25*63/150);
  if(width>63){
    width=63;
    u8g.drawRBox(28,2,width,11,2);
  }
  if(width>5 && width<=63){
    u8g.drawRBox(28,2,width,11,2);
  }
  
  width=int(voc*63/0.6);
  Serial.println(width);
  if(width>63){width=63;
    u8g.drawRBox(28,18,width,11,2);
  }
  if(width>5 && width<=63){
    u8g.drawRBox(28,18,width,11,2);
  }

  width=int(temp*63/100)+20;

  if(width>63){
    width=63;
    u8g.drawRBox(28,34,width,11,2);
  }
  if(width>5 && width<=63){
    u8g.drawRBox(28,34,width,11,2);
  }
  
  width=int(humi*63/100);
  if(width>63){
    width=63;
    u8g.drawRBox(28,50,width,11,2);
  }
  if(width>5 && width<=63){
    u8g.drawRBox(28,50,width,11,2);
  }
  

  u8g.drawBitmapP( 0, 2, 2,12, feng);
  u8g.drawBitmapP( 13, 2, 2,12, chen);
  u8g.drawBitmapP( 0, 18, 2,12, jia);
  u8g.drawBitmapP( 13, 18, 2,12, quan);
  u8g.drawBitmapP( 0, 34, 2,12, wen);
  u8g.drawBitmapP( 13, 34, 2,12, du);
  u8g.drawBitmapP( 0, 50, 2,12, shi);
  u8g.drawBitmapP( 13, 50, 2,12, du);

  //char pm=char(pm25);
  u8g.setFont(u8g_font_7x13);
  //int w=pm25.length();
  u8g.setPrintPos(94,12);
  u8g.print(pm25);
  //w=w*7+94;
  u8g.setPrintPos(115,12);
  u8g.print("ug");
  u8g.setPrintPos(94,28);
  u8g.print(voc);
  u8g.setPrintPos(94,44);
  //w=sizeof(temp);
  u8g.print(temp);
  //w=w*7+94;
  u8g.drawBitmapP(118,34, 2,12, duc);
  u8g.setPrintPos(94,60);
  u8g.print(humi);
  u8g.setPrintPos(108,60);
  u8g.print("%");
}


float Pm25(int ledpower,int dustpin)
{
  int delayTime=280;
  int delayTime2=40;
  int offTime=9680;
  float dustVal=0;
  int i=30;
  float pm25 = 0;
  pinMode(ledpower,OUTPUT);
  pinMode(dustpin,INPUT); 
  for(i=0;i<30;i++){
    digitalWrite(ledpower,LOW); // power on the LED
    delayMicroseconds(delayTime);
    dustVal +=analogRead(dustpin); // read the dust value
    delayMicroseconds(delayTime2);
    digitalWrite(ledpower,HIGH); // turn the LED off
    delayMicroseconds(offTime);
  }
  dustVal=dustVal/30;
  if(dustVal>184.5){
    pm25=0.83984375*dustVal-99.9;
  }
  else pm25=dustVal*0.298373984;
  return pm25;
}
/*
float  AQI_EXCHANGE(float pm25){
  float aqi=0;
  if(pm25<15.4) {
    aqi=(50-0)/(15.4-0)*(pm25-0)+0;
  }
  else if(pm25<40.4) aqi=(100-51)/(40.4-15.5)*(pm25-15.5)+51;
  else if(pm25<65.4) aqi=(150-101)/(65.4-40.5)*(pm25-40.5)+101;
  else if(pm25<150.4) aqi=(200-151)/(150.4-65.5)*(pm25-65.5)+151;
  else if(pm25<250.4) aqi=(300-201)/(250.4-150.5)*(pm25-150.5)+201;
  else if(pm25<350.4) aqi=(400-301)/(350.4-250.5)*(pm25-250.5)+301;
  else aqi=(500-401)/(500.4-350.5)*(pm25-350.5)+401;
  return aqi;
}
*/
float voc(int voc_pin){
  float ppm=0;
  float volt=analogRead(voc_pin);
  float volt1= volt*5/1024;
  if(volt1<=2.1 && volt1>0)
  {
    ppm=0.68*volt1-0.43;
  }  
  if((volt1<=2.38) && (volt1 > 2.1))
  {
    ppm=3.57*volt1-6.5;
  }
  if((volt1<=2.57) && (volt1 > 2.38))
  {
    ppm=5.26*volt1-10.52;
  }
  if((volt1<=2.71) && (volt1 > 2.57))
  {
    ppm=7.14*volt1-15.35;
  }
  if((volt1<=2.82) && (volt1 > 2.71))
  {
    ppm=9.09*volt1-20.64;
  }
  if((volt1<=2.92) && (volt1 > 2.82))
  {
    ppm=10*volt1-23.2;
  }
  if((volt1<=3.01) && (volt1 > 2.92))
  {
    ppm=11.11*volt1-26.44;
  }
  if((volt1<=3.09) && (volt1 > 3.01))
  {
    ppm=12.5*volt1-30.63;
  }
  if((volt1<=3.16) && (volt1 > 3.09))
  {
    ppm=14.29*volt1-36.16;
  }
  if((volt1<=3.49) && (volt1 > 3.16))
  {
    ppm=16.67*volt1-43.68;
  }
  if((volt1<=3.69) && (volt1 > 3.49))
  {
    ppm=25*volt1-72.25;
  }
  if((volt1<=3.87) && (volt1 > 3.69))
  {
    ppm=27.78*volt1-82.5;
  }
  if((volt1<=4.02) && (volt1 > 3.87))
  {
    ppm=33.33*volt1-104;
  }
  if((volt1<=4.15) && (volt1 > 4.02))
  {
    ppm=38.46*volt1-124.6;
  }
  if((volt1<=4.27) && (volt1 > 4.15))
  {
    ppm=41.67*volt1-137.93;
  }
  if((volt1<=4.38) && (volt1 > 4.27))
  {
    ppm=45.45*volt1-154.07;
  }
  if((volt1<=4.48) && (volt1 > 4.38))
  {
    ppm=50*volt1-174;
  }
  if((volt1<=4.57) && (volt1 > 4.48))
  {
    ppm=55.56*volt1-198.9;
  }
  if((volt1<=4.74) && (volt1 > 4.57))
  {
    ppm=55.56*volt1-198.9;
  }
  if((volt1<=5) && (volt1 > 4.74))
  {
    ppm=62.5*volt1-231.25;
  }
  float m=30/22.4*ppm;
  return m;
}
