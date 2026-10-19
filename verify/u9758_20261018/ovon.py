"""List AI override-timer onsets (ai_override rising) per car in aistep / step CSVs.
FUN_00422ba0 calls FUN_00417370 (timer 1440) for each car of a drained type-4 contact with
FUN_0040e470 != 1, so an AI<->AI contact sets two AI cars' timers on the same frame (AA) and a
player contact sets one (P?).
Each onset prints as frame:kind cars @k, k = the car's call index relative to its (b) window
start (first call with c4 != 0, re/tools/ai_ctrl_window.py); '*' = inside [0, 220).
usage: ovon.py <dir-or-csv>...  [--glob PATTERN]"""
import csv, sys, os, glob, collections


def scan(path):
    rows = collections.defaultdict(list)
    for r in csv.DictReader(open(path, newline='')):
        rows[int(r['v'])].append(r)
    on = []
    for v, rs in rows.items():
        i0 = next((i for i, x in enumerate(rs) if int(x['c4']) != 0), None)
        p = 0
        for i, r in enumerate(rs):
            ov = int(float(r['ai_override']))
            if ov > p:
                on.append((int(r['frame']), v, None if i0 is None else i - i0, ov))
            p = ov
    return on


def fmt(on):
    byf = collections.defaultdict(list)
    for f, v, k, ov in on:
        byf[f].append((v, k, ov))
    parts = []
    for f in sorted(byf):
        cs = sorted(byf[f])
        ks = [k for _, k, _ in cs]
        inw = any(k is not None and 0 <= k < 220 for k in ks)
        kind = 'AA' if len(cs) >= 2 else 'P?'
        parts.append('%s%d:%s%s@%s(%d)' % ('*' if inw else '', f, kind, '+'.join(str(v) for v, _, _ in cs),
                                           '/'.join(str(k) for k in ks), cs[0][2]))
    return '  '.join(parts)


def main(args):
    pat = '*.aistep.csv'
    if '--glob' in args:
        pat = args[args.index('--glob') + 1]
        args = [a for i, a in enumerate(args) if a != '--glob' and (i == 0 or args[i - 1] != '--glob')]
    files = []
    for a in args:
        files += sorted(glob.glob(os.path.join(a, pat))) if os.path.isdir(a) else [a]
    for fp in files:
        print('%-26s %s' % (os.path.basename(fp).split('.')[0], fmt(scan(fp))))


if __name__ == '__main__':
    main(sys.argv[1:])
