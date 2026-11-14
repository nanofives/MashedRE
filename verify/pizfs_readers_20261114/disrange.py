import sys, pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
pe = pefile.PE(sys.argv[1], fast_load=True)
md = Cs(CS_ARCH_X86, CS_MODE_32)
base = pe.OPTIONAL_HEADER.ImageBase
for a in sys.argv[2:]:
    lo, hi = (int(x, 16) for x in a.split(':'))
    data = pe.get_data(lo - base, hi - lo + 16)
    print(f'--- {lo:08x}..{hi:08x}')
    for i in md.disasm(data, lo):
        if i.address >= hi: break
        print(f'{i.address:08x}  {i.bytes.hex():<20} {i.mnemonic} {i.op_str}')
