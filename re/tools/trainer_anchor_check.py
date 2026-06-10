# Locate SciLor MFL.CT code patterns in OUR anchored MASHED.exe.
# The CT's MFL.exe is a different build (direct VA check MISMATCHed), so we
# wildcard-scan for the distinctive opcode structure and recover our build's
# RVAs + the absolute data addresses embedded in the displacements.
#
# Usage: py -3.12 re/tools/trainer_anchor_check.py
import struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "original" / "MASHED.exe.unpatched"

# Pattern syntax: hex bytes, '??' = wildcard. Capture spec: (label, offset-in-
# pattern, kind) where kind 'abs4' = absolute 4-byte data address,
# 'rel4' = call rel32 (target = va_of_next_insn + rel).
PATTERNS = [
    {
        "name": "camera_height_block",
        # fld [ebp-0xB8]; fdiv [abs]; fdiv [abs]; fadd [abs]; fstp [ebp-0xB4];
        # fld [ebp-20]; fmul [ebp-0xB4]; fstp [ebp-20]  (x,y,z scale follows)
        "pat": "D9 85 48 FF FF FF "
               "D8 35 ?? ?? ?? ?? "
               "D8 35 ?? ?? ?? ?? "
               "D8 05 ?? ?? ?? ?? "
               "D9 9D 4C FF FF FF "
               "D9 45 E0 D8 8D 4C FF FF FF D9 5D E0",
        "caps": [("dist_cam_multiplier", 8, "abs4"),
                 ("height_dist_divider", 14, "abs4"),
                 ("height_static_add", 20, "abs4")],
    },
    {
        "name": "elimination_distance_die",
        # mov [esp+1C],0; call X; fcomp [abs]; fnstsw ax; test ah,44; jp
        "pat": "C7 44 24 1C 00 00 00 00 "
               "E8 ?? ?? ?? ?? "
               "D8 1D ?? ?? ?? ?? "
               "DF E0 F6 C4 44 0F 8A",
        "caps": [("distance_fn_call", 9, "rel4"),
                 ("distance_die_const", 15, "abs4")],
    },
]


def parse_pe(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    sec0 = pe + 24 + opt_size
    secs = []
    for i in range(nsec):
        s = sec0 + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode()
        vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
        secs.append((name, vaddr, vsz, roff, rsz))
    return base, secs


def scan(data, base, secs, pat_hex):
    toks = pat_hex.split()
    pat = [None if t == "??" else int(t, 16) for t in toks]
    hits = []
    for name, vaddr, vsz, roff, rsz in secs:
        if name != ".text":
            continue
        blob = data[roff:roff + rsz]
        first = pat[0]
        i = blob.find(bytes([first]))
        while i != -1 and i + len(pat) <= len(blob):
            ok = True
            for j, p in enumerate(pat):
                if p is not None and blob[i + j] != p:
                    ok = False
                    break
            if ok:
                hits.append(base + vaddr + i)
            i = blob.find(bytes([first]), i + 1)
    return hits


def main():
    data = EXE.read_bytes()
    base, secs = parse_pe(data)

    def va_to_off(va):
        rva = va - base
        for _, vaddr, vsz, roff, rsz in secs:
            if vaddr <= rva < vaddr + max(vsz, rsz):
                return roff + (rva - vaddr)
        return None

    rc = 0
    for spec in PATTERNS:
        hits = scan(data, base, secs, spec["pat"])
        print(f"{spec['name']}: {len(hits)} hit(s)")
        if not hits:
            rc = 1
        for va in hits:
            off = va_to_off(va)
            print(f"  at VA 0x{va:08x}")
            for label, po, kind in spec["caps"]:
                raw = struct.unpack_from("<I", data, off + po)[0]
                if kind == "abs4":
                    print(f"    {label} = [0x{raw:08x}]")
                else:
                    tgt = (va + po + 4 + raw) & 0xFFFFFFFF
                    print(f"    {label} -> 0x{tgt:08x}")
    return rc


if __name__ == "__main__":
    sys.exit(main())
