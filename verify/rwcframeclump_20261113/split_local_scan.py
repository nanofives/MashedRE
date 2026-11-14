"""RWC-FRAME-CLUMP: find provisional RwCore sources whose address-taken scalar locals may stand for one larger
stack buffer (the defect class behind the cup.dff AVs: Ghidra splits a struct / array local into several scalars,
the source passes the first one's address to a callee or walks a pointer from it, and the port's VS2022 frame does
not keep the pieces contiguous).

A finding is `&local_N` (or `pX = &local_N`) where local_N is declared as a scalar and Ghidra's stack naming puts
another declared local in the next 4 bytes above it (local_(N-4) or local_(N-1..N-3), uStack_ / iStack_ alike).
This flags candidates only; each one is checked against the original's disassembly before any edit.

usage: py -3.12 verify/rwcframeclump_20261113/split_local_scan.py [--kinds provisional] [--reach]
"""
import argparse
import csv
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
BODIES = os.path.join(ROOT, 'verify', 'rwccorerun_20261111', 'RwCore_Bodies.tsv')
DECL = re.compile(r'^\s*(?:unsigned\s+|signed\s+)?[A-Za-z_]\w*\s*\**\s*((?:local|[a-z]Stack)_([0-9a-f]+))\s*(\[[^\]]*\])?\s*;', re.M)
ADDR = re.compile(r'&((?:local|[a-z]Stack)_([0-9a-f]+))\b(?!\s*\[)')


def scan(path):
    s = open(path, encoding='utf-8', errors='replace').read()
    decl = {}
    for m in DECL.finditer(s):
        decl[m.group(1)] = (int(m.group(2), 16), bool(m.group(3)))
    offs = {v[0]: k for k, v in decl.items()}
    out = []
    for m in ADDR.finditer(s):
        name = m.group(1)
        if name not in decl or decl[name][1]:
            continue
        off = decl[name][0]
        nb = [offs[o] for o in (off - 1, off - 2, off - 3, off - 4) if o in offs]
        if nb:
            line = s.count('\n', 0, m.start()) + 1
            text = s.splitlines()[line - 1].strip()
            out.append((line, name, ','.join(nb), text[:110]))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--kinds', default='provisional')
    a = ap.parse_args()
    kinds = set(a.kinds.split(','))
    n = 0
    for r in csv.DictReader(open(BODIES, encoding='utf-8'), delimiter='\t'):
        if r['kind'] not in kinds or not r['source'].startswith('re/match/'):
            continue
        p = os.path.join(ROOT, r['source'])
        if not os.path.exists(p):
            continue
        for line, name, nb, text in scan(p):
            print('%s\t%s\t%s:%d\t%s (next: %s)\t%s' % (r['rva'], r['best_pct'], r['source'], line, name, nb, text))
            n += 1
    print('# %d candidate(s)' % n)


if __name__ == '__main__':
    main()
