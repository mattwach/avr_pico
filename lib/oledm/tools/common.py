#!/usr/bin/python3

import collections
import sys

# Given a .png and yaml file, create a fixed with font.

def MakeFont(props, filename):
    props['chars'] = collections.defaultdict(list)

    with open(filename) as f:
        in_bitmap = False
        character = -1
        for line in f:
            sline = line.strip()
            if sline.startswith('// Character '):
                in_bitmap = True
                c = sline.split()[2]
                if c.startswith('\''):
                  character = ord(c[1:-1])
                else:
                  character = int(c)
                continue

            if not in_bitmap:
                if sline.startswith('Width: '):
                    props['source_width'] = int(sline.split()[1])
                elif sline.startswith('Height: '):
                    props['source_height'] = int(sline.split()[1])

            if in_bitmap:
                if sline.startswith('Bitmap: '):
                    sline = sline.replace('Bitmap:', '')
                if sline.endswith('\\'):
                    sline = sline[:-1]
                if sline.startswith('Unicode:'):
                    in_bitmap = False
                elif not line.startswith('//'):
                    sline = sline.strip()
                    props['chars'][character].append(sline)

def DumpChar(data, c, f):
    f.write('\n// Character %u (0x%02X) (%c)\n' % (c, c, chr(c)))
    f.write('\n'.join(data))


def DumpFont(props, f):
    for c in props['chars']:
        DumpChar(props['chars'][c], c, f)
