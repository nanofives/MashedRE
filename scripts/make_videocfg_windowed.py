# Build a videocfg.bin that defaults to a windowed-mode launch.
#
# Boots that switch to fullscreen flicker the monitor (bad for fast iteration).
# Windowed never changes display mode → no flicker. The selector dialog will
# still appear (MASHED always shows it), but with these values pre-loaded
# the default highlighted choice is windowed; clicking OK = windowed launch.
#
# Structure inferred from canonical_videocfg_640x480.bin (fullscreen variant):
#   +0x000: mode_index (display-mode-list index from the engine's enum)
#   +0x004: adapter name (NUL-terminated, up to 32 bytes)
#   +0x18c: width  (uint32)
#   +0x190: height (uint32)
#   +0x194: bpp    (uint32)
#   +0x198: fs flag (uint32; 1 = fullscreen, 0 = windowed)
#   +0x1a4: 1      (purpose unclear; preserved)
#
# Usage:
#   py -3.12 scripts/make_videocfg_windowed.py        # 800x600 windowed by default
#   py -3.12 scripts/make_videocfg_windowed.py 640 480
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CANONICAL_FS = ROOT / 'scripts' / 'canonical' / 'videocfg_640x480.bin'
OUT_FILE     = ROOT / 'scripts' / 'canonical' / 'videocfg_windowed.bin'


def build(width: int, height: int) -> bytes:
    # Start from the fullscreen canonical, then flip resolution + fs flag.
    base = bytearray(CANONICAL_FS.read_bytes())
    # Mode index 0 = the first entry in the engine's display enum, which is
    # always "<native> [Window]" on this machine. Preselect that.
    struct.pack_into('<I', base, 0x000, 0)
    # Clear the optional mode string at +0x100 — empty is valid (we saw both
    # populated and empty in real saves).
    for i in range(0x100, 0x130):
        base[i] = 0
    struct.pack_into('<I', base, 0x18c, width)
    struct.pack_into('<I', base, 0x190, height)
    struct.pack_into('<I', base, 0x194, 32)
    struct.pack_into('<I', base, 0x198, 0)   # fullscreen flag OFF
    return bytes(base)


def main():
    w = int(sys.argv[1]) if len(sys.argv) > 1 else 800
    h = int(sys.argv[2]) if len(sys.argv) > 2 else 600
    OUT_FILE.write_bytes(build(w, h))
    print(f"wrote {OUT_FILE.name}  size=512  {w}x{h} windowed (mode_index=0)")
    print(f"deploy:  copy /Y {OUT_FILE.relative_to(ROOT)} original\\videocfg.bin")
    return 0


if __name__ == '__main__':
    sys.exit(main())
