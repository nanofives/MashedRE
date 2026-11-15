"""RO-PIZFS-SLOT-TRACE: print N instructions before (and M after) each given call site, decoding from
the containing function start (function_inventory.csv), plus every instruction in that function that
mentions a given register-offset pattern.

usage: py -3.12 callctx.py <exe> <inventory.csv> <before> <after> <site_hex>[,<site_hex>..]
"""
import bisect, csv, sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

pe = pefile.PE(sys.argv[1], fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
md = Cs(CS_ARCH_X86, CS_MODE_32)
inv = []
with open(sys.argv[2], newline='') as f:
    for row in csv.DictReader(f):
        inv.append((int(row['rva'], 16), int(row['body_size'] or 0)))
inv.sort()
starts = [a for a, _ in inv]
nb, na = int(sys.argv[3]), int(sys.argv[4])
for s in sys.argv[5].split(','):
    site = int(s, 16)
    i = bisect.bisect_right(starts, site) - 1
    fs, sz = inv[i]
    data = pe.get_data(fs - base, max(sz, site - fs + 64))
    ins = list(md.disasm(data, fs))
    k = next((j for j, x in enumerate(ins) if x.address == site), None)
    print(f'--- site {site:08x} in fn {fs:08x}' + ('' if k is not None else ' [NOT ANCHORED]'))
    if k is None:
        continue
    for x in ins[max(0, k - nb): k + na + 1]:
        print(f'{x.address:08x}  {x.mnemonic} {x.op_str}')
