// NRF2410 test sender

#include <avr/interrupt.h>
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <oledm/console.h>
#include <oledm/font/terminus6x8.h>
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <pstr/pstr.h>
#include <rf24/rf24.h>

#define OLEDM_INIT oledm_basic_init

struct Console console;
struct OLEDM display;
RF24 radio;

//// Radio address.  Transmitter and receiver need to match
//const uint8_t address[6] = "Xach0W";
const uint8_t address[6] = "LIDDIE";

// When the chip is sleeping, it's not measuring MS.  So
// We keep track of approximate sleep time.  It's approximate
// because the RF24 IRQ can wake up the chip early.
uint16_t up_sec = 0;
uint16_t num_sent = 0;

void fatal(const char* msg) {
  console_print(&console, msg);
  console_print(&console, "\n");
  while (1) {
    lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}


void init_oled(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  console_init(&console, &display, terminus6x8);
  console_print(&console, "Ready\n");
}


bool_t init_radio(void) {
  rf24_init(&radio, 9, 10); // CE, CSN
  if (!rf24_begin(&radio)) {
    return FALSE;
  }
  rf24_enable_dynamic_payloads(&radio);
  rf24_set_data_rate(&radio, RF24_250KBPS);
  rf24_set_pa_level(&radio, RF24_PA_HIGH);
  rf24_open_writing_pipe(&radio, address);
  rf24_stop_listening(&radio);
  return TRUE;
}


void transmit() {
  rf24_power_up(&radio);
  uint8_t s[64];
  s[0] = 0;
  cs_to_ps_cpy(s, "TEST 0 | ");
  pstrcat(s, u16_to_ps(up_sec));
  cs_to_ps_cat(s, " | ");
  pstrcat(s, u16_to_ps(num_sent)); 
  pstrappend(s, '\n');

  console_pprint(&console, s);

  rf24_write(&radio, ps_to_cs(s), s[0], FALSE);
  rf24_power_down(&radio);
  ++num_sent;
}

void loop(void) {
  for (uint16_t i = 0;; ++i) {
	transmit();
	lowpower_powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
	up_sec += 4;
  }
}


int main(void) {
  sei();
  init_oled();
  if (!init_radio()) {
    fatal("RF24 Init Failed");
  }
  loop();
}
