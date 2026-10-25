"""re/frida/run_diff.py pointed at the private root (WORKER_RULES): same harness, same registry and negctl
fixtures, only MASHED_EXE / GAMESAVE_REF moved to D:\\MashedWT\\D4-DC-82a0_root\\original. The .asi under test is
run_diff's own ASI_PATH (this worktree's mashedmod/build/mashed_re_dev.asi). Usage: same argv as run_diff.py."""
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(WT / 're' / 'frida'))
import run_diff  # noqa: E402

run_diff.MASHED_EXE = Path(r"D:\MashedWT\D4-DC-82a0_root\original\MASHED.exe")
run_diff.GAMESAVE_REF = run_diff.MASHED_EXE.parent / 'gamesave.bin.refbak_20260830'

if __name__ == '__main__':
    run_diff.main()
