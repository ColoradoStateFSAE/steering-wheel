#if !defined(MOCK_ADAFRUIT_MCP_2515) && defined(NATIVE)
#define MOCK_ADAFRUIT_MCP_2515

#include <Arduino.h>

class Adafruit_MCP2515 {
  public:
    Adafruit_MCP2515() = default;
    virtual ~Adafruit_MCP2515() = default;
    virtual int begin(long baudRate) = 0;
    virtual int beginPacket(int id) = 0;
    virtual size_t write(uint8_t *buffer, size_t size) = 0;
    virtual int endPacket() = 0;
    virtual int parsePacket() = 0;
};

#endif
