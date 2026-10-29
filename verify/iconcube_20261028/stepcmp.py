"""First differing rows between two MASHED_AI_STEPDUMP csv files.

Usage: py -3.12 stepcmp.py <a.step.csv> <b.step.csv> [n=3]
Rows are paired by line index. Prints the row count of each file, then for the first n differing lines the
frame / seq / v and every column whose value differs (a -> b).
"""
import sys


def main():
    a_path, b_path = sys.argv[1], sys.argv[2]
    n = int(sys.argv[3]) if len(sys.argv) > 3 else 3
    with open(a_path) as f:
        a = f.read().splitlines()
    with open(b_path) as f:
        b = f.read().splitlines()
    print('rows', len(a) - 1, len(b) - 1)
    hdr = a[0].split(',')
    shown = 0
    for i in range(1, min(len(a), len(b))):
        if a[i] == b[i]:
            continue
        ra, rb = a[i].split(','), b[i].split(',')
        diffs = ['%s %s -> %s' % (hdr[k] if k < len(hdr) else k, ra[k] if k < len(ra) else '', rb[k] if k < len(rb) else '')
                 for k in range(max(len(ra), len(rb)))
                 if (ra[k] if k < len(ra) else None) != (rb[k] if k < len(rb) else None)]
        print('line %d frame %s seq %s v %s: %s' % (i, ra[0], ra[1], ra[2], '; '.join(diffs[:12])))
        shown += 1
        if shown >= n:
            break
    if shown == 0:
        print('no differing line in the common prefix')


if __name__ == '__main__':
    main()
