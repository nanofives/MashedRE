#!/usr/bin/env py -3.12
"""dump_asm.py — cheap capstone disassembly of MASHED functions for pipeline workers.

Given function VAs (optionally with an explicit byte size as VA:size), prints each
function's x86 disassembly from the SHA-anchored original/MASHED.exe.unpatched. NO
Ghidra, NO MCP — a few ms. Cluster workers call this so they author reimpls from
ground-truth asm without paying ~25k tokens for a Ghidra session each.

Usage:
  py -3.12 scripts/dump_asm.py 0x004dfaa0:14 0x004dfa10:20 ...
  py -3.12 scripts/dump_asm.py 0x004dfaa0          # no size -> until ret / 256B
"""
import os
import sys

import pefile
import capstone

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "original", "MASHED.exe.unpatched")


def main(argv):
    if not argv:
        print("usage: dump_asm.py 0xVA[:size] [0xVA[:size] ...]")
        return 2
    pe = pefile.PE(EXE, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    text = next(s for s in pe.sections if s.Name.rstrip(b"\x00") == b".text")
    text_va = base + text.VirtualAddress
    code = text.get_data()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for tok in argv:
        va_s, _, size_s = tok.partition(":")
        try:
            va = int(va_s, 16)
        except ValueError:
            continue
        size = int(size_s) if size_s.isdigit() else 256
        off = va - text_va
        if off < 0 or off >= len(code):
            print(f"; {va_s}: out of .text range")
            continue
        print(f"; ===== {hex(va)}  ({size} bytes) =====")
        consumed = 0
        for ins in md.disasm(code[off:off + size], va):
            print(f"  {ins.address:08x}  {ins.mnemonic:7s} {ins.op_str}".rstrip())
            consumed += ins.size
            # if no explicit size, stop at the first ret (small getters are 1-ret)
            if not size_s.isdigit() and ins.mnemonic.startswith("ret"):
                break
            if consumed >= size:
                break
        print()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
