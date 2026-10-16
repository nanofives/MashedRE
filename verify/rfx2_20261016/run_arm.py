"""rfx2 (copy of rfx's run_arm.py, out -> log/rfx2, cites -> cites_rfx2.json): one measurement arm = the d1r1 matched-pose replay on the 9 frames + the cited 3D diffs.

    py -3.12 run_arm.py <exe> <arm_name> [--tracks training,arctic,city] [--no-run] [KNOB[=V] ...]

For each track, runs verify/d1r1_20261016/run_replay3d.py (the d1r1 recipe, unchanged) against the
original trace log/d1r1/orig_<track>/trace.csv into log/rfx2/<arm>/<track>/, then
re/tools/drawlist_diff.py (3D mode, --trace, --cite cites_d1r1.json) per frame into
log/rfx2/<arm>/<track>/diff_<K>.json, and prints rows per frame with the C04/C08/C09 rows listed.
MEASUREMENT ONLY: spawns and stops only its own PIDs (run_replay3d.py).
"""
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
HERE = Path(__file__).resolve().parent
WT = HERE.parents[1]
TRACKS = {
    "training": ("Training", [884, 924, 959]),
    "arctic": ("Arctic", [1051, 1091, 1126]),
    "city": ("City", [761, 801, 836]),
}
GROUP = ("C04", "C08", "C09")


def main() -> int:
    args = sys.argv[1:]
    exe, arm = args[0], args[1]
    tracks = list(TRACKS)
    norun = False
    brake = False      # [rfx2 amendment 1] carlod's MASHED_CAR_BRAKE_ROWS (verify/carlod_20261016/brake_rows.json)
    knobs = []
    i = 2
    while i < len(args):
        a = args[i]
        if a == "--tracks":
            tracks = args[i + 1].split(","); i += 2; continue
        if a == "--rfx-cites":     # adjudicate with verify/rfx_20261016/cites_rfx.json
            i += 1; continue
        if a == "--brake":
            brake = True; i += 1; continue
        if a == "--no-run":
            norun = True; i += 1; continue
        knobs.append(a); i += 1
    out = ROOT / "log" / "rfx2" / arm
    summary = {"exe": exe, "knobs": knobs, "frames": {}}
    brake_rows = json.loads((WT / "verify" / "carlod_20261016" / "brake_rows.json").read_text())
    for tr in tracks:
        view, shots = TRACKS[tr]
        tk = list(knobs)
        br = {k: v for k, v in brake_rows.get(tr, {}).items() if v}
        if brake and br:
            tk.append("MASHED_CAR_BRAKE_ROWS=" + ",".join(f"{k}:{v}" for k, v in br.items()))
        od = out / tr
        trace = ROOT / "log" / "d1r1" / f"orig_{tr}" / "trace.csv"
        if not norun:
            cmd = [sys.executable, str(WT / "verify" / "d1r1_20261016" / "run_replay3d.py"), exe, str(od),
                   "--trace", str(trace), "--shots", ",".join(map(str, shots)), "--track", view,
                   "--frames", "1450", *tk]
            subprocess.run(cmd, check=False)
        for k in shots:
            a = ROOT / "log" / "d1r1" / f"orig_{tr}" / f"orig_row_{k:05d}.draws.json"
            b = od / "d1r2" / f"stand_row_{k:05d}.draws.json"
            j = od / f"diff_{k:05d}.json"
            if not b.exists():
                print(f"{tr} {k}: MISSING standalone record {b}")
                continue
            subprocess.run([sys.executable, str(WT / "re" / "tools" / "drawlist_diff.py"), str(a), str(b),
                            "--trace", str(trace), "--cite",
                            str(WT / "verify" / "rfx2_20261016" / "cites_rfx2.json"), "--json", str(j)],
                           check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if not j.exists():
                print(f"{tr} {k}: diff failed"); continue
            d = json.loads(j.read_text())
            rows = d["rows"]
            red = [r for r in rows if not r.get("cite")]
            grp = [r for r in rows if (r.get("cite") or "").startswith(GROUP)]
            summary["frames"][f"{tr}:{k}"] = {"rows": len(rows), "red": len(red), "group": len(grp),
                                              "matched": d.get("matched")}
            print(f"{tr:<9} {k}  rows {len(rows):>3}  red {len(red)}  C04/08/09 {len(grp):>2}  "
                  f"matched {d.get('matched')}")
            for r in red:
                print(f"     RED {r['kind']} {r['tex']}: {r.get('detail', '')[:160]}")
            for r in grp:
                print(f"     {r['cite']:<14} {r['kind']:<7} {r['tex']}")
    (out / "summary.json").write_text(json.dumps(summary, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
