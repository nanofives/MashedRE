# RETIRED PATCH — DO NOT APPLY (kept as un-applier / refusal guard).
#
# 2026-06-01 root cause (see re/analysis/CHANGELOG.md and memory
# project_boot_crash_rws_stacksmash_and_d3d9): NOPping the 25-byte powerups
# call sequence at RVA 0x0040295d causes a downstream stack-imbalance
# ret-to-0 — it WAS boot crash #2 (eip=0). Removing the patch made the
# baseline boot to the real main menu (verify/boot_powerups_removed.png).
# The original "FUN_004b6940 piz reader crash" this patch tried to solve was
# a Frida phantom.
#
# Behavior now: if the NOPs are present, restore the original 25 bytes
# (un-apply); if the original bytes are present, refuse and exit non-zero.
#
# Safety:
#   1. The original is preserved as MASHED.exe.unpatched (created only on
#      first run; not overwritten by subsequent runs).
#   2. The expected pre-patch 25-byte signature is verified before writing,
#      so re-running on an already-patched file is a no-op.
#   3. Restore by deleting/renaming the patched MASHED.exe and restoring
#      .unpatched.
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.unpatched'

# The 25 bytes we expect to find (from Ghidra disasm at 0x0040295d..0x402975).
PRE_PATCH = bytes.fromhex(
    '68 b4 c3 5c 00'   # PUSH 0x5cc3b4 ("d:\\toastart\\common\\powerups\\powerups.piz")
    'e8 19 29 09 00'   # CALL 0x00495280  (open piz wrapper)
    'e8 74 91 05 00'   # CALL 0x0045bae0
    'e8 0f 60 01 00'   # CALL 0x00418980  (thunk_FUN_0041a060)
    'e8 7a 29 09 00'   # CALL 0x004952f0  (close piz)
    .replace(' ', '')
)
POST_PATCH = b'\x90' * 25
ANCHOR_SHA = 'bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e'


def main():
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")

    data = MASHED_EXE.read_bytes()
    if len(PRE_PATCH) != 25 or len(POST_PATCH) != 25:
        sys.exit("internal error: signature length")

    current_sha = hashlib.sha256(data).hexdigest()
    print(f"current MASHED.exe sha256: {current_sha}")

    if PRE_PATCH in data:
        print("RETIRED: powerups call sequence intact — leaving it that way.")
        print("This patch was root-caused 2026-06-01 as boot crash #2 (stack-")
        print("imbalance ret-to-0). It must NOT be applied. Nothing to do.")
        sys.exit("refusing to apply retired patch")

    if POST_PATCH in data:
        # Old patch is live — un-apply it by restoring the original bytes.
        occurrences = []
        pos = 0
        while True:
            i = data.find(POST_PATCH, pos)
            if i == -1:
                break
            occurrences.append(i)
            pos = i + 1
        if len(occurrences) != 1:
            sys.exit(f"expected exactly 1 occurrence of the 25-NOP block, "
                     f"found {len(occurrences)} — restore manually from "
                     f"{BACKUP.name}")
        offset = occurrences[0]
        if not BACKUP.exists():
            sys.exit(f"NOPs found but {BACKUP.name} missing — cannot verify; "
                     f"restore manually")
        restored = bytearray(data)
        restored[offset:offset + 25] = PRE_PATCH
        MASHED_EXE.write_bytes(bytes(restored))
        post_sha = hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()
        print(f"UN-APPLIED: original 25 bytes restored at offset 0x{offset:x}")
        print(f"new sha256: {post_sha}")
        return 0

    sys.exit("neither pre- nor post-patch signature found — different build? "
             "Abort.")


if __name__ == '__main__':
    sys.exit(main())
