/* Trinket/Gemma compatible IR read sketch
This sketch/program uses an Adafruit Trinket or Gemma
ATTiny85 based mini microcontroller and a PNA4602 or TSOP38238 to
read an IR code and perform a function.  
Based on Adafruit tutorial http://learn.adafruit.com/ir-sensor/using-an-ir-sensor
and http://learn.adafruit.com/trinket-gemma-mini-theramin-music-maker
*/

#include <IRremote.h>

//******** Neopixel ***********//
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
#define NEOPIXEL_PIN 4
#define NUM_LEDS 10
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint32_t blue          = strip.Color(0, 0, 255);
uint32_t leaf_green    = strip.Color(30, 44, 0);
uint32_t hot_pink      = strip.Color(48, 0, 24);
uint32_t yellowOrange  = strip.Color(44, 30, 0);

int delayVal = 50;

// Convenient 2D point structure
struct Point {
  float x;
  float y;
};

float phase = 0.0;
float phaseIncrement = 0.03;  // Controls the speed of the moving points. Higher == faster. I like 0.08 .
float colorStretch = 0.11;    // Higher numbers will produce tighter color bands. I like 0.11 .

// ******** IR ******** //
const uint32_t IR_REMOTE_POWER     = 0x8322A15E;
const uint32_t IR_REMOTE_SELECT    = 0x8322A659;
const uint32_t IR_REMOTE_MUTE      = 0x8322AE51;
const uint32_t IR_REMOTE_V_UP      = 0x8322A25D;
const uint32_t IR_REMOTE_V_DOWN    = 0x8322A35C;
const uint32_t IR_REMOTE_MODE      = 0x8322B24D;
const uint32_t IR_REMOTE_REWIND    = 0x8322A55A;
const uint32_t IR_REMOTE_PLAY      = 0x8322B04F;
const uint32_t IR_REMOTE_FORWARD   = 0x8322A45B;
const uint32_t HEADER              = 0x83220000;

const uint32_t MASK = 0xFFFE0000;

int RECV_PIN = 5;

IRrecv irrecv(RECV_PIN);

decode_results results;
 
void setup() {
  Serial.begin(9600);
  Serial.println("IR-instructable-simple");
  irrecv.enableIRIn(); // Start the receiver

  // Neopixel setup
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
 
void loop() {
  
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if (results.value == IR_REMOTE_SELECT) {
      Serial.println("SELECT");
      chase(hot_pink, false);
      chase(blue, true);
    }
    else if (results.value == IR_REMOTE_PLAY) {
      Serial.println("PLAY");
      chase(leaf_green, false);
      chase(blue, true);
      
    } else {
      Serial.println("OTHER BUTTON");
      chase(yellowOrange, false);
      chase(blue, true);
    }

    irrecv.resume(); // Receive the next value
  }

  

} // end loop
 

static void chase(uint32_t color, boolean turnOff) {
  for(uint16_t i=0; i<strip.numPixels()+4; i++) {
      strip.setPixelColor(i  , color); // Draw new pixel
      if (turnOff) {
        strip.setPixelColor(i-1, 0); // Erase pixel a few steps back
      }
      strip.show();
      delay(delayVal);
  }
}
