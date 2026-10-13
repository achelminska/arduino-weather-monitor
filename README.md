# 🌡️ Arduino Weather Monitor

A simple weather station built with Arduino UNO that reads temperature and humidity from a DHT22 sensor and displays the data on an OLED screen — triggered by a button press.

## ✨ Features

- On-demand temperature & humidity readings (triggered by button)
- Data displayed on a 1.3" OLED screen (128x64), large readable digits with units
- Non-blocking software debounce (`millis()`-based) for reliable button input
- Sensor error handling — shows a message instead of `nan` when the DHT22 is not responding
- Respects the DHT22 minimum sampling interval (2 s) — rapid presses re-show the last good reading
- Blinks the on-board LED if the display is not detected on I2C

## 🛒 Components

| Component | Description |
|---|---|
| Arduino UNO R3 | Microcontroller |
| DHT22 (AM2302) | Temperature & humidity sensor |
| 10 kΩ resistor | Pull-up for the DHT22 data line (see note below) |
| OLED 1.3" 128x64 I2C | Display |
| Pushbutton (4-pin, 6x6mm) | Trigger for readings |
| Breadboard + jumper wires | Wiring |
| USB cable (see *Uploading*) | Programming + power |

## 🔌 Wiring

| Component | Pin | Arduino Pin |
|---|---|---|
| DHT22 | VCC (pin 1) | 5V |
| DHT22 | DATA (pin 2) | D2 |
| DHT22 | NC (pin 3) | — not connected |
| DHT22 | GND (pin 4) | GND |
| 10 kΩ resistor | — | between DHT22 DATA and DHT22 VCC |
| OLED | VCC | 5V |
| OLED | GND | GND |
| OLED | SCL | A5 |
| OLED | SDA | A4 |
| Button | PIN 1 | D8 |
| Button | PIN 2 | GND |

DHT22 pins are numbered 1–4 from left to right when looking at the grille (front) of the sensor.

### Pull-up resistor for the DHT22

The bare 4-pin DHT22 needs a pull-up resistor on its data line: a 10 kΩ resistor connected between **DATA** and **VCC**. On a breadboard that means one leg of the resistor in the same row as the sensor's DATA pin and the other leg in the same row as its VCC pin. Without it the sensor often still works with short wires, but readings become unreliable.

If you have the DHT22 on a small **3-pin breakout board** (with `+`, `out`, `−` labels), the resistor is already on the board and you can skip it.

### Display driver (SSD1306 vs SH1106)

Many 1.3" 128x64 OLED modules use the **SH1106** controller rather than SSD1306. With the Adafruit SSD1306 library an SH1106 screen typically shows the image shifted by a couple of pixels or garbage on the edges. If that happens, switch to an SH1106 library (e.g. `Adafruit SH110X` and `Adafruit_SH1106G`, or `U8g2`) — the rest of the wiring stays the same. Most modules use I2C address `0x3C`; if nothing shows up, try `0x3D` in `OLED_ADDRESS`.

### Button

The button uses the Arduino's internal pull-up (`INPUT_PULLUP`), so no external resistor is needed: the pin reads `HIGH` when idle and `LOW` when pressed.

## 📐 Circuit Diagram

![Circuit Diagram](assets/circuit.png)

> ⚠️ The diagram omits two things that the real circuit needs: the **OLED VCC → 5V** wire and the **10 kΩ pull-up resistor** on the DHT22 data line. Follow the wiring table above.

## 📦 Libraries

Install the following libraries via Arduino IDE → *Sketch → Include Library → Manage Libraries…*:

- `DHT sensor library` — Adafruit
- `Adafruit Unified Sensor` — Adafruit
- `Adafruit SSD1306` — Adafruit
- `Adafruit GFX Library` — Adafruit (`Adafruit BusIO` is pulled in automatically)

## 🚀 Uploading to the board

1. **Cable.** The Arduino UNO R3 has a **USB-B** socket (the square "printer" plug). You need a *USB-A to USB-B* cable plus a USB-C adapter, or a *USB-C to USB-B* cable directly, for a modern Mac. Make sure it is a data cable, not a charge-only one. (The newer UNO R4 uses USB-C.)
2. **Driver (clones only).** An original UNO uses an ATmega16U2 USB chip and needs no driver on macOS. Many clones use a **CH340** chip; recent macOS versions include a driver, but if the board does not appear under ports, install the CH34x driver from WCH.
3. **Arduino IDE.** Open `arduino-weather-monitor/arduino-weather-monitor.ino`, select *Tools → Board → Arduino UNO* and *Tools → Port → /dev/cu.usbmodem…* (original) or */dev/cu.usbserial…* / *wchusbserial…* (CH340 clone).
4. Click **Upload** (→). The on-board TX/RX LEDs flicker during the upload and the display shows *Press the button* when done.

## 🧭 How It Works

1. On start the display shows *Weather Monitor — Press the button*
2. Press the button → Arduino reads temperature and humidity from the DHT22
3. The values are shown in large digits with units (°C, %)
4. Press again for a fresh reading (the DHT22 can only be sampled every 2 s; presses within that window re-show the last reading)
5. If the sensor does not respond, *Sensor error — Check DHT22 wiring* is shown instead

## 📁 Project structure

```
arduino-weather-monitor/
├── arduino-weather-monitor/
│   └── arduino-weather-monitor.ino   # the sketch (folder name must match the file name for Arduino IDE)
├── assets/
│   └── circuit.png
├── LICENSE
└── README.md
```

## 🔮 Future Ideas

- Auto-refresh every N seconds
- Min/max temperature tracking
- Buzzer alert when temperature exceeds threshold
- Battery-powered portable version

## 🛠️ Simulation

Try it yourself in Wokwi: https://wokwi.com/projects/461770914765795329

---

*Built as a hobby project to explore Arduino and embedded systems programming.*
