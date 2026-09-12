"""Caller-screened promotion-candidate picker.

WHY THIS EXISTS (promote-round r253, 2026-09-12). A clean GREEN diff does not make a
C3. `re/CONFIDENCE.md`'s C2->C3 rule needs at least one caller at C2+ OR an
*identified* caller -- one with a recovered real name or a documented library role.
It says explicitly that an anonymous, un-analyzed `FUN_xxxxxxxx` caller does NOT
satisfy it, because promoting on one is an island promotion.

`re/analysis/plans/c3_filter_v4.py` does not apply that check, so it happily passes
rows that cannot be promoted however clean their diff. `0x004f10e0` cost a full
author + build + path1 + path2 cycle before that surfaced: both its callers are
anonymous (`FUN_004e4300` at C1, `FUN_004e41e0` with no hooks.csv row at all), so it
stayed C2 with its evidence banked.

On the r254 cached batch this screen kept 67 of 102 rows and rejected 35 -- a 34%
waste rate avoided. It was validated by confirming it flags `0x004f10e0`.

USAGE
  py -3.12 re/tools/decomp_pc.py --file rvas.txt --callers --json -o batch.json
  py -3.12 re/tools/caller_screen.py batch.json

Read-only: touches nothing but hooks.csv and the JSON you pass it.
"""
import csv
import io
import json
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
REPO = os.path.join(REPO, 'Mashed') if not os.path.exists(os.path.join(REPO, 'hooks.csv')) else REPO

if len(sys.argv) < 2:
    sys.exit(__doc__)
BATCH = sys.argv[1]

raw = open(os.path.join(REPO, 'hooks.csv'), 'rb').read().decode('utf-8')
H = {}
for r in csv.reader(io.StringIO(raw, newline='')):
    if r and re.fullmatch(r'[0-9a-fA-F]{8}', r[0].strip()):
        H[r[0].strip().lower()] = {'name': r[1], 'conf': r[3]}

J = json.load(open(BATCH, encoding='utf-8'))

ANON = re.compile(r'^(FUN_|LAB_|thunk_FUN_|sub_)')


def qualifies(caller_key):
    """C2+ counts. So does an identified name (not a bare FUN_/LAB_/thunk_/sub_)."""
    ci = H.get(caller_key)
    if not ci:
        return None                       # no row at all -> anonymous
    if ci['conf'] in ('C2', 'C3', 'C4'):
        return '%s %s %s' % (caller_key, ci['name'], ci['conf'])
    if not ANON.match(ci['name']):
        return '%s %s %s(named)' % (caller_key, ci['name'], ci['conf'])
    return None


out = []
for f in J.get('functions', []):
    a = (f.get('address') or f.get('va') or f.get('requested') or '').lower()
    if not a:
        continue
    k = a.replace('0x', '')
    me = H.get(k)
    if not me or me['conf'] != 'C2':
        continue
    good = []
    for c in (f.get('callers') or []):
        m = re.search(r'0x([0-9a-fA-F]{8})', str(c))
        if m:
            q = qualifies(m.group(1).lower())
            if q:
                good.append(q)
    body = re.sub(r'/\*.*?\*/', '', f.get('decomp') or '', flags=re.S)
    nlines = len([l for l in body.split('\n') if l.strip()])
    out.append((bool(good), nlines, a, me['name'], len(f.get('callers') or []), good))

promotable = sorted([o for o in out if o[0]], key=lambda o: o[1])
blocked = sorted([o for o in out if not o[0]], key=lambda o: o[1])

print('C2 rows in batch: %d   promotable (caller gate OK): %d   caller-blocked: %d'
      % (len(out), len(promotable), len(blocked)))
print()
print('--- PROMOTABLE, smallest first ---')
for _, n, a, nm, nc, good in promotable[:30]:
    print('%3d lines  %s  %-26s callers=%d  ok=%s' % (n, a, nm, nc, good[0]))
print()
print('--- CALLER-BLOCKED: do NOT author these, the C3 would be refused ---')
for _, n, a, nm, nc, _g in blocked[:20]:
    print('%3d lines  %s  %-26s callers=%d' % (n, a, nm, nc))
