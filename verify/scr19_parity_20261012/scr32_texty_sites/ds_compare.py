# ds_compare.py <before.json> <after.json> [--verbose]
# Record-by-record compare of two standalone MASHED_DBG_DRAWSTREAM dumps (first frame key of each).
# Records are paired in stream order. A record "differs" when its texture/state list or its vertex
# bytes differ. Glyph records are s[0] == 9 (menu font). For each differing record prints the stream
# index, texture, and the vertex-bbox delta (after - before). Exit summary: total, differing, of which
# glyph, non-glyph, and count mismatch.
import json, struct, sys
def load(p):
    d = json.load(open(p)); k = sorted(d)[0]; return d[k]
def bbox(rec):
    b = bytes.fromhex(rec["v"]); vs = [struct.unpack_from("<4fI2f", b, j*28) for j in range(len(b)//28)]
    if not vs: return None
    return (min(v[0] for v in vs), min(v[1] for v in vs), max(v[0] for v in vs), max(v[1] for v in vs),
            tuple(sorted(set(v[4] for v in vs))))
a, b = load(sys.argv[1]), load(sys.argv[2])
verbose = "--verbose" in sys.argv
diff = glyph = other = 0
deltas = {}
for i, (ra, rb) in enumerate(zip(a, b)):
    if ra.get("s") == rb.get("s") and ra.get("v") == rb.get("v"): continue
    diff += 1
    tex = (rb.get("s") or [None])[0]
    if tex == 9: glyph += 1
    else: other += 1
    ba, bb = bbox(ra), bbox(rb)
    if ba and bb:
        dd = (round(bb[0]-ba[0], 4), round(bb[1]-ba[1], 4), round((bb[2]-bb[0])-(ba[2]-ba[0]), 4),
              round((bb[3]-bb[1])-(ba[3]-ba[1]), 4), "col" if ba[4] != bb[4] else "")
        key = (tex,) + dd
        deltas[key] = deltas.get(key, 0) + 1
        if verbose: print(f"  rec {i:4d} tex {tex} dx {dd[0]:+.4f} dy {dd[1]:+.4f} dw {dd[2]:+.4f} dh {dd[3]:+.4f} {dd[4]}")
print(f"records before {len(a)} after {len(b)} | differing {diff} (glyph {glyph}, non-glyph {other})"
      + ("" if len(a) == len(b) else f" | COUNT MISMATCH {len(b)-len(a):+d}"))
for k, n in sorted(deltas.items(), key=lambda kv: -kv[1]):
    print(f"  {n:4d} x tex {k[0]} dx {k[1]:+.4f} dy {k[2]:+.4f} dw {k[3]:+.4f} dh {k[4]:+.4f} {k[5]}")
