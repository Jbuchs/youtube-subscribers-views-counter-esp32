/*
* Name: Youtube subscribers and views counter
* Description: Simple Youtube subscribers and views counter with display and transitions
* Author: BARJO
* Version: 1.0
*/

// Display
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
// Wifi
#include <WiFiManager.h>
// Youtube
#include <WiFiClientSecure.h>
#include <YoutubeApi.h>
// Timer
#include <RBD_Timer.h>


// Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wifi
WiFiClientSecure client;

// Youtube Api
#define API_KEY "yourYoutubeApiKey" // <-- change this
#define CHANNEL_ID "yourChannelId" // <-- change this
YoutubeApi api(API_KEY, client);
bool apiError = false;
bool firstTime = true;

// Timer
RBD::Timer timer;

// Icon
// 'yt', 32x32px
const unsigned char icon_yt [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 
	0x7f, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xef, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xff, 0xe0, 0x7f, 0xff, 
	0xff, 0xe0, 0x1f, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xe0, 0x00, 0xff, 
	0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x1f, 0xff, 
	0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe, 
	0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 144)
const int icon_allArray_LEN = 1;
const unsigned char* icon_allArray[1] = {
	icon_yt
};

// Slider
int currentSlideNumber = 1;
int totalSlideNumber = 2;

// Counters
float subscribersCount = 0;
String sCount = "";
float viewsCount = 0;
String vCount = "";
bool viewsBiggerThan1K = false;
bool subscribersBiggerThan1K = false;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, OLED);

  // Wifi manager
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("NetworkSSID","networkPassword"); // <-- change this

  if(!res) {
    Serial.println("Failed to connect");
    // clear display
    display.clearDisplay();

    // show error on screen
    display.setTextSize(1); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Error");
    display.println("Wifi connection error");

    // show on display
    display.display();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("Successfully connected to the Wifi");
  }

  client.setInsecure();

  // Timer for time between API requests
  timer.setTimeout(60000);
  timer.restart();
}

void loop() {
  
  // Get Youtube data first time, and every minute
  if(timer.onRestart() || firstTime) {
    if (api.getChannelStatistics(CHANNEL_ID)) {
      // subscribers and views
      subscribersCount = api.channelStats.subscriberCount;
      viewsCount = api.channelStats.viewCount;

      // if subscribers count bigger than 1K
      if (subscribersCount >= 1000) {
        subscribersCount = subscribersCount / 1000;
        subscribersBiggerThan1K = true;
        sCount = String(subscribersCount, 2) + "K";
      }
      // if views count bigger than 1K
      if (viewsCount >= 1000) {
        viewsCount = viewsCount / 1000;
        viewsBiggerThan1K = true;
        vCount = String(viewsCount, 2) + "K";
      }
      firstTime = false;
    } else {
      apiError = true;
    }
  }

  // Show data on display (slider) if no error with APIc:\Users\joelb\OneDrive\Bureau\Karac - Dossiers en cours\acces.kdbx
  if (!apiError) {
    // clear display
    display.clearDisplay();
    switch (currentSlideNumber) {
      case 1:
        // icon
        display.drawBitmap(0, 2, icon_yt, 32, 32, WHITE);
        // subscribers
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(38, 6);
        if (subscribersBiggerThan1K) {
          display.println(sCount);
        } else {
          display.println(subscribersCount, 0);
        }
        // legend
        display.setTextSize(1); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(38, 24);
        display.println("SUBS");

        // show on display
        display.display();
        delay(2000);

        // transition (optional)
        for (int j=0 ; j<SCREEN_WIDTH+1 ; j++) {
          // clear display
          display.clearDisplay();
          // icon
          display.drawBitmap(0+j, 2, icon_yt, 32, 32, WHITE);
          // subscribers
          display.setTextSize(2); // Draw 2X-scale text
          display.setTextColor(SSD1306_WHITE);
          display.setTextWrap(false);
          display.setCursor(38+j, 6);
          if (subscribersBiggerThan1K) {
            display.println(sCount);
          } else {
            display.println(subscribersCount, 0);
          }
          // legend
          display.setTextSize(1); // Draw 2X-scale text
          display.setTextColor(SSD1306_WHITE);
          display.setTextWrap(false);
          display.setCursor(38+j, 24);
          display.println("SUBS");

          // show on display
          display.display();
        }
        break;

      case 2:
        // icon
        display.drawBitmap(0, 2, icon_yt, 32, 32, WHITE);
        // subscribers
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setTextWrap(false);
        display.setCursor(38, 6);
        if (viewsBiggerThan1K) {
          display.println(vCount);
        } else {
          display.println(viewsCount, 0);
        }
        // legend
        display.setTextSize(1); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(38, 24);
        display.println("VIEWS");

        // show on display
        display.display();
        delay(2000);

        // transition (optional)
        for (int j=0 ; j<SCREEN_WIDTH+1 ; j++) {
          // clear display
          display.clearDisplay();
          // icon
          display.drawBitmap(0+j, 2, icon_yt, 32, 32, WHITE);
          // subscribers
          display.setTextSize(2); // Draw 2X-scale text
          display.setTextColor(SSD1306_WHITE);
          display.setTextWrap(false);
          display.setCursor(38+j, 6);
          if (viewsBiggerThan1K) {
            display.println(vCount);
          } else {
            display.println(viewsCount, 0);
          }
          // legend
          display.setTextSize(1); // Draw 2X-scale text
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(38+j, 24);
          display.println("VIEWS");

          // show on display
          display.display();
        }
        break;
    }

    // increase slide number
    if (currentSlideNumber < totalSlideNumber) {
      currentSlideNumber++;
    } else {
      currentSlideNumber = 1;
    }
  } else {
    // clear display
    display.clearDisplay();

    // show error on screen
    display.setTextSize(1); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Youtube API Error");

    // show on display
    display.display();
  }
  

}
