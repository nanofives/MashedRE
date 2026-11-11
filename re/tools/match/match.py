"""Matching decompilation check for MASHED.exe (adapted from SimGolf re/tools/match.py, 2026-10-30).

Compile annotated C/C++ with a period compiler and compare each function with the bytes of the anchored binary
(original/MASHED.exe.unpatched, read as a file; image base 0x400000, no base relocations).

    py -3.12 re/tools/match/match.py re/match/pilot/f_0046dbe0.c
    py -3.12 re/tools/match/match.py <file> --flags "/O2" --show --toolchain vc19

Source annotations (one per function, on the line before its definition):
    // MATCH: 0x<va> <decorated-or-plain symbol name>          (module MASHED.exe implied; "MASHED.exe" may precede)
    // FLAGS: /O2                                               (--flags overrides; default DEFAULT_FLAGS)
    // LANG c | c++                                             (default c: Rich prodid 28 = Utc13 C, 373 objects)
    // TOOLCHAIN ddk | rw9044 | vc19                            (default ddk = cl 13.00.9176 / c2 13.00.9178)
                                                                (rw9044 = RenderWare rows, L-131: VC6 SP5 cl 12.00.8804
                                                                 + Processor Pack c2 13.00.9044, Rich prodid 0x30)
`--only 0x<addr>` compares one annotated function (used by match_permute.py).

Comparison: the function's bytes come from its own COMDAT section (/Gy is always passed). Every relocation site in
the obj is masked; on the original side (no relocations in the exe) an operand is an address when it falls inside
the image's sections, unless the recompiled obj holds that same value as a plain literal. Both sides are
disassembled and compared instruction by instruction; accuracy = matching instructions / max(len). Two extra
scores help classify a residual: `regs` (register names folded) and `ops` (mnemonics only).
"""
from __future__ import annotations

import argparse
import difflib
import os
import pathlib
import re
import struct
import subprocess
import sys
import tempfile

import capstone
import pefile

ROOT = pathlib.Path(__file__).resolve().parents[3]
# worktrees carry neither original/ nor tools/ (both untracked): use the main checkout's
MAIN = ROOT.parent.parent if ROOT.parent.name == ".worktrees" else ROOT
EXE = MAIN / "original" / "MASHED.exe.unpatched"
FUNCS = ROOT / "re" / "console" / "pc_funcs.csv"
DEFAULT_FLAGS = "/O2"
DEFAULT_TOOLCHAIN = "ddk"
_VC19 = pathlib.Path(r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207\bin\Hostx86\x86")
TOOLCHAINS = {
    "ddk": {"bin": MAIN / "tools" / "vc7ddk" / "bin", "extra": [],
            "label": "cl/c1/c1xx 13.00.9176 + c2 13.00.9178 (XP DDK)"},
    # L-131 (2026-11-11): the 349 Rich prodid 0x30 build 9044 objects (Utc12_2_C) are the VC6 SP5 Processor Pack
    # back end. tools/vc6pp/bin = cl.exe 12.00.8804 + c1.dll 12.00.8867 + c1xx.dll 12.00.8964 (VC6 SP5) and c2.dll
    # 13.00.9044 from vcpp5.exe (Microsoft download, verify/rwcompiler9044_20261111/RESULT_MATCH-RW-COMPILER-9044.md).
    "rw9044": {"bin": MAIN / "tools" / "vc6pp" / "bin", "extra": [],
               "label": "VC6 SP5 cl 12.00.8804 + Processor Pack c2 13.00.9044"},
    "vc19": {"bin": _VC19, "extra": ["/GS-", "/arch:IA32"], "label": "cl 19.44.35207 (VS 2022)"},
}
REG = re.compile(r"\b(e?[abcd]x|[abcd][lh]|e?[sd]i|e?bp|st\(\d\)|xmm\d)\b")


# ------------------------------------------------------------------ COFF
def coff(path: pathlib.Path):
    d = path.read_bytes()
    machine, nsec, _, symptr, nsym, opt, _ = struct.unpack_from("<HHIIIHH", d, 0)
    if machine == 0 and nsec == 0xFFFF:
        raise SystemExit("bigobj COFF not supported")
    strtab = symptr + nsym * 18

    def name(raw):
        if raw[:4] == b"\0\0\0\0":
            off = struct.unpack_from("<I", raw, 4)[0]
            return d[strtab + off:d.index(b"\0", strtab + off)].decode()
        return raw.rstrip(b"\0").decode()
    secs = []
    for i in range(nsec):
        o = 20 + opt + 40 * i
        n = name(d[o:o + 8])
        size, rawptr, relptr, _, nrel = struct.unpack_from("<IIIIH", d, o + 16)
        rels = [struct.unpack_from("<IIH", d, relptr + 10 * k) for k in range(nrel)]
        secs.append({"name": n, "data": d[rawptr:rawptr + size] if rawptr else b"", "relocs": rels})
    syms, i = [], 0
    while i < nsym:
        o = symptr + 18 * i
        n = name(d[o:o + 8])
        value, secnum, typ, cls, naux = struct.unpack_from("<IhHBB", d, o + 8)
        syms.append({"name": n, "value": value, "sec": secnum, "type": typ, "cls": cls, "index": i})
        i += 1 + naux
    return secs, syms


def file_toolchain(text: str) -> str:
    m = re.search(r"^\s*//\s*TOOLCHAIN\s+(\S+)\s*$", text, re.M)
    tc = m.group(1) if m else DEFAULT_TOOLCHAIN
    if tc not in TOOLCHAINS:
        raise SystemExit(f"unknown // TOOLCHAIN {tc!r}; known: {sorted(TOOLCHAINS)}")
    return tc


def compile_obj(src: pathlib.Path, flags: str, lang: str = "c", toolchain: str = DEFAULT_TOOLCHAIN,
                errors_ok: bool = False):
    """Returns the obj path, or (with errors_ok) (None, compiler output) on a failed compile."""
    tc = TOOLCHAINS[toolchain]
    if not (tc["bin"] / "cl.exe").exists():
        raise SystemExit(f"toolchain {toolchain!r} not present: no cl.exe in {tc['bin']}")
    tmp = pathlib.Path(tempfile.mkdtemp(prefix="m7_"))
    out = tmp / (src.stem + ".obj")
    env = {"PATH": str(tc["bin"]), "INCLUDE": "", "LIB": "", "SystemRoot": os.environ.get("SystemRoot", r"C:\Windows"),
           "TEMP": str(tmp), "TMP": str(tmp)}
    srcarg = f"/Tc{src}" if lang == "c" else f"/Tp{src}"
    cmd = [str(tc["bin"] / "cl.exe"), "/nologo", "/c", "/Gy", *tc["extra"], *flags.split(), f"/Fo{out}",
           f"/Fd{tmp}\\", srcarg]
    r = subprocess.run(cmd, capture_output=True, text=True, env=env)
    if r.returncode != 0 or not out.exists():
        if errors_ok:
            return None, r.stdout + r.stderr
        raise SystemExit(f"compile failed ({toolchain}):\n{r.stdout}{r.stderr}")
    return (out, r.stdout) if errors_ok else out


# ------------------------------------------------------------------ original side
_FUNCS = None


def function_table():
    global _FUNCS
    if _FUNCS is None:
        rows = [ln.split(",") for ln in FUNCS.read_text().splitlines()[1:] if ln.strip()]
        _FUNCS = {int(r[0], 16): int(r[2]) for r in rows}
    return _FUNCS


def ghidra_size(addr: int) -> int | None:
    """Ghidra body size (re/console/pc_funcs.csv), capped at the next function's entry."""
    t = function_table()
    if addr not in t:
        return None
    later = [e for e in t if e > addr]
    return min(t[addr], min(later) - addr) if later else t[addr]


_PE = None


def pe():
    global _PE
    if _PE is None:
        _PE = pefile.PE(str(EXE), fast_load=True)
    return _PE


def original_bytes(addr: int, n: int) -> bytes:
    p = pe()
    return p.get_data(addr - p.OPTIONAL_HEADER.ImageBase, n)


# ------------------------------------------------------------------ compare
def function_from_obj(secs, syms, wanted: str):
    cands = [s for s in syms if s["sec"] > 0 and s["type"] == 0x20 and s["name"] == wanted] or \
            [s for s in syms if s["sec"] > 0 and s["type"] == 0x20 and wanted in s["name"]]
    if not cands:
        raise SystemExit(f"symbol {wanted!r} not in obj; functions: {[s['name'] for s in syms if s['type'] == 0x20]}")
    s = cands[0]
    sec = secs[s["sec"] - 1]
    start = s["value"]
    later = sorted(x["value"] for x in syms if x["sec"] == s["sec"] and x["type"] == 0x20 and x["value"] > start)
    end = later[0] if later else len(sec["data"])
    by_index = {x["index"]: x for x in syms}
    const_syms = {"___except_list", "__except_list"}

    def is_const(symidx):
        sym = by_index.get(symidx, {})
        return sym.get("sec") == -1 or sym.get("name") in const_syms

    relocs = [off - start for off, symidx, _ in sec["relocs"] if start <= off < end and not is_const(symidx)]
    local = {off - start: by_index[symidx]["value"] - start for off, symidx, _ in sec["relocs"]
             if start <= off < end and by_index.get(symidx, {}).get("sec") == s["sec"]}
    return sec["data"][start:end], relocs, s["name"], local


def normalize(code: bytes, base: int, start: int, is_abs, obj: bool = False):
    out = []
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    for ins in md.disasm(code, base):
        m = ins.mnemonic
        if m == "call" and ins.bytes[0] == 0xE8:
            key = "call <fn>"
        elif ins.bytes[0] == 0xE9 and ins.operands and (
                is_abs(ins, 0, ins.operands[0].imm) if obj
                else not (start <= ins.operands[0].imm < start + len(code))):
            key = "jmp <fn>"
        elif ins.bytes[0] in (0xE8, 0xE9, 0xEB, 0xE0, 0xE1, 0xE2, 0xE3) or (ins.bytes[0] in range(0x70, 0x80)) or (
                ins.bytes[0] == 0x0F and ins.bytes[1] in range(0x80, 0x90)):
            tgt = ins.operands[0].imm if ins.operands else 0
            off = (tgt - start) & 0xFFFFFFFF
            if off >= 0x80000000:
                off -= 0x100000000
            key = f"{m} +0x{off:x}"
        else:
            ops = []
            for i, op in enumerate(ins.operands):
                if op.type == capstone.x86.X86_OP_IMM:
                    ops.append("<addr>" if is_abs(ins, i, op.imm & 0xFFFFFFFF) else hex(op.imm & 0xFFFFFFFF))
                elif op.type == capstone.x86.X86_OP_MEM:
                    mem = op.mem
                    disp = mem.disp & 0xFFFFFFFF
                    parts = [ins.reg_name(mem.base) if mem.base else "",
                             ins.reg_name(mem.index) + f"*{mem.scale}" if mem.index else ""]
                    d = "<addr>" if is_abs(ins, i, disp) else (hex(mem.disp) if mem.disp else "")
                    seg = (ins.reg_name(mem.segment) + ":") if mem.segment else ""
                    ops.append(f"{seg}{op.size}[" + "+".join(x for x in parts + [d] if x) + "]")
                else:
                    ops.append(ins.reg_name(op.reg))
            key = m + " " + ",".join(ops)
        out.append((f"{m} {ins.op_str}", key))
    return out


def strip_switch_tables(code: bytes, local: dict) -> bytes:
    targets = [t for site, t in local.items() if site < t < len(code) and t in local]
    return code[:min(targets)] if targets else code


def literal_values(code: bytes, is_abs) -> set[int]:
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    out = set()
    for ins in md.disasm(code, 0):
        for i, op in enumerate(ins.operands):
            if op.type == capstone.x86.X86_OP_IMM:
                v = op.imm & 0xFFFFFFFF
            elif op.type == capstone.x86.X86_OP_MEM:
                v = op.mem.disp & 0xFFFFFFFF
            else:
                continue
            if not is_abs(ins, i, v):
                out.add(v)
    return out


def ratio(a, b):
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    return sum(bl.size for bl in sm.get_matching_blocks()) / max(len(a), len(b), 1), sm


def trim_pad(a):
    while a and (a[-1][0].split()[0] in ("int3", "nop")
                 or a[-1][0] in ("lea ecx, [ecx]", "mov edi, edi", "lea esi, [esi]", "lea esp, [esp]",
                                 "lea ebx, [ebx]")):
        a.pop()
    return a


def score_function(addr: int, secs, syms, sym: str, size: int | None = None):
    """(acc, regs_acc, ops_acc, a, b, full_name, sm)."""
    mine, relocs, full, local = function_from_obj(secs, syms, sym)
    size = size or ghidra_size(addr)
    if size is None:
        raise SystemExit(f"no Ghidra size for 0x{addr:08x}; refusing to guess the original's length")
    orig = original_bytes(addr, size)
    reloc_sites = set(relocs)

    def mine_abs(ins, i, v, _r=reloc_sites):
        op = ins.operands[i]
        if op.type == capstone.x86.X86_OP_IMM and ins.imm_offset:
            return ins.address + ins.imm_offset in _r
        if op.type == capstone.x86.X86_OP_MEM and ins.disp_offset:
            return ins.address + ins.disp_offset in _r
        return False

    p = pe()
    base = p.OPTIONAL_HEADER.ImageBase
    lo = base + min(s.VirtualAddress for s in p.sections)
    hi = base + p.OPTIONAL_HEADER.SizeOfImage
    body = strip_switch_tables(mine, local)
    mine_literals = literal_values(body, mine_abs)

    def orig_abs(ins, i, v):
        return lo <= v < hi and v not in mine_literals

    a = trim_pad(normalize(body, 0, 0, mine_abs, obj=True))
    b = trim_pad(normalize(orig, addr, addr, orig_abs))
    acc, sm = ratio([x[1] for x in a], [x[1] for x in b])
    regs, _ = ratio([REG.sub("R", x[1]) for x in a], [REG.sub("R", x[1]) for x in b])
    ops, _ = ratio([x[1].split()[0] for x in a], [x[1].split()[0] for x in b])
    return acc, regs, ops, a, b, full, sm


def byte_compare(addr: int, secs, syms, sym: str):
    """Whole obj symbol (code + jump/index tables + any tail after them) against the same number of original
    bytes, relocation sites (4 bytes each) masked on both sides. Independent of Ghidra's body size, so it settles
    functions whose epilogue follows a switch table. Returns (n_bytes, differing unmasked byte offsets)."""
    mine, relocs, full, local = function_from_obj(secs, syms, sym)
    while mine and mine[-1] in (0xCC, 0x90):
        mine = mine[:-1]
    orig = original_bytes(addr, len(mine))
    masked = {r + k for r in relocs for k in range(4)}
    bad = [i for i in range(len(mine)) if i not in masked and mine[i] != orig[i]]
    # relocations to labels in this same section (jump tables, internal jumps): the original's absolute target
    # must sit at the same offset from the function start as ours (stored addend added)
    # REL32 sites (a self-recursive `call`) hold a displacement in the original, not an address
    s = [x for x in syms if x["sec"] > 0 and x["type"] == 0x20 and x["name"] == full][0]
    rel32 = {off - s["value"] for off, _, rtype in secs[s["sec"] - 1]["relocs"] if rtype == 0x14}
    for site, tgt in local.items():
        if site + 4 <= len(mine):
            if site in rel32:
                ours = tgt + int.from_bytes(mine[site:site + 4], "little", signed=True)
                theirs = site + 4 + int.from_bytes(orig[site:site + 4], "little", signed=True)
            else:
                ours = tgt + int.from_bytes(mine[site:site + 4], "little")
                theirs = int.from_bytes(orig[site:site + 4], "little") - addr
            if ours != theirs:
                bad.append(site)
    return len(mine), sorted(bad)


def parse_annotations(text: str):
    return [(int(a, 16), s) for a, s in re.findall(r"//\s*MATCH:\s*(?:MASHED\.exe\s+)?(0x[0-9a-fA-F]+)\s+(\S+)", text)]


def file_flags(text: str):
    m = re.search(r"//\s*FLAGS(?:\s+MASHED\.exe)?:\s*(.+)", text)
    return m.group(1).strip() if m else None


def file_lang(text: str):
    m = re.search(r"//\s*LANG\s+(c\+\+(?:cpp)?|cpp|c)(?![\w+])", text)
    return ("c++" if m.group(1) != "c" else "c") if m else "c"


def compare(src: pathlib.Path, flags: str = "", show: bool = False, only: int | None = None, quiet: bool = False,
            toolchain: str | None = None, bytes_mode: bool = False):
    text = src.read_text()
    anns = parse_annotations(text)
    lang = file_lang(text)
    tc = toolchain or file_toolchain(text)
    fl = flags or file_flags(text) or DEFAULT_FLAGS
    obj = None
    results = []
    for addr, sym in anns:
        if only is not None and addr != only:
            continue
        if obj is None:
            obj = coff(compile_obj(src, fl, lang, tc))
        secs, syms = obj
        acc, regs, ops, a, b, full, sm = score_function(addr, secs, syms, sym)
        results.append({"addr": addr, "sym": sym, "full": full, "acc": acc, "regs": regs, "ops": ops,
                        "n_mine": len(a), "n_orig": len(b), "flags": fl, "toolchain": tc, "a": a, "b": b, "sm": sm})
        results[-1]["bytes_n"], results[-1]["bytes_bad"] = byte_compare(addr, secs, syms, sym)
        if quiet:
            continue
        print(f"0x{addr:08x} {full}: {100 * acc:.1f}% (regs {100 * regs:.1f}%, ops {100 * ops:.1f}%)"
              f"  {len(a)}/{len(b)} ins  flags={fl} tc={tc}")
        if show or acc < 1:
            print_diff(a, b, sm, show)
        if bytes_mode:
            n, bad = results[-1]["bytes_n"], results[-1]["bytes_bad"]
            print(f"   bytes: {n} B whole symbol, {len(bad)} differing unmasked bytes"
                  + (f" (first at +0x{bad[0]:x})" if bad else " -> BYTE-IDENTICAL (relocations masked)"))
    return results


def print_diff(a, b, sm, show=False):
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal" and not show:
            continue
        for k in range(max(i2 - i1, j2 - j1)):
            x = a[i1 + k][0] if i1 + k < i2 else ""
            y = b[j1 + k][0] if j1 + k < j2 else ""
            print(f"   {'=' if tag == 'equal' else '!'} {x:44} | {y}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("--flags", default="")
    ap.add_argument("--show", action="store_true")
    ap.add_argument("--only", default="")
    ap.add_argument("--toolchain", default=None)
    ap.add_argument("--bytes", action="store_true", help="also compare the whole obj symbol byte for byte")
    a = ap.parse_args()
    res = compare(pathlib.Path(a.src), a.flags, a.show, int(a.only, 16) if a.only else None, toolchain=a.toolchain, bytes_mode=a.bytes)
    return 0 if res and all(r["acc"] == 1 for r in res) else 1


if __name__ == "__main__":
    sys.exit(main())
