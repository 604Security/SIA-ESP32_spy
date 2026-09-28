// petbot Phase 0: onboard PDM mic test (esp32 core 2.0.x I2S API).
// Prints one audio level (RMS) every 50 ms. Open Tools > Serial Plotter
// at 115200 and speak or clap: the line should jump.
// Mic pins: CLK GPIO42, DATA GPIO41. Needs Tools > PSRAM = OPI PSRAM.
// Reads with esp_i2s::i2s_read (blocking), same as the lab's Wav_Record_dataset.

#include <I2S.h>
#include <math.h>

const int SAMPLE_RATE = 16000;
const int WINDOW = SAMPLE_RATE / 20;  // 50 ms

int16_t buf[WINDOW];

void setup() {
  Serial.begin(115200);
  delay(2000);  // give the USB serial time to come up
  Serial.println("petbot mic_test starting");

  I2S.setAllPins(-1, 42, 41, -1, -1);
  if (!I2S.begin(PDM_MONO_MODE, SAMPLE_RATE, 16)) {
    Serial.println("Failed to initialize I2S!");
    while (1) delay(1000);
  }
  Serial.println("I2S ok");
}

void loop() {
  size_t bytesRead = 0;
  esp_err_t err = esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, buf, sizeof(buf), &bytesRead, portMAX_DELAY);
  int n = bytesRead / sizeof(int16_t);
  if (err != ESP_OK || n == 0) {
    Serial.printf("i2s_read error %d, %d bytes\n", err, (int)bytesRead);
    delay(500);
    return;
  }

  // Subtract the mean: the PDM mic has a DC offset that would otherwise dominate the RMS
  double mean = 0;
  for (int i = 0; i < n; i++) mean += buf[i];
  mean /= n;
  double sumSq = 0;
  for (int i = 0; i < n; i++) sumSq += (buf[i] - mean) * (buf[i] - mean);
  Serial.print("level:");
  Serial.println((int)sqrt(sumSq / n));
}
