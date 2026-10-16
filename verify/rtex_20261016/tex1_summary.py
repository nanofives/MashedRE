"""Summarise the original's stage-1 (tex1) draws in d1r1's draw records.

Usage: py -3.12 tex1_summary.py <draws.json> [...]
Groups draws with a stage-1 texture by (tex, tex1, fvf, stride, rs, tss) and prints counts.
RS order (camera_probe.py RS_LIST): ZENABLE ZWRITE ZFUNC ATEST AREF AFUNC ABLEND SRC DST CULL FOG
LIGHTING. TSS order: stage0 COLOROP, stage0 ALPHAOP, stage1 COLOROP.
"""
import collections
import json
import sys

for path in sys.argv[1:]:
    d = json.load(open(path))
    ev = [e for e in d["events"] if e.get("e") == "d"]
    groups = collections.Counter()
    tris = collections.Counter()
    for e in ev:
        t1 = e.get("tex1", "-")
        if t1 == "-" and e.get("tss", [0, 0, 0])[2] in (1, -1):
            continue
        key = (e.get("tex"), t1, e.get("fvf"), e.get("st"), tuple(e.get("rs", [])), tuple(e.get("tss", [])))
        groups[key] += 1
        tris[key] += e.get("tri", 0)
    print(f"== {path}: {len(ev)} draws, {sum(groups.values())} with stage-1 active")
    for k, n in sorted(groups.items(), key=lambda kv: (-kv[1], str(kv[0]))):
        print(f"  n={n:3d} tri={tris[k]:5d} tex={k[0]!s:24} tex1={k[1]!s:18} fvf=0x{(k[2] or 0):x} st={k[3]} rs={list(k[4])} tss={list(k[5])}")
