"""RO-BOOT-CRASH-3: score every body run of this root against the no-body references of the same root and the same boot
class (RO-MENU-REPLAY rule: a class-mismatched run is not body evidence; references of another root differ in the
module-path string at 0x00773c4c..0x00773c70, so only this root's references are used).

    py -3.12 verify/robootcrash3_20261112/score_by_class.py <data dir> [--out verdicts.json]

Class = (0x007f1030, 0x006147d8) at tick 0, read through prov_compare's boot_class. Every body run is compared with
each same-class reference pair (or one reference given twice when the class has one). Runs with no same-class
reference are listed as UNSCORED.
"""
import itertools
import json
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1] / "re" / "frida"))
from prov_compare import verdict  # noqa: E402

D = pathlib.Path(sys.argv[1])
out = sys.argv[sys.argv.index("--out") + 1] if "--out" in sys.argv else ""
runs = sorted(p for p in D.iterdir() if p.is_dir() and (p / "meta.json").exists() and p.name[0] in "qf")
refs = [p for p in runs if p.name.startswith("q")]
bodies = [p for p in runs if p.name.startswith("f")]


def cls(p):
    v = verdict(str(p), str(refs[0]), str(refs[0]))
    c = v["boot_class"]["run"]
    return (c.get("0x7f1030"), c.get("0x6147d8"))


rc = {p.name: cls(p) for p in runs}
for n, c in rc.items():
    print(f"class {n:18s} {c}")
res = []
for b in bodies:
    same = [r for r in refs if rc[r.name] == rc[b.name]]
    pairs = list(itertools.combinations(same, 2)) or [(r, r) for r in same]
    if not pairs:
        res.append({"run": b.name, "class": rc[b.name], "verdict": "UNSCORED (no same-class reference)"})
        print(f"{b.name:18s} UNSCORED class {rc[b.name]}")
        continue
    for a, c in pairs:
        v = verdict(str(b), str(a), str(c))
        row = {"run": b.name, "class": rc[b.name], "ref": [a.name, c.name], "verdict": v["verdict"],
               "identical_for": v["identical_for"], "ticks": v.get("ticks_by_segment"), "first": v.get("first"),
               "boot_class_match": v.get("boot_class_match"), "allowed_blocks": v.get("allowed_blocks"), "notes": v.get("notes")}
        res.append(row)
        print(f"{b.name:18s} vs {a.name}/{c.name}: {v['verdict']} {v['identical_for']} first={v.get('first')}")
# reference self-consistency inside each class
for a, c in itertools.combinations(refs, 2):
    if rc[a.name] == rc[c.name]:
        v = verdict(str(a), str(c), str(c))
        res.append({"run": a.name, "class": rc[a.name], "ref": [c.name, c.name], "verdict": v["verdict"],
                    "identical_for": v["identical_for"], "kind": "reference pair"})
        print(f"REFPAIR {a.name} vs {c.name}: {v['verdict']} {v['identical_for']}")
if out:
    pathlib.Path(out).write_text(json.dumps({"classes": {k: list(v) for k, v in rc.items()}, "verdicts": res}, indent=1))
