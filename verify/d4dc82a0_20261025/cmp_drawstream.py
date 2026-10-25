"""Compare two standalone MASHED_DBG_DRAWSTREAM dumps field by field, ignoring the return-address list `r`
(the code layout moves between builds). Prints the frame/draw counts and every other differing field."""
import json, sys
a = json.load(open(sys.argv[1])); b = json.load(open(sys.argv[2]))
diffs = []; draws = 0
def walk(x, y, path):
    if type(x) != type(y): diffs.append(('type', path)); return
    if isinstance(x, dict):
        for k in sorted(set(x) | set(y)):
            if k == 'r': continue
            if k not in x or k not in y: diffs.append(('key', path + '/' + k)); continue
            walk(x[k], y[k], path + '/' + k)
    elif isinstance(x, list):
        if len(x) != len(y): diffs.append(('len', path, len(x), len(y))); return
        for n, (i, j) in enumerate(zip(x, y)): walk(i, j, f'{path}[{n}]')
    elif x != y: diffs.append(('val', path, x, y))
for f in sorted(set(a) | set(b)):
    if f in a and f in b and isinstance(a[f], list): draws += len(a[f])
    walk(a.get(f), b.get(f), f)
# checker draws (vertex = x y z rhw colour u v, 28 bytes; colour at byte 16): colour bytes are only 00000000 / 808080ff (the animated background)
def checker(v):
    cols = {v[i:i + 8] for i in range(32, len(v), 56)} if isinstance(v, str) else set()
    return bool(cols) and cols <= {'00000000', '808080ff', '00808080'}
chk = sum(1 for d in diffs if d[0] == 'val' and d[1].endswith('/v') and checker(d[2]) and checker(d[3]))
print(f'frames {len(a)}/{len(b)} draws {draws} non-r diffs {len(diffs)} (checker vertex-only {chk}, other {len(diffs) - chk})')
for d in diffs:
    if not (d[0] == 'val' and d[1].endswith('/v') and checker(d[2]) and checker(d[3])): print(d)
