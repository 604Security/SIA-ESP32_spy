// SIA - Secret Intelligence Agency: a spy game for AI petbot.
//
// Agents log in on the web page (http://sia.local or the address printed on serial),
// do missions with spy gadgets, and earn points to rank up.
//
//   Missions: Signal Rookie, Ghost Walk, Code Breaker, Secret Courier, Trap Master, Evidence Hunt
//   Gadgets:  Signal lamp (Morse on the LED), Listening bug (mic), Spy cam, Code machine,
//             Sound trap (camera snaps evidence when it hears a noise), Case files (microSD)
//
// Points live in flash (Preferences), evidence photos and the mission log on the microSD card.
// The OLED shows the logged-in agent: megaspy = unicorn in spy shades, spyhunter = spy in a fedora.
//
// Needs: esp32 core 2.0.17, Tools > PSRAM = OPI PSRAM, library U8g2,
//        secrets.h with WIFI_SSID / WIFI_PASSWORD (copy from secrets.h.example).
// Flash: make spy

#include <WiFi.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include <I2S.h>
#include <SD.h>
#include <SPI.h>
#include <Preferences.h>
#include <U8g2lib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "secrets.h"
#include "page.h"
#include "spy_art.h"

// ---------------------------------------------------------------- pins
const int LED_PIN = 21;  // orange user LED, active LOW. Shared with the SD card CS!
const int SD_CS = 21;
const uint8_t OLED_ADDR = 0x3C;

// XIAO ESP32S3 Sense camera
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  10
#define SIOD_GPIO_NUM  40
#define SIOC_GPIO_NUM  39
#define Y9_GPIO_NUM    48
#define Y8_GPIO_NUM    11
#define Y7_GPIO_NUM    12
#define Y6_GPIO_NUM    14
#define Y5_GPIO_NUM    16
#define Y4_GPIO_NUM    18
#define Y3_GPIO_NUM    17
#define Y2_GPIO_NUM    15
#define VSYNC_GPIO_NUM 38
#define HREF_GPIO_NUM  47
#define PCLK_GPIO_NUM  13

// Vancouver time for the case files (NTP over Wi-Fi)
const char* TIMEZONE = "PST8PDT,M3.2.0,M11.1.0";

// ---------------------------------------------------------------- agents & missions
const int AGENTS = 2;
const char* AGENT_IDS[AGENTS] = {"megaspy", "spyhunter"};

enum MissionId { M_SIGNAL, M_GHOST, M_CODE, M_COURIER, M_TRAP, M_EVIDENCE, MISSIONS };
const char* MISSION_IDS[MISSIONS] = {"signal", "ghost", "code", "courier", "trap", "evidence"};
const char* MISSION_NAMES[MISSIONS] = {"Signal Rookie", "Ghost Walk", "Code Breaker", "Secret Courier", "Trap Master", "Evidence Hunt"};
const int MISSION_POINTS[MISSIONS] = {50, 30, 40, 10, 40, 20};

struct AgentData {
  uint32_t points;
  uint16_t done[MISSIONS];  // how many times each mission was completed
};
AgentData agents[AGENTS];
volatile int activeAgent = 0;

const char* SIGNAL_WORDS[] = {"SPY", "CAT", "DOG", "SUN", "HI", "SOS", "MAP", "KEY", "RUN", "HAT", "BOX", "YES", "EGG", "OWL", "FOX", "ZOO"};
const char* CODE_PHRASES[] = {"MEET AT NOON", "FIND THE KEY", "HIDE THE MAP", "THE CAT NAPS", "SPY AT DAWN", "CODE RED",
                              "GO TO BASE", "EAT A COOKIE", "LOOK UP HIGH", "DANCE PARTY", "UNICORN WINS", "SECRET CAKE"};
const char* EVIDENCE_TARGETS[] = {"something red", "a shoe", "your pet", "a book", "a spoon", "a toy", "something round",
                                  "something that starts with S", "a secret hiding spot", "a plant", "something blue", "a hat"};
#define COUNT(a) (sizeof(a) / sizeof(a[0]))

const unsigned long GHOST_MS = 20000;   // Ghost Walk: stay quiet this long
const int GHOST_LIMIT = 250;            // louder than this (RMS) for 2+ windows (0.1 s) resets the Ghost Walk timer
const unsigned long COURIER_COOLDOWN = 120000;
const unsigned long TRAP_ARM_DELAY = 10000;   // time to hide after arming
const unsigned long TRAP_COOLDOWN = 8000;     // between catches

struct Mission {
  volatile int id = -1;  // MissionId, or -1 when no mission is running
  int agent = 0;
  char secret[16] = "";   // signal word / code phrase
  char cipher[16] = "";
  int shift = 0;
  const char* target = "";
  bool hint = false;
  volatile bool done = false;
  volatile unsigned long quietSince = 0;
  unsigned long startedAt = 0;
} mission;

unsigned long lastCourier[AGENTS] = {0, 0};

// ---------------------------------------------------------------- hardware state
bool cameraOk = false, micOk = false, screenOk = false;
volatile bool sdBusy = false;  // an SD session owns GPIO21 while true
SemaphoreHandle_t sdLock;
Preferences prefs;
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

volatile int soundLevel = 0;  // RMS of the latest 50 ms of audio
volatile int soundPeak = 0;   // loudest since the page last asked

// OLED overlays
char screenMsg[16] = "";
char screenMsgTitle[8] = "";
unsigned long messageUntil = 0;
char rewardText[8] = "";
unsigned long rewardUntil = 0, alertUntil = 0, snapUntil = 0;

// ---------------------------------------------------------------- sound trap
struct TrapEvent {
  uint32_t caseNo;
  char when[20];
  int level;
  int photos;
  int agent;
};
volatile bool trapArmed = false;
volatile bool trapFired = false;
volatile int trapFiredLevel = 0;
int trapAgent = 0, trapSens = 5;
volatile int trapThreshold = 600;
unsigned long trapArmedAt = 0, trapLastFire = 0;
bool trapPointsGiven = false;
TrapEvent trapEvents[5];
int trapEventCount = 0;
uint32_t trapCatches = 0;

// Sensitivity 1 (only loud crashes) .. 10 (hears a whisper) -> RMS threshold 3000 .. 80
int thresholdFor(int sens) {
  sens = constrain(sens, 1, 10);
  return (int)(80.0 * pow(3000.0 / 80.0, (10 - sens) / 9.0));
}

// ---------------------------------------------------------------- small helpers
String jsonEscape(const String& s) {
  String out;
  for (char c : s) {
    if (c == '"' || c == '\\') out += '\\';
    if ((uint8_t)c >= 32) out += c;
  }
  return out;
}

String nowStr() {
  time_t t = time(nullptr);
  if (t < 1700000000) return "boot+" + String(millis() / 1000) + "s";  // clock not set yet
  struct tm tmv;
  localtime_r(&t, &tmv);
  char buf[20];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmv);
  return buf;
}

// Uppercase A-Z only, so "meet at noon!" matches "MEET AT NOON"
String lettersOnly(const String& s) {
  String out;
  for (char c : s) {
    c = toupper(c);
    if (c >= 'A' && c <= 'Z') out += c;
  }
  return out;
}

// Caesar cipher: shift each letter forward, keep everything else
void caesar(const char* in, int shift, char* out, size_t len) {
  size_t i = 0;
  for (; in[i] && i < len - 1; i++) {
    char c = toupper(in[i]);
    out[i] = (c >= 'A' && c <= 'Z') ? 'A' + (c - 'A' + shift + 26) % 26 : c;
  }
  out[i] = 0;
}

int agentIndex(const char* id) {
  for (int i = 0; i < AGENTS; i++)
    if (!strcmp(id, AGENT_IDS[i])) return i;
  return -1;
}

// ---------------------------------------------------------------- microSD (shares GPIO21 with the LED)
bool sdOpen() {
  xSemaphoreTake(sdLock, portMAX_DELAY);
  sdBusy = true;
  delay(2);
  if (!SD.begin(SD_CS)) {
    SD.end();
    pinMode(LED_PIN, OUTPUT);
    sdBusy = false;
    xSemaphoreGive(sdLock);
    return false;
  }
  if (!SD.exists("/sia")) SD.mkdir("/sia");
  return true;
}

void sdClose() {
  SD.end();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  sdBusy = false;
  xSemaphoreGive(sdLock);
}

void logLine(const String& line) {
  Serial.println("[log] " + line);
  if (!sdOpen()) return;
  File f = SD.open("/sia/log.txt", FILE_APPEND);
  if (f) {
    f.println(line);
    f.close();
  }
  sdClose();
}

// ---------------------------------------------------------------- points
void loadAgents() {
  for (int i = 0; i < AGENTS; i++) {
    memset(&agents[i], 0, sizeof(AgentData));
    prefs.getBytes(AGENT_IDS[i], &agents[i], sizeof(AgentData));
  }
}

void saveAgent(int a) {
  prefs.putBytes(AGENT_IDS[a], &agents[a], sizeof(AgentData));
}

void award(int a, int m, int pts, const String& note) {
  agents[a].points += pts;
  agents[a].done[m]++;
  saveAgent(a);
  snprintf(rewardText, sizeof(rewardText), "+%d", pts);
  rewardUntil = millis() + 3000;
  logLine(nowStr() + " | " + AGENT_IDS[a] + " | " + MISSION_NAMES[m] + " | +" + pts + (note.length() ? String(" | ") + note : String()));
}

// ---------------------------------------------------------------- signal lamp (Morse on the LED)
const char* MORSE[36] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..",
                         "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-",
                         "-.--", "--..", "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...",
                         "---..", "----."};

const char* morseFor(char c) {
  c = toupper(c);
  if (c >= 'A' && c <= 'Z') return MORSE[c - 'A'];
  if (c >= '0' && c <= '9') return MORSE[26 + c - '0'];
  return nullptr;
}

struct LampStep {
  uint8_t units;
  bool on;
  int8_t letter;  // index into lampText, for the OLED
  char symbol;    // '.' or '-' while on
};
LampStep lampSteps[200];
volatile int lampCount = 0;
volatile bool lampActive = false;
bool lampRepeat = false;
unsigned lampUnit = 200;
char lampText[24] = "";
volatile int lampStep = 0;

// Dot = 1 unit on, dash = 3, gap inside a letter = 1, between letters = 3, between words = 7.
void lampPlay(const char* text, unsigned unitMs, bool repeat) {
  lampActive = false;
  int n = 0;
  strncpy(lampText, text, sizeof(lampText) - 1);
  lampText[sizeof(lampText) - 1] = 0;
  for (int i = 0; lampText[i] && n < 190; i++) {
    const char* code = morseFor(lampText[i]);
    if (!code) {  // space or unknown: stretch the last gap to a word gap
      if (n > 0 && !lampSteps[n - 1].on) lampSteps[n - 1].units = 7;
      continue;
    }
    for (int k = 0; code[k] && n < 190; k++) {
      lampSteps[n++] = {(uint8_t)(code[k] == '-' ? 3 : 1), true, (int8_t)i, code[k]};
      lampSteps[n++] = {1, false, (int8_t)i, 0};
    }
    lampSteps[n - 1].units = 3;
  }
  if (n == 0) return;
  lampSteps[n - 1].units = 10;  // pause before repeating
  lampCount = n;
  lampUnit = unitMs;
  lampRepeat = repeat;
  lampStep = 0;
  lampActive = true;
}

void lampStop() {
  lampActive = false;
}

void setLed(bool on) {
  digitalWrite(LED_PIN, on ? LOW : HIGH);
}

void updateLed() {
  static unsigned long stepStart = 0;
  static int lastStep = -1;
  if (sdBusy) return;
  if (!lampActive) {
    setLed(false);
    lastStep = -1;
    return;
  }
  unsigned long now = millis();
  if (lampStep != lastStep) {
    lastStep = lampStep;
    stepStart = now;
  }
  const LampStep& s = lampSteps[lampStep];
  setLed(s.on);
  if (now - stepStart >= s.units * lampUnit) {
    if (lampStep + 1 < lampCount) lampStep = lampStep + 1;
    else if (lampRepeat) lampStep = 0;
    else lampActive = false;
  }
}

// ---------------------------------------------------------------- mic
// Start order matters on core 2.0.17: if I2S (the mic) is started after the camera,
// esp_camera_fb_get() times out from then on. setup() starts the mic first.
bool micStart() {
  I2S.setAllPins(-1, 42, 41, -1, -1);
  return I2S.begin(PDM_MONO_MODE, 16000, 16);
}

void micTask(void*) {
  static int16_t buf[800];  // 50 ms at 16 kHz
  int warmup = 10;          // the first ~0.5 s after start is noisy
  int loudRun = 0;          // consecutive loud windows, so a single click doesn't count
  for (;;) {
    size_t bytes = 0;
    if (esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, buf, sizeof(buf), &bytes, pdMS_TO_TICKS(200)) != ESP_OK || bytes == 0) {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }
    int n = bytes / sizeof(int16_t);
    // Subtract the mean: the PDM mic has a DC offset that would otherwise dominate the RMS
    double mean = 0;
    for (int i = 0; i < n; i++) mean += buf[i];
    mean /= n;
    double sumSq = 0;
    for (int i = 0; i < n; i++) sumSq += (buf[i] - mean) * (buf[i] - mean);
    int rms = (int)sqrt(sumSq / n);
    if (warmup > 0) {
      warmup--;
      continue;
    }
    soundLevel = rms;
    if (rms > soundPeak) soundPeak = rms;

    unsigned long now = millis();
    loudRun = rms > GHOST_LIMIT ? loudRun + 1 : 0;
    if (mission.id == M_GHOST && !mission.done && loudRun >= 2) mission.quietSince = now;
    if (trapArmed && !trapFired && now - trapArmedAt > TRAP_ARM_DELAY && now - trapLastFire > TRAP_COOLDOWN &&
        rms > trapThreshold) {
      trapFiredLevel = rms;
      trapFired = true;
    }
  }
}

// ---------------------------------------------------------------- camera
bool initCamera() {
  camera_config_t c = {};
  c.ledc_channel = LEDC_CHANNEL_0;
  c.ledc_timer = LEDC_TIMER_0;
  c.pin_d0 = Y2_GPIO_NUM;
  c.pin_d1 = Y3_GPIO_NUM;
  c.pin_d2 = Y4_GPIO_NUM;
  c.pin_d3 = Y5_GPIO_NUM;
  c.pin_d4 = Y6_GPIO_NUM;
  c.pin_d5 = Y7_GPIO_NUM;
  c.pin_d6 = Y8_GPIO_NUM;
  c.pin_d7 = Y9_GPIO_NUM;
  c.pin_xclk = XCLK_GPIO_NUM;
  c.pin_pclk = PCLK_GPIO_NUM;
  c.pin_vsync = VSYNC_GPIO_NUM;
  c.pin_href = HREF_GPIO_NUM;
  c.pin_sscb_sda = SIOD_GPIO_NUM;
  c.pin_sscb_scl = SIOC_GPIO_NUM;
  c.pin_pwdn = PWDN_GPIO_NUM;
  c.pin_reset = RESET_GPIO_NUM;
  c.xclk_freq_hz = 20000000;
  c.frame_size = FRAMESIZE_VGA;
  c.pixel_format = PIXFORMAT_JPEG;
  c.grab_mode = CAMERA_GRAB_LATEST;
  c.fb_location = CAMERA_FB_IN_PSRAM;
  c.jpeg_quality = 12;
  c.fb_count = 2;
  if (esp_camera_init(&c) != ESP_OK) return false;

  sensor_t* s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {  // OV3660 (XIAOML Kit) starts flipped and over-saturated
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  return true;
}

// Takes `count` photos and saves them as /sia/case_NNNN_k.jpg. Returns the case number (0 = failed).
uint32_t saveEvidence(int count, int* saved) {
  *saved = 0;
  if (!cameraOk) return 0;
  uint32_t caseNo = prefs.getUInt("case", 0) + 1;
  if (!sdOpen()) return 0;
  for (int k = 1; k <= count; k++) {
    if (k > 1) delay(300);
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) continue;
    char path[32];
    snprintf(path, sizeof(path), "/sia/case_%04lu_%d.jpg", (unsigned long)caseNo, k);
    File f = SD.open(path, FILE_WRITE);
    if (f) {
      if (f.write(fb->buf, fb->len) == fb->len) (*saved)++;
      f.close();
    }
    esp_camera_fb_return(fb);
  }
  sdClose();
  if (*saved == 0) return 0;
  prefs.putUInt("case", caseNo);
  snapUntil = millis() + 1500;
  return caseNo;
}

// ---------------------------------------------------------------- sound trap
void handleTrap() {
  if (!trapFired) return;
  trapLastFire = millis();
  alertUntil = millis() + 4000;
  int level = trapFiredLevel;
  int saved = 0;
  uint32_t caseNo = saveEvidence(3, &saved);
  trapCatches++;

  TrapEvent ev = {caseNo, "", level, saved, trapAgent};
  strncpy(ev.when, nowStr().c_str(), sizeof(ev.when) - 1);
  for (int i = COUNT(trapEvents) - 1; i > 0; i--) trapEvents[i] = trapEvents[i - 1];
  trapEvents[0] = ev;
  if (trapEventCount < (int)COUNT(trapEvents)) trapEventCount++;

  String note = "intruder caught (loudness " + String(level) + ")" + (caseNo ? ", case " + String(caseNo) : "");
  if (!trapPointsGiven) {
    trapPointsGiven = true;
    award(trapAgent, M_TRAP, MISSION_POINTS[M_TRAP], note);
  } else {
    logLine(nowStr() + " | " + AGENT_IDS[trapAgent] + " | Sound trap | " + note);
  }
  trapFired = false;
}

// ---------------------------------------------------------------- missions
void stopMission() {
  if (mission.id == M_SIGNAL) lampStop();
  if (mission.id == M_CODE) messageUntil = 0;
  mission.id = -1;
}

// Starts a mission for agent `a`; returns the public part of the briefing as JSON fields.
String startMission(int m, int a) {
  stopMission();
  mission.agent = a;
  mission.hint = false;
  mission.done = false;
  mission.startedAt = millis();
  String j;
  switch (m) {
    case M_SIGNAL: {
      strcpy(mission.secret, SIGNAL_WORDS[random(COUNT(SIGNAL_WORDS))]);
      lampPlay(mission.secret, 300, true);  // slow, so it can be read by eye
      j = ",\"letters\":" + String(strlen(mission.secret));
      break;
    }
    case M_GHOST:
      mission.quietSince = millis();
      j = ",\"seconds\":" + String(GHOST_MS / 1000);
      break;
    case M_CODE: {
      strcpy(mission.secret, CODE_PHRASES[random(COUNT(CODE_PHRASES))]);
      mission.shift = random(1, 6);
      caesar(mission.secret, mission.shift, mission.cipher, sizeof(mission.cipher));
      strcpy(screenMsg, mission.cipher);
      strcpy(screenMsgTitle, "DECODE");
      messageUntil = ULONG_MAX;
      j = ",\"cipher\":\"" + String(mission.cipher) + "\",\"shift\":" + mission.shift;
      break;
    }
    case M_EVIDENCE:
      mission.target = EVIDENCE_TARGETS[random(COUNT(EVIDENCE_TARGETS))];
      j = ",\"target\":\"" + String(mission.target) + "\"";
      break;
    default:
      return "";  // courier and trap don't need starting
  }
  mission.id = m;
  return j;
}

// ---------------------------------------------------------------- OLED
const int PX = 36, PW = 36;  // right-hand panel: x 36..71

void drawCentered(const char* s, int y) {
  int w = u8g2.getStrWidth(s);
  u8g2.drawStr(PX + (PW - w) / 2, y, s);
}

void updateScreen() {
  static unsigned long last = 0;
  unsigned long now = millis();
  if (!screenOk || now - last < 50) return;
  last = now;
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);

  if (now < alertUntil) {  // intruder! flash the whole screen
    bool inv = (now / 250) % 2;
    if (inv) {
      u8g2.drawBox(0, 0, 72, 40);
      u8g2.setDrawColor(0);
    }
    u8g2.setFont(u8g2_font_helvB10_tr);
    u8g2.drawStr((72 - u8g2.getStrWidth("ALERT!")) / 2, 17, "ALERT!");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr((72 - u8g2.getStrWidth("INTRUDER")) / 2, 32, "INTRUDER");
    u8g2.sendBuffer();
    return;
  }

  // mascot of the logged-in agent, with a lens glint sweeping by now and then
  int phase = (now / 150) % 20;
  int frame = phase < 4 ? phase : 0;
  u8g2.drawXBMP(0, 0, SPY_ART_W, SPY_ART_H, activeAgent == 0 ? UNICORN_SPY[frame] : AGENT_SPY[frame]);

  char buf[16];
  if (now < rewardUntil) {
    u8g2.setFont(u8g2_font_helvB10_tr);
    drawCentered(rewardText, 16);
    u8g2.setFont(u8g2_font_5x8_tr);
    drawCentered("points!", 30);
  } else if (now < snapUntil) {
    u8g2.setFont(u8g2_font_helvB10_tr);
    drawCentered("SNAP!", 24);
  } else if (now < messageUntil) {
    u8g2.setFont(u8g2_font_5x8_tr);
    drawCentered(screenMsgTitle, 7);
    u8g2.setFont(u8g2_font_6x10_tr);
    int len = strlen(screenMsg);
    for (int row = 0; row < 2 && row * 6 < len; row++) {
      strncpy(buf, screenMsg + row * 6, 6);
      buf[6] = 0;
      drawCentered(buf, 20 + row * 11);
    }
  } else if (mission.id == M_GHOST && !mission.done) {
    long left = (long)(GHOST_MS - (now - mission.quietSince)) / 1000 + 1;
    u8g2.setFont(u8g2_font_5x8_tr);
    drawCentered("shhh...", 8);
    u8g2.setFont(u8g2_font_helvB10_tr);
    snprintf(buf, sizeof(buf), "%ld", max(left, 0L));
    drawCentered(buf, 28);
  } else if (mission.id == M_EVIDENCE) {
    u8g2.setFont(u8g2_font_6x10_tr);
    drawCentered("SNAP", 16);
    drawCentered("IT!", 28);
  } else if (lampActive) {
    const LampStep& s = lampSteps[lampStep];
    u8g2.setFont(u8g2_font_5x8_tr);
    drawCentered("MORSE", 8);
    if (mission.id != M_SIGNAL) {  // in the mission the letters are the secret!
      u8g2.setFont(u8g2_font_helvB10_tr);
      buf[0] = lampText[s.letter];
      buf[1] = 0;
      drawCentered(buf, 24);
    }
    if (s.on) {
      if (s.symbol == '-') u8g2.drawBox(46, 32, 16, 4);
      else u8g2.drawDisc(54, 34, 2);
    }
  } else if (trapArmed) {
    u8g2.setFont(u8g2_font_5x8_tr);
    drawCentered("TRAP", 8);
    unsigned long armedFor = now - trapArmedAt;
    if (armedFor < TRAP_ARM_DELAY) {
      u8g2.setFont(u8g2_font_helvB10_tr);
      snprintf(buf, sizeof(buf), "%lu", (TRAP_ARM_DELAY - armedFor + 999) / 1000);
      drawCentered(buf, 25);
    } else {
      drawCentered("ARMED", 19);
      // loudness bar, with a tick at the trigger level
      float lvl = log10f(max((int)soundLevel, 10)) / 3.6f, thr = log10f(trapThreshold) / 3.6f;
      u8g2.drawFrame(38, 26, 32, 8);
      u8g2.drawBox(40, 28, (int)(28 * min(lvl, 1.0f)), 4);
      int tx = 40 + (int)(28 * min(thr, 1.0f));
      u8g2.drawVLine(tx, 24, 12);
    }
  } else {
    u8g2.setFont(u8g2_font_helvB10_tr);
    drawCentered("SIA", 13);
    u8g2.setFont(u8g2_font_4x6_tr);
    drawCentered(AGENT_IDS[activeAgent], 23);
    snprintf(buf, sizeof(buf), "%lu pts", (unsigned long)agents[activeAgent].points);
    drawCentered(buf, 33);
  }
  u8g2.sendBuffer();
}

// ---------------------------------------------------------------- web
void urlDecode(char* s) {
  char* o = s;
  for (; *s; s++) {
    if (*s == '+') *o++ = ' ';
    else if (*s == '%' && isxdigit(s[1]) && isxdigit(s[2])) {
      char hex[3] = {s[1], s[2], 0};
      *o++ = (char)strtol(hex, nullptr, 16);
      s += 2;
    } else *o++ = *s;
  }
  *o = 0;
}

bool queryParam(httpd_req_t* req, const char* key, char* out, size_t len) {
  char query[200];
  out[0] = 0;
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) return false;
  if (httpd_query_key_value(query, key, out, len) != ESP_OK) return false;
  urlDecode(out);
  return true;
}

// Reads ?agent=<id>; falls back to the logged-in agent
int queryAgent(httpd_req_t* req) {
  char id[16];
  int a = queryParam(req, "agent", id, sizeof(id)) ? agentIndex(id) : -1;
  return a >= 0 ? a : activeAgent;
}

esp_err_t sendJson(httpd_req_t* req, const String& body) {
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-store");
  return httpd_resp_send(req, body.c_str(), body.length());
}

String agentsJson() {
  String j = "[";
  for (int a = 0; a < AGENTS; a++) {
    if (a) j += ",";
    j += String("{\"id\":\"") + AGENT_IDS[a] + "\",\"points\":" + agents[a].points + ",\"done\":[";
    for (int m = 0; m < MISSIONS; m++) j += (m ? "," : "") + String(agents[a].done[m]);
    j += "]}";
  }
  return j + "]";
}

String trapJson() {
  unsigned long now = millis();
  long arming = trapArmed && now - trapArmedAt < TRAP_ARM_DELAY ? (TRAP_ARM_DELAY - (now - trapArmedAt) + 999) / 1000 : 0;
  String j = String("{\"armed\":") + (trapArmed ? "true" : "false") + ",\"arming\":" + arming + ",\"sens\":" + trapSens +
             ",\"threshold\":" + trapThreshold + ",\"level\":" + soundLevel + ",\"agent\":\"" + AGENT_IDS[trapAgent] +
             "\",\"catches\":" + trapCatches + ",\"events\":[";
  for (int i = 0; i < trapEventCount; i++) {
    const TrapEvent& e = trapEvents[i];
    if (i) j += ",";
    j += String("{\"case\":") + e.caseNo + ",\"when\":\"" + e.when + "\",\"level\":" + e.level + ",\"photos\":" + e.photos +
         ",\"agent\":\"" + AGENT_IDS[e.agent] + "\"}";
  }
  return j + "]}";
}

String missionJson() {
  if (mission.id < 0) return "null";
  String j = String("{\"id\":\"") + MISSION_IDS[mission.id] + "\",\"agent\":\"" + AGENT_IDS[mission.agent] +
             "\",\"done\":" + (mission.done ? "true" : "false");
  if (mission.id == M_GHOST) {
    unsigned long quiet = mission.done ? GHOST_MS : min(millis() - mission.quietSince, GHOST_MS);
    j += ",\"quietMs\":" + String(quiet) + ",\"goalMs\":" + GHOST_MS;
  }
  if (mission.id == M_CODE) j += ",\"cipher\":\"" + String(mission.cipher) + "\",\"shift\":" + mission.shift;
  if (mission.id == M_EVIDENCE) j += ",\"target\":\"" + String(mission.target) + "\"";
  if (mission.id == M_SIGNAL) j += ",\"letters\":" + String(strlen(mission.secret));
  return j + "}";
}

esp_err_t indexHandler(httpd_req_t* req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, PAGE_HTML, HTTPD_RESP_USE_STRLEN);
}

esp_err_t stateHandler(httpd_req_t* req) {
  String j = String("{\"agents\":") + agentsJson() + ",\"active\":\"" + AGENT_IDS[activeAgent] + "\",\"mission\":" +
             missionJson() + ",\"trap\":" + trapJson() + ",\"time\":\"" + nowStr() + "\",\"camera\":" +
             (cameraOk ? "true" : "false") + ",\"mic\":" + (micOk ? "true" : "false") + ",\"screen\":" +
             (screenOk ? "true" : "false") + ",\"ip\":\"" + WiFi.localIP().toString() + "\",\"rssi\":" + WiFi.RSSI() + "}";
  return sendJson(req, j);
}

esp_err_t agentHandler(httpd_req_t* req) {
  char id[16];
  if (queryParam(req, "id", id, sizeof(id)) && agentIndex(id) >= 0) activeAgent = agentIndex(id);
  return sendJson(req, String("{\"active\":\"") + AGENT_IDS[activeAgent] + "\"}");
}

esp_err_t soundHandler(httpd_req_t* req) {
  int peak = soundPeak;
  soundPeak = soundLevel;
  return sendJson(req, String("{\"level\":") + soundLevel + ",\"peak\":" + peak + "}");
}

esp_err_t lampHandler(httpd_req_t* req) {
  char text[24];
  if (queryParam(req, "text", text, sizeof(text)) && text[0]) {
    if (mission.id == M_SIGNAL) stopMission();
    lampPlay(text, 200, false);
  } else {
    lampStop();
  }
  return sendJson(req, String("{\"playing\":") + (lampActive ? "true" : "false") + "}");
}

esp_err_t missionStartHandler(httpd_req_t* req) {
  char id[12];
  queryParam(req, "id", id, sizeof(id));
  int m = -1;
  for (int i = 0; i < MISSIONS; i++)
    if (!strcmp(id, MISSION_IDS[i])) m = i;
  if (m < 0) return sendJson(req, "{\"error\":\"unknown mission\"}");
  String extra = startMission(m, queryAgent(req));
  return sendJson(req, String("{\"id\":\"") + MISSION_IDS[m] + "\"" + extra + "}");
}

esp_err_t missionStateHandler(httpd_req_t* req) {
  return sendJson(req, missionJson());
}

esp_err_t missionStopHandler(httpd_req_t* req) {
  stopMission();
  return sendJson(req, "{\"stopped\":true}");
}

esp_err_t missionHintHandler(httpd_req_t* req) {
  if (mission.id != M_SIGNAL) return sendJson(req, "{\"error\":\"no signal mission\"}");
  mission.hint = true;
  String morse;
  for (int i = 0; mission.secret[i]; i++) morse += String(i ? "  " : "") + morseFor(mission.secret[i]);
  return sendJson(req, "{\"morse\":\"" + morse + "\"}");
}

esp_err_t missionAnswerHandler(httpd_req_t* req) {
  char answer[32];
  queryParam(req, "answer", answer, sizeof(answer));
  if (mission.id != M_SIGNAL && mission.id != M_CODE) return sendJson(req, "{\"error\":\"no mission to answer\"}");
  if (mission.done) return sendJson(req, "{\"correct\":true,\"points\":0}");
  bool correct = lettersOnly(answer) == lettersOnly(mission.secret);
  int pts = 0;
  if (correct) {
    int m = mission.id;
    pts = MISSION_POINTS[m];
    if (m == M_SIGNAL && mission.hint) pts /= 2;
    mission.done = true;
    if (m == M_SIGNAL) lampStop();
    award(mission.agent, m, pts, String("answer ") + mission.secret);
  }
  return sendJson(req, String("{\"correct\":") + (correct ? "true" : "false") + ",\"points\":" + pts +
                           ",\"total\":" + agents[mission.agent].points + "}");
}

esp_err_t courierHandler(httpd_req_t* req) {
  char text[40], shiftStr[4];
  queryParam(req, "text", text, sizeof(text));
  queryParam(req, "shift", shiftStr, sizeof(shiftStr));
  int a = queryAgent(req);
  int shift = constrain(atoi(shiftStr), 0, 25);
  // keep it to what fits the screen: 12 characters
  char clean[13];
  int n = 0;
  for (char* p = text; *p && n < 12; p++)
    if (*p >= 32 && *p < 127) clean[n++] = toupper(*p);
  clean[n] = 0;
  if (!n) return sendJson(req, "{\"error\":\"empty message\"}");
  caesar(clean, shift, screenMsg, sizeof(screenMsg));
  strcpy(screenMsgTitle, "SECRET");
  messageUntil = millis() + 60000;

  int pts = 0;
  unsigned long now = millis();
  if (lastCourier[a] == 0 || now - lastCourier[a] > COURIER_COOLDOWN) {
    lastCourier[a] = now;
    pts = MISSION_POINTS[M_COURIER];
    award(a, M_COURIER, pts, String("sent ") + screenMsg);
  }
  long wait = pts ? 0 : (long)(COURIER_COOLDOWN - (now - lastCourier[a])) / 1000;
  return sendJson(req, String("{\"cipher\":\"") + jsonEscape(screenMsg) + "\",\"points\":" + pts + ",\"wait\":" + wait + "}");
}

esp_err_t trapHandler(httpd_req_t* req) {
  char arm[4], sens[4];
  if (queryParam(req, "sens", sens, sizeof(sens))) {
    trapSens = constrain(atoi(sens), 1, 10);
    trapThreshold = thresholdFor(trapSens);
  }
  if (queryParam(req, "arm", arm, sizeof(arm))) {
    if (arm[0] == '1') {
      trapAgent = queryAgent(req);
      trapArmedAt = millis();
      trapPointsGiven = false;
      trapArmed = true;
    } else {
      trapArmed = false;
    }
  }
  return sendJson(req, trapJson());
}

esp_err_t snapHandler(httpd_req_t* req) {
  int a = queryAgent(req);
  int saved = 0;
  uint32_t caseNo = saveEvidence(1, &saved);
  if (!caseNo) return sendJson(req, "{\"error\":\"Couldn't save the photo. Is the memory card in?\"}");
  int pts = 0;
  if (mission.id == M_EVIDENCE && !mission.done && mission.agent == a) {
    mission.done = true;
    pts = MISSION_POINTS[M_EVIDENCE];
    award(a, M_EVIDENCE, pts, String("photo of ") + mission.target + ", case " + caseNo);
  } else {
    logLine(nowStr() + " | " + AGENT_IDS[a] + " | Spy cam | evidence photo, case " + caseNo);
  }
  char name[24];
  snprintf(name, sizeof(name), "case_%04lu_1.jpg", (unsigned long)caseNo);
  return sendJson(req, String("{\"case\":") + caseNo + ",\"file\":\"" + name + "\",\"points\":" + pts + "}");
}

esp_err_t filesHandler(httpd_req_t* req) {
  if (!sdOpen()) return sendJson(req, "{\"error\":\"No memory card found.\",\"photos\":[],\"log\":[]}");
  // photos, newest first (names sort by case number)
  std::vector<String> photos;
  File dir = SD.open("/sia");
  for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
    String n = f.name();
    if (n.endsWith(".jpg")) photos.push_back(n);
  }
  dir.close();
  std::sort(photos.begin(), photos.end(), [](const String& x, const String& y) { return x > y; });
  // the last ~3 KB of the mission log
  std::vector<String> lines;
  File log = SD.open("/sia/log.txt");
  if (log) {
    if (log.size() > 3000) {
      log.seek(log.size() - 3000);
      log.readStringUntil('\n');  // skip the partial line
    }
    while (log.available()) {
      String l = log.readStringUntil('\n');
      l.trim();
      if (l.length()) lines.push_back(l);
    }
    log.close();
  }
  sdClose();

  String j = "{\"photos\":[";
  for (size_t i = 0; i < photos.size() && i < 60; i++) j += (i ? ",\"" : "\"") + jsonEscape(photos[i]) + "\"";
  j += "],\"log\":[";
  for (int i = (int)lines.size() - 1, k = 0; i >= 0 && k < 40; i--, k++) j += (k ? ",\"" : "\"") + jsonEscape(lines[i]) + "\"";
  return sendJson(req, j + "]}");
}

// Serves /sd?f=case_0001_1.jpg from the card
esp_err_t sdFileHandler(httpd_req_t* req) {
  char name[32];
  queryParam(req, "f", name, sizeof(name));
  if (!name[0] || strchr(name, '/') || strstr(name, "..") || !String(name).endsWith(".jpg")) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  if (!sdOpen()) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  File f = SD.open(String("/sia/") + name);
  if (!f) {
    sdClose();
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Cache-Control", "max-age=86400");
  static uint8_t chunk[4096];
  size_t n;
  esp_err_t res = ESP_OK;
  while (res == ESP_OK && (n = f.read(chunk, sizeof(chunk))) > 0) res = httpd_resp_send_chunk(req, (const char*)chunk, n);
  f.close();
  sdClose();
  if (res == ESP_OK) httpd_resp_send_chunk(req, nullptr, 0);
  return res;
}

esp_err_t resetHandler(httpd_req_t* req) {
  char confirm[8];
  int a = queryAgent(req);
  if (!queryParam(req, "confirm", confirm, sizeof(confirm)) || strcmp(confirm, "yes")) return sendJson(req, "{\"error\":\"confirm=yes needed\"}");
  memset(&agents[a], 0, sizeof(AgentData));
  saveAgent(a);
  logLine(nowStr() + " | " + AGENT_IDS[a] + " | HQ | points reset");
  return sendJson(req, "{\"reset\":true}");
}

esp_err_t captureHandler(httpd_req_t* req) {
  camera_fb_t* fb = cameraOk ? esp_camera_fb_get() : nullptr;
  if (!fb) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "image/jpeg");
  esp_err_t res = httpd_resp_send(req, (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return res;
}

#define PART_BOUNDARY "siaframe"
esp_err_t streamHandler(httpd_req_t* req) {
  if (!cameraOk) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=" PART_BOUNDARY);
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  char part[64];
  for (;;) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) return ESP_FAIL;
    size_t hlen = snprintf(part, sizeof(part), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    esp_err_t res = httpd_resp_send_chunk(req, "\r\n--" PART_BOUNDARY "\r\n", strlen("\r\n--" PART_BOUNDARY "\r\n"));
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, part, hlen);
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    if (res != ESP_OK) return res;  // the page closed the spy cam
  }
}

void startWeb() {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.max_uri_handlers = 24;
  cfg.stack_size = 10240;
  cfg.lru_purge_enable = true;
  httpd_handle_t web = nullptr;
  if (httpd_start(&web, &cfg) == ESP_OK) {
    httpd_uri_t uris[] = {
      {"/", HTTP_GET, indexHandler, nullptr},
      {"/api/state", HTTP_GET, stateHandler, nullptr},
      {"/api/agent", HTTP_GET, agentHandler, nullptr},
      {"/api/sound", HTTP_GET, soundHandler, nullptr},
      {"/api/lamp", HTTP_GET, lampHandler, nullptr},
      {"/api/mission/start", HTTP_GET, missionStartHandler, nullptr},
      {"/api/mission/state", HTTP_GET, missionStateHandler, nullptr},
      {"/api/mission/stop", HTTP_GET, missionStopHandler, nullptr},
      {"/api/mission/hint", HTTP_GET, missionHintHandler, nullptr},
      {"/api/mission/answer", HTTP_GET, missionAnswerHandler, nullptr},
      {"/api/courier", HTTP_GET, courierHandler, nullptr},
      {"/api/trap", HTTP_GET, trapHandler, nullptr},
      {"/api/snap", HTTP_GET, snapHandler, nullptr},
      {"/api/files", HTTP_GET, filesHandler, nullptr},
      {"/api/reset", HTTP_GET, resetHandler, nullptr},
      {"/sd", HTTP_GET, sdFileHandler, nullptr},
      {"/capture", HTTP_GET, captureHandler, nullptr},
    };
    for (auto& u : uris) httpd_register_uri_handler(web, &u);
  }

  // The spy cam stream gets its own server so it doesn't block the game
  cfg.server_port = 81;
  cfg.ctrl_port = 32769;
  httpd_handle_t stream = nullptr;
  if (httpd_start(&stream, &cfg) == ESP_OK) {
    httpd_uri_t s = {"/stream", HTTP_GET, streamHandler, nullptr};
    httpd_register_uri_handler(stream, &s);
  }
}

// ---------------------------------------------------------------- serial menu
const char BANNER[] = R"ART(
   ____ ___    _
  / ___|_ _|  / \        *  TOP SECRET  *
  \___ \| |  / _ \
   ___) | | / ___ \      Secret Intelligence Agency
  |____/___/_/   \_\     HQ for AI petbot
)ART";

void printMenu() {
  Serial.println(BANNER);
  Serial.printf("  camera %s   mic %s   screen %s\n", cameraOk ? "OK" : "--", micOk ? "OK" : "--", screenOk ? "OK" : "--");
  for (int a = 0; a < AGENTS; a++) Serial.printf("  agent %-10s %lu points\n", AGENT_IDS[a], (unsigned long)agents[a].points);
  Serial.println();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("  Wi-Fi not connected (\"%s\"). Check secrets.h, and that it's a 2.4 GHz network.\n", WIFI_SSID);
    return;
  }
  String base = "http://" + WiFi.localIP().toString();
  Serial.println("  SIA HQ is online. Agents, report for duty:\n");
  Serial.printf("    HQ          %s/\n", base.c_str());
  Serial.printf("    Missions    %s/#missions\n", base.c_str());
  Serial.printf("    Gadgets     %s/#gadgets\n", base.c_str());
  Serial.printf("    Sound trap  %s/#trap\n", base.c_str());
  Serial.printf("    Case files  %s/#files\n", base.c_str());
  Serial.println("\n    (http://sia.local/ works too on most computers)");
  Serial.println("  Press Enter here to show this menu again.\n");
}

// ---------------------------------------------------------------- main
void setup() {
  Serial.begin(115200);
  delay(2000);  // give the USB serial time to come up
  Serial.println("SIA HQ starting...");

  pinMode(LED_PIN, OUTPUT);
  setLed(false);
  sdLock = xSemaphoreCreateMutex();
  randomSeed(esp_random());

  prefs.begin("sia", false);
  loadAgents();

  Wire.begin();
  Wire.beginTransmission(OLED_ADDR);
  screenOk = Wire.endTransmission() == 0;
  if (screenOk) {
    u8g2.begin();
    u8g2.setBusClock(400000);
    updateScreen();
  }

  micOk = micStart();  // must come before the camera, see micStart()
  cameraOk = initCamera();
  if (micOk) xTaskCreatePinnedToCore(micTask, "mic", 4096, nullptr, 1, nullptr, 1);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname("sia");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(250);
    updateScreen();
  }
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.setSleep(false);  // snappier pages and spy cam
    configTzTime(TIMEZONE, "pool.ntp.org", "time.nist.gov");
    MDNS.begin("sia");
    MDNS.addService("http", "tcp", 80);
    startWeb();
  }
  printMenu();
}

void loop() {
  updateLed();
  updateScreen();
  handleTrap();
  if (mission.id == M_GHOST && !mission.done && millis() - mission.quietSince >= GHOST_MS) {
    mission.done = true;
    award(mission.agent, M_GHOST, MISSION_POINTS[M_GHOST], "20 seconds of silence");
  }
  if (Serial.available()) {
    while (Serial.available()) Serial.read();
    printMenu();
  }
  delay(5);
}
