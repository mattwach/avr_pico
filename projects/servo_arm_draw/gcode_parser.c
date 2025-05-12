// Parses gcode into x/y coordinates
#include "gcode_parser.h"

#include <string.h>
#include <avr/pgmspace.h>

//#define DEBUG
#ifdef DEBUG
#include <stdio.h>
#define DLOG(...) printf(__VA_ARGS__)
#else
#define DLOG(...)
#endif

//
// Error text
//

#define ERROR_STRING_LENGTH 8
static const char g_error_codes[] PROGMEM = (
  "OK      "  // No error
  "NumParse"  // ERR_NUMBER_PARSE
  "NumLarge"  // ERR_NUMBER_TOO_LARGE
  "FeedHigh"  // ERR_FEED_RATE_OUT_OF_RANGE
  "Token?  "  // ERR_UNEXPECTED_TOKEN
  "ScalHigh"  // ERR_SCALE_OUT_OF_RANGE
  "NumLong "  // ERR_NUMBER_TOO_LONG
  "BadCode "  // ERR_BAD_GCODE
  "MissCode"  // ERR_MISSING_GCODE
  "MissingN"  // ERR_MISSING_LINE_NUMBER
  "NSequenc"  // ERR_LINE_NUMBER_SEQUENCE
  "NTooHigh"  // ERR_LINE_NUMBER_TOO_LARGE
  "Data>EOF"  // ERR_DATA_AFTER_EOF
  "Chars?  "  // ERR_UNEXPECTED_CHARACTERS
  "Missing%"  // ERR_MISSING_START_OF_FILE_MARKER
);

//
// Global state
//

#define MAX_BUFF_SIZE 16

static bool_t (*g_state)(const char, struct GCodeParseData*);
static bool_t (*g_next_state)(const char, struct GCodeParseData*);
static char g_buff[MAX_BUFF_SIZE];  // Used to hold numbers for parsing
static uint8_t g_buff_size;

#define GCODE_MOVE 1
#define GCODE_SCALE 51
#define GCODE_ZERO 54

static uint8_t g_code;  // current gcode
static char g_current_token; // current token

static int16_t g_int_value; // Parsing a float -> int portion
static int16_t g_frac_value; // Parsing a flot -> fractional portion

static double g_x_origin; // x offset (default 0, set with G54)
static double g_y_origin; // y offset (default 0, set with G54)
static double g_scale; // scale (default 1, set with G51)

//
// forward declarations
//

static bool_t consume_comment(const char c, struct GCodeParseData* d);
static bool_t parse_next_token(const char c, struct GCodeParseData* d);
static bool_t start_of_line(const char c, struct GCodeParseData* d);

//
// Helper functions
//

// Parse an integer of fractional part of a float.
// If fractional, provide the fractional numbers only without
// any '.' character.
static int16_t atoi16(bool_t fractional, uint8_t* error) {
  bool_t negative = FALSE;
  int32_t n = 0;
  uint32_t power = 1;

  if (fractional) {
    if (g_buff_size > 4) {
      // Max precision is 4 digits, ignore the rest.
      g_buff_size = 4;
    }
    // Pad the fractional buffer to 4 digits
    // so we can always divide by 10000 to get the value.
    while (g_buff_size < 4) {
      g_buff[g_buff_size++] = '0';
    }
  }

  // Need to walk the buffer backwards (power starts at 1)
  for (int8_t i = (g_buff_size - 1); i >= 0; --i) {
    const char c = g_buff[i];
    if (c == '-') {
      if (fractional || i > 0) {
        *error = ERR_NUMBER_PARSE;
      }
      negative = TRUE;
    } else if (c < '0' || c > '9') {
      *error = ERR_NUMBER_PARSE;
    } else if (power >= 100000) {
      *error = ERR_NUMBER_TOO_LARGE;
    } else {
      n += (c - '0') * power;
      power *= 10;
    }
  }

  if (n > 0xFFFF) {  // uint16_t limits
    *error = ERR_NUMBER_TOO_LARGE;
  }

  if (negative) {
    n = -n;
  }
  return (int16_t)n;
}

static inline bool_t is_whitespace(const char c) {
  return c == ' ' || c == '\t' || c == '\r';
}

static inline bool_t is_cr(const char c) {
  return c == '\n';
}

// If it's a comment, set the state to consume the rest of the comment
static bool_t is_comment(
  const char c,
  bool_t (*next_state)(const char, struct GCodeParseData*)) {
  if (c != ';') {
    return FALSE;
  }
  g_next_state = next_state;
  g_state = consume_comment;
  return TRUE;
}

//
// State machine functions.  All must have the g_state function signature.
//

// We are in the middle of a comment
static bool_t consume_comment(const char c, struct GCodeParseData* d) {
  DLOG("consume_comment: '%c'\n", c);
  if (is_cr(c)) {
    g_state = g_next_state;
    DLOG("  gcode: %d\n", g_code);
    return g_code == GCODE_MOVE;
  }
  return FALSE;
}

// We have g_int_value, g_frac_value and g_current_token.
// Time to populate the appropriate field.
static bool_t process_tokens(const char c, struct GCodeParseData* d) {
  // Not a real state, so no log
  if (d->error) {
    return FALSE;
  }
  double val = (double)g_int_value;
  const double frac_val = (double)g_frac_value / 10000.0;
  
  if (val >= 0) {
   val += frac_val;
  } else {
   val -= frac_val;
  }

  switch (g_code) {
    case GCODE_MOVE:
      if (g_current_token == 'F') {
        if (val < 1 || val > 255) {
          d->error = ERR_FEED_RATE_OUT_OF_RANGE;
        } else {
          d->feed_rate = (uint8_t)val;
          DLOG("  d->feed_rate: %f\n", d->feed_rate);
        }
      } else if (g_current_token == 'X') {
        d->x = g_x_origin + (g_scale * val);
        DLOG("  d->x: %f\n", d->x);
      } else if (g_current_token == 'Y') {
        d->y = g_y_origin + (g_scale * val);
        DLOG("  d->y: %f\n", d->y);
      } else if (g_current_token == 'Z') {
        d->pen_is_down = val <= 0;
        DLOG("  d->pen_is_down: %d\n", d->pen_is_down);
      } else {
        d->error = ERR_UNEXPECTED_TOKEN;
      }
      break;
    case GCODE_ZERO:
      if (g_current_token == 'X') {
        g_x_origin = val;
        DLOG("  g_x_origin: %f\n", g_x_origin);
      } else if (g_current_token == 'Y') {
        g_y_origin = val;
        DLOG("  g_y_origin: %f\n", g_y_origin);
      } else {
        d->error = ERR_UNEXPECTED_TOKEN;
      }
      break;
    case GCODE_SCALE:
      if (g_current_token == 'P') {
        if (val <= 0.0) {
          d->error = ERR_SCALE_OUT_OF_RANGE;
        }
        g_scale = val;
        DLOG("  g_scale: %f\n", g_scale);
      } else {
        d->error = ERR_UNEXPECTED_TOKEN;
      }
      break;
  }

  if (is_comment(c, start_of_line)) {
    return FALSE;
  }

  if (is_cr(c)) {
    g_state = start_of_line;
    return (d->error == 0) && (g_code == GCODE_MOVE);
  }

  g_state = parse_next_token;
  return FALSE;
}

// Get the fractiona; portion of a value for F,P,X,Y,Z
static bool_t get_token_frac(const char c, struct GCodeParseData* d) {
  DLOG("get_token_frac: '%c'\n", c);
  if (is_whitespace(c) || is_cr(c) || c == ';') {
    g_frac_value = atoi16(TRUE, &(d->error));
    DLOG("  g_frac_value: %d\n", g_frac_value);
    return process_tokens(c, d);
  } else if (g_buff_size >= MAX_BUFF_SIZE) { 
    d->error = ERR_NUMBER_TOO_LONG;
  } else {
    g_buff[g_buff_size++] = c;
  }

  return FALSE;
}

// Get the integer portion of a value for F,P,X,Y,Z
static bool_t get_token_int(const char c, struct GCodeParseData* d) {
  DLOG("get_token_int: '%c'\n", c);
  if (is_whitespace(c) || is_cr(c) || c == ';' || c == '.') {
    if (!g_buff_size) {
      if (c != '.') {
        d->error = ERR_NUMBER_PARSE;
      }
    } else {
      g_int_value = atoi16(FALSE, &(d->error));
      DLOG("  g_int_value: %d\n", g_int_value);
    }
    if (c == '.') {
      g_buff_size = 0;
      g_state = get_token_frac;
    } else {
      return process_tokens(c, d);
    }
  } else if (g_buff_size >= MAX_BUFF_SIZE) { 
    d->error = ERR_NUMBER_TOO_LONG;
  } else {
    g_buff[g_buff_size++] = c;
  }

  return FALSE;
}

// Start the parse of a F,P,X,Y,Z token
static bool_t parse_next_token(const char c, struct GCodeParseData* d) {
  DLOG("parse_next_token: '%c'\n", c);
  if (is_cr(c)) {
    g_state = start_of_line;
    return g_code == GCODE_MOVE;
  }

  if (!is_whitespace(c) && !is_comment(c, start_of_line)) {
    g_current_token = c;
    g_buff_size = 0;
    g_int_value = 0;
    g_frac_value = 0;
    g_state = get_token_int;
  }

  return FALSE;
}

// Parse the Gcode value (e.g. NN of GNN)
static bool_t get_g_code(const char c, struct GCodeParseData* d) {
  DLOG("get_g_code: '%c'\n", c);
  if (is_whitespace(c) || is_cr(c)) {
    if (!g_buff_size) {
      d->error = ERR_BAD_GCODE;
      return FALSE;
    }

    int16_t n = atoi16(FALSE, &(d->error));
    if (d->error) {
      return FALSE;
    }

    switch (n) {
      case GCODE_MOVE:
      case GCODE_SCALE:
      case GCODE_ZERO:
        g_code = (uint8_t)n;
        g_current_token = 0;
        g_state = parse_next_token;
        DLOG("  g_code: %d\n", g_code);
        break;
      default:
        d->error = ERR_BAD_GCODE;
        break;
    }
  } else if (g_buff_size >= 2) { 
    d->error = ERR_BAD_GCODE;
  } else {
    g_buff[g_buff_size++] = c;
  }
  return FALSE;
}

// Looking for 'G'
static bool_t look_for_g(const char c, struct GCodeParseData* d) {
  DLOG("look_for_g: '%c'\n", c);
  if (!is_whitespace(c)) {
    if (c == 'G') {
      g_buff_size = 0;
      g_state = get_g_code;
    } else {
      d->error = ERR_MISSING_GCODE;
    }
  }
  return FALSE;
}

// Reading in a line number
static bool_t get_line_number(const char c, struct GCodeParseData* d) {
  DLOG("get_line_number: '%c'\n", c);
  if (is_whitespace(c)) {
    if (g_buff_size == 0) {
      d->error = ERR_MISSING_LINE_NUMBER;
      return FALSE;
    }

    int16_t n = atoi16(FALSE, &(d->error));
    if (d->error) {
      return FALSE;
    }
    
    if (n != (++d->line_number)) {
      d->error = ERR_LINE_NUMBER_SEQUENCE;
    } else {
      DLOG("  d->line_number: %d\n", d->line_number);
      g_state = look_for_g;
    }
  } else if (g_buff_size >= 5) { 
    d->error = ERR_LINE_NUMBER_TOO_LARGE;
  } else {
    g_buff[g_buff_size++] = c;
  }
  return FALSE;
}

// end of file
static bool_t end_of_file(const char c, struct GCodeParseData* d) {
  DLOG("end_of_file: '%c'\n", c);
  if (!is_whitespace(c) && !is_cr(c) && !is_comment(c, end_of_file)) {
    d->error = ERR_DATA_AFTER_EOF;
  }
  return FALSE;
}

// The start of a line
static bool_t start_of_line(const char c, struct GCodeParseData* d) {
  DLOG("start_of_line: '%c'\n", c);
  g_code = 0;
  if (!is_whitespace(c) && !is_cr(c) && !is_comment(c, start_of_line)) {
    if (c == '%') {
      d->eof = TRUE;
      g_state = end_of_file;
    } else if (c == 'N') {
      g_buff_size = 0;
      g_state = get_line_number;
    } else {
      d->error = ERR_MISSING_LINE_NUMBER;
    }
  }
  return FALSE;
}

// Eating up whitespace till we see a CR
static bool_t find_next_line(const char c, struct GCodeParseData* d) {
  DLOG("find_next_line: '%c'\n", c);
  if (!is_whitespace(c) && !is_comment(c, start_of_line)) {
    if (c == '\n') {
      g_state = start_of_line;
    } else {
      d->error = ERR_UNEXPECTED_CHARACTERS;
    }
  }
  return FALSE;
}

// we are at the very start of a file
static bool_t start_of_file(const char c, struct GCodeParseData* d) {
  DLOG("start_of_file: '%c'\n", c);
  if (!is_whitespace(c) && !is_cr(c) && !is_comment(c, start_of_file)) {
    if (c != '%') {
      d->error = ERR_MISSING_START_OF_FILE_MARKER;
    } else {
      g_state = find_next_line;
    }
  }
  return FALSE;
}

//
// Public functions
//

const char* gcode_error_str(uint8_t error) {
  const char* pgm_addr = g_error_codes + error * ERROR_STRING_LENGTH;
  char* dest = g_buff;
  for (uint8_t i=0; i<ERROR_STRING_LENGTH; ++i) {
    *(dest++) = pgm_read_byte(pgm_addr++);
  }
  *dest = '\0';
  return g_buff;
}

void gcode_parser_init(struct GCodeParseData* d) {
  memset(d, 0, sizeof(struct GCodeParseData));
  d->feed_rate = 10;
  g_state = start_of_file;
  g_buff_size = 0;
  g_code = 0;
  g_x_origin = 0.0;
  g_y_origin = 0.0;
  g_scale = 1.0;
}

bool_t gcode_parse(const char c, struct GCodeParseData* d) {
  if (d->error) {
    return FALSE;
  }
  return g_state(c, d);
}
