"""Print the original disassembly of MASHED.exe functions (size from re/console/pc_funcs.csv).
    py -3.12 re/tools/match/odis.py 0x00423e60 [0x...]"""
import sys, pathlib
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import capstone, match
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for a in sys.argv[1:]:
    addr = int(a, 16); n = match.ghidra_size(addr)
    print(f"; 0x{addr:08x} size {n}")
    for ins in md.disasm(match.original_bytes(addr, n), addr):
        print(f"  {ins.address:08x}  {ins.bytes.hex():20} {ins.mnemonic} {ins.op_str}")
