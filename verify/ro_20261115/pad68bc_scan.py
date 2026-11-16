"""RO-CTL-PAD-68BC: every .text memory operand whose displacement lies in the DAT_0068bb60[4] record array
(0x0068bb60..0x0068bcff, 4 records of 0x68 bytes), any base/index, plus immediates in that range. Static scan of MASHED.exe.unpatched.
Run: py -3.12 verify/ro_20261115/pad68bc_scan.py"""
import pathlib, sys
import capstone, pefile

ROOT = pathlib.Path(__file__).resolve().parents[2]
EXE = ROOT / "original" / "MASHED.exe.unpatched"
if not EXE.exists():
    EXE = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
pe = pefile.PE(str(EXE))
base = pe.OPTIONAL_HEADER.ImageBase
text = next(s for s in pe.sections if s.Name.rstrip(b"\0") == b".text")
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
md.skipdata = True
LO, HI = 0x0068bb60, 0x0068bd00
for ins in md.disasm(text.get_data(), base + text.VirtualAddress):
    if ins.id == 0:
        continue
    for op in ins.operands:
        d = None
        if op.type == capstone.x86.X86_OP_MEM:
            d = op.mem.disp & 0xffffffff
        elif op.type == capstone.x86.X86_OP_IMM:
            d = op.imm & 0xffffffff
        if d is not None:
            if LO <= d < HI:
                print(f"0x{ins.address:08x}  {ins.mnemonic} {ins.op_str}   disp=0x{d:08x} kind={'mem' if op.type == capstone.x86.X86_OP_MEM else 'imm'}")
