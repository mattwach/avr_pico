// NRF2410 motion detector sender
// Transmits a series of packets when motion is discovered
//
// Protocol:
//
// MOVE -1    -> No motion has occured recently
// MOVE 0     -> Motion has just occured.
// MOVE 1..N  -> Motion states.  After motion, the code transists through all states
//
// Hardware connection:
//
//              +--------------------+
//         N/C -| PB5            VCC |- PWM_VCC
//              |                    |
//         CSN -| PB3  ATTiny85  PB2 |- SCK
//              |                    |
//  MOTION_PIN -| PB4            PB1 |- MISO
//              |                    |
//     PWM_GND -| GND            PB0 |- MOSI
//              +--------------------+
//
// The module I'm using
//
// .   GND .    VCC
// .   CE(VCC) .CSN
// .   SCLK .   MISO
// .   MOSI .   IRQ (N/C)
//
// BTW MOSI/MISO are swapped for some reason...

#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <rf24/rf24.h>

#define MOTION_PIN 4 

RF24 radio;

//// Radio address.  Transmitter and receiver need to match
const uint8_t address[6] = "M0T10N";

bool_t init_radio(void) {
  rf24_init(&radio, 3, 3); // CE (N/C), CSN
  if (!rf24_begin(&radio)) {
    return FALSE;
  }
  rf24_set_payload_size(&radio, 1);
  rf24_set_data_rate(&radio, RF24_250KBPS);
  rf24_set_pa_level(&radio, RF24_PA_HIGH);
  rf24_open_writing_pipe(&radio, address);
  rf24_stop_listening(&radio);
  return TRUE;
}


void transmit(uint8_t i) {
  rf24_power_up(&radio);
  char s = PINB & (1 << MOTION_PIN) ? 'T' : 'F'; 
  rf24_write(&radio, &s, 1, TRUE);
  rf24_power_down(&radio);
}

void loop(void) {
  for (uint16_t i = 0;; ++i) {
    transmit(i % 10);
    lowpower_powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
  }
}

int main(void) {
  DDRB &= ~(1 << MOTION_PIN);  // Set as input (I think its the default anyway)
  PORTB &= ~(1 << MOTION_PIN);  // disable pullup (asserting the default)
  while (!init_radio()) {
    lowpower_powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  loop();
}
