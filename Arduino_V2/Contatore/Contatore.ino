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
#define MILLIS_SERVER_GET 3000

// const char* ssid = "REPLACE_WITH_YOUR_SSID";
// const char* psw = "REPLACE_WITH_YOUR_PASSWORD";
// const String server = "http://ADDRESS:PORT/LOCATION";

const String servers[] = {
  "http://localhost:8000/sum",
  "https://server_address:8000/sum"
};

struct Network{
  const char* ssid,
  const char* psw
};

Network networks[] {
  {"SSID 1", "PASS 1"},
  {"SSID 2", "PASS 2"}
};

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
const const byte arr_0[] = {2,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35}   //f
                ;
const byte arr_1[] = {0,1,2,3,4,5,         //a
                30,31,32,33,34,35}   //f
                ;
const byte arr_2[] = {6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
const byte arr_3[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
const byte arr_4[] = {0,1,2,3,4,5,         //a
                18,19,20,21,22,23,   //d
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
const byte arr_5[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                36,37,38,39,40,41}   //g
                ;
const byte arr_6[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                36,37,38,39,40,41}   //g
                ;
const byte arr_7[] = {0,1,2,3,4,5,         //a
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35}   //f
                ;
const byte arr_8[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                12,13,14,15,16,17,   //c
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
const byte arr_9[] = {0,1,2,3,4,5,         //a
                6,7,8,9,10,11,       //b
                18,19,20,21,22,23,   //d
                24,25,26,27,28,29,   //e
                30,31,32,33,34,35,   //f
                36,37,38,39,40,41}   //g
                ;
          
int old_liters = 0;
int old_light = 0;
uint8_t hue = 0;

unsigned long lastRequest = 0;

int liters=0;
int light=50;
bool testMode=false;

void setup() {
  Serial.begin(115200);
  ledSetup();
  delay(300);
  //initial values, whenever it will read real values, it refresh them
  setSignColor();
  refreshLeds(8888);
  FastLED.setBrightness(50);
  FastLED.show();

  wifiSetup();

  randomSeed(analogRead(0));
}

void loop() {
  unsigned long now = millis();
  if(now - lastRequest >= MILLIS_SERVER_GET || lastRequest == 0){
    lastRequest = now;
    String payload = getPayload();
    Serial.println(payload);
    if(!testMode) {
      int ind_separator = payload.indexOf('#');
      if (ind_separator != -1) {
        int payload_length = payload.length();
        liters = payload.substring(0, ind_separator).toInt();
        light  = payload.substring(ind_separator + 1, payload_length).toInt();
        //Serial.println(liters);
        //Serial.println(light);
      }
      //TODO manage better ways to detect a bad payload!!
    }
    else {
      liters = random(1000, 9999);
    }

    if(liters != old_liters) {
      old_liters = liters;
      refreshLeds(liters);
      FastLED.setBrightness(light);
    }
  
    hue++;
    setSignColor();
    FastLED.show();
  }
}


void setSignColor()
{
  fill_solid(sign_a, SIGN_A_LEDS, CHSV(hue, 255, 255));
  fill_solid(sign_b, SIGN_B_LEDS, CHSV(hue, 255, 255));
}

void refreshLeds(int liters)
{
  Serial.println(liters);

  fill_solid(units, UNIT_LEDS, CRGB::Black);
  fill_solid(tens, TENS_LEDS, CRGB::Black);
  fill_solid(hundreds, HUND_LEDS, CRGB::Black);
  fill_solid(thousands, THOU_LEDS, CRGB::Black);

  writeDigit(liters%10, units);
  if(liters>=10) writeDigit((liters/10)%10, tens);
  if(liters>=100) writeDigit((liters/100)%10, hundreds);
  if(liters>=1000) writeDigit((liters/1000)%10, thousands);
}

void writeDigit(int num, CRGB* digit)
{
  switch(num)
  {
    case 0:
      for(byte i=0;i<sizeof(arr_0);i++) digit[arr_0[i]] = CHSV(hue, 255, 255);
    break;
    case 1:
      for(byte i=0;i<sizeof(arr_1);i++) digit[arr_1[i]] = CHSV(hue, 255, 255);
    break;
    case 2:
      for(byte i=0;i<sizeof(arr_2);i++) digit[arr_2[i]] = CHSV(hue, 255, 255);
    break;
    case 3:
      for(byte i=0;i<sizeof(arr_3);i++) digit[arr_3[i]] = CHSV(hue, 255, 255);
    break;
    case 4:
      for(byte i=0;i<sizeof(arr_4);i++) digit[arr_4[i]] = CHSV(hue, 255, 255);
    break;
    case 5:
      for(byte i=0;i<sizeof(arr_5);i++) digit[arr_5[i]] = CHSV(hue, 255, 255);
    break;
    case 6:
      for(byte i=0;i<sizeof(arr_6);i++) digit[arr_6[i]] = CHSV(hue, 255, 255);
    break;
    case 7:
      for(byte i=0;i<sizeof(arr_7);i++) digit[arr_7[i]] = CHSV(hue, 255, 255);
    break;
    case 8:
      for(byte i=0;i<sizeof(arr_8);i++) digit[arr_8[i]] = CHSV(hue, 255, 255);
    break;
    case 9:
      for(byte i=0;i<sizeof(arr_9);i++) digit[arr_9[i]] = CHSV(hue, 255, 255);
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
    for(auto srv : servers){
      http.begin(srv);
      int code = http.GET();
      if (code == 200) {
        payload = http.getString();
        http.end();
        break;
      }
      else {
        payload = "ERROR code: " + code;
      }
      http.end();
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    payload = "Wifi disconnected";
    wifiSetup();
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
  int num_to_debug = 7777;
  for(Network network : networks){
    WiFi.begin(network.ssid, network.psw);
    Serial.print("Connecting");
    int count = 0;
    while(WiFi.status() != WL_CONNECTED && count < 20) {
      delay(500);
      refreshLeds(num_to_debug);
      Serial.print(".");
      count++;
    }

    Serial.println("");
    if(WiFi.status() == WL_CONNECTED){
      Serial.println("Connected to WiFi network.");
      return;
    }
    num_to_debug = 9999;
  }
  Serial.println("Cant connect. Entering testMode");
  testMode = true;
}
