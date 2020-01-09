//////////////////////////////////////////////////////////////////
// Change Log
//
// 15 JUN 2017 - Initial Rev. A for HackerBox guide
// 10 JUL 2017 - replaced beep() with post from sconklin
// 12 JUL 2017 - added BitHead graphic to LCD functionality
// 15 JUL 2017 - clean up display text a little
// 15 JUL 2017 - default to muted stated
// 15 JUL 2017 - dislplay mute indicator on upper left of screen
// 15 JUL 2017 - tighten timing loops for more "entertainment"


#include "WiFi.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_NeoPixel.h>

// TFT Display Pins
#define TFT_CS    5
#define TFT_DC    22
#define TFT_MOSI  23
#define TFT_CLK   18
#define TFT_RST   21
#define TFT_MISO  19

// NeoPixel Values
#define PIXELPIN   13 //pin that controls leds
#define NUMPIXELS  6  //nuber of leds
#define pixlux    20  //saturation level for NeoPixels colors

// Audio Buzzer Values
const int buzzerPin = 12;
const int f = 349;
const int gS = 415;
const int a = 440;
const int cH = 523;
const int eH = 659;
const int fH = 698;
const int e6 = 1319;
const int g6 = 1568;
const int a6 = 1760;
const int as6 = 1865;
const int b6 = 1976;
const int c7 = 2093;
const int d7 = 2349;
const int e7 = 2637;
const int f7 = 2794;
const int g7 = 3136;

int muted = false;
char ssid[]="HackerBoxer_SeaPrimate";  //put your handle after the underscore
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);
char hackers_found[13][70];
int next_hacker_found = 0;
unsigned long debounce;
byte mute = 16;
volatile byte state = LOW;

void setup() 
{
  debounce = millis();
  tft.begin();
  tft.setRotation(3); // rotate 3*(pi/2)
  pixels.begin();
  pinMode(buzzerPin, OUTPUT);
  for (int i; i<13; i++)
    hackers_found[i][0] = 0; //empty array of strings
  pinMode(mute, INPUT_PULLUP);
  touchAttachInterrupt(digitalPinToInterrupt(mute), mutebutton, CHANGE);  //threshold 40

}

void loop() 
{
  // display BitHead HackerBoxes Mascot
  BitHead2LCD();
  // cycle some NeoPixel Rainbows
  RainbowLEDcycle(17);
  // scan other SSIDs
  wifiScan2LCD();
  // cycle some NeoPixel Rainbows
  WiFi.softAP(ssid, NULL, 1, 0, 1);
  // Play Mario Theme on Buzzer
  RainbowLEDcycle(17);  
  // start broadcating SSID (AP on)
  if (!muted)
    MarioTheme();
  // chill here for a while
  delay(3000);
  //diplay list of found hackers tagged
  found2LCD();
  // cycle some NeoPixel Rainbows
  RainbowLEDcycle(17);
  // Play Imperial March on Buzzer
  if (!muted)
    ImperialMarch();
  // chill here for a while
  delay(3000);
  // stop broadcating SSID (AP off)  
  WiFi.softAPdisconnect(1);
}

void wifiScan2LCD() 
{
  int netsfound;
  int displaylines=13;
  
  tft.fillScreen(ILI9341_BLACK);
  paint_mute_indicator();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(4);
  tft.println(" Nets Scanned");
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);

  while (displaylines > 0)
  {
    netsfound = WiFi.scanNetworks();
    if (netsfound==0)
    {
      tft.println(". . .");
      displaylines--;
    }
    for (int i = 0; i < netsfound; ++i) 
    {
      if (WiFi.SSID(i).startsWith("HackerBoxer"))
      {
        WiFi.SSID(i).toCharArray(hackers_found[next_hacker_found],70);
        hackers_found[next_hacker_found][25] = 0;  //truncate for display
        next_hacker_found++; 
        if (next_hacker_found == 13)
          next_hacker_found = 0;
      }
      else
      {
        // Print SSID and RSSI for each network found
        tft.print(" [");
        tft.print(WiFi.RSSI(i));
        tft.print("] ");
        tft.print(WiFi.SSID(i).substring(0,17));
        tft.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        delay(50);
        displaylines--;
      }
    }
    delay(700); // Wait before scanning again
  }
}

void found2LCD() 
{
  tft.fillScreen(ILI9341_BLACK);
  paint_mute_indicator();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(4);
  tft.println(" Hacker Tags");
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  for (int i=0; i<13; i++)
  {
    tft.print(" ");
    tft.println(hackers_found[i]+12); //+12 to removed "HackerBoxer_"
  }
}

void RainbowLEDcycle(int cycles)
{
  int i=0;
  while(cycles) 
  { 
    pixels.setPixelColor(i, pixels.Color(pixlux,0,0));
    i = (i==5) ? 0 : (i+1);
    pixels.setPixelColor(i, pixels.Color(pixlux,pixlux,0));
    i = (i==5) ? 0 : (i+1);
    pixels.setPixelColor(i, pixels.Color(0,pixlux,0));
    i = (i==5) ? 0 : (i+1);
    pixels.setPixelColor(i, pixels.Color(0,0,pixlux));
    i = (i==5) ? 0 : (i+1);
    pixels.setPixelColor(i, pixels.Color(pixlux,0,pixlux));
    i = (i==5) ? 0 : (i+1);
    pixels.setPixelColor(i, pixels.Color(pixlux,0,pixlux));
    i = (i==5) ? 0 : (i+1);
    i = (i==5) ? 0 : (i+1);
    pixels.show();
    delay(150);
    cycles--;
  }
}

void beep(int tone, int duration)
{
  int noteDelay = int(500000.0/tone);
  
  for (float i = 0.0; i < float(duration/1000.0); i += (1.0/tone))
  {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(noteDelay);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(noteDelay);
  }
  delay(30);
}

 
void  ImperialMarch()
{
  beep(a, 500);
  beep(a, 500);    
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);  
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
  delay(500);
  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);  
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
}

void  MarioTheme()
{
  beep(e7,150);
  beep(e7,150);
  delay(150);
  beep(e7,150);  
  delay(150);
  beep(c7,150);
  beep(e7,150);
  delay(150);
  beep(g7,150);
  delay(450);
  beep(g6,150);
  delay(450);
  beep(c7,150);
  delay(300);
  beep(g6,150);
  delay(300);
  beep(e6,150);
  delay(300);
  beep(a6,150);
  delay(150);
  beep(b6,150);
  delay(150);
  beep(as6,150);
  beep(a6,150);
  delay(150);
  beep(g6,112);
  beep(e7,112); 
  beep(g7,112);
  beep(a6,150);
  delay(150);
  beep(f7,150);
  beep(g7,150);
  delay(150);
  beep(e7,150);
  delay(150); 
  beep(c7,150);
  beep(d7,150);
  beep(b6,150);
}

void mutebutton()
{
  //if ((millis() - debounce) > 100)
  //if (mute.pressed)
  {
    //debounce = millis();
    //mute.pressed = false;
    if (muted)
    {
      muted = false;
    }
    else
    {
      muted = true;
    }
  paint_mute_indicator();
  }
}

void paint_mute_indicator()
{
  if (muted)
  {
    tft.fillRect(0, 0, 18, 18, ILI9341_RED);
  }
  else
  {
    tft.fillRect(0, 0, 18, 18, ILI9341_GREEN);
  }
}

void BitHead2LCD() //Brute-force TFT bit blit of the HackerBoxes mascot BitHead
{
char *bithead[] = {
"00000000000000000000000000000000000000000000000000001111111111111111111110000000000000000000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000000000000111111111111111111111111111111110000000000000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000000001111111111111111111111111111111111111111000000000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000001111111111111111111111111111111111111111111111100000000000000000000000000000000000000000000",
"00000000000000000000000000000000000001111111111111111111111111111111111111111111111111111100000000000000000000000000000000000000000",
"00000000000000000000000000000000000111111111111111111111111111111111111111111111111111111111100000000000000000000000000000000000000",
"00000000000000000000000000000000011111111111111111111100000000000000000000000111111111111111111000000000000000000000000000000000000",
"00000000000000000000000000000001111111111111111110000000000000000000000000000000011111111111111110000000000000000000000000000000000",
"00000000000000000000000000000111111111111111100011110000011100000001110000001110000011111111111111100000000000000000000000000000000",
"00000000000000000000000000001111111111111111000111111000011110000011111000011110000011111111111111111000000000000000000000000000000",
"00000000000000000000000000111111111111110011000110011100001110000111001100000110000011001111111111111110000000000000000000000000000",
"00000000000000000000000001111111111111100011101110001100000110000110001100000110000111001110111111111111000000000000000000000000000",
"00000000000000000000000011111111111101100011101100001100001110000110001100000110000110000110011111111111110000000000000000000000000",
"00000000000000000000000111111111110001100011101100001100001110000110001100000110000110000110000111111111111000000000000000000000000",
"00000000000000000000001111111111100001100011101110001100000110000110001100000110000111001110000111111111111100000000000000000000000",
"00000000000000000000011111111111100001110011000110011100000110000110001100000110000011001100000111111111111110000000000000000000000",
"00000000000000000000111111111111111000111111000111111000111111100011111000011111100011111100011111111111111111000000000000000000000",
"00000000000000000001111111111111111000011110000011110000111111100001111000011111100001111000011111100111111111100000000000000000000",
"00000000000000000011111111110000000000000000000000000000000000000000000000000000000000000000000000000011111111110000000000000000000",
"00000000000000000111111111100000000000000000000000000000000000000000000000000000000000000000000000000001111111111000000000000000000",
"00000000000000001111111111100001111000001111100001111000000111000000111110000111110000111100000011110000111111111100000000000000000",
"00000000000000001111111111100001111000011101110001111000001111000001110110001110111000111100000111111000011111111110000000000000000",
"00000000000000011111111101110000011000011000110000011000000011000001100011001100011000001100001110011100001111111110000000000000000",
"00000000000000111111111000110000011000011000110000011000000011000001100011001100011000001100001110001100001111111111000000000000000",
"00000000000000111111111000110000011000011000111000011000000011000001100011001100011000001100001100001100001111111111100000000000000",
"00000000000001111111111000110000011000011000110000011000000011000001100011001100011000001100001100001100001111111111100000000000000",
"00000000000001111111111000110000011000011000110000011000000011000001100011001100011000001100001110011100000101111111110000000000000",
"00000000000011111111011001100000111000011101110000011000000011100001110111001110111000001100000110011000001110111111110000000000000",
"00000000000011111110011111100011111110001111100001111110001111111000111110000111110000111111000111111000111111111111111000000000000",
"00000000000111111110000111000001111100000111000000111100000111110000011100000011100000111110000001100000011111011111111000000000000",
"00000000000111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111100000000000",
"00000000001111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111100000000000",
"00000000001111111000111110000111110000011100000011100000011111000011111000011110000001111000001111000001111000001111111110000000000",
"00000000001111110001111111000111111000111100000011100000111111100011111100011110000011111100001111000001111000001111111110000000000",
"00000000011111110001100011001110011000001100000001100000110001100110001100000110000011001110000011000000011000000011111110000000000",
"00000000011111100001100011001100011000001100000001100000110001100110001100000110000111000110000011000000011000000011111111000000000",
"00000000011111100001100011001100011100001100000001100000110001100110001110000110000111000110000011000000011000000011111111000000000",
"00000000111111100001100011001100011100001100000001100000110001100110001100000110000111000110000011000000011000000011111111000000000",
"00000000111111100001100011001100011000001100000001100000110001100110001100000110000111000110000011000000011000000011111111100000000",
"00000000111111100001110111001110011000001100000001110000111011100111011100000110000011001100000111000000011000000011111111100000000",
"00000000111111111000111110000111110000111111000111111100011111000011111000011111100011111100011111110001111110001111111111100000000",
"00000001111111110000011100000011100000111111000011111000001110000001110000011111100000111000001111100001111100001111111111100000000",
"00000001111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111100000000",
"00000001111110000011000000001000000001000000001000000000100000000100000001100000000100000000110000000010000000010000011111110000000",
"00000001111100001111100001111000000111110000111100000111100000111100000011111000011111000011111000011110000001110000001111110000000",
"00000001111100001101110000111000001110111000111100000111100000011100000111011000111011100011011100011110000001110000011111110000000",
"00000011111100011100110000011000001100011000001100000001100000001100000110001100110001100110001100000110000000110000011111110000000",
"00000011111100011000111000011000001100011000001100000001100000001100001110001100110001100110001110000110000000110000011111110000000",
"00000011111100011000111000011000011100011000001100000001100000001100001110001100110001100110001110000110000000110000011111110000000",
"00000011111000011000111000011000001100011000001100000001100000001100001110001100110001100110001110000110000000110000011111111000000",
"00000011111000011100110000011000001100011000001100000001100000001100000110001100110001100110001100000110000000110000011111111000000",
"00000011111100001101110000011100001110111000011100000001110000001110000111011100111011100111011100000111000000111000011111111000000",
"00000011111110001111100001111110000111110001111111000111111000111111000011111000011111000011111000011111100011111110001111111000000",
"00000011111000000011000000000000000011000000000000000000000000000000000001100000001100000000110000000000000000000000000111111000000",
"00000011111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000011111001110000001110000001111000001111000000110000000111000000111000000011000000011100000011100000011100000001100111111000000",
"00000111110011111000011110000011111100011111100001111000001111000001111110001111000001111110000111100000111111000111100011111000000",
"00000111110110011100000110000111001100011001100000111000000011000011100110000011000001100111000001100001110011000001100011111000000",
"00000111111110001100000110000110001110111000110000011000000011000011000110000011000001100011000001100001100011000001100011111000000",
"00000111111110001100000110000110001110110000110000011000000011000011000111000011000011100011000001100001100011100001100011111000000",
"00000111111110001100000110000110001110110000110000011000000011000011000111000011000011100011000001100001100011100001100011111000000",
"00000111111110001100000110000110001110111000110000011000000011000011000111000011000001100011000001100001100011000001100011111000000",
"00000111110110001100000110000111001100011001110000011000000011000011100110000011000001100111000001100001110011000001100011111000000",
"00000111110111111000011111100011111100011111100011111100001111110001111110001111110001111110000111111000111111000111111111111000000",
"00000111110011111000011111100001111000001111000011111110001111110000111100001111110000111100001111111000011110000111111111111000000",
"00000111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111000000",
"00000111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111000000",
"00000111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111110000000",
"00000111111000000010000000000000000111111100000000000000000000000000000000000000000000000000000000000000000000000000001111110000000",
"00000011111100000110000000000001111111111111110000000000000000000000000000000000011111111111111000000000000000100000001111110000000",
"00000011111100001110000000000111111111111111111100000000000000000000000000000001111111111111111111000000000001100000001111110000000",
"00000011111100001110000000011111111111111111111111000000000000000000000000000111111111111111111111110000000001100000011111110000000",
"00000011111100001100000000111111111111111111111111100000000000000000000000001111111111111111111111111100000001110000011111110000000",
"00000011111100011100000001111111111111111111111111110000000000000000000000011111111111111111111111111110000001110000011111100000000",
"00000011111110011100000011111111111111111111111111111000000000000000000000111111111111111111111111111111000001110000011111100000000",
"00000011111110011000000111111111111111111111111111111100000000000000000001111111111111111111111111111111100001110000111111100000000",
"00000011111110011000001111111111111111111111111111111110000000000000000001111111111111111111111111111111110001111000111111100000000",
"00000011111111011000011111111111111111111111111111111110000000000000000001111111111111111111111111111111110001111000111111100000000",
"00000011111111111000011111111111111111111111111111111111000000000000000001111111111111111111111111111111111001111001111111000000000",
"00000001111111111000111111111111111111111111111111111111000000000000000011111111111111111111111111111111111001111001111111000000000",
"00000001111111110000111111111111111111111111111111111111000000000000000011111111111111111111111111111111111101111011111111000000000",
"00000001111111110000111111111111111111111111111111111111000000000000000011111111111111111111111111111111111101111011111110000000000",
"00000001111111110001111111111111111111111111111111111110000000000000000001111111111111111111111111111111111100111111111110000000000",
"00000001111111110001111111111111111111111111111111111110000000000000000001111111111111111111111111111111111100111111111110000000000",
"00000001111111110000111111111111111111111111111111111100000000000000000001111111111111111111111111111111111100111111111100000000000",
"00000000111111100000111111111111111111111111111111111000000000000000000001111111111111111111111111111111111100111111111100000000000",
"00000000111111100000111111111111111111111111111111110000000000000000000001111111111111111111111111111111111100011111111100000000000",
"00000000111111100000111111111111111111111111111111100000000000000000000000111111111111111111111111111111111100011111111000000000000",
"00000000111111100000011111111111111111111111111111100000000000000000000000111111111111111111111111111111111000011111111000000000000",
"00000000111111100000011111111111111111111111111110000000000000000000000000011111111111111111111111111111111000001111110000000000000",
"00000000111111100000001111111111111111111111111100000000000000000000000000001111111111111111111111111111111000001111110000000000000",
"00000000111111100000000111111111111111111111111000000000000000000000000000000111111111111111111111111111110000001111111000000000000",
"00000000111111100000000111111111111111111111110000000000000000000000000000000011111111111111111111111111100000000111111000000000000",
"00000000111111100000000011111111111111111111000000000000000000100000000000000001111111111111111111111111100000000111111100000000000",
"00000000111111100000000001111111111111111100000000000000000100110000000000000000011111111111111111111111000000000011111100000000000",
"00000000111111100000000000111111111111110000000000000000000110110000000000000000000111111111111111111100000000000011111110000000000",
"00000000111111100000000000011111111110000000000000000000001110111000000000000000000001111111111111110000000000000001111110000000000",
"00000000111111100000000000001110000000000000000000000000001110111100000000000000000000000111111110000000000000000001111110000000000",
"00000000111111100000000000000000000000000000000000000000011110111100000000000000000000000000000000000000000000000000111110000000000",
"00000000111111110000000000000000000000000000000000000000011110111110000000000000000000000000000000000000000000000000111110000000000",
"00000000011111110000000000000000000000000000000000000000111110111110000000000000000000000000000000000000000000000001111110000000000",
"00000000011111110000000000000000000000000000000000000000111110111110000000000000000000000000000000000000000000000001111110000000000",
"00000000001111111000000000000000000000000000000000000001111110111111000000000000000000000000000000000000000000000011111110000000000",
"00000000001111111000000000000000000000000000000000000001111111111111000000000000000000000000000000000000000000000011111100000000000",
"00000000000111111100000000000000000000000000000000000011111110111111000000000000000000000000000000000000000000000111111100000000000",
"00000000000111111110000000000000000000000000000000000011111110111111000000000000000000000000000000000000000000001111111000000000000",
"00000000000011111111000000000000000000000000000000000011111110111111000000000000000000000000000000000000000000011111111000000000000",
"00000000000001111111110000000000001100000000000000000011111100111111000000000000000000000000000110000000000000111111110000000000000",
"00000000000000111111111111111111111110000000000000000011111100111111000000000000000000000000011111000000000001111111110000000000000",
"00000000000000011111111111111111111110000000000000000011111100111111000000000000000000000001111111100000000011111111100000000000000",
"00000000000000001111111111111111111111000000000000000011111000111111000000000000000000000011111111111100011111111111000000000000000",
"00000000000000000111111111111111111111000000000000000011110000011111000000000000000000000111111111111111111111111110000000000000000",
"00000000000000000001111111111111111111000000000000000001100000001111000000000000000000000111111111111111111111111100000000000000000",
"00000000000000000000111111111111111111000000000000000000000000000110000000000000000000001111111111111111111111111000000000000000000",
"00000000000000000000001111111111111111000000000000000000000000000000000000000000000000011111110111111111111111110000000000000000000",
"00000000000000000000000011111100011111000000000000000000000000000000000000000000000000011111100000111111111111000000000000000000000",
"00000000000000000000000000000000011111000110000000000000000000000000000000000000000000011111100000000001110000000000000000000000000",
"00000000000000000000000000000000011111101111000000000000000000000000000000000000000000111111100000000000000000000000000000000000000",
"00000000000000000000000000000000011111101111011100000000000000000000000000000011000000111111000000000000000000000000000000000000000",
"00000000000000000000000000000000011111101111111100001000000000100000001000000111000000111111000000000000000000000000000000000000000",
"00000000000000000000000000000000011111101111111100011100000001100000011100000111000000111111000000000000000000000000000000000000000",
"00000000000000000000000000000000011111111111111100011110000001100000011110000111000000111111000000000000000000000000000000000000000",
"00000000000000000000000000000000011111111110111100011110000011100000011110000111000000111111000000000000000000000000000000000000000",
"00000000000000000000000000000000011111111110111100011110000011100000011110000111000000111110000000000000000000000000000000000000000",
"00000000000000000000000000000000011111111100111100011110000011100000011110000111000000111110000000000000000000000000000000000000000",
"00000000000000000000000000000000011111111100111100011110000011100000011110000111000011111110000000000000000000000000000000000000000",
"00000000000000000000000000000000011111111100111000011110000011100000011110000111100111111110000000000000000000000000000000000000000",
"00000000000000000000000000000000001111111100111000011110000011100000011110000111100111111110000000000000000000000000000000000000000",
"00000000000000000000000000000000001111111100111000011110000011100000001110000111101111111110000000000000000000000000000000000000000",
"00000000000000000000000000000000000111111110111000011110000011100000001110000111111111111110000000000000000000000000000000000000000",
"00000000000000000000000000000000000111111111111000001110000011100000001110000011111111111100000000000000000000000000000000000000000",
"00000000000000000000000000000000000011111111111000001110000011100000001110000011111111111100000000000000000000000000000000000000000",
"00000000000000000000000000000000000001111111111110011110000011110000001111000111111111111000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000111111111111111110000011110000011111111111111111111000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000011111111111111110000111111111111111111111111111110000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000000111111111111111111111111111111111111111111111000000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000000011111111111111111111111111111111111111111110000000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000000000011111111111111111111111111111111111111000000000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000000000000001111111111111111111111111111111000000000000000000000000000000000000000000000000000",
"00000000000000000000000000000000000000000000000000000000111110011111111100000000000000000000000000000000000000000000000000000000000"};

const int bithead_width = 131;
const int bithead_height = 147;
    
  tft.fillScreen(ILI9341_BLACK);
  paint_mute_indicator();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(4);
  tft.println(" HackerBoxes");

  for (int y=0; y < bithead_height; y++)
  {
    for (int x=0; x < bithead_width; x++)
    {
      if (bithead[y][x] == '1')
      { 
        tft.drawPixel(x+100, y+55, ILI9341_YELLOW);
      }
    }
  }
}
