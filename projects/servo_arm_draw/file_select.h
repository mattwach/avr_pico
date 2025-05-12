#ifndef FILE_SELECT_H
#define FILE_SELECT_H

// Used to find/select files. 

#include <oledm/text.h>

#define MAX_FILES 32

void file_select_init(void);

#define FILE_SELECT_OPEN_ERROR -1
#define FILE_SELECT_FILE_TEST 0
#define FILE_SELECT_CALIBRATE_SERVOS 1
#define FILE_SELECT_PEN_UP 2
#define FILE_SELECT_PEN_DOWN 3
#define FILE_SELECT_FILE_OPEN 4
int8_t file_select(struct Text* text, uint32_t* file_size);

#endif