"""Resolve exe-relative offsets (exe+0x...) against an MSVC .map. usage: py -3.12 resolve.py <map> <off> [<off> ...]"""
import bisect, re, sys

mapf = sys.argv[1]
syms, base = [], None
for ln in open(mapf, encoding="latin-1"):
    m = re.search(r"Preferred load address is ([0-9a-fA-F]+)", ln)
    if m:
        base = int(m.group(1), 16)
    m = re.match(r"\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(\S.*)?$", ln)
    if m:
        syms.append((int(m.group(2), 16), m.group(1), (m.group(3) or "").strip()))
syms.sort()
keys = [a for a, _, _ in syms]
print("preferred base 0x%x" % base)
for a in sys.argv[2:]:
    off = int(a.lstrip("+"), 16)
    va = base + off
    i = bisect.bisect_right(keys, va) - 1
    print("+0x%x  %s+0x%x  [%s]" % (off, syms[i][1], va - syms[i][0], syms[i][2]))
