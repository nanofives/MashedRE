"""r10: instrument gates + drawlist_diff 3D scoring for the 10 capfix tracks.

    py -3.12 verify\\r10_20261016\\score_tracks.py [--cite cites_r10.json] [--prefix diff_] [--tracks 1,2,...]

Per track and shot row K: reads the capfix original record (verify/capfix_20261016/<NN_Track>/orig_row_<K>.draws.json) and
the replay's stand/d1r2/stand_row_<K>.draws.json, runs the d1r1 gate functions (check_gates.orig_gates / stand_gates), then
re/tools/drawlist_diff.py with --trace and --cite. Writes <NN_Track>/<prefix><K>.json / .txt and scores_<prefix>.txt.
"""
import argparse
import csv
import importlib.util
import json
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
CAPFIX = ROOT / "verify" / "capfix_20261016"
DIFF = ROOT / "re" / "tools" / "drawlist_diff.py"

spec = importlib.util.spec_from_file_location("cg", ROOT / "verify" / "d1r1_20261016" / "check_gates.py")
cg = importlib.util.module_from_spec(spec)
spec.loader.exec_module(cg)

DIRS = {1: "01_Egypt", 2: "02_Neustein", 4: "04_Highway", 5: "05_Sands", 6: "06_SuperG", 7: "07_Roundabout",
        8: "08_Storm", 9: "09_Forest", 10: "10_Dump", 11: "11_Warzone"}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--cite", default=str(HERE / "cites_r10.json"))
    ap.add_argument("--prefix", default="diff_")
    ap.add_argument("--stand", default="stand", help="stand subdir name under each track dir")
    ap.add_argument("--tracks", default=",".join(str(t) for t in DIRS))
    ap.add_argument("--cap", default=str(CAPFIX), help="root holding <NN_Track>/orig_row_*.draws.json + trace.csv + summary.json")
    ap.add_argument("--work", default=str(HERE), help="root holding <NN_Track>/stand; diff outputs are written here too")
    a = ap.parse_args()
    CAP, WORK = Path(a.cap).resolve(), Path(a.work).resolve()
    all_cites = json.loads(Path(a.cite).read_text())
    lines, gate_lines = [], []
    hdr = (f"{'track':<14}{'row':>6} {'orig':>5} {'matched':>8} {'orig tri':>9} {'m.tri':>7} {'m.tri%':>7} | "
           f"{'rows':>4} {'cited':>5} {'red':>4} verdict")
    lines.append(hdr)
    ok_all = True
    for t in [int(x) for x in a.tracks.split(",")]:
        d = DIRS[t]
        cap, sd = CAP / d, WORK / d / a.stand
        # per-track cites: an entry with "tracks": [...] applies to those track names only (drawlist3d has no such key)
        tname = d.split("_", 1)[1]
        mine = [c for c in all_cites if "tracks" not in c or tname in c["tracks"]]
        cf = WORK / d / f"cites_used_{a.prefix.strip('_')}.json"
        cf.write_text(json.dumps(mine, indent=1))
        trace = cap / "trace.csv"
        og = cg.orig_gates(cap)
        sg, _ = cg.stand_gates(sd, trace) if (sd / "d1r2").exists() else ([], 0)
        for g in og:
            gate_lines.append(f"orig  {d:<14} {json.dumps(g)}")
            ok_all &= g["I-COVER"] and g["I-FRAME"] and g["I-NAME"]
        for g in sg:
            gate_lines.append(f"stand {d:<14} {json.dumps(g)}")
            ok_all &= g["I-SYNC"] and g["I-NAME"]
        shots = json.loads((cap / "summary.json").read_text())["shot_rows"]
        if len(sg) != len(shots):
            ok_all = False
            gate_lines.append(f"stand {d:<14} MISSING {len(shots) - len(sg)} of {len(shots)} replay records")
        for k in shots:
            fa = cap / f"orig_row_{k:05d}.draws.json"
            fb = sd / "d1r2" / f"stand_row_{k:05d}.draws.json"
            if not fb.exists():
                lines.append(f"{d:<14}{k:>6} UNSCORED (no replay record)")
                continue
            jo = WORK / d / f"{a.prefix}{k:05d}.json"
            r = subprocess.run([sys.executable, str(DIFF), str(fa), str(fb), "--trace", str(trace),
                                "--cite", str(cf), "--json", str(jo), "--max-rows", "500"],
                               capture_output=True, text=True)
            (WORK / d / f"{a.prefix}{k:05d}.txt").write_text(r.stdout + r.stderr)
            j = json.loads(jo.read_text())
            rows = j["rows"]
            red = [x for x in rows if "cite" not in x]
            ver = "GREEN" if not rows else ("CITED" if not red else "RED")
            tri_a = j["tri_a"]
            # matched triangles are printed by the tool; recompute from the json info if present
            m = re.search(r"matched (\d+) draws \((\d+) tri\)", r.stdout)
            mtri = int(m.group(2)) if m else 0
            lines.append(f"{d:<14}{k:>6} {j['na']:>5} {j['matched']:>8} {tri_a:>9} {mtri:>7} "
                         f"{100 * mtri / max(1, tri_a):>6.1f}% | {len(rows):>4} {len(rows) - len(red):>5} "
                         f"{len(red):>4} {ver}")
    out = "\n".join(lines) + "\n\nINSTRUMENT GATES: " + ("PASS" if ok_all else "FAIL") + "\n"
    (WORK / f"scores_{a.prefix.strip('_')}.txt").write_text(out)
    (WORK / f"gates_{a.prefix.strip('_')}.txt").write_text("\n".join(gate_lines) + "\n")
    print(out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
