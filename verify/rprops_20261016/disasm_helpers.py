#!/usr/bin/env python3
"""Disassemble the enclosing helper of each given call-site VA in MASHED.exe (read-only).

The per-track hook-table bodies are not Ghidra functions, so the function start is found by the
compiler's alignment: the first byte after a `ret` (c3 / c2 xx xx) followed by 0x90/0xCC padding.
Usage: py -3.12 verify/rprops_20261016/disasm_helpers.py 0x00448de0 0x004494c0 ...
"""
import sys
from pathlib import Path

import capstone

sys.path.insert(0, str(Path(__file__).parent))
from dump_hooktable import load, va2off  # noqa: E402

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"


def func_start(d, ib, secs, va):
    o = va2off(va, ib, secs)
    i = o
    while i > 8:
        # padding run followed (looking back) by ret
        if d[i - 1] in (0x90, 0xCC) and d[i] not in (0x90, 0xCC):
            j = i - 1
            while d[j] in (0x90, 0xCC):
                j -= 1
            if d[j] == 0xC3 or d[j - 2] == 0xC2:
                return va - (o - i)
        i -= 1
    return None


def main():
    d, ib, secs = load(EXE)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for a in sys.argv[1:]:
        va = int(a, 16)
        st = func_start(d, ib, secs, va)
        print(f"==== call site 0x{va:08x}  helper start 0x{st:08x}" if st else f"==== 0x{va:08x} start?")
        if not st:
            continue
        o = va2off(st, ib, secs)
        for ins in md.disasm(d[o:o + 0x140], st):
            print(f"  0x{ins.address:08x}  {ins.mnemonic:6s} {ins.op_str}")
            if ins.mnemonic == 'ret':
                break


if __name__ == '__main__':
    main()
