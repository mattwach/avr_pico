#ifndef SSD1306_CONSOLE_H
#define SSD1306_CONSOLE_H

#include "oledm.h"
#include "text.h"
#include <inttypes.h>

// Console wrapper/helper for SSD1306.
//
// Usage:
//
// struct Console console;
// struct OLEDM display;
//
// void main(void) {
//    oledm_init(&display);
//    console_init(&console, &display, terminus6x8);
//    console_print(&console, "Hello World!\n");
//}

struct Console {
  struct Text text;
  uint8_t wrap;        // Defaults to true, clear to zero to prevent long text
                       // lines from wrapping.
  column_t text_width;  // Number of columns to use.  Autocalculated,
                       // but feel free to override if needed.
  uint8_t text_height;  // Number of rows that each line of text shouldtake.
                       // autocalculated but overridable.
  uint8_t line[32];    // current text line
  uint8_t line_pos;    // position on line
};

// Initialize console
void console_init(struct Console* console, struct OLEDM* display, const void* font);

// Prints a message
void console_print(struct Console* console, const char* msg);
void console_printlen(struct Console* console, const char* msg, uint8_t len);
// Print pascal string
static inline void console_pprint(struct Console* console, const uint8_t* pstr) {
    console_printlen(console, (const char*)(pstr + 1), *pstr);
}

#endif
