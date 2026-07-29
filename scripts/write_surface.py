#!/usr/bin/env py -3.12
"""write_surface.py — derive the WRITE SURFACE of a function's call tree.

Why this exists
---------------
The snapshot/restore A/B lane (mashedmod/src/mashed_re/Ai/AiControllerAB.cpp) is
the only verification that works for MUTATORS, which is ~95% of the C2 pool. It
is proven — 5 functions C2→C3 across ~79k paired calls, 0 confirmed mismatches —
and it has not scaled for one reason: every target needs its write surface
enumerated by hand before the driver can snapshot and restore it.

That analysis is slow and it is error-prone in a way that silently degrades the
result. `W8 0x0063bd90` was MISSING from the first version of the AiController
windows, so an un-restored MINE-side write there let the ORIG side skip a rare
RNG draw — which showed up as unexplained "raw transients" until someone found
it two sessions later.

A store is a store; the disassembler can see them all. This walks the call tree
and reports every write, classified by how its address is formed.

Classification
  abs         mov [0x006403b0], ...      exact global, snapshot 4/2/1 bytes
  glob_deref  mov ecx,[0x007dc578] / mov [ecx+0x34], ...
                                         window at *(global) + disp
  arg_deref   mov ecx,[esp+4] / mov [ecx+8], ...
                                         caller-supplied; window depends on the
                                         argument, so the driver must snapshot
                                         from the live pointer at call time
  indexed     mov [eax+ecx*4+0x34], ...  array write; stride from the scale
  unresolved  base provenance not tracked within the function

`unresolved` is the honest bucket and it must not be read as "no write" —
it means read the function.

Usage:
  py -3.12 scripts/write_surface.py 0x00418860 [--depth 3] [--verbose]
"""
import bisect
import collections
import csv
import io
import re
import sys
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "original" / "MASHED.exe.unpatched"

# Library bands: their internals are third-party and are not part of a port's
# write surface in any actionable sense (we do not reimplement them, and both
# A/B sides call the same original code).
BANDS = [(0x004ec000, 0x004fc9e0, "D3DX9-PSGP"),
         (0x00516000, 0x00529fff, "libpng/zlib"),
         (0x005c0000, 0x005c8000, "MSVC-CRT"),
         (0x0057c5b0, 0x005a5820, "qhull/RWPhysics")]


def in_band(va):
    for lo, hi, n in BANDS:
        if lo <= va <= hi:
            return n
    return None


def load():
    pe = pefile.PE(str(EXE), fast_load=True)
    return pe.OPTIONAL_HEADER.ImageBase, pe.get_memory_mapped_image()


def body_len(va, base, data, starts, maxlen=0x600):
    """Bound the body: next known function start, else padding after a ret.

    Bounded because an unbounded scan runs into the neighbouring function and
    attributes ITS writes to this one — four separate confident-wrong answers
    came from exactly that mistake in the screens this was built alongside.
    """
    j = bisect.bisect_right(starts, va)
    limit = min(starts[j], va + maxlen) if j < len(starts) else va + maxlen
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    end, seen_ret = va, False
    for i in md.disasm(data[va - base:va - base + (limit - va)], va):
        if i.mnemonic in ("nop", "int3"):
            if seen_ret:
                break
            continue
        end = i.address + i.size
        if i.mnemonic in ("ret", "retn"):
            seen_ret = True
    return end - va


STORE = re.compile(r"(byte|word|dword|qword) ptr \[([^\]]+)\], ")
ABS = re.compile(r"^0x[0-9a-f]+$")
REGDISP = re.compile(r"^(e[a-z]{2})(?:\s*\+\s*(0x[0-9a-f]+|\d+))?$")
INDEXED = re.compile(r"^(e[a-z]{2})\s*\+\s*(e[a-z]{2})\*(\d)(?:\s*\+\s*(0x[0-9a-f]+))?$")


def scan(va, base, data, starts):
    """Stores made by ONE function body, with base provenance."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    n = body_len(va, base, data, starts)
    prov = {}        # reg -> ('abs', addr) | ('arg', off) | ('call',) | ('other',)
    out, callees = [], []
    for i in md.disasm(data[va - base:va - base + n], va):
        op = i.op_str
        # --- track where each register's value came from
        if i.mnemonic == "mov" and "," in op:
            dst, src = (x.strip() for x in op.split(",", 1))
            if re.match(r"^e[a-z]{2}$", dst):
                m = re.match(r"^dword ptr \[(0x[0-9a-f]+)\]$", src)
                ma = re.match(r"^dword ptr \[esp(?:\s*\+\s*(0x[0-9a-f]+|\d+))?\]$", src)
                if m:      prov[dst] = ("abs", int(m.group(1), 16))
                elif ma:   prov[dst] = ("arg", ma.group(1) or "0")
                elif ABS.match(src): prov[dst] = ("imm", int(src, 16))
                else:      prov[dst] = ("other",)
        elif i.mnemonic == "lea" and "," in op:
            dst = op.split(",")[0].strip()
            m = re.search(r"\[(e[a-z]{2})", op)
            if re.match(r"^e[a-z]{2}$", dst):
                prov[dst] = prov.get(m.group(1), ("other",)) if m else ("other",)
        elif i.mnemonic == "call":
            for r in ("eax", "ecx", "edx"):
                prov[r] = ("call",)
            if op.startswith("0x"):
                callees.append(int(op, 16))
        # --- record stores
        m = STORE.search(op)
        if m and i.mnemonic in ("mov", "and", "or", "xor", "add", "sub"):
            width, expr = m.group(1), m.group(2).strip()
            sz = {"byte": 1, "word": 2, "dword": 4, "qword": 8}[width]
            if ABS.match(expr):
                out.append(("abs", int(expr, 16), sz, i.address))
                continue
            mi = INDEXED.match(expr)
            if mi:
                b = prov.get(mi.group(1), ("other",))
                if b[0] == "imm":
                    b = ("abs-imm", b[1])
                out.append(("indexed", (b, int(mi.group(3)), int(mi.group(4) or "0", 16)),
                            sz, i.address))
                continue
            mr = REGDISP.match(expr)
            if mr:
                reg, disp = mr.group(1), int(mr.group(2) or "0", 16) if mr.group(2) else 0
                if reg in ("esp", "ebp"):
                    continue                    # locals
                b = prov.get(reg, ("other",))
                # A base loaded by IMMEDIATE is an absolute write: `mov esi,
                # 0x0089a344 / mov [esi+4], eax` targets 0x0089a348. Leaving
                # these unresolved buried most of W1 (the per-vehicle AI record
                # block) in the human-review pile when the address is right
                # there in the instruction.
                if b[0] == "imm":
                    out.append(("abs", b[1] + disp, sz, i.address))
                    continue
                kind = {"abs": "glob_deref", "arg": "arg_deref"}.get(b[0], "unresolved")
                out.append((kind, (b, disp), sz, i.address))
                continue
            out.append(("unresolved", expr, sz, i.address))
    return out, callees


def walk(root, depth, base, data, starts):
    seen, frontier, all_stores, tree = set(), [root], [], []
    for d in range(depth + 1):
        nxt = []
        for fn in frontier:
            if fn in seen or in_band(fn):
                continue
            seen.add(fn)
            tree.append((d, fn))
            st, cs = scan(fn, base, data, starts)
            for s in st:
                all_stores.append((fn, d) + s)
            nxt += cs
        frontier = nxt
    return all_stores, tree


# Ground truth: the write surface a human derived by hand for FUN_00418860's
# call tree, documented in mashedmod/src/mashed_re/Ai/AiControllerAB.cpp and
# proven over ~79k paired A/B calls. `--self-test` checks this tool against it.
# W8 is the important row: it was MISSING from the human's first pass and caused
# unexplained transients until found two sessions later. A tool that cannot
# rediscover it is not worth trusting to replace the manual step.
GROUND_TRUTH = [
    ("W1 AI records",    0x0089a360, 0x0089a8a0),
    ("W2 ctrl blocks",   0x007f1038, 0x007f1298),
    ("W3 0x8032d4",      0x008032d4, 0x00803324),
    ("W4 0x7f0ff8",      0x007f0ff8, 0x007f0ffc),
    ("W5 speed fields",  0x008816f4, 0x008816f4 + 4 * 0xd04),
    ("W6 RNG ctx",       0x007dc578, 0x007dc57c),
    ("W7 slot states",   0x005f2770, 0x005f2774),
    ("W8 dedup counter", 0x0063bd90, 0x0063bd94),
]


def self_test(base, data, starts):
    stores, _ = walk(0x00418860, 3, base, data, starts)
    hit = set()
    for fn, d, kind, tgt, sz, at in stores:
        addrs = []
        if kind == "abs":
            addrs = [tgt]
        elif kind == "glob_deref" and tgt[0][0] == "abs":
            addrs = [tgt[0][1]]
        elif kind == "indexed" and tgt[0][0] in ("abs", "abs-imm"):
            addrs = [tgt[0][1]]
        for a in addrs:
            for name, lo, hi in GROUND_TRUTH:
                if lo <= a <= hi:
                    hit.add(name)
    print("SELF-TEST against the hand-derived AiControllerAB windows:\n")
    for name, lo, hi in GROUND_TRUTH:
        print(f"  {'FOUND ' if name in hit else 'MISSED'}  {name:18s} 0x{lo:08x}..0x{hi:08x}")
    print(f"\n  {len(hit)}/{len(GROUND_TRUTH)} windows rediscovered automatically.")
    print("  MISSED rows are the reason the human-review bucket is not optional.")
    return len(hit)


def main():
    if "--self-test" in sys.argv:
        base, data = load()
        starts = sorted({int(r["rva"], 16) for r in
                         csv.DictReader(io.open(ROOT / "hooks.csv", encoding="utf-8"))
                         if r["rva"] and not r["rva"].startswith("#")
                         and int(r["rva"], 16) >= 0x400000})
        sys.exit(0 if self_test(base, data, starts) >= 6 else 1)
    root = int(sys.argv[1], 16)
    depth = int(sys.argv[sys.argv.index("--depth") + 1]) if "--depth" in sys.argv else 3
    base, data = load()
    starts = sorted({int(r["rva"], 16) for r in
                     csv.DictReader(io.open(ROOT / "hooks.csv", encoding="utf-8"))
                     if r["rva"] and not r["rva"].startswith("#")
                     and int(r["rva"], 16) >= 0x400000})
    stores, tree = walk(root, depth, base, data, starts)
    print(f"write surface of 0x{root:08x}, call tree depth {depth}: "
          f"{len(tree)} functions, {len(stores)} stores\n")

    kinds = collections.Counter(s[2] for s in stores)
    print("  by kind:", dict(kinds), "\n")

    absw = collections.defaultdict(int)
    for fn, d, kind, tgt, sz, at in stores:
        if kind == "abs":
            absw[tgt] = max(absw[tgt], sz)
    print(f"  ABSOLUTE globals written ({len(absw)}) — snapshot these directly:")
    for a in sorted(absw):
        print(f"    0x{a:08x}  {absw[a]}B")

    gd = sorted({(t[0][1], t[1]) for fn, d, k, t, sz, at in stores
                 if k == "glob_deref" and t[0][0] == "abs"})
    print(f"\n  VIA GLOBAL POINTER ({len(gd)}) — window at *(global) + disp:")
    for g, disp in gd:
        print(f"    *(0x{g:08x}) + 0x{disp:x}")

    ix = [(t, sz) for fn, d, k, t, sz, at in stores if k == "indexed"]
    print(f"\n  INDEXED/array writes ({len(ix)}) — base+index*scale+disp:")
    for (b, scale, disp), sz in sorted(set((tuple(map(str, [t[0], t[1], t[2]])), s)
                                           for t, s in ix))[:12]:
        print(f"    base={b} scale={scale} disp={disp} width={sz}B")

    unres = [s for s in stores if s[2] in ("unresolved", "arg_deref")]
    print(f"\n  NEEDS A HUMAN ({len(unres)}) — arg-relative or untracked base."
          f"\n  These are NOT 'no write'. Read them.")
    for fn, d, k, t, sz, at in unres[:10]:
        print(f"    0x{at:08x} (in 0x{fn:08x}, depth {d})  {k}  {t}")


if __name__ == "__main__":
    main()
