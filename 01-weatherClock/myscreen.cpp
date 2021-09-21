#include "myscreen.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void initScreen(void)
{
    // Use this initializer if you're using a 1.8" TFT
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

// Use this initializer (uncomment) if you're using a 1.44" TFT
//  tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
    tft.setTextWrap(false); // Allow text to run off right edge
    tft.fillScreen(ST7735_BLACK);

}

void testTft(void)
{
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);

    tft.setCursor(0, 30);
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(1);
    tft.println("Hello World!");    
}
