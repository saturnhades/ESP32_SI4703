#pragma once
/*
 * ESP32 SI4703 FM Radio Library
 * Phase-1: Minimum Viable Driver (MVD)
 *
 * Public API ONLY
 * No register access here
 * No blocking calls
 */

#include <Arduino.h>
#include <Wire.h>

#include "SI4703_regs.h"

class SI4703 {
public:
  // ---------- Constructor ----------
  SI4703();

  // ---------- Core ----------
  bool begin(int sda = 21, int scl = 22, uint32_t i2cClock = 100000);
  void reset();

  // ---------- Tuning ----------
  void setFrequency(float mhz);   // 87.5 – 108.0
  float getFrequency() const;

  // ---------- Audio ----------
  void setVolume(uint8_t volume); // 0 – 15
  uint8_t getRSSI();

  // ---------- Engine ----------
  void process();                 // non-blocking (stub in Phase-1)

private:
  // ---------- I2C ----------
  uint8_t _i2cAddr;
  int _sda;
  int _scl;
  uint32_t _i2cClock;

  // ---------- Registers ----------
  uint16_t _regs[SI4703_REGISTER_COUNT];

  // ---------- State ----------
  float _currentFreq;
  bool _initialized;

  // ---------- Low-level helpers (internal) ----------
  void readRegisters();
  bool writeRegisters();

  void powerUp();
};
