"""PROV-DIVERGED-18 (2026-11-11): every diverging tick of a PROV replay, not only the first.

prov_compare.verdict stops at the first bad tick. This runs the same compare (its own source, with the stop replaced by
a collect), so the allowances (allowed blocks, MASK rows, the tick-0 load-length rule) are exactly prov_compare's.

    py -3.12 re/tools/prov/prov_ticks_all.py <run> <ref_a> <ref_b> [--pair A,B]
"""
import argparse, inspect, pathlib, sys, textwrap

ROOT = pathlib.Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "re" / "frida"))
import prov_compare  # noqa: E402

src = textwrap.dedent(inspect.getsource(prov_compare.verdict))
OLD = "            first = {\"tick\": i,"
assert OLD in src and "            break\n" in src, "prov_compare.verdict changed shape"
src = src.replace("def verdict(", "def verdict_all(", 1)
src = src.replace(OLD, "            ALL.append({\"tick\": i, \"blocks\": [hex(va(j)) for j in bad[:8]], \"nblocks\": len(bad), "
                       "\"raw\": raw[:6]})\n            first = first or {\"tick\": i,", 1)
src = src.replace("            break\n", "            continue\n", 1)
ns = dict(vars(prov_compare))
ALL = []
ns["ALL"] = ALL
exec(compile(src, "verdict_all", "exec"), ns)

ap = argparse.ArgumentParser()
ap.add_argument("run")
ap.add_argument("ref_a")
ap.add_argument("ref_b")
ap.add_argument("--pair", action="append", default=[])
a = ap.parse_args()
v = ns["verdict_all"](pathlib.Path(a.run), pathlib.Path(a.ref_a), pathlib.Path(a.ref_b), [p.split(",") for p in a.pair])
print(f"{a.run}: ticks {v['ticks']}, diverging ticks {len(ALL)}")
for r in ALL[:40]:
    print(f"  tick {r['tick']}: {r['nblocks']} blocks {r['blocks']} raw {r['raw']}")
if len(ALL) > 40:
    print(f"  ... {len(ALL) - 40} more, last tick {ALL[-1]['tick']}")
