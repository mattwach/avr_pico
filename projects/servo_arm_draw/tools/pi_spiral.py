import gcode
import math
import simple_font

OUTER_RAD = 27.0
INNER_RAD = 0.1
RAD_STEP = 1.0
SPACING = 2.1
FONT_WIDTH = 2
FONT_HEIGHT = 4

XORIGIN = -12
YORIGIN = 97

def make_pi():
    q, r, t, k, m, x = 1, 0, 1, 1, 3, 3
    j = 0
    while True:
        if 4 * q + r - t < m * t:
            yield str(m)
            if j == 0:
                yield '.'
            j += 1
            q, r, t, k, m, x = 10*q, 10*(r-m*t), t, k, (10*(3*q+r))//t - 10*m, x
        else:
            q, r, t, k, m, x = q*k, (2*q+r)*x, t*x, k+1, (q*(7*k+2)+r*x)//(t*x), x+2

def main():
    g = gcode.GCode()
    angle = math.pi / 2
    rad = OUTER_RAD

    g.origin(XORIGIN, YORIGIN)

    for digit in make_pi():
        # put a character at the current position
        x = math.cos(angle) * rad
        y = math.sin(angle) * rad
        normal = angle - math.pi / 2
        g.push_local_xform(x=x, y=y, angle=normal)
        simple_font.draw_char(g, digit, 0, 0, FONT_WIDTH, FONT_HEIGHT)
        g.pop_xform()
        
        # need to convert the spacing to an angle
        angle_step = SPACING / rad 
        angle -= angle_step
        rad -= RAD_STEP * angle_step

        if rad <= INNER_RAD:
            break

    g.dump()



if __name__ == '__main__':
    main()
