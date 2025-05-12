
import struct
import sys

def main():
    out = None
    last_rounds = -1
    for line in sys.stdin:
        if line.startswith("Done"):
            continue
        lines = line.split()
        if len(lines) != 2:
            continue
        rounds = int(lines[0])
        val = int(lines[1], 16)
        if last_rounds != rounds:
            if out:
                out.close()
            fname = 'out/entropy-%d.bin' % rounds
            print 'Creating %s' % fname
            out = open(fname, 'wb')
            last_rounds = rounds
        out.write(struct.pack('B', val))
    out.close()

main()
