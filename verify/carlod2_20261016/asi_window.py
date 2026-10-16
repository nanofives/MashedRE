"""carlod2: run one command inside the shared-.asi deploy window (orchestrator rule, 2026-10-16).

    py -3.12 asi_window.py <my.asi> <log.txt> -- <command ...>

1. refuse if any MASHED.exe process is running (a foreign session could be using the shared .asi);
2. back up original\\mashed_re_dev.asi and log its SHA-256;
3. copy <my.asi> over it, log the deployed SHA-256 (must equal <my.asi>'s);
4. run <command> with MASHED_VERIFY_ASI=<absolute path of the shared .asi>;
5. in `finally`: log the SHA-256 found at the end (a mismatch means someone overwrote it mid-run), restore the backup,
   log the restored SHA-256 (must equal the backup's).
"""
import hashlib
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
SHARED = MAIN / "original" / "mashed_re_dev.asi"


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest() if Path(p).exists() else "ABSENT"


def mashed_pids():
    out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq MASHED.exe", "/FO", "CSV", "/NH"],
                         capture_output=True, text=True).stdout
    return [ln.split(",")[1].strip('"') for ln in out.splitlines() if ln.startswith('"MASHED')]


def main():
    i = sys.argv.index("--")
    mine, logp, cmd = Path(sys.argv[1]), Path(sys.argv[2]), sys.argv[i + 1:]
    log = open(logp, "a")

    def L(s):
        line = f"{time.strftime('%H:%M:%S')} {s}"
        print(line, flush=True); log.write(line + "\n"); log.flush()

    pids = mashed_pids()
    if pids:
        L(f"REFUSED: foreign MASHED.exe running {pids}")
        return 3
    bak = SHARED.with_suffix(".asi.carlod2bak")
    shutil.copy2(SHARED, bak)
    L(f"backup   sha={sha(bak)}  ({SHARED})")
    rc = 1
    try:
        shutil.copy2(mine, SHARED)
        L(f"mine     sha={sha(mine)}  ({mine})")
        L(f"deployed sha={sha(SHARED)}  equal_to_mine={sha(SHARED) == sha(mine)}")
        env = dict(os.environ, MASHED_VERIFY_ASI=str(SHARED))
        L("run: " + " ".join(cmd))
        rc = subprocess.run(cmd, env=env).returncode
        L(f"command exit={rc}")
    finally:
        end = sha(SHARED)
        L(f"at end   sha={end}  still_mine={end == sha(mine)}")
        shutil.copy2(bak, SHARED)
        L(f"restored sha={sha(SHARED)}  equal_to_backup={sha(SHARED) == sha(bak)}")
        bak.unlink()
    log.close()
    return rc


if __name__ == "__main__":
    sys.exit(main())
