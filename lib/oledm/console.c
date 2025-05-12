#include "console.h"

#include "avr_flash.h"
#include <string.h>

void console_init(struct Console* console, struct OLEDM* display, const void* vfont) {
  text_init(&(console->text), vfont, display);
  const struct FontASCII* font = (const struct FontASCII*)vfont;
  console->wrap = 1;
  text_verifyFont(&(console->text));
  if (!display->error) {
      console->text_width = display->visible_columns / pgm_read_byte_near(&font->width);
      console->text_height = pgm_read_byte_near(&font->height);
  } else {
      // just guess that the font width is 6.
      console->text_width = display->visible_columns / 6;
      console->text_height = 1;
  }
  console->line_pos = 0;
}

void console_print(struct Console* console, const char* msg) {
    console_printlen(console, msg, strlen(msg));
}

static void flush(struct Console* console) {
    console->text.column = 0;
#ifdef OLEDM_EMULATE_MONOCHROME
    const uint8_t visible_rows = console->text.display->visible_rows >> 3; // div by 8
#else
    const uint8_t visible_rows = console->text.display->visible_rows;
#endif
    console->text.row = visible_rows - console->text_height;
    oledm_vscroll(console->text.display, console->text_height);
    text_strLen(&(console->text), (const char*)console->line, console->text_width);
    console->line_pos = 0;
}

void console_printlen_no_cr(
    struct Console* console,
    const char* msg,
    uint8_t len) {
    while ((len + console->line_pos) >= console->text_width) {
        // fill the buffer and print the line
        const uint8_t num_chars = console->text_width - console->line_pos;
        memcpy(console->line + console->line_pos, msg, num_chars);
        flush(console);
        msg += num_chars;
        len -= num_chars;

        if (!console->wrap) {
            // just abort here, ignmoring any text that follows
            return;
        }
    }

    memcpy(console->line + console->line_pos, msg, len);
    console->line_pos += len;
}

void console_printlen(struct Console* console, const char* msg, uint8_t len) {
    // scan the line looking for a cr.  If one is found, print up to the CR,
    // then fill and print the line again
    const char* startmsg = msg;
    const char* endmsg = msg + len;
    for (; msg < endmsg; ++msg) {
        if (*msg == '\n') {
            console_printlen_no_cr(console, startmsg, msg - startmsg);
            if (console->line_pos > 0) {
                // fill the current line with spaces
                memset(
                    console->line + console->line_pos,
                    ' ',
                    console->text_width - console->line_pos);
                flush(console);
                // reset startmsg to after the CR
            }
            startmsg = msg + 1;
        }
    }
    // now that all the CRs are taken care of, print whatever is left
    console_printlen_no_cr(console, startmsg, endmsg - startmsg);
}
