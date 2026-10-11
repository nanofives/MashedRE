#!/usr/bin/env py -3.12
"""standin_census.py -- which C2 rows would heap/import stand-ins make A/B-testable?

Plan item #3 step 0 (plans/do-you-think-we-foamy-pelican.md). Static only: walks each hooks.csv row's
direct-call tree in MASHED.exe.unpatched with capstone and records the first depth at which the tree
reaches one of these blocker classes:

  crt_alloc   _malloc 0x004a45fb, __nh_malloc 0x004a45cf, _calloc 0x004a467e, plus every CRT-band
              function whose own body calls one of those within 64 bytes of entry (operator new shape)
  crt_free    _free 0x004a460d, plus CRT-band thunks that call it within 64 bytes (operator delete shape)
  rw_alloc    call [reg+0x108|0x110|0x114|0x118] with reg loaded from [0x007d3ff8] (RwGlobals
              memoryFuncs rwmalloc/rwrealloc/rwcalloc, memoryAlloc) -- re/analysis/rwglobals_layout_20260911.md
  rw_free     call/jmp [reg+0x10c|0x11c] off [0x007d3ff8] (rwfree, memoryFree)
  rw_file     call [reg+X] off [0x007d3ff8] with X in 0xd8..0x104 (fileFuncs block, VFS_Open cites +0xf0/+0xf4)
  imp_heap    IAT calls HeapAlloc/HeapFree/VirtualAlloc/GlobalAlloc/LocalAlloc/...
  imp_file    IAT calls CreateFile*/ReadFile/WriteFile/CloseHandle/SetFilePointer/FindFirstFile*/...
  imp_other   any other IAT call (time, registry, window, COM, D3D/DirectInput entry points)

It does NOT judge whether a row is otherwise A/B-able. Output: one TSV row per hooks.csv row.

Usage: py -3.12 re/tools/standin_census.py [--depth 2] [--out re/coverage/standin_census.tsv]
"""
import argparse
import bisect
import collections
import csv
import re
import sys
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
# A worktree has no original/ of its own (never junction it): walk up to the main checkout's copy.
EXE = next((p / "original" / "MASHED.exe.unpatched" for p in (ROOT, *ROOT.parents)
            if (p / "original" / "MASHED.exe.unpatched").exists()), ROOT / "original" / "MASHED.exe.unpatched")
RWG = 0x007d3ff8
CRT_BAND = (0x004a0000, 0x004b3fff)   # calibrated CRT band (hooks.csv lever3 notes)
CRT_ALLOC = {0x004a45fb: "_malloc", 0x004a45cf: "__nh_malloc", 0x004a467e: "_calloc"}
CRT_FREE = {0x004a460d: "_free"}
RW_ALLOC_OFF = {0x108, 0x110, 0x114, 0x118}
RW_FREE_OFF = {0x10c, 0x11c}
HEAP_IMP = re.compile(r"^(Heap|Virtual|Global|Local)(Alloc|Free|ReAlloc|Create|Destroy|Size)")
FILE_IMP = re.compile(r"^(CreateFile|ReadFile|WriteFile|CloseHandle|SetFilePointer|GetFileSize|FindFirstFile|"
                      r"FindNextFile|FindClose|DeleteFile|GetFileAttributes|CreateDirectory|FlushFileBuffers|"
                      r"SetEndOfFile|MoveFile|CopyFile)")


def load():
    pe = pefile.PE(str(EXE))
    base = pe.OPTIONAL_HEADER.ImageBase
    data = pe.get_memory_mapped_image()
    iat = {}
    for d in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
        for imp in d.imports:
            nm = imp.name.decode() if imp.name else f"ord{imp.ordinal}"
            iat[imp.address] = f"{d.dll.decode().lower()}!{nm}"
    return base, data, iat


def imp_class(name):
    fn = name.split("!", 1)[1]
    if HEAP_IMP.match(fn):
        return "imp_heap"
    if FILE_IMP.match(fn):
        return "imp_file"
    return "imp_other"


MEMREF = re.compile(r"^dword ptr \[(0x[0-9a-f]+)\]$")
REGOFF = re.compile(r"^dword ptr \[(e[a-z]{2})(?:\s*\+\s*(0x[0-9a-f]+|\d+))?\]$")


def scan_body(va, size, base, data, iat):
    """(direct call targets, set of (class, detail)) for one body."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    callees, hits = set(), set()
    rwreg, impreg = {}, {}
    for i in md.disasm(data[va - base:va - base + size], va):
        m, op = i.mnemonic, i.op_str
        if m == "mov":
            parts = [p.strip() for p in op.split(",", 1)]
            if len(parts) == 2 and re.fullmatch(r"e[a-z]{2}", parts[0]):
                dst, src = parts
                rwreg.pop(dst, None)
                impreg.pop(dst, None)
                mm = MEMREF.match(src)
                if mm:
                    a = int(mm.group(1), 16)
                    if a == RWG:
                        rwreg[dst] = True
                    elif a in iat:
                        impreg[dst] = iat[a]
            continue
        if m not in ("call", "jmp"):
            continue
        if op.startswith("0x"):
            t = int(op, 16)
            if m == "call" or not (va <= t < va + size):
                callees.add(t)
            continue
        mm = MEMREF.match(op)
        if mm and int(mm.group(1), 16) in iat:
            nm = iat[int(mm.group(1), 16)]
            hits.add((imp_class(nm), nm))
            continue
        if op in impreg:
            hits.add((imp_class(impreg[op]), impreg[op]))
            continue
        mm = REGOFF.match(op)
        if mm and mm.group(1) in rwreg:
            off = int(mm.group(2) or "0", 0)
            if off in RW_ALLOC_OFF:
                hits.add(("rw_alloc", f"RwGlobals+0x{off:x}"))
            elif off in RW_FREE_OFF:
                hits.add(("rw_free", f"RwGlobals+0x{off:x}"))
            elif 0xd8 <= off <= 0x104:
                hits.add(("rw_file", f"RwGlobals+0x{off:x}"))
    return callees, hits


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--depth", type=int, default=2)
    ap.add_argument("--out", default=str(ROOT / "re" / "coverage" / "standin_census.tsv"))
    a = ap.parse_args()

    base, data, iat = load()
    inv = {}
    for r in csv.DictReader(open(ROOT / "re/analysis/function_inventory.csv", encoding="utf-8")):
        try:
            inv[int(r["rva"], 16)] = int(r["body_size"])
        except ValueError:
            pass
    rows = [r for r in csv.DictReader(open(ROOT / "hooks.csv", encoding="utf-8"))
            if r["rva"] and not r["rva"].startswith("#")]
    for r in rows:
        v = int(r["rva"], 16)
        r["_va"] = v + 0x400000 if v < 0x400000 else v
    starts = sorted(set(inv) | {r["_va"] for r in rows})

    cache = {}

    def body(va):
        if va not in cache:
            size = inv.get(va)
            if not size:
                j = bisect.bisect_right(starts, va)
                size = min((starts[j] - va) if j < len(starts) else 0x200, 0x600)
            cache[va] = scan_body(va, size, base, data, iat)
        return cache[va]

    # operator new/delete shapes: CRT-band bodies <= 64 bytes that directly call an allocator/free
    alloc, free = dict(CRT_ALLOC), dict(CRT_FREE)
    for va, sz in inv.items():
        if CRT_BAND[0] <= va <= CRT_BAND[1] and sz <= 64:
            c, _ = body(va)
            if c & set(CRT_ALLOC):
                alloc.setdefault(va, f"crt_alloc_thunk_{va:08x}")
            if c & set(CRT_FREE):
                free.setdefault(va, f"crt_free_thunk_{va:08x}")

    reach = {}
    for r in csv.DictReader(open(ROOT / "re/coverage/reach.tsv", encoding="utf-8"), delimiter="\t"):
        reach[int(r["va"], 16)] = (r["reached_menu"], r["reached_race"])

    def classify(root):
        first = {}           # class -> (depth, detail)
        seen, frontier = {root}, [root]
        for d in range(a.depth + 1):
            nxt = []
            for va in frontier:
                if va in alloc and va != root:
                    first.setdefault("crt_alloc", (d, alloc[va]))
                    continue
                if va in free and va != root:
                    first.setdefault("crt_free", (d, free[va]))
                    continue
                if not (0x401000 <= va < 0x5d0000):
                    continue
                c, h = body(va)
                for cls, det in h:
                    first.setdefault(cls, (d, det))
                for t in c:
                    if t not in seen:
                        seen.add(t)
                        nxt.append(t)
            frontier = nxt
        return first

    classes = ["crt_alloc", "crt_free", "rw_alloc", "rw_free", "rw_file", "imp_heap", "imp_file", "imp_other"]
    out = open(a.out, "w", encoding="utf-8", newline="")
    w = csv.writer(out, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "name", "subsystem", "confidence", "reached_menu", "reached_race"]
               + classes + ["details"])
    for r in rows:
        f = classify(r["_va"])
        rm, rr = reach.get(r["_va"], ("", ""))
        w.writerow([r["rva"], r["name"], r["subsystem"], r["confidence"], rm, rr]
                   + [str(f[c][0]) if c in f else "" for c in classes]
                   + [";".join(f"{c}@{f[c][0]}={f[c][1]}" for c in classes if c in f)])
    out.close()
    print(f"wrote {a.out}; alloc thunks {len(alloc) - len(CRT_ALLOC)}, free thunks {len(free) - len(CRT_FREE)}")


if __name__ == "__main__":
    sys.exit(main())
