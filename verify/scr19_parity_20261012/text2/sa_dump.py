# sa_dump.py <sa_drawstream.json> [--all] -- list standalone MASHED_DBG_DRAWSTREAM records (first frame key):
# index, texture/state list head, vertex bbox in the dump's own units, colour of vertex 0, u/v bbox.
# Glyph records (s[0] == 9) are skipped unless --all.
import json, struct, sys
d = json.load(open(sys.argv[1])); k = sorted(d)[0]
allr = "--all" in sys.argv
for i, rec in enumerate(d[k]):
    s = rec.get("s") or [None]
    if s[0] == 9 and not allr: continue
    b = bytes.fromhex(rec["v"]); n = len(b) // 28
    if n == 0: continue
    vs = [struct.unpack_from("<4fI2f", b, 28 * j) for j in range(n)]
    xs = [v[0] for v in vs]; ys = [v[1] for v in vs]; us = [v[5] for v in vs]; vv = [v[6] for v in vs]
    print(f"{i:4d} tex {s[0]} x {min(xs):8.3f}..{max(xs):8.3f} y {min(ys):8.3f}..{max(ys):8.3f} "
          f"col {vs[0][4]:08x} u {min(us):.4f}..{max(us):.4f} v {min(vv):.4f}..{max(vv):.4f} n{n}")
