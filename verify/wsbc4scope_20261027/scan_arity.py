"""D4-WSB-C4-SCOPE: highest entry stack slot a function reads (recursive descent, esp delta per path).

  py -3.12 verify/wsbc4scope_20261027/scan_arity.py <rva> [...]

Prints, per function, every entry slot K>=1 (entry_esp + 4*K) that an esp-relative operand touches, read or write,
with the first instruction address. Calls adjust esp by the callee's `ret N`. Reads ONLY MASHED.exe.unpatched.
"""
import sys
import capstone
from capstone import x86_const as X
sys.path.insert(0, __import__("os").path.dirname(__file__))
from scan_argslot_callers import ins_at, ret_pop
for a in sys.argv[1:]:
    a = int(a, 16); work = [(a, 0, None)]; seen = {}; slots = {}; notes = []
    while work:
        pc, d, eaxv = work.pop()
        while pc not in seen:
            seen[pc] = d; i = ins_at(pc)
            if i is None: notes.append(f"{pc:#x} undecodable"); break
            m = i.mnemonic
            for op in i.operands:
                if op.type == X.X86_OP_MEM and op.mem.base == X.X86_REG_ESP and not op.mem.index:
                    off = op.mem.disp - d
                    if off >= 4:
                        k = off // 4
                        acc = "W" if op.access & capstone.CS_AC_WRITE or m.startswith("fst") else "R"
                        slots.setdefault((k, acc), pc)
                if op.type == X.X86_OP_MEM and op.mem.base == X.X86_REG_EBP: notes.append(f"{pc:#x} ebp-based {m} {i.op_str}")
            if m == "mov" and i.op_str.startswith("eax, 0x"): eaxv = int(i.op_str[5:], 16)
            if m == "push": d += 4
            elif m == "pop": d -= 4
            elif m in ("add", "sub") and i.operands[0].type == X.X86_OP_REG and i.operands[0].reg == X.X86_REG_ESP and i.operands[1].type == X.X86_OP_IMM:
                d += i.operands[1].imm if m == "sub" else -i.operands[1].imm
            elif m == "call":
                if i.operands[0].type == X.X86_OP_IMM:
                    if i.operands[0].imm == 0x4a3440: d += eaxv or 0
                    else:
                        rp = ret_pop(i.operands[0].imm); d -= max(rp) if rp else 0
                else: notes.append(f"{pc:#x} indirect call {i.op_str}")
            if m == "ret": break
            if m == "jmp":
                if i.operands[0].type == X.X86_OP_IMM: pc = i.operands[0].imm; continue
                notes.append(f"{pc:#x} indirect jmp"); break
            if m.startswith("j") or m in ("loop", "jecxz"): work.append((i.operands[0].imm, d, eaxv))
            pc = i.address + i.size
    ks = sorted(slots)
    print(f"== {a:#010x}: max slot read {max([k for k, c in ks if c == 'R'], default=0)}  "
          + " ".join(f"p{k}{c}@{slots[(k, c)]:#x}" for k, c in ks))
    for n in dict.fromkeys(notes[:6]): print("   ", n)
