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
#define NEOPIXEL_PIN 9
#define NUM_LEDS 29
#define NUM_LEDS_HERE 12
#define LEDS_HERE_START 18
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int delayVal = 50;
int DISPLAY_DELAY = 5000;//ms

// Colors used in the random flash method.
// just add new {nnn, nnn, nnn}, lines. They will be picked out randomly
//                          R   G   B
uint8_t greenColors[][3] = {{30, 44, 0},    // leaf_green
                            {0, 24, 0},     // green
                            {0, 50, 24},    // sapphire
                            {12, 36, 23},   // mediumSeaGreen
                            {0, 50, 20},    // mediumSpringGreen
                            };
#define FAVCOLORS sizeof(greenColors) / 3

uint32_t white         = strip.Color(24, 24, 24);
uint32_t hot_pink      = strip.Color(48, 0, 24);
uint32_t light_blue    = strip.Color(0, 24, 24);
uint32_t bright_blue   = strip.Color(10, 24, 50);
uint32_t leaf_green    = strip.Color(30, 44, 0);
uint32_t purple        = strip.Color(17, 0, 44);
uint32_t yellowOrange  = strip.Color(44, 30, 0);
uint32_t sapphire      = strip.Color(0, 50, 24);
uint32_t green         = strip.Color(0, 24, 0);
uint32_t red           = strip.Color(24, 0, 0);
uint32_t blue          = strip.Color(0, 0, 24);
uint32_t salmon_pink   = strip.Color(236, 79, 100); 
uint32_t cream         = strip.Color(246, 216, 180);
uint32_t lavendar      = strip.Color(174, 113, 208);
uint32_t deep_purple   = strip.Color(91, 44, 86); 
uint32_t medium_purple = strip.Color(232, 100, 255);
uint32_t yellow        = strip.Color(200, 200, 20);
uint32_t mediumSeaGreen       = strip.Color(12, 36, 23);
uint32_t mediumSpringGreen    = strip.Color(0, 50, 20);
uint32_t lightGreen       = strip.Color(10, 50, 10);
uint32_t honeyDew         = strip.Color(30, 51, 30);
uint32_t rosyBrown        = strip.Color(38, 35, 35);
uint32_t thistle          = strip.Color(43, 24, 43);
uint32_t plum             = strip.Color(44, 20, 44);
uint32_t mediumOrchid     = strip.Color(44, 10, 44);
uint32_t mediumPurple     = strip.Color(44, 5, 44);
uint32_t mediumSlateBlue  = strip.Color(10, 10, 50);
uint32_t cornFlowerBlue   = strip.Color(5, 10, 50);
uint32_t skyBlue          = strip.Color(20, 30, 50);
uint32_t lightSkyBlue     = strip.Color(27, 30, 50);
uint32_t paleTurquoise    = strip.Color(20, 30, 48);
uint32_t powderBlue       = strip.Color(25, 35, 50);
uint32_t aquamarine       = strip.Color(25, 51, 42);
uint32_t turquoise        = strip.Color(13, 45, 42);
uint32_t mediumTurquoise  = strip.Color(14, 42, 41);
uint32_t lime  = strip.Color(0, 100, 0);

//************************ NEOPIXEL PLASMA ************************//
// Convenient 2D point structure
struct Point {
  float x;
  float y;
};

float phase = 0.0;
float phaseIncrement = 0.03;  // Controls the speed of the moving points. Higher == faster. I like 0.08 .
float colorStretch = 0.11; // Higher numbers will produce tighter color bands. I like 0.11 .

//**********************************************************************//

uint32_t pixelColors[NUM_LEDS];
uint32_t color;
float fadeRate = 0.96;

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

enum mode {
  SOMEWHERE,
  HERE
};

mode currentMode = SOMEWHERE;

int RECV_PIN = 5;

IRrecv irrecv(RECV_PIN);

decode_results results;

//********************* Motion Sensor ************************//
const int MOTION_PIN = 2; // Pin connected to motion detector
const int LED_PIN = 13; // LED pin - active-high
 
void setup() {
  Serial.begin(9600);
  Serial.println("IR-instructable-simple");
  irrecv.enableIRIn(); // Start the receiver

  // Motion sensor setup
  // The PIR sensor's output signal is an open-collector, 
  // so a pull-up resistor is required:
  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // Neopixel setup
  strip.begin();
  chasePersist(NUM_LEDS, blue); // initialize all pixels to color
}
 
void loop() {

  Serial.println("reading sensor...");
  int proximity = digitalRead(MOTION_PIN);
  if (proximity == LOW) { // If the sensor's output goes low, motion is detected
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Motion detected!");
    if (currentMode == SOMEWHERE) {
      indicateSomewhere();
    } else if (currentMode == HERE) {
      indicateHere();
    }
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }

  Serial.println("reading IR...");
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if (results.value == IR_REMOTE_SELECT) {
      Serial.println("SELECT");
      currentMode = SOMEWHERE;
      indicateSomewhere();
    }
    else if (results.value == IR_REMOTE_PLAY) {
      Serial.println("PLAY");
      currentMode = HERE;
      indicateHere();
    }

    irrecv.resume(); // Receive the next value
  }

} // end loop


void indicateSomewhere() {
  blue_sparkles();
  chasePersist(NUM_LEDS, blue);
}
void indicateHere() {
  // turn off the LEDS behind "SOMEW"
  for (int i = 0; i < LEDS_HERE_START; i++) {
    strip.setPixelColor(i, 0);
  }
  for (int i = 0; i < 500; i++) {
    neopixel_plasma();
  }
    rainbowCycle(15);
}

