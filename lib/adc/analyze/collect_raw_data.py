
import subprocess
import sys

def main():
    for line in sys.stdin:
        if not line.startswith('Done'):
            continue
        sys.stdout.write(line)
        sys.stdout.flush()
        _, rounds, _ = line.split()
        subprocess.check_call(['ent', 'out/entropy-%s.bin' % rounds], stdout=sys.stdout)
        sys.stdout.write('--------\n')
        sys.stdout.flush()

main()

