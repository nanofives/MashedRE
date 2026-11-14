"""For each candidate hit instruction outside 0x004b6xxx, show the instruction that a linear sweep
from an anchor actually decodes over the 4 hit bytes.
Anchor: the inventory function start when the address is inside that function's body; otherwise the
first byte after the previous inventory body that is not 0x90/0xCC padding.
Usage: py -3.12 cover.py <exe> <inventory.csv> <inrange.tsv>
"""
import sys, csv, bisect, struct
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
pe = pefile.PE(sys.argv[1], fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
md = Cs(CS_ARCH_X86, CS_MODE_32)
funcs = sorted((int(r['rva'], 16), r['name'], int(r['body_size'] or 0)) for r in csv.DictReader(open(sys.argv[2])))
fst = [f[0] for f in funcs]
text = [s for s in pe.sections if s.Name.startswith(b'.text')][0]
tva = base + text.VirtualAddress
data = text.get_data()
seen = set()
for line in open(sys.argv[3]):
    c = line.rstrip('\n').split('\t')
    iva = int(c[1], 16); v = int(c[3][6:], 16)
    # locate hit offset: the 4 value bytes inside the candidate instruction
    raw = struct.pack('<I', v)
    o = data.find(raw, iva - tva, iva - tva + 16)
    hva = tva + o
    if hva in seen: continue
    seen.add(hva)
    i = bisect.bisect_right(fst, iva) - 1
    s, n, sz = funcs[i]
    if iva < s + sz:
        anchor, how = s, n
    else:
        p = s + sz - tva
        while data[p] in (0x90, 0xCC): p += 1
        anchor, how = tva + p, f'after {n} body'
        if anchor > hva: anchor, how = s, n
    cov = None
    for ins in md.disasm(data[anchor - tva:hva - tva + 16], anchor):
        if ins.address <= hva < ins.address + ins.size or ins.address > hva:
            cov = ins; break
    desc = f'{cov.address:08x} {cov.mnemonic} {cov.op_str}' if cov else 'none'
    same = cov is not None and cov.address == iva
    print(f'{c[0]}\t{c[1]}\tvalue={v:08x}\thitbytes@{hva:08x}\tanchor={anchor:08x}({how})\tlinear={desc}\t{"CONFIRMED" if same else "MISDECODE"}')
