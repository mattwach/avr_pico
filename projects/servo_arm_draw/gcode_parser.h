#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include <error_codes.h>

// Parsing errors

#define ERR_NUMBER_PARSE 1
#define ERR_NUMBER_TOO_LARGE 2
#define ERR_FEED_RATE_OUT_OF_RANGE 3
#define ERR_UNEXPECTED_TOKEN 4
#define ERR_SCALE_OUT_OF_RANGE 5
#define ERR_NUMBER_TOO_LONG 6
#define ERR_BAD_GCODE 7
#define ERR_MISSING_GCODE 8
#define ERR_MISSING_LINE_NUMBER 9
#define ERR_LINE_NUMBER_SEQUENCE 10
#define ERR_LINE_NUMBER_TOO_LARGE 11
#define ERR_DATA_AFTER_EOF 12
#define ERR_UNEXPECTED_CHARACTERS 13
#define ERR_MISSING_START_OF_FILE_MARKER 14
#define MAX_ERROR_CODE 14

// Parses a gcode file into x,y,feed_rate,pen_down items.
//
// Th file data is fed in one character at a time.

struct GCodeParseData {
  double x;  // x of last parsed point
  double y;  // y of last parsed point
  uint8_t feed_rate;  // in mm/s
  bool_t pen_is_down;  // Are we drawing or just moving?
  bool_t eof; // End of file found
  uint32_t line_number;  // Last parsed line number
  uint8_t error;  // if != 0, there was an error
};

// Returns a string version of a given error
const char* gcode_error_str(uint8_t error);

// initialize state, including internal global state.
// Always call this when starting to parse a file, otherwise
// global state will not be correctly setup.
void gcode_parser_init(struct GCodeParseData* d);

// Parses a file, one character at a time
//
// Args:
//  c: the next character to parse
//  d: Pointer to data structure to fill
//
// Returns:
//   true, if d was updated
//
// d.error should be checked after every call to see if there
// are any errors to report to the user.
bool_t gcode_parse(const char c, struct GCodeParseData* d);

#endif
