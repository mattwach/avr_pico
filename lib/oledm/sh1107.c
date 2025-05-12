#include "oledm.h"

#include <string.h>
#include <error_codes.h>

#include "oledm_driver_common.inc"

// Commented out commands are not used by this driver at this time.
enum ESH1106Commands
{
    SH1107_SETLOWCOLUMN     = 0x00,
    SH1107_SETHIGHCOLUMN    = 0x10,
    //SH1107_SETMEMORYADDRESS_PAGE = 0x20,
    //SH1107_SETMEMORYADDRESS_VERT = 0x21,
    //SH1106_DEACTIVATE_SCROLL = 0x2E,
    //SH1107_SETCONTRAST      = 0x81,
    //SH1107_DCDC_OFF         = 0x8A,
    //SH1107_DCDC_ON         = 0x8B,
    SH1107_SEGREMAP_DOWN     = 0xA0,
    SH1107_SEGREMAP_UP       = 0xA1,
    //SH1107_SETMULTIPLEX     = 0xA8,
    SH1107_DISPLAY_NORMAL   = 0xA4,
    //SH1107_DISPLAY_FORCE_ON = 0xA5,
    //SH1107_NORMALDISPLAY    = 0xA6,
    //SH1107_INVERTDISPLAY    = 0xA7,
    SH1107_DISPLAYOFF       = 0xAE,
    SH1107_DISPLAYON        = 0xAF,
    //SH1107_DCDC_MODE        = 0xAD,
    SH1107_SETPAGE          = 0xB0,
    //SH1107_COMSCANINC       = 0xC0,
    SH1107_COMSCANDEC       = 0xC8,
    //SH1107_SETDISPLAYOFFSET = 0xD3,
    //SH1107_SETDISPLAYCLOCKDIV = 0xD5,
    //SH1107_SETPRECHARGE     = 0xD9,
    //SH1107_SETSTARTLINE     = 0xDC,
    //SH1107_SETVCOMDETECT    = 0xDB,
    //SH1107_NOP              = 0xE3,
    //SH1107_READMODIFYWRITE  = 0xE0,
    //SH1107_END              = 0xEE,
};

// Globals.  The SH1107 does not have the nice horozontal addressing
// mode that mades display communication more efficient.  So this
// driver emulates it for the rest of the code base, which can then simply
// assume it exists.
// 
// The downside of this "transparent" implementation is that the resulting
// globals prevent multiple SH1107 from being used at the same time (well,
// it's possible if you take care not to set the window before doing ops).
//
// Another approach would be to unhide the implementation and move these vars
// into the OLEDM structure.  This would waste a few bytes of memory when using
// SSD1306.
//
// Note: It's legal for start > end just as it's supported in the hardware.
// When this is the case, we implicitly wrap at the memory boundary (as the
// hardware does)
column_t address_start_column;
column_t address_end_column;
uint8_t address_start_page;
uint8_t address_end_page;
column_t address_column;
uint8_t address_page;
bool_t address_wrap_triggered;

#define DISPLAY_MEMORY_ROWS 16
#define DISPLAY_MEMORY_COLUMNS 128
// % is expensive on AVR so let's use a heuristic that assumes we are not
// too far away.  It's reasonable since our types are uint8_t which can't be too
// far away.
#define WRAP_COLUMN(x) ((x) >= DISPLAY_MEMORY_COLUMNS ? \
                       (x) - DISPLAY_MEMORY_COLUMNS : (x))


static void _reset_globals(void) {
  address_start_column = 0;
  address_end_column = DISPLAY_MEMORY_COLUMNS - 1;
  address_start_page = 0;
  address_end_page = 0;
  address_column = 0;
  address_page = 0;
  address_wrap_triggered = FALSE;
}

void oledm_basic_init(struct OLEDM* display) {
  memset(display, 0, sizeof(struct OLEDM));
  //display->column_offset = 2;
  display->visible_columns = 128;
  display->visible_rows = DISPLAY_MEMORY_ROWS;
  display->memory_columns = DISPLAY_MEMORY_COLUMNS;
  display->memory_rows = DISPLAY_MEMORY_ROWS;
  //display->option_bits = OLEDM_ALTERNATE_COM_CONFIG;
  _reset_globals();
}

void oledm_start(struct OLEDM* display) {
  _reset_globals();
  oledm_ifaceInit();

  error_t* err = &(display->error);

  oledm_startCommands(err);
  oledm_command(SH1107_DISPLAYOFF, err);

  // Vertical scrolling may not be supported by the sh1107...
  //oledm_command(SH1107_SETSTARTLINE, err);
  //oledm_command(display->start_line, err);

  // Default: SH1107_SETCONTRAST, 0x7F

  // Default: SH1107_NORMALDISPLAY
  // Default: SH1107_SETMULTIPLEX, 63
  // Default: SH1107_SETDISPLAYOFFSET, 0x00
  // Default: SH1107_SETDISPLAYCLOCKDIV, 0x00
  // Default: SH1107_SETPRECHARGE, 0x22

  if (display->option_bits & OLEDM_ALTERNATE_COM_CONFIG) {
    oledm_command(SH1107_COMSCANDEC, err);
  }
  if (display->option_bits & OLEDM_COM_LEFT_RIGHT_REMAP) {
    oledm_command(SH1107_SEGREMAP_UP, err);
  }

  // Default: SH1107_SETVCOMDETECT, 0x20

  // Turn the display back on
  oledm_command(SH1107_DISPLAY_NORMAL, err);
  oledm_command(SH1107_DISPLAYON, err);
  oledm_stop(display);
}

void oledm_set_memory_bounds(
    struct OLEDM* display,
    column_t left_column,
    uint8_t top_row,
    column_t right_column,
    uint8_t bottom_row) {

  address_start_column = WRAP_COLUMN(left_column);
  address_end_column = WRAP_COLUMN(right_column);
  address_start_page = top_row & (DISPLAY_MEMORY_ROWS - 1);
  address_end_page = bottom_row & (DISPLAY_MEMORY_ROWS - 1);
  address_column = address_start_column;
  address_page = address_start_page;
  address_wrap_triggered = FALSE;

  error_t* err = &(display->error);

  oledm_startCommands(err);
  oledm_command(SH1107_SETPAGE | (address_page & 0x0F), err);
  oledm_command(SH1107_SETLOWCOLUMN | (address_column & 0x0F), err);
  oledm_command(SH1107_SETHIGHCOLUMN | ((address_column >> 4) & 0x0F), err);
  oledm_stop(display);
}

void oledm_write_pixels(struct OLEDM* display, uint8_t byte){ 
  if (address_wrap_triggered) {
    address_wrap_triggered = FALSE;

    // next row
    address_column = address_start_column;
    address_page = (address_page + 1) & (DISPLAY_MEMORY_ROWS - 1);

    if (address_page == ((address_end_page + 1) & (DISPLAY_MEMORY_ROWS - 1))) {
      address_page = address_start_page;
    }

    // tell the hardware
    error_t* err = &(display->error);
    oledm_stop(display); // <-- need to stop the data transfer
    oledm_startCommands(err);
    oledm_command(SH1107_SETPAGE | (address_page & 0x0F), err);
    oledm_command(SH1107_SETLOWCOLUMN | (address_column & 0x0F), err);
    oledm_command(SH1107_SETHIGHCOLUMN | ((address_column >> 4) & 0x0F), err);
    oledm_stop(display);
    oledm_start_pixels(display); // <-- resume data transfer
  }

  oledm_ifaceWriteData(byte, &(display->error));

  address_column = WRAP_COLUMN(address_column + 1);
  if (address_column == WRAP_COLUMN(address_end_column + 1)) {
    // We need to wrap for the next byte, but this could be the last byte
    // we'll see anyway so defer the page set until a bye is actually received.
    address_wrap_triggered = TRUE;
  }
}

void oledm_vscroll(struct OLEDM* display, int8_t rows) {
  // Even though there may be less than display->visible_rows visible on
  // a smaller display, the memory still supports 8 rows.  The logic
  // needs to respect this fact or you'll end up showing regions
  // that can not be updated.
  display->start_line = (display->start_line + rows) & (DISPLAY_MEMORY_ROWS - 1);

  // I have not figured out how to vertical scroll on the SH1107 and am not sure
  // its possible.  Need to look at the spec again.  The code below scrolls
  // by column instead of by row.
  //error_t* err = &(display->error);
  //oledm_startCommands(err);
  //oledm_command(SH1107_SETSTARTLINE, err);
  //oledm_command(display->start_line, err);
  //oledm_stop(display);
}

void oledm_clear(struct OLEDM* display, uint8_t byte) {
  if (display->error) {
    return;
  }

  error_t* err = &(display->error);
  uint8_t row = 0;
  for (; row < DISPLAY_MEMORY_ROWS; ++row) {
    oledm_startCommands(err);
    oledm_command(SH1107_SETPAGE | row, err);
    oledm_command(SH1107_SETLOWCOLUMN | ((DISPLAY_MEMORY_COLUMNS - 1) & 0x0F), err);
    oledm_command(SH1107_SETHIGHCOLUMN | ((DISPLAY_MEMORY_COLUMNS - 1) >> 4), err);
    oledm_stop(display);

    column_t col = 0;
    oledm_start_pixels(display);
    for (; col < DISPLAY_MEMORY_COLUMNS; ++col) {
      oledm_ifaceWriteData(byte, &(display->error));
    }
    oledm_stop(display);
  }
}

void oledm_display_off(struct OLEDM* display) {
  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SH1107_DISPLAYOFF, err);
  oledm_stop(display);
}

void oledm_display_on(struct OLEDM* display) {
  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SH1107_DISPLAYON, err);
  oledm_stop(display);
}
