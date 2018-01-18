#include <FastLED.h>
#include <EEPROM.h>
#define NUM_LEDS 300
#define LEDPIN 6
#define LEDPINEXAMPLE 7

bool draw = false;
CRGB leds[NUM_LEDS];
CRGB ledExample[1];
int red = 0; const int redDown = 53; const int redUp = 52;
int green = 0; const int greenDown = 51; const int greenUp = 50;
int blue = 255; const int blueDown = 49; const int blueUp = 48;
int brightness = 255; const int brightnessDown = 44; const int brightnessUp = 45;

const int relay = 42; //Relay  as  an input

int segmentLength = 3;

//delayTime is the amount of time before the leds move
int delayTime = 500; const int delayDown = 46; const int delayUp = 47;

void IO(bool write = true){
  int address = 0;
  if(write) {
    EEPROM.put(address, red);
    address += sizeof(int);
    EEPROM.put(address, green);
    address += sizeof(int);
    EEPROM.put(address, blue);
    address += sizeof(int);
    EEPROM.put(address, brightness);
    address += sizeof(int);
    EEPROM.put(address, delayTime);
    address += sizeof(int);
  } else {
    EEPROM.get(address, red);
    address += sizeof(int);
    EEPROM.get(address, green);
    address += sizeof(int);
    EEPROM.get(address, blue);
    address += sizeof(int);
    EEPROM.get(address, brightness);
    address += sizeof(int);
    EEPROM.get(address, delayTime);
    address += sizeof(int);    
  }
  Serial.println("****IO****");
}

void setup() {
  //Serial.begin(9600);
  FastLED.addLeds<WS2812B, LEDPIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, LEDPINEXAMPLE, GRB>(ledExample, 1);

  //Set Buttons as inputs
  pinMode(redDown, INPUT_PULLUP); pinMode(redUp, INPUT_PULLUP);
  pinMode(greenDown, INPUT_PULLUP); pinMode(greenUp, INPUT_PULLUP);
  pinMode(blueDown, INPUT_PULLUP); pinMode(blueUp, INPUT_PULLUP);
  pinMode(brightnessDown, INPUT_PULLUP); pinMode(brightnessUp, INPUT);
  pinMode(delayDown, INPUT_PULLUP); pinMode(delayUp, INPUT_PULLUP);

  //Setup the relay as an input
  pinMode(relay, INPUT_PULLUP);

  //Retreive all data from EEPROM
  IO(false);
}

void input(){
  
  bool changed = false;
  if(digitalRead(redUp) == LOW){
    red++;  
    changed = true;
  }
  if(digitalRead(redDown) == LOW)  {
   red--;
   changed = true;
  }
  red = constrain(red, 0, 255);
  
  if(digitalRead(greenUp) == LOW){
    green++;  
    changed = true;
  }
  if(digitalRead(greenDown) == LOW) {
    green--;
    changed = true;
  }
  green = constrain(green, 0, 255);
  
  if(digitalRead(blueUp) == LOW){
    blue++;  
    changed = true;
  }
  if(digitalRead(blueDown) == LOW)  {
    blue--;
    changed = true;
  }
  blue = constrain(blue, 0, 255);
  
  if(digitalRead(brightnessUp) == LOW){
    brightness++;
    changed = true;
  }
  if(digitalRead(brightnessDown) == LOW){
    brightness--;
    changed = true;
  }
  brightness = constrain(brightness, 0, 255);
  
  if(digitalRead(delayUp) == LOW){
    delayTime += 5;
    changed = true;
  }
  if(digitalRead(delayDown) == LOW){
    delayTime -= 5;
    changed = true;
  }
  delayTime = constrain(delayTime, 5, 1000);
  
  if(changed)
  {
    IO();
  }

  if(digitalRead(relay) == LOW) {
    draw = true;
  } else draw = false;
  
}

CRGB pixel;
CRGB setLED(bool on){ 
  pixel = CRGB::Black;
  if (on)
  {
    pixel.r = red;
    pixel.g = green;
    pixel.b = blue;
  }

  return pixel;
}

int counter = 0;
int offset = 0;//Offset goes up to segmentLength so it seems like the leds flow
bool flip = true;
void teardrop(){  
  flip = true;
  counter = 0;
  for(int i = offset - segmentLength;  i <= NUM_LEDS/2; i++)
  {
    if(i >= 0)
    {
      leds[i] = setLED(flip);
    }
    counter++;
    if(counter >= segmentLength)
    {
      counter = 0;
      flip = !flip;
    }
    Serial.println(i);
  }  
  flip = true;
  counter = 0;
  for(int i = NUM_LEDS - offset + segmentLength + segmentLength - 1; i > NUM_LEDS/2; i--)
  {
    if(i < NUM_LEDS)
    {
      leds[i] = setLED(flip);
    }
    counter++;
    if(counter >= segmentLength)
    {
      counter = 0;
      flip = !flip;
    }
    Serial.println(i);
  }
  offset++;
  if(offset >= segmentLength*2)
    offset = 0;

  //FastLED.setBrightness(brightness);
  FastLED.show();  
  FastLED.clear(false);
}

void loop() {
  bool test = true;
  if(draw == true){
    input();
    teardrop();
    delay(delayTime);
  } else {
    FastLED.clear(false);
    input();
    ledExample[0] = setLED(true);
    FastLED.show();
    delay(25);  
  }
}
