"""disasm_asi_export.py <asi> <ExportName> [nbytes] : capstone listing of an export of the built .asi."""
import sys

import capstone
import pefile

asi, name = sys.argv[1], sys.argv[2]
n = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x60
pe = pefile.PE(asi)
base = pe.OPTIONAL_HEADER.ImageBase
sym = [e for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name and e.name.decode() == name]
assert sym, f"{name} not exported"
rva = sym[0].address
off = pe.get_offset_from_rva(rva)
data = open(asi, "rb").read()[off:off + n]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
print(f"{name} rva=0x{rva:x} file_off=0x{off:x}")
for i in md.disasm(data, base + rva):
    print(f"  0x{i.address:08x}  {i.bytes.hex():16s} {i.mnemonic} {i.op_str}")
