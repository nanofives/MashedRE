#!/usr/bin/env python3
"""List every `call rel32` / `jmp rel32` site targeting the given VAs in MASHED.exe .text (read-only).

Ghidra's function list misses the per-track hook-table bodies (they are reached only through the
table), so its --callers output is incomplete for them. A raw E8/E9 scan is not.

Usage: py -3.12 verify/rprops_20261016/find_calls.py 0x00442410 0x00442420 [--exe path]
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from dump_hooktable import load, va2off  # noqa: E402

DEFAULT = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    exe = DEFAULT
    if '--exe' in sys.argv:
        exe = sys.argv[sys.argv.index('--exe') + 1]
        args = [a for a in args if a != exe]
    targets = {int(a, 16) for a in args}
    d, ib, secs = load(exe)
    # .text = first section
    vrva, vsz, rptr, rsz = secs[0]
    base_va = ib + vrva
    end = rptr + min(vsz, rsz)
    hits = {t: [] for t in targets}
    i = rptr
    while i < end - 5:
        op = d[i]
        if op in (0xE8, 0xE9):
            rel, = struct.unpack_from('<i', d, i + 1)
            va = base_va + (i - rptr)
            tgt = (va + 5 + rel) & 0xFFFFFFFF
            if tgt in targets:
                hits[tgt].append((va, 'call' if op == 0xE8 else 'jmp'))
        i += 1
    for t, lst in hits.items():
        print(f"target 0x{t:08x}: {len(lst)} site(s)")
        for va, k in lst:
            print(f"   0x{va:08x} {k}")


if __name__ == '__main__':
    main()
