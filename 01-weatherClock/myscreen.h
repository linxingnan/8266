#ifndef MY_SCREEN_H
#define MY_SCREEN_H
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     D1
#define TFT_RST    D2  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     D3

#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

void testTft(void);
void initScreen(void);

extern Adafruit_ST7735 tft;

#endif
