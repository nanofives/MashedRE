r"""D4-FLIP-L29-L30: the default gate arms one at a time with ROOT/log present, so RaceSession's and the videocfg
writer's lines (log/mashed_re.log) are kept per arm. Before the first arm ROOT/videocfg.bin is removed, after each arm
its sha is recorded. Arms go through run_gates.py (A4_OUT / A4_ROOT from the environment).
"""
import hashlib, os, shutil, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(os.environ["A4_ROOT"])
OUT = Path(os.environ["A4_OUT"])
LOG = ROOT / "log" / "mashed_re.log"
CFG = ROOT / "videocfg.bin"


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else "absent"


(ROOT / "log").mkdir(exist_ok=True)
OUT.mkdir(parents=True, exist_ok=True)
if CFG.exists():
    CFG.unlink()
for arm in sys.argv[1:]:
    LOG.write_text("", encoding="utf-8")
    rc = subprocess.run([sys.executable, str(HERE / "run_gates.py"), arm]).returncode
    shutil.copy2(LOG, OUT / f"{arm}.racelog")
    txt = LOG.read_text(encoding="utf-8", errors="replace")
    print(f"LOGGED {arm} rc={rc} aiveh_applied={txt.count('[D3-AI-VEHICLES] applied')} "
          f"begin={txt.count('RaceSession::Begin')} videocfg_lines={[l for l in txt.splitlines() if '[videocfg]' in l]} "
          f"videocfg.bin={sha(CFG)[:16]}", flush=True)
