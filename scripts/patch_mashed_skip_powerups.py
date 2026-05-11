# One-time on-disk patch of MASHED.exe to skip the powerups.piz section.
#
# Background: Mashed crashes inside FUN_004b6940 (piz reader inner) on the
# second piz open of the boot sequence on Win11 + RTX 5070 Ti. Every
# attempted runtime patch path (UAL+.asi, Frida-injected .asi, Frida byte
# patch) destabilizes Mashed before it even reaches the patch site. The
# only stable launch path is Explorer-launched bare MASHED.exe.
#
# This script writes the patch directly to MASHED.exe so the user can
# launch normally. The 25-byte sequence at RVA 0x0040295d (which pushes
# the powerups.piz string, opens it, calls two helpers, then closes) is
# replaced with NOPs.
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

    if current_sha == ANCHOR_SHA:
        print("  matches the known-good version anchor (unpatched)")
    else:
        # Could be already patched, or could be a different build. Don't refuse
        # outright — check by looking for the signature.
        if PRE_PATCH not in data and POST_PATCH in data:
            print("  doesn't match anchor but post-patch signature present — already patched")
            return 0
        if PRE_PATCH not in data:
            sys.exit(f"  sha != anchor and pre-patch signature NOT FOUND. Different build? Abort.")

    # Find the offset of the unique pre-patch byte sequence.
    occurrences = []
    pos = 0
    while True:
        i = data.find(PRE_PATCH, pos)
        if i == -1: break
        occurrences.append(i)
        pos = i + 1
    if len(occurrences) != 1:
        sys.exit(f"expected exactly 1 occurrence of pre-patch signature, found {len(occurrences)}")
    offset = occurrences[0]
    print(f"pre-patch sequence at file offset 0x{offset:x} (RVA approximation 0x0040295d)")

    # Backup if not already present.
    if not BACKUP.exists():
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f"  backed up to {BACKUP.name}")
    else:
        print(f"  backup already exists at {BACKUP.name} (not overwriting)")

    # Apply patch.
    patched = bytearray(data)
    patched[offset:offset+25] = POST_PATCH
    MASHED_EXE.write_bytes(bytes(patched))

    post_sha = hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()
    print(f"\npatched MASHED.exe sha256: {post_sha}")
    print(f"  25 bytes at offset 0x{offset:x} → all 0x90 (NOP)")
    print(f"  to restore: copy {BACKUP.name} over MASHED.exe")
    return 0


if __name__ == '__main__':
    sys.exit(main())
