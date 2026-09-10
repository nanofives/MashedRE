#!/usr/bin/env python3
"""decomp2port.py -- Lane 2 transcriber: Ghidra decompilation -> verbatim MSVC C++ port TU.

    py -3.12 re/tools/decomp_pc.py --file rvas.txt --callees --port --json -o decomp.json
    py -3.12 re/tools/decomp2port.py decomp.json                 # classify only (dry run)
    py -3.12 re/tools/decomp2port.py decomp.json --emit Lane2/Batch_20260910.cpp --apply

WHAT IT IS
----------
The verbatim ports in this repo (the RWP island, K1..K24) were written by pasting Ghidra's C
and applying the same mechanical edits every time: Ghidra typedefs, `DAT_xxxxxxxx` globals as
reads through the absolute address, `FUN_xxxxxxxx` callees as raw-RVA thunks (or the port's
symbol when one exists), `extern "C" __cdecl` + `RH_ScopedInstall`, and a shadow A/B wrapper.
This script does those edits. It does NOT understand the function; it refuses anything that
needs understanding, and it says why per row so the yield is an honest number.

INPUT: the JSON from `decomp_pc.py --port` (decompiler prototype, typed globals, callee
prototypes -- the plain C text does not carry those).

ACCEPTS (kind=port): every parameter is a stack slot; no `unaff_`/`in_` register inputs; no
indirect `(**(code **)...)` calls; every callee has a usable decompiler prototype (or an
existing port symbol); no varargs; body free of Ghidra pseudo-ops it cannot map.

REFUSES with a reason (nothing emitted for that row):
  REGISTER_ABI        param in a register or `unaff_ESI`/`in_EAX` in the body -> needs naked asm
  INDIRECT_CALL       `(**(code **)(...))(...)` -- calling convention of the target unknown
  CALLEE_PROTO        a callee has no decompiler prototype and no existing port
  VARARGS / PSEUDO_OP  `...`, `CONCAT`, `SUB4`, `ZEXT`, `halt_baddata`, `in_ST0`, `float10` returns
  DECOMP_MISSING      the decompiler produced nothing usable

WHAT A GENERATED TU IS NOT: it is not C3. It is a C2 row's reimplementation, at best. It becomes
C3 only through the shadow lane (`shadow_gen.py` wrapper is emitted for return-value functions;
void functions are emitted un-shadowed and flagged NEEDS_REGION) plus the caller/callee gate.

HAZARDS HANDLED MECHANICALLY (each also logged per row so a reviewer sees them):
  * Ghidra prints float arguments as `0x3f800000` when the callee's param type is float:
    the argument is rewritten to `f32bits(0x3f800000u)` (bit reinterpretation), never a
    numeric conversion.
  * `.rdata` constants are read through their address at runtime (`*(const float*)0x...`),
    exactly like the writable globals; nothing is folded into a literal.
  * Globals typed `undefined` (size 1) are `unsigned char` so `&DAT_x + n` stays byte
    arithmetic, as Ghidra meant it.
"""
import argparse
import json
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "mashedmod" / "src" / "mashed_re"
RSP_ASI = ROOT / "mashedmod" / "asi_sources.rsp"

GHIDRA_TYPES = {
    "undefined": "unsigned char", "undefined1": "unsigned char", "undefined2": "unsigned short",
    "undefined4": "unsigned int", "undefined8": "unsigned long long", "byte": "unsigned char",
    "uint": "unsigned int", "ushort": "unsigned short", "ulonglong": "unsigned long long",
    "longlong": "long long", "bool": "bool", "int": "int", "char": "char", "short": "short",
    "float": "float", "double": "double", "void": "void", "code": "void",
}
PSEUDO = re.compile(r"\b(CONCAT\d+|SUB\d+|ZEXT\d+|SEXT\d+|halt_baddata|in_ST0|in_FS_OFFSET|swi\(|coprocessor)")
INDIRECT = re.compile(r"\(\*+\s*\(code\s*\*+\)|code|\(\s*\*+\s*[A-Za-z_][\w\[\]\.\->+ ]*\)\s*\(")
REGVAR = re.compile(r"\b(unaff_[A-Z]{2,3}|in_[A-Z]{2,3})\b")
GLOBAL = re.compile(r"\b(_?DAT_[0-9a-fA-F]{8}|PTR_[A-Za-z_]*_[0-9a-fA-F]{8}|(?:_?s|_?u|_?f)_[A-Za-z0-9_]+_[0-9a-fA-F]{8})\b")
CALLNAME = re.compile(r"\b(FUN_[0-9a-fA-F]{8})\s*\(")
HEXLIT_FLOAT = re.compile(r"^0x[0-9a-fA-F]{8}$")


def ctype(t):
    """Ghidra type name -> C++ type text (pointers preserved)."""
    t = t.strip()
    m = re.match(r"^([A-Za-z_][A-Za-z0-9_]*)\s*((?:\s*\*)*)$", t)
    if not m:
        return None
    base, stars = m.group(1), m.group(2).replace(" ", "")
    if base in GHIDRA_TYPES:
        return GHIDRA_TYPES[base] + (" " + stars if stars else "")
    if re.match(r"^(int|float|double|char|short|long|unsigned|bool|void)$", base):
        return t
    return None                                   # struct/typedef we do not know


def existing_ports():
    """RVA -> port symbol for every live RH_ScopedInstall in the tree."""
    m = {}
    rx = re.compile(r"^[ \t]*RH_ScopedInstall\(\s*(\w+)\s*,\s*0x0*([0-9a-fA-F]+)\s*\)", re.M)
    for dp, _, fn in os.walk(SRC):
        if Path(dp).name == "Lane2":
            continue            # generated TUs are candidates, not ports to call or to skip
        for f in fn:
            if f.endswith(".cpp"):
                s = Path(dp, f).read_text(encoding="utf-8", errors="replace")
                for mm in rx.finditer(s):
                    m.setdefault(mm.group(2).lower().zfill(8), mm.group(1))
    return m


def strip_header_comment(c):
    # Ghidra emits `/* ... */` plate comments and WARNING lines before the prototype line
    lines = c.splitlines()
    out, depth = [], 0
    for ln in lines:
        if depth == 0 and ln.strip().startswith("/*") and "*/" in ln:
            continue
        if ln.strip().startswith("/*"):
            depth += 1
        if depth:
            if "*/" in ln:
                depth -= 1
            continue
        out.append(ln)
    return "\n".join(out).strip("\n")


def body_of(c):
    """Return (prototype_line, body_text_between_braces)."""
    i = c.find("{")
    j = c.rfind("}")
    if i < 0 or j < 0:
        return None, None
    return c[:i].strip(), c[i + 1:j]


# INDIRECT-CALL IDIOM TABLE. An indirect call is refused unless its base is a known global
# function-pointer table whose calling convention is established by an existing verified hand
# port. Entry: global name -> (calling convention, source of the ruling). Add an entry only with
# a citation; a wrong convention here becomes a crash in every generated port that uses it.
INDIRECT_IDIOMS = {
    # RW device slot: `(**(code **)(DAT_007d3ff8 + 0x20))(8, 0)` etc. Hand ports call it
    # __cdecl (Frontend/MenuDrawLoopTwin.cpp `vt20`, D3d9Render/RwIm2DBridge.cpp kRwDeviceSlot).
    "DAT_007d3ff8": ("__cdecl", "MenuDrawLoopTwin.cpp vt20 / RwIm2DBridge.cpp"),
}
IDIOM_CALL = re.compile(r"\(\*\*\s*\(code\s*\*\*\)\s*\(\s*(DAT_[0-9a-fA-F]{8})\s*\+\s*(0x[0-9a-fA-F]+)\s*\)\s*\)\s*\(")


def rewrite_idioms(body, log):
    """Replace known table-slot indirect calls with L2_slot_<global>_<off>_<arity>(args).
    Returns (body, helpers) or (None, reason) when an idiom has inconsistent arity."""
    helpers = {}
    out, pos = [], 0
    while True:
        m = IDIOM_CALL.search(body, pos)
        if not m:
            break
        gname, off = m.group(1), m.group(2)
        if gname not in INDIRECT_IDIOMS:
            pos = m.end()
            continue
        # paren-match the argument list starting at m.end()-1 ('(')
        k = m.end() - 1
        depth, q = 0, k
        while q < len(body):
            if body[q] == "(":
                depth += 1
            elif body[q] == ")":
                depth -= 1
                if depth == 0:
                    break
            q += 1
        if depth != 0:
            return None, "IDIOM:unbalanced call"
        argtxt = body[k + 1:q]
        parts, d, cur = [], 0, []
        for ch in argtxt:
            if ch in "([":
                d += 1
            elif ch in ")]":
                d -= 1
            if ch == "," and d == 0:
                parts.append("".join(cur).strip()); cur = []
            else:
                cur.append(ch)
        tail = "".join(cur).strip()
        if tail:
            parts.append(tail)
        cc, why = INDIRECT_IDIOMS[gname]
        name = f"L2_slot_{gname[4:].lower()}_{off}_{len(parts)}"
        helpers[name] = (gname, off, len(parts), cc, why)
        cast = ", ".join(f"(unsigned int)({a})" for a in parts)
        out.append(body[pos:m.start()])
        out.append(f"{name}({cast})")
        pos = q + 1
    out.append(body[pos:])
    text = "".join(out)
    decls = []
    for name, (gname, off, n, cc, why) in sorted(helpers.items()):
        sig = ", ".join(f"unsigned int a{i}" for i in range(n)) or "void"
        args = ", ".join(f"a{i}" for i in range(n))
        types = ", ".join(["unsigned int"] * n)
        addr = "0x" + gname[4:].lower() + "u"
        decls.append(f"// idiom: (*({gname}+{off}))(...) is {cc} per {why}")
        decls.append(f"static inline unsigned int {name}({sig}) {{ return reinterpret_cast<unsigned int({cc}*)({types})>("
                     f"*reinterpret_cast<unsigned int*>(*reinterpret_cast<unsigned int*>({addr}) + {off}))({args}); }}")
        log.append(f"IDIOM {gname}+{off} arity={n} {cc}")
    return text, decls


def transcribe(fn, ports, log):
    name = fn["name"]
    rva = fn["entry"][2:].lower().zfill(8)
    c = fn.get("decomp")
    proto = fn.get("proto")
    if not c or not proto:
        return None, "DECOMP_MISSING"
    c = strip_header_comment(c)
    _, body = body_of(c)
    if body is None:
        return None, "DECOMP_MISSING"
    if proto.get("varargs"):
        return None, "VARARGS"
    if PSEUDO.search(body):
        return None, "PSEUDO_OP:" + PSEUDO.search(body).group(1)
    if REGVAR.search(body):
        return None, "REGISTER_ABI:" + REGVAR.search(body).group(1)
    for p in proto["params"]:
        if not p["storage"].startswith("Stack"):
            return None, "REGISTER_ABI:param " + p["name"] + " in " + p["storage"]
    body, idiom_decls = rewrite_idioms(body, log)
    if body is None:
        return None, idiom_decls
    if INDIRECT.search(body):
        return None, "INDIRECT_CALL"
    # HIDDEN REGISTER ARGUMENT: a local that is advanced in a loop but never passed to the
    # no-arg call inside that loop (`puVar1 = &DAT_x; do { FUN_y(); puVar1 += 0x208; } while`)
    # means the callee reads it from a register Ghidra did not model. Calling the original
    # through a cdecl thunk leaves that register undefined -> crash at boot (0x004219c0,
    # Lane 3 first run 2026-09-10). Refuse when a zero-arg FUN_ call shares a loop body with a
    # pointer/int local that is only ever assigned, never read as an argument or operand.
    for m0 in re.finditer(r"FUN_[0-9a-fA-F]{8}\s*\(\s*\)", body):
        # crude but honest: any local named like Ghidra's pointer/int temporaries that appears
        # on the LHS of `+=`/`= x + n` and nowhere else as an rvalue
        for lv in set(re.findall(r"((?:p[a-z]*Var|iVar|uVar)\d+)\s*=\s*\s*\+", body)):
            body_nodecl = re.sub(r"^[ 	]*[A-Za-z_][\w \*]*%s;[ 	]*$" % lv, "", body, flags=re.M)
            reads = len(re.findall(r"%s" % lv, body_nodecl))
            writes = len(re.findall(r"%s\s*=" % lv, body))
            conds = len(re.findall(r"%s[^;]*[<>]" % lv, body)) + len(re.findall(r"[<>][^;]*%s" % lv, body))
            if reads - writes - conds <= 1:      # the only other read is its own increment
                return None, "HIDDEN_REG_ARG:" + lv + " advanced but never passed to " + m0.group(0).split("(")[0]
    ret = ctype(proto["ret"])
    if ret is None:
        return None, "UNKNOWN_TYPE:ret " + proto["ret"]
    if proto["ret"] == "float10":
        return None, "PSEUDO_OP:float10-return"
    params = []
    for p in proto["params"]:
        t = ctype(p["type"])
        if t is None:
            return None, "UNKNOWN_TYPE:param " + p["type"]
        params.append((t, p["name"]))

    # --- callees ------------------------------------------------------------------
    # Signature source, in order: the callee's DECOMPILER prototype; if the call sites in
    # this body pass a different number of arguments, the CALL SITE wins (arity N, every
    # param `undefined4`) and the call goes through a raw-RVA thunk even when a port
    # exists -- we never change a port's signature to fit a caller. Every argument is
    # C-cast to the declared parameter type: Ghidra's C is loosely typed (int<->pointer
    # implicit), MSVC is not, and a C cast preserves the bit pattern verbatim.
    callees = {cp["name"]: cp for cp in fn.get("callee_protos", [])}
    used = sorted(set(CALLNAME.findall(body)))
    def find_calls(text):
        """Yield (name, start, end, argtxt) for every FUN_xxxxxxxx(...) call, paren-matched."""
        for m in re.finditer(r"\bFUN_[0-9a-fA-F]{8}\b", text):
            k = m.end()
            while k < len(text) and text[k] in " \t":
                k += 1
            if k >= len(text) or text[k] != "(":
                continue
            depth, q = 0, k
            while q < len(text):
                if text[q] == "(":
                    depth += 1
                elif text[q] == ")":
                    depth -= 1
                    if depth == 0:
                        break
                q += 1
            if depth != 0:
                continue
            yield m.group(0), m.start(), q + 1, text[k + 1:q]

    def split_args(argtxt):
        parts, depth, cur = [], 0, []
        for ch in argtxt:
            if ch in "([":
                depth += 1
            elif ch in ")]":
                depth -= 1
            if ch == "," and depth == 0:
                parts.append("".join(cur).strip())
                cur = []
            else:
                cur.append(ch)
        tail = "".join(cur).strip()
        if tail:
            parts.append(tail)
        return parts

    site_arity = {}
    for cn, _, _, argtxt in find_calls(body):
        site_arity.setdefault(cn, set()).add(len(split_args(argtxt)))

    decls, sigs = [], {}          # cn -> (ret, [types], target_symbol)
    for cn in used:
        cp = callees.get(cn)
        crva = (cp["entry"][2:].lower().zfill(8)) if cp else cn[4:].lower()
        dp = (cp or {}).get("dproto")
        arities = site_arity.get(cn, set())
        if len(arities) != 1:
            return None, "CALLEE_PROTO:inconsistent call-site arity " + cn
        n_site = arities.pop()
        cret, ctypes_ = None, None
        if dp and not dp.get("varargs"):
            ok = True
            tl = []
            for pp in dp["params"]:
                if not pp["storage"].startswith("Stack"):
                    return None, "CALLEE_PROTO:register-abi " + cn
                t = ctype(pp["type"])
                if t is None:
                    ok = False
                    break
                tl.append(t)
            r = ctype(dp["ret"])
            if dp["ret"] == "float10":
                r = "long double"
            if ok and r is not None and len(tl) == n_site:
                cret, ctypes_ = r, tl
            elif ok and r is not None and len(tl) < n_site:
                # The callee's own decompilation saw FEWER stack parameters than this call site
                # passes: the extra argument travels in a register Ghidra did not model. A cdecl
                # thunk leaves that register undefined. Both crashes of the first Lane 3 run
                # (eip 0x005bbf59, 0x00421980 -> FUN_0055dec0/FUN_00559ee0) were this shape.
                return None, f"CALLEE_REG_ARG:{cn} proto={len(tl)} site={n_site}"
            elif ok and r is not None:
                log.append(f"CALLEE_ARITY {cn} proto={len(tl)} site={n_site} -> raw thunk")
        if ctypes_ is None:
            if not dp:
                log.append(f"CALLEE_NO_DPROTO {cn} -> raw thunk arity {n_site}, ret undefined4")
            cret, ctypes_ = "unsigned int", ["unsigned int"] * n_site
            target = None
        else:
            target = ports.get(crva)
        if target and cret is not None:
            sigs[cn] = (cret, ctypes_, target)
            decls.append(f'extern "C" {cret} __cdecl {target}({", ".join(ctypes_) or "void"});   // 0x{crva} (ported)')
        else:
            sig = ", ".join(f"{t} a{i}" for i, t in enumerate(ctypes_)) or "void"
            args = ", ".join(f"a{i}" for i in range(len(ctypes_)))
            sigs[cn] = (cret, ctypes_, "L2T_" + cn)
            decls.append(f"static inline {cret} L2T_{cn}({sig}) {{ "
                         f"{'return ' if cret != 'void' else ''}"
                         f"reinterpret_cast<{cret}(__cdecl*)({', '.join(ctypes_)})>(0x{crva}u)({args}); }}"
                         f"   // 0x{crva} (original, not ported)")

    def rewrite(text):
        """Rewrite every FUN_ call (innermost first) to its target with C-cast arguments."""
        out, pos = [], 0
        calls = list(find_calls(text))
        # only top-level calls here; nested ones are handled by recursing into argtxt
        top = []
        last_end = -1
        for c in calls:
            if c[1] >= last_end:
                top.append(c)
                last_end = c[2]
        for cn, st, en, argtxt in top:
            out.append(text[pos:st])
            cret, tl, target = sigs[cn]
            parts = [rewrite(a) for a in split_args(argtxt)]
            cast = []
            for i, a in enumerate(parts):
                t = tl[i] if i < len(tl) else "unsigned int"
                if t in ("float", "double") and HEXLIT_FLOAT.match(a):
                    log.append(f"FLOAT_LITERAL_AS_INT {cn} arg{i} {a}")
                    cast.append(f"f32bits({a}u)")
                else:
                    cast.append(f"({t})({a})")
            out.append(f"{target}({', '.join(cast)})")
            pos = en
        out.append(text[pos:])
        return "".join(out)
    body = rewrite(body)

    # --- globals ------------------------------------------------------------------
    gdefs = []
    gmap = {g["name"]: g for g in fn.get("globals", [])}
    # Ghidra prints `_DAT_x` when a smaller symbol overlaps at the same address (its own
    # WARNING line says so); the symbol map still carries it under `DAT_x`. Match by address.
    by_addr = {g["addr"][2:].lower(): g for g in fn.get("globals", []) if g.get("addr")}
    for gname in sorted(set(GLOBAL.findall(body))):
        g = gmap.get(gname)
        if not g:
            m = re.search(r"([0-9a-fA-F]{8})$", gname)
            if m:
                g = by_addr.get(m.group(1).lower())
                if g and not g.get("writable") and g["type"] == "undefined" and gname.startswith("_"):
                    g = dict(g, type="undefined4")   # `_DAT_` of an undefined byte read as a dword
        if not g or not g.get("addr"):
            return None, "GLOBAL_UNTYPED:" + gname
        t = ctype(g["type"])
        if t is None:
            return None, "UNKNOWN_TYPE:global " + gname + " " + g["type"]
        cv = "" if g.get("writable") else "const "
        gdefs.append(f"#define {gname} (*({cv}{t}*){g['addr']}u)   // {g['block']}, {g['type']}")

    # --- assemble ---------------------------------------------------------------
    sig = ", ".join(f"{t} {n}" for t, n in params) or "void"
    argnames = ", ".join(n for _, n in params)
    out = []
    out.append(f"// ---------------------------------------------------------------------------")
    out.append(f"// 0x{rva}  {name}  -- GENERATED by re/tools/decomp2port.py from the Ghidra")
    out.append(f"// decompilation (verbatim; no semantic edits). Size {fn.get('size')} B. Plate:")
    out.append(f"// hooks.csv row 0x{rva}. C-level unchanged by generation; see file header.")
    out.append(f"// ---------------------------------------------------------------------------")
    out += gdefs
    out += idiom_decls
    out += decls
    # The hook symbol carries the L2_ prefix: HookSystem installs L2_* hooks ONLY when
    # MASHED_HOOK_ONLY names them (or MASHED_HOOK_LANE2=1), so an unverified generated
    # port never rides along in a default .asi run. Callees keep their own names.
    hook = f"L2_{name}"
    impl = f"{hook}_impl"
    out.append(f'extern "C" {ret} __cdecl {hook}({sig});')
    out.append(f"static {ret} __cdecl {impl}({sig})")
    out.append("{" + body.rstrip() + "\n}")
    # Lane 3: every generated port (void or not) is verified by page-level write tracking
    # (Core/ShadowTrack.h); the return is compared too when there is one.
    out.append(f'extern "C" {ret} __cdecl {hook}({sig}) {{')
    out.append(f'    SHADOW_AB_COUNTER(ab, "{hook}", 0x{rva}u, ShadowAB::kPhaseRace);')
    out.append(f"    {'return ' if ret != 'void' else ''}ShadowAB::RunTracked(ab, {impl}, SHADOW_STACK_WINDOW()"
               + (f", {argnames}" if argnames else "") + ");")
    out.append("}")
    out.append(f"RH_ScopedInstall({hook}, 0x{rva});")
    for gname in sorted(set(GLOBAL.findall(body))):
        out.append(f"#undef {gname}")
    out.append("")
    kind = "ret" if ret != "void" else "void(tracked)"
    return "\n".join(out), kind


HEADER = '''// ============================================================================
//  {fname} -- Lane 2 GENERATED verbatim ports (re/tools/decomp2port.py, {date})
//  Every function below is a mechanical transcription of Ghidra's decompilation of
//  MASHED.exe (anchor in CLAUDE.md). Nothing here is understood or verified by
//  generation: a row moves to C3 only through the shadow A/B (return-value functions
//  carry a ShadowAB::Run wrapper) plus the caller/callee gate in re-classify.
//  Globals are read through their absolute addresses (.asi target only; NOT in
//  exe_sources.rsp). Unported callees are raw-RVA thunks.
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>

namespace {{
inline float f32bits(unsigned int u) {{ float f; std::memcpy(&f, &u, 4); return f; }}
}}
typedef unsigned char  undefined;
typedef unsigned char  undefined1;
typedef unsigned short undefined2;
typedef unsigned int   undefined4;
typedef unsigned char  byte;
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef long double    float10;
typedef void           code;

'''


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("json", nargs="?", default="", help="decomp_pc.py --port output")
    ap.add_argument("--emit", default="", help="TU path relative to mashedmod/src/mashed_re (e.g. Lane2/Batch_x.cpp)")
    ap.add_argument("--apply", action="store_true", help="write the TU and append it to asi_sources.rsp")
    ap.add_argument("--emit-dir", default="", help="directory relative to mashedmod/src/mashed_re: ONE TU per function (L2_<rva>.cpp)")
    ap.add_argument("--prune-failed", default="", help="build log: drop every L2_*.cpp TU that errored from asi_sources.rsp (+ delete it), report COMPILE_FAIL")
    ap.add_argument("--report", default="", help="TSV of per-row verdicts")
    args = ap.parse_args()

    if args.prune_failed:
        log_txt = Path(args.prune_failed).read_text(encoding="utf-8", errors="replace")
        failed = sorted(set(re.findall(r"(L2_[0-9a-f]{8})\.cpp\(\d+\): error", log_txt)))
        rsp = RSP_ASI.read_text(encoding="utf-8")
        for f in failed:
            rsp = re.sub(r'^"[^"\n]*%s\.cpp"\r?\n' % f, "", rsp, flags=re.M)
            for pth in SRC.rglob(f + ".cpp"):
                pth.unlink()
        RSP_ASI.write_text(rsp, encoding="utf-8")
        print(f"pruned {len(failed)} TU(s) with compile errors: " + " ".join(failed))
        if args.report and Path(args.report).exists():
            rows_txt = Path(args.report).read_text(encoding="utf-8").splitlines()
            out = []
            for ln in rows_txt:
                t = ln.split("\t")
                if len(t) >= 3 and ("L2_" + t[0]) in failed and t[2].startswith("PORT"):
                    pat = r"L2_%s\.cpp\(\d+\): (error C\d+: [^\n]{0,80})" % t[0]
                    errs = "; ".join(sorted(set(re.findall(pat, log_txt)))[:2])
                    t[2] = "COMPILE_FAIL"
                    if len(t) == 3:
                        t.append(errs)
                    else:
                        t[3] = errs
                out.append("\t".join(t))
            Path(args.report).write_text("\n".join(out) + "\n", encoding="utf-8")
        return 0
    d = json.load(open(args.json, encoding="utf-8"))
    ports = existing_ports()
    rows, chunks = [], []
    import collections, datetime
    for fn in d["functions"]:
        if "error" in fn:
            rows.append((fn.get("requested", ""), fn.get("name", ""), "DECOMP_MISSING", ""))
            continue
        log = []
        rva = fn["entry"][2:].lower().zfill(8)
        if rva in ports:
            rows.append((rva, fn["name"], "SKIP:already-ported " + ports[rva], ""))
            continue
        text, kind = transcribe(fn, ports, log)
        if text is None:
            rows.append((rva, fn["name"], "REFUSE:" + kind, ""))
        else:
            rows.append((rva, fn["name"], "PORT:" + kind, "; ".join(log)))
            chunks.append(text)
    tally = collections.Counter(r[2].split(":")[0] + (":" + r[2].split(":")[1] if r[2].startswith("REFUSE") or r[2].startswith("PORT") else "") for r in rows)
    print(f"rows={len(rows)}  " + "  ".join(f"{k}={v}" for k, v in sorted(tally.items())))
    for r in rows:
        print(f"  {r[0]}  {r[1]:<22} {r[2]:<40} {r[3]}")
    if args.report:
        with open(args.report, "w", encoding="utf-8") as f:
            f.write("rva\tname\tverdict\tnotes\n")
            for r in rows:
                f.write("\t".join(r) + "\n")
    if args.emit_dir and chunks:
        # one TU per function so a compile failure costs one row, not the batch
        import datetime as _dt
        outdir = SRC / args.emit_dir
        rsp = RSP_ASI.read_text(encoding="utf-8")
        added = 0
        port_rows = [x for x in rows if x[2].startswith("PORT")]
        for r, text in zip(port_rows, chunks):
            fname = f"L2_{r[0]}.cpp"
            if args.apply:
                outdir.mkdir(parents=True, exist_ok=True)
                (outdir / fname).write_text(
                    HEADER.format(fname=fname, date=_dt.date.today().isoformat()) + text,
                    encoding="utf-8")
                rel = args.emit_dir.rstrip("/\\") + "\\" + fname
                if ('"' + rel + '"') not in rsp:
                    rsp = rsp.rstrip("\n") + "\n" + '"' + rel + '"' + "\n"
                    added += 1
        if args.apply:
            RSP_ASI.write_text(rsp, encoding="utf-8")
            print(f"wrote {len(chunks)} TUs under {args.emit_dir}; {added} registered in asi_sources.rsp")
        else:
            print(f"--emit-dir without --apply: would write {len(chunks)} TUs under {args.emit_dir}")
    if args.emit and chunks:
        fname = Path(args.emit).name
        tu = HEADER.format(fname=fname, date=datetime.date.today().isoformat()) + "\n".join(chunks)
        if args.apply:
            path = SRC / args.emit
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(tu, encoding="utf-8")
            rel = args.emit.replace("/", "\\")
            rsp = RSP_ASI.read_text(encoding="utf-8")
            if f'"{rel}"' not in rsp:
                RSP_ASI.write_text(rsp.rstrip("\n") + f'\n"{rel}"\n', encoding="utf-8")
            print(f"wrote {path.relative_to(ROOT)} ({len(chunks)} functions); registered in asi_sources.rsp")
        else:
            print(f"--emit without --apply: would write {len(chunks)} functions to {args.emit}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
