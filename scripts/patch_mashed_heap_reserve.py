# Patch ORIGINAL MASHED.exe's PE optional-header SizeOfHeapReserve so ntdll places
# the default process heap HIGH (off the low 0x30000 region) at process init.
#
# Root cause (this boot): MASHED's 2004 MSVC CRT __sbh init corrupts its own
# process heap when the loader places it at low 0x30000 -> 0xC0000005 write to
# 0x5477 in ntdll!RtlpHeap. The heap base is chosen by ntdll from SizeOfHeapReserve;
# a larger reserve cannot fit in the cramped low address space, so the heap is
# forced above the image (like Frida's incidental 0xdc0000 shift) and init no longer
# corrupts. This patch touches ONLY the heap-reserve field — NO image base, NO RVAs,
# NO code change. Fully reversible.
#
# Usage:
#   py -3.12 scripts/patch_mashed_heap_reserve.py --show
#   py -3.12 scripts/patch_mashed_heap_reserve.py --set 0x04000000
#   py -3.12 scripts/patch_mashed_heap_reserve.py --restore
import sys, struct, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.preheappatch"


def fields(b):
    e_lfanew = struct.unpack_from("<I", b, 0x3C)[0]
    assert b[e_lfanew:e_lfanew+4] == b"PE\x00\x00", "bad PE sig"
    opt = e_lfanew + 24                      # PE sig(4) + COFF(20)
    magic = struct.unpack_from("<H", b, opt)[0]
    assert magic == 0x10B, f"not PE32 (magic 0x{magic:x})"
    return {
        "SizeOfStackReserve": opt + 0x48,
        "SizeOfStackCommit":  opt + 0x4C,
        "SizeOfHeapReserve":  opt + 0x50,
        "SizeOfHeapCommit":   opt + 0x54,
    }


def show(b):
    f = fields(b)
    for name, off in f.items():
        print(f"  {name:20s} @file 0x{off:06x} = 0x{struct.unpack_from('<I', b, off)[0]:08x}")


def main():
    b = bytearray(EXE.read_bytes())
    if "--show" in sys.argv or len(sys.argv) == 1:
        print(f"=== {EXE.name} PE heap/stack sizes ==="); show(b); return 0
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}")
        else:
            print("no backup to restore"); return 1
        return 0
    if "--set" in sys.argv:
        val = int(sys.argv[sys.argv.index("--set") + 1], 16)
        off = fields(b)["SizeOfHeapReserve"]
        old = struct.unpack_from("<I", b, off)[0]
        if not BAK.exists():
            shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
        struct.pack_into("<I", b, off, val)
        EXE.write_bytes(bytes(b))
        print(f"SizeOfHeapReserve 0x{old:08x} -> 0x{val:08x}  ({EXE.name})")
        return 0
    print("usage: --show | --set 0xHEX | --restore"); return 2


if __name__ == "__main__":
    sys.exit(main())
