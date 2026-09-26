# petbot roadmap — mlsysbook.ai labs mapped to petbot

The Harvard *Machine Learning Systems* book has four hands-on labs for the XIAO ESP32S3
Sense (XIAOML Kit), written by Marcelo Rovai. Each petbot phase follows one lab. Do the
lab as written first, then repeat it with petbot data.

Lab index: https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/xiao_esp32s3.html
Lab code (local): `~/workstation/github/Mjrovai/XIAO-ESP32S3-Sense`

Workflow for every ML phase: **capture data on the XIAO → train in Edge Impulse Studio →
export Arduino library (INT8) → deploy sketch → add petbot behaviour.**

---

## Phase 0 — Board bring-up
**Lab:** [Setup](https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/setup/setup.html)

- Install "esp32 by Espressif Systems" **2.0.17**, board `XIAO_ESP32S3`, PSRAM = **OPI PSRAM**
- Blink the LED, run the mic test (`XIAOML_Kit_code/XIAOML_Kit_Mic_Test`), and run the camera web server
  (`Camera_HTTP_Server_STA`)
- Format a microSD card as FAT32 (needed for audio dataset capture)

**Done when:** the camera stream shows in a browser and the mic test shows audio levels.

---

## Phase 1 — Wake word  ← first goal
**Lab:** [Keyword Spotting (KWS)](https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/kws/kws.html)

| Lab | petbot version |
|---|---|
| Classes: `yes`, `no`, `noise`, `unknown` | Classes: `jasmin`, `noise`, `unknown` |
| Google Speech Commands, 1,500+ clips/word | Our own recordings plus Speech Commands for `unknown` and `noise` |
| LED lights on "yes" | petbot "wakes up": LED/OLED shows it is listening, then starts Phase 2 vision |

**Steps**
1. Run the lab end-to-end with yes/no to prove the pipeline.
2. **Wake word: "Jasmin"** (JAZ-min). It isn't in Speech Commands, so we record it ourselves.
3. Record data with `Wav_Record_dataset` (10 s WAVs on SD, split into 1 s clips):
   - `jasmin`: aim for **≥300 clips** (500+ is better). Use several speakers, tones, speeds,
     distances (0.5–3 m) and rooms. Record on the XIAO mic itself so the training audio matches the device.
   - `unknown`: Speech Commands words **plus our own recordings of sound-alikes**, e.g.
     "jazz", "jasmine rice", "just in", "has been", "Jason", "chasm", so the model learns
     what *isn't* the wake word.
   - `noise`: household background (TV, fan, kitchen, the pet itself) plus Speech Commands `_background_noise_`.
4. Edge Impulse: 1 s window, 16 kHz, **MFCC**, small 2-layer CNN (lab settings), data augmentation on.
5. Deploy from `xiao_esp32s3_microphone_led` / `xiaoml-kit_kws_oled`.
   Mic pins: CLK GPIO42, DATA GPIO41.
6. Tune: require confidence ≥ ~0.8 on 2 consecutive windows to cut false wakes.

**Gotchas from the lab:** PSRAM must be on or I2S init fails; the Edge Impulse example
targets ESP-EYE, so remap the I2S pins to 42/41; upload WAVs to Edge Impulse instead of using the
data forwarder (audio is too fast for it).

**Done when:** petbot reliably wakes on "Jasmin" and rarely on TV/background talk.

---

## Phase 2 — Vision: "is my pet there?"
**Lab:** [Image Classification](https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/image_classification/image_classification.html)

| Lab | petbot version |
|---|---|
| Classes: `background`, `box`, `wheel` | Classes: `empty`, `pet` (or one class per pet: `dog`, `cat`, …) (plus `person` if useful) |
| ~50 images/class | Start with ~100/class from petbot's real viewpoint, varied lighting |
| MobileNetV2 α=0.35, 96×96 RGB | Same |

- Capture from the XIAO itself (`take_photos_command` or camera web server) so training
  images match the deployed camera/lens/height.
- Reference numbers: ~205 ms inference, about 3.4 FPS on-device, 233 KB RAM.
- **petbot behaviour:** wake word → take a photo → answer "pet present / not present" (OLED/LED/serial).

---

## Phase 3 — Vision: "where is my pet?"
**Lab:** [Object Detection (FOMO)](https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/object_detection/object_detection.html)

| Lab | petbot version |
|---|---|
| Detect oranges and frogs | Detect `pet` (optionally `bowl`, `toy`) |
| FOMO MobileNetV2 0.35, 96×96 grayscale, centroids | Same, and use centroid x for left/center/right |
| ~143 ms, ~7 FPS | Same |

- Label bounding boxes in Edge Impulse (use AI-assisted tracking to speed it up).
- FOMO works best with similar-size, non-overlapping objects and a threshold ≥ 0.8.
- **petbot behaviour:** report the pet's position; later, drive a servo/motor to turn
  toward it (actuator TBD).

---

## Phase 4 (optional) — Motion: "was I picked up / knocked over?"
**Lab:** [Motion Classification & Anomaly Detection](https://mlsysbook.ai/kits/contents/seeed/xiao_esp32s3/motion_classification/motion_classification.html)
Requires the XIAOML Kit IMU board (LSM6DS3TR-C).

| Lab | petbot version |
|---|---|
| Classes: maritime / terrestrial / lift / idle | Classes: `idle`, `picked_up`, `bumped` (pet nudging it), `moving` |
| 50 Hz, 2 s window, spectral features, dense NN | Same |
| K-means anomaly score | Flag "something unusual happened" (tipped over, dropped) |

---

## Later ideas
- **Pet sound detection:** reuse the Phase 1 KWS pipeline with classes such as `bark`, `meow`, `whine`, `noise`.
- **Chain everything:** wake word → vision check → report over Wi-Fi/BLE.

## Open decisions
- [x] Wake word: **"Jasmin"** (custom recordings)
- [ ] Which pet(s)? That sets the Phase 2/3 classes.
- [ ] Plain Sense board or XIAOML Kit with IMU/OLED (affects Phase 4 and OLED output)
