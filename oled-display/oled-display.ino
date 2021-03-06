/*********************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x64 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16  // 'ups_icon_55_64'


 // 'ups_icon_56_64'
const unsigned char myBitmap [] PROGMEM = {
  0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x1e, 
  0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x06, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x70, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x02, 0x40, 0x00, 0x00, 0x03, 0xff, 0xff, 0xf9, 0x40, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf9, 0x40, 
  0x00, 0x03, 0xff, 0xff, 0xff, 0xf9, 0x40, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf9, 0x40, 0x00, 0xff, 
  0xff, 0xff, 0xff, 0xf9, 0x40, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x40, 0x0f, 0xff, 0xff, 0xff, 
  0xff, 0xf9, 0x40, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x40, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf9, 
  0x40, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x43, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x47, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf9, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x4f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf9, 0x4c, 0x1f, 0x0f, 0x00, 0x7f, 0xc0, 0x79, 0x4c, 0x1f, 0x0e, 0x00, 0x3f, 0x80, 
  0x39, 0x4c, 0x1f, 0x0e, 0x00, 0x1f, 0x00, 0x39, 0x4c, 0x1f, 0x0e, 0x0c, 0x0e, 0x0f, 0x39, 0x4c, 
  0x1f, 0x0e, 0x1f, 0x0e, 0x1f, 0xb9, 0x4c, 0x1f, 0x0e, 0x1f, 0x06, 0x1f, 0xf9, 0x4c, 0x1f, 0x0e, 
  0x1f, 0x86, 0x1f, 0xf9, 0x4c, 0x1f, 0x0e, 0x1f, 0x86, 0x0f, 0xf9, 0x4c, 0x1f, 0x0e, 0x1f, 0x86, 
  0x03, 0xf9, 0x4c, 0x1f, 0x0e, 0x1f, 0x83, 0x01, 0xf9, 0x4c, 0x1f, 0x0e, 0x1f, 0x83, 0x80, 0x79, 
  0x4c, 0x1f, 0x0e, 0x1f, 0x83, 0xe0, 0x39, 0x4c, 0x1f, 0x0e, 0x1f, 0x87, 0xf0, 0x39, 0x4c, 0x1f, 
  0x0e, 0x1f, 0x87, 0xf8, 0x19, 0x4c, 0x1f, 0x0e, 0x1f, 0x87, 0xfc, 0x19, 0x4c, 0x1f, 0x0e, 0x1f, 
  0x07, 0xfc, 0x19, 0x4c, 0x1f, 0x0e, 0x1f, 0x0e, 0xfc, 0x19, 0x4e, 0x0e, 0x0e, 0x1c, 0x0e, 0x78, 
  0x38, 0x4e, 0x00, 0x0e, 0x00, 0x1e, 0x00, 0x38, 0x4f, 0x00, 0x0e, 0x00, 0x3e, 0x00, 0x7a, 0x4f, 
  0x80, 0x1e, 0x00, 0x7f, 0x00, 0xf2, 0x0f, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xf2, 0x27, 0xff, 0xfe, 
  0x1f, 0xff, 0xff, 0xf2, 0x27, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xe4, 0x03, 0xff, 0xfe, 0x1f, 0xff, 
  0xff, 0xe4, 0x13, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xc0, 0x09, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xc8, 
  0x08, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0x90, 0x04, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0x10, 0x02, 0x3f, 
  0xfe, 0x1f, 0xff, 0xfe, 0x20, 0x01, 0x1f, 0xfe, 0x1f, 0xff, 0xfc, 0x40, 0x00, 0x8f, 0xff, 0xff, 
  0xff, 0xf1, 0x80, 0x00, 0x43, 0xff, 0xff, 0xff, 0xc3, 0x00, 0x00, 0x30, 0xff, 0xff, 0xff, 0x0c, 
  0x00, 0x00, 0x0c, 0x3f, 0xff, 0xfe, 0x10, 0x00, 0x00, 0x03, 0x0f, 0xff, 0xf0, 0x40, 0x00, 0x00, 
  0x00, 0xc3, 0xff, 0xc1, 0x80, 0x00, 0x00, 0x00, 0x30, 0xff, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x0c, 
  0x1c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};





//// 'ups_icon_64_74'
//const unsigned char ups_bmp [] PROGMEM = {
//  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x07, 0xf0, 0x07, 0xff, 0xff, 
//  0xff, 0xfc, 0x1f, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 
//  0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 
//  0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 
//  0xbf, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xbf, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x0f, 
//  0xbf, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x0f, 0xbf, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x0f, 
//  0xbf, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xbf, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 
//  0xbf, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xbf, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
//  0xbf, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xbf, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
//  0xbf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
//  0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
//  0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
//  0xb1, 0x10, 0x22, 0x07, 0xf0, 0x00, 0x7e, 0x0f, 0xb1, 0xf0, 0x3e, 0x3f, 0xfe, 0x01, 0xff, 0x8f, 
//  0xb1, 0xf0, 0x3e, 0x3f, 0xff, 0x03, 0xff, 0x8f, 0xb1, 0xf0, 0x3e, 0x3f, 0xff, 0x87, 0xff, 0x8f, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x1f, 0x87, 0xc1, 0x8f, 0xb1, 0xf0, 0x3e, 0x3e, 0x0f, 0xcf, 0x80, 0x0f, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xcf, 0x80, 0x0f, 0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xcf, 0x80, 0x0f, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xef, 0xc0, 0x0f, 0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xe7, 0xe0, 0x0f, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x03, 0xe7, 0xf8, 0x0f, 0xb1, 0xf0, 0x3e, 0x3e, 0x03, 0xe3, 0xfe, 0x0f, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x03, 0xe1, 0xff, 0x0f, 0xb1, 0xf0, 0x3e, 0x3e, 0x03, 0xe0, 0x7f, 0x8f, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x03, 0xe0, 0x3f, 0x8f, 0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xe0, 0x0f, 0xcf, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xe0, 0x0f, 0xcf, 0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xc0, 0x07, 0xcf, 
//  0xb1, 0xf0, 0x3e, 0x3e, 0x07, 0xc0, 0x07, 0xcf, 0xb1, 0xf8, 0x3e, 0x3e, 0x0f, 0xc8, 0x07, 0xcf, 
//  0xb0, 0xfc, 0x7e, 0x3e, 0x1f, 0x86, 0x0f, 0xcd, 0xb0, 0xff, 0xfe, 0x3f, 0xff, 0x87, 0xff, 0x8d, 
//  0xb0, 0x7f, 0xfe, 0x3f, 0xff, 0x07, 0xff, 0x0d, 0xb8, 0x3f, 0xfc, 0x3f, 0xfe, 0x07, 0xfe, 0x0d, 
//  0xd8, 0x1f, 0xf0, 0x3f, 0xf8, 0x01, 0xfc, 0x0d, 0xd8, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x1d, 
//  0xd8, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x1b, 0xdc, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x1b, 
//  0xec, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3b, 0xee, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x37, 
//  0xf6, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x77, 0xf7, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0xef, 
//  0xfb, 0x80, 0x00, 0x3e, 0x00, 0x00, 0x00, 0xdf, 0xfd, 0xc0, 0x00, 0x3e, 0x00, 0x00, 0x01, 0xdf, 
//  0xfe, 0xe0, 0x00, 0x3e, 0x00, 0x00, 0x03, 0xbf, 0xff, 0x70, 0x00, 0x3e, 0x00, 0x00, 0x0f, 0x7f, 
//  0xff, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xff, 0xff, 0xde, 0x00, 0x00, 0x00, 0x00, 0x79, 0xff, 
//  0xff, 0xe7, 0x80, 0x00, 0x00, 0x01, 0xf7, 0xff, 0xff, 0xf9, 0xe0, 0x00, 0x00, 0x07, 0xcf, 0xff, 
//  0xff, 0xfe, 0xf8, 0x00, 0x00, 0x1f, 0x3f, 0xff, 0xff, 0xff, 0x3e, 0x00, 0x00, 0x7c, 0xff, 0xff, 
//  0xff, 0xff, 0xcf, 0x80, 0x01, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xf0, 0x07, 0xcf, 0xff, 0xff, 
//  0xff, 0xff, 0xfc, 0x7c, 0x1f, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xfc, 0xff, 0xff, 0xff, 
//  0xff, 0xff, 0xff, 0xe7, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x8f, 0xff, 0xff, 0xff, 
//  0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
//};





#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {
  Serial.begin(9600);

  Serial.println("Start");

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  //display.display();
  //delay(2000);
  display.clearDisplay();
  display.display();
  delay(2000);

  //display.drawPixel(10, 10, WHITE);
//  display.drawLine(0, 0, display.width() - 1, 0 , WHITE);
//
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 2);
//  display.println("Pickup scheduled!");
//  display.println("");
//  display.println("ETA 11:45 PM");


  //testdrawbitmap(ups_bmp, 64, 72);
  display.drawBitmap(32, 0, myBitmap, 56, 64, WHITE);
  
  


 // display.drawLine(0, 30, display.width() - 1, 30 , WHITE);

  //testdrawline();
  display.display();

  //  display.display(); // show splashscreen
  //  delay(2000);
  //  display.clearDisplay();   // clears the screen and buffer
  //
  //  // draw a single pixel
  //  display.drawPixel(10, 10, WHITE);
  //  display.display();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // draw many lines
  //  testdrawline();
  //  display.display();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // draw rectangles
  //  testdrawrect();
  //  display.display();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // draw multiple rectangles
  //  testfillrect();
  //  display.display();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // draw mulitple circles
  //  testdrawcircle();
  //  display.display();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // draw a white circle, 10 pixel radius
  //  display.fillCircle(display.width()/2, display.height()/2, 10, WHITE);
  //  display.display();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  testdrawroundrect();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  testfillroundrect();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  testdrawtriangle();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  testfilltriangle();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // draw the first ~12 characters in the font
  //  testdrawchar();
  //  display.display();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // draw scrolling text
  //  testscrolltext();
  //  delay(2000);
  //  display.clearDisplay();
  //
  //  // text display tests
  //  display.setTextSize(1);
  //  display.setTextColor(WHITE);
  //  display.setCursor(0,0);
  //  display.println("Hello, world!");
  //  display.setTextColor(BLACK, WHITE); // 'inverted' text
  //  display.println(3.141592);
  //  display.setTextSize(2);
  //  display.setTextColor(WHITE);
  //  display.print("0x"); display.println(0xDEADBEEF, HEX);
  //  display.display();
  //  delay(2000);
  //
  //  // miniature bitmap display
  //  display.clearDisplay();
  //  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  //  display.display();
  //
  //  // invert the display
  //  display.invertDisplay(true);
  //  delay(1000);
  //  display.invertDisplay(false);
  //  delay(1000);
  //
  //  // draw a bitmap icon and 'animate' movement
  //  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}


void loop() {

}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  srandom(666);     // whatever seed

  // initialize
  for (uint8_t f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random() % display.width();
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random() % 5 + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], myBitmap, w, h, WHITE);
    }
    display.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS],  myBitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random() % display.width();
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random() % 5 + 1;
      }
    }
  }
}


void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
}

void testdrawcircle(void) {
  for (int16_t i = 0; i < display.height(); i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, WHITE);
    display.display();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // alternate colors
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, color % 2);
    display.display();
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i = 0; i < min(display.width(), display.height()) / 2; i += 5) {
    display.drawTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, WHITE);
    display.display();
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i = min(display.width(), display.height()) / 2; i > 0; i -= 5) {
    display.fillTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawroundrect(void) {
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, WHITE);
    display.display();
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawrect(void) {
  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, WHITE);
    display.display();
  }
}

void testdrawline() {
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, WHITE);
    display.display();
  }
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(0, 0, display.width() - 1, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, display.height() - 1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = display.width() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, WHITE);
    display.display();
  }
  delay(250);
}

void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.clearDisplay();
  display.println("scroll");
  display.display();

  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}
