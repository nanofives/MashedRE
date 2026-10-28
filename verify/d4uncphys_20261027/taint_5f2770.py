import pefile, capstone, re, sys
from capstone.x86 import X86_OP_MEM, X86_OP_REG
pe=pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
B=pe.OPTIONAL_HEADER.ImageBase
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.detail=True
sites=[int(x,16) for x in re.findall(r"//\s+(0x[0-9a-f]{8}) in FUN_\w+\s+\[READ\]\s+MOV \w+,(?:dword ptr )?\[0x005f2770\]", open(sys.argv[1]).read())]
def full(r): # map sub-regs to 32-bit
    r=r.lower(); m={'al':'eax','ah':'eax','ax':'eax','bl':'ebx','bh':'ebx','bx':'ebx','cl':'ecx','ch':'ecx','cx':'ecx','dl':'edx','dh':'edx','dx':'edx','si':'esi','di':'edi'}
    return m.get(r,r)
out=[]
for s in sites:
    code=pe.get_data(s-B,0x200)
    taint={}  # reg -> level (1 = table base 0x005f2728, 2 = record ptr)
    n=0
    for i in md.disasm(code,s):
        n+=1
        if n>60 or i.mnemonic in ('ret','jmp') and n>1: break
        ops=i.operands
        if n==1:
            taint[full(i.reg_name(ops[0].reg))]=1; continue
        if i.mnemonic.startswith('call'):
            for r in ('eax','ecx','edx'): taint.pop(r,None)
            continue
        # memory write?
        if len(ops)>=1 and ops[0].type==X86_OP_MEM and i.mnemonic not in ('cmp','test','push'):
            base=full(i.reg_name(ops[0].mem.base)) if ops[0].mem.base else None
            idx=full(i.reg_name(ops[0].mem.index)) if ops[0].mem.index else None
            lv=taint.get(base) or taint.get(idx)
            if lv: out.append((hex(s),hex(i.address),f"L{lv}",i.mnemonic,i.op_str))
        if len(ops)==2 and ops[0].type==X86_OP_REG:
            d=full(i.reg_name(ops[0].reg))
            if i.mnemonic in ('mov','lea','movzx','movsx'):
                if ops[1].type==X86_OP_REG and full(i.reg_name(ops[1].reg)) in taint: taint[d]=taint[full(i.reg_name(ops[1].reg))]
                elif ops[1].type==X86_OP_MEM:
                    base=full(i.reg_name(ops[1].mem.base)) if ops[1].mem.base else None
                    idx=full(i.reg_name(ops[1].mem.index)) if ops[1].mem.index else None
                    lv=taint.get(base) or taint.get(idx)
                    if i.mnemonic=='lea' and lv: taint[d]=lv
                    elif lv==1: taint[d]=2
                    elif lv==2: taint.pop(d,None)   # value read out of a record: not a pointer we follow
                    else: taint.pop(d,None)
                else: taint.pop(d,None)
            elif i.mnemonic in ('add','sub','imul','shl') and d in taint: pass
            elif d in taint and i.mnemonic not in ('cmp','test'): taint.pop(d,None)
print("sites",len(sites))
for o in out: print(*o)
