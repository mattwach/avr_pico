#ifndef GCODE_FILE_H
#define GCODE_FILE_H

#include <petit_fatfs/pff.h>
#include <error_codes.h>

// Opens gcode.txt, checks for integrity
FRESULT gcode_file_check(uint32_t file_size);
void gcode_file_draw(uint32_t file_size, bool_t check_mode);

#endif