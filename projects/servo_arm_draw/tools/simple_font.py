# render a simple font

import gcode

CHAR_MAP = {
  '!': (
      (0.5, 0, False),
      (0.5, 0.25, True),

      (0.5, 0.5, False),
      (0.5, 1, True),
  ),
  '"': (
      (0.4, 0.5, False),
      (0.4, 1, True),

      (0.6, 0.5, False),
      (0.6, 1, True),
  ),
  '#': (
      (0.4, 0.1, False),
      (0.4, 0.9, True),

      (0.1, 0.6, False),
      (0.9, 0.6, True),

      (0.6, 0.9, False),
      (0.6, 0.1, True),

      (0.9, 0.4, False),
      (0.1, 0.4, True),
  ),
  '$': (
      (0.25, 0.25, False),
      (0.75, 0.25, True),
      (0.75, 0.5, True),
      (0.25, 0.5, True),
      (0.25, 0.75, True),
      (0.75, 0.75, True),

      (0.5, 1, False),
      (0.5, 0, True),
  ),
  '%': (
     (0.2, 0.65, False),
     (0.35, 0.65, True),
     (0.35, 0.8, True),
     (0.2, 0.8, True),
     (0.2, 0.65, True),

     (0.1, 0.1, False),
     (0.9, 0.9, True),

     (0.8, 0.2, False),
     (0.65, 0.2, True),
     (0.65, 0.35, True),
     (0.8, 0.35, True),
     (0.8, 0.2, True),
  ),
  '&': (
      (0.75, 0.5, False),
      (0.5, 0, True),
      (0, 0, True),
      (0, 0.5, True),
      (0.5, 0.75, True),
      (0.5, 1, True),
      (0, 1, True),
      (0, 0.75, True),
      (0.75, 0, True),
  ),
  '\'': (
      (0.5, 0.5, False),
      (0.5, 1, True),
  ),
  '(': (
      (0.5, 0, False),
      (0.25, 0.25, True),
      (0.25, 0.75, True),
      (0.5, 1, True),
  ),
  ')': (
      (0.5, 0, False),
      (0.75, 0.25, True),
      (0.75, 0.75, True),
      (0.5, 1, True),
  ),
  '*': (
      (0.25, 0.5, False),
      (0.75, 0.5, True),

      (0.75, 0.75, False),
      (0.25, 0.25, True),

      (0.5, 0.25, False),
      (0.5, 0.75, True),

      (0.25, 0.75, False),
      (0.75, 0.25, True),
  ),
  '+': (
      (0.1, 0.5, False),
      (0.9, 0.5, True),

      (0.5, 0.9, False),
      (0.5, 0.1, True),
  ),
  '-': (
      (0.1, 0.5, False),
      (0.9, 0.5, True),
  ),
  '.': (
      (0.25, 0, False),
      (0, 0, True),
      (0, 0.25, True),
      (0.25, 0.25, True),
      (0.25, 0, True),
  ),
  '/': (
      (0.75, 1, False),
      (0.25, 0, True),
  ),
  '0': (
      (0.75, 1, False),
      (0.25, 0, True),
      (0.75, 0, True),
      (1, 0.25, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
      (0, 0.25, True),
      (0.25, 0, True),
  ),
  '1': (
      (0.5, 0, False),
      (0.5, 1, True),
  ),
  '2': (
      (1, 0, False),
      (0, 0, True),
      (0.75, 0.5, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
  ),
  '3': (
      (0, 0.25, False),
      (0.25, 0, True),
      (0.75, 0, True),
      (1, 0.25, True),
      (0.75, 0.5, True),
      (0.25, 0.5, True),
      (0.75, 0.5, False),
      (1, 0.75, True),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
  ),
  '4': (
      (1, 0.25, False),
      (0, 0.25, True),
      (0.75, 1, True),
      (0.75, 0, True),
  ),
  '5': (
      (0, 0, False),
      (0.75, 0, True),
      (1, 0.25, True),
      (0.75, 0.5, True),
      (0, 0.5, True),
      (0, 1, True),
      (1, 1, True),
  ),
  '6': (
      (0, 0.5, False),
      (0.75, 0.5, True),
      (1, 0.25, True),
      (0.75, 0, True),
      (0.25, 0, True),
      (0, 0.25, True),
      (0, 0.75, True),
      (0.25, 1, True),
      (0.75, 1, True),
  ),
  '7': (
      (0.25, 0, False),
      (1, 1, True),
      (0, 1, True),
  ),
  '8': (
      (0.75, 0.5, False),
      (1, 0.25, True),
      (0.75, 0, True),
      (0.25, 0, True),
      (0, 0.25, True),
      (0.25, 0.5, True),
      (0.75, 0.5, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
      (0.25, 0.5, True),
  ),
  '9': (
      (0.25, 0, False),
      (0.75, 0, True),
      (1, 0.25, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
      (0.25, 0.5, True),
      (1, 0.5, True),
  ),
  ':': (
      (0.25, 0.65, False),
      (0, 0.65, True),
      (0, 0.9, True),
      (0.25, 0.9, True),
      (0.25, 0.65, True),

      (0.25, 0.1, False),
      (0, 0.1, True),
      (0, 0.35, True),
      (0.25, 0.35, True),
      (0.25, 0.1, True),
  ),
  ';': (
      (0.25, 0.65, False),
      (0, 0.65, True),
      (0, 0.9, True),
      (0.25, 0.9, True),
      (0.25, 0.65, True),

      (0.25, 0.25, False),
      (0, 0.1, True),
      (0, 0.5, True),
      (0.25, 0.5, True),
      (0.25, 0.25, True),
  ),
  '<': (
      (0.75, 1, False),
      (0.25, 0.5, True),
      (0.75, 0, True),
  ),
  '=': (
      (0.9, 0.65, False),
      (0.1, 0.65, True),

      (0.1, 0.35, False),
      (0.9, 0.35, True),
  ),
  '>': (
      (0.25, 1, False),
      (0.75, 0.5, True),
      (0.25, 0, True),
  ),
  '?': (
      (0.0, .75, False),
      (0.25, 1, True),
      (0.75, 1, True),
      (1, 0.75, True),
      (0.5, 0.5, True),

      (0.625, 0, False),
      (0.375, 0, True),
      (0.375, 0.25, True),
      (0.625, 0.25, True),
      (0.625, 0, True),
  ),
  '@': (
      (0.75, 0.25, False),
      (0.75, 0.75, True),
      (0.25, 0.75, True),
      (0.25, 0.25, True),
      (1, 0.25, True),
      (1, 1, True),
      (0, 1, True),
      (0, 0, True),
      (1, 0, True),
  ),
  'A': (
      (1, 0.5, False),
      (0, 0.5, True),

      (0, 0, False),
      (0, 0.5, True),
      (0.5, 1, True),
      (1, 0.5, True),
      (1, 0, True),
  ),
  'B': (
      (0, 0.5, False),
      (0.75, 0.5, True),
      (1, 0.25, True),
      (0.75, 0, True),
      (0, 0, True),
      (0, 1, True),
      (0.75, 1, True),
      (1, 0.75, True),
      (0.75, 0.5, True),
  ),
  'C': (
      (1, 0.75, False),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
      (0, 0.25, True),
      (0.25, 0, True),
      (0.75, 0, True),
      (1, 0.25, True),
  ),
  'D': (
      (0.75, 0, False),
      (1, 0.25, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0, 1, True),
      (0, 0, True),
      (0.75, 0, True),
  ),
  'E': (
      (0, 0.5, False),
      (0.5, 0.5, True),
      
      (1, 1, False),
      (0, 1, True),
      (0, 0, True),
      (1, 0, True),
  ),
  'F': (
      (0, 0.5, False),
      (0.5, 0.5, True),
      
      (1, 1, False),
      (0, 1, True),
      (0, 0, True),
  ),
  'G': (
      (1, 0.75, False),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
      (0, 0.25, True),
      (0.25, 0, True),
      (0.75, 0, True),
      (1, 0.25, True),
      (1, 0.5, True),
      (0.5, 0.5, True),
  ),
  'H': (
      (0, 1, False),
      (0, 0, True),

      (0, 0.5, False),
      (1, 0.5, True),

      (1, 1, False),
      (1, 0, True),
  ),
  'I': (
      (1, 1, False),
      (0, 1, True),

      (0.5, 1, False),
      (0.5, 0, True),
      
      (0, 0, False),
      (1, 0, True),
  ),
  'J': (
      (1, 1, False),
      (1, 0.25, True),
      (0.75, 0, True),
      (0.25, 0, True),
      (0, 0.25, True),
  ),
  'K': (
      (0, 0, False),
      (0, 1, True),

      (0.75, 1, False),
      (0, 0.5, True),
      (1, 0, True),
  ),
  'L': (
      (0, 1, False),
      (0, 0, True),
      (1, 0, True),
  ),
  'M': (
      (0, 0, False),
      (0, 1, True),
      (0.5, 0.5, True),
      (1, 1, True),
      (1, 0, True),
  ),
  'N': (
      (0, 0, False),
      (0, 1, True),
      (1, 0, True),
      (1, 1, True),
  ),
  'O': (
      (1, 0, False),
      (1, 1, True),
      (0, 1, True),
      (0, 0, True),
      (1, 0, True),
  ),
  'P': (
      (0, 0.5, False),
      (0.75, 0.5, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0, 1, True),
      (0, 0, True),
  ),
  'Q': (
      (0.75, 0, False),
      (1, 0.25, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
      (0, 0.25, True),
      (0.25, 0, True),
      (0.75, 0, True),

      (0.5, 0.5, False),
      (1, 0, True),
  ),
  'R': (
      (0, 0.5, False),
      (0.75, 0.5, True),
      (1, 0.75, True),
      (0.75, 1, True),
      (0, 1, True),
      (0, 0, True),

      (0.25, 0.5, False),
      (1, 0, True),
  ),
  'S': (
      (1, 0.75, False),
      (0.75, 1, True),
      (0.25, 1, True),
      (0, 0.75, True),
      (0.25, 0.5, True),
      (0.75, 0.5, True),
      (1, 0.25, True),
      (0.75, 0, True),
      (0.25, 0, True),
      (0, 0.25, True),
  ),
  'T': (
      (0, 1, False),
      (1, 1, True),

      (0.5, 1, False),
      (0.5, 0, True),
  ),
  'U': (
      (0, 1, False),
      (0, 0.25, True),
      (0.25, 0, True),
      (0.75, 0, True),
      (1, 0.25, True),
      (1, 1, True),
  ),
  'V': (
      (0, 1, False),
      (0.5, 0, True),
      (1, 1, True),
  ),
  'W': (
      (0, 1, False),
      (0.25, 0, True),
      (0.5, 0.5, True),
      (0.75, 0, True),
      (1, 1, True),
  ),
  'X': (
      (0, 0, False),
      (1, 1, True),

      (0, 1, False),
      (1, 0, True),
  ),
  'Y': (
      (0, 1, False),
      (0.5, 0.5, True),
      (1, 1, True),

      (0.5, 0.5, False),
      (0.5, 0, True),
  ),
  'Z': (
      (0, 1, False),
      (1, 1, True),
      (0, 0, True),
      (1, 0, True),
  ),
  '[': (
      (0.75, 1, False),
      (0.25, 1, True),
      (0.25, 0, True),
      (0.75, 0, True),
  ),
  '\\': (
      (0.25, 1, False),
      (0.75, 0, True),
  ),
  ']': (
      (0.25, 1, False),
      (0.75, 1, True),
      (0.75, 0, True),
      (0.25, 0, True),
  ),
  '^': (
      (0.25, 0.5, False),
      (0.5, 0.75, True),
      (0.75, 0.5, True),
  ),
  '_': (
      (0, 0, False),
      (1, 0, True),
  ),
  '`': (
      (0.25, 1, False),
      (0.5, 0.5, True),
  ),
  '{': (
      (0.5, 1, False),
      (0.25, 1, True),
      (0.25, 0.75, True),
      (0.0, 0.5, True),
      (0.25, 0.25, True),
      (0.25, 0, True),
      (0.5, 0, True),
  ),
  '|': (
      (0.5, 1, False),
      (0.5, 0, True),
  ),
  '}': (
      (0.5, 1, False),
      (0.75, 1, True),
      (0.75, 0.75, True),
      (1, 0.5, True),
      (0.75, 0.25, True),
      (0.75, 0, True),
      (0.5, 0, True),
  ),
  '~': (
      (0, 0.5, False),
      (0.25, 0.75, True),
      (0.5, 0.5, True),
      (0.75, 0.75, True),
  ),
}

def draw_char(g, char, x=0, y=0, w=1, h=1):
  if char not in CHAR_MAP:
    return

  g.comment('"%c" @ (%g,%g)' % (char, x, y))
  for lx, ly, pen_down in CHAR_MAP[char]:
    g.move(x + (lx * w), y + (ly * h), pen_down) 

def draw_str(g, str, x=0, y=0, w=1, h=1):
  base_x = x
  for c in str:
    if c == '\n':
      x = base_x
      y -= h * 1.25
    else:
      draw_char(g, c, x, y, w, h)
      x += w * 1.25

if __name__ == '__main__':
  g = gcode.GCode()
  draw_str(g, '    !\"#$%&\'()*+-\n  ./0123456789:;<=\n >?@[\\]ABCDEFGHIJKL\n MNOPQRSTUVWXYZ^\n   _`{|}~', -28, 37, 5, 7)
  g.dump()
