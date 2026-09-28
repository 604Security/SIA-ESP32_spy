# petbot

Pet robot project built on the **Seeed XIAO ESP32S3 Sense**, using on-device TinyML
(vision, sound, motion).

## Project

- **Goal:** a pet-aware bot. First a **wake word ("Jasmin")**, then **computer vision** to find the pet.
- **Plan:** follow the mlsysbook.ai XIAO labs, re-themed for petbot. See **[ROADMAP.md](ROADMAP.md)**.
  Phase 0 bring-up → 1 wake word (KWS) → 2 pet image classification → 3 pet object detection (FOMO) → 4 motion (optional)
- **Actuators / outputs:** TBD
- **Power:** TBD (USB-C 5V, or 3.7V LiPo via the XIAO battery pads / IMU board header)

## Hardware

### Seeed XIAO ESP32S3 Sense
- SoC: ESP32-S3R8, dual-core Xtensa LX7 @ 240 MHz
- Memory: 512 KB SRAM, **8 MB OPI PSRAM**, 8 MB flash
- Camera: detachable DVP camera (OV2640 on the original Sense, OV3660 in the XIAOML Kit;
  OV5640 compatible), max 1600×1200
- Microphone: onboard PDM digital mic — CLK **GPIO42**, DATA **GPIO41** (I2S/PDM)
- microSD: FAT32, up to 32 GB, CS **GPIO21**
- Wi-Fi 2.4 GHz + BLE 5.0, U.FL antenna connector
- USB-C for power and programming; LiPo charge circuit; deep sleep ~14 µA
- Size: 21 × 17.5 mm

### XIAOML Kit IMU/OLED expansion board (if used)
- IMU: 6-axis **LSM6DS3TR-C**, I2C addr **0x6A** — library "Seeed Arduino LSM6DS3"
- OLED: 0.42" mono, 72×40 px, SSD1315 (SSD1306-compatible), I2C addr **0x3C** — library "U8g2"
- Uses the XIAO default I2C pins (D4 = SDA, D5 = SCL)
- Reset button, 3.7 V battery connector
- **Remove the camera heat sink before fitting the expansion board.**

## Toolchain

- Arduino IDE 2.x (AppImage in `~/tools/`)
- Board package: **"esp32 by Espressif Systems"**
  - Board: `XIAO_ESP32S3` (FQBN `esp32:esp32:XIAO_ESP32S3`)
  - Tools → PSRAM: **OPI PSRAM** (required for camera + ML)
  - Do **not** use the "Arduino ESP32 Boards" package (`arduino:esp32`) — that is for the Nano ESP32.
- **Version:** esp32 core pinned at **2.0.17** (downgraded from 3.3.12 on 2026-09-28), as the
  Harvard/Rovai labs require for Edge Impulse–exported libraries. Do not update it in Boards Manager.
  Lab sketches written for 3.x (e.g. `XIAOML_Kit_Mic_Test`, which uses `ESP_I2S.h`) won't build on it;
  use the 2.x `I2S.h` API instead.
- CLI: `arduino-cli` 1.5.1 in `~/tools/arduino-cli/`, using the IDE's config
  (`~/.arduinoIDE/arduino-cli.yaml`). The repo `Makefile` wraps it:
  `make build|flash SKETCH=firmware/phase0/blink`, `make monitor`, `make phase0` (build all).
  FQBN with options: `esp32:esp32:XIAO_ESP32S3:PSRAM=opi,UploadSpeed=115200`.
  Run `make` from the repo root.
- Serial port: `/dev/ttyACM0` (native USB). If upload fails, hold BOOT while plugging in.
- Camera pin map: `#define CAMERA_MODEL_XIAO_ESP32S3`
- ML: Edge Impulse Studio (train → export Arduino library), plus local
  `ArduTFLite` / `Chirale_TensorFlowLite` libraries in `~/Arduino/libraries`

### USB / upload troubleshooting (learned 2026-09-28)
- The board does **not** mount as a USB drive. It shows up as a serial port: `lsusb` lists
  `303a:1001 Espressif USB JTAG/serial debug unit`, and `/dev/ttyACM0` appears.
  Check with `watch -n1 'lsusb | grep -i 303a; ls /dev/ttyACM*'`.
- Not in `lsusb` at all → the cable is likely charge-only (use a data cable), or the board is behind the dock/hub.
- **Plug the board directly into the laptop**, not through the ThinkPad USB-C dock.
- **Upload fails with "No serial data received" / "Unable to verify flash chip connection"**
  right after "Stub running..." → esptool's stub loader drops the link. Changing the baud rate
  (921600, 115200, even 9600) does not help, because the port is virtual.
  **Fix: `--no-stub`**, which `make flash` passes by default (`UPLOAD_FLAGS` in the Makefile).
  The Arduino IDE Upload button can't pass it, so always flash with `make flash`.
- Quick link check: `python3 ~/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool.py --chip esp32s3 -p /dev/ttyACM0 --no-stub flash_id`
  (should report 8 MB flash).
- Reading serial non-interactively (e.g. from Claude's shell): `arduino-cli monitor` under `timeout`/pipes
  prints nothing. Use `stty -F /dev/ttyACM0 115200 raw -echo; timeout 5 cat /dev/ttyACM0` instead.
  `make monitor` is fine in an interactive terminal.
- ModemManager is running and can probe ACM ports. It didn't block uploads, but if the port
  acts up: `sudo systemctl stop ModemManager`.

## Reference material

### Harvard "Machine Learning Systems" book (CS249r)
- Free open textbook by Prof. Vijay Janapa Reddi (Harvard) and contributors: https://mlsysbook.ai
- Source: https://github.com/harvard-edge/cs249r_book
- Hardware labs for this board were written with Marcelo Rovai (UNIFEI):
  - Kit overview: https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/xiao_esp32s3.html
  - Setup: https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/setup/setup.html
  - Image classification: https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/image_classification/image_classification.html
  - Object detection: https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/object_detection/object_detection.html
  - Keyword spotting: https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/kws/kws.html
  - Motion classification & anomaly detection: https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/motion_classification/motion_classification.html

### Seeed XIAOML Kit
- The companion hardware kit for the book (XIAO ESP32S3 Sense + IMU/OLED board, antenna,
  32 GB microSD, ~$39): https://www.seeedstudio.com/The-XIAOML-Kit.html

### Marcelo Rovai's lab code (local clone)
- `~/workstation/github/Mjrovai/XIAO-ESP32S3-Sense` (upstream: https://github.com/Mjrovai/XIAO-ESP32S3-Sense)
  - `XIAOML_Kit_code/` — kit sketches: image classification (+OLED), KWS (+OLED),
    motion classification/anomaly detection, IMU/OLED/mic tests
  - `Camera_HTTP_Server_STA/`, `Streeming_Video/` — camera over Wi-Fi
  - `take_photos_command/`, `camera_round_display_save_jpeg/` — dataset capture
  - `Wav_Record_dataset/`, `xiao_esp32s3_microphone*/` — audio capture
  - PDFs of the image classification, object detection, motion, and KWS tutorials

### Seeed docs
- XIAO ESP32S3 getting started: https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/

## Repo conventions

- Firmware sketches live in `firmware/<phase>/<sketch>/<sketch>.ino`.
- Wi-Fi credentials go in a per-sketch `secrets.h` (gitignored); copy it from `secrets.h.example`.
- Installers/tooling live in `~/tools/`, not in this repo (see `.gitignore`).
- Keep the "Status / next steps" section below current at the end of each session.

## Status / next steps

Current phase: **Phase 0 — board bring-up** (see ROADMAP.md)

- [x] Decide goals: wake word first, then vision
- [x] Wake word: "Jasmin"
- [ ] Open decisions (see ROADMAP.md): pet type(s), plain Sense or XIAOML Kit
- [x] Phase 0: esp32 core downgraded to 2.0.17; arduino-cli + Makefile set up
- [x] Phase 0: sketches written and compiling in `firmware/phase0/` (blink, mic_test, sd_test, camera_webserver)
- [x] Phase 0 on hardware: board detected, USB upload fixed (`--no-stub`), `blink` flashed and running
- [x] Phase 0 on hardware: `mic_test` flashed; quiet-room level ~10–25 (RMS, DC removed)
- [x] Phase 0 on hardware: `wifi_test` OK on "home-wifi" (2.4 GHz ch 6), IP 192.168.86.189 (DHCP), RSSI -48 dBm,
  pings to gateway/8.8.8.8 and DNS all OK, laptop → board ping OK
- [ ] Phase 0 on hardware: `sd_test` (needs FAT32 card), `camera_webserver` (fill in `secrets.h`)
- [ ] Phase 1: run KWS lab as written, then record "Jasmin" dataset and train the wake word
