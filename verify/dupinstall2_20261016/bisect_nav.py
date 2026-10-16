"""Binary-search the hook registry index range [lo, hi) for the hook(s) whose install makes MASHED die at
`[nav] booting to menu` (exit 0xC0000005, EIP 0x1). Uses MASHED_HOOK_LO / MASHED_HOOK_HI (Core/HookSystem.cpp) with
whatever .asi is deployed. A probe is "BAD" when nav_probe reports proc.poll()=0xc0000005.

    py -3.12 bisect_nav.py <n_hooks> [<lo> <hi>]

Assumes one culprit in [lo, hi); verifies the full range first and the empty range, then halves. Prints one line
per probe and the final single index. Only pids spawned by nav_probe.py are touched.
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
PROBE = HERE / "nav_probe.py"


def bad(lo: int, hi: int, tag: str) -> bool:
    p = subprocess.run([sys.executable, str(PROBE), "--env", f"MASHED_HOOK_LO={lo}", "--env", f"MASHED_HOOK_HI={hi}", tag],
                       capture_output=True, text=True, timeout=240)
    txt = p.stdout + p.stderr
    m = re.search(r"proc\.poll\(\)=(\S+)", txt)
    crashed = bool(m and m.group(1).lower() == "0xc0000005")
    ok = "drive_to_race=True" in txt
    print(f"  [{lo:5d},{hi:5d}) -> {'BAD(AV)' if crashed else ('ok' if ok else 'INCONCLUSIVE: ' + (m.group(1) if m else '?'))}", flush=True)
    (HERE / "bisect_logs").mkdir(exist_ok=True)
    (HERE / "bisect_logs" / f"{tag}.txt").write_text(txt, encoding="utf-8")
    if not crashed and not ok:
        raise SystemExit("inconclusive probe, stop and look at bisect_logs")
    return crashed


def main() -> int:
    n = int(sys.argv[1])
    lo = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    hi = int(sys.argv[3]) if len(sys.argv) > 3 else n
    if not bad(lo, hi, f"full_{lo}_{hi}"):
        print("full range does not reproduce")
        return 1
    while hi - lo > 1:
        mid = (lo + hi) // 2
        if bad(lo, mid, f"b_{lo}_{mid}"):
            hi = mid
        elif bad(mid, hi, f"b_{mid}_{hi}"):
            lo = mid
        else:
            print(f"neither half reproduces alone: interaction between [{lo},{mid}) and [{mid},{hi})")
            return 2
    print(f"CULPRIT index {lo}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
