#!/usr/bin/env py -3.12
"""repatch_original.py — one-command recovery after restoring original/.

Run this AFTER you have copied a clean Mashed install back into `original/`
(the WORKTREE-SYMLINK-WIPE incident emptied it; see re/diag/KNOWN_ISSUES.md).
It re-applies the boot patches + canonical videocfg, then tells you the two
machine-specific steps to finish. Idempotent — every patch script self-checks.

It does NOT fetch the install (you supply the files). It refuses to run if
original/ still looks empty.

  py -3.12 scripts/repatch_original.py            # apply
  py -3.12 scripts/repatch_original.py --check     # just report state
"""
import hashlib
import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ORIG = os.path.join(ROOT, "original")
EXE = os.path.join(ORIG, "MASHED.exe")
UNPATCHED = os.path.join(ORIG, "MASHED.exe.unpatched")
ANCHOR_SHA = "BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"

# The boot-patch sequence (CLAUDE.md "Runtime state"). skip_powerups is RETIRED
# (refuses). skip_movies is OPTIONAL (skip_intro preferred) — omitted here.
PATCHES = [
    "patch_mashed_show_windowed.py",
    "patch_mashed_skip_audio_com.py",
    "patch_mashed_skip_selector.py",
    "patch_mashed_skip_controller_dialog.py",
    "patch_mashed_fix_camera_res.py",
    "patch_mashed_disable_log.py",
    "patch_mashed_fix_fopen.py",
    "patch_mashed_fix_joypad.py",
    "patch_mashed_skip_intro.py",
]


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest().upper()


def main():
    check_only = "--check" in sys.argv
    if not os.path.isdir(ORIG) or not os.listdir(ORIG):
        print("FATAL: original/ is EMPTY. Restore a clean Mashed install into "
              "original/ first (the install you keep / re-decompress), then re-run.")
        return 2
    if not os.path.exists(EXE) and not os.path.exists(UNPATCHED):
        print("FATAL: neither original/MASHED.exe nor MASHED.exe.unpatched present "
              "— the install is incomplete.")
        return 2

    # Establish the anchor backup from a clean MASHED.exe if needed.
    if not os.path.exists(UNPATCHED):
        if os.path.exists(EXE):
            sha = sha256(EXE)
            if sha == ANCHOR_SHA:
                if not check_only:
                    shutil.copy2(EXE, UNPATCHED)
                    print(f"made anchor backup MASHED.exe.unpatched (clean SHA matches).")
                else:
                    print("would create MASHED.exe.unpatched (MASHED.exe is clean).")
            else:
                print(f"WARNING: MASHED.exe SHA {sha[:16]}... != anchor {ANCHOR_SHA[:16]}... "
                      f"It may already be patched, or the wrong build. Not creating .unpatched.")
    else:
        print(f"anchor backup present (.unpatched SHA {sha256(UNPATCHED)[:16]}...).")

    if check_only:
        print("--check: not applying patches. Re-run without --check to apply.")
        return 0

    print("\n=== applying boot patches (idempotent) ===")
    failed = []
    for p in PATCHES:
        sp = os.path.join(ROOT, "scripts", p)
        if not os.path.exists(sp):
            print(f"  SKIP {p} (missing)")
            continue
        rc = subprocess.run([sys.executable, sp], cwd=ROOT).returncode
        print(f"  {'OK ' if rc == 0 else 'ERR'} {p} (rc={rc})")
        if rc != 0:
            failed.append(p)

    # Canonical windowed videocfg.
    canon = os.path.join(ROOT, "scripts", "canonical", "videocfg_windowed.bin")
    dst = os.path.join(ORIG, "videocfg.bin")
    if os.path.exists(canon):
        shutil.copy2(canon, dst)
        print("  OK  copied canonical videocfg_windowed.bin -> original/videocfg.bin")

    print("\n=== finish (machine-specific — run these yourself) ===")
    print("  1. mashedmod\\build_d3d9_shim.bat        # rebuild the windowed d3d9 shim")
    print("  2. pwsh scripts\\setup_mashed_compat.ps1  # per-machine AppCompat layer")
    print("  3. py -3.12 scripts\\diag.py doctor        # confirm original=OK, render, etc.")
    if failed:
        print(f"\nNOTE: these patches reported errors: {failed} — inspect before booting.")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
