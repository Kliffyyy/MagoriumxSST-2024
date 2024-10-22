// FUNCTIONS ------------------------------------------------------------------

void resetValues() {
  currentIndex = 0;
  memset(valuesTVOC1, 0, sizeof(valuesTVOC1));
  memset(valuesCO21, 0, sizeof(valuesCO21));
  memset(valuesTVOC2, 0, sizeof(valuesTVOC2));
  memset(valuesCO22, 0, sizeof(valuesCO22));
  memset(valuesTVOC3, 0, sizeof(valuesTVOC3));
  memset(valuesCO23, 0, sizeof(valuesCO23));
  tft1.fillScreen(ST7735_BLACK);
}

// mux sgp30
void selectMuxChannel(uint8_t channel) {
  if (channel > 7) return; // Channel limit for 8-channel multiplexers like TCA9548A
  Wire.beginTransmission(multiplexerAddress);
  Wire.write(1 << channel); // Select channel
  Wire.endTransmission();
}

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

// graph and displays
void measureAndDisplay() {
  int valueTVOC1 = 0, valueCO21 = 0;
  int valueTVOC2 = 0, valueCO22 = 0;
  int valueTVOC3 = 0, valueCO23 = 0;

  // Measure SGP1
  selectMuxChannel(0);
  if (sgp1Initialized && sgp1.IAQmeasure()) {
    valueTVOC1 = sgp1.TVOC;
    valueCO21 = sgp1.eCO2;
    valuesTVOC1[currentIndex] = valueTVOC1;
    valuesCO21[currentIndex] = valueCO21;
  } else {
    valueTVOC1 = -1;  // Error indicator
    valueCO21 = -1;
  }

  // Measure SGP2
  selectMuxChannel(1);
  if (sgp2Initialized && sgp2.IAQmeasure()) {
    valueTVOC2 = sgp2.TVOC;
    valueCO22 = sgp2.eCO2;
    valuesTVOC2[currentIndex] = valueTVOC2;
    valuesCO22[currentIndex] = valueCO22;
  } else {
    valueTVOC2 = -1;  // Error indicator
    valueCO22 = -1;
  }

  // Measure SGP3
  selectMuxChannel(5);
  if (sgp3Initialized && sgp3.IAQmeasure()) {
    valueTVOC3 = sgp3.TVOC;
    valueCO23 = sgp3.eCO2;
    valuesTVOC3[currentIndex] = valueTVOC3;
    valuesCO23[currentIndex] = valueCO23;
  } else {
    valueTVOC3 = -1;  // Error indicator
    valueCO23 = -1;
  }

  // Increment index and wrap around if necessary
  currentIndex = (currentIndex + 1) % maxValues;

  // Draw the graph on TFT1 based on the current mode (TVOC or CO2)
  drawGraph();

  // Display values on TFT2
  displayCurrentValues(valueTVOC1, valueCO21, valueTVOC2, valueCO22, valueTVOC3, valueCO23);
}

void drawGraph() {
  int graphHeight = 100;  // Height of the graph area
  int graphWidth = 90;    // Width of the graph area
  int startX = 3;         // X-coordinate starting point
  int startY = 8;         // Y-coordinate starting point

  // Clear screen and redraw the graph boundaries
  tft1.fillRect(startX + 3, startY + 3, graphWidth - 5, graphHeight - 5, ST7735_BLACK);  // Clear the graph area
  tft1.drawRect(startX, startY, graphWidth, graphHeight, ST7735_WHITE);                  // Draw boundary

  // Draw Y-axis labels and tick marks
  drawYAxisLabels(graphHeight, startX, startY, graphWidth);

  // Plot based on the current mode (TVOC or CO2)
  if (currentMode == 0) {  // TVOC Mode (0 to 400 ppb)
    plotGraph(valuesTVOC1, ST7735_RED, startX, startY, graphWidth, graphHeight, 400);
    plotGraph(valuesTVOC2, ST7735_YELLOW, startX, startY, graphWidth, graphHeight, 400);
    plotGraph(valuesTVOC3, ST7735_BLUE, startX, startY, graphWidth, graphHeight, 400);
  } else {  // CO2 Mode (0 to 2000 ppm)
    plotGraph(valuesCO21, ST7735_GREEN, startX, startY, graphWidth, graphHeight, 2000);
    plotGraph(valuesCO22, ST7735_MAGENTA, startX, startY, graphWidth, graphHeight, 2000);
    plotGraph(valuesCO23, ST7735_WHITE, startX, startY, graphWidth, graphHeight, 2000);
  }
}

void drawYAxisLabels(int graphHeight, int startX, int startY, int graphWidth) {
  int numTicks = 5;                            // Number of ticks on Y-axis
  int tickLength = 4;                          // Length of tick marks
  int yLabelSpacing = graphHeight / numTicks;  // Spacing between tick marks

  for (int i = 0; i <= numTicks; i++) {
    int yPos = startY + graphHeight - (i * yLabelSpacing);  // Position of the tick mark

    // Draw tick mark on the right side
    tft1.drawLine(startX + graphWidth, yPos, startX + graphWidth + tickLength, yPos, ST7735_WHITE);  // Draw the tick mark

    // Display labels based on the current mode
    int yValue;
    if (currentMode == 0) {            // TVOC Mode
      yValue = (400 / numTicks) * i;   // Scale TVOC from 0 to 400
    } else {                           // CO2 Mode
      yValue = (2000 / numTicks) * i;  // Scale CO2 from 0 to 2000
    }

    // Print the Y-axis label on the right side
    tft1.setTextColor(ST7735_WHITE);
    tft1.setTextSize(1);
    tft1.setCursor(startX + graphWidth + tickLength + 5, yPos - 4);  // Adjust the cursor position for labels
    tft1.print(yValue);
  }
}

void plotGraph(int values[], uint16_t color, int startX, int startY, int graphWidth, int graphHeight, int yMax) {
  int lastX = startX;
  int lastY = startY + graphHeight - map(values[0], 0, yMax, 0, graphHeight);  // Scale Y-axis

  for (int i = 1; i < maxValues; i++) {
    int x = startX + map(i, 0, maxValues, 0, graphWidth);
    int y = startY + graphHeight - map(values[i], 0, yMax, 0, graphHeight);  // Adjust Y-axis scaling

    tft1.drawLine(lastX, lastY, x, y, color);  // Draw line between points
    lastX = x;
    lastY = y;
  }
}

void displayCurrentValues(int tvoc1, int co21, int tvoc2, int co22, int tvoc3, int co23) {
  tft2.setCursor(10, 30);
  tft2.setTextColor(ST7735_WHITE);
  tft2.setTextSize(1);

  tft2.fillScreen(ST7735_BLACK);  // Clear screen

  // Display TVOC and CO2 values from all three sensors
  tft2.printf("TVOC1: %d ppb\nCO2-1: %d ppm\n", tvoc1, co21);
  tft2.printf("TVOC2: %d ppb\nCO2-2: %d ppm\n", tvoc2, co22);
  tft2.printf("TVOC3: %d ppb\nCO2-3: %d ppm\n", tvoc3, co23);
}


// SD card 