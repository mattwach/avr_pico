#include "bitmap.h"

#include <test/unit_test.h>

// OLEDM fakes to reduce the depth of the unit tests
// these just keep a simple log
uint8_t oledm_log[16384];
uint8_t oledm_idx;
uint8_t bitmap_data[16];
struct OLEDM display;
struct Bitmap bitmap;

#define SET_BOUNDS 0xBB
#define WRITE_DATA 0xCC
#define START_DATA 0xDD
#define STOP 0xEE

static struct FontASCII font = {
    .id = { 'F', 'A', 'S', '1' },
    .first_char = 'H',
    .last_char = 'I',
    .width = 3,
    .height = 2,
    .data = {
      0xFF, 0x00, 0xFF,  // H
      0xFF, 0x01, 0xFF,
      0x01, 0xFF, 0x01,  // I
      0x80, 0xFF, 0x80,
    },
};

void init() {
  memset(&display, 0, sizeof(display));
  display.memory_rows = 2;
  display.memory_columns = 3;
  display.visible_rows = 2;
  display.visible_columns = 3;
  bitmap.columns = 3;
  bitmap.rows = 2;
  bitmap.data = bitmap_data;
  memset(bitmap_data, 0xDC, sizeof(bitmap_data));
  memset(oledm_log, 0, sizeof(oledm_log));
  oledm_idx = 0;
}

void oledm_start_pixels(struct OLEDM* display) {
  oledm_log[oledm_idx++] = START_DATA;
}

void oledm_stop(struct OLEDM* display) {
  oledm_log[oledm_idx++] = STOP;
}

void oledm_set_bounds(struct OLEDM* display,
  uint8_t left_column, uint8_t top_row,
  uint8_t right_column, uint8_t bottom_row) {
  oledm_log[oledm_idx++] = SET_BOUNDS;
  oledm_log[oledm_idx++] = left_column;
  oledm_log[oledm_idx++] = top_row;
  oledm_log[oledm_idx++] = right_column;
  oledm_log[oledm_idx++] = bottom_row;
}

void oledm_write_pixels(struct OLEDM* display, uint8_t byte) {
  oledm_log[oledm_idx++] = WRITE_DATA;
  oledm_log[oledm_idx++] = byte;
}

void test_OR(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0x00,
				0x00, 0x02, 0x02,
				0x00, 0xFF, 0xFF,

				0x02, 0x00, 0x02,
				0x02, 0x02, 0x02,
				0x02, 0xFF, 0xFF,

				0xFF, 0x00, 0xFF,
				0xFF, 0x02, 0xFF,
				0xFF, 0xFF, 0xFF,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_OR(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_AND(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0x00,
				0x00, 0x02, 0x00,
				0x00, 0xFF, 0x00,

				0x02, 0x00, 0x00,
				0x02, 0x02, 0x02,
				0x02, 0xFF, 0x02,

				0xFF, 0x00, 0x00,
				0xFF, 0x02, 0x02,
				0xFF, 0xFF, 0xFF,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_AND(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_SET(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0x00,
				0x00, 0x02, 0x02,
				0x00, 0xFF, 0xFF,

				0x02, 0x00, 0x00,
				0x02, 0x02, 0x02,
				0x02, 0xFF, 0xFF,

				0xFF, 0x00, 0x00,
				0xFF, 0x02, 0x02,
				0xFF, 0xFF, 0xFF,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_SET(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_NSET(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0xFF,
				0x00, 0x02, 0xFD,
				0x00, 0xFF, 0x00,

				0x02, 0x00, 0xFF,
				0x02, 0x02, 0xFD,
				0x02, 0xFF, 0x00,

				0xFF, 0x00, 0xFF,
				0xFF, 0x02, 0xFD,
				0xFF, 0xFF, 0x00,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_NSET(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_NAND(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0x00,
				0x00, 0x02, 0x00,
				0x00, 0xFF, 0x00,

				0x02, 0x00, 0x02,
				0x02, 0x02, 0x00,
				0x02, 0xFF, 0x00,

				0xFF, 0x00, 0xFF,
				0xFF, 0x02, 0xFD,
				0xFF, 0xFF, 0x00,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_NAND(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_NOR(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0xFF,
				0x00, 0x02, 0xFD,
				0x00, 0xFF, 0x00,

				0x02, 0x00, 0xFF,
				0x02, 0x02, 0xFF,
				0x02, 0xFF, 0x02,

				0xFF, 0x00, 0xFF,
				0xFF, 0x02, 0xFF,
				0xFF, 0xFF, 0xFF,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_NOR(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_XOR(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0x00,
				0x00, 0x02, 0x02,
				0x00, 0xFF, 0xFF,

				0x02, 0x00, 0x02,
				0x02, 0x02, 0x00,
				0x02, 0xFF, 0xFD,

				0xFF, 0x00, 0xFF,
				0xFF, 0x02, 0xFD,
				0xFF, 0xFF, 0x00,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_XOR(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_XNOR(void) {
		// b, p, result
		uint8_t data[] = {
				0x00, 0x00, 0xFF,
				0x00, 0x02, 0xFD,
				0x00, 0xFF, 0x00,

				0x02, 0x00, 0xFD,
				0x02, 0x02, 0xFF,
				0x02, 0xFF, 0x02,

				0xFF, 0x00, 0x00,
				0xFF, 0x02, 0x02,
				0xFF, 0xFF, 0xFF,
		};

		int i=0;
		for (; i < sizeof(data); i+=3) {
				uint8_t result = data[i];
				bitmap_XNOR(&result, data[i+1]);
				assert_int_equal(data[i+2], result);
		}
}

void test_render_fast(void) {
  init();
  bitmap_render_fast(&display, &bitmap, 0, 0);
  assert_buff_equal(
    ((uint8_t[]){
      SET_BOUNDS, 0, 0, 2, 1, // inclusive
      START_DATA,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      STOP
    }),
    oledm_log,
    oledm_idx
  );
}

void test_render_fast_offset(void) {
  init();
  bitmap_render_fast(&display, &bitmap, 1, 2);
  assert_buff_equal(
    ((uint8_t[]){
      SET_BOUNDS, 1, 2, 3, 3,
      START_DATA,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      STOP
    }),
    oledm_log,
    oledm_idx
  );
}

void test_render(void) {
  init();
  bitmap_render(&display, &bitmap, 0, 0);
  assert_buff_equal(
    ((uint8_t[]){
      SET_BOUNDS, 0, 0, 3, 0,
      START_DATA,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      STOP,
      SET_BOUNDS, 0, 1, 3, 1,
      START_DATA,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      STOP
    }),
    oledm_log,
    oledm_idx
  );
}

void test_render_offsetx(void) {
  init();
  bitmap_render(&display, &bitmap, 1, 0);
  assert_buff_equal(
    ((uint8_t[]){
      SET_BOUNDS, 1, 0, 3, 0,
      START_DATA,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      STOP,
      SET_BOUNDS, 1, 1, 3, 1,
      START_DATA,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      STOP
    }),
    oledm_log,
    oledm_idx
  );
}


void test_render_offsety(void) {
  init();
  bitmap_render(&display, &bitmap, 0, 1);
  assert_buff_equal(
    ((uint8_t[]){
      SET_BOUNDS, 0, 1, 3, 1,
      START_DATA,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      WRITE_DATA, 0xDC,
      STOP,
    }),
    oledm_log,
    oledm_idx
  );
}

void test_fill(void) {
  init();
  bitmap_fill(&bitmap, 0x5E);
  assert_buff_equal(
    ((uint8_t[]){0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_00(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_00_NAND(void) {
  init();
  bitmap_fill(&bitmap, 0xFF);
  bitmap_point(&bitmap, 0, 0, bitmap_NAND);

  assert_buff_equal(
    ((uint8_t[]){0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_01(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_0F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 0, 15, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_0X(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 0, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_20(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 2, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_21(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 2, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_2F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 2, 15, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_2X(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 2, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_point_30(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 3, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_is_set_true(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_point(&bitmap, 0, 0, bitmap_OR);
  bitmap_point(&bitmap, 0, 15, bitmap_OR);
  bitmap_point(&bitmap, 2, 0, bitmap_OR);
  bitmap_point(&bitmap, 2, 15, bitmap_OR);

  assert_int_equal(TRUE, bitmap_is_set(&bitmap, 0, 0) != 0);
  assert_int_equal(TRUE, bitmap_is_set(&bitmap, 0, 15) != 0);
  assert_int_equal(TRUE, bitmap_is_set(&bitmap, 2, 0) != 0);
  assert_int_equal(TRUE, bitmap_is_set(&bitmap, 2, 15) != 0);
}

void test_is_set_false(void) {
  init();
  bitmap_fill(&bitmap, 0x00);

  assert_int_equal(FALSE, bitmap_is_set(&bitmap, 0, 0) != 0);
  assert_int_equal(FALSE, bitmap_is_set(&bitmap, 0, 15) != 0);
  assert_int_equal(FALSE, bitmap_is_set(&bitmap, 2, 0) != 0);
  assert_int_equal(FALSE, bitmap_is_set(&bitmap, 2, 15) != 0);
}

void test_is_set_oob(void) {
  init();
  bitmap_fill(&bitmap, 0xFF);

  assert_int_equal(FALSE, bitmap_is_set(&bitmap, 3, 0) != 0);
  assert_int_equal(FALSE, bitmap_is_set(&bitmap, 0, 16) != 0);
  assert_int_equal(FALSE, bitmap_is_set(&bitmap, 3, 16) != 0);
}

void test_byte_00(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_byte(&bitmap, 0, 0, 0xA5, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_byte_02(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_byte(&bitmap, 0, 2, 0xA5, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x94, 0x00, 0x00, 0x02, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_byte_02_NAND(void) {
  init();
  bitmap_fill(&bitmap, 0xFF);
  bitmap_byte(&bitmap, 0, 2, 0xA5, bitmap_NAND);

  assert_buff_equal(
   ((uint8_t[]){0x6B, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_byte_0A(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_byte(&bitmap, 0, 10, 0xA5, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x94, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_byte_20(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_byte(&bitmap, 2, 0, 0xA5, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0xA5, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_byte_22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_byte(&bitmap, 2, 2, 0xA5, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x94, 0x00, 0x00, 0x02, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_001(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 0, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_001_NAND(void) {
  init();
  bitmap_fill(&bitmap, 0xFF);
  bitmap_hline(&bitmap, 0, 0, 1, bitmap_NAND);

  assert_buff_equal(
    ((uint8_t[]){0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_0X1(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 0, 16, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_301(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 3, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_002(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 0, 0, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_104(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 1, 0, 4, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_2F1(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 2, 15, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_0F2(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 0, 15, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_hline_1F4(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_hline(&bitmap, 1, 15, 4, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_000(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 0, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_001(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 0, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_001_NAND(void) {
  init();
  bitmap_fill(&bitmap, 0xFF);
  bitmap_vline(&bitmap, 0, 0, 1, bitmap_NAND);

  assert_buff_equal(
    ((uint8_t[]){0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_008(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 0, 0, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_028(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 0, 2, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFC, 0x00, 0x00, 0x03, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_088(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 0, 8, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_0A8(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 0, 10, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_0X8(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 0, 16, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_200(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 2, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_201(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 2, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_208(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 2, 0, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_228(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 2, 2, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0xFC, 0x00, 0x00, 0x03, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_288(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 2, 8, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_2A8(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 2, 10, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_2X8(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 2, 16, 8, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_vline_301(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_vline(&bitmap, 3, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_0000(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 0, 0, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_000F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 0, 0, 0, 15, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_001F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 0, 0, 1, 15, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0x00, 0x00, 0x7F, 0x80, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_0030(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 0, 0, 3, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_0033(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 0, 0, 3, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_0F0F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 0, 15, 0, 15, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_0X0X(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 3, 0, 3, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_2020(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 2, 0, 2, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_2F2F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 2, 15, 2, 15, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_3030(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 3, 0, 3, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_3300(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 3, 3, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_line_3X3X(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_line(&bitmap, 3, 16, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_0000(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 0, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_0003(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 0, 0, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_0030(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 0, 0, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_0011(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_1011(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_0111(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 1, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_1121(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 1, 2, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_0131(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 1, 3, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_1112(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 1, 1, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_1113(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 1, 1, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_1122(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 1, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x06, 0x06, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_003F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 0, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_013F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 1, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_0F3F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 0, 15, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_103F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 0, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_203F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 2, 0, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_113F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 1, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0xFE, 0xFE, 0x00, 0xFF, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_1E22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 14, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_1F22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 1, 15, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_2E22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 2, 14, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_filled_rect_2F22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_rect(&bitmap, 2, 15, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_0000(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 0, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_0003(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 0, 0, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_0030(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 0, 0, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_0011(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_1011(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_0111(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 1, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_1121(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 1, 2, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_0131(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 1, 3, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_1112(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 1, 1, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_1113(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 1, 1, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_1122(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 1, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x06, 0x06, 0x00, 0x00, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_003F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 0, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0x01, 0xFF, 0xFF, 0x80, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_013F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 1, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xFE, 0x02, 0xFE, 0xFF, 0x00, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_0F3F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 0, 15, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_103F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 0, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0xFF, 0x01, 0x00, 0xFF, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_203F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 2, 0, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_113F(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 1, 3, 16, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0xFE, 0x02, 0x00, 0xFF, 0x00, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_1E22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 14, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_1F22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 1, 15, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_2E22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 2, 14, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_rect_2F22(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  bitmap_rect(&bitmap, 2, 15, 2, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_oval_0011(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 0, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_0711(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 0, 7, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_3300(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 3, 3, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_3301(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 3, 3, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_3310(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 3, 3, 1, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_3311(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 3, 3, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x08, 0x14, 0x08, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_3323(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 3, 3, 2, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x7F, 0x41, 0x7F, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_3332(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 3, 3, 3, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x1C, 0x14, 0x22, 0x22, 0x22, 0x14, 0x1C, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_3333(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 3, 3, 3, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x1C, 0x22, 0x41, 0x41, 0x41, 0x22, 0x1C, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_7011(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 7, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_oval_7711(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_oval(&bitmap, 7, 7, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_0011(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 0, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_0711(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 0, 7, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_3300(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 3, 3, 0, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_3301(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 3, 3, 0, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_3310(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 3, 3, 1, 0, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_3311(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 3, 3, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x08, 0x1C, 0x08, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_3323(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 3, 3, 2, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_3332(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 3, 3, 3, 2, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x1C, 0x1C, 0x3E, 0x3E, 0x3E, 0x1C, 0x1C, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_3333(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 3, 3, 3, 3, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x1C, 0x3E, 0x7F, 0x7F, 0x7F, 0x3E, 0x1C, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_7011(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 7, 0, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_filled_oval_7711(void) {
  init();
  bitmap.columns = 8;
  bitmap.rows = 1;
  bitmap_fill(&bitmap, 0x00);
  bitmap_filled_oval(&bitmap, 7, 7, 1, 1, bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0,
                 0xDC, 0xDC}),
    bitmap_data,
    10);
}

void test_flood_fill_set_all(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  uint16_t buff[16];

  bitmap_flood_fill(&bitmap, 1, 7, TRUE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0xFF, 0xFF,
                 0xFF, 0xFF, 0xFF,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_flood_fill_clear_all(void) {
  init();
  bitmap_fill(&bitmap, 0xFF);
  uint16_t buff[16];

  bitmap_flood_fill(&bitmap, 1, 7, FALSE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_flood_fill_set_occupied(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  uint16_t buff[16];
  bitmap_point(&bitmap, 1, 7, bitmap_XOR);

  bitmap_flood_fill(&bitmap, 1, 7, TRUE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x80, 0x00,
                 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_flood_fill_clear_occupied(void) {
  init();
  bitmap_fill(&bitmap, 0xFF);
  uint16_t buff[16];
  bitmap_point(&bitmap, 1, 7, bitmap_XOR);

  bitmap_flood_fill(&bitmap, 1, 7, FALSE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0x7F, 0xFF,
                 0xFF, 0xFF, 0xFF,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_flood_fill_find_gap(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  uint16_t buff[16];
  bitmap_hline(&bitmap, 0, 2, 3, bitmap_OR);  // line across
  bitmap_point(&bitmap, 1, 2, bitmap_XOR); // make a hole

  bitmap_flood_fill(&bitmap, 1, 7, TRUE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0xFF, 0xFF, 0xFF,
                 0xFF, 0xFF, 0xFF,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_flood_fill_blocked_1(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  uint16_t buff[16];
  bitmap_hline(&bitmap, 0, 2, 1, bitmap_OR);  // line across
  bitmap_vline(&bitmap, 1, 2, 14, bitmap_OR);  // line down

  bitmap_flood_fill(&bitmap, 0, 7, TRUE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0xFC, 0xFC, 0x00,
                 0xFF, 0xFF, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_flood_fill_blocked_2(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  uint16_t buff[16];
  bitmap_hline(&bitmap, 0, 2, 1, bitmap_OR);  // line across
  bitmap_vline(&bitmap, 1, 2, 14, bitmap_OR);  // line down

  bitmap_flood_fill(&bitmap, 2, 7, TRUE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0x07, 0xFF, 0xFF,
                 0x00, 0xFF, 0xFF,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_flood_fill_blocked_3(void) {
  init();
  bitmap_fill(&bitmap, 0x00);
  uint16_t buff[16];
  bitmap_hline(&bitmap, 0, 2, 1, bitmap_OR);  // line across
  bitmap_vline(&bitmap, 1, 2, 14, bitmap_OR);  // line down

  bitmap_flood_fill(&bitmap, 1, 7, FALSE, buff, 16);

  assert_buff_equal(
    ((uint8_t[]){0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00,
                 0xDC, 0xDC}),
    bitmap_data,
    8);
}

void test_str_00(void) {
  init();
  bitmap.columns = 6;
  bitmap_fill(&bitmap, 0x00);
  bitmap_str(
    &bitmap,
    &font,
    "HI",
    0,
    0,
    bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){
      0xFF, 0x00, 0xFF, 0x01, 0xFF, 0x01,
      0xFF, 0x01, 0xFF, 0x80, 0xFF, 0x80,
      0xDC}),
    bitmap_data,
    13);
}

void test_str_01(void) {
  init();
  bitmap.columns = 6;
  bitmap_fill(&bitmap, 0x00);
  bitmap_str(
    &bitmap,
    &font,
    "HI",
    0,
    1,
    bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){
      0xFE, 0x00, 0xFE, 0x02, 0xFE, 0x02,
      0xFF, 0x02, 0xFF, 0x00, 0xFF, 0x00,
      0xDC}),
    bitmap_data,
    13);
}

void test_str_10(void) {
  init();
  bitmap.columns = 6;
  bitmap_fill(&bitmap, 0x00);
  bitmap_str(
    &bitmap,
    &font,
    "HI",
    1,
    0,
    bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){
      0x00, 0xFF, 0x00, 0xFF, 0x01, 0xFF,
      0x00, 0xFF, 0x01, 0xFF, 0x80, 0xFF,
      0xDC}),
    bitmap_data,
    13);
}

void test_str_11(void) {
  init();
  bitmap.columns = 6;
  bitmap_fill(&bitmap, 0x00);
  bitmap_str(
    &bitmap,
    &font,
    "HI",
    1,
    1,
    bitmap_XOR);

  assert_buff_equal(
    ((uint8_t[]){
      0x00, 0xFE, 0x00, 0xFE, 0x02, 0xFE,
      0x00, 0xFF, 0x02, 0xFF, 0x00, 0xFF,
      0xDC}),
    bitmap_data,
    13);
}

int main(void) {
  test(test_OR);
  test(test_AND);
  test(test_SET);
  test(test_NSET);
  test(test_NAND);
  test(test_NOR);
  test(test_XOR);
  test(test_XNOR);

  test(test_render_fast);
  test(test_render_fast_offset);
  test(test_render);
  test(test_render_offsetx);
  test(test_render_offsety);
  test(test_fill);

  test(test_point_00);
  test(test_point_00_NAND);
  test(test_point_01);
  test(test_point_0F);
  test(test_point_0X);
  test(test_point_20);
  test(test_point_21);
  test(test_point_2F);
  test(test_point_2X);
  test(test_point_30);

  test(test_is_set_true);
  test(test_is_set_false);
  test(test_is_set_oob);

  test(test_byte_00);
  test(test_byte_02);
  test(test_byte_02_NAND);
  test(test_byte_0A);
  test(test_byte_20);
  test(test_byte_22);

  test(test_hline_001);
  test(test_hline_001_NAND);
  test(test_hline_0X1);
  test(test_hline_301);
  test(test_hline_002);
  test(test_hline_104);
  test(test_hline_2F1);
  test(test_hline_0F2);
  test(test_hline_1F4);

  test(test_vline_000);
  test(test_vline_001);
  test(test_vline_001_NAND);
  test(test_vline_008);
  test(test_vline_028);
  test(test_vline_088);
  test(test_vline_0A8);
  test(test_vline_0X8);
  test(test_vline_200);
  test(test_vline_201);
  test(test_vline_208);
  test(test_vline_228);
  test(test_vline_288);
  test(test_vline_2A8);
  test(test_vline_2X8);
  test(test_vline_301);

  test(test_line_0000);
  test(test_line_000F);
  test(test_line_001F);
  test(test_line_0030);
  test(test_line_0033);
  test(test_line_0F0F);
  test(test_line_0X0X);
  test(test_line_2020);
  test(test_line_2F2F);
  test(test_line_3030);
  test(test_line_3300);
  test(test_line_3X3X);

  test(test_filled_rect_0000);
  test(test_filled_rect_0003);
  test(test_filled_rect_0030);
  test(test_filled_rect_0011);
  test(test_filled_rect_1011);
  test(test_filled_rect_0111);
  test(test_filled_rect_1121);
  test(test_filled_rect_0131);
  test(test_filled_rect_1112);
  test(test_filled_rect_1113);
  test(test_filled_rect_1122);
  test(test_filled_rect_003F);
  test(test_filled_rect_013F);
  test(test_filled_rect_0F3F);
  test(test_filled_rect_103F);
  test(test_filled_rect_203F);
  test(test_filled_rect_113F);
  test(test_filled_rect_1E22);
  test(test_filled_rect_1F22);
  test(test_filled_rect_2E22);
  test(test_filled_rect_2F22);

  test(test_rect_0000);
  test(test_rect_0003);
  test(test_rect_0030);
  test(test_rect_0011);
  test(test_rect_1011);
  test(test_rect_0111);
  test(test_rect_1121);
  test(test_rect_0131);
  test(test_rect_1112);
  test(test_rect_1113);
  test(test_rect_1122);
  test(test_rect_003F);
  test(test_rect_013F);
  test(test_rect_0F3F);
  test(test_rect_103F);
  test(test_rect_203F);
  test(test_rect_113F);
  test(test_rect_1E22);
  test(test_rect_1F22);
  test(test_rect_2E22);
  test(test_rect_2F22);

  test(test_oval_0011);
  test(test_oval_0711);
  test(test_oval_3300);
  test(test_oval_3301);
  test(test_oval_3310);
  test(test_oval_3311);
  test(test_oval_3323);
  test(test_oval_3332);
  test(test_oval_3333);
  test(test_oval_7011);
  test(test_oval_7711);

  test(test_filled_oval_0011);
  test(test_filled_oval_0711);
  test(test_filled_oval_3300);
  test(test_filled_oval_3301);
  test(test_filled_oval_3310);
  test(test_filled_oval_3311);
  test(test_filled_oval_3323);
  test(test_filled_oval_3332);
  test(test_filled_oval_3333);
  test(test_filled_oval_7011);
  test(test_filled_oval_7711);

  test(test_flood_fill_set_all);
  test(test_flood_fill_clear_all);
  test(test_flood_fill_set_occupied);
  test(test_flood_fill_clear_occupied);
  test(test_flood_fill_find_gap);
  test(test_flood_fill_blocked_1);
  test(test_flood_fill_blocked_2);
  test(test_flood_fill_blocked_3);

  test(test_str_00);
  test(test_str_01);
  test(test_str_10);
  test(test_str_11);

  return 0;
}
