#!/usr/bin/env python3
"""Disassemble a VA range of MASHED.exe (original/MASHED.exe.unpatched, read-only) with capstone and
annotate every absolute memory operand that lands in the image with its dword / float / double value.

    py -3.12 verify/rprops2_20261016/disasm_range.py 0x00441d40 0x00442402 [out.txt]

The decomp is not the witness for x87 streams (verify/d3_gatefire_20261008/leg2_bytes.py note: it reads
qword constants as floats and hides _ftol2 operands), so the bytes are listed here.
"""
import struct
import sys
from pathlib import Path

import capstone

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "rprops_20261016"))
from dump_hooktable import load, va2off  # noqa: E402

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"


def main():
    a = sys.argv[1:]
    lo, hi = int(a[0], 16), int(a[1], 16)
    out = open(a[2], "w", encoding="utf8") if len(a) > 2 else sys.stdout
    d, ib, secs = load(EXE)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    off = va2off(lo, ib, secs)
    code = d[off:off + (hi - lo)]
    for ins in md.disasm(code, lo):
        note = ""
        for op in ins.operands:
            if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and op.mem.disp:
                va = op.mem.disp & 0xFFFFFFFF
                try:
                    o = va2off(va, ib, secs)
                    u32, = struct.unpack_from("<I", d, o)
                    f32, = struct.unpack_from("<f", d, o)
                    f64, = struct.unpack_from("<d", d, o)
                    note = f"   ; [0x{va:08x}] u32=0x{u32:08x} f32={f32!r} f64={f64!r}"
                except Exception:
                    note = f"   ; [0x{va:08x}] (unmapped)"
        print(f"0x{ins.address:08x}  {ins.mnemonic:<7s} {ins.op_str}{note}", file=out)


if __name__ == "__main__":
    main()
