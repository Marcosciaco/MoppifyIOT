# MoppifyIOT

## Hardware Requirements

- **Adafruit Huzzah32 Feather (ESP32)** board
- **RFID-RC522** reader
- **RGB NeoPixel LED strip**
- Jumper wires for connections

## Libraries Used

- `MFRC522` for RFID reader control
- `WiFi` for network connection
- `HTTPClient` for HTTP communication with the API
- `Adafruit NeoPixel` for LED control
- `ArduinoJson` for JSON parsing

Install these libraries through the Arduino Library Manager if not already installed.

## Circuit Connections

- Connect RFID-RC522 reader to Huzzah32 Feather as follows:
  - `SDA` to GPIO 12
  - `RST` to GPIO 13
  - `MOSI` to GPIO 23
  - `MISO` to GPIO 19
  - `SCK` to GPIO 18
  - `GND` to GND
  - `VCC` to 3.3V
- Connect NeoPixel LED strip to GPIO 15.

## Setup Instructions

1. Open `rfid_cleaning_cart.ino` in the Arduino IDE.
2. Set your WiFi credentials in the `ssid` and `password` variables.
3. Update the `apiUrl` variable with your server's URL.
4. Compile and upload the code to your ESP32 Huzzah32 Feather board.

## Usage

1. Power the system and connect to WiFi.
2. Scan an RFID tag. The system will send the tag data to the server for login or logout.
3. The NeoPixel LED strip will indicate the status of the operation:
   - **Green** for janitor login/logout
   - **Red** for admin login/logout
