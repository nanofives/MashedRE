"""RO-BOOT-CRASH-3: strict form of score_by_class.py. Each body run against each single reference of the same root
and the same boot class, given as both references (prov_compare verdict(run, q, q): 0 allowed blocks; MASK / RULE
dwords of re/frida/warp_normalize.tsv still apply). Class = 0x007f1030 and 0x006147d8 at tick 0 (prov_compare) plus
the tick-0 hash of block 0x0067d800 (second boot marker found here, see the RESULT).

    py -3.12 verify/robootcrash3_20261112/score_strict.py <data dir> [--out strict.json]
"""
import json
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1] / "re" / "frida"))
from detorig import DATA_LO  # noqa: E402
from detorig_ticks import load  # noqa: E402
from prov_compare import verdict  # noqa: E402

D = pathlib.Path(sys.argv[1])
out = sys.argv[sys.argv.index("--out") + 1] if "--out" in sys.argv else ""
runs = sorted(p for p in D.iterdir() if p.is_dir() and (p / "ticks.bin").exists() and p.name[0] in "qf")


def cls(p):
    _, t, r = load(str(p))
    raw = [v for lo, hi in t["raw"] for v in range(lo, hi, 4)]
    w = lambda va: f"{r[0][2 + t['nblk'] + raw.index(va)]:08x}"
    return (w(0x7f1030), w(0x6147d8), r[0][2 + (0x67d800 - DATA_LO) // t["block"]])


c = {p.name: cls(p) for p in runs}
res = []
for b in (p for p in runs if p.name.startswith("f")):
    same = [q for q in runs if q.name.startswith("q") and c[q.name] == c[b.name]]
    if not same:
        print(f"{b.name:16s} UNSCORED class {c[b.name]}")
        res.append({"run": b.name, "class": list(c[b.name]), "verdict": "UNSCORED"})
    for q in same:
        v = verdict(str(b), str(q), str(q))
        print(f"{b.name:16s} vs {q.name}: {v['verdict']} {v['identical_for']} allowed={len(v['allowed_blocks'])} "
              f"class_match={v.get('boot_class_match')} first={v.get('first')}")
        res.append({"run": b.name, "ref": q.name, "class": list(c[b.name]), "verdict": v["verdict"],
                    "identical_for": v["identical_for"], "allowed_blocks": v["allowed_blocks"],
                    "ticks_by_segment": v.get("ticks_by_segment"), "first": v.get("first"), "rc": v.get("rc")})
if out:
    pathlib.Path(out).write_text(json.dumps({"classes": {k: list(v) for k, v in c.items()}, "verdicts": res}, indent=1))
