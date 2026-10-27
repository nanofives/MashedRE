# D4-WSB-BVH-WRITESET static arg-width walk. Recursive descent from the entry of the ORIGINAL body
# (MASHED.exe.unpatched), esp delta d = entry_esp - esp tracked per path (0x004a3440 stack probe = sub esp, eax).
# usage: py -3.12 scan_argwidth.py <entry> <list_lo> <list_hi> <slot> [<slot> ...]   (slot = entry-esp offset, 4 = param_1)
# recursive-descent walk from entry, esp delta tracked per path (d = entry_esp - esp);
# prints every esp-based operand that lands on an entry slot in the given list, plus the next
# instruction (to show masking). Stack probe 0x4a3440 = sub esp, eax (eax from 'mov eax, imm' at entry).
import sys, pefile, capstone
from capstone import x86_const as X
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
text = next(s for s in pe.sections if s.Name.startswith(b".text"))
base = 0x400000 + text.VirtualAddress; code = text.get_data()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail=True
a=int(sys.argv[1],16); LO=int(sys.argv[2],16); HI=int(sys.argv[3],16); slots=[int(x,16) for x in sys.argv[4:]]
def ins_at(pc): return next(md.disasm(code[pc-base:pc-base+16], pc), None)
seen={}; work=[(a,0,0)]; hits=[]; rets=[]; conflicts=[]; n=0; eaxv=0
while work:
    pc,d,eaxv=work.pop()
    while True:
        if pc in seen:
            if seen[pc]!=d: conflicts.append((pc,seen[pc],d))
            break
        seen[pc]=d; i=ins_at(pc); n+=1
        if i is None: hits.append((pc,d,"UNDECODABLE","")); break
        for op in i.operands:
            if op.type==X.X86_OP_MEM and op.mem.base==X.X86_REG_ESP and op.mem.index==0:
                ent=op.mem.disp-d
                for s in slots:
                    if s<=ent<s+4:
                        nx=ins_at(i.address+i.size)
                        hits.append((pc,d,f"slot+{ent:#x} sz={op.size} {i.mnemonic} {i.op_str}", f"next: {nx.mnemonic} {nx.op_str}"))
        m=i.mnemonic; nxt=i.address+i.size
        if m=="mov" and i.op_str.startswith("eax, 0x"): eaxv=int(i.op_str[5:],16)
        if m=="push": d+=4
        elif m=="pop": d-=4
        elif m in("sub","add") and i.operands[0].type==X.X86_OP_REG and i.operands[0].reg==X.X86_REG_ESP and i.operands[1].type==X.X86_OP_IMM:
            d+= i.operands[1].imm if m=="sub" else -i.operands[1].imm
        elif m=="call" and i.op_str=="0x4a3440": d+=eaxv
        if m=="ret": rets.append((pc,d)); break
        if m=="jmp":
            if i.operands[0].type==X.X86_OP_IMM: pc=i.operands[0].imm; continue
            hits.append((pc,d,"INDIRECT JMP "+i.op_str,"")); break
        if m.startswith("j") or m in ("loop","jecxz"):
            work.append((i.operands[0].imm,d,eaxv))
        pc=nxt
LST=sorted(seen)
for p in LST:
    if LO<=p<HI: ii=ins_at(p); print(f"  {p:08x} d={seen[p]:#x} {ii.mnemonic} {ii.op_str}")
print(f"walked {n} insns; rets {[(hex(p),hex(x)) for p,x in rets]}; esp conflicts {len(conflicts)}")
for h in sorted(hits): print(f"{h[0]:08x} d={h[1]:#x} {h[2]}   {h[3]}")
