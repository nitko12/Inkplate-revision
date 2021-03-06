#ifndef __MCP_H__
#define __MCP_H__

#define MCP23017_IODIRA 0x00
#define MCP23017_GPPUA  0x0C
#define MCP23017_ADDR   0x20

#define MCP23017_GPIOA    0x12
#define MCP23017_IOCONA   0x0A
#define MCP23017_INTCONA  0x08
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA  0x06
#define MCP23017_INTFA    0x0E
#define MCP23017_INTCAPA  0x10
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOB    0x13

#include "Arduino.h"
#include "Wire.h"

class Mcp
{
  public:
    bool mcpBegin(uint8_t _addr, uint8_t *_r);
    void readMCPRegisters(uint8_t _addr, uint8_t *k);
    void readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n);
    void readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k);
    void updateAllRegisters(uint8_t _addr, uint8_t *k);
    void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d);
    void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n);

    void pinModeMCP(uint8_t _pin, uint8_t _mode);
    void digitalWriteMCP(uint8_t _pin, uint8_t _state);
    uint8_t digitalReadMCP(uint8_t _pin);
    void setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain, uint8_t polarity);
    void setIntPin(uint8_t _pin, uint8_t _mode);
    void removeIntPin(uint8_t _pin);
    uint16_t getINT();
    uint16_t getINTstate();
    void setPorts(uint16_t _d);
    uint16_t getPorts();

    uint8_t mcpRegsInt[22];

  private:
};

#endif