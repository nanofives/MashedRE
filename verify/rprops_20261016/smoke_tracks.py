#!/usr/bin/env python3
"""Smoke-run the standalone on tracks with no pose-matched original capture (everything but TRAINING /
Arctic / City) to check the new placement code loads, logs its PROP-PAIR / PROP-CAMMAN lines with the
decoded counts, and the process exits cleanly. Private game root; spawns and stops only its own PID.

    py -3.12 smoke_tracks.py <exe> <out_dir> [piz basename ...]
"""
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(os.environ.get("RPROPS_ROOT", r"C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed\dfd5f16e-a355-4a6a-9772-942f8f25087d\scratchpad\gameroot"))
DEFAULT = ["rouabout", "Highway", "Warzone", "sands", "Storm", "Egypt", "dump", "Forest", "Neustein", "SuperG"]


def main():
    exe, out = sys.argv[1], Path(sys.argv[2]).resolve()
    tracks = sys.argv[3:] or DEFAULT
    out.mkdir(parents=True, exist_ok=True)
    for t in tracks:
        o = out / t
        o.mkdir(parents=True, exist_ok=True)
        env = dict(os.environ)
        env.update({
            "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": t,
            "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
            "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
            "MASHED_DET_FRAMES": "300", "MASHED_VERIFY_OUT": str(o),
            "MASHED_PLAYERTRACE_OUT": str(o / "ptrace.log"),
        })
        p = subprocess.Popen([exe], cwd=str(ROOT), env=env)
        try:
            rc = p.wait(300)
        except subprocess.TimeoutExpired:
            p.kill(); rc = p.wait(15)
        lines = []
        lg = ROOT / "mashed_re.log"
        if lg.exists():
            for ln in lg.read_text(errors="replace").splitlines():
                if "PROP-PAIR" in ln or "PROP-CAMMAN" in ln:
                    lines.append(ln.strip())
        (o / "placement_lines.txt").write_text("\n".join(lines) + "\n")
        print(f"{t}: exit={rc} placement lines={len(lines)}", flush=True)
        for ln in lines:
            print("   ", ln, flush=True)


if __name__ == "__main__":
    main()
