// Some example code that shows how to do 10 bit PWM with ATMega238
// Helpful site: https://wolles-elektronikkiste.de/en/timer-and-pwm-part-2-16-bit-timer1

#include <adc/adc.h>
#include <lowpower/lowpower.h>
#include <util/delay.h>

//#define REVERSE_POT
#define ADC_CHANNEL 0
#define ADC_DEAD_ZONE 100  // bottom 10%
#define MAX_ADC_VALUE (1023 - ADC_DEAD_ZONE)

const uint16_t max_pwm = ((uint32_t)F_CPU / 400);  // Updates will be around 400Hz

static void turn_off_pwm() {
  if ((TCCR1B & 7) == 0) {
    // Already off
    return;
  }
  TCCR1A = 0x00;  
  TCCR1B = 0x00;  // Turn off timer and disconnect outputs
}

static void turn_on_pwm() {
  if (TCCR1B & 7) {
    // Already on
    return;
  }
  OCR1A = 0;
  TCCR1A = 0x82;  // COM1A1 - Clear OC1A on compare match
                  // WGM11 - PWM mode with ICR1 as top
  TCCR1B = 0x19;  // WGM13, WGM12 - PWM Mode with ICR1 on top
                  // CS10 - Full speed for now
  ICR1 = max_pwm;  // maximum count value (1023?)
}

static void update() {
  uint16_t adc_value = adc_read16(
      ADC_CHANNEL,
      ADC_REF_VCC,
      ADC_PRESCALER_128);
#ifdef REVERSE_POT
  adc_value = 1023 - adc_value;
#endif

  if (adc_value <= ADC_DEAD_ZONE) {
    turn_off_pwm();
    return;
  }
  adc_value -= ADC_DEAD_ZONE;

  turn_on_pwm();
  OCR1A = (uint16_t)((uint32_t)max_pwm * (uint32_t)adc_value / MAX_ADC_VALUE);
}

int main() {
  DDRB |= (1 << 1);  // Set PB1 as an output
  while (1) {
    update();
    lowpower_idle(
        SLEEP_15MS,
        ADC_ON,
        TIMER2_OFF,
        TIMER1_ON,
        TIMER0_OFF,
        SPI_OFF,
        USART0_OFF,
        TWI_OFF); 
  }
}
