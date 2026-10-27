"""D4-WSB-C4-BROAD-A: U-9026 static check. For every reference to a target RVA in the anchored, unpatched
MASHED.exe (direct call/jmp rel32 and any dword equal to the RVA), disassemble from the return address and
track the six status flags (CF PF AF ZF SF OF) still holding the callee's value. Report the first instruction
that TESTS a live flag (hazard), or the point where all six are overwritten (clean). Follows jmp imm and
call imm (into the callee, with a return stack, so a callee's own ret resumes after its call site); stops at
an unmatched ret / indirect call / indirect jmp (reported as such).

  py -3.12 verify/wsbbroada_20261027/scan_flags_after_call.py <exe.unpatched> 0x0055deb0
"""
import hashlib, struct, sys
import capstone
from capstone import x86_const as X

ANCHOR = "bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e"
p, tgt = sys.argv[1], int(sys.argv[2], 16)
b = open(p, "rb").read()
assert hashlib.sha256(b).hexdigest() == ANCHOR, "binary is not the anchor"
pe = struct.unpack_from("<I", b, 0x3c)[0]; nsec = struct.unpack_from("<H", b, pe + 6)[0]
opt = struct.unpack_from("<H", b, pe + 20)[0]; base = struct.unpack_from("<I", b, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + opt + i * 40
    vs, va, rs, ro = struct.unpack_from("<IIII", b, o + 8)
    secs.append((b[o:o + 8].rstrip(b"\0").decode(), va, vs, ro, rs))
def f2v(off):
    for n, va, vs, ro, rs in secs:
        if ro <= off < ro + rs: return base + va + off - ro
def v2f(v):
    for n, va, vs, ro, rs in secs:
        if base + va <= v < base + va + rs: return v - base - va + ro
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
FL = {"CF": (X.X86_EFLAGS_TEST_CF, X.X86_EFLAGS_MODIFY_CF | X.X86_EFLAGS_RESET_CF | X.X86_EFLAGS_SET_CF | X.X86_EFLAGS_UNDEFINED_CF),
      "PF": (X.X86_EFLAGS_TEST_PF, X.X86_EFLAGS_MODIFY_PF | X.X86_EFLAGS_RESET_PF | X.X86_EFLAGS_SET_PF | X.X86_EFLAGS_UNDEFINED_PF),
      "AF": (X.X86_EFLAGS_TEST_AF, X.X86_EFLAGS_MODIFY_AF | X.X86_EFLAGS_RESET_AF | X.X86_EFLAGS_SET_AF | X.X86_EFLAGS_UNDEFINED_AF),
      "ZF": (X.X86_EFLAGS_TEST_ZF, X.X86_EFLAGS_MODIFY_ZF | X.X86_EFLAGS_RESET_ZF | X.X86_EFLAGS_SET_ZF | X.X86_EFLAGS_UNDEFINED_ZF),
      "SF": (X.X86_EFLAGS_TEST_SF, X.X86_EFLAGS_MODIFY_SF | X.X86_EFLAGS_RESET_SF | X.X86_EFLAGS_SET_SF | X.X86_EFLAGS_UNDEFINED_SF),
      "OF": (X.X86_EFLAGS_TEST_OF, X.X86_EFLAGS_MODIFY_OF | X.X86_EFLAGS_RESET_OF | X.X86_EFLAGS_SET_OF | X.X86_EFLAGS_UNDEFINED_OF)}
WHOLE = {"pushfd", "pushf", "lahf"}   # read every flag
def walk(pc):
    live = set(FL); trail = []; stack = []
    for _ in range(400):
        off = v2f(pc)
        ins = next(md.disasm(b[off:off + 16], pc), None)
        if ins is None: return "UNDECODABLE", trail
        trail.append(f"{ins.address:#010x} {ins.mnemonic} {ins.op_str}")
        ef = ins.eflags
        read = {f for f in live if ef & FL[f][0]}
        if ins.mnemonic in WHOLE: read = set(live)
        if read: return "HAZARD reads " + ",".join(sorted(read)), trail
        live -= {f for f in live if ef & FL[f][1]}
        if not live: return "CLEAN all six flags rewritten", trail
        if ins.mnemonic == "ret":
            if stack: pc = stack.pop(); continue
            return "RET with live " + ",".join(sorted(live)), trail
        if ins.mnemonic == "call":
            if ins.operands[0].type == X.X86_OP_IMM:
                stack.append(ins.address + ins.size); pc = ins.operands[0].imm; continue
            return "INDIRECT CALL with live " + ",".join(sorted(live)), trail
        if ins.mnemonic == "jmp":
            if ins.operands[0].type == X.X86_OP_IMM: pc = ins.operands[0].imm; continue
            return "INDIRECT JMP with live " + ",".join(sorted(live)), trail
        pc = ins.address + ins.size
    return "STEP LIMIT", trail
refs = []
for n, va, vs, ro, rs in secs:
    for off in range(ro, ro + rs - 4):
        if b[off] in (0xE8, 0xE9):
            v = f2v(off); rel = struct.unpack_from("<i", b, off + 1)[0]
            if (v + 5 + rel) & 0xffffffff == tgt: refs.append(("call" if b[off] == 0xE8 else "jmp", v, v + 5))
    i = b.find(struct.pack("<I", tgt), ro, ro + rs)
    while i != -1:
        refs.append(("dword", f2v(i), None)); i = b.find(struct.pack("<I", tgt), i + 1, ro + rs)
print(f"target {tgt:#010x}: {len(refs)} references")
for kind, site, ret in refs:
    if kind != "call":
        print(f"  {kind} at {site:#010x}: not a direct call, listed only"); continue
    verdict, trail = walk(ret)
    print(f"  call at {site:#010x}: {verdict}")
    for t in trail: print("      " + t)
