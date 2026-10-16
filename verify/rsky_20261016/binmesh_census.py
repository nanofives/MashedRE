"""rsky: how many world sectors take the BinMesh PLG mesh order, per track (the line BuildWorld logs to log/librw_scene.txt).

    py -3.12 verify\\rsky_20261016\\binmesh_census.py <exe> <private gameroot> > binmesh_census.txt

A short deterministic run of each of the 13 tracks (MASHED_DET_FRAMES=40), the same private-root rules as run13.py.
"""
import os
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from run13 import TRACKS  # noqa: E402


def main():
    exe, root = Path(sys.argv[1]).resolve(), Path(sys.argv[2]).resolve()
    if not (root / "original" / "TOASTART" / "Common" / "Frontend.piz").exists():
        print("REFUSED: the private root needs original\\ inside it"); return 2
    for t, (d, view, _, _, _) in TRACKS.items():
        if t >= 20:
            continue
        log = root / "log" / "librw_scene.txt"
        if log.exists():
            log.unlink()
        env = dict(os.environ)
        env.update({"MASHED_ROOT": str(root), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": view, "MASHED_CAR": "1",
                    "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4", "MASHED_WIN_POS": "primary-bl",
                    "MASHED_DETERMINISTIC": "1", "MASHED_DET_FRAMES": "40"})
        p = subprocess.Popen([str(exe)], cwd=str(root), env=env)
        try:
            p.wait(300)
        except subprocess.TimeoutExpired:
            p.kill(); p.wait(15)
        txt = log.read_text(errors="replace") if log.exists() else ""
        lines = re.findall(r"world: sectors with geometry=\d+, BinMesh mesh order applied=\d+ \(list order changed in \d+\)", txt)
        print(f"{d:<16} " + " | ".join(lines) if lines else f"{d:<16} (no line)", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
