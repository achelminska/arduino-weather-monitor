// Arduino Weather Monitor
// Reads temperature and humidity from a DHT22 on button press
// and shows them on a 128x64 I2C OLED (SH1106, 1.3").

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// ---- Hardware configuration ------------------------------------------------
const uint8_t SCREEN_WIDTH = 128;
const uint8_t SCREEN_HEIGHT = 64;
const uint8_t OLED_ADDRESS = 0x3C;   // most 128x64 modules; some use 0x3D
const int8_t  OLED_RESET = -1;       // no reset pin on the module

const uint8_t DHT_PIN = 2;
const uint8_t DHT_TYPE = DHT22;

const uint8_t BUTTON_PIN = 8;        // wired to GND, internal pull-up enabled

// ---- Timing ----------------------------------------------------------------
const unsigned long DEBOUNCE_MS = 50;       // ignore contact bounce shorter than this
const unsigned long SENSOR_MIN_INTERVAL_MS = 2000;  // DHT22 cannot be read faster than every 2 s

// ---- Objects ---------------------------------------------------------------
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---- Button state (software debounce, non-blocking) ------------------------
int lastRawButtonState = HIGH;       // last raw reading from the pin
int stableButtonState = HIGH;        // debounced state
unsigned long lastBounceTime = 0;

unsigned long lastReadTime = 0;      // when the sensor was last read
bool haveReading = false;
float lastTemperature = NAN;
float lastHumidity = NAN;

// ---- Display helpers -------------------------------------------------------
void showMessage(const __FlashStringHelper *line1, const __FlashStringHelper *line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  if (line2) {
    display.println(line2);
  }
  display.display();
}

void showReading(float temperature, float humidity) {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Temperature"));
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(temperature, 1);
  display.print(F(" C"));

  display.setTextSize(1);
  display.setCursor(0, 34);
  display.println(F("Humidity"));
  display.setTextSize(2);
  display.setCursor(0, 44);
  display.print(humidity, 1);
  display.print(F(" %"));

  display.display();
}

// ---- Sensor ----------------------------------------------------------------
void takeReading() {
  // The DHT22 returns the previous value (or garbage) if polled faster than
  // every 2 s, so just redisplay the last good reading in that case.
  unsigned long now = millis();
  if (haveReading && now - lastReadTime < SENSOR_MIN_INTERVAL_MS) {
    showReading(lastTemperature, lastHumidity);
    return;
  }

  showMessage(F("Reading..."), nullptr);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  lastReadTime = millis();

  if (isnan(temperature) || isnan(humidity)) {
    showMessage(F("Sensor error"), F("Check DHT22 wiring"));
    return;
  }

  lastTemperature = temperature;
  lastHumidity = humidity;
  haveReading = true;
  showReading(temperature, humidity);
}

// ---- Arduino entry points --------------------------------------------------
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  dht.begin();

  if (!display.begin(OLED_ADDRESS, true)) {
    // Display not found: blink the on-board LED forever so the failure is visible.
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(200);
    }
  }

  showMessage(F("Weather Monitor"), F("Press the button"));
}

void loop() {
  int rawState = digitalRead(BUTTON_PIN);

  // Restart the debounce timer whenever the raw reading changes.
  if (rawState != lastRawButtonState) {
    lastBounceTime = millis();
    lastRawButtonState = rawState;
  }

  // Accept the new state only once it has been steady for DEBOUNCE_MS.
  if (millis() - lastBounceTime > DEBOUNCE_MS && rawState != stableButtonState) {
    stableButtonState = rawState;

    // Button is wired to GND with the internal pull-up, so LOW = pressed.
    if (stableButtonState == LOW) {
      takeReading();
    }
  }
}
