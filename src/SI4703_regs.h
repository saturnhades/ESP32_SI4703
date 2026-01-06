#pragma once
#include <Arduino.h>

/*
 * SI4703 Register Map & Bit Definitions
 * ESP32 SI4703 Library
 *
 * This file contains ONLY register addresses, bit masks,
 * enums, and constants. No logic.
 *
 * Datasheet: Silicon Labs Si4702/03
 */

// =======================
// I2C ADDRESS
// =======================
// NOTE: Arduino Wire uses 7-bit address (no R/W bit)
#define SI4703_I2C_ADDR  0x10


// =======================
// REGISTER ADDRESSES
// =======================
enum SI4703_Register : uint8_t {
  REG_DEVICEID    = 0x00,
  REG_CHIPID      = 0x01,
  REG_POWERCFG    = 0x02,
  REG_CHANNEL     = 0x03,
  REG_SYSCONFIG1  = 0x04,
  REG_SYSCONFIG2  = 0x05,
  REG_SYSCONFIG3  = 0x06,
  REG_TEST1       = 0x07,
  REG_STATUSRSSI  = 0x0A,
  REG_READCHAN    = 0x0B,
  REG_RDSA        = 0x0C,
  REG_RDSB        = 0x0D,
  REG_RDSC        = 0x0E,
  REG_RDSD        = 0x0F
};


// =======================
// POWERCFG (0x02)
// =======================
#define POWERCFG_SMUTE     (1 << 15)
#define POWERCFG_DMUTE     (1 << 14)
#define POWERCFG_SKMODE    (1 << 10)
#define POWERCFG_SEEKUP    (1 << 9)
#define POWERCFG_SEEK      (1 << 8)
#define POWERCFG_DISABLE   (1 << 6)
#define POWERCFG_ENABLE    (1 << 0)


// =======================
// CHANNEL (0x03)
// =======================
#define CHANNEL_TUNE       (1 << 15)
#define CHANNEL_CHAN_MASK  0x03FF


// =======================
// SYSCONFIG1 (0x04)
// =======================
#define SYSCONFIG1_RDS     (1 << 12)
#define SYSCONFIG1_DE      (1 << 11)   // De-emphasis: 1 = 50us (EU), 0 = 75us (US)
#define SYSCONFIG1_STCIEN  (1 << 14)


// =======================
// SYSCONFIG2 (0x05)
// =======================
#define SYSCONFIG2_VOLUME_MASK  0x000F

// Channel spacing
#define SYSCONFIG2_SPACE_200KHZ (0 << 4)
#define SYSCONFIG2_SPACE_100KHZ (1 << 4)
#define SYSCONFIG2_SPACE_50KHZ  (2 << 4)


// =======================
// SYSCONFIG3 (0x06)
// =======================
#define SYSCONFIG3_SKSNR_MASK   0x00F0
#define SYSCONFIG3_SKCNT_MASK   0x000F


// =======================
// STATUSRSSI (0x0A)
// =======================
#define STATUSRSSI_RDSR    (1 << 15)
#define STATUSRSSI_STC     (1 << 14)
#define STATUSRSSI_SFBL    (1 << 13)
#define STATUSRSSI_AFCRL  (1 << 12)
#define STATUSRSSI_RDSS   (1 << 11)
#define STATUSRSSI_STEREO (1 << 8)
#define STATUSRSSI_RSSI_MASK 0x00FF


// =======================
// READCHAN (0x0B)
// =======================
#define READCHAN_CHAN_MASK 0x03FF


// =======================
// RDS BLOCKS
// =======================
#define RDS_GROUP_TYPE_MASK  0xF000
#define RDS_GROUP_VERSION_B  (1 << 11)
#define RDS_PROGRAM_TYPE_MASK 0x03E0
#define RDS_TEXT_AB_FLAG     (1 << 4)


// =======================
// BAND LIMITS (EU DEFAULT)
// =======================
#define SI4703_FREQ_MIN_MHZ  87.5f
#define SI4703_FREQ_MAX_MHZ  108.0f
#define SI4703_CHANNEL_SPACING_MHZ 0.1f   // 100 kHz (EU)


// =======================
// INTERNAL CONSTANTS
// =======================
#define SI4703_REGISTER_COUNT 16
#define SI4703_CONTROL_REG_START 0x02
#define SI4703_CONTROL_REG_END   0x07


// =======================
// DRIVER STATES (for state machine)
// =======================
enum class SI4703_State : uint8_t {
  IDLE,
  POWERING_UP,
  TUNING,
  SEEKING,
  RDS_READING,
  ERROR
};


// =======================
// SEEK DIRECTION
// =======================
enum class SI4703_SeekDirection : uint8_t {
  DOWN = 0,
  UP   = 1
};
