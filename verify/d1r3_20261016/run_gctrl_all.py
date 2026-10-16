# D1-residue R3: sim-level equality check on every track.
# For each of the 13 tracks, run the G-CTRL recipe (run_gctrl.py, 4000 deterministic frames)
# with BOTH exes and compare the step.csv / gates.csv hashes. These two files are written to
# private paths, so they are immune to the shared main-root log collisions that affect
# player_trace.log.
#
# Usage: py -3.12 run_gctrl_all.py <exeA> <labelA> <exeB> <labelB> <out.md> [frames]
import subprocess, sys, re
from pathlib import Path

HERE = Path(__file__).resolve().parent
TRACKS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
          "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]


def one(exe, name, track, frames):
    r = subprocess.run([sys.executable, str(HERE / "run_gctrl.py"), exe, name, "0", track,
                        str(frames)], capture_output=True, text=True)
    step = re.search(r"\.step\.csv\s+lines=(\d+)\s+sha8=(\w+)", r.stdout)
    gates = re.search(r"\.gates\.csv\s+lines=(\d+)\s+sha8=(\w+)", r.stdout)
    ex = re.search(r"exit=(\S+)", r.stdout)
    return (step.group(2) if step else "MISSING", gates.group(2) if gates else "MISSING",
            ex.group(1) if ex else "?")


def main():
    exe_a, la, exe_b, lb, out = sys.argv[1:6]
    frames = int(sys.argv[6]) if len(sys.argv) > 6 else 4000
    rows = []
    for t in TRACKS:
        a = one(exe_a, f"all_{la}_{t}", t, frames)
        b = one(exe_b, f"all_{lb}_{t}", t, frames)
        same = (a[0] == b[0] and a[1] == b[1] and a[0] != "MISSING")
        rows.append((t, a, b, same))
        print(f"{t:<9} {la}: step={a[0]} gates={a[1]} exit={a[2]} | {lb}: step={b[0]} "
              f"gates={b[1]} exit={b[2]} | {'SAME' if same else 'DIFFERENT'}", flush=True)
    lines = [f"# G-CTRL recipe, {frames} frames, every track: {la} vs {lb}", "",
             f"| track | {la} step | {la} gates | {lb} step | {lb} gates | verdict |",
             "|---|---|---|---|---|---|"]
    for t, a, b, s in rows:
        lines.append(f"| {t} | {a[0]} | {a[1]} | {b[0]} | {b[1]} | {'SAME' if s else '**DIFFERENT**'} |")
    n_same = sum(1 for r in rows if r[3])
    lines += ["", f"{n_same}/{len(rows)} tracks identical."]
    Path(out).write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"{n_same}/{len(rows)} tracks identical")


if __name__ == "__main__":
    main()
