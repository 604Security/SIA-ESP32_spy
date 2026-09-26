# petbot

Pet robot project built on the **Seeed XIAO ESP32S3 Sense**, using on-device TinyML
(vision, sound, motion).

> Project goals, behaviours, actuators, and power setup are still TBD — fill in
> the "Project" section as they are decided.

## Project

- **Goal:** TBD
- **Behaviours:** TBD (e.g. recognise pet via camera, react to sounds, detect motion)
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
- **Version gotcha:** this machine has esp32 **3.3.12** installed. The Harvard/Rovai labs
  recommend **2.0.x** for Edge Impulse–exported libraries. If an Edge Impulse library
  fails to compile, downgrade the board package to 2.0.x.
- Serial port: `/dev/ttyACM0` (native USB). If upload fails, hold BOOT while plugging in.
- Camera pin map: `#define CAMERA_MODEL_XIAO_ESP32S3`
- ML: Edge Impulse Studio (train → export Arduino library), plus local
  `ArduTFLite` / `Chirale_TensorFlowLite` libraries in `~/Arduino/libraries`

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

- Installers/tooling live in `~/tools/`, not in this repo (see `.gitignore`).
- Keep the "Status / next steps" section below current at the end of each session.

## Status / next steps

- [ ] Decide project goal and behaviours (fill in "Project" above)
- [ ] Confirm hardware: plain XIAO ESP32S3 Sense or full XIAOML Kit (IMU/OLED board)?
- [ ] Run the setup lab: blink, camera web server, mic test
- [ ] Pick the first ML capability (e.g. pet image classification) and collect a dataset
