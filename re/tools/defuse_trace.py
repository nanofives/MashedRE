"""Def-use trace of FUN_0056f1f0's arg2 (the local_78 row buffer) to settle whether it
writes past dword 26, which would mean the port's local_78[27] is too small.

Not a regex: walks the linear instruction stream, tracks WHICH registers currently hold
the buffer pointer (arg2), and reports every memory write through one of them plus every
point where the taint is lost. Linear-sweep only, so branch merges are approximated --
flagged in the output rather than glossed.
"""
import capstone
import pefile

VA_LO, VA_HI = 0x0056f1f0, 0x0056f341
pe = pefile.PE("original/MASHED.exe", fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
data = open("original/MASHED.exe", "rb").read()
off = pe.get_offset_from_rva(VA_LO - base)

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
ins = [i for i in md.disasm(data[off:off + 0x400], VA_LO) if i.address <= VA_HI]

# arg2 lives at [esp+0x1c] once the 5 pushes are done (ecx,ebx,ebp,esi,edi).
# 0x0056f234  mov ecx,[esp+0x1c]   -> ecx tainted
# 0x0056f238  mov ebx,ecx          -> ebx tainted
tainted = set()
writes = []
lost = []

for i in ins:
    m, ops = i.mnemonic, i.op_str
    # taint source: a load of arg2 off the stack
    if m == "mov" and "[esp + 0x1c]" in ops and not ops.startswith("dword ptr"):
        dst = ops.split(",")[0].strip()
        tainted.add(dst)
        continue
    # propagate reg<-reg
    if m == "mov" and "," in ops and "[" not in ops:
        dst, src = (x.strip() for x in ops.split(",", 1))
        if src in tainted:
            tainted.add(dst)
            continue
        if dst in tainted:                       # overwritten with something else
            tainted.discard(dst)
            lost.append((i.address, f"{m} {ops}", dst))
            continue
    # lea / arithmetic that redefines a tainted reg
    if m in ("lea", "add", "sub", "shl", "shr", "xor", "or", "and", "inc", "dec", "pop",
             "movsx", "movzx", "imul") and ops:
        dst = ops.split(",")[0].strip()
        if dst in tainted:
            tainted.discard(dst)
            lost.append((i.address, f"{m} {ops}", dst))
    # a memory WRITE whose base register is tainted
    if m in ("mov", "movsx", "movzx") and ops.startswith(("dword ptr [", "word ptr [", "byte ptr [")):
        inside = ops[ops.index("[") + 1: ops.index("]")]
        parts = inside.replace("+", " ").split()
        b = parts[0] if parts else ""
        if b in tainted:
            disp = 0
            for p in parts[1:]:
                if p.startswith("0x"):
                    disp = int(p, 16)
            writes.append((i.address, f"{m} {ops}", b, disp))

print(f"FUN_0056f1f0  0x{VA_LO:08x}..0x{VA_HI:08x}   {len(ins)} instructions")
print(f"\nWRITES through a register holding arg2 ({len(writes)}):")
if writes:
    for a, txt, reg, disp in writes:
        print(f"  {a:#010x}  {txt:<40} base={reg} disp={disp:#x} = dword {disp // 4}")
    mx = max(w[3] for w in writes)
    print(f"\n  max displacement {mx:#x} = dword {mx // 4}")
    print(f"  port buffer is local_78[27] -> dwords 0..26 (0x00..0x68)")
    print("  VERDICT:", "OVERRUN -- buffer too small" if mx // 4 > 26
          else "within bounds, 27 dwords is sufficient")
else:
    print("  none -- arg2 is never written through while tainted")
    print("  VERDICT: FUN_0056f1f0 does not write the row buffer; it is READ-ONLY there,")
    print("           so it is cleared as the overrun source.")

print(f"\nTAINT LOST at ({len(lost)}) -- linear sweep, so a branch could re-taint:")
for a, txt, reg in lost:
    print(f"  {a:#010x}  {txt:<40} ({reg} redefined)")
