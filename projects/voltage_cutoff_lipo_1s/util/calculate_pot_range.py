#!/usr/bin/env python3

import argparse
import sys

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("rtop", type=float, help="Fixed resistor, connected to VDD")
  parser.add_argument("rpot", type=float, help="Potentiometer, connected to between rtop and rbot")
  parser.add_argument("rbot", type=float, help="Fixed resistor, connected to GND")
  args = parser.parse_args()

  rtop = args.rtop
  rpot = args.rpot
  rbot = args.rbot
  rsum = rtop + rpot + rbot
  max_current = 4.2 / rsum

  print('Current @ 4.2: %.0f uA' % (max_current * 1000000.0))
  print('Power @ 4.2: %.0f uW' % (max_current * 4.2 * 1000000.0))

  volts = 2.9
  maxr = 0
  minr = rpot + 1
  while volts < 4.3:
    r = calc_rpot_setting(rtop, rpot, rbot, volts)
    maxr = max(r, maxr)
    minr = min(r, minr)
    volts += 0.1

  print('Utilized pot range: %.0f' % (maxr - minr))

def calc_rpot_setting(rtop, rpot, rbot, volts):
  # The goal is to find the rpot setting where we would see
  # 1V at the divider
  #
  # e.g.
  #
  #         rtop   rpot   rbot
  #  VCC ---/\/\---/\/\---/\/\--- GND
  #                 |
  #                1.0V
  i = volts / (rtop + rpot + rbot)
  vbot = rbot * i
  r = (1.0 - vbot) / i
  if r > rpot:
    print('Cant set pot for %.1f V' % volts)
    return rpot
  if r < 0:
    print('Cant set pot for %.1f V' % volts)
    return 0.0

  print('Pot setting for %.1f V: %.0f' % (volts, r))
  return r

if __name__ == '__main__':
  main()
