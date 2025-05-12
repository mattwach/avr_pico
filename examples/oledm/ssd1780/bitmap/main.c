#include <oledm/bitmap.h>
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <oledm/font/terminus6x8.h>
#include <util/delay.h>
#include <string.h>

#define DISPLAY_WIDTH 96
#define DISPLAY_ROWS 8
#define OLEDM_INIT oledm_basic_init
#define DISPLAY_HEIGHT (DISPLAY_ROWS << 3)

struct OLEDM display;
struct Bitmap bitmap;
uint8_t bitmap_data[DISPLAY_WIDTH * DISPLAY_ROWS];
#define FILL_BUFF_SIZE 128
uint16_t fill_buff[FILL_BUFF_SIZE];

#define FRAMES 300

struct Demo {
  const char* title;
  uint8_t clear_byte;
  void (*bit_op)(uint8_t* b, uint8_t p);
  void (*fn)(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)); 
};

static inline uint8_t centerx(const char* str) {
  return (DISPLAY_WIDTH - strlen(str) * 6) >> 1;
}

static inline uint8_t centery() {
  return ((DISPLAY_ROWS >> 1) << 3) - 1;
}

void points(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  int8_t j_dir = 1;
  uint8_t j = 5;
  uint16_t i=0;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();
  for (; i <FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    uint8_t y = 0;
    for (; y < DISPLAY_HEIGHT; y += j) {
      uint8_t x = 0;
      for (; x < DISPLAY_WIDTH; x += j) {
        bitmap_point(&bitmap, x, y, bit_op);
      }
    }
    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_XOR);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);

    j+=j_dir;
    if (j == 32 || j == 5) {
      j_dir = -j_dir;
    }
  }
}

void lines(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  int8_t j_dir = 1;
  uint8_t j = 5;
  uint16_t i=0;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();
  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    uint8_t y = 0;
    for (; y < DISPLAY_HEIGHT; y += j) {
      bitmap_hline(&bitmap, 0, y, DISPLAY_WIDTH, bit_op); 
    }

    uint8_t x = 0;
    for (; x < DISPLAY_WIDTH; x += j) {
      bitmap_vline(&bitmap, x, 0, DISPLAY_HEIGHT, bit_op); 
    }

    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);

    j+=j_dir;
    if (j == 32 || j == 5) {
      j_dir = -j_dir;
    }
  }
}

void lines2(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  uint16_t i=0;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();
  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    const uint8_t offset = i & 0xF;
    uint8_t x = offset;
    for (; x < DISPLAY_WIDTH; x += 16) {
      bitmap_line(
          &bitmap,
          x,
          0,
          DISPLAY_WIDTH - x - 1,
          DISPLAY_HEIGHT - 1,
          bit_op);
    }

    uint8_t y = x - DISPLAY_WIDTH;
    for (; y < DISPLAY_HEIGHT; y += 16) {
      bitmap_line(
          &bitmap,
          DISPLAY_WIDTH - 1,
          y,
          0,
          DISPLAY_HEIGHT - y - 1,
          bit_op);
    }

    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);
  }
}

void text(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  int8_t j_dir = 1;
  uint8_t j = 5;
  uint16_t i=0;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();
  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    uint8_t y = 0;
    uint8_t base = 'A';
    for (; y < DISPLAY_HEIGHT; y += j, ++base) {
      uint8_t x = 0;
      char c = base;
      for (; x < DISPLAY_WIDTH; x += j) {
        bitmap_strLen(&bitmap, terminus6x8, &c, 1, x, y, bit_op);
        ++c;
      }
    }

    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);

    j+=j_dir;
    if (j == 32 || j == 5) {
      j_dir = -j_dir;
    }
  }
}

void rect(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  uint16_t i=0;
  int8_t j_dir = 1;
  uint8_t j = 5;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();

  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    uint8_t offset = 0;
    for (; DISPLAY_HEIGHT > (offset * 2); offset += 5) {
      bitmap_rect(
          &bitmap,
          j + offset,
          j + offset,
          DISPLAY_WIDTH - offset * 2,
          DISPLAY_HEIGHT - offset * 2,
          bit_op
          );
    }

    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);

    j+=j_dir;
    if (j == DISPLAY_HEIGHT || j == 0) {
      j_dir = -j_dir;
    }
  }
}

void filled_rect(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  uint16_t i=0;
  int8_t j_dir = 1;
  uint8_t j = 5;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();

  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    uint8_t offset = 0;
    for (; DISPLAY_HEIGHT > (offset * 2); offset += 5) {
      bitmap_filled_rect(
          &bitmap,
          j + offset,
          j + offset,
          DISPLAY_WIDTH - offset * 2,
          DISPLAY_HEIGHT - offset * 2,
          bit_op
          );
    }

    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);

    j+=j_dir;
    if (j == DISPLAY_HEIGHT || j == 0) {
      j_dir = -j_dir;
    }
  }
}

void oval(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  uint16_t i=0;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();

  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    const uint8_t f = i & 0x3f;
    const uint8_t cx = DISPLAY_WIDTH >> 1;
    const uint8_t cy = DISPLAY_HEIGHT >> 1;
    bitmap_oval(&bitmap, cx, cy, f, 0x3f - f, bit_op);

    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);
  }
}

void filled_oval(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  uint16_t i=0;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();

  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    const uint8_t f = i & 0x3f;
    const uint8_t cx = DISPLAY_WIDTH >> 1;
    const uint8_t cy = DISPLAY_HEIGHT >> 1;
    bitmap_filled_oval(&bitmap, cx, cy, f, 0x3f - f, bit_op);

    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);
  }
}

void map_point(uint16_t p, uint8_t* x, uint8_t* y) {
  const uint16_t full = DISPLAY_WIDTH * 2 + DISPLAY_HEIGHT * 2;
  // normalize to the same screen
  while (p >= full) {
    p -= full;
  }

  if (p < DISPLAY_WIDTH) {
    // top
    *y = 0;
    *x = p;
    return;
  }

  if (p < (DISPLAY_WIDTH + DISPLAY_HEIGHT)) {
    // right
    *x = DISPLAY_WIDTH - 1;
    *y = p - DISPLAY_WIDTH;
    return;
  }

  if (p < (DISPLAY_WIDTH * 2 + DISPLAY_HEIGHT)) {
    // bottom
    *x = DISPLAY_WIDTH - (p - DISPLAY_WIDTH - DISPLAY_HEIGHT);
    *y = DISPLAY_HEIGHT - 1;
    return;
  }

  // left
  *x = 0;
  *y = DISPLAY_HEIGHT - (p - DISPLAY_WIDTH * 2 - DISPLAY_HEIGHT);
}

void flood_fill(const char* title, uint8_t clear_byte, void (*bit_op)(uint8_t* b, uint8_t p)) {
  uint16_t i=0;
  const uint8_t tx = centerx(title);
  const uint8_t ty = centery();

  for (; i < FRAMES; ++i) {
    bitmap_fill(&bitmap, clear_byte);

    // Three triangular points that trace around the screen
    uint16_t p0 = i;
    uint16_t p1 = i * 3 + DISPLAY_WIDTH + 3;
    uint16_t p2 = i * 4 + DISPLAY_WIDTH + DISPLAY_HEIGHT + DISPLAY_WIDTH / 2 + 7;

    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
    uint8_t x2;
    uint8_t y2;
    map_point(p0, &x0, &y0);
    map_point(p1, &x1, &y1);
    map_point(p2, &x2, &y2);
    bitmap_line(&bitmap, x0, y0, x1, y1, bit_op);
    bitmap_line(&bitmap, x1, y1, x2, y2, bit_op);
    bitmap_line(&bitmap, x2, y2, x0, y0, bit_op);

    // find the center of the triangle
    const uint8_t x01 = (x0 + x1) >> 1;
    const uint8_t y01 = (y0 + y1) >> 1;
    const uint8_t x02 = (x0 + x2) >> 1;
    const uint8_t y02 = (y0 + y2) >> 1;
    const uint8_t cx = (x01 + x02) >> 1;
    const uint8_t cy = (y01 + y02) >> 1;

    bitmap_flood_fill(&bitmap, cx, cy, TRUE, fill_buff, FILL_BUFF_SIZE);


    bitmap_str(&bitmap, terminus6x8, title, tx, ty, bitmap_SET);
    bitmap_render_fast(&display, &bitmap, 0, 0);
    _delay_ms(16);
  }
}

struct Demo demos[] = {
  {"Points", 0x00, bitmap_OR, points},
  {"Points", 0xFF, bitmap_NAND, points},
  {"Lines", 0x00, bitmap_OR, lines},
  {"Lines", 0xFF, bitmap_NAND, lines},
  {"Lines", 0x00, bitmap_OR, lines2},
  {"Lines", 0xFF, bitmap_NAND, lines2},
  {"Text", 0x00, bitmap_OR, text},
  {"Text", 0xFF, bitmap_NAND, text},
  {"Rects", 0x00, bitmap_OR, rect},
  {"Rects", 0xFF, bitmap_NAND, rect},
  {"Filled Rects", 0x00, bitmap_XOR, filled_rect},
  {"Filled Rects", 0xFF, bitmap_XOR, filled_rect},
  {"Oval", 0x00, bitmap_OR, oval},
  {"Oval", 0xFF, bitmap_NAND, oval},
  {"Filled Oval", 0x00, bitmap_XOR, filled_oval},
  {"Filled Oval", 0xFF, bitmap_XOR, filled_oval},
  {"Flood Fill", 0x00, bitmap_OR, flood_fill},
};

int main(void) {
  OLEDM_INIT(&display);
  oledm_start(&display);

  bitmap.rows = DISPLAY_ROWS;
  bitmap.columns = DISPLAY_WIDTH;
  bitmap.data = bitmap_data;
  uint8_t color = 0;

  while (1) {
    int i=0;
    for (; i < (sizeof(demos) / sizeof(demos[0])); ++i) {
      do {
        display.fg_color = rgb16_by_index(++color);
      } while (display.fg_color == 0); 
      struct Demo* demo = demos + i;
      demo->fn(demo->title, demo->clear_byte, demo->bit_op);
    }
  }
}
