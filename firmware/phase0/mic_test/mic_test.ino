// petbot Phase 0: onboard PDM mic test (esp32 core 2.0.x I2S API).
// Prints one audio level (RMS) every 50 ms. Open Tools > Serial Plotter
// at 115200 and speak or clap: the line should jump.
// Mic pins: CLK GPIO42, DATA GPIO41. Needs Tools > PSRAM = OPI PSRAM.

#include <I2S.h>
#include <math.h>

const int SAMPLE_RATE = 16000;
const int WINDOW = SAMPLE_RATE / 20;  // 50 ms

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  I2S.setAllPins(-1, 42, 41, -1, -1);
  if (!I2S.begin(PDM_MONO_MODE, SAMPLE_RATE, 16)) {
    Serial.println("Failed to initialize I2S!");
    while (1);
  }
}

void loop() {
  double sumSq = 0;
  int n = 0;
  while (n < WINDOW) {
    int sample = I2S.read();
    if (sample == 0 || sample == -1 || sample == 1) continue;  // no data / invalid
    sumSq += (double)sample * sample;
    n++;
  }
  Serial.print("level:");
  Serial.println((int)sqrt(sumSq / n));
}
