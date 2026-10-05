#!/usr/bin/env python3
"""Decode and compare the CHAMPIONSHIP SPAN of two gamesave.bin files.

PRE-REGISTRATION: verify/d4_save_20261005/PREREG_SPAN.md.

WHY. verify/d4_save_20261005/RESULT_SAVE.md reported 345 bytes differing between
original/gamesave.bin and the standalone's mashed_re_gamesave.bin, "entirely inside
the tail", relying on re/tools/gamesave_parse.py's region table (profile 0x0004 +
0x2443C, so a "tail" from 0x24440 that SerializeToBuffer does not write). That
framing is WRONG. re/tools/gamesave_edit.py:9-14 documents the CHAMPIONSHIP SPAN at
0x24A40, 13 rows of 0x30 bytes, and the divergence begins at 0x24a44 = span row 0
column 1, with the 0x30 row stride matching the repeating value this tool prints.
So the differing bytes are PROGRESSION STATE, not an un-serialized tail.

Layout is taken ONLY from gamesave_edit.py's header. Unnamed columns are printed as
raw dwords and given no semantic name (NO-GUESSING).

Usage:
  py -3.12 re/tools/gamesave_spandiff.py <ref.bin> <subject.bin> [--json]
"""
import argparse
import json
import struct

SIZE = 0x24FA0
MAGIC = 0xDEADBEEF
SPAN_BASE = 0x24A40          # gamesave_edit.py:10
ROWS = 13                    # gamesave_edit.py:10
STRIDE = 0x30                # gamesave_edit.py:10
COLS = 12                    # gamesave_edit.py:10
SPAN_END = SPAN_BASE + ROWS * STRIDE

# gamesave_edit.py:11-14. Only these four have a documented meaning.
NAMED = {1: 'col1 challenge-cup launch gate',
         3: 'col3 cup membership',
         4: 'col4 track known',
         11: 'col11 quick-battle launch gate'}


def load(p):
    b = open(p, 'rb').read()
    assert len(b) == SIZE, '%s: size %d, want %d' % (p, len(b), SIZE)
    m, = struct.unpack_from('<I', b, 0)
    assert m == MAGIC, '%s: magic 0x%08X, want 0x%08X' % (p, m, MAGIC)
    return b


def span(b):
    return [[struct.unpack_from('<I', b, SPAN_BASE + r * STRIDE + c * 4)[0]
             for c in range(COLS)] for r in range(ROWS)]


def ka_layout(rows):
    """Validate gamesave_edit.py's documented invariants ON THE REFERENCE FILE."""
    c4 = sum(1 for r in rows if r[4] == 2)
    c3 = sum(1 for i in range(4) if rows[i][3] == 2)
    c1 = sum(1 for r in rows if r[1] != 0)
    c11 = sum(1 for r in rows if r[11] != 0)
    return {'col4_eq2': '%d of %d' % (c4, ROWS), 'col4_pass': c4 == ROWS,
            'col3_eq2_rows0_3': '%d of 4' % c3, 'col3_pass': c3 == 4,
            'col1_nonzero_rows': '%d of %d' % (c1, ROWS), 'col1_pass': c1 == 1,
            'col11_nonzero_rows': '%d of %d' % (c11, ROWS), 'col11_pass': c11 == 1,
            'pass': c4 == ROWS and c3 == 4 and c1 == 1 and c11 == 1}


def run(refp, subp):
    a, b = load(refp), load(subp)
    ra, rb = span(a), span(b)
    out = {'ref': refp, 'subject': subp,
           'span': {'base': hex(SPAN_BASE), 'end': hex(SPAN_END),
                    'rows': ROWS, 'stride': hex(STRIDE), 'cols': COLS},
           'ka_layout': ka_layout(ra)}

    # G-REGION — how much of the divergence the span actually explains
    db = [i for i in range(SIZE) if a[i] != b[i]]
    dd = [o for o in range(0, SIZE - 3, 4)
          if struct.unpack_from('<I', a, o)[0] != struct.unpack_from('<I', b, o)[0]]
    inb = [i for i in db if SPAN_BASE <= i < SPAN_END]
    out['g_region'] = {
        'differing_bytes': len(db), 'differing_dwords': len(dd),
        'bytes_inside_span': len(inb), 'bytes_outside_span': len(db) - len(inb),
        'first_diff': hex(db[0]) if db else None, 'last_diff': hex(db[-1]) if db else None,
        'outside_span_range': ([hex(min(i for i in db if not (SPAN_BASE <= i < SPAN_END))),
                                hex(max(i for i in db if not (SPAN_BASE <= i < SPAN_END)))]
                               if len(db) - len(inb) else None),
        'note': 'descriptive, no pass/fail; bounds what the span explains'}

    # G-PROG — the four named columns, per row, both files
    tbl = []
    for r in range(ROWS):
        tbl.append({'row': r,
                    'ref':  {k: ra[r][k] for k in NAMED},
                    'subj': {k: rb[r][k] for k in NAMED},
                    'differs': ra[r] != rb[r]})
    out['g_prog'] = {'rows': tbl,
                     'ref_counts': {NAMED[k]: sum(1 for r in ra if r[k]) for k in NAMED},
                     'subj_counts': {NAMED[k]: sum(1 for r in rb if r[k]) for k in NAMED}}

    # G-VERDICT, fixed in the pre-registration before this ran
    zero_rows = sum(1 for r in rb if all(r[k] == 0 for k in NAMED))
    bad_col4 = [r for r in range(ROWS) if rb[r][4] not in (0, 2)]
    out['g_verdict'] = {'subject_rows_all_named_zero': '%d of %d' % (zero_rows, ROWS),
                        'subject_col4_out_of_domain_rows': bad_col4}
    if bad_col4:
        out['g_verdict']['verdict'] = ('FORMAT-DIFFERENCE -- subject col4 takes values outside '
                                       '{0,2} on rows %s' % bad_col4)
    elif zero_rows >= 12:
        out['g_verdict']['verdict'] = (
            'CONTENT-DIFFERENCE -- the format matches and the subject simply describes LESS '
            'progression; the original would read it as a near-empty profile, not reject it')
    else:
        out['g_verdict']['verdict'] = 'INCONCLUSIVE'
    return out


def report(o):
    print('ref     %s' % o['ref'])
    print('subject %s' % o['subject'])
    s = o['span']
    print('span    %s..%s, %d rows x %s, %d cols   (gamesave_edit.py:10)'
          % (s['base'], s['end'], s['rows'], s['stride'], s['cols']))

    k = o['ka_layout']
    print('\nKA-LAYOUT on the REFERENCE file (must pass before anything is read across)')
    print('   col4 == 2 on %-8s  %s' % (k['col4_eq2'], 'PASS' if k['col4_pass'] else 'FAIL'))
    print('   col3 == 2 on rows 0..3: %-5s %s' % (k['col3_eq2_rows0_3'],
                                                  'PASS' if k['col3_pass'] else 'FAIL'))
    print('   col1 non-zero on %-8s  %s (want exactly 1)'
          % (k['col1_nonzero_rows'], 'PASS' if k['col1_pass'] else 'FAIL'))
    print('   col11 non-zero on %-7s  %s (want exactly 1)'
          % (k['col11_nonzero_rows'], 'PASS' if k['col11_pass'] else 'FAIL'))
    print('   KA-LAYOUT %s' % ('PASS' if k['pass'] else 'FAIL -- leg VOID'))
    if not k['pass']:
        return

    g = o['g_region']
    print('\nG-REGION (descriptive)')
    print('   %d differing bytes / %d differing dwords, %s..%s'
          % (g['differing_bytes'], g['differing_dwords'], g['first_diff'], g['last_diff']))
    print('   inside the span:  %d of %d' % (g['bytes_inside_span'], g['differing_bytes']))
    print('   outside the span: %d of %d%s'
          % (g['bytes_outside_span'], g['differing_bytes'],
             ('  range %s..%s' % tuple(g['outside_span_range'])) if g['outside_span_range'] else ''))

    p = o['g_prog']
    print('\nG-PROG  the four DOCUMENTED columns (unnamed columns deliberately not interpreted)')
    print('   row |      col1      |      col3      |      col4      |     col11      ')
    print('       |  ref    subj   |  ref    subj   |  ref    subj   |  ref    subj   ')
    for r in p['rows']:
        print('   %3d | %5d  %5d   | %5d  %5d   | %5d  %5d   | %5d  %5d  %s'
              % (r['row'], r['ref'][1], r['subj'][1], r['ref'][3], r['subj'][3],
                 r['ref'][4], r['subj'][4], r['ref'][11], r['subj'][11],
                 '*' if r['differs'] else ''))
    print('   rows non-zero, reference: %s' % p['ref_counts'])
    print('   rows non-zero, subject  : %s' % p['subj_counts'])

    v = o['g_verdict']
    print('\nG-VERDICT  subject rows with all four named columns zero: %s'
          % v['subject_rows_all_named_zero'])
    print('   %s' % v['verdict'])


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument('ref')
    ap.add_argument('subject')
    ap.add_argument('--json', action='store_true')
    a = ap.parse_args(argv)
    o = run(a.ref, a.subject)
    print(json.dumps(o, indent=1)) if a.json else report(o)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
