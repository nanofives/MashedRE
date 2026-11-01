"""re/frida/run_verify_hook.py (path2) pointed at the private root: only MASHED_EXE moves to
D:\\MashedWT\\D4-DC-RFX-CORE_root\\original. With auto-hook enabled the dinput8 shim loads that root's own
mashed_re_dev.asi, so the root holds a copy of this worktree's build (SHA-256 in the RESULT) and MASHED_VERIFY_ASI
points the agent at that same file (run_verify_hook.py's note at its ASI_PATH). Usage: same argv as run_verify_hook.py."""
import os
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
ROOT_ORIG = Path(r"D:\MashedWT\D4-DC-RFX-CORE_root\original")
os.environ['MASHED_VERIFY_ASI'] = str(ROOT_ORIG / 'mashed_re_dev.asi')
sys.path.insert(0, str(WT / 're' / 'frida'))
import run_verify_hook  # noqa: E402

run_verify_hook.MASHED_EXE = ROOT_ORIG / 'MASHED.exe'

if __name__ == '__main__':
    sys.exit(run_verify_hook.main())
