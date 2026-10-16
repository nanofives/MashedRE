"""print chosen key= fields of motion_diag lines by series index (a8_launch port_series indexing:
only lines carrying both sp= and b14=). usage: md_win.py <log> <lo> <hi> key [key ...]"""
import re, sys
path, lo, hi = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
keys = sys.argv[4:]
idx = -1
for line in open(path, errors='replace'):
    if 'sp=' not in line or 'b14=' not in line:
        continue
    idx += 1
    if idx < lo: continue
    if idx > hi: break
    out = []
    for k in keys:
        m = re.search(r'(?:^|\s)' + re.escape(k) + r'=(\[[^\]]*\]|\([^)]*\)|\S+)', line)
        out.append(f'{k}={m.group(1) if m else "-"}')
    print(f'{idx:4d} ' + ' '.join(out))
