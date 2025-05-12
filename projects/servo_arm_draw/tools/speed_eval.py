import gcode
import simple_font

def make_speed(speed):
    g = gcode.GCode()
    g.origin(0, 97)
    g.default_flow = speed
    r = 40
    while r > 5:
        g.polygon(r=r, sides=3)
        r *= 0.75

    simple_font.draw_str(g, str(speed), -50, -10, 7, 10)
    fname = '/tmp/spd%u.gcd' % speed
    with open(fname, 'w') as fout:
        g.dump(fout)
    print('Created %s' % fname)

def output():
  speeds = [10, 20, 40, 80, 160, 320]

  for speed in speeds:
      make_speed(speed)

output()

