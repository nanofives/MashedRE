# run_with_unlocked_save.py — run ONE command against an edited original save,
# then restore the reference, sha-verified, no matter what.
#
# T-ARCTIC needs the ORIGINAL to load an unlocked gamesave.bin, but
# original/gamesave.bin is the diffing reference and is shared across concurrent
# sessions. This wrapper keeps the swap window to a SINGLE command:
#   1. assert original/gamesave.bin currently == the durable backup (pristine)
#   2. copy <edited> over original/gamesave.bin
#   3. run <command...> (which launches MASHED + Frida and captures)
#   4. finally: restore the backup and verify sha; refuse to exit dirty
#
# The durable backup original/gamesave.bin.refbak_20260830 must already exist
# (made by the parent). This script never deletes it. If restore ever fails the
# sha check it prints a loud RECOVERY line and exits non-zero.
import hashlib, shutil, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
REF  = ROOT / "original" / "gamesave.bin"
BAK  = ROOT / "original" / "gamesave.bin.refbak_20260830"


def sha(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


def main():
    if len(sys.argv) < 3 or sys.argv[2] != "--":
        sys.exit("usage: run_with_unlocked_save.py <edited.bin> -- <command...>")
    edited = Path(sys.argv[1])
    cmd = sys.argv[3:]
    if not cmd:
        sys.exit("no command given after --")
    if not BAK.exists():
        sys.exit(f"durable backup missing: {BAK} (parent must create it first)")
    if not edited.exists():
        sys.exit(f"edited save missing: {edited}")

    bak_sha = sha(BAK)
    if sha(REF) != bak_sha:
        sys.exit("REF is not pristine (differs from backup) — refusing to start; "
                 "restore original/gamesave.bin from the backup and re-run")

    print(f"[swap] {edited.name} -> original/gamesave.bin (ref backed up, sha {bak_sha[:16]})")
    shutil.copyfile(edited, REF)
    rc = 1
    try:
        rc = subprocess.run(cmd).returncode
    finally:
        shutil.copyfile(BAK, REF)
        if sha(REF) == bak_sha:
            print(f"[restore] original/gamesave.bin restored, sha verified {bak_sha[:16]}")
        else:
            print("!!! RECOVERY NEEDED: restore sha MISMATCH. "
                  f"copy {BAK} over {REF} by hand before any capture.")
            sys.exit(3)
    sys.exit(rc)


if __name__ == "__main__":
    main()
