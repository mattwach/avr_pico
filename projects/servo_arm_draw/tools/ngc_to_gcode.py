import sys

PEN_IS_DOWN = False
FEED = 0

def add_code(codes, sp_line, pen_down, feed):
    global PEN_IS_DOWN
    global FEED

    new_line = ['G01']
    for t in sp_line:
        if t[0] == 'X':
            new_line.append(t)
        elif t[0] == 'Y':
            new_line.append(t)

    if pen_down and not PEN_IS_DOWN:
        new_line.append('Z0')
    elif not pen_down and PEN_IS_DOWN:
        new_line.append('Z1')

    if feed != FEED:
        new_line.append('F%f' % feed)


    FEED = feed
    PEN_IS_DOWN = pen_down
    codes.append(' '.join(new_line))

def make_gcode():

    print('%')

    codes =  [
      'G54 X-40 Y50',
      'G51 P0.4',
    ]

    pen_down = False
    feed = 20
    for line in sys.stdin:
        sp_line = line.split()
        if len(sp_line) < 1:
            continue
        code = sp_line[0]

        if code == 'G00':
            pen_down = False
            add_code(codes, sp_line, pen_down, feed)
        elif code == 'M3':
            pen_down = True
        elif code == 'M5':
            pen_down = False
        elif code == 'G01':
            add_code(codes, sp_line, pen_down, feed)
        elif code == 'G02':
            add_code(codes, sp_line, pen_down, feed)
        elif code == 'G03':
            add_code(codes, sp_line, pen_down, feed)

    for line_number, code in enumerate(codes):
        print('N%02u %s' % (line_number + 1, code))

    print('%')


make_gcode()
