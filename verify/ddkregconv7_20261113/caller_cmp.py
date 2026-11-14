"""DDKOBJ-REGCONV-FALLBACK-7: for each unit ddkobj.py refuses with fail:externalized-static-register-convention,
compile the source twice with the recorded DDK compiler and flags: as written (the helper copy `static`), and with the
helper made non-static (the same text transformation static_convention_ok uses). Then compare, function by function,
every function of the object EXCEPT the helper copies themselves (relocation fields masked).

Why this is the question that matters: ddkobj.py externalizes the helper copy (its body goes dead) and repoints the
in-object call sites to the exe body, which VS2022 compiled with the standard convention. In the non-static compile cl
must call the helper with the standard convention (stack arguments, caller cleanup, eax/ecx/edx clobbered). So if
every kept caller is byte-identical between the two compiles, its call sites already follow the standard convention
and the repoint is safe. If a caller differs, it relies on the private convention and the unit must stay refused.

Also prints, per helper, its call sites in the plain object (disassembly window before each call).

usage: py -3.12 verify/ddkregconv7_20261113/caller_cmp.py [src ...]
"""
from __future__ import annotations

import pathlib
import re
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "ddkobj"))
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import coffedit as CE  # noqa: E402
import ddkobj as D     # noqa: E402
import match as M      # noqa: E402
import capstone        # noqa: E402

UNITS = {
    "re/match/t01g/004111c0.c": ["FUN_004103a0"],
    "re/match/s06/00427ca0.c": ["FUN_00427880"],
    "re/match/s06/00427e00.c": ["FUN_00427680", "FUN_004277a0"],
    "re/match/s06/00427f00.c": ["FUN_00427680", "FUN_00427840"],
    "re/match/s06/00427ff0.c": ["FUN_00427680", "FUN_004277a0"],
    "re/match/s06/00428140.c": ["FUN_00427680", "FUN_004277a0"],
    "re/match/s09/00458920.c": ["FUN_00458570"],
}

CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)


def compile_text(src: pathlib.Path, text: str, info):
    tmp = pathlib.Path(tempfile.mkdtemp(prefix="rc7_")) / src.name
    tmp.write_text(text, encoding="utf-8")
    r = M.compile_obj(tmp, info["flags"], info["lang"], info["toolchain"], errors_ok=True)
    if r[0] is None:
        raise SystemExit(f"compile failed: {src}\n{r[1]}")
    return CE.Coff.load(r[0])


def funcs(co):
    out = {}
    for s in co.syms:
        if s.sec > 0 and s.is_func:
            sec = co.section(s.sec)
            b = bytearray(sec.data)
            for off, _si, _ty in sec.relocs:
                b[off:off + 4] = b"\0\0\0\0"
            out[CE.undecorate(s.name)] = (bytes(b), sec, s)
    return out


def call_sites(co, helper):
    """(caller, offset, disasm lines up to and after the call) for every rel32 call to helper."""
    res = []
    hsyms = {s.index for s in co.syms if CE.undecorate(s.name) == helper}
    for s in co.syms:
        if not (s.sec > 0 and s.is_func) or CE.undecorate(s.name) == helper:
            continue
        sec = co.section(s.sec)
        for off, si, _ty in sec.relocs:
            if si in hsyms and off >= 1 and sec.data[off - 1] == 0xE8:
                ins = list(CS.disasm(bytes(sec.data), 0))
                idx = next(i for i, x in enumerate(ins) if x.address == off - 1)
                win = ins[max(0, idx - 6): idx + 3]
                res.append((CE.undecorate(s.name), off - 1,
                            [f"  {x.address:04x}: {x.mnemonic} {x.op_str}" for x in win]))
    return res


CS.detail = True
GPR = {"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp"}
SUB = {"al": "eax", "ah": "eax", "ax": "eax", "bl": "ebx", "bh": "ebx", "bx": "ebx", "cl": "ecx", "ch": "ecx",
       "cx": "ecx", "dl": "edx", "dh": "edx", "dx": "edx", "si": "esi", "di": "edi", "bp": "ebp"}


def live_in(code: bytes, base: int):
    """Registers the code reads before writing them, on the straight-line path from the entry to the first branch or
    return (calls are stepped over: eax/ecx/edx count as written after one). A `push` of esi/edi/ebx/ebp (or ecx, the slot reserve) in the
    prologue is a save, not a read; any other push reads its register. Returns [(reg, address, instruction)]."""
    written, out, prologue = set(), [], True
    for x in CS.disasm(code, base):
        rd, wr = x.regs_access()
        names_r = {SUB.get(x.reg_name(r), x.reg_name(r)) for r in rd}
        names_w = {SUB.get(x.reg_name(r), x.reg_name(r)) for r in wr}
        if prologue and x.mnemonic == "push" and x.op_str in ("esi", "edi", "ebx", "ebp", "ecx"):
            names_r = set()     # callee-saved register save in the prologue
        elif not (x.mnemonic == "sub" and x.op_str.startswith("esp")):
            prologue = False
        if x.mnemonic == "xor" and len(x.operands) == 2 and x.op_str.split(",")[0].strip() == \
                x.op_str.split(",")[1].strip():
            names_r = set()
        for r in sorted((names_r & GPR) - written):
            out.append((r, x.address, f"{x.mnemonic} {x.op_str}"))
            written.add(r)          # report each register once
        written |= names_w & GPR
        if x.mnemonic == "call":
            written |= {"eax", "ecx", "edx"}   # clobbered by any standard-convention callee
        if x.mnemonic.startswith("j") or x.mnemonic.startswith("ret"):
            break
    return out


def never_written(code: bytes, base: int):
    """Registers some instruction of the whole body reads (prologue saves excluded) that no instruction writes (epilogue
    `pop` restores excluded, a `call` counts as writing eax/ecx/edx). Such a register can only hold a caller value."""
    reads, writes, prologue = {}, set(), True
    for x in CS.disasm(code, base):
        rd, wr = x.regs_access()
        names_r = {SUB.get(x.reg_name(r), x.reg_name(r)) for r in rd} & GPR
        names_w = {SUB.get(x.reg_name(r), x.reg_name(r)) for r in wr} & GPR
        if prologue and x.mnemonic == "push" and x.op_str in ("esi", "edi", "ebx", "ebp", "ecx"):
            names_r = set()
        elif not (x.mnemonic == "sub" and x.op_str.startswith("esp")):
            prologue = False
        if x.mnemonic == "pop":
            names_w = set()
        if x.mnemonic == "call":
            names_w |= {"eax", "ecx", "edx"}
        for r in names_r:
            reads.setdefault(r, (x.address, f"{x.mnemonic} {x.op_str}"))
        writes |= names_w
    return [(r, a, t) for r, (a, t) in sorted(reads.items()) if r not in writes]


def original_window(addr: int, before: int = 12, after: int = 5):
    code = M.original_bytes(addr - before, before + after)
    # resync: start at the first offset whose decode lands exactly on addr
    for s in range(before + 1):
        ins = list(CS.disasm(code[s:], addr - before + s))
        if any(x.address == addr for x in ins):
            return [f"  {x.address:08x}: {x.mnemonic} {x.op_str}" for x in ins if x.address <= addr]
    return ["  (no decode resync)"]


def main(argv):
    sel = argv or list(UNITS)
    for rel in sel:
        src = ROOT / rel
        info = D.parse_source(src)
        helpers = UNITS[rel]
        plain = compile_text(src, info["text"], info)
        t2 = info["text"]
        t2 = re.sub(r"#\s*define\s+MATCH_STATIC\s+static\b", "#define MATCH_STATIC", t2)
        for h in helpers:
            t2 = re.sub(r"(^|\n)([ \t]*)static(\s+[^;{(]*\b" + re.escape(h) + r"\s*\()", r"\1\2\3", t2)
        nonst = compile_text(src, t2, info)
        fa, fb = funcs(plain), funcs(nonst)
        print(f"== {rel}  helpers {', '.join(helpers)}")
        verdict_ok = True
        for name in sorted(set(fa) | set(fb)):
            if name in helpers:
                ha, hb = fa.get(name), fb.get(name)
                print(f"   helper {name}: static {len(ha[0]) if ha else '-'} B, non-static "
                      f"{len(hb[0]) if hb else '-'} B, same={bool(ha and hb and ha[0] == hb[0])}")
                continue
            a, b = fa.get(name), fb.get(name)
            same = bool(a and b and a[0] == b[0])
            if not same:
                verdict_ok = False
            print(f"   caller-side {name}: plain {len(a[0]) if a else '-'} B, non-static {len(b[0]) if b else '-'} B, "
                  f"{'IDENTICAL' if same else 'DIFFERENT'}")
        rva_of = {CE.undecorate(n): r for r, n in info["matches"]}
        for h in helpers:
            hrva = int(h.split("_")[1], 16)
            size = M.ghidra_size(hrva) or 0x100
            orig_li = live_in(M.original_bytes(hrva, size), hrva)
            ddk_li = live_in(fa[h][0], 0) if h in fa else []
            print(f"   helper {h} live-in registers, original 0x{hrva:08x}: "
                  + (", ".join(f"{r} @0x{a:08x} ({t})" for r, a, t in orig_li) or "none"))
            print(f"   helper {h} live-in registers, DDK static copy: "
                  + (", ".join(f"{r} @+0x{a:x} ({t})" for r, a, t in ddk_li) or "none"))
            nw = never_written(M.original_bytes(hrva, size), hrva)
            print(f"   helper {h} read and never written, original: "
                  + (", ".join(f"{r} @0x{a:08x} ({t})" for r, a, t in nw) or "none"))
            for caller, off, lines in call_sites(plain, h):
                print(f"   call {caller}+0x{off:x} -> {h} (plain object):")
                print("\n".join(lines))
                if caller in rva_of:
                    print(f"   same site in the original, 0x{rva_of[caller] + off:08x}:")
                    print("\n".join(original_window(rva_of[caller] + off)))
        print(f"   VERDICT {'callers-standard-convention' if verdict_ok else 'callers-depend-on-private-convention'}")


if __name__ == "__main__":
    main(sys.argv[1:])
