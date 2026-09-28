// petbot Phase 0: Wi-Fi check.
// Joins the network in secrets.h (copy from secrets.h.example; 2.4 GHz only),
// prints IP / signal / channel, then pings the router and the internet every 10 s.
// From the laptop you can also run: ping <IP printed here>

#include <WiFi.h>
#include "ping/ping_sock.h"
#include "secrets.h"

const char* INTERNET_HOST = "8.8.8.8";

volatile int pingOk = 0;
volatile uint32_t pingTotalMs = 0;
volatile bool pingDone = false;

static void onPingSuccess(esp_ping_handle_t hdl, void* args) {
  uint32_t ms;
  esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &ms, sizeof(ms));
  pingOk++;
  pingTotalMs += ms;
}

static void onPingEnd(esp_ping_handle_t hdl, void* args) {
  pingDone = true;
}

// Sends `count` ICMP echoes to `ip` and prints received/avg time.
void ping(const char* label, IPAddress ip, int count = 4) {
  esp_ping_config_t cfg = ESP_PING_DEFAULT_CONFIG();
  cfg.target_addr.type = IPADDR_TYPE_V4;
  cfg.target_addr.u_addr.ip4.addr = static_cast<uint32_t>(ip);
  cfg.count = count;
  cfg.timeout_ms = 1000;
  cfg.interval_ms = 200;

  esp_ping_callbacks_t cbs = {};
  cbs.on_ping_success = onPingSuccess;
  cbs.on_ping_end = onPingEnd;

  pingOk = 0;
  pingTotalMs = 0;
  pingDone = false;
  esp_ping_handle_t session;
  if (esp_ping_new_session(&cfg, &cbs, &session) != ESP_OK) {
    Serial.printf("  %-9s %s: could not start ping\n", label, ip.toString().c_str());
    return;
  }
  esp_ping_start(session);
  while (!pingDone) delay(50);
  esp_ping_delete_session(session);

  Serial.printf("  %-9s %-15s %d/%d replies", label, ip.toString().c_str(), pingOk, count);
  if (pingOk) Serial.printf(", avg %lu ms", (unsigned long)(pingTotalMs / pingOk));
  Serial.println(pingOk ? "  OK" : "  FAIL");
}

void setup() {
  Serial.begin(115200);
  delay(2000);  // give the USB serial time to come up
  Serial.printf("\npetbot wifi_test: connecting to \"%s\"", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > 20000) {
      Serial.printf("\nFailed to connect (status %d). Check SSID/password and that it's 2.4 GHz.\n", WiFi.status());
      return;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.printf(" connected in %lu ms\n", millis() - start);
  Serial.printf("  IP       %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("  Gateway  %s\n", WiFi.gatewayIP().toString().c_str());
  Serial.printf("  DNS      %s\n", WiFi.dnsIP().toString().c_str());
  Serial.printf("  RSSI     %d dBm\n", WiFi.RSSI());
  Serial.printf("  Channel  %d\n", WiFi.channel());
  Serial.printf("  MAC      %s\n", WiFi.macAddress().c_str());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    return;
  }
  Serial.printf("[%lus] RSSI %d dBm\n", millis() / 1000, WiFi.RSSI());
  ping("gateway", WiFi.gatewayIP());
  IPAddress internet;
  internet.fromString(INTERNET_HOST);
  ping("internet", internet);
  IPAddress resolved;
  if (WiFi.hostByName("google.com", resolved)) {
    Serial.printf("  DNS       google.com -> %s  OK\n", resolved.toString().c_str());
  } else {
    Serial.println("  DNS       google.com lookup  FAIL");
  }
  delay(10000);
}
