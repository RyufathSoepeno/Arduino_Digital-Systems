/*
  Advanced Weather Forecast Node for ESP32
  Features:
   - Reads BME280 (temp, hum, pressure)
   - Fetches weather + forecast from OpenWeather (One Call / Current)
   - Displays status on SSD1306 OLED
   - Publishes metrics/forecast to MQTT
   - Caches last successful forecast to LittleFS
   - OTA updates support (basic)
   - Backoff, retry, power-friendly scheduling (deep sleep option)
  
  Required libraries:
   - WiFi.h
   - HTTPClient.h
   - ArduinoJson
   - Adafruit_BME280
   - Adafruit_Sensor
   - Adafruit_SSD1306
   - PubSubClient
   - LittleFS (or SPIFFS)
   - Update.h / ArduinoOTA (optional)
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>
#include <LittleFS.h>
#include "SPIFFS.h"   // fallback if you prefer SPIFFS
#include <ArduinoOTA.h>

// ----------------- USER CONFIG -----------------------
#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASS      "YOUR_WIFI_PASSWORD"

#define OPENWEATHER_APIKEY "YOUR_OPENWEATHER_API_KEY"
// Use lat/lon for location (preferred for OneCall)
#define LOCATION_LAT  -6.200000   // Example: Jakarta latitude
#define LOCATION_LON  106.816666  // Example: Jakarta longitude

// MQTT Settings (change to your broker)
#define MQTT_SERVER "mqtt.example.com"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt_user"
#define MQTT_PASS "mqtt_pass"
#define MQTT_TOPIC_METRICS "home/weather_node/metrics"
#define MQTT_TOPIC_FORECAST "home/weather_node/forecast"

// Polling & sleep intervals
const uint32_t FETCH_INTERVAL_SECONDS = 15 * 60; // 15 minutes between online fetches
const uint32_t SENSOR_INTERVAL_SECONDS = 60;     // measure local sensor every minute
const uint8_t MAX_HTTP_RETRIES = 3;

// Pins / devices
#define BME_SDA_PIN 21
#define BME_SCL_PIN 22

// OLED settings (128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_SDA BME_SDA_PIN
#define I2C_SCL BME_SCL_PIN

// LittleFS cache file
const char *CACHE_FILE = "/weather_cache.json";

// -------------- Globals -------------------------------
Adafruit_BME280 bme; // BME280 object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

uint32_t lastFetchTime = 0;
uint32_t lastSensorTime = 0;

// Simple exponential backoff state
uint8_t httpFailCount = 0;
uint32_t nextAllowedFetchAt = 0; // epoch seconds

// Utility: convert millis() to epoch seconds using system time (requires NTP if you want true epoch)
uint32_t nowEpoch() {
  // If system time not synced, fallback to millis based rough time.
  time_t t = time(NULL);
  if (t < 1000000000) { // not synced
    return millis() / 1000;
  }
  return (uint32_t)t;
}

// ---------------- Functions ---------------------------

// Connect to WiFi
void wifiConnect() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(200);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed.");
  }
}

// Initialize LittleFS for caching
bool initFS() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed, trying SPIFFS...");
    if (!SPIFFS.begin()) {
      Serial.println("SPIFFS mount failed too.");
      return false;
    }
  }
  return true;
}

// Save JSON string to cache file
void writeCache(const String &json) {
  File f = LittleFS.open(CACHE_FILE, "w");
  if (!f) {
    Serial.println("Failed to open cache for writing");
    return;
  }
  f.print(json);
  f.close();
}

// Read cached JSON (if exists). Returns empty string if not found.
String readCache() {
  if (!LittleFS.exists(CACHE_FILE)) return "";
  File f = LittleFS.open(CACHE_FILE, "r");
  if (!f) return "";
  String s = f.readString();
  f.close();
  return s;
}

// Format JSON docs to string with safety
String jsonToString(DynamicJsonDocument &doc) {
  String out;
  serializeJson(doc, out);
  return out;
}

// Publish metrics to MQTT
void publishMetrics(const String &payload, const char *topic) {
  if (mqttClient.connected()) {
    bool ok = mqttClient.publish(topic, payload.c_str());
    Serial.print("MQTT publish to "); Serial.print(topic); Serial.print(" -> ");
    Serial.println(ok ? "OK" : "Failed");
  } else {
    Serial.println("MQTT not connected, cannot publish.");
  }
}

// Setup MQTT connection (simple)
void mqttConnect() {
  if (mqttClient.connected()) return;
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  Serial.print("Connecting to MQTT...");
  if (mqttClient.connect("weather_node_esp32", MQTT_USER, MQTT_PASS)) {
    Serial.println("connected");
    // Subscribe to OTA or command topics if desired
    // mqttClient.subscribe("home/weather_node/cmd");
  } else {
    Serial.print("failed, rc=");
    Serial.println(mqttClient.state());
  }
}

// Parse and display cached JSON (fallback)
void showCachedForecast() {
  String cached = readCache();
  if (cached.length() == 0) {
    Serial.println("No cached forecast");
    return;
  }
  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, cached);
  if (err) {
    Serial.println("Failed to parse cached JSON");
    return;
  }
  // Minimal display of cached data
  const char* summary = doc["current"]["weather"][0]["description"] | "N/A";
  float temp = doc["current"]["temp"] | 0.0;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Cached Forecast");
  display.println(summary);
  display.print("T: "); display.print(temp); display.println(" C");
  display.display();
}

// Fetch weather from OpenWeather One Call (v2.5/3 compatible)
// Note: One Call requires lat/lon and may require paid subscription for some features.
// We fetch current + daily summary.
bool fetchWeatherFromAPI(String &outResponse) {
  if (nowEpoch() < nextAllowedFetchAt) {
    Serial.println("Fetch blocked by backoff; skipping");
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFi - cannot fetch");
    return false;
  }

  HTTPClient http;
  String endpoint = String("http://api.openweathermap.org/data/2.5/onecall?lat=")
                    + String(LOCATION_LAT, 6)
                    + "&lon=" + String(LOCATION_LON, 6)
                    + "&units=metric&exclude=minutely&appid=" + OPENWEATHER_APIKEY;
  Serial.print("Requesting: "); Serial.println(endpoint);

  http.begin(endpoint);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.print("HTTP code: "); Serial.println(httpCode);
    if (httpCode == HTTP_CODE_OK) {
      outResponse = http.getString();
      http.end();
      // success => reset fail count/backoff
      httpFailCount = 0;
      nextAllowedFetchAt = 0;
      return true;
    } else {
      Serial.print("Unexpected HTTP response: ");
      Serial.println(httpCode);
    }
  } else {
    Serial.print("HTTP failed: ");
    Serial.println(http.errorToString(httpCode));
  }
  http.end();

  // Backoff logic
  httpFailCount = min((uint8_t)httpFailCount + 1, (uint8_t)10);
  uint32_t backoffSeconds = (1 << min(httpFailCount, (uint8_t)6)) * 30; // exponential up to some limit
  nextAllowedFetchAt = nowEpoch() + backoffSeconds;
  Serial.print("Setting backoff, nextAllowedFetchAt in seconds: "); Serial.println(backoffSeconds);
  return false;
}

// Render the display with local + remote summary
void renderDisplay(float t, float h, float p, DynamicJsonDocument *remoteDoc) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Weather Node");
  display.print("Local: "); display.print(t); display.print(" C ");
  display.print(h); display.println(" %");
  display.print("P: "); display.print(p/100.0F); display.println(" hPa");

  if (remoteDoc) {
    // show remote summary (current + next day)
    const char* desc = (*remoteDoc)["current"]["weather"][0]["description"] | "N/A";
    float rt = (*remoteDoc)["current"]["temp"] | 0.0;
    display.println();
    display.print("Now: "); display.print(rt); display.print(" C ");
    display.println(desc);

    // daily high/low if available
    if ((*remoteDoc).containsKey("daily")) {
      float dmax = (*remoteDoc)["daily"][0]["temp"]["max"] | 0.0;
      float dmin = (*remoteDoc)["daily"][0]["temp"]["min"] | 0.0;
      display.print("H:"); display.print(dmax); display.print(" L:"); display.print(dmin);
      display.println(" C");
    }
  } else {
    display.println();
    display.println("Remote: (none)");
  }

  display.display();
}

// Process fetched JSON (publish, cache, display)
void processFetchedWeather(const String &jsonStr) {
  // Parse JSON
  // Use a sufficiently large buffer; OneCall returns a lot — adjust if you add more features.
  StaticJsonDocument<8192> doc;
  DeserializationError err = deserializeJson(doc, jsonStr);
  if (err) {
    Serial.print("JSON parse error: ");
    Serial.println(err.c_str());
    return;
  }

  // Cache raw JSON for offline fallback
  writeCache(jsonStr);

  // Extract key values
  float remoteTemp = doc["current"]["temp"] | NAN;
  const char* remoteDesc = doc["current"]["weather"][0]["description"] | "unknown";

  // Create a smaller JSON to publish via MQTT (compact)
  StaticJsonDocument<512> pub;
  pub["remote_temp"] = remoteTemp;
  pub["remote_desc"] = remoteDesc;
  pub["timestamp"] = nowEpoch();

  String pubStr;
  serializeJson(pub, pubStr);
  publishMetrics(pubStr, MQTT_TOPIC_FORECAST);

  // Also display using local sensor values
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure();

  // Render
  // Build a copy dynamic doc pointer for rendering
  DynamicJsonDocument *dynDoc = new DynamicJsonDocument(8192);
  deserializeJson(*dynDoc, jsonStr);
  renderDisplay(t, h, p, dynDoc);
  delete dynDoc;
}

// Setup OTA (basic)
void setupOTA() {
  // Optional: set a password or configure hostname
  ArduinoOTA.setHostname("esp32-weather-node");
  // ArduinoOTA.setPassword("admin"); // set if desired

  ArduinoOTA.onStart([]() {
    Serial.println("OTA start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA end");
  });
  ArduinoOTA.onError([](ota_error_t err) {
    Serial.printf("OTA Error[%u]: ", err);
  });

  ArduinoOTA.begin();
  Serial.println("OTA ready");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Advanced Weather Forecast Node starting...");

  // Initialize I2C, display, filesystem, sensor
  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
  }

  if (!initFS()) {
    Serial.println("Filesystem failed to start - caching disabled");
  }

  if (!bme.begin(0x76)) { // BME280 common I2C address 0x76 or 0x77
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  } else {
    Serial.println("BME280 found");
  }

  // WiFi connect (and basic time sync for epoch)
  wifiConnect();

  // NTP time sync (optional but useful for accurate timestamps and scheduling)
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for time sync...");
  uint32_t t0 = millis();
  while (time(NULL) < 1000000000 && millis() - t0 < 10000) {
    delay(200);
  }

  // MQTT connect
  mqttConnect();

  // OTA
  setupOTA();

  // show cached if no network fetch immediately
  String cached = readCache();
  if (cached.length() > 0) {
    DynamicJsonDocument dd(4096);
    if (!deserializeJson(dd, cached)) {
      renderDisplay(bme.readTemperature(), bme.readHumidity(), bme.readPressure(), &dd);
    }
  }

  // initial times
  lastFetchTime = nowEpoch() - FETCH_INTERVAL_SECONDS; // force immediate fetch in loop
  lastSensorTime = nowEpoch() - SENSOR_INTERVAL_SECONDS;
}

// Main loop
void loop() {
  ArduinoOTA.handle(); // handle OTA if a client is updating

  // Maintain MQTT
  if (!mqttClient.connected()) {
    mqttConnect();
  } else {
    mqttClient.loop();
  }

  uint32_t now = nowEpoch();

  // Local sensor periodic read
  if (now - lastSensorTime >= SENSOR_INTERVAL_SECONDS) {
    lastSensorTime = now;
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure();
    Serial.printf("Local sensor T: %.2f C  H: %.2f %%  P: %.2f Pa\n", t, h, p);

    // Publish local sensor to MQTT
    StaticJsonDocument<256> sensorDoc;
    sensorDoc["local_temp"] = t;
    sensorDoc["local_humidity"] = h;
    sensorDoc["local_pressure"] = p / 100.0F; // hPa
    sensorDoc["timestamp"] = now;
    String s;
    serializeJson(sensorDoc, s);
    publishMetrics(s, MQTT_TOPIC_METRICS);

    // update display with local values (remote may be stale)
    // If you want to preserve remote data on display, you'd re-render with remote doc.
    renderDisplay(t, h, p, nullptr);
  }

  // Remote forecast fetch
  if (now - lastFetchTime >= FETCH_INTERVAL_SECONDS) {
    lastFetchTime = now;

    // Respect backoff
    if (now < nextAllowedFetchAt) {
      Serial.println("Skipping fetch due to backoff");
    } else {
      String response;
      bool ok = fetchWeatherFromAPI(response);
      if (ok) {
        Serial.println("Fetched weather successfully");
        processFetchedWeather(response);
      } else {
        Serial.println("Failed to fetch weather from API");
        // show cached if available
        String cached = readCache();
        if (cached.length()) {
          DynamicJsonDocument dd(4096);
          if (!deserializeJson(dd, cached)) {
            renderDisplay(bme.readTemperature(), bme.readHumidity(), bme.readPressure(), &dd);
          } else {
            Serial.println("Cached JSON parse error");
          }
        }
      }
    }
  }

  // Optional: deep sleep scheduling to save power
  // If you'd like to use deep sleep, comment out the main loop flow above and
  // put the ESP32 to deep sleep for N seconds after publishing.
  //
  // Example (uncomment to use; adjust wakeTimeS):
  // uint64_t wakeTimeS = 600; // sleep 10 minutes
  // esp_sleep_enable_timer_wakeup(wakeTimeS * 1000000ULL);
  // Serial.println("Going to deep sleep...");
  // delay(100);
  // esp_deep_sleep_start();

  delay(200); // small yield
}
