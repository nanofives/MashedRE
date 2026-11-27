"""Resolve exe offsets (from an [RWC_CORE] AV stack line) to the nearest preceding symbol of a link map.

    py -3.12 verify/ddkobj_addrreg_20261127/mapaddr.py <map> 0x66EC 0x1E0449 ...
"""
import bisect
import re
import sys

mp = sys.argv[1]
pref = 0
syms = []
for line in open(mp, errors="replace"):
    m = re.match(r"\s*Preferred load address is ([0-9a-fA-F]+)", line)
    if m:
        pref = int(m.group(1), 16)
    m = re.match(r"\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+\S*\s*(\S+\.obj)?", line)
    if m:
        syms.append((int(m.group(2), 16), m.group(1), m.group(3) or ""))
syms.sort()
keys = [s[0] for s in syms]
for a in sys.argv[2:]:
    va = pref + int(a, 16)
    i = bisect.bisect_right(keys, va) - 1
    s = syms[i] if i >= 0 else (0, "?", "")
    print(f"exe+{a} va 0x{va:08x} = {s[1]}+0x{va - s[0]:x} ({s[2]})")
