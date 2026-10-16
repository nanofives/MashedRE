"""byte_check.py <image> [<image> ...] : is the original's __ftol body (0x004a2c48..0x004a2cbc, 117 bytes, original/MASHED.exe.unpatched)
present byte for byte in each built image, and where. Also reports the bytes of FtolVerbatim's wrapper pattern (fld dword [esp+4] / call rel32 / ret)
that precedes or follows it, so the thunk onto the body can be seen in the exe and the asi.

Evidence for hooks.csv 0x004a2c48 `byte-identical` after the dualcopy3 edit: the body was NOT edited (FtolVerbatim was added after it),
and this proves the compiled body still equals the original's instruction bytes in both targets.
"""
import sys

import pefile

ORIG = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
VA, N = 0x004A2C48, 117

o = pefile.PE(ORIG)
ob = o.get_data(VA - o.OPTIONAL_HEADER.ImageBase, N)
print(f"original 0x{VA:08x}..0x{VA + N:08x} ({N} bytes): {ob[:16].hex()} ... {ob[-8:].hex()}")
for path in sys.argv[1:]:
    data = open(path, "rb").read()
    hits, i = [], data.find(ob)
    while i >= 0:
        hits.append(i)
        i = data.find(ob, i + 1)
    print(f"{path}: {len(hits)} exact match(es)", end="")
    if hits:
        pe = pefile.PE(path)
        for h in hits:
            rva = pe.get_rva_from_offset(h)
            print(f"  file_off=0x{h:x} rva=0x{rva:x}", end="")
            # the wrapper: fld dword ptr [esp+4] = d9 44 24 04, e8 rel32, c3 -- look in the 64 bytes before the body
            # the wrapper FtolVerbatim: d9 44 24 04 (fld dword [esp+4]) e8 rel32 (call body) c3, anywhere in the image
            pat, j, found = bytes.fromhex("d9442404e8"), data.find(bytes.fromhex("d9442404e8")), []
            while j >= 0:
                rel = int.from_bytes(data[j + 5:j + 9], "little", signed=True)
                tgt = pe.get_rva_from_offset(j) + 9 + rel
                if tgt == rva and data[j + 9] == 0xC3:
                    found.append(pe.get_rva_from_offset(j))
                j = data.find(pat, j + 1)
            print(f"  FtolVerbatim wrapper(s) calling this body: {[hex(x) for x in found]}", end="")
    print()
