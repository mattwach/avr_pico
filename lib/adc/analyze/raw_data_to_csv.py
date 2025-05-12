# Sample Record:
# 
# Done 67 1800026
# Entropy = 7.872935 bits per byte.
# 
# Optimum compression would reduce the size
# of this 60858 byte file by 1 percent.
# 
# Chi square distribution for 60858 samples is 10625.81, and randomly
# would exceed this value 0.01 percent of the times.
# 
# Arithmetic mean value of data bytes is 128.8310 (127.5 = random).
# Monte Carlo value for Pi is 3.137138914 (error 0.14 percent).
# Serial correlation coefficient is 0.017649 (totally uncorrelated = 0.0).
# --------

import sys

OUTPUT_TIME_MS = 209000

FIELDS = (
    'Round',
    'Bytes',
    'Time mS',
    'uS Per Bit',
    'Entropy Bits',
    'Compression %',
    'Chi Square Value',
    'Chi Square %',
    'Arithmetic Mean',
    'Monte Carlo Value',
    'Monte Carlo Error %',
    'Serial Correrelation Coefficient',
)

def Done(record, linesp):
    # Done 67 1800026
    record['Round'] = linesp[1]
    record['Time mS'] = linesp[2]


def Entropy(record, linesp):
    # Entropy = 7.872935 bits per byte.
    record['Entropy Bits'] = linesp[2]


def Optimum(record, linesp):
    # of this 60858 byte file by 1 percent.
    record['Bytes'] = linesp[2]
    record['Compression %'] = linesp[6]
    record['uS Per Bit'] = str((int(record['Time mS']) - OUTPUT_TIME_MS) * 1000 / (int(record['Bytes']) * 8))


def ChiSquare(record, linesp):
    # Chi square distribution for 60858 samples is 10625.81, and randomly
    record['Chi Square Value'] = linesp[7][:-1]


def ChiSquarePct(record, linesp):
    # would exceed this value 0.01 percent of the times.
    record['Chi Square %'] = linesp[4]


def ArithmeticMean(record, linesp):
    # Arithmetic mean value of data bytes is 128.8310 (127.5 = random).
    record['Arithmetic Mean'] = linesp[7]


def MonteCarlo(record, linesp):
    # Monte Carlo value for Pi is 3.137138914 (error 0.14 percent).
    record['Monte Carlo Value'] = linesp[6]
    record['Monte Carlo Error %'] = linesp[8]


def SerialCorrelation(record, linesp):
    # Serial correlation coefficient is 0.017649 (totally uncorrelated = 0.0).
    record['Serial Correrelation Coefficient'] = linesp[4]


PARSERS = {
    'Done': Done,
    'Entropy': Entropy,
    'of': Optimum,
    'Chi': ChiSquare,
    'would': ChiSquarePct,
    'Arithmetic': ArithmeticMean,
    'Monte': MonteCarlo,
    'Serial': SerialCorrelation,
}

def writeRecord(record):
    sys.stdout.write(','.join(record[column] for column in FIELDS))
    sys.stdout.write('\n')

def main():
    sys.stdout.write(','.join(FIELDS))
    sys.stdout.write('\n')
    record = {}
    for line in sys.stdin:
        linet = line.strip()
        if not linet:
            continue
        linesp = linet.split()
        if linesp[0] == '--------':
            writeRecord(record)
            record = {}
        elif linesp[0] in PARSERS:
            PARSERS[linesp[0]](record, linesp)

main()
