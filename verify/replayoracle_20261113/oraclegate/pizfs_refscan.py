"""RO-ORACLE-GATE run 46: static reference scan of the pizfs mask range in the anchored MASHED.exe.

Lists every .text instruction whose memory displacement OR immediate operand falls in
  0x008ad800..0x008cd9c0 (the 0x004b6xxx counter block plus the chunk cache 0x008ad9c0..0x008cd9bf),
plus every 4-byte-aligned dword in every non-code section except .rsrc whose value points into the range (data pointers).
Reports for each hit: address, mnemonic/operands, operand kind (mem disp / imm), access (read/write via capstone regs_access).
Run: py -3.12 verify/replayoracle_20261113/oraclegate/pizfs_refscan.py [<exe>]
"""
import sys, struct, pathlib
import capstone, pefile

ROOT = pathlib.Path(__file__).resolve().parents[3]
exe = sys.argv[1] if len(sys.argv) > 1 else str(ROOT / "original" / "MASHED.exe.unpatched")
LO, HI = 0x008ad800, 0x008cd9c0
pe = pefile.PE(exe)
base = pe.OPTIONAL_HEADER.ImageBase
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
md.skipdata = True
print("exe", exe)
for s in pe.sections:
    name = s.Name.rstrip(b"\0").decode()
    data = s.get_data()
    va0 = base + s.VirtualAddress
    if s.Characteristics & 0x20000000:   # executable
        for ins in md.disasm(data, va0):
            if ins.id == 0:
                continue
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_MEM:
                    d = op.mem.disp & 0xffffffff
                    kind = "mem" if (op.mem.base == 0 and op.mem.index == 0) else "mem+reg"
                elif op.type == capstone.x86.X86_OP_IMM:
                    d = op.imm & 0xffffffff
                    kind = "imm"
                else:
                    continue
                if LO <= d < HI:
                    acc = {0: "-", 1: "read", 2: "write", 3: "rw"}.get(op.access, str(op.access))
                    print(f"{name}\t0x{ins.address:08x}\t{ins.mnemonic} {ins.op_str}\t{kind}\t0x{d:08x}\t{acc}")
    elif name != ".rsrc":   # .rsrc holds icon/bitmap bytes, not pointers
        for i in range(0, len(data) - 3, 4):
            v = struct.unpack_from("<I", data, i)[0]
            if LO <= v < HI:
                print(f"{name}\t0x{va0 + i:08x}\tdword 0x{v:08x}\tdata-ptr\t0x{v:08x}\t-")
