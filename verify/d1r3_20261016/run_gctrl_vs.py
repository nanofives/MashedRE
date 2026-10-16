# D1-residue R3: sim-level comparison of ONE exe against the baseline hashes already recorded
# by run_gctrl_all.py (GCTRL_ALL_base_vs_csafe.md, "base" columns), on every track.
#
# Usage: py -3.12 run_gctrl_vs.py <exe> <label> <baseline.md> <out.md> [frames] [track,track,...]
import re, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
TRACKS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
          "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]


def baseline(md):
    out = {}
    for ln in Path(md).read_text(encoding="utf-8").splitlines():
        m = re.match(r"\|\s*(\w+)\s*\|\s*(\w+)\s*\|\s*(\w+)\s*\|", ln)
        if m and m.group(1) in TRACKS:
            out[m.group(1)] = (m.group(2), m.group(3))
    return out


def one(exe, name, track, frames):
    r = subprocess.run([sys.executable, str(HERE / "run_gctrl.py"), exe, name, "0", track,
                        str(frames)], capture_output=True, text=True)
    s = re.search(r"\.step\.csv\s+lines=\d+\s+sha8=(\w+)", r.stdout)
    g = re.search(r"\.gates\.csv\s+lines=\d+\s+sha8=(\w+)", r.stdout)
    return (s.group(1) if s else "MISSING", g.group(1) if g else "MISSING")


def main():
    exe, label, bmd, out = sys.argv[1:5]
    frames = int(sys.argv[5]) if len(sys.argv) > 5 else 4000
    tracks = sys.argv[6].split(",") if len(sys.argv) > 6 else TRACKS
    base = baseline(bmd)
    rows = []
    for t in tracks:
        h = one(exe, f"vs_{label}_{t}", t, frames)
        same = h == base.get(t)
        rows.append((t, h, base.get(t), same))
        print(f"{t:<9} {label}: step={h[0]} gates={h[1]} | base: {base.get(t)} | "
              f"{'SAME' if same else 'DIFFERENT'}", flush=True)
    n = sum(1 for r in rows if r[3])
    lines = [f"# {label} vs baseline, G-CTRL recipe {frames} frames", "",
             "| track | step | gates | verdict |", "|---|---|---|---|"]
    for t, h, b, s in rows:
        lines.append(f"| {t} | {h[0]} | {h[1]} | {'SAME' if s else '**DIFFERENT**'} |")
    lines += ["", f"{n}/{len(rows)} tracks identical to the baseline."]
    Path(out).write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"{label}: {n}/{len(rows)} tracks identical")


if __name__ == "__main__":
    main()
