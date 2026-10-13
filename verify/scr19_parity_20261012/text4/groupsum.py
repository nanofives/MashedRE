# groupsum.py <diff.json> [<diff.json> ...] -- text4 per-group summary of drawlist_diff --json outputs.
# Original-side rows are grouped by their caller (rets[1]); standalone-only (extra) rows carry no original
# caller and are grouped by the standalone symbol of their first two returns (needs the build's map, which
# drawlist_diff already resolved into the rets strings) or "extra:other".
# Per frame pair: matched count (from the tool), then per group: mismatched kinds, missing, extra.
import json, sys
from collections import Counter

GROUPS = [
    ("checker",   {"0x42e89c"}),
    ("band",      {"0x474072", "0x4740b7", "0x473f6c", "0x473f87"}),
    ("backdrop",  {"0x42e65a", "0x42e7ba", "0x42e80c", "0x42e434", "0x42e462", "0x42e4a6", "0x42e4bc"}),
    ("preview",   {"0x42ea94", "0x42eafa", "0x42eb43", "0x42eb65", "0x42eb7f", "0x42ebb9"}),
    ("panelbox",  {"0x436996"}),
    ("divider",   {"0x4369d8"}),
    ("cars",      {"0x438149", "0x43817e", "0x4381b6", "0x4381eb"}),
    ("vs",        {"0x438247", "0x43827a", "0x4382ad"}),
    ("arrow",     {"0x43b3e3", "0x43b206", "0x43b595"}),
    ("pu_frame",  {"0x43baee"}),
    ("pu_icon",   {"0x43bbea"}),
    ("vehicle",   {"0x43bcf8", "0x43bc7c"}),
]

def group_a(rec):
    r = rec.get("rets") or []
    if len(r) < 2:
        return "other"
    for name, keys in GROUPS:
        if r[1] in keys or r[0] in keys:
            return name
    c = int(r[1], 16)
    if 0x0043c5b0 <= c < 0x0043d2a0:
        return "plate"
    return "other"

def group_b(rec):
    r = " ".join((rec.get("rets") or [])[:2])
    for tag, name in (("LogoOverlayDraw", "checker"), ("LogoGradientQuadPx", "band")):
        if tag in r:
            return name
    return "other"

for path in sys.argv[1:]:
    d = json.load(open(path))
    print(f"=== {path}")
    for pair, v in d.items():
        rows = Counter()
        for m in v["mismatched"]:
            rows[(group_a(m["a"]), "mism:" + m["kind"])] += 1
        for a in v["missing"]:
            rows[(group_a(a), "missing")] += 1
        for b in v["extra"]:
            rows[(group_b(b), "extra")] += 1
        cell = ", ".join(f"{g}/{k}={n}" for (g, k), n in sorted(rows.items()))
        print(f"  {pair}: matched={v['matched']} | {cell or 'none'}")
