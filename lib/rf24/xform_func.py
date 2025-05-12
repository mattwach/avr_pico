#!/usr/bin/python3

import sys

REPLACE = (
  ('delayMicroseconds', '_delay_us'),
)

IGNORE = set((
  'BV',
  'defined',
  'digitalWrite',
  'for',
  'if',
  'IF_SERIAL_DEBUG',
  'pinMode',
  'printf',
  'printf_P',
  'print_observe_tx',
  'PSTR',
  'rf24_min',
  'SPISettings',
  'strlen',
  'strlen_P',
  'transfer',
  'transfernb',
  'while',
  'RF24',
  'GPIO',
))

STARTWITH_IGNORE = (
  'SPIClass::',
)

LINE_REPLACE = [
 ('bool', 'bool_t'),
]

MEMBERS = (
  'ce_pin',
  'csn_pin',
  'spi_speed',
  'p_variant',
  'payload_size',
  'dynamic_payloads_enabled',
  'pipe0_reading_address',
  'addr_width',
  'txDelay',
  'csDelay',
  'failureDetected',
)

LINE_REPLACE.extend((k, 'rf24->%s' % k) for k in MEMBERS)

def check_fn(fn_name):
    if '.' in fn_name:
        return None, None

    for key in STARTWITH_IGNORE:
      if fn_name.startswith(key):
        return None, None

    first_param = 'rf24'
    if 'RF24::' in fn_name:
      first_param = 'const RF24* rf24'

    fn_name = fn_name.replace('RF24::', '')

    if fn_name in IGNORE:
        return fn_name, ''

    for frm, to in REPLACE:
      if fn_name == frm:
        return to, ''

    new_fn = []
    changed = False
    for c in fn_name:
        if c >= 'A' and c <= 'Z':
            changed = True
            new_fn.append('_')
            new_fn.append(c.lower())
        else:
            new_fn.append(c)

    new_fn = ''.join(new_fn)

    new_fn = new_fn.replace('_p_a', '_pa')
    new_fn = new_fn.replace('_t_x', '_tx')
    new_fn = new_fn.replace('_r_x', '_rx')
    new_fn = new_fn.replace('_i_r_q', '_irq')
    new_fn = new_fn.replace('_c_r_c', '_crc')
    if not new_fn.startswith('rf24_'):
        new_fn = 'rf24_' + new_fn

    sys.stderr.write('   %s -> %s\n' % (fn_name, new_fn))
    return new_fn, first_param

def process(line):
    printed = False
    new_line = ''
    replace_list = []
    maybe_start_idx = 0
    closeout_mode = False
    for idx, c in enumerate(line):
        if closeout_mode:
          if c != ' ':
            if c != ')':
              replace_list.append((idx, idx, ', '))
            closeout_mode = False

        if closeout_mode:
          continue

        if maybe_start_idx:
            if c == '(':
                if not printed:
                    printed = True
                    sys.stderr.write(line)
                new_name, first_param = check_fn(line[maybe_start_idx: idx])
                if new_name:
                    replace_list.append((maybe_start_idx, idx+1, new_name + '(' + first_param))
                    closeout_mode = first_param
                maybe_start_idx = None
            elif (not (
                  (c >= 'a' and c <= 'z') or
                  (c >= 'A' and c <= 'Z') or
                  (c >= '0' and c <= '9') or
                  (c in '_:.'))):
                maybe_start_idx = None

        elif (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z'):
            maybe_start_idx = idx

    if replace_list:
        replace_list.reverse()
        for start_idx, end_idx, new_txt in replace_list:
            line = line[:start_idx] + new_txt + line[end_idx:]

    for frm, to in LINE_REPLACE:
      line = line.replace(frm, to)

    sys.stdout.write(line)
            

def main():
    for line in sys.stdin:
        process(line)

if __name__ == '__main__':
    main()
