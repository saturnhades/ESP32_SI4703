/*
 * ESP32 SI4703 FM Radio Library
 * Phase-1: Minimum Viable Driver (MVD)
 */

#include "SI4703.h"

// ------------------------------------------------
// Constructor
// ------------------------------------------------
SI4703::SI4703()
: _i2cAddr(SI4703_I2C_ADDR),
  _sda(-1),
  _scl(-1),
  _i2cClock(100000),
  _currentFreq(0.0f),
  _initialized(false)
{
  memset(_regs, 0, sizeof(_regs));
}

// ------------------------------------------------
// Begin / Init
// ------------------------------------------------
bool SI4703::begin(int sda, int scl, uint32_t i2cClock)
{
  _sda = sda;
  _scl = scl;
  _i2cClock = i2cClock;

  Wire.begin(_sda, _scl);
  Wire.setClock(_i2cClock);

  delay(10); // small settle delay (acceptable at init)

  // Read initial registers to verify presence
  readRegisters();

  // Basic sanity check: DEVICEID register should not be 0x0000
  if (_regs[REG_DEVICEID] == 0x0000) {
    return false;
  }

  powerUp();

  _initialized = true;
  return true;
}

// ------------------------------------------------
// Reset (soft reset via power cycle)
// ------------------------------------------------
void SI4703::reset()
{
  if (!_initialized) return;

  // Disable chip
  readRegisters();
  _regs[REG_POWERCFG] |= POWERCFG_DISABLE;
  writeRegisters();

  delay(10);

  powerUp();
}

// ------------------------------------------------
// Power-up sequence (minimal, stable)
// ------------------------------------------------
void SI4703::powerUp()
{
  // Read current state
  readRegisters();

  // Enable oscillator (TEST1)
  _regs[REG_TEST1] = 0x8100;

  // Enable STC interrupt + RDS (safe defaults)
  _regs[REG_SYSCONFIG1] =
      SYSCONFIG1_RDS |
      SYSCONFIG1_DE;   // 50µs de-emphasis (EU)

  // Channel spacing = 100 kHz (EU)
  _regs[REG_SYSCONFIG2] =
      SYSCONFIG2_SPACE_100KHZ;

  // Enable chip, unmute
  _regs[REG_POWERCFG] =
      POWERCFG_ENABLE |
      POWERCFG_DMUTE;

  writeRegisters();

  delay(110); // datasheet max power-up time
}

// ------------------------------------------------
// Set Frequency (MHz)
// ------------------------------------------------
void SI4703::setFrequency(float mhz)
{
  if (!_initialized) return;

  // Clamp
  if (mhz < SI4703_FREQ_MIN_MHZ) mhz = SI4703_FREQ_MIN_MHZ;
  if (mhz > SI4703_FREQ_MAX_MHZ) mhz = SI4703_FREQ_MAX_MHZ;

  // Convert MHz → channel
  uint16_t channel =
    (uint16_t)((mhz - SI4703_FREQ_MIN_MHZ) / SI4703_CHANNEL_SPACING_MHZ);

  readRegisters();

  _regs[REG_CHANNEL] &= ~CHANNEL_CHAN_MASK;
  _regs[REG_CHANNEL] |= channel;
  _regs[REG_CHANNEL] |= CHANNEL_TUNE;

  writeRegisters();

  // Clear TUNE bit (non-blocking approach)
  delay(60); // minimal required tune delay

  readRegisters();
  _regs[REG_CHANNEL] &= ~CHANNEL_TUNE;
  writeRegisters();

  _currentFreq = mhz;
}

// ------------------------------------------------
// Get Frequency (cached)
// ------------------------------------------------
float SI4703::getFrequency() const
{
  return _currentFreq;
}

// ------------------------------------------------
// Set Volume (0–15)
// ------------------------------------------------
void SI4703::setVolume(uint8_t volume)
{
  if (!_initialized) return;

  if (volume > 15) volume = 15;

  readRegisters();
  _regs[REG_SYSCONFIG2] &= ~SYSCONFIG2_VOLUME_MASK;
  _regs[REG_SYSCONFIG2] |= volume;
  writeRegisters();
}

// ------------------------------------------------
// Get RSSI
// ------------------------------------------------
uint8_t SI4703::getRSSI()
{
  if (!_initialized) return 0;

  readRegisters();
  return (uint8_t)(_regs[REG_STATUSRSSI] & STATUSRSSI_RSSI_MASK);
}

// ------------------------------------------------
// Process (stub for now)
// ------------------------------------------------
void SI4703::process()
{
  // Phase-1: nothing here
  // Phase-2: seek state machine
  // Phase-3: RDS processing
}

// ------------------------------------------------
// Read all registers
// ------------------------------------------------
void SI4703::readRegisters()
{
  Wire.requestFrom(_i2cAddr, (uint8_t)32);

  // Registers start at 0x0A and wrap
  for (uint8_t reg = 0x0A; ; reg++) {
    if (reg == 0x10) reg = 0x00;

    uint16_t hi = Wire.read();
    uint16_t lo = Wire.read();
    _regs[reg] = (hi << 8) | lo;

    if (reg == 0x09) break;
  }
}

// ------------------------------------------------
// Write control registers (0x02 → 0x07)
// ------------------------------------------------
bool SI4703::writeRegisters()
{
  Wire.beginTransmission(_i2cAddr);

  for (uint8_t reg = REG_POWERCFG; reg <= REG_TEST1; reg++) {
    Wire.write(_regs[reg] >> 8);
    Wire.write(_regs[reg] & 0xFF);
  }

  return (Wire.endTransmission() == 0);
}
