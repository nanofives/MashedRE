#!/usr/bin/env python3
"""For each camera-man array base found in the 12 placement helpers, print the instructions that
first write array[0] (the creation helper), so the source clump index (course+0x10110+idx*4) is
read from the bytes. Read-only linear sweep of MASHED.exe .text.

Usage: py -3.12 verify/rprops_20261016/find_camman_arrays.py
"""
import sys
from pathlib import Path

import capstone

sys.path.insert(0, str(Path(__file__).parent))
from dump_hooktable import load  # noqa: E402

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
ARRAYS = {  # array base -> (track, N) from the placement helpers
    0x896460: ("arctic", 5), 0x896280: ("dump", 13), 0x896260: ("egypt?", 6),
    0x896240: ("neustein", 5), 0x896230: ("rouabout", 4), 0x895FE0: ("storm", 6),
    0x895960: ("forest", 6), 0x894D60: ("highway", 9), 0x88FFC0: ("sands", 6),
    0x88FFA0: ("superg", 6), 0x88FF80: ("training", 3), 0x88FF20: ("warzone", 9),
}


def main():
    d, ib, secs = load(EXE)
    vrva, vsz, rptr, rsz = secs[0]
    base = ib + vrva
    code = d[rptr:rptr + min(vsz, rsz)]
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    ins_list = []
    off = 0
    while off < len(code):          # capstone stops at the first bad byte: resume after it
        got = False
        for i in md.disasm(code[off:], base + off):
            ins_list.append(i)
            off = i.address - base + i.size
            got = True
        if not got or off < len(code) and (not ins_list or ins_list[-1].address - base + ins_list[-1].size != off):
            off += 1
        elif off >= len(code):
            break
    idx = {i.address: n for n, i in enumerate(ins_list)}
    for arr, (trk, n) in ARRAYS.items():
        print(f"==== array 0x{arr:06x} {trk} N={n}")
        needle = f"0x{arr:x}"
        hits = [k for k, i in enumerate(ins_list) if needle in i.op_str and
                i.mnemonic in ('mov', 'push', 'lea') and 'dword ptr [0x%x]' % arr in i.op_str]
        for k in hits[:3]:
            for j in range(max(0, k - 6), min(len(ins_list), k + 3)):
                i = ins_list[j]
                print(f"   0x{i.address:08x} {i.mnemonic:6s} {i.op_str}")
            print("   ---")


if __name__ == '__main__':
    main()

