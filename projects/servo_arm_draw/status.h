#ifndef STATUS_H
#define STATUS_H

// Provide a uinfied way to communicate status to the user

#include <oledm/text.h>
#include <petit_fatfs/pff.h>

void status_init(struct Text* text);

// build up status one peice at a time
void status_start(const char* str);
void status_start_on(uint8_t row, const char* str);
void status_start_on_pstr(uint8_t row, const uint8_t* pstr);
void status_char(char c);
void status_str(const char* str);
void status_pstr(const uint8_t* pstr);
void status_end();  // clear to the end of a line
void status_column(uint8_t column); // character, not pixel

// conversions
const char* fresult_to_str(FRESULT result);

// convienience
FRESULT status_fresult(const char* str, FRESULT result);
void status_msg(const char* str);
void status_msg_on(uint8_t row, const char* str);

#endif