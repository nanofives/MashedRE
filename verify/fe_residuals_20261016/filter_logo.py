"""Drop draws whose first return address resolves (MSVC map) to LogoOverlayDraw. Usage: filter_logo.py in.json map out.json"""
import json, sys, bisect
sys.path.insert(0, 're/tools')
from drawlist_diff import load_msvc_map
addrs, names = load_msvc_map(sys.argv[2])
d = json.load(open(sys.argv[1])); out = {}
drop = 0
for k, rows in d.items():
    keep = []
    for r in rows:
        ret = int((r.get('r') or r.get('rets') or ['0'])[0], 16)
        i = bisect.bisect_right(addrs, ret) - 1
        if i >= 0 and names[i] == 'LogoOverlayDraw': drop += 1; continue
        keep.append(r)
    out[k] = keep
json.dump(out, open(sys.argv[3], 'w'))
print('dropped', drop)
