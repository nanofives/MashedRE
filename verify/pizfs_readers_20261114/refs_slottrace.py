"""RO-PIZFS-SLOT-TRACE: find every reference to a list of target addresses in MASHED.exe.

For each target: E8/E9 rel32 sites in .text, and raw little-endian dword occurrences in any section
(with the section name). Each .text site is linear-sweep anchored: decoded from the containing
function start in function_inventory.csv (or the nearest preceding 0xCC/0x90-padding boundary) and
kept as CONFIRMED only when an instruction starts at exactly that site.

usage: py -3.12 refs_slottrace.py <MASHED.exe.unpatched> <function_inventory.csv> <hex> [<hex> ...]
"""
import csv, struct, sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

pe = pefile.PE(sys.argv[1], fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
md = Cs(CS_ARCH_X86, CS_MODE_32)
inv = []
with open(sys.argv[2], newline='') as f:
    for row in csv.DictReader(f):
        try:
            inv.append(int(row.get('rva') or row.get('address') or list(row.values())[0], 16))
        except Exception:
            pass
inv.sort()
targets = [int(x, 16) for x in sys.argv[3:]]
text = next(s for s in pe.sections if s.Name.rstrip(b'\0') == b'.text')
tlo = base + text.VirtualAddress
tdata = text.get_data()
thi = tlo + len(tdata)


def start_for(site):
    import bisect
    i = bisect.bisect_right(inv, site) - 1
    s = inv[i] if i >= 0 else tlo
    # padding boundary after s, before site
    p = site - 1
    while p > s and p > site - 0x400:
        if tdata[p - tlo] in (0xCC, 0x90) and tdata[p - 1 - tlo] in (0xCC, 0x90, 0xC3):
            return p + 1 if p + 1 <= site else s
        p -= 1
    return s


def anchored(site):
    s = start_for(site)
    for ins in md.disasm(tdata[s - tlo: site - tlo + 16], s):
        if ins.address == site:
            return ins
        if ins.address > site:
            return None
    return None


def func_of(site):
    import bisect
    i = bisect.bisect_right(inv, site) - 1
    return inv[i] if i >= 0 else 0


for t in targets:
    print(f'=== target {t:08x}')
    for off in range(len(tdata) - 5):
        op = tdata[off]
        if op in (0xE8, 0xE9):
            rel = struct.unpack_from('<i', tdata, off + 1)[0]
            site = tlo + off
            if site + 5 + rel == t:
                ins = anchored(site)
                if ins:
                    print(f'  {"call" if op == 0xE8 else "jmp "} {site:08x}  fn {func_of(site):08x}  {ins.mnemonic} {ins.op_str}')
    pat = struct.pack('<I', t)
    for s in pe.sections:
        d = s.get_data()
        name = s.Name.rstrip(b'\0').decode()
        i = d.find(pat)
        while i >= 0:
            va = base + s.VirtualAddress + i
            if name == '.text':
                # find instruction covering it
                hit = None
                for k in range(1, 12):
                    ins = anchored(va - k)
                    if ins and ins.address + ins.size >= va + 4:
                        hit = ins
                        break
                desc = f'{hit.address:08x} fn {func_of(hit.address):08x} {hit.mnemonic} {hit.op_str}' if hit else 'unanchored'
                print(f'  dword .text {va:08x}  {desc}')
            else:
                print(f'  dword {name} {va:08x}')
            i = d.find(pat, i + 1)
