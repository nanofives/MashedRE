"""objdis.py <obj> <symbol>  -- disassemble one function of a COFF object (capstone)."""
import sys, pathlib, capstone
sys.path.insert(0, "re/tools/match"); import coffpe
o = coffpe.Obj(pathlib.Path(sys.argv[1]).read_bytes(), sys.argv[1])
cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for f in o.functions():
    if f["name"] == sys.argv[2]:
        d = o.sections[f["sec"] - 1]["data"][f["off"]:f["off"] + f["size"]]
        print(f"; {f['name']} size {f['size']}")
        for i in cs.disasm(d, 0): print(f"  {i.address:04x}  {i.bytes.hex():20s} {i.mnemonic} {i.op_str}")
