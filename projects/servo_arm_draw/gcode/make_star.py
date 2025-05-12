import math

OFFSET_X = -10
OFFSET_Y = 100
SCALE = 1.0
RADIUS = 30
ROT = math.pi / 7
X = []
Y = []

for point in range(5):
  angle = 2 * math.pi * point / 5 + ROT
  X.append(math.cos(angle) * RADIUS)
  Y.append(math.sin(angle) * RADIUS)

lines = [
    'G54 X%f Y%f' % (OFFSET_X, OFFSET_Y),
    'G51 P%f' % SCALE,
    'G01 X%.2f Y%.2f Z1 F20' % (X[0], Y[0]),
    'G01 X%.2f Y%.2f Z0' % (X[2], Y[2]),
    'G01 X%.2f Y%.2f' % (X[4], Y[4]),
    'G01 X%.2f Y%.2f' % (X[1], Y[1]),
    'G01 X%.2f Y%.2f' % (X[3], Y[3]),
    'G01 X%.2f Y%.2f' % (X[0], Y[0]),
    'G01 X0 Y-30 Z1',
]

print '%'
for n, l in enumerate(lines):
    print 'N%02d %s' % (n+1, l)
print '%'



