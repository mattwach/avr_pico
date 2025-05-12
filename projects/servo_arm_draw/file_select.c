
#include "file_select.h"

#include <util/delay.h>

#include "rotary_encoder.h"
#include "status.h"

#include <error_codes.h>
#include <petit_fatfs/diskio.h>
#include <petit_fatfs/pff.h>
#include <servo/servo_atmega.h>

static FATFS g_fs;
static DIR g_dir;
static FILINFO g_fno;
static const char* g_load_stage;
static FRESULT g_load_result = FR_NOT_READY;
static bool_t g_test_mode;

#define MAX_NAME_SIZE 9
static char g_file_names[MAX_NAME_SIZE * MAX_FILES];
static uint32_t g_file_sizes[MAX_FILES];
static uint8_t g_num_files;

#define LINE_WIDTH 15
static char buff[LINE_WIDTH + 1];
static const char suffix[] = ".GCD";

static FRESULT fr_check(const char* load_stage, FRESULT load_result) {
    g_load_stage = load_stage;
    g_load_result = load_result;
    return load_result;
}

bool_t is_gcd_file(void) {
    if (g_fno.fattrib & (AM_HID | AM_SYS | AM_DIR)) {
        return FALSE;
    }

    uint8_t i = 0;
    for (; i < 13 && g_fno.fname[i]; ++i);
    if (i < 6 || i > 12) {
        return FALSE;
    }

    const char* src = g_fno.fname + i - 4; 
    const char* compare = suffix;

    for (; *src; ++src, ++compare) {
        if (*src != *compare) {
            return FALSE;
        }
    }

    return TRUE;
}

void file_select_init(void) {
  g_num_files = 0;
  g_test_mode = TRUE;

  if (fr_check("disk_init", disk_initialize() == 0 ? FR_OK : FR_DISK_ERR)) {
      return;
  }

  if (fr_check("sd_mount", pf_mount(&g_fs))) {
      return;
  }

  if (fr_check("opendir", pf_opendir(&g_dir, ""))) {
      return;
  }

  while (g_num_files < MAX_FILES) {
      if (fr_check("readdir", pf_readdir(&g_dir, &g_fno))) {
          return;
      }

      if (g_fno.fname[0] == '\0') {
          // done reading files
          break;
      }

      if (!is_gcd_file()) {
          continue;
      }

      g_file_sizes[g_num_files] = g_fno.fsize;

      char* src = g_fno.fname;
      char* dest = g_file_names + (g_num_files * MAX_NAME_SIZE);
      for (; *src != '.'; ++src, ++dest) {
          *dest = *src;
      }
      ++dest;
      *dest = '\0'; 
      ++g_num_files;
  }
}

static char* strcpy(char* dest, const char* src) {
    for (; *src; ++src, ++dest) {
        *dest = *src;
    }
    *dest = '\0';
    return dest;
}

// draw_str should always return 15 characters
static const char* draw_str(uint8_t draw_idx) {
    const char* draw_str = g_test_mode ? "TEST " : "DRAW ";

    char* dest = buff;
    const char* src = draw_str;

    switch (draw_idx) {
        case FILE_SELECT_FILE_TEST:
          src = g_test_mode ? "DRAW MODE" : "TEST MODE";
          break;
        case FILE_SELECT_CALIBRATE_SERVOS:
          src = "CALIBRATE";
          break;
        case FILE_SELECT_PEN_UP:
          src = "PEN UP";
          break;
        case FILE_SELECT_PEN_DOWN:
          src = "PEN DOWN";
          break;
        default:
          dest = strcpy(buff, src);
          src = g_file_names +
          (MAX_NAME_SIZE * (draw_idx - FILE_SELECT_FILE_OPEN));
          break;
    }

    dest = strcpy(dest, src);

    // pad with spaces
    for (; dest - buff < LINE_WIDTH; ++dest) {
        *dest = ' ';
    }
    *dest = '\0';  // probably not needed, due to static init.

    return buff;
}

#define SELECTION_ROW 2
static void draw_entry(struct Text* text, int8_t draw_idx) {
  text->column = 0;
  text_str(text, text->row == SELECTION_ROW ? "-> " : "   ");
  text_str(text, draw_str(draw_idx));
  text_str(text, text->row == SELECTION_ROW ? "<- " : "   ");
  ++text->row;
}

static int8_t draw_list(struct Text* text, uint8_t max_index) {
    text->row = 0;
    text->column = 0;

    if (g_load_result) {
        // There was a load error.  Report the error
        text_str(text, g_load_stage);
        text_str(text, ": ");
        text_str(text, fresult_to_str(g_load_result));
        text_clear_row(text);
        ++text->row;
    }

    // The draw strategy is to always draw the current selection
    // on SELECTION_ROW 
    const int8_t select_idx = (int8_t)rotary_value();
    int8_t draw_idx = select_idx - SELECTION_ROW;
    for (; draw_idx < 0 && text->row < SELECTION_ROW; ++draw_idx) {
        text->column = 0;
        text_clear_row(text);
        ++text->row;
    }

    for (; draw_idx <= max_index && text->row < 8; ++draw_idx) {
        draw_entry(text, draw_idx);
    }

    for (; text->row < 8; ++text->row) {
      text->column = 0;
      text_clear_row(text);
    }

    return select_idx;
}

int8_t file_select(struct Text* text, uint32_t* file_size) {
    _delay_ms(150);  // Give any moving servos time to settle
    servo_set_count(0);  // Disable servos if they are active
    rotary_use_interrupts(TRUE);
    bool_t toggle = rotary_button_toggle();
    const uint8_t max_index = FILE_SELECT_FILE_OPEN + g_num_files - 1;
    rotary_set(0, max_index, 0);
    int8_t select_idx = 0;

    while (TRUE) {
        if (toggle != rotary_button_toggle()) {
          toggle = !toggle;
          if (select_idx == FILE_SELECT_FILE_TEST) {
              g_test_mode = !g_test_mode;
          } else {
            break;
          }
        }
        select_idx = draw_list(text, max_index);
        _delay_ms(16);
    }

    if (select_idx >= FILE_SELECT_FILE_OPEN) {
        const uint8_t file_index = select_idx - FILE_SELECT_FILE_OPEN;
        *file_size = g_file_sizes[file_index];

        // Open the file
        char* dest = strcpy(
            buff,
            g_file_names +
            MAX_NAME_SIZE * file_index);
        strcpy(dest, suffix);
        if (pf_open(buff) != FR_OK) {
          select_idx = FILE_SELECT_OPEN_ERROR;
        } else {
          select_idx = g_test_mode ? FILE_SELECT_FILE_TEST : FILE_SELECT_FILE_OPEN;
        }
    }

    rotary_use_interrupts(FALSE);
    return select_idx;
}
