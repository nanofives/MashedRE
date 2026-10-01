#!/usr/bin/env python3
"""a12_clamp.py - D2 attempt 12 step 3, confirmation leg. Decide WHERE in the
original's frame `+0x9e4` stops matching `|+0x9b0..0x9b8|`, and read the port's
clamp-#6 arm / grip / k in the same speed bands.

Registered naming bar, PREREG_3.md section 4: a writer is named only if its gate
condition is checked ON THE RUNNING ORIGINAL through an entry-only hook or an
existing captured channel, with a count. This is that check, and it needs no new
run: scenario_launch --fixup-probe already samples `+0x9e4` and `+0x9b0..0x9b8`
TOGETHER at five entry sites per frame, so `speed / |vel|` is a single-row
statistic at each site, with no join at all.

  site 2  0x00467650  A6a entry          +0x9e4 still holds s_mid(f-1)
  site 1  0x004709a0  substep entry      the first sample AFTER A6a returns
  site 3  0x0046f6c0  substep member
  site 4  0x00469aa0  substep member
  site 0  0x0046ef70  contact fixup entry

If the original's clamp #6 shrank the velocity after the `0x004686cc` latch, the
site-1 ratio would be well above 1. If it is 1.000 the clamp did not shrink it.

Usage: a12_clamp.py --csv <orig.fixupprobe.csv> [--dump <port a6a_dump.log>]
"""
import csv, math, re, sys

from a11_accum import med, BANDS

SITE_NAME = {0: 'fixup  0x0046ef70', 1: 'substep 0x004709a0',
             2: 'A6a-ent 0x00467650', 3: 'wheel  0x0046f6c0',
             4: 'worldc 0x00469aa0'}


def mag(v):
    return math.sqrt(sum(x * x for x in v))


def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    rows = []
    for r in csv.DictReader(open(a['csv'], newline='')):
        v = (float(r['velx']), float(r['vely']), float(r['velz']))
        rows.append(dict(seq=int(r['seq']), site=int(r['site']),
                         frame=int(r['frame']), v=v, m=mag(v),
                         sp=float(r['speed']), gnd=float(r['gnd'])))
    rows.sort(key=lambda x: x['seq'])
    print('probe rows %d' % len(rows))

    print('\n=== ORIGINAL: +0x9e4 / |+0x9b0..b8| at each entry site, by band of |vel| ===')
    print('(a value of 1.000 means nothing has changed the velocity since +0x9e4 '
          'was last written)')
    hdr = 'site                  band          n |   med ratio    p95 ratio   med |vel|'
    print(hdr)
    print('-' * len(hdr))
    for site in (2, 1, 3, 4, 0):
        any_row = False
        for lo, hi in BANDS:
            b = [r for r in rows if r['site'] == site and lo <= r['m'] < hi
                 and abs(r['gnd'] - 4.0) < 1e-6 and r['m'] > 1e-6]
            if len(b) < 10:
                continue
            rt = sorted(r['sp'] / r['m'] for r in b)
            print('%-21s %-11s %4d | %10.6f %12.6f %11.1f'
                  % (SITE_NAME[site] if not any_row else '', '%d-%d' % (lo, hi),
                     len(b), med(rt), rt[int(0.95 * len(rt))],
                     med([r['m'] for r in b])))
            any_row = True
        if not any_row:
            print('%-21s %s' % (SITE_NAME[site], 'no band with n>=10'))
        print('')

    # first site-1 row that follows each site-2 row, i.e. the sample immediately
    # after A6a returns, with a COUNT of how often one exists (coverage check).
    nfollow, nmiss = 0, 0
    follow = []
    for i, r in enumerate(rows):
        if r['site'] != 2:
            continue
        for j in range(i + 1, min(i + 8, len(rows))):
            if rows[j]['site'] == 2:
                break
            if rows[j]['site'] == 1:
                follow.append(rows[j]); nfollow += 1
                break
        else:
            nmiss += 1
    print('[coverage] site-2 rows with a following site-1 row before the next '
          'site-2: %d, without: %d' % (nfollow, nmiss))
    for lo, hi in BANDS:
        b = [r for r in follow if lo <= r['m'] < hi and abs(r['gnd'] - 4.0) < 1e-6]
        if len(b) < 10:
            continue
        print('   first-after-A6a  %-11s n=%4d  med ratio %.6f  med |vel| %.1f'
              % ('%d-%d' % (lo, hi), len(b),
                 med([r['sp'] / r['m'] for r in b]), med([r['m'] for r in b])))

    if 'dump' not in a:
        return

    print('\n=== PORT: clamp #6 arm / grip / kVel from a6a_dump.log ===')
    pat = re.compile(r'snap\.vel=([^ ]+) .*?snap\.sp=([-\d.e+]+) .*?snap\.gc=([-\d.e+]+)'
                     r'.*?act\.speed=([-\d.e+]+) act\.grip=([-\d.e+]+) '
                     r'act\.kvel=([-\d.e+]+) act\.kav=([-\d.e+]+) act\.arm=(-?\d+) '
                     r'act\.clamp=(\d+)')
    pr = []
    for ln in open(a['dump'], 'r', errors='replace'):
        m = pat.search(ln)
        if not m:
            continue
        v = tuple(float(x) for x in m.group(1).split(','))
        pr.append(dict(m=mag(v), sp=float(m.group(2)), gc=float(m.group(3)),
                       aspeed=float(m.group(4)), grip=float(m.group(5)),
                       kvel=float(m.group(6)), kav=float(m.group(7)),
                       arm=int(m.group(8)), clamp=int(m.group(9))))
    print('port dump rows %d' % len(pr))
    hdr = ('band          n | clampRan  arm=0  arm=1 |   med grip     med kVel'
           '   med sp/|vel|   med |vel|')
    print(hdr)
    print('-' * len(hdr))
    for lo, hi in BANDS:
        b = [r for r in pr if lo <= r['m'] < hi and abs(r['gc'] - 4.0) < 1e-6
             and r['m'] > 1e-6]
        if len(b) < 10:
            continue
        print('%-13s %4d | %8d %6d %6d | %10.4g %12.6f %13.6f %11.1f'
              % ('%d-%d' % (lo, hi), len(b),
                 sum(1 for r in b if r['clamp']),
                 sum(1 for r in b if r['arm'] == 0),
                 sum(1 for r in b if r['arm'] == 1),
                 med([r['grip'] for r in b]), med([r['kvel'] for r in b]),
                 med([r['sp'] / r['m'] for r in b]), med([r['m'] for r in b])))


if __name__ == '__main__':
    main(sys.argv[1:])
