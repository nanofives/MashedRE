"""List the d1r1 cited rows of given cite groups, per frame (rfx lane helper).

usage: py -3.12 verify/rfx_20261016/rows_of.py [cited.json ...] [--cite REGEX]
Default: the 9 d1r1 evidence diffs, cites C04/C08/C09.
"""
import glob
import json
import os
import re
import sys

args = [a for a in sys.argv[1:] if not a.startswith('--')]
cite = 'C04|C08|C09'
if '--cite' in sys.argv:
    cite = sys.argv[sys.argv.index('--cite') + 1]
    args = [a for a in args if a != cite]
here = os.path.dirname(os.path.abspath(__file__))
if not args:
    args = sorted(glob.glob(os.path.join(here, '..', 'd1r1_20261016', 'evidence', '*_cited_stand_rb_*.json')))
rx = re.compile(cite)
tot = {}
for p in args:
    d = json.load(open(p))
    rows = [r for r in d['rows'] if r.get('cite') and rx.search(r['cite'])]
    print('== %s  rows=%d (cited in group: %d)' % (os.path.basename(p), len(d['rows']), len(rows)))
    for r in rows:
        tot[r['cite']] = tot.get(r['cite'], 0) + 1
        print('  %-14s %-8s %-18s n=%s off=%s  %s' % (r['cite'], r['kind'], r['tex'], r.get('n'), r.get('offscreen'), r.get('detail', '')[:260]))
print('totals', tot)
