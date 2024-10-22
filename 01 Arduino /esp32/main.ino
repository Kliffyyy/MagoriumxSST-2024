void setup() {
  pinMode(smallMotor1, OUTPUT);

  // serial monitor start
  Serial.begin(115200);
  
  // I2C start
  Wire.begin(21, 22);
  
  // Initialize displays
  tft1.initR(INITR_BLACKTAB);
  tft1.fillScreen(ST7735_BLACK); // Clear the screen with black color
  tft1.setRotation(0); // Rotate the display as needed
  
  tft2.initR(INITR_BLACKTAB);
  tft2.fillScreen(ST7735_BLACK); // Clear the screen with black color
  tft2.setRotation(0); // Rotate the display as needed

  // Initialize the button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor

  // Draw the graph boundary on TFT1
  drawGraph();

  // Define and configure the watchdog timer
  esp_task_wdt_add(NULL);
  
  // Initialize the sensors
  selectMuxChannel(0);
  if (sgp1.begin()) {
      sgp1Initialized = true;
      Serial.println("SGP1 Initialized");
  } else {
      sgp1Initialized = false;
      Serial.println("Failed to initialize SGP1!");
  }

  selectMuxChannel(1);
  if (sgp2.begin()) {
      sgp2Initialized = true;
      Serial.println("SGP2 Initialized");
  } else {
      sgp2Initialized = false;
      Serial.println("Failed to initialize SGP2!");
  }
  selectMuxChannel(5);
  if (sgp3.begin()) {
      sgp3Initialized = true;
      Serial.println("SGP3 Initialized");
  } else {
      sgp3Initialized = false;
      Serial.println("Failed to initialize SGP3!");
  }
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Check for button press
    if (digitalRead(BUTTON_PIN) == LOW && !buttonPressed) {
        buttonPressed = true;
        currentMode = !currentMode; // Switch between TVOC and CO2 modes
        tft1.fillScreen(ST7735_BLACK); // Clear the graph when mode is switched
        drawGraph(); // Redraw the graph with the new mode
    }
    
    // Debounce the button
    if (digitalRead(BUTTON_PIN) == HIGH) {
        buttonPressed = false;
    }
    
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Measure and display data from the sensors
        measureAndDisplay();

        // Watchdog timer reset
        esp_task_wdt_reset();
    }
}