// Uses a pot + 12c powered 64x32 display to control a servo.
// You can change the display to 128x64 with a small tweak to the
// code below...
//
// Wiring
//
//         +--------+
// N/C    -|        |- VDD
// N/C    -| Tiny85 |- N/C
// N/C    -|        |- Servo PWM
// VSS    -|        |- N/C
//         +--------+

#include <avr/io.h>
#include <servo/servo_attiny.h>
#include <util/delay.h>

#define SERVO_PIN PB1
#define MIN_PWM 800
#define MAX_PWM 2200
#define PWM_STEP 200
#define PWM_DELAY_MS 5000

int16_t pwm;
int16_t pwm_step;

void update_servo() {

  servo_set(SERVO_PIN, pwm);

  pwm += pwm_step;
  if (pwm >= MAX_PWM) {
    pwm = MAX_PWM;
    pwm_step = -pwm_step;
  } else if (pwm <= MIN_PWM) {
    pwm = MIN_PWM;
    pwm_step = -pwm_step;
  }
}

int main(void) {
  pwm = MIN_PWM;
  pwm_step = PWM_STEP;
  while (1) {
    update_servo();
    _delay_ms(PWM_DELAY_MS); // 100 updates / second maximum
  }
}
