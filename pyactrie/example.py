#!/usr/bin/env python

import sys
import pyactrie

t = pyactrie.actrie()

with open('forbid.txt') as f:
    for line in f:
        line = line.strip('\n')
        if line != '':
            t.insert(line)

t.build()

for line in sys.stdin:
    print(t.process(line))

