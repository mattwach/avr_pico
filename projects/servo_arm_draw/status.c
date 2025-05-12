#include "status.h"

static struct Text* g_text;

void status_init(struct Text* text) {
    g_text = text;
}

// build up status one peice at a time
void status_start(const char* str) {
  g_text->row = 0;
  g_text->column = 0;
  text_str(g_text, str);
}

void status_start_on(uint8_t row, const char* str) {
  g_text->row = row;
  g_text->column = 0;
  text_str(g_text, str);
}

void status_start_on_pstr(uint8_t row, const uint8_t* pstr) {
  g_text->row = row;
  g_text->column = 0;
  text_pstr(g_text, pstr);
}

void status_str(const char* str) {
    text_str(g_text, str);
}

void status_char(char c) {
    text_char(g_text, c);
}

void status_pstr(const uint8_t* pstr) {
    text_pstr(g_text, pstr);
}

void status_end() {
    text_clear_row(g_text);
}

void status_column(uint8_t column) {
    g_text->column = column * 6;
}

const char* fresult_to_str(FRESULT result) {
  switch (result) {
    case FR_OK:
      return "OK";
    case FR_NOT_READY:
      return "NOT_READY";
    case FR_DISK_ERR:
      return "DISK_ERR";
    case FR_NO_FILESYSTEM:
      return "NO_FILESYSTEM";
    case FR_NOT_ENABLED:
      return "NOT_ENABLED";
    case FR_NO_FILE:
      return "NO_FILE";
    default:
      return "INTERNAL";
  }
}

FRESULT status_fresult(const char* str, FRESULT result) {
    status_start(str);
    status_str(": ");
    status_str(fresult_to_str(result));
    status_end();
    return result;
}

void status_msg(const char* str) {
    status_start(str);
    status_end();
}

void status_msg_on(uint8_t row, const char* str) {
    status_start_on(row, str);
    status_end();
}