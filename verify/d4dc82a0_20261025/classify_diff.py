"""Split every non-matched row of drawlist_diff --json output into checker rows (all four vertex colours in
{0x00000000, 0xff808080}: the animated background, 42 rows per frame) and everything else. Prints per file:
matched, checker non-matched, other non-matched, and the other rows themselves."""
import json, sys
CHK = {0, 0xff808080}
def colours(e):
    if 'a' in e or 'b' in e:   # mismatched pair
        return [c for side in ('a', 'b') if side in e for c in e[side].get('colors', [])]
    return e.get('colors', [])
for path in sys.argv[1:]:
    d = json.load(open(path)); m = chk = 0; other = []
    for fr, v in d.items():
        m += v['matched']
        for kind in ('mismatched', 'missing', 'extra'):
            for e in v.get(kind, []):
                cs = colours(e)
                if cs and all(c in CHK for c in cs): chk += 1
                else: other.append((fr, kind, round(e.get('x', e.get('a', {}).get('x', 0)), 2),
                                    round(e.get('y', e.get('a', {}).get('y', 0)), 2), e.get('rets', e.get('a', {}).get('rets', []))[:2]))
    print(f'{path}: matched={m} checker_nonmatched={chk} other_nonmatched={len(other)}')
    for o in other[:12]: print('   ', o)
