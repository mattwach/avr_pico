#!/usr/bin/python3

import os
import sys
import yaml

import common
from io import StringIO

def MakeCCharComment(data, c, f):
  comment = StringIO()
  common.DumpChar(data, c, comment)
  for line in comment.getvalue().split('\n'):
    if line:
      f.write('  // %s\n' % line)


def MakeCCharHex(data, row, column, f):
  # MSB is the bottom of the bitmap
  value = 0x00
  mask = 0x01
  for bit in range(8):
    if data[(row * 8) + bit][column] == '#':
      value |= mask
    mask = mask << 1
  f.write(' 0x%02X,' % value)


def MakeCChar(data, c, columns, rows, f):
  print('working on %s' % c)
  MakeCCharComment(data, c, f)
  f.write(' ');
  for row in range(rows):
    for column in range(columns):
      MakeCCharHex(data, row, column, f)
    f.write('\n');


def MakeCSource(props, basename):
  with open(basename + '.c', 'w') as f:
    f.write('\n'.join((
      '// Generated font data for %s' % basename,
      '',
      '#include "avr_flash.h"',
      '',
      'const uint8_t %s[] PROGMEM = {' % basename,
      '  0x46, 0x41, 0x53, 0x31,  // id: FAS1',
      '  0x%02X, // first_char' % props['first_char'],
      '  0x%02X, // last_char' % props['last_char'],
      '  0x%02X, // width' % props['columns'],
      '  0x%02X, // height' % props['rows'],
      '',
    )))

    # write out each char
    for c in range(props['first_char'], props['last_char'] + 1):
      MakeCChar(props['chars'][c], c, props['columns'], props['rows'], f)

    f.write('};\n')


def MakeCHeader(props, basename):
  with open(basename + '.h', 'w') as f:
    f.write('\n'.join((
      '#ifndef SSD1306_TEXT_FONT_%s' % basename.upper(),
      '#define SSD1306_TEXT_FONT_%s' % basename.upper(),
      '// Generated font data for %s' % basename,
      '',
      '#include "avr_flash.h"',
      '',
      'extern const uint8_t %s[] PROGMEM;' % basename,
      '',
      '#endif  // SSD1306_TEXT_FONT_%s' % basename.upper(),
      '',
    )))

def main(args):
    if len(args) != 2:
        sys.error('Usage: make_fixed_ascii_font.py <config.yaml>')

    with open(args[1]) as f:
        props = yaml.safe_load(f)
    common.MakeFont(props, props['source'])
    basename = os.path.splitext(args[1])[0]
    MakeCSource(props, basename)
    MakeCHeader(props, basename)

if __name__ == '__main__':
    main(sys.argv)
