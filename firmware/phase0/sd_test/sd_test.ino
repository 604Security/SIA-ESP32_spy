// petbot Phase 0: microSD check. Card must be FAT32 (<= 32 GB).
// Mounts the card (CS = GPIO21), prints type/size, lists the root,
// and writes/reads back a test file.

#include "FS.h"
#include "SD.h"
#include "SPI.h"

const int SD_CS = 21;

void setup() {
  Serial.begin(115200);
  delay(2000);  // give the USB serial time to come up
  Serial.println("petbot sd_test starting");

  if (!SD.begin(SD_CS)) {
    Serial.println("Card mount failed: check the card is inserted and FAT32");
    return;
  }
  uint8_t type = SD.cardType();
  if (type == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.printf("Card type: %s\n",
                type == CARD_MMC ? "MMC" : type == CARD_SD ? "SDSC" : type == CARD_SDHC ? "SDHC" : "UNKNOWN");
  Serial.printf("Card size: %llu MB\n", SD.cardSize() / (1024 * 1024));

  Serial.println("Root directory:");
  File root = SD.open("/");
  for (File f = root.openNextFile(); f; f = root.openNextFile()) {
    Serial.printf("  %s%s  %u bytes\n", f.name(), f.isDirectory() ? "/" : "", f.size());
  }

  File w = SD.open("/petbot_test.txt", FILE_WRITE);
  if (!w) {
    Serial.println("Write failed");
    return;
  }
  w.println("petbot SD OK");
  w.close();

  File r = SD.open("/petbot_test.txt");
  Serial.print("Read back: ");
  while (r.available()) Serial.write(r.read());
  r.close();
  Serial.println("SD test passed");
}

void loop() {
}
