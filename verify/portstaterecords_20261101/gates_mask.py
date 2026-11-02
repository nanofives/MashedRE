# MATCH-PORTSTATE-RECORDS P3 / P4 witness on the H arm gates.csv (U9186GateDump).
# Columns 5..9 (1-based) read the original's absolute record addresses, columns 10..14 the same fields on the port
# record. P3: in the knob-ON file abs == rec on every row; in the knob-OFF file abs is 0 on every row.
# P4 (masked): with abs replaced by rec in both files, the two files are equal row for row.
# Usage: py -3.12 gates_mask.py <H_DEF.gates.csv> <H_PSR.gates.csv>
import sys

ABS = range(4, 9)    # 0-based 4..8 = veh_type_abs, state0_abs, rec_t_abs, rec_x_abs, rec_z_abs
REC = range(9, 14)   # 0-based 9..13 = the _rec twins


def rows(path):
    with open(path, newline='') as f:
        lines = f.read().splitlines()
    return lines[0], [ln.split(',') for ln in lines[1:]]


def num(s):
    try:
        return float(s)
    except ValueError:
        return s


def main():
    h0, off = rows(sys.argv[1])
    h1, on = rows(sys.argv[2])
    rc = 0
    print('header equal:', h0 == h1)
    print('rows off/on:', len(off), len(on))
    off_nonzero = sum(1 for r in off if any(num(r[i]) != 0 for i in ABS))
    on_mismatch = [r for r in on if any(num(r[a]) != num(r[b]) for a, b in zip(ABS, REC))]
    on_nonzero = sum(1 for r in on if any(num(r[i]) != 0 for i in ABS))
    print('P3 off: rows with a nonzero abs column:', off_nonzero)
    print('P3 on : rows with abs != rec:', len(on_mismatch), ' rows with a nonzero abs column:', on_nonzero)
    for r in on_mismatch[:5]:
        print('   ', ','.join(r[:14]))
    if off_nonzero or on_mismatch:
        rc = 1

    def masked(r):
        r = list(r)
        for a, b in zip(ABS, REC):
            r[a] = r[b]
        return r
    n = min(len(off), len(on))
    diff = [(i, c) for i in range(n) for c in range(max(len(off[i]), len(on[i])))
            if (masked(off[i])[c:c + 1] != masked(on[i])[c:c + 1])]
    print('P4 masked: differing cells:', len(diff), ' row count equal:', len(off) == len(on))
    if diff:
        i, c = diff[0]
        print('   first: row', i + 2, 'col', c + 1, h0.split(',')[c] if c < len(h0.split(',')) else '?',
              'off=', masked(off[i])[c:c + 1], 'on=', masked(on[i])[c:c + 1])
        cols = sorted({c for _, c in diff})
        print('   columns:', [h0.split(',')[c] if c < len(h0.split(',')) else c for c in cols][:20])
        rc = 1
    if len(off) != len(on):
        rc = 1
    print('rc', rc)
    return rc


if __name__ == '__main__':
    sys.exit(main())
