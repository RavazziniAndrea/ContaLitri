#include <ping.h>
#include <ESP32Ping.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <FastLED.h>

#define UNIT_LEDS 42
#define TENS_LEDS 42
#define HUND_LEDS 42
#define THOU_LEDS 42
#define SIGN_A_LEDS 45 
#define SIGN_B_LEDS 45 
#define UNIT_PIN 33
#define TENS_PIN 25
#define HUND_PIN 26
#define THOU_PIN 27
#define SIGN_A_PIN 14
#define SIGN_B_PIN 12

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* psw = "REPLACE_WITH_YOUR_PASSWORD";
const String server = "http://ADDRESS:PORT/LOCATION";

CRGB units[UNIT_LEDS];
CRGB tens[TENS_LEDS];
CRGB hundreds[HUND_LEDS];
CRGB thousands[THOU_LEDS];
CRGB sign_a[SIGN_A_LEDS];
CRGB sign_b[SIGN_B_LEDS];

/*
 *    e
 *  d   f
 *    g 
 *  c   a
 *    b
 */
byte arr_0[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35}   //f
                ;
byte arr_1[] = {0,1,2,3,4,5,         //a
                30,31,32,33,34,35}   //f
                ;
byte arr_2[] = {6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
byte arr_3[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
byte arr_4[] = {0,1,2,3,4,5,         //a
                18,19,20,21,22,23,   //d
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
byte arr_5[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                36,37,38,39,40,41}   //g
                ;
byte arr_6[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                36,37,38,39,40,41}   //g
                ;
byte arr_7[] = {0,1,2,3,4,5,         //a
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35}   //f
                ;
byte arr_8[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
byte arr_9[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
          
int old_liters = 0;
int old_light = 0;
int r=254;
int b=0;
int g=0;

int liters=0;
int light=50;
byte test=0;

void setup() {
  Serial.begin(115200);
  ledSetup();
  delay(1000);
  //initial values, whenever it will read real values, it refresh them
  turnOnSign();
  refreshLeds(8888);
  FastLED.setBrightness(50);
  FastLED.show();

  wifiSetup();
}

void loop() {
  String payload = getPayload();
  Serial.println(payload);
  if(test != 1)
  {
    int ind_separator = payload.indexOf('#');
    if (ind_separator != -1) {
      int payload_length = payload.length();
      liters = payload.substring(0, ind_separator).toInt();
      light  = payload.substring(ind_separator + 1, payload_length).toInt();
      //Serial.println(liters);
      //Serial.println(light);
    }
  }
  else
  {
    liters = (liters+1111)%10000; //random numbers when it doesn't connect to the network 
  }
  if(liters != 0/* && light != 0*/)
  {
    turnOnSign();
    refreshLeds(liters);
    FastLED.setBrightness(light);
    FastLED.show();
  }
  else
  {
    Serial.print("ERROR connectin to the server");
  }

  refreshColors();

  delay(1000);
}

void refreshColors()
{
  if(r>0 && b<=254 && g==0)
  {
    r--; 
    b++;
  }
  else
  {
    if(b>0 && g<=254 && r==0)
    {
      b--; 
      g++;
    }
    else
    {
      g--; 
      r++;
    }
  }
}

void turnOnSign()
{
  for(byte i=0;i<sizeof(sign_a);i++)
  {
    sign_a[i] = CRGB(0,0,0);
    sign_b[i] = CRGB(0,0,0);
    sign_a[i] = CRGB(r,g,b);
    sign_b[i] = CRGB(r,g,b);
  }
}

void refreshLeds(int liters)
{
  Serial.println(liters);
  for(byte i=0;i<TENS_LEDS;i++) 
  {
    units[i]=CRGB(0,0,0);
    tens[i]=CRGB(0,0,0);
    hundreds[i]=CRGB(0,0,0);
    thousands[i]=CRGB(0,0,0);
  }
  writeUnits(liters%10);
  if(liters>=10) writeTens((liters/10)%10);
  if(liters>=100) writeHundreds((liters/100)%10);
  if(liters>=1000) writeThousands((liters/1000)%10);
}

void writeUnits(int num)
{
  switch(num)
  {
    case 0:
      for(byte i=0;i<sizeof(arr_0);i++) units[arr_0[i]] = CRGB(r,g,b);
    break;
    case 1:
      for(byte i=0;i<sizeof(arr_1);i++) units[arr_1[i]] = CRGB(r,g,b);
    break;
    case 2:
      for(byte i=0;i<sizeof(arr_2);i++) units[arr_2[i]] = CRGB(r,g,b);
    break;
    case 3:
      for(byte i=0;i<sizeof(arr_3);i++) units[arr_3[i]] = CRGB(r,g,b);
    break;
    case 4:
      for(byte i=0;i<sizeof(arr_4);i++) units[arr_4[i]] = CRGB(r,g,b);
    break;
    case 5:
      for(byte i=0;i<sizeof(arr_5);i++) units[arr_5[i]] = CRGB(r,g,b);
    break;
    case 6:
      for(byte i=0;i<sizeof(arr_6);i++) units[arr_6[i]] = CRGB(r,g,b);
    break;
    case 7:
      for(byte i=0;i<sizeof(arr_7);i++) units[arr_7[i]] = CRGB(r,g,b);
    break;
    case 8:
      for(byte i=0;i<sizeof(arr_8);i++) units[arr_8[i]] = CRGB(r,g,b);
    break;
    case 9:
      for(byte i=0;i<sizeof(arr_9);i++) units[arr_9[i]] = CRGB(r,g,b);
    break;
    default:
    break;
  }
}
void writeTens(int num)
{
  switch(num)
  {
    case 0:
      for(byte i=0;i<sizeof(arr_0);i++) tens[arr_0[i]] = CRGB(r,g,b);
    break;
    case 1:
      for(byte i=0;i<sizeof(arr_1);i++) tens[arr_1[i]] = CRGB(r,g,b);
    break;
    case 2:
      for(byte i=0;i<sizeof(arr_2);i++) tens[arr_2[i]] = CRGB(r,g,b);
    break;
    case 3:
      for(byte i=0;i<sizeof(arr_3);i++) tens[arr_3[i]] = CRGB(r,g,b);
    break;
    case 4:
      for(byte i=0;i<sizeof(arr_4);i++) tens[arr_4[i]] = CRGB(r,g,b);
    break;
    case 5:
      for(byte i=0;i<sizeof(arr_5);i++) tens[arr_5[i]] = CRGB(r,g,b);
    break;
    case 6:
      for(byte i=0;i<sizeof(arr_6);i++) tens[arr_6[i]] = CRGB(r,g,b);
    break;
    case 7:
      for(byte i=0;i<sizeof(arr_7);i++) tens[arr_7[i]] = CRGB(r,g,b);
    break;
    case 8:
      for(byte i=0;i<sizeof(arr_8);i++) tens[arr_8[i]] = CRGB(r,g,b);
    break;
    case 9:
      for(byte i=0;i<sizeof(arr_9);i++) tens[arr_9[i]] = CRGB(r,g,b);
    break;
    default:
    break;
  }
}
void writeHundreds(int num)
{
  switch(num)
  {
    case 0:
      for(byte i=0;i<sizeof(arr_0);i++) hundreds[arr_0[i]] = CRGB(r,g,b);
    break;
    case 1:
      for(byte i=0;i<sizeof(arr_1);i++) hundreds[arr_1[i]] = CRGB(r,g,b);
    break;
    case 2:
      for(byte i=0;i<sizeof(arr_2);i++) hundreds[arr_2[i]] = CRGB(r,g,b);
    break;
    case 3:
      for(byte i=0;i<sizeof(arr_3);i++) hundreds[arr_3[i]] = CRGB(r,g,b);
    break;
    case 4:
      for(byte i=0;i<sizeof(arr_4);i++) hundreds[arr_4[i]] = CRGB(r,g,b);
    break;
    case 5:
      for(byte i=0;i<sizeof(arr_5);i++) hundreds[arr_5[i]] = CRGB(r,g,b);
    break;
    case 6:
      for(byte i=0;i<sizeof(arr_6);i++) hundreds[arr_6[i]] = CRGB(r,g,b);
    break;
    case 7:
      for(byte i=0;i<sizeof(arr_7);i++) hundreds[arr_7[i]] = CRGB(r,g,b);
    break;
    case 8:
      for(byte i=0;i<sizeof(arr_8);i++) hundreds[arr_8[i]] = CRGB(r,g,b);
    break;
    case 9:
      for(byte i=0;i<sizeof(arr_9);i++) hundreds[arr_9[i]] = CRGB(r,g,b);
    break;
    default:
    break;
  }
}
void writeThousands(int num)
{
  switch(num)
  {
    case 0:
      for(byte i=0;i<sizeof(arr_0);i++) thousands[arr_0[i]] = CRGB(r,g,b);
    break;
    case 1:
      for(byte i=0;i<sizeof(arr_1);i++) thousands[arr_1[i]] = CRGB(r,g,b);
    break;
    case 2:
      for(byte i=0;i<sizeof(arr_2);i++) thousands[arr_2[i]] = CRGB(r,g,b);
    break;
    case 3:
      for(byte i=0;i<sizeof(arr_3);i++) thousands[arr_3[i]] = CRGB(r,g,b);
    break;
    case 4:
      for(byte i=0;i<sizeof(arr_4);i++) thousands[arr_4[i]] = CRGB(r,g,b);
    break;
    case 5:
      for(byte i=0;i<sizeof(arr_5);i++) thousands[arr_5[i]] = CRGB(r,g,b);
    break;
    case 6:
      for(byte i=0;i<sizeof(arr_6);i++) thousands[arr_6[i]] = CRGB(r,g,b);
    break;
    case 7:
      for(byte i=0;i<sizeof(arr_7);i++) thousands[arr_7[i]] = CRGB(r,g,b);
    break;
    case 8:
      for(byte i=0;i<sizeof(arr_8);i++) thousands[arr_8[i]] = CRGB(r,g,b);
    break;
    case 9:
      for(byte i=0;i<sizeof(arr_9);i++) thousands[arr_9[i]] = CRGB(r,g,b);
    break;
    default:
    break;
  }
}


String getPayload() {
  String payload;

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(server);
    delay(100);
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      payload = http.getString();
      delay(100);
    }
    else {
      payload = "ERROR code: " + httpResponseCode;
    }
    // Free resources
    http.end();
    delay(300);
  }
  else {
    Serial.println("WiFi Disconnected");
    payload = "Wifi disconnected";
  }
  return payload;
}

void ledSetup()
{
  FastLED.addLeds<WS2812, UNIT_PIN, GRB>(units, UNIT_LEDS);
  FastLED.addLeds<WS2812, TENS_PIN, GRB>(tens, TENS_LEDS);
  FastLED.addLeds<WS2812, HUND_PIN, GRB>(hundreds, HUND_LEDS);
  FastLED.addLeds<WS2812, THOU_PIN, GRB>(thousands, THOU_LEDS);
  FastLED.addLeds<WS2812, SIGN_A_PIN, GRB>(sign_a, SIGN_A_LEDS);
  FastLED.addLeds<WS2812, SIGN_B_PIN, GRB>(sign_b, SIGN_B_LEDS);
}

void wifiSetup()
{
  WiFi.begin(ssid, psw);
  Serial.print("Connecting");
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    count++;
    if(count >= 60)
    {
      //Serial.println("Cannot connect to the network. Starting random numbers");
      test=1;
      return;
    }
  }
  Serial.println("");
  Serial.println("Connected to WiFi network.");
}
