#include <ds3231/ds3231.h>
#include <twi/twi.h>
#include <string.h>

#define DS3231_ADDRESS 0x68

void ds3231_init(struct DS3231 *device) {
  memset(device, 0, sizeof(*device));
}

void ds3231_read_with_offset(
    struct DS3231 *device, uint8_t offset, uint8_t length) {
  error_t* err = &(device->error);
  twi_startWrite(DS3231_ADDRESS, err);
  twi_writeNoStop(offset, err);
  twi_readWithStop(DS3231_ADDRESS, device->data + offset, length, err);
}

void ds3231_write_with_offset(
    struct DS3231 *device, uint8_t offset, uint8_t length) {
  error_t* err = &(device->error);
  twi_startWrite(DS3231_ADDRESS, err);
  twi_writeNoStop(offset, err);
  for (uint8_t i = offset; i < offset + length; ++i) {
    twi_writeNoStop(device->data[i], err);
  }
  twi_stop(err);
}

uint8_t ds3231_parse_hours_24(const struct DS3231 *device) {
  uint8_t hours = device->data[2] & 0x0F;
  if (device->data[2] & 0x10) {
    hours += 10;
  }
  if (device->data[2] & 0x40) {
    // 12 hour mode
    if (device->data[2] & 0x20) {
      hours += 12;
    }
  } else {
    if (device->data[2] & 0x20) {
      hours += 20;
    }
  }
  return hours;
}

void ds3231_set_hours_24(struct DS3231 *device, uint8_t hours) {
  device->data[2] = hours % 10;
  if (hours >= 20) {
    device->data[2] |= 0x20;
  } else if (hours >= 10) {
    device->data[2] |= 0x10;
  }
}
