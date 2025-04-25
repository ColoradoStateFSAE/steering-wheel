#include <Adafruit_MCP2515.h>
#include <SPI.h>

// ----------------------------------------------------
// Pin/baudrate setup for the Adafruit MCP2515 library
// ----------------------------------------------------

// The Adafruit library uses these definitions internally.
// For the Adafruit RP2040 CAN Bus Feather, CS_PIN is PIN_CAN_CS.
#if defined(ARDUINO_ADAFRUIT_FEATHER_RP2040_CAN)
  #define CS_PIN PIN_CAN_CS
#else
  #define CS_PIN 7 // Fallback if you're on a different board
#endif

// Choose your CAN baud rate (250 kbps in this example).
#define CAN_BAUDRATE 250000

// Create MCP2515 object
Adafruit_MCP2515 mcp(CS_PIN);

// ----------------------------------------------------
// Application variables
// ----------------------------------------------------

// Storage for incoming CAN data
static uint32_t rxId = 0;
static uint8_t rxBuf[8];
static uint8_t rxLen = 0;

// Example output pins
const int DPO1 = 3;
const int DPO2 = 5;
const int DPO3 = 6;
const int DPO4 = 9;

// Example input pins
const int DPI1input = 4;
const int DPI2input = 4;
const int DPI3input = 4;
const int DPI4input = 4;

// Track digital input states
bool DPI1in = false;
bool DPI2in = false;
bool DPI3in = false;
bool DPI4in = false;

// For analog reading
int scaledvalue1 = 0;
int scaledvalue2 = 0;
int scaledvalue3 = 0;
int scaledvalue4 = 0;

// Task intervals (milliseconds)
unsigned long task1Interval = 100;  // Keep-alive
unsigned long task2Interval = 20;   // Send analog values
unsigned long task3Interval = 20;   // Drive digital outputs
unsigned long task4Interval = 20;   // Send digital input states

// Millis trackers
unsigned long task1Millis = 0;
unsigned long task2Millis = 0;
unsigned long task3Millis = 0;
unsigned long task4Millis = 0;

// Received outputs for pins
uint8_t DPO1out = 0;
uint8_t DPO2out = 0;
uint8_t DPO3out = 0;
uint8_t DPO4out = 0;

// ----------------------------------------------------
// Setup
// ----------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("Adafruit MCP2515 + RP2040 CAN Feather Example");

  // Initialize SPI
  SPI.begin();

  // Initialize MCP2515 at the chosen baud rate
  if (!mcp.begin(CAN_BAUDRATE)) {
    Serial.println("Error initializing MCP2515.");
    while (1) { delay(10); }
  }
  Serial.println("MCP2515 Initialized Successfully!");

  // Configure pins
  pinMode(DPO1, OUTPUT);
  pinMode(DPO2, OUTPUT);
  pinMode(DPO3, OUTPUT);
  pinMode(DPO4, OUTPUT);

  pinMode(DPI1input, INPUT_PULLUP);
  pinMode(DPI2input, INPUT_PULLUP);
  pinMode(DPI3input, INPUT_PULLUP);
  pinMode(DPI4input, INPUT_PULLUP);

  Serial.println("Setup complete!");
}

// ----------------------------------------------------
// Main Loop
// ----------------------------------------------------
void loop() {
  unsigned long currentMillis = millis();

  // 1) Send KeepAlive
  if (currentMillis - task1Millis >= task1Interval) {
    task1Millis = currentMillis;
    SendKeepAlive();
  }

  // 2) Send Analog Values
  if (currentMillis - task2Millis >= task2Interval) {
    task2Millis = currentMillis;
    SendAnalogValues();
  }

  // 3) Drive Digital Outputs
  if (currentMillis - task3Millis >= task3Interval) {
    task3Millis = currentMillis;
    DriveDigitalPin();
  }

  // 4) Send Digital Input States
  if (currentMillis - task4Millis >= task4Interval) {
    task4Millis = currentMillis;
    SendDPIValues();
  }

  // Check for incoming CAN frames
  int packetSize = mcp.parsePacket();
  if (packetSize) {
    // We have a new packet
    rxId = mcp.packetId();    // 11-bit or 29-bit ID
    rxLen = packetSize;       // Number of data bytes
    if (!mcp.packetRtr()) {
      // For non-RTR, read the data
      for (int i = 0; i < rxLen; i++) {
        rxBuf[i] = mcp.read();
      }
    }
    // Process the received frame
    canRead();
  }
}

// ----------------------------------------------------
// Process Incoming CAN Data
// ----------------------------------------------------
void canRead() {
  // Example logic: If ID is 0x2D1 or 0x2D3, read outputs
  if (rxId == 0x2D1 && rxLen >= 5) {
    DPO1out = rxBuf[0];
    DPO2out = rxBuf[4];
  }
  else if (rxId == 0x2D3 && rxLen >= 5) {
    DPO3out = rxBuf[0];
    DPO4out = rxBuf[4];
  }
}

// ----------------------------------------------------
// Drive Digital Outputs (DPOx) based on received data
// ----------------------------------------------------
void DriveDigitalPin() {
  // DPO1
  if (DPO1out < 0xFA) {
    analogWrite(DPO1, map(DPO1out, 0x00, 0xF9, 0, 255));
  } else if (DPO1out == 0xFA) {
    digitalWrite(DPO1, HIGH);
  } else if (DPO1out == 0x00) {
    digitalWrite(DPO1, LOW);
  }

  // DPO2
  if (DPO2out < 0xFA) {
    analogWrite(DPO2, map(DPO2out, 0x00, 0xF9, 0, 255));
  } else if (DPO2out == 0xFA) {
    digitalWrite(DPO2, HIGH);
  } else if (DPO2out == 0x00) {
    digitalWrite(DPO2, LOW);
  }

  // DPO3
  if (DPO3out < 0xFA) {
    analogWrite(DPO3, map(DPO3out, 0x00, 0xF9, 0, 255));
  } else if (DPO3out == 0xFA) {
    digitalWrite(DPO3, HIGH);
  } else if (DPO3out == 0x00) {
    digitalWrite(DPO3, LOW);
  }

  // DPO4
  if (DPO4out < 0xFA) {
    analogWrite(DPO4, map(DPO4out, 0x00, 0xF9, 0, 255));
  } else if (DPO4out == 0xFA) {
    digitalWrite(DPO4, HIGH);
  } else if (DPO4out == 0x00) {
    digitalWrite(DPO4, LOW);
  }
}

// ----------------------------------------------------
// Send Keep-Alive frame (ID 0x2C7, 5 bytes)
// ----------------------------------------------------
void SendKeepAlive() {
  mcp.beginPacket(0x2C7);  // 11-bit ID
  mcp.write(0x10);
  mcp.write(0x09);
  mcp.write(0x0A);
  mcp.write(0x00);
  mcp.write(0x00);
  mcp.endPacket();         // Sends the frame
}

// ----------------------------------------------------
// Send Digital Input States (IDs 0x2C3 and 0x2C5, 8 bytes each)
// ----------------------------------------------------
void SendDPIValues() {
  // Read digital inputs
  bool dpi1 = (digitalRead(DPI1input) == HIGH);
  bool dpi2 = (digitalRead(DPI2input) == HIGH);
  bool dpi3 = (digitalRead(DPI3input) == HIGH);
  bool dpi4 = (digitalRead(DPI4input) == HIGH);

  // First frame (ID 0x2C3)
  mcp.beginPacket(0x2C3);
  mcp.write(dpi1 ? 250 : 0);  // Byte 0
  mcp.write(0);               // Byte 1
  mcp.write(0);               // Byte 2
  mcp.write(0);               // Byte 3
  mcp.write(dpi2 ? 250 : 0);  // Byte 4
  mcp.write(0);               // Byte 5
  mcp.write(0);               // Byte 6
  mcp.write(0);               // Byte 7
  mcp.endPacket();

  // Second frame (ID 0x2C5)
  mcp.beginPacket(0x2C5);
  mcp.write(dpi3 ? 250 : 0);
  mcp.write(0);
  mcp.write(0);
  mcp.write(0);
  mcp.write(dpi4 ? 250 : 0);
  mcp.write(0);
  mcp.write(0);
  mcp.write(0);
  mcp.endPacket();
}

// ----------------------------------------------------
// Send Analog Values (ID 0x2C1, 8 bytes)
// ----------------------------------------------------
void SendAnalogValues() {
  // Example: read A0-A3 and scale to 12 bits (0-4095)
  scaledvalue1 = map(analogRead(A0), 0, 1023, 0, 4095);
  scaledvalue2 = map(analogRead(A1), 0, 1023, 0, 4095);
  scaledvalue3 = map(analogRead(A2), 0, 1023, 0, 4095);
  scaledvalue4 = map(analogRead(A3), 0, 1023, 0, 4095);

  // Send as MSB/LSB pairs for each channel
  mcp.beginPacket(0x2C1);
  // value1
  mcp.write((scaledvalue1 >> 8) & 0xFF);
  mcp.write(scaledvalue1 & 0xFF);
  // value2
  mcp.write((scaledvalue2 >> 8) & 0xFF);
  mcp.write(scaledvalue2 & 0xFF);
  // value3
  mcp.write((scaledvalue3 >> 8) & 0xFF);
  mcp.write(scaledvalue3 & 0xFF);
  // value4
  mcp.write((scaledvalue4 >> 8) & 0xFF);
  mcp.write(scaledvalue4 & 0xFF);
  mcp.endPacket();
}
