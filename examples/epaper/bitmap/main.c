#include <oledm/epaper.h>
#include <oledm/bitmap.h>
#include <oledm/font/terminus8x16.h>
#include <lowpower/lowpower.h>

// 64x72 -> 576 bytes of memory that will fill a 296x128 display
// as a 5x2 grid, with purposeful overlap
#define YROWS 9
#define YSIZE (YROWS * 8)
#define XSIZE 64
#define XCOUNT 5
#define YCOUNT 2

struct OLEDM display;
struct Bitmap bitmap;

uint8_t bitmap_memory[XSIZE * YROWS];
uint16_t fill_queue[32];

static void init(void) {
  oledm_basic_init(&display);
//  display.option_bits |= OLEDM_ROTATE_180;
  oledm_start(&display);
  bitmap.columns = XSIZE;
  bitmap.rows = YROWS;
  bitmap.data = bitmap_memory;
  oledm_clear(&display, 0x00);
  display.option_bits |= OLEDM_WRITE_COLOR_RAM;
  oledm_clear(&display, 0x00);
  display.option_bits &= ~OLEDM_WRITE_COLOR_RAM;
  epaper_swap_buffers(&display, SLEEP_MODE_0);
  epaper_set_partial_mode(&display);
}

void fill(void) {
  bitmap_fill(&bitmap, 0x55);
}

void lines(void) {
  bitmap_line(&bitmap, 5, 5, XSIZE - 5, YSIZE - 5, bitmap_OR);
  bitmap_line(&bitmap, XSIZE - 5, 5, 5, YSIZE - 5, bitmap_OR);
}

void rect(void) {
  bitmap_rect(&bitmap, 5, 5, XSIZE - 10, YSIZE - 10, bitmap_OR);
}

void filled_rect(void) {
  bitmap_filled_rect(&bitmap, 5, 5, XSIZE - 10, YSIZE - 10, bitmap_OR);
}

void oval(void) {
  bitmap_oval(
      &bitmap, XSIZE / 2, YSIZE / 2, XSIZE / 2 - 5, YSIZE / 2 - 5, bitmap_OR);
}

void filled_oval(void) {
  bitmap_filled_oval(
      &bitmap, XSIZE / 2, YSIZE / 2, XSIZE / 2 - 5, YSIZE / 2 - 5, bitmap_OR);
}

void circle(void) {
  bitmap_circle(
      &bitmap, XSIZE / 2, YSIZE / 2, XSIZE / 3, bitmap_OR);
}

void filled_circle(void) {
  bitmap_filled_circle(
      &bitmap, XSIZE / 2, YSIZE / 2, XSIZE / 3, bitmap_OR);
}

void points(void) {
  for (uint8_t x = 5; x < XSIZE; x += 5) {
    for (uint8_t y = 5; y < YSIZE; y += 5) {
      bitmap_point(&bitmap, x, y, bitmap_OR);
    }
  }
}

void flood_fill(void) {
  bitmap_line(&bitmap, 5, 5, XSIZE - 5, YSIZE - 5, bitmap_OR);
  bitmap_line(&bitmap, XSIZE - 5, YSIZE - 5, 5, YSIZE - 5,bitmap_OR);
  bitmap_line(&bitmap, 5, YSIZE - 5, 5, 5, bitmap_OR);
  bitmap_flood_fill(
      &bitmap,
      XSIZE / 2 - 5,
      YSIZE / 2 + 5, 1,
      fill_queue,
      sizeof(fill_queue) / sizeof(fill_queue[0]));
}

void hello_world(void) {
  bitmap_str(&bitmap, terminus8x16, "Hello", 12, 10, bitmap_OR);
  bitmap_str(&bitmap, terminus8x16, "World!", 8, 30, bitmap_OR);
}


int main(void) {
  init();
  void (*draw_fn[])(void) = {
    fill,
    lines,
    rect,
    filled_rect,
    oval,
    filled_oval,
    circle,
    filled_circle,
    points,
    flood_fill,
    hello_world,
  };
  const uint8_t num_tests = sizeof(draw_fn) / sizeof(draw_fn[0]);
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t i = 0;
  while (1) {
    if ((x == 0) && (y==0)) {
      oledm_clear(&display, 0x00);
    }

    bitmap_fill(&bitmap, 0x00);
    draw_fn[i]();
    bitmap_render(&display, &bitmap, x * XSIZE, y * YROWS);
    //display.option_bits |= OLEDM_WRITE_COLOR_RAM;
    //bitmap_render(&display, &bitmap, x * XSIZE, y * YROWS);
    //display.option_bits &= ~OLEDM_WRITE_COLOR_RAM;

    ++i;
    if (i >= num_tests) {
      i = 0;
    }

    ++x;
    if (x >= XCOUNT) {
      x = 0;
      ++y;
      if (y >= YCOUNT) {
        y = 0;
        epaper_update_partial(&display, SLEEP_MODE_0);
        lowpower_powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
        epaper_set_partial_mode(&display);
        //oledm_start(&display);
      }
    }
  }
}
