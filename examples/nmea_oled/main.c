
#include <avr/io.h>
#include <lowpower/lowpower.h>
#include <oledm/font/terminus6x8.h>
#include <oledm/font/terminus16x32_numbers.h>
#include <oledm/oledm.h>
#include <oledm/text.h>
#include <oledm/ssd1306_init.h>
#include <nmea_decoder/nmea_decoder.h>
#include <uart/uart.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <port/port.h>
#include <pstr/pstr.h>

#include <string.h>

#define I2C_ADDRESS 0x40
#define UTC_OFFSET 8

struct DMS {
  uint8_t degrees;
  uint8_t minutes;
  float seconds;
  char direction;
  uint8_t is_valid;
};

struct State {
  struct DMS latitude;
  struct DMS longitude;
  uint8_t l_sats_in_view;
  uint8_t l_sats_in_use;
  uint8_t n_sats_in_view;
  uint8_t n_sats_in_use;
  uint8_t p_sats_in_view;
  uint8_t p_sats_in_use;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t time_is_valid;
  uint8_t day;
  uint8_t month;
  uint8_t year;
  uint8_t date_is_valid;
};

struct OLEDM display;
struct Text text_6x8;
struct Text text_16x32;
struct NMEA_decoder gps;
struct State state;


static void init() {
  port_set_all_input_pullup();
  memset(&state, 0, sizeof(state));
  nmea_init(&gps);
  _delay_ms(15);
  uart_init(9600, ENABLE_RX | ENABLE_RX_INTERRUPT);
  oledm_basic_init(&display);
  _delay_ms(15);
  text_init(&text_6x8, terminus6x8, &display);
  text_init(&text_16x32, terminus16x32_numbers, &display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);
  sei();
}

static void one_day_back(void) {
  if (!state.date_is_valid) {
    return;
  }

  --state.day;
  if (state.day > 0) {
    return;
  }

  --state.month;
  if (state.month == 0) {
    state.month = 12;
    if (state.year > 0) {
      --state.year;
    } else {
      state.year = 99;
    }
  } 

  switch (state.month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      state.day = 31;
      break;
    case 2:
      state.day = (state.year % 4 == 0) ? 29 : 28;
      break;
    default:
      state.day = 30;
      break;
  }
}

static void offset_for_utc(void) {
  if (!state.time_is_valid) {
    return;
  }

  if (UTC_OFFSET > state.hours) {
    state.hours += 24;
    one_day_back();
  }

  state.hours -= UTC_OFFSET;
}

static void parse_rmc(void) {
  gps.last_error = 0;
  nmea_rmc_time(&gps, &state.hours, &state.minutes, &state.seconds);
  state.time_is_valid = (gps.last_error == 0);

  gps.last_error = 0;
  nmea_rmc_date(&gps, &state.day, &state.month, &state.year);
  state.date_is_valid = (gps.last_error == 0);
  
  offset_for_utc();

  gps.last_error = 0;
  nmea_rmc_latitude_dms(
      &gps,
      &state.latitude.degrees,
      &state.latitude.minutes,
      &state.latitude.seconds,
      &state.latitude.direction);
  state.latitude.is_valid = (gps.last_error == 0);

  gps.last_error = 0;
  nmea_rmc_longitude_dms(
      &gps,
      &state.longitude.degrees,
      &state.longitude.minutes,
      &state.longitude.seconds,
      &state.longitude.direction);
  state.longitude.is_valid = (gps.last_error == 0);
}

static void update_sats(void) {
  text_6x8.row = 0;
  text_6x8.column = 0;
  text_str(&text_6x8, "Satellites: ");
  text_pstr(&text_6x8, u8_to_ps(
        state.l_sats_in_use + state.n_sats_in_use + state.p_sats_in_use));
  text_char(&text_6x8, '/');
  text_pstr(&text_6x8, u8_to_ps(
        state.l_sats_in_view + state.n_sats_in_view + state.p_sats_in_view));
  text_clear_row(&text_6x8);
}

static void update_time(void) {
  text_16x32.row = 1;
  text_16x32.column = 0;
  if (!state.time_is_valid) {
    text_str(&text_16x32, "  :  :  ");
    return;
  }

  text_char(&text_16x32, '0' + state.hours / 10);
  text_char(&text_16x32, '0' + state.hours % 10);
  text_char(&text_16x32, ':');
  text_char(&text_16x32, '0' + state.minutes / 10);
  text_char(&text_16x32, '0' + state.minutes % 10);
  text_char(&text_16x32, ':');
  text_char(&text_16x32, '0' + state.seconds / 10);
  text_char(&text_16x32, '0' + state.seconds % 10);
}

static void update_date(void) {
  // format: 2022/01/25 (10 characters)
  text_6x8.row = 5;
  text_6x8.column = 64;
  if (!state.date_is_valid) {
    text_str(&text_6x8, "    /  /  ");
    return;
  }

  text_str(&text_6x8, "20");
  text_char(&text_6x8, '0' + state.year / 10);
  text_char(&text_6x8, '0' + state.year % 10);
  text_char(&text_6x8, '/');
  text_char(&text_6x8, '0' + state.month / 10);
  text_char(&text_6x8, '0' + state.month % 10);
  text_char(&text_6x8, '/');
  text_char(&text_6x8, '0' + state.day / 10);
  text_char(&text_6x8, '0' + state.day % 10);
}

/*
static void make_degree_mark() {
  // // Character 34 (0x22) (")
  // --##--
  // -#- #-
  // -#- #-
  // --##--
  // ------
  // ------
  // ------
  // ------
  oledm_start_pixels(&display);
  oledm_write_pixels(&display, 0x00);
  oledm_write_pixels(&display, 0x06);
  oledm_write_pixels(&display, 0x09);
  oledm_write_pixels(&display, 0x09);
  oledm_write_pixels(&display, 0x06);
  oledm_write_pixels(&display, 0x00);
  text_6x8.column += 6;
  oledm_stop(&display);
}
*/

static void update_coord(uint8_t row, const struct DMS* coord) {
  text_6x8.row = row;
  text_6x8.column = 0;
  if (coord->is_valid) {
    if (coord->degrees < 100) {
      text_char(&text_6x8, ' ');
    }
    if (coord->degrees < 10) {
      text_char(&text_6x8, ' ');
    }
    text_pstr(&text_6x8, u8_to_ps(coord->degrees));
    //make_degree_mark();
    text_char(&text_6x8, ' ');

    if (coord->minutes < 10) {
      text_char(&text_6x8, ' ');
    }
    text_pstr(&text_6x8, u8_to_ps(coord->minutes));
    text_str(&text_6x8, "' ");

    uint8_t seconds = (uint8_t)coord->seconds;
    uint8_t seconds_frac = (uint8_t)(coord->seconds * 100) % 100;

    if (seconds < 10) {
      text_char(&text_6x8, ' ');
    }
    text_pstr(&text_6x8, u8_to_ps(seconds));
    text_char(&text_6x8, '.');
    text_char(&text_6x8, '0' + seconds_frac / 10);
    text_char(&text_6x8, '0' + seconds_frac % 10);
    text_str(&text_6x8, "\" ");

    text_char(&text_6x8, coord->direction);
  }
  text_clear_row(&text_6x8);
}

static void update_display(void) {
  update_sats();
  update_time(); 
  update_date();
  update_coord(6, &state.latitude);
  update_coord(7, &state.longitude);
}

static void update_state(void) {
  uint8_t changed = 0;
  for (; gps.ready; nmea_command_done(&gps)) {
    if (nmea_is_gga(&gps)) {
      switch (nmea_parse_sat_type(&gps)) {
        case 'L':
          state.l_sats_in_use = nmea_gga_num_satellites_in_use(&gps); 
          break;
        case 'N':
          state.n_sats_in_use = nmea_gga_num_satellites_in_use(&gps); 
          break;
        case 'P':
          state.p_sats_in_use = nmea_gga_num_satellites_in_use(&gps); 
          break;
        default:
          break;
      }
      changed = 1;
    } else if (nmea_is_gsv(&gps)) {
      switch (nmea_parse_sat_type(&gps)) {
        case 'L':
          state.l_sats_in_view = nmea_gsv_num_sats(&gps);
          break;
        case 'N':
          state.n_sats_in_view = nmea_gsv_num_sats(&gps);
          break;
        case 'P':
          state.p_sats_in_view = nmea_gsv_num_sats(&gps);
          break;
        default:
          break;
      }
      changed = 1;
    } else if (nmea_is_rmc(&gps)) {
      parse_rmc();
      changed = 1;
    }
  }

  if (changed) {
    update_display();
  }
}

int main(void) {
  init();

  while (1) {
    lowpower_idle(
        SLEEP_8S,
        ADC_OFF,
        TIMER2_OFF,
        TIMER1_OFF,
        TIMER0_OFF,
        SPI_OFF,
        USART0_ON,
        TWI_OFF);
    _delay_ms(50);
    update_state();
  }
}

ISR(USART_RX_vect) {
  uint8_t byte = 0;
  while (read_no_block(&byte)) {
    nmea_encode(&gps, (char)byte);
  }
}
