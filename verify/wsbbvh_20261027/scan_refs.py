# D4-WSB-BVH-WRITESET: every instruction in the ORIGINAL .text (MASHED.exe.unpatched) whose memory
# displacement or immediate falls in [lo, hi). Byte-anchored: finds each 4-byte LE occurrence of a value in
# range, then decodes candidate starts 1..11 bytes back and keeps the ones whose instruction covers the
# bytes and carries that value as disp/imm (so no linear-sweep resync can skip a real reference).
# usage: py -3.12 scan_refs.py <lo> <hi>
import sys, pefile, capstone
from capstone import x86_const as X
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
text = next(s for s in pe.sections if s.Name.startswith(b".text"))
base = 0x400000 + text.VirtualAddress; code = text.get_data()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
out = set()
for off in range(len(code) - 3):
    v = int.from_bytes(code[off:off + 4], "little")
    if not (lo <= v < hi):
        continue
    for back in range(1, 12):
        s = off - back
        if s < 0:
            break
        i = next(md.disasm(code[s:s + 16], base + s), None)
        if i is None or s + i.size < off + 4:
            continue
        vals = [op.mem.disp & 0xffffffff for op in i.operands if op.type == X.X86_OP_MEM] + \
               [op.imm & 0xffffffff for op in i.operands if op.type == X.X86_OP_IMM]
        if v in vals:
            acc = "".join(("W" if op.access & capstone.CS_AC_WRITE else "") + ("R" if op.access & capstone.CS_AC_READ else "")
                          for op in i.operands if op.type == X.X86_OP_MEM) or "imm"
            out.add((i.address, v, acc, f"{i.mnemonic} {i.op_str}"))
for a, v, acc, t in sorted(out):
    print(f"{a:08x}  val={v:08x}  {acc:3}  {t}")
