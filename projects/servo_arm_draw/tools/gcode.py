# Utilities for creating gcode for the drawing machine

import math
import sys

def _make_num(v):
  s = '%.4f' % v
  last_idx = len(s) - 1
  while s[last_idx] == '0':
    last_idx -= 1
  if s[last_idx] == '.':
    last_idx -= 1
  return s[:last_idx + 1]

class Code(object):
  def __init__(self, code, args, comment=None):
    self.code = code
    # args are just strings
    self.args = args
    self.comment = comment

class Transformation(object):
  def __init__(self, x, y, angle, xscale, yscale):
    self.x = x
    self.y = y
    self.angle = angle
    self.xscale = xscale
    self.yscale = yscale

  def transform(self, x, y):
    sx = x * self.xscale
    sy = y * self.yscale

    x = sx * math.cos(self.angle) - sy * math.sin(self.angle)
    y = sx * math.sin(self.angle) + sy * math.cos(self.angle)

    x = x + self.x
    y = y + self.y

    return x, y

class GCode(object):

  def __init__(self):
    self.pscale = None
    self.originx = None
    self.originy = None
    self.flow = None
    self.default_flow = 20
    self.x = -9999999999
    self.y = -9999999999
    self.pen_down = False
    self.codes = []
    self.transform_stack = [
      Transformation(x=0, y=0, angle=0, xscale=1, yscale=1),
    ]

  def _check_init(self):
    if self.originx is None:
      self.origin(-40, 80)
    if self.pscale is None:
      self.scale(1)


  def dump(self, fout=sys.stdout):
    fout.write('%\n')
    line_number = 0
    for code in self.codes:
      if code.comment and code.code is None:
        fout.write(';%s\n' % code.comment)
      elif code.args:
        line_number += 1
        fout.write(
             'N%u G%02u %s\n' % (line_number, code.code, ' '.join(code.args)))
    fout.write('%\n')

  def push_local_xform(self, x=0, y=0, angle=0, xscale=1, yscale=1):
    top = self.transform_stack[-1]
    self.transform_stack.append(
        Transformation(
          x=x + top.x,
          y=y + top.y,
          angle=angle + top.angle,
          xscale=xscale * top.xscale,
          yscale=yscale * top.yscale))

  def push_global_xform(self, x=0, y=0, angle=0, xscale=1, yscale=1):
    self.transform_stack.append(Transformation(x, y, angle, xscale, yscale))

  def pop_xform(self):
    if len(self.transform_stack) > 1:
      self.transform_stack.pop()

  def comment(self, msg):
    self.codes.append(Code(None, None, msg))

  def move(self, x=0, y=0, pen_down=False, flow=None):
    self._check_init()
    x, y = self.transform_stack[-1].transform(x, y)

    args = []

    if x != self.x:
      args.append('X' + _make_num(x))
      self.x = x

    if y != self.y:
      args.append('Y' + _make_num(y))
      self.y = y

    if pen_down != self.pen_down:
      if pen_down:
        args.append('Z0')
      else:
        args.append('Z1')
      self.pen_down = pen_down

    if flow and flow != self.flow:
      args.append('F' + _make_num(flow))
      self.flow = flow

    if not flow and not self.flow:
      args.append('F' + _make_num(self.default_flow))
      self.flow = self.default_flow

    self.codes.append(Code(1, args))

  def add_connected_sequence(self, pairs):
    """Adds a connected sequence.

    A connected sequence is a sequence where the end point
    is the same as the start point.  This algorithm finds the
    closest pair to the current x,y and uses that
    as the starting point.  This can save on movement
    time.
    """

    best_pair_idx = None
    best_pair_dist_sq = None

    for idx, p in enumerate(pairs):
      x, y = p
      delta_x = x - self.x
      delta_y = y - self.y
      dist_sq = delta_x * delta_x + delta_y * delta_y
      if best_pair_dist_sq is None or dist_sq < best_pair_dist_sq:
        best_pair_dist_sq = dist_sq
        best_pair_idx = idx

    self.move(pairs[best_pair_idx][0], pairs[best_pair_idx][1], False)

    for p in pairs[best_pair_idx+1:]:
      self.move(p[0], p[1], True)

    for p in pairs[0:best_pair_idx+1]:
      self.move(p[0], p[1], True)

  def rectangle(self, x1=0, y1=0, x2=1, y2=1):
    """Draw a rectangle"""
    self.add_connected_sequence((
      (x1, y1),
      (x2, y1),
      (x2, y2),
      (x1, y2),
    ))

  def polygon(self, x=0, y=0, r=1, sides=None, start_angle=0, end_angle=None):
    """Draw a polygon or circle."""
    if end_angle is None:
      end_angle = start_angle + math.pi * 2

    span = end_angle - start_angle

    if not sides:
      # make each segment 2 mm in size
      sides = abs(end_angle - start_angle) * r / 2
      if sides < 5:
        sides = 5

    step = span / sides
    angle = start_angle
    points = []
    while True:
      points.append((x + r * math.cos(angle), y + r * math.sin(angle)))
      angle += step

      if step > 0 and angle > end_angle:
        break
      if step < 0 and angle < end_angle:
        break

    self.add_connected_sequence(points)

  def origin(self, x, y):
    args = []

    if x != self.originx:
      args.append('X%.5g' % x)
      self.originx = x

    if y != self.originy:
      args.append('Y%.5g' % y)
      self.originy = y

    self.codes.append(Code(54, args))

  def scale(self, scalev):
    args = []

    if scalev != self.pscale:
      args.append('P%.5g' % scalev)
      self.pscale = scalev

    self.codes.append(Code(51, args))

if __name__ == '__main__':
  # a little test sequence
  g = GCode()
  g.rectangle()
  g.polygon(r=10)
  g.dump()

