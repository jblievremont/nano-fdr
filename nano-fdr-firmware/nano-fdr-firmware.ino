/**
 * This file is part of nano-fdr.
 *
 * nano-fdr is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * nano-fdr is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * nano-fdr. If not, see <https://www.gnu.org/licenses/>.
 */

#include <Adafruit_CircuitPlayground.h>
#include <EEPROM.h>

#define PIXEL_COUNT 10

#define FDR_MODE_RECORD 0
#define FDR_MODE_RETRIEVE 1
uint8_t mode = FDR_MODE_RETRIEVE;

struct TelemetryPoint {
  float accelX;
  float accelY;
  float accelZ;
};
#define SAMPLE_SIZE sizeof(TelemetryPoint)

bool started = false;
uint8_t currentPixel = 0;

void setup(void) {
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(50);

  if (CircuitPlayground.slideSwitch()) {
    mode = FDR_MODE_RECORD;
    setup_record();
  } else {
    mode = FDR_MODE_RETRIEVE;
    setup_retrieve();
  }
}

void setup_record(void) {
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G);
  // TODO Check whether EEPROM[0] has data, allow manual clear before new session
  EEPROM[0] = 0;
}

void setup_retrieve(void) {
  // Start retrieval mode: all pixels green
  setAllPixels(0x00ff00);

  Serial.begin(115200);
  while(!Serial) {
    ;
  }

  uint8_t sampleCount = EEPROM[0];
  uint32_t maxIndex = min(sampleCount * SAMPLE_SIZE, EEPROM.length());
  TelemetryPoint current;

  // Start JSON array
  Serial.println("[");
  for (uint32_t offset = sizeof(uint8_t); offset < maxIndex; offset += SAMPLE_SIZE) {

    // Cycle an "off" pixel during retrieval
    setAllPixels(0x00ff00);
    CircuitPlayground.setPixelColor((offset / SAMPLE_SIZE) % PIXEL_COUNT, 0x000000);

    EEPROM.get(offset, current);

    Serial.print("  {");
    Serial.print("\"x\":");
    Serial.print(current.accelX);
    Serial.print(",\"y\":");
    Serial.print(current.accelY);
    Serial.print(",\"z\":");
    Serial.print(current.accelZ);
    Serial.print("}");

    if (offset + SAMPLE_SIZE < maxIndex) {
      Serial.println(",");
    }
  }
  Serial.println("\n]");

  // Retrieval is over, shutdown all pixels
  setAllPixels(0x000000);
}

void loop(void) {
  if (mode == FDR_MODE_RECORD) {
    uint8_t sampleCount = EEPROM[0];
    if ((sampleCount + 1) * SAMPLE_SIZE > EEPROM.length()) {
      // Memory full: all red
      setAllPixels(0xff0000);
    } else if(! started) {
      // Record standby: all yellow
      setAllPixels(0xffff00);
      started = CircuitPlayground.rightButton();
    } else {
      // Record running: cycle one yellow pixel
      CircuitPlayground.clearPixels();
      CircuitPlayground.setPixelColor((currentPixel ++) % PIXEL_COUNT, 0xffff00);
      TelemetryPoint newSample;
      newSample.accelX = CircuitPlayground.motionX();
      newSample.accelY = CircuitPlayground.motionY();
      newSample.accelZ = CircuitPlayground.motionZ();
      EEPROM.put(sizeof(uint8_t) + sampleCount * sizeof(TelemetryPoint), newSample);
      sampleCount += 1;
      EEPROM[0] = sampleCount;
    }
    delay(60);
  }
}

void setAllPixels(uint32_t color) {
  for (uint8_t pixel = 0; pixel < PIXEL_COUNT; pixel ++) {
    CircuitPlayground.setPixelColor(pixel, color);
  }
}
