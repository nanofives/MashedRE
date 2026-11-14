"""List E8/E9 rel32 sites and absolute dword refs to given targets in .text (byte scan, then linear-sweep check).
Usage: py -3.12 callers.py <exe> <inventory.csv> <target> [...]"""
import sys, struct, csv, bisect, pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
pe = pefile.PE(sys.argv[1], fast_load=True); base = pe.OPTIONAL_HEADER.ImageBase
md = Cs(CS_ARCH_X86, CS_MODE_32)
funcs = sorted((int(r['rva'], 16), r['name'], int(r['body_size'] or 0)) for r in csv.DictReader(open(sys.argv[2])))
fst = [f[0] for f in funcs]
t = [s for s in pe.sections if s.Name.startswith(b'.text')][0]; tva = base + t.VirtualAddress; d = t.get_data()
def confirm(iva):
    i = bisect.bisect_right(fst, iva) - 1; s, n, sz = funcs[i]
    a = s
    if iva >= s + sz:
        p = s + sz - tva
        while d[p] in (0x90, 0xCC): p += 1
        a = tva + p
    for ins in md.disasm(d[a - tva:iva - tva + 16], a):
        if ins.address >= iva: return ins.address == iva, n, ins
    return False, n, None
for tg in (int(x, 16) for x in sys.argv[3:]):
    for o in range(len(d) - 5):
        if d[o] in (0xE8, 0xE9) and (tva + o + 5 + struct.unpack_from('<i', d, o + 1)[0]) & 0xffffffff == tg:
            ok, n, ins = confirm(tva + o)
            if ok: print(f'{tg:08x}\t{tva+o:08x}\t{ins.mnemonic} {ins.op_str}\t{n}')
    raw = struct.pack('<I', tg)
    for sec in pe.sections:
        sd = sec.get_data(); o = sd.find(raw)
        while o >= 0:
            print(f'{tg:08x}\tdword@{base+sec.VirtualAddress+o:08x}\t{sec.Name.rstrip(bytes(1)).decode()}')
            o = sd.find(raw, o + 1)
