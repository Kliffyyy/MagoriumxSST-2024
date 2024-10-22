// IMPORTS ------------------------------------------------------------------
#include <SPI.h>
#include <Wire.h>
#include "esp_task_wdt.h"

#include "Adafruit_SGP30.h"
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SD.h>


// OBJECTS ------------------------------------------------------------------
const int smallMotor1 = 35;

bool currentMode = 0; // 0 for TVOC mode, 1 for CO2 mode
bool buttonPressed = false;


// SGP30 ------------------------------------------------------------------
// SGP30 sensor objects
Adafruit_SGP30 sgp1;
Adafruit_SGP30 sgp2;
Adafruit_SGP30 sgp3;

unsigned long previousMillis = 0;
const long interval = 1000; // Interval for measurements (in milliseconds)

// Multiplexer address (e.g., for TCA9548A it's usually 0x70)
const int multiplexerAddress = 0x70;

// Arrays to store the last 20 values for all six datasets (TVOC and CO2 from three sensors)
const int maxValues = 20;
int valuesTVOC1[maxValues] = {0};
int valuesCO21[maxValues] = {0};

int valuesTVOC2[maxValues] = {0};
int valuesCO22[maxValues] = {0};

int valuesTVOC3[maxValues] = {0};
int valuesCO23[maxValues] = {0};

int currentIndex = 0; // Index to keep track of the current value position

bool sgp1Initialized = false;
bool sgp2Initialized = false;
bool sgp3Initialized = false;


// DISPLAYS ------------------------------------------------------------------
// Pin definitions for the ST7735
// Display 1
#define TFT1_CS 18
#define TFT1_RST 4
#define TFT1_DC 5
#define TFT1_SCLK 15
#define TFT1_MOSI 2
#define BUTTON_PIN 19  // Button pin definition

#define TFT2_CS 12
#define TFT2_RST 27
#define TFT2_DC 23
#define TFT2_SCLK 13
#define TFT2_MOSI 14

// defining TFT displays
Adafruit_ST7735 tft1 = Adafruit_ST7735(TFT1_CS, TFT1_DC, TFT1_MOSI, TFT1_SCLK, TFT1_RST);
Adafruit_ST7735 tft2 = Adafruit_ST7735(TFT2_CS, TFT2_DC, TFT2_MOSI, TFT2_SCLK, TFT2_RST);