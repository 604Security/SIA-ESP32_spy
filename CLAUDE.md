# SIA — Secret Intelligence Agency

Spy game firmware for the **Seeed XIAO ESP32S3 Sense** (XIAOML Kit), forked from **AI petbot**
(github.com/604Security/ai-petbot) on 2026-09-30. Everything below "SIA game" is inherited from petbot
and still applies (hardware, toolchain, gotchas).

## SIA game

- **Name:** always "SIA — Secret Intelligence Agency" in the app and code. Keep wording simple and clear for kids.
- **Agents:** `megaspy` (unicorn theme mode: pink/purple UI, unicorn in spy shades on the OLED) and `spyhunter`
  (classic spy: green/cyan UI, spy in a fedora on the OLED).
- **Firmware:** `firmware/sia_spy/` (`make spy`). One sketch: `sia_spy.ino` (game, gadgets, HTTP API),
  `page.h` (the whole web app), `spy_art.h` (OLED mascots, generated).
- **Look (picked 2026-09-30 from the numbered options pages in `design/`, see `design/README.md`):**
  HQ = Night-Vision Cam, login = fingerprint scanner (hold 1.5 s), OLED idle = code rain with a flashing
  SIA / agent-name badge, mascots = megaspy in a ninja mask (tails flutter), spyhunter in a tuxedo + top hat.
  For new design choices, make a numbered options page of live previews in `design/`; the user likes picking that way.
- **Art:** `python3 spy_art.py` previews; `python3 spy_art.py h > spy_art.h` and `python3 spy_art.py page`
  regenerate the OLED header and the page copy. Mascots come from `outfits.py` (10 outfits per agent),
  built on `unicorn.py` (the petbot unicorn).
- **Storage:** points per agent in Preferences (namespace `sia`, key = agent id, plus `case` counter);
  evidence photos `/sia/case_NNNN_k.jpg` and the log `/sia/log.txt` on the SD card.
  SD shares GPIO21 with the LED, so every SD access goes through `sdOpen()`/`sdClose()` (mutex, LED paused).
- **Time:** NTP with Vancouver time zone (`PST8PDT`), used in case-file timestamps.
- **Tuning (measured 2026-09-30):** room noise median ~40 RMS, clicks spike to ~900. Ghost Walk resets only on
  >250 RMS for 2+ windows (0.1 s). Trap sensitivity 1..10 maps to thresholds 3000..80 (7 = 267; a laptop beep hit 273).
- **Testing tip:** `?agent=<id>` in the URL logs in without the login screen (also handy for headless screenshots:
  use a tall window, 900x1250; at 900x700 headless Chrome paints a dark band over the header).

## Hardware project (inherited from AI petbot)

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
- OLED: U8g2 2.36 installed; constructor `U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE)`.
  The OLED is one-colour (white), so colour art goes on the web page instead.

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
- **Mic + camera together:** start I2S (the mic) *before* `esp_camera_init()`. If I2S starts after the
  camera (even once, then stopped), `esp_camera_fb_get()` times out from then on. See `petbot_test.ino`.
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

- Firmware sketches live in `firmware/<name>/<name>.ino` (`sia_spy`, `petbot_test`, `phase0/*`).
- Wi-Fi credentials go in a per-sketch `secrets.h` (gitignored); copy it from `secrets.h.example`.
- Installers/tooling live in `~/tools/`, not in this repo (see `.gitignore`).
- Keep the "Status / next steps" section below current at the end of each session.

## Status / next steps

**SIA:** v1 working on hardware 2026-09-30: all 6 missions, gadgets, sound trap (3-photo burst), case files,
per-agent OLED mascots. Both agents reset to 0 points after testing. Test photos (cases 1-2) and log lines from
2026-09-30 are on the SD card.

Ideas next: IMU "don't move the case" mission (tilt alarm), Phase 2 pet/person detection to confirm Evidence Hunt
targets, a hidden HQ page for spyhunter (reset points, add missions).

Inherited petbot status: **Phase 1 — wake word** (see ROADMAP.md). Phase 0 done 2026-09-28.

- [x] Decide goals: wake word first, then vision
- [x] Wake word: "Jasmin"
- [x] Hardware: **XIAOML Kit** (I2C scan found OLED 0x3C + IMU 0x6A)
- [ ] Open decision (see ROADMAP.md): pet type(s)
- [x] Phase 0: esp32 core downgraded to 2.0.17; arduino-cli + Makefile set up
- [x] Phase 0: sketches written and compiling in `firmware/phase0/` (blink, mic_test, sd_test, camera_webserver)
- [x] Phase 0 on hardware: board detected, USB upload fixed (`--no-stub`), `blink` flashed and running
- [x] Phase 0 on hardware: `mic_test` flashed; quiet-room level ~10–25 (RMS, DC removed)
- [x] Phase 0 on hardware: `wifi_test` OK on the home 2.4 GHz network (name in `secrets.h`, ch 6), IP 192.168.86.189 (DHCP), RSSI -48 dBm,
  pings to gateway/8.8.8.8 and DNS all OK, laptop → board ping OK
- [x] Phase 0 on hardware: `camera_webserver` running at http://192.168.86.189 (stream on :81/stream,
  still frame at /capture); 320x240 capture verified
- [x] Phase 0 on hardware: `sd_test` passed (32 GB SDHC, FAT32, write/read OK)
- [x] **AI petbot test menu** (`firmware/petbot_test`, `make petbot-test`): kid-friendly web test menu (lights, sounds,
  camera, OLED unicorn + messages, SD, Wi-Fi) at http://192.168.86.189 or http://petbot.local
- [ ] Phase 1: next step: run the KWS lab as written (yes/no) to prove the Edge Impulse → Arduino pipeline, then record "Jasmin" dataset and train the wake word
