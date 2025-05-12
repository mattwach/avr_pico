#include <avr/interrupt.h>
#include <lowpower/lowpower.h>
#include <pstr/pstr.h>
#include <oledm/oledm.h>
#include <oledm/console.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus6x8.h>
#include <petit_fatfs/diskio.h>
#include <petit_fatfs/pff.h>

// Atmega328P:
//  A4 <- SDA
//  A5 <- SCL
//  ADC1 <- ADC

#define OLEDM_INIT oledm_basic_init
//#define OLED64x32
//#define OLEDM_INIT ssd1306_64x32_a_init
//#define OLEDM_INIT ssd1306_128x32_a_init

struct Console console;
struct OLEDM display;
FATFS fs;

void check(const char* str, FRESULT result) {
  console_print(&console, str);
  console_print(&console, ": ");
  switch (result) {
    case FR_OK:
      console_print(&console, "OK\n");
      return;
    case FR_NOT_READY:
      console_print(&console, "NOT_READY\n");
      break;
    case FR_DISK_ERR:
      console_print(&console, "DISK_ERR\n");
      break;
    case FR_NO_FILESYSTEM:
      console_print(&console, "NO_FILESYSTEM\n");
      break;
    case FR_NOT_ENABLED:
      console_print(&console, "NOT_ENABLED\n");
      break;
    case FR_NO_FILE:
      console_print(&console, "NO_FILE\n");
      break;
    default:
      console_print(&console, "INTERNAL\n");
      break;
  }

  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}

void cat_file() {
  char c;
  unsigned int bytes_read = 1;
  while (bytes_read > 0) {
    FRESULT result = pf_read(&c, 1, &bytes_read);
    if (result != FR_OK) {
      check("pf_read", result);
    }
    if (bytes_read > 0) {
      console_printlen(&console, &c, 1);
    }
  }
  console_print(&console, "\n");
}

int main(void) {
  sei();
  OLEDM_INIT(&display);
  oledm_start(&display);

  oledm_clear(&display, 0x00);
  console_init(&console, &display, terminus6x8);

  console_print(&console, "Started\n");

  check("disk_initialize", disk_initialize() == 0 ? FR_OK : FR_DISK_ERR);
  check("pf_mount", pf_mount(&fs));
  check("pf_open", pf_open("HELLO.TXT"));

  cat_file();

  console_print(&console, "Finished\n");
  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}
