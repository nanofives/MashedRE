#!/usr/bin/env python3
"""Fields inside the noise floor in every shared band, per collateral.py --mode banded stdout; prints the set difference.
Usage: py -3.12 cross_diff.py <stdout_A> <stdout_B>"""
import re, sys

def within(path):
    txt = open(path, encoding='utf-8', errors='replace').read()
    m = re.search(r'=== WITHIN THE NOISE FLOOR in every shared band: \d+ fields ===\n(.*?)(\n===|\Z)', txt, re.S)
    return set(re.findall(r's\.v\d\.\w+', m.group(1))) if m else set()

a, b = within(sys.argv[1]), within(sys.argv[2])
print('inside the floor in %s only: %s' % (sys.argv[1], sorted(a - b)))
print('inside the floor in %s only: %s' % (sys.argv[2], sorted(b - a)))
