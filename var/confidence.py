#!/usr/bin/python
'''
confidence.py

"THE BEER-WARE LICENSE" (Revision 42):
<filipeutzig@gmail.com> wrote this file. As long as you retain this
notice you can do whatever you want with this stuff. If we meet some
day, and you think this stuff is worth it, you can buy me a beer in
return.

Initial version by Filipe Utzig <filipeutzig@gmail.com> on 4/8/15.

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
"SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in
this document are to be interpreted as described in RFC 2119.

Script that receive a number of elements and calculate the confidence
interval of data
'''

import sys
import numpy as np
from scipy import stats

def print_usage():
    print sys.argv[0] + "<input_file> <output_file>"
    print ""
    print "e.g." + sys.argv[0] + "my_data.txt results.txt"

def _parse_comment(line):
    #TODO

def mean_confidence_interval(data, confidence = 0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    return m, m-h, m+h

def main(args):
    if len(args) != 3:
        print_usage()
        exit()

    confidence = 0.99
    data_file = args[1]
    output_file = args[2]
    data = {}


    _file = open(data_file, 'r')
    for line in _file:
        if line[0] == '#':
            _parse_comment(line)

        data.append(line)


if __name__ == '__main__':
    main(sys.argv)

