"""D4-WSB-C4-SCOPE: does any caller read back an entry stack slot the callee overwrote?

  py -3.12 verify/wsbc4scope_20261027/scan_argslot_callers.py <rva>:<slot,slot..> [...] > argslot_callers.txt

slot = param index K (param_K lives at entry_esp + 4*K). Reads ONLY original/MASHED.exe.unpatched.

1. Static caller set: every `call rel32` / `jmp rel32` in .text whose target is <rva>, plus every 4-byte absolute
   value equal to <rva> anywhere in the image (a data reference = an indirect caller this scan cannot walk).
2. For each call site, walk forward from the return address (jcc both ways, jmp imm followed, depth cap 400
   instructions per site, ret ends a path), with r = esp_now - esp_before_call (0 right after a cdecl return,
   -N after a callee `ret N`). param_K's slot sits at r_slot = 4*(K-1) relative to esp_before_call.
   Per path and per slot:
     READ    an [esp+disp] operand that covers the slot, read before any write to it
     KILL    a write that covers the slot (store or push landing on it): slot dead on that path
     PARAM   a `call` while the slot is at or above esp: the callee's own param window covers the slot
     FREED   esp rises above the slot (add esp / pop): slot below esp, dead on that path
   Also prints any esp-relative access through ebp (caller frame pointer) as EBPFRAME, which this walk does not
   map onto the slot.
"""
import sys, struct
import pefile, capstone
from capstone import x86_const as X

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
pe = pefile.PE(EXE, fast_load=True)
text = next(s for s in pe.sections if s.Name.startswith(b".text"))
TB = 0x400000 + text.VirtualAddress; CODE = text.get_data(); TE = TB + len(CODE)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
X87W = {"fst", "fstp", "fist", "fistp", "fisttp", "fbstp", "fnstcw", "fstcw", "fnstsw", "fstsw"}
_c = {}
def ins_at(pc):
    if pc not in _c:
        _c[pc] = next(md.disasm(CODE[pc - TB:pc - TB + 16], pc), None) if TB <= pc < TE else None
    return _c[pc]

def ret_pop(t):
    seen, work, rets = set(), [t], set()
    while work and len(seen) < 6000:
        pc = work.pop()
        while pc not in seen:
            seen.add(pc); i = ins_at(pc)
            if i is None: break
            if i.mnemonic == "ret": rets.add(i.operands[0].imm if i.operands else 0); break
            if i.mnemonic == "jmp":
                if i.operands[0].type == X.X86_OP_IMM: pc = i.operands[0].imm; continue
                break
            if i.mnemonic.startswith("j") or i.mnemonic in ("loop", "jecxz"): work.append(i.operands[0].imm)
            pc = i.address + i.size
    return rets

def sites(target):
    out = []
    for off in range(len(CODE) - 5):
        op = CODE[off]
        if op in (0xE8, 0xE9):
            rel = struct.unpack_from("<i", CODE, off + 1)[0]
            if TB + off + 5 + rel == target:
                i = ins_at(TB + off)
                if i is not None and i.address == TB + off and i.mnemonic in ("call", "jmp"):
                    out.append((TB + off, i.mnemonic))
    data = []
    for s in pe.sections:
        b = s.get_data(); va = 0x400000 + s.VirtualAddress
        for k in range(len(b) - 3):
            if struct.unpack_from("<I", b, k)[0] == target: data.append(va + k)
    return out, data

def walk_site(site, kind, slots, pop):
    if kind == "jmp":
        return [f"    {site:08x} tail jmp: the jumping function's own caller owns the slots (walk that function's callers)"]
    res = []; start = site + 5
    work = [(start, -pop, {k: True for k in slots}, 0)]; seen = set()
    while work:
        pc, r, live, n = work.pop()
        live = dict(live)
        while n < 400 and any(live.values()):
            key = (pc, r, tuple(sorted(k for k, v in live.items() if v)))
            if key in seen: break
            seen.add(key); i = ins_at(pc); n += 1
            if i is None: res.append(f"    {pc:08x} UNDECODABLE"); break
            m = i.mnemonic
            for op in i.operands:
                if op.type != X.X86_OP_MEM: continue
                b = op.mem.base
                if b == X.X86_REG_EBP and op.mem.disp < 0x400:
                    res.append(f"    {pc:08x} EBPFRAME {m} {i.op_str}")
                if b != X.X86_REG_ESP or op.mem.index: continue
                lo = r + op.mem.disp; hi = lo + (op.size or 4)
                for k in slots:
                    if not live[k]: continue
                    s = 4 * (k - 1)
                    if lo < s + 4 and s < hi:
                        wr = (op.access & capstone.CS_AC_WRITE) or m in X87W
                        rd = (op.access & capstone.CS_AC_READ) and m != "lea"
                        if rd and not (wr and m == "mov"):
                            res.append(f"    {pc:08x} READ  param_{k} slot  {m} {i.op_str}")
                        if wr: res.append(f"    {pc:08x} KILL  param_{k} slot  {m} {i.op_str}"); live[k] = False
            if m == "push":
                r -= 4
                for k in slots:
                    if live[k] and 4 * (k - 1) == r:
                        res.append(f"    {pc:08x} KILL  param_{k} slot  push {i.op_str}"); live[k] = False
            elif m == "pop": r += 4
            elif m in ("add", "sub") and i.operands[0].type == X.X86_OP_REG and i.operands[0].reg == X.X86_REG_ESP \
                    and i.operands[1].type == X.X86_OP_IMM:
                r += i.operands[1].imm if m == "add" else -i.operands[1].imm
            elif m in ("mov", "lea") and i.operands[0].type == X.X86_OP_REG and i.operands[0].reg == X.X86_REG_ESP:
                res.append(f"    {pc:08x} ESPRESET {m} {i.op_str} (walk stops)"); break
            for k in slots:
                if live[k] and 4 * (k - 1) + 4 <= r:
                    res.append(f"    {pc:08x} FREED param_{k} slot (r={r:+#x})"); live[k] = False
            if m == "call":
                for k in slots:
                    if live[k] and 4 * (k - 1) >= r:
                        res.append(f"    {pc:08x} PARAM param_{k} slot is the callee's param_{(4*(k-1)-r)//4+1}: {i.op_str}")
                if i.operands[0].type == X.X86_OP_IMM:
                    rp = ret_pop(i.operands[0].imm); r += max(rp) if rp else 0
            if m == "ret": res.append(f"    {pc:08x} RET with slots live {[k for k in slots if live[k]]}"); break
            if m == "jmp":
                if i.operands[0].type == X.X86_OP_IMM: pc = i.operands[0].imm; continue
                res.append(f"    {pc:08x} INDIRECT JMP (walk stops)"); break
            if m.startswith("j") or m in ("loop", "jecxz"): work.append((i.operands[0].imm, r, dict(live), n))
            pc = i.address + i.size
    return res

for arg in (sys.argv[1:] if __name__ == "__main__" else []):
    t, ks = arg.split(":"); t = int(t, 16); slots = [int(k) for k in ks.split(",")]
    rp = ret_pop(t); pop = max(rp) if rp else 0
    cs, data = sites(t)
    print(f"== {t:#010x} slots {slots}  callee ret {sorted(rp)}  direct sites {len(cs)}  abs refs {[hex(d) for d in data]}")
    for s, kind in cs:
        print(f"  site {s:08x} {kind}")
        lines = walk_site(s, kind, slots, pop)
        for l in dict.fromkeys(lines): print(l)
