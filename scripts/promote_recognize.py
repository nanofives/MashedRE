# Recognize frontier leaves that fit existing early_window handlers WITHOUT a boot.
# Focus: eax_implicit_void (EAX as implicit `this`, no stack args, no calls, verbatim-asm-portable).
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
EXE=r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
pe=pefile.PE(EXE,fast_load=True); base=pe.OPTIONAL_HEADER.ImageBase
sec=next(s for s in pe.sections if s.Name.rstrip(b'\x00')==b'.text')
data=sec.get_data(); tva=base+sec.VirtualAddress
md=Cs(CS_ARCH_X86,CS_MODE_32)
def body(va,n):
    o=va-tva; return data[o:o+n]
rows=[]
for ln in open(r"C:\Users\maria\Desktop\Proyectos\Mashed\re\analysis\plans\promote_frontier.tsv",encoding='utf-8'):
    p=ln.rstrip('\n').split('\t')
    if len(p)<4 or not p[0].startswith('00'): continue
    try: rows.append((int(p[0],16),p[1],p[2],int(p[3])))
    except: pass

DONE={0x4773f0}
def classify_eax_implicit(insns):
    """EAX is the implicit this: used as [eax+...] base before being assigned;
    no stack-arg reads ([esp+...]); no ecx/edx/esi/edi/ebp as INPUTS; no calls;
    ends in ret. Verbatim-asm-portable."""
    eax_written_first=False
    used_eax_mem=False
    for ins in insns:
        m=ins.mnemonic; op=ins.op_str
        if m=='call': return None
        if '[esp' in op or '[ebp' in op: return None
        # any memory operand referencing a non-eax base reg as input source -> not pure-eax
        # (allow scratch ecx/edx/dl used as VALUES set inside; but not as mem base)
        for br in ['ecx','edx','esi','edi','ebx']:
            if '['+br in op: return None
        if '[eax' in op:
            used_eax_mem=True
        # detect eax getting clobbered as a destination (mov/lea/xor/pop eax, ...) BEFORE used as base
        dest=op.split(',')[0].strip()
        if dest=='eax' and not used_eax_mem and m in ('mov','lea','xor','pop','add','sub','or','and'):
            return None  # eax set before used as this -> not implicit-this
    if not used_eax_mem: return None
    if insns[-1].mnemonic!='ret': return None
    return 'eax_implicit_void'

hits=[]
for va,name,sub,sz in rows:
    if sz<6 or sz>120 or va in DONE: continue
    insns=list(md.disasm(body(va,min(sz,120)),va))
    if len(insns)<2: continue
    # must end in ret within the decoded window
    if not any(i.mnemonic=='ret' for i in insns): continue
    insns=insns[:[i.mnemonic for i in insns].index('ret')+1]
    k=classify_eax_implicit(insns)
    if k:
        nwrite=sum(1 for i in insns if i.op_str.strip().startswith('[eax'))
        hits.append((va,name,sub,sz,nwrite))
print(f"eax_implicit_void candidates: {len(hits)}")
for va,name,sub,sz,nw in sorted(hits, key=lambda x:-x[4]):
    print(f"  0x{va:08x} {sub:9s} {sz:3d}B  eax-mem-ops={nw}")
