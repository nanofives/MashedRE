"""For each target RVA: every direct `call rel32` site in MASHED.exe .text, then a forward walk
(following jcc/jmp, depth-limited) until EAX is fully written (kill), or a read of EAX/AX/AL/AH."""
import sys, struct, pefile, capstone
from capstone import x86_const as X
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
text = next(s for s in pe.sections if s.Name.startswith(b".text"))
base = 0x400000 + text.VirtualAddress; code = text.get_data()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
FULL = {X.X86_REG_EAX}; LOW16 = {X.X86_REG_AX, X.X86_REG_AL, X.X86_REG_AH}
def walk(pc, depth, seen):
    out = []
    for _ in range(40):
        if pc in seen: return out
        seen.add(pc)
        off = pc - base
        ins = next(md.disasm(code[off:off + 16], pc), None)
        if ins is None: return out + [("?", pc, "undecodable")]
        rd, wr = ins.regs_access()
        if ins.mnemonic in ("ret", "retn"): return out + [("RET", pc, "")]   # eax passes up: caller's caller
        hitf = [r for r in rd if r in FULL]; hitl = [r for r in rd if r in LOW16]
        if ins.mnemonic in ("xor", "sub") and len(ins.operands) == 2 and ins.op_str in ("eax, eax",): hitf = []; hitl = []
        if hitf: return out + [("READ32", pc, ins.mnemonic + " " + ins.op_str)]
        if hitl: out = out + [("read16", pc, ins.mnemonic + " " + ins.op_str)]
        if ins.mnemonic == "call": return out + [("KILL", pc, "call")]
        if X.X86_REG_EAX in wr: return out + [("KILL", pc, ins.mnemonic + " " + ins.op_str)]
        if ins.mnemonic.startswith("j"):
            op = ins.operands[0]
            if op.type != X.X86_OP_IMM: return out + [("?", pc, "indirect " + ins.op_str)]
            if depth > 0: out += walk(op.imm, depth - 1, seen)
            if ins.mnemonic == "jmp": return out
        pc = ins.address + ins.size
    return out + [("?", pc, "walk limit")]
for a in sys.argv[1:]:
    tgt = int(a, 16); sites = []
    for i in range(len(code) - 5):
        if code[i] == 0xE8 and (base + i + 5 + struct.unpack_from("<i", code, i + 1)[0]) & 0xffffffff == tgt:
            sites.append(base + i)
    print(f"== {a}: {len(sites)} direct call sites")
    for s in sites:
        r = walk(s + 5, 3, set())
        print("  %08x -> %s" % (s, "; ".join(f"{k}@{p:08x} {d}" for k, p, d in r)))
