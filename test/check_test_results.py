# checks the run tests against the available ones

import subprocess
import sys

def ParseLastRun(filename):
    with open(filename) as f:
        lines = (l.strip() for l in f)
        running_lines = (l for l in lines if l.startswith('Running '))
        return list(t.split()[1].replace(':', '') for t in running_lines)
            

def ParseTestObject(filename):
    lines = subprocess.check_output(['nm', filename], encoding='utf8').split('\n')
    split_lines = (l.split() for l in lines)
    defined = (l[2] for l in split_lines if len(l) >= 3 and l[1] == 'T')
    remove_underscore = lambda l: l[1:] if l.startswith('_') else l
    underscore_removed = (remove_underscore(l) for l in defined)
    return list(l for l in underscore_removed if l.startswith('test_'))

def Main(args):
    if len(args) != 3:
        sys.exit('Usage: python check_test_results.py last_test_run.txt my_test.o')

    last_run_list = set(ParseLastRun(args[1]))
    tests_in_object = set(ParseTestObject(args[2]))

    tests_not_run = tests_in_object.difference(last_run_list)
    if tests_not_run:
        sys.exit('The following tests seem to be missing from %s\'s main() '
                 'function: %s' % (args[2], ', '.join(sorted(tests_not_run))))

    extra_tests = last_run_list.difference(tests_in_object)
    if extra_tests:
        sys.exit('The following tests were run but not found in %s (???): %s' %
                 (args[2], ', '.join(sorted(extra_tests))))

    sys.stdout.write('Ran %u/%u tests sucessfully.\n' %
                     (len(last_run_list), len(tests_in_object)))

if __name__ == '__main__':
    Main(sys.argv)
