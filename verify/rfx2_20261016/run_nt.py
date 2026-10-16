"""rfx2: the non-team HUD reference (Team Play off, PREREG_rfx2.md amendment 2) — one arm = the d1r1
matched-pose replay of log/rfx2/nt_training/trace.csv on its three shot rows + the cited 3D diffs.

    py -3.12 run_nt.py <exe> <arm> [--no-run] [KNOB[=V] ...]

The HUD inputs come from log/rfx2/nt_training/hud_inputs_nt.txt (camera_probe.py --hud-out on the same
run, make_hud_inputs.py). MEASUREMENT ONLY: spawns and stops only its own PIDs (run_replay3d.py).
"""
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
HERE = Path(__file__).resolve().parent
WT = HERE.parents[1]
REF = ROOT / "log" / "rfx2" / "nt_training"
SHOTS = [840, 880, 915]


def main() -> int:
    args = sys.argv[1:]
    exe, arm = args[0], args[1]
    norun = "--no-run" in args
    knobs = [a for a in args[2:] if a != "--no-run"]
    knobs.append("MASHED_HUD_REPLAY=" + str(REF / "hud_inputs_nt.txt"))
    out = ROOT / "log" / "rfx2" / arm / "training"
    trace = REF / "trace.csv"
    if not norun:
        subprocess.run([sys.executable, str(WT / "verify" / "d1r1_20261016" / "run_replay3d.py"), exe, str(out),
                        "--trace", str(trace), "--shots", ",".join(map(str, SHOTS)), "--track", "Training",
                        "--frames", "1300", *knobs], check=False)
    for k in SHOTS:
        a = REF / f"orig_row_{k:05d}.draws.json"
        b = out / "d1r2" / f"stand_row_{k:05d}.draws.json"
        j = out / f"diff_{k:05d}.json"
        if not b.exists():
            print(f"{k}: MISSING standalone record {b}"); continue
        subprocess.run([sys.executable, str(WT / "re" / "tools" / "drawlist_diff.py"), str(a), str(b), "--trace",
                        str(trace), "--cite", str(HERE / "cites_rfx2.json"), "--json", str(j)],
                       check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        d = json.loads(j.read_text())
        rows = d["rows"]
        red = [r for r in rows if not r.get("cite")]
        c08 = [r for r in rows if (r.get("cite") or "").startswith("C08")]
        print(f"training  {k}  rows {len(rows):>3}  red {len(red)}  C08 {len(c08)}  matched {d.get('matched')}")
        for r in red:
            print(f"     RED {r['kind']} {r['tex']}: {r.get('detail', '')[:160]}")
        for r in c08:
            print(f"     {r['cite']:<14} {r['kind']:<7} {r['tex']}  {r.get('detail', '')[:90]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
