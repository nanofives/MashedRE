#!/usr/bin/env py -3.12
"""patch_mashed_fix_camera_res.py — make MASHED boot on ANY display configuration.

ROOT CAUSE (traced 2026-06-13, see re/analysis/BOOT_CRASH_ROOTCAUSE_2026-06-13.md):
MASHED's screen-dimension getters return the SELECTED VIDEO MODE's size. With the
resolution selector silenced (patch_mashed_skip_selector.py), MASHED auto-picks the
desktop's best mode (e.g. 2560x1440). DisplayInit (FUN_004921d0) then builds the
active camera's frameBuffer raster at that size via FUN_004670a0 -> RwRasterCreate.
But the d3d9 shim forces the device BACKBUFFER to 640x480, so a 2560x1440 render-
target raster fails to create against the 640x480 device. The camera build returns
0 leaving DAT_006905b0->[0x60] (frameBuffer raster) NULL; the boot path doesn't
check and the viewport-inherit (FUN_0042d4a0 -> FUN_004c7760) derefs the null raster
-> AV at 0x004c7785 ~4s into boot. This made boot depend on the monitor topology
(it worked when the auto-picked mode happened to be small enough).

FIX: force the two screen-dimension getters to return the shim's forced backbuffer
size (640x480) instead of the selected-mode globals, so the camera frameBuffer
raster is always created at exactly the device backbuffer size -> creation succeeds
on ANY display configuration. The getters are the ONLY readers of DAT_00616028/2c
(verified via Ghidra reference_to), so this consistently makes 640x480 the effective
internal resolution everywhere (camera, viewport, HUD) — matching the 640x480
backbuffer the shim already presents 1:1.

  FUN_00498bc0 @ VA 0x00498bc0 (file 0x98bc0):  A1 28 60 61 00 C3  (mov eax,[0x616028];ret)
    -> B8 80 02 00 00 C3  (mov eax,640;ret)
  FUN_00498bd0 @ VA 0x00498bd0 (file 0x98bd0):  A1 2C 60 61 00 C3  (mov eax,[0x61602c];ret)
    -> B8 E0 01 00 00 C3  (mov eax,480;ret)

COUPLING: 640x480 must equal the d3d9 shim's kForceBackBufferWidth/Height
(mashedmod/src/d3d9_shim/d3d9_shim.cpp). If the shim's forced backbuffer changes,
update WIDTH/HEIGHT below to match.

Idempotent + self-checking. `--restore` reverts. Operates on original/MASHED.exe;
original/MASHED.exe.unpatched (the SHA anchor) is never touched.
"""
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "original", "MASHED.exe")

# Default 640x480; --hires forces 1280x960 (2x) to match the d3d9 shim's
# MASHED_HIRES backbuffer for high-res parity capture. The chosen size MUST equal
# the shim's forced backbuffer (else the cam-raster-vs-backbuffer mismatch AVs).
WIDTH, HEIGHT = (1280, 960) if "--hires" in sys.argv else (640, 480)

# (file_offset, original 6 bytes "mov eax,[glob];ret"). Patched form is always
# "B8 <imm32> C3" (mov eax,imm32; ret) — same length.
SITES = [
    (0x98BC0, bytes.fromhex("a128606100c3"), WIDTH),
    (0x98BD0, bytes.fromhex("a12c606100c3"), HEIGHT),
]


def is_patched_form(b):  # any "mov eax,imm32; ret"
    return len(b) == 6 and b[0] == 0xB8 and b[5] == 0xC3


def main():
    restore = "--restore" in sys.argv
    if not os.path.exists(EXE):
        sys.exit(f"missing {EXE}")
    data = bytearray(open(EXE, "rb").read())

    changed = False
    for off, orig, val in SITES:
        patched = bytes((0xB8,)) + int(val).to_bytes(4, "little") + bytes((0xC3,))
        cur = bytes(data[off:off + 6])
        want_to = orig if restore else patched
        if cur == want_to:
            print(f"  0x{off:06x}: already {'restored' if restore else 'patched'}")
            continue
        # Accept the original OR ANY previously-patched res form as a valid base
        # (so we can re-target a different res, and --restore always recovers the
        # original getter regardless of which res was applied).
        if cur != orig and not is_patched_form(cur):
            sys.exit(f"  0x{off:06x}: UNEXPECTED bytes {cur.hex(' ')} "
                     f"— aborting, re-anchor first")
        data[off:off + 6] = want_to
        changed = True
        print(f"  0x{off:06x}: {cur.hex(' ')} -> {want_to.hex(' ')}")

    if changed:
        open(EXE, "wb").write(data)
        print(f"  {'restored' if restore else 'patched'} {EXE} "
              f"(screen dims -> {WIDTH}x{HEIGHT} {'reverted' if restore else 'forced'})")
    else:
        print("  no change needed")


if __name__ == "__main__":
    main()
