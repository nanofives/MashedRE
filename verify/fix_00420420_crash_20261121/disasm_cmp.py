# Disassemble the PROV body of 0x00420420 (bodies309.json, ext relocations resolved) next to the original bytes.
# py -3.12 verify/fix_00420420_crash_20261121/disasm_cmp.py [lo hi]   (body offsets, default 0x0 0x140)
import json, struct, sys
import capstone

BODIES = "D:/MashedWT/PROV-INSITU-REF1_data/bodies309.json"
EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
RVA = 0x00420420
lo, hi = (int(sys.argv[1], 0), int(sys.argv[2], 0)) if len(sys.argv) > 2 else (0, 0x140)

b = [x for x in json.load(open(BODIES))["bodies"] if x["sym"] == "_FUN_00420420"][0]
code = bytearray(bytes.fromhex(b["code"]))
BASE = 0x10000000  # virtual base for the body listing; rel32 calls fixed so targets print as original VAs
for off, tgt in b["ext"].items():
    off = int(off)
    # rel32 at off: value = tgt - (BASE + off + 4)
    struct.pack_into("<i", code, off, (tgt - (BASE + off + 4)) & 0xffffffff if tgt - (BASE + off + 4) >= 0 else tgt - (BASE + off + 4))

pe = open(EXE, "rb").read()
e = struct.unpack_from("<I", pe, 0x3c)[0]
nsec = struct.unpack_from("<H", pe, e + 6)[0]
opt = struct.unpack_from("<H", pe, e + 20)[0]
ib = struct.unpack_from("<I", pe, e + 24 + 28)[0]
sec = e + 24 + opt
fo = None
for i in range(nsec):
    va, rs, rp = struct.unpack_from("<III", pe, sec + i * 40 + 12)
    vs = struct.unpack_from("<I", pe, sec + i * 40 + 8)[0]
    if va <= RVA - ib < va + max(vs, rs):
        fo = RVA - ib - va + rp
orig = pe[fo:fo + b["orig_size"]]

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
print("=== body (offsets) ===")
for i in md.disasm(bytes(code), BASE):
    o = i.address - BASE
    if lo <= o < hi:
        print(f"  +{o:04x}  {i.mnemonic} {i.op_str}")
print("=== original ===")
for i in md.disasm(orig, RVA):
    o = i.address - RVA
    if lo <= o < hi + 0x20:
        print(f"  {i.address:08x} +{o:04x}  {i.mnemonic} {i.op_str}")
