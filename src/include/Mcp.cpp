#include "Mcp.h"

// LOW LEVEL:

bool Mcp::mcpBegin(uint8_t _addr, uint8_t *_r)
{
    Wire.beginTransmission(_addr);
    int error = Wire.endTransmission();
    if (error)
        return false;
    readMCPRegisters(_addr, _r);
    _r[0] = 0xff;
    _r[1] = 0xff;
    updateAllRegisters(_addr, _r);
    return true;
}

void Mcp::readMCPRegisters(uint8_t _addr, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)22);
    for (int i = 0; i < 22; ++i)
    {
        k[i] = Wire.read();
    }
}

void Mcp::readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.endTransmission();

    Wire.requestFrom(_addr, _n);
    for (int i = 0; i < _n; ++i)
    {
        k[_regName + i] = Wire.read();
    }
}

void Mcp::readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)1);
    k[_regName] = Wire.read();
}

void Mcp::updateAllRegisters(uint8_t _addr, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(0x00);
    for (int i = 0; i < 22; ++i)
    {
        Wire.write(k[i]);
    }
    Wire.endTransmission();
}

void Mcp::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.write(_d);
    Wire.endTransmission();
}

void Mcp::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    for (int i = 0; i < _n; ++i)
    {
        Wire.write(k[_regName + i]);
    }
    Wire.endTransmission();
}

// HIGH LEVEL:

void Mcp::pinModeMCP(uint8_t _pin, uint8_t _mode)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    switch (_mode)
    {
    case INPUT:
        mcpRegsInt[MCP23017_IODIRA + _port] |= 1 << _p;   // Set it to input
        mcpRegsInt[MCP23017_GPPUA + _port] &= ~(1 << _p); // Disable pullup on that pin
        updateRegister(MCP23017_ADDR, MCP23017_IODIRA + _port, mcpRegsInt[MCP23017_IODIRA + _port]);
        updateRegister(MCP23017_ADDR, MCP23017_GPPUA + _port, mcpRegsInt[MCP23017_GPPUA + _port]);
        break;

    case INPUT_PULLUP:
        mcpRegsInt[MCP23017_IODIRA + _port] |= 1 << _p; // Set it to input
        mcpRegsInt[MCP23017_GPPUA + _port] |= 1 << _p;  // Enable pullup on that pin
        updateRegister(MCP23017_ADDR, MCP23017_IODIRA + _port, mcpRegsInt[MCP23017_IODIRA + _port]);
        updateRegister(MCP23017_ADDR, MCP23017_GPPUA + _port, mcpRegsInt[MCP23017_GPPUA + _port]);
        break;

    case OUTPUT:
        mcpRegsInt[MCP23017_IODIRA + _port] &= ~(1 << _p); // Set it to output
        mcpRegsInt[MCP23017_GPPUA + _port] &= ~(1 << _p);  // Disable pullup on that pin
        updateRegister(MCP23017_ADDR, MCP23017_IODIRA + _port, mcpRegsInt[MCP23017_IODIRA + _port]);
        updateRegister(MCP23017_ADDR, MCP23017_GPPUA + _port, mcpRegsInt[MCP23017_GPPUA + _port]);
        break;
    }
}

void Mcp::digitalWriteMCP(uint8_t _pin, uint8_t _state)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    if (mcpRegsInt[MCP23017_IODIRA + _port] & (1 << _p))
        return;
    _state ? (mcpRegsInt[MCP23017_GPIOA + _port] |= (1 << _p)) : (mcpRegsInt[MCP23017_GPIOA + _port] &= ~(1 << _p));
    updateRegister(MCP23017_ADDR, MCP23017_GPIOA + _port, mcpRegsInt[MCP23017_GPIOA + _port]);
}

uint8_t Mcp::digitalReadMCP(uint8_t _pin)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    readMCPRegister(MCP23017_ADDR, MCP23017_GPIOA + _port, mcpRegsInt);
    return (mcpRegsInt[MCP23017_GPIOA + _port] & (1 << _p)) ? HIGH : LOW;
}

void Mcp::setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain, uint8_t polarity)
{
    intPort &= 1;
    mirroring &= 1;
    openDrain &= 1;
    polarity &= 1;
    mcpRegsInt[MCP23017_IOCONA + intPort] = (mcpRegsInt[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (1 << mirroring);
    mcpRegsInt[MCP23017_IOCONA + intPort] = (mcpRegsInt[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (1 << openDrain);
    mcpRegsInt[MCP23017_IOCONA + intPort] = (mcpRegsInt[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (1 << polarity);
    updateRegister(MCP23017_ADDR, MCP23017_IOCONA + intPort, mcpRegsInt[MCP23017_IOCONA + intPort]);
}

void Mcp::setIntPin(uint8_t _pin, uint8_t _mode)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    switch (_mode)
    {
    case CHANGE:
        mcpRegsInt[MCP23017_INTCONA + _port] &= ~(1 << _p);
        break;

    case FALLING:
        mcpRegsInt[MCP23017_INTCONA + _port] |= (1 << _p);
        mcpRegsInt[MCP23017_DEFVALA + _port] |= (1 << _p);
        break;

    case RISING:
        mcpRegsInt[MCP23017_INTCONA + _port] |= (1 << _p);
        mcpRegsInt[MCP23017_DEFVALA + _port] &= ~(1 << _p);
        break;
    }
    mcpRegsInt[MCP23017_GPINTENA + _port] |= (1 << _p);
    updateRegister(MCP23017_ADDR, MCP23017_GPINTENA, mcpRegsInt, 6);
}

void Mcp::removeIntPin(uint8_t _pin)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    mcpRegsInt[MCP23017_GPINTENA + _port] &= ~(1 << _p);
    updateRegister(MCP23017_ADDR, MCP23017_GPINTENA, mcpRegsInt, 2);
}

uint16_t Mcp::getINT()
{
    readMCPRegisters(MCP23017_ADDR, MCP23017_INTFA, mcpRegsInt, 2);
    return ((mcpRegsInt[MCP23017_INTFB] << 8) | mcpRegsInt[MCP23017_INTFA]);
}

uint16_t Mcp::getINTstate()
{
    readMCPRegisters(MCP23017_ADDR, MCP23017_INTCAPA, mcpRegsInt, 2);
    return ((mcpRegsInt[MCP23017_INTCAPB] << 8) | mcpRegsInt[MCP23017_INTCAPA]);
}

void Mcp::setPorts(uint16_t _d)
{
    mcpRegsInt[MCP23017_GPIOA] = _d & 0xff;
    mcpRegsInt[MCP23017_GPIOB] = (_d >> 8) & 0xff;
    updateRegister(MCP23017_ADDR, MCP23017_GPIOA, mcpRegsInt, 2);
}

uint16_t Mcp::getPorts()
{
    readMCPRegisters(MCP23017_ADDR, MCP23017_GPIOA, mcpRegsInt, 2);
    return ((mcpRegsInt[MCP23017_GPIOB] << 8) | (mcpRegsInt[MCP23017_GPIOA]));
}