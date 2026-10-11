"""Byte-level membership check (re/analysis/directshow_baseclasses_band_2026-10-11.md) for the 0x0049c5d0..0x004a2680 range (DirectShow BaseClasses ruling).
Run from the repo or a worktree root; reads original/MASHED.exe.unpatched (or ../../original from a worktree) and hooks.csv.
Writes a TSV to argv[1]."""
import bisect, csv, re, struct, sys
import capstone, pefile

# Band widened 2026-10-11 (second ruling) from 0x004a2680 to 0x004a2b5f; argv[2] overrides HI.
LO, HI = 0x0049c5d0, (int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x004a2b5f)
import os
pe = pefile.PE(next(x for x in ('original/MASHED.exe.unpatched', '../../original/MASHED.exe.unpatched') if os.path.exists(x)))
base = pe.OPTIONAL_HEADER.ImageBase
img = pe.get_memory_mapped_image()
secs = [(s.Name.rstrip(b'\0').decode(), base + s.VirtualAddress, s.Misc_VirtualSize) for s in pe.sections]
text = [s for s in secs if s[0] == '.text'][0]
TB, TE = text[1], text[1] + text[2]
def rd(va, n): return img[va - base: va - base + n]

iat = {}
for e in pe.DIRECTORY_ENTRY_IMPORT:
    for i in e.imports:
        iat[i.address] = (i.name or b'?').decode()

hooks = {}
for r in csv.DictReader(open('hooks.csv', encoding='utf-8')):
    if re.fullmatch(r'[0-9a-fA-F]{8}', r['rva'] or ''):
        hooks[int(r['rva'], 16)] = r
starts = sorted(hooks)
def owner(va):
    i = bisect.bisect_right(starts, va) - 1
    return starts[i] if i >= 0 else None
def band(fn):
    r = hooks.get(fn)
    return r['subsystem'] if r else '?'

# linear sweep of .text
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.skipdata = True
code = rd(TB, TE - TB)
calls = []      # (site, target, kind)
imm = []        # (site, value)
impcalls = []   # (site, name)
for ins in md.disasm(code, TB):
    b = ins.bytes
    if ins.mnemonic in ('call', 'jmp') and b[0] in (0xE8, 0xE9) and len(b) == 5:
        calls.append((ins.address, (ins.address + 5 + struct.unpack('<i', b[1:])[0]) & 0xffffffff, ins.mnemonic))
    for op in getattr(ins, 'operands', []) if False else []:
        pass
    # imm32 / disp32 values anywhere in the instruction (push imm, mov r,imm, mov [m],imm, [disp])
    s = ins.op_str
    for m in re.finditer(r'0x([0-9a-f]{6,8})\b', s):
        v = int(m.group(1), 16)
        if v in iat and ins.mnemonic in ('call', 'mov', 'jmp'):
            impcalls.append((ins.address, iat[v]))
        elif base <= v < base + pe.OPTIONAL_HEADER.SizeOfImage:
            imm.append((ins.address, v))

# code refs to each value
from collections import defaultdict
callers = defaultdict(list)
for site, tgt, k in calls:
    callers[tgt].append((site, k))
coderef = defaultdict(list)
for site, v in imm:
    coderef[v].append(site)

# data dword refs to in-range functions (outside .text)
datarefs = defaultdict(list)
for name, va, sz in secs:
    if name == '.text':
        continue
    blob = rd(va, sz)
    for off in range(0, len(blob) - 3, 4):
        v = struct.unpack_from('<I', blob, off)[0]
        if LO <= v <= HI + 0x40 and v in hooks:
            datarefs[v].append(va + off)

# table starts referenced by code: nearest code-referenced address at or below a slot
data_code_refd = sorted(v for v in coderef if not (TB <= v < TE))
def table_of(slot):
    i = bisect.bisect_right(data_code_refd, slot) - 1
    if i < 0:
        return None
    s = data_code_refd[i]
    return s if slot - s < 0x400 else None

def inr(fn): return fn is not None and LO <= fn <= HI

rows = [v for v in starts if LO <= v <= HI]
out = []
for idx, f in enumerate(rows):
    nxt = starts[starts.index(f) + 1] if starts.index(f) + 1 < len(starts) else f + 0x40
    ext = (f, nxt)
    # referrers
    ext_callers = sorted({owner(s) for s, k in callers.get(f, []) if not inr(owner(s))})
    in_callers = sorted({owner(s) for s, k in callers.get(f, []) if inr(owner(s))})
    ext_code = sorted({owner(s) for s in coderef.get(f, []) if not inr(owner(s))})
    in_code = sorted({owner(s) for s in coderef.get(f, []) if inr(owner(s))})
    tabs = []
    ext_tab = []
    for slot in datarefs.get(f, []):
        t = table_of(slot)
        if t is None:
            ext_tab.append(('orphan', slot))
            continue
        owners = sorted({owner(s) for s in coderef[t]})
        if all(inr(o) for o in owners):
            tabs.append(t)
        else:
            ext_tab.append((t, [o for o in owners if not inr(o)]))
    # callees out of range (non-import)
    outcalls = sorted({t for s, t, k in calls if ext[0] <= s < ext[1] and not (LO <= t <= HI)})
    out_bands = sorted({band(owner(t)) for t in outcalls})
    imps = sorted({n for s, n in impcalls if ext[0] <= s < ext[1]})
    game_callee = [hex(t) for t in outcalls if not band(owner(t)).startswith('third-party-library')]
    fits = (not ext_callers) and (not ext_code) and (not ext_tab) and (not game_callee)
    out.append({
        'rva': f'{f:08x}', 'subsystem': hooks[f]['subsystem'], 'conf': hooks[f]['confidence'],
        'name': hooks[f]['name'],
        'in_callers': len(in_callers), 'in_coderef': len(in_code), 'cluster_tables': ' '.join(sorted({hex(t) for t in tabs})),
        'ext_callers': ' '.join(hex(x) if x else '?' for x in ext_callers),
        'ext_coderef': ' '.join(hex(x) if x else '?' for x in ext_code),
        'ext_tables': ' '.join(str(x) for x in ext_tab),
        'out_callees': ' '.join(hex(t) for t in outcalls), 'out_callee_bands': ' '.join(out_bands),
        'game_callees': ' '.join(game_callee),
        'imports': ' '.join(imps), 'fits': 'yes' if fits else 'NO'})

def is_lib(fn):
    r = hooks.get(fn)
    if not r: return False
    return r['subsystem'].startswith('third-party-library') or r['name'].startswith('_') or r['name'].startswith('FID_')
dsecs=[s for s in secs if s[0] != '.text']
def in_data(v): return any(va <= v < va+sz for n,va,sz in dsecs)
refs_by_addr=defaultdict(set)
for site,v in imm:
    if in_data(v): refs_by_addr[v].add(owner(site))
res=[]
for o in out:
    f=int(o['rva'],16); i=starts.index(f); nxt=starts[i+1]
    gc=[t for t in (int(x,16) for x in o['out_callees'].split()) if not is_lib(owner(t))]
    dat=sorted({v for s,v in imm if f<=s<nxt and in_data(v)})
    shared=[]
    for v in dat:
        fp=[g for g in refs_by_addr[v] if g is not None and not inr(g) and not is_lib(g)]
        if fp: shared.append((hex(v),[hex(g) for g in sorted(fp)][:4]))
    o['game_callees2']=' '.join(hex(t)+':'+hooks.get(owner(t),{}).get('name','?') for t in gc)
    o['data_refs']=' '.join(hex(v) for v in dat)
    o['data_shared_with_first_party']=str(shared) if shared else ''
    o['fits2']='NO' if (gc or shared) else 'yes'
    res.append(o)
w = csv.DictWriter(open(sys.argv[1], "w", newline='', encoding='utf-8'), fieldnames=list(res[0]), delimiter='	')
w.writeheader(); w.writerows(res)
from collections import Counter
print(Counter(o['fits2'] for o in res), Counter((o['subsystem'],o['fits2']) for o in res))
for o in res:
    if o['fits2']=='NO': print(o['rva'],o['subsystem'],o['conf'],o['name'],'| gc',o['game_callees2'],'| shared',o['data_shared_with_first_party'])
