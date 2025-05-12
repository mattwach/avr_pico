#ifndef DS3231_DS3231_H
#define DS3231_DS3231_H
// Interface API for the DS3231 Real time clock
//
// Example usage:
//
// // Onetime setup
// struct DS3231 ds3231;
// ds3231_init(&ds3231);
//
// // Read current time
// ds3231_read(&ds3231);
// // Note that these function are paring the data snapshotted in
// // ds3231 by ds3231_read. This approach avoids race conditions
// // that can occur when reading them separately.
// uint8_t hours = ds3231_parse_hour_24(&ds3231);
// uint8_t minutes = ds3231_parse_minutes(&ds3231);
// uint8_t seconds = ds3231_parse_seconds(&ds3231);
// 

#include <inttypes.h>
#include <error_codes.h>

// You can add this is a compiler DEF (or ahead of the header include) to
// increase the read/write size
// 0x07 = time only
// 0x0E = Include alarm data
// 0x10 = Include control/status data
// 0x11 = Include aging offset
// 0x13 = Include Temperature data
#ifndef DS3231_READ_LENGTH
#define DS3231_READ_LENGTH 7
#endif

struct DS3231 {
  // If there are communication errors, the code is put here.
  // If this field is non-zero, subsequent calls become no-ops
  // until it is cleared.
  error_t error;
  uint8_t data[DS3231_READ_LENGTH];  
};

void ds3231_init(struct DS3231 *device);

// Reads partial data.  Note that sizeof(DS3231.data) must be >= offset_length
// as the data is read into data + offset (so that parsing functions can work properly)
void ds3231_read_with_offset(
    struct DS3231* device, uint8_t offset, uint8_t length);
static inline void ds3231_read(struct DS3231* device) {
  ds3231_read_with_offset(device, 0, DS3231_READ_LENGTH);
}

// Writes partial data.  Rules are the same as ds3231_read_with_offset
void ds3231_write_with_offset(
    struct DS3231* device, uint8_t offset, uint8_t length);
static inline void ds3231_write(struct DS3231* device) {
  ds3231_write_with_offset(device, 0, DS3231_READ_LENGTH);
}

//
// Getters and setters. 
// Note that, depending on DS3231_READ_LENGTH, some of these may not be defined
//
// A ds3231_read (or ds3231_read_with_offset) must be called to populate
// the DS3231 structure before calling any parse functions.
//
// Calling any of the set functions only updates the DS3231 buffer.
// A call to ds3231_write (or ds3231_write_with_offset) must be
// made to change the device state.
#if DS3231_READ_LENGTH > 0
static inline uint8_t ds3231_parse_seconds(const struct DS3231* device) {
  return (device->data[0] >> 4) * 10 + (device->data[0] & 0x0F);
}

static inline void ds3231_set_seconds(struct DS3231* device, uint8_t seconds) {
  device->data[0] = ((seconds / 10) << 4) | (seconds % 10);
}
#endif

#if DS3231_READ_LENGTH > 1
static inline uint8_t ds3231_parse_minutes(const struct DS3231* device) {
  return (device->data[1] >> 4) * 10 + (device->data[1] & 0x0F);
}

static inline void ds3231_set_minutes(struct DS3231* device, uint8_t minutes) {
  device->data[1] = ((minutes / 10) << 4) | (minutes % 10);
}
#endif

#if DS3231_READ_LENGTH > 2
// always 24h format
uint8_t ds3231_parse_hours_24(const struct DS3231* device);
void ds3231_set_hours_24(struct DS3231* device, uint8_t hours);
#endif

#if DS3231_READ_LENGTH > 3
// Reads 0=Sunday, 1=Monday, ...
// Note this is 1 less than the hardware returns so that array indexing (into
// an array of weekday strings) is more convienent.
static inline uint8_t ds3231_parse_day_of_week(const struct DS3231* device) {
  return device->data[3] - 1;
}

static inline void ds3231_set_day_of_week(struct DS3231* device, uint8_t day_of_week) {
  device->data[3] = day_of_week + 1;
}
#endif

#if DS3231_READ_LENGTH > 4
// Returns 1-31
static inline uint8_t ds3231_parse_day_of_month(const struct DS3231* device) {
  return (device->data[4] >> 4) * 10 + (device->data[4] & 0x0F);
}

static inline void ds3231_set_day_of_month(struct DS3231* device, uint8_t day_of_month) {
  device->data[4] = ((day_of_month / 10) << 4) | (day_of_month % 10);
}
#endif

#if DS3231_READ_LENGTH > 5
// Returns 1-12
static inline uint8_t ds3231_parse_month(const struct DS3231* device) {
  return ((device->data[5] >> 4) & 0x01) * 10 + (device->data[5] & 0x0F);
}

static inline void ds3231_set_month(struct DS3231* device, uint8_t month) {
  device->data[5] = ((month / 10) << 4) | (month % 10);
}
#endif

#if DS3231_READ_LENGTH > 6
// Returns 0-99
static inline uint8_t ds3231_parse_year(const struct DS3231* device) {
  return (device->data[6] >> 4) * 10 + (device->data[6] & 0x0F);
}

static inline void ds3231_set_year(struct DS3231* device, uint8_t year) {
  device->data[6] = ((year / 10) << 4) | (year % 10);
}
#endif

#endif

