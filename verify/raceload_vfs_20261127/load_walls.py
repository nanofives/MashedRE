"""RACE-VFS-COURSELUA (run 56, decision B second form): count the walls left on the race load.

Input, all read-only:
  re/coverage/reach.tsv           the ORIGINAL's INT3 census (one row per hooks.csv entry VA; README there).
                                  `reached_race_load` = executed in the race_load phase (warp poke DAT_00771968 = 2
                                  until phase 3), majority of runs.
  porttop_bind.py index (--index) rva -> exe bodies, every TU of mashedmod/exe_sources.rsp ('match-include' = a TU that
                                  compiles the rva's re/match source; 'rh' = a hand body registered by RH_ScopedInstall)
  hooks.csv                       confidence and c4= tags
  re/match/**/<rva>.c             a source exists
  re/match/PROVISIONAL.tsv, re/match/ADMITTED.tsv

Each rva of the chosen phase set gets one class (first that applies):
  bound        an exe TU compiles its re/match source (the top-down route has a body for it)
  hand         only a hand exe body (RH_ScopedInstall in an exe TU): runs, but not the original's code
  lib          subsystem third-party-library[lua|msvc-crt|zlib|libpng|qhull] with no exe body: bound by symbol through the
               binder's lib / crt routes when a caller is bound (not counted as a wall, listed apart)
  c4_unbound   hooks.csv C4 and a re/match source, no exe body: a binding wall (data row)
  src_not_c4   a re/match source, not C4 (provisional / admitted / draft): a match or admission wall
  no_source    a hooks.csv row, no re/match source: a source wall
A function with no hooks.csv row is not in the census at all (the census arms hooks.csv VAs only), so `no_row` is a
separate, hand-listed set (--no-row), never inferred.

The census has no call order, so every unbound function of the phase set counts: it is the count of walls between the
start of the load and the race anchor, of which the exe has passed only bound ones (a function the exe executed before
its stop has an exe body by construction).

usage: py -3.12 verify/raceload_vfs_20261127/load_walls.py --index <index.json> [--phase race_load|race_not_menu]
       [--no-row 004b6630,...] [--tsv <out.tsv>]
"""
import argparse
import collections
import csv
import glob
import json
import os
import re
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LIBS = ('lua', 'msvc-crt', 'zlib', 'libpng', 'qhull', 'crt')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--index', required=True)
    ap.add_argument('--phase', default='race_load', choices=('race_load', 'race_not_menu', 'race'))
    ap.add_argument('--no-row', default='')
    ap.add_argument('--tsv')
    a = ap.parse_args()

    idx = json.load(open(a.index, encoding='utf-8'))
    rows = {}
    with open(os.path.join(ROOT, 'hooks.csv'), encoding='utf-8', newline='') as f:
        for r in csv.reader(f):
            if r and re.fullmatch(r'[0-9a-fA-F]{8}', r[0]):
                rows.setdefault(r[0].lower(), r)
    srcs = collections.defaultdict(list)
    for p in glob.glob(os.path.join(ROOT, 're', 'match', '**', '*.c'), recursive=True):
        m = re.fullmatch(r'(?:[A-Za-z0-9]+_)?([0-9a-fA-F]{8})\.c', os.path.basename(p))
        if m:
            srcs[m.group(1).lower()].append(os.path.relpath(p, ROOT).replace(os.sep, '/'))
    listed = {}
    for name in ('PROVISIONAL.tsv', 'ADMITTED.tsv'):
        p = os.path.join(ROOT, 're', 'match', name)
        if os.path.exists(p):
            for ln in open(p, encoding='utf-8').read().splitlines()[1:]:
                f = ln.split('\t')
                if f and re.fullmatch(r'(0x)?[0-9a-fA-F]{8}', f[0].strip()):
                    listed[f[0].strip().lower().replace('0x', '')] = name.split('.')[0].lower()

    phase = []
    with open(os.path.join(ROOT, 're', 'coverage', 'reach.tsv'), encoding='utf-8') as f:
        hdr = f.readline().rstrip('\n').split('\t')
        col = {h: i for i, h in enumerate(hdr)}
        for ln in f:
            v = ln.rstrip('\n').split('\t')
            rva = v[0].lower()
            menu, race, load = v[col['reached_menu']], v[col['reached_race']], v[col['reached_race_load']]
            if (a.phase == 'race_load' and load == '1') or (a.phase == 'race' and race == '1') or \
               (a.phase == 'race_not_menu' and race == '1' and menu == '0'):
                phase.append(rva)

    out = []
    for rva in sorted(set(phase)):
        r = rows.get(rva)
        sub = r[2] if r else ''
        conf = r[3] if r else ''
        notes = r[8] if r and len(r) > 8 else ''
        bodies = idx.get(rva, [])
        hows = {b[2] for b in bodies}
        if 'match-include' in hows:
            cls = 'bound'
        elif 'rh' in hows:
            cls = 'hand'
        elif any(l in sub for l in LIBS):
            cls = 'lib'
        elif conf == 'C4' and srcs.get(rva):
            cls = 'c4_unbound'
        elif srcs.get(rva):
            cls = 'src_not_c4'
        else:
            cls = 'no_source'
        c4 = (re.search(r'c4=([a-z-]+)', notes) or [None, ''])[1] if conf == 'C4' else ''
        out.append((rva, cls, conf, c4, sub, listed.get(rva, ''), ';'.join(srcs.get(rva, [])[:2]),
                    ';'.join('%s@%s' % (b[0], b[1]) for b in bodies[:2])))
    no_row = [x.strip().lower() for x in a.no_row.split(',') if x.strip()]

    cnt = collections.Counter(o[1] for o in out)
    print('phase %s: %d functions of the original (re/coverage/reach.tsv)' % (a.phase, len(out)))
    for k in ('bound', 'hand', 'lib', 'c4_unbound', 'src_not_c4', 'no_source'):
        print('  %-11s %5d' % (k, cnt.get(k, 0)))
    walls = cnt.get('hand', 0) + cnt.get('c4_unbound', 0) + cnt.get('src_not_c4', 0) + cnt.get('no_source', 0)
    print('  walls (hand + c4_unbound + src_not_c4 + no_source) %d, plus no_row (not in the census) %d listed: %s'
          % (walls, len(no_row), ' '.join(no_row) or '-'))
    sn = collections.Counter(o[5] or 'none' for o in out if o[1] == 'src_not_c4')
    print('  src_not_c4 by listing: %s' % dict(sn))
    bs = collections.Counter((o[1], o[4].split('[')[0]) for o in out if o[1] not in ('bound', 'lib'))
    print('  walls by subsystem: %s' % ', '.join('%s/%s %d' % (k[0], k[1], v) for k, v in sorted(bs.items())))
    if a.tsv:
        with open(a.tsv, 'w', encoding='utf-8', newline='') as f:
            f.write('rva\tclass\tconfidence\tc4\tsubsystem\tlisted\tsource\texe_body\n')
            for o in out:
                f.write('\t'.join(o) + '\n')
            for r in no_row:
                f.write('%s\tno_row\t\t\t\t\t\t\n' % r)
        print('wrote %s' % a.tsv)


if __name__ == '__main__':
    sys.exit(main())
