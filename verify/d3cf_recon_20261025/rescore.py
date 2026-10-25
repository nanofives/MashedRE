"""Copy a run13 out dir (no BMPs) and re-score it with a given cites file (offline, no game run)."""
import shutil, subprocess, sys
from pathlib import Path

src, dst, cites, wt = Path(sys.argv[1]), Path(sys.argv[2]), sys.argv[3], Path(sys.argv[4])
if dst.exists():
    shutil.rmtree(dst)
shutil.copytree(src, dst, ignore=shutil.ignore_patterns("*.bmp", "*.png"))
r = subprocess.run([sys.executable, str(wt / "verify/rsky_20261016/run13.py"), "score", "--out", str(dst),
                    "--cite", str(wt / cites)], capture_output=True, text=True, cwd=wt)
sys.stdout.write(r.stdout + r.stderr)
