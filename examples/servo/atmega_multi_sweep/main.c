// Sweep 3 servos back and forth at 3 different speeds

#include <servo/servo_atmega.h>
#include <util/delay.h>

// All port D
#define S0_PIN 3  // D3
#define S1_PIN 4  // D4
#define S2_PIN 5  // D5

#define MAX_FREQ 2000
#define MIN_FREQ 1000

#define S0_SPEED 13
#define S1_SPEED 23
#define S2_SPEED 37


void advance(uint8_t slot, uint16_t* freq, int16_t *delta) {
  (*freq) += (*delta);
  if (*freq < MIN_FREQ || *freq > MAX_FREQ) {
    (*delta) = -(*delta);
    (*freq) += (*delta);
  }
  servo_set_period_us(slot, *freq);
}

int main(void) {
  servo_init_slot(0, 0, 0, 1 << S0_PIN, MIN_FREQ);
  servo_init_slot(1, 0, 0, 1 << S1_PIN, MIN_FREQ);
  servo_init_slot(2, 0, 0, 1 << S2_PIN, MIN_FREQ);
  servo_set_count(3);

  uint16_t s0 = MIN_FREQ;
  uint16_t s1 = MIN_FREQ;
  uint16_t s2 = MIN_FREQ;

  int16_t d0 = S0_SPEED;
  int16_t d1 = S1_SPEED;
  int16_t d2 = S2_SPEED;

  while (1) {
    advance(0, &s0, &d0);
    advance(1, &s1, &d1);
    advance(2, &s2, &d2);
    _delay_ms(20);
  }
}
