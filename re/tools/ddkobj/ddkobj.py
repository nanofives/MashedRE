"""DDKOBJ variant of mashed_re.exe (DDKOBJ-SPIKE, 2026-11-11).

Links the XP DDK objects of byte-matched sources (re/match/**/<rva>.c, the certified bytes) into a second exe in place
of the VS2022 compile of the same text. The default build is not touched: this tool reads the default build's
objects (mashedmod/build/obj/exe, link.rsp) and writes everything under its own --out directory.

    py -3.12 re/tools/ddkobj/ddkobj.py build [--units 0x00492370,0x00443300 | --units all | --units-file f]
                                             [--out mashedmod/build/ddkobj] [--link] [--keep-going]
    py -3.12 re/tools/ddkobj/ddkobj.py check  [--out ...]     # post-link byte check vs MASHED.exe.unpatched

How one unit (one re/match source F, included by the exe wrapper W) is substituted:
  1. F is compiled by the DDK exactly as match.py does (cl 13.00.9176, /c /Gy + F's // FLAGS:, clean env, as C).
  2. Symbol rewrite in that DDK object (coffedit.py):
     - the MATCH function's definition takes the exe name W gives it (`#define FUN_x Orig_x` -> _Orig_x, decoration
       kept); other external definitions in F get a `__ddk<rva>` suffix so they cannot collide with W's copies;
     - each undefined name that W #defines to an exe symbol is renamed to it (the wrapper's own binding, verbatim);
     - each MGLOBAL / MGLOBALA name becomes `__ddkabs_<addr>`, an absolute symbol (ddk_abs.obj) at the original
       address, which the exe maps through g_b17_low_arena_pad, exactly as the VS2022 wrapper's (*(T*)addr) does;
     - everything else (CRT helpers, /GS cookie, __real@ literals, names W does not rename) is left alone.
  3. W's VS2022 object is copied and edited: its definition of the exe name is renamed `<name>__vs` (dead code that
     /OPT:REF drops) and every relocation in W that pointed at it is repointed to a new undefined symbol of the exe
     name, so W's own callers reach the DDK body as well. Static wrapper targets cannot be reached from another
     object: they are promoted to externals under a unique name when W still emits them (never inlined away).
  4. The variant link response = the default link.rsp with W's object replaced by the edited copy, plus the DDK
     objects and ddk_abs.obj. The link line is the default one plus /NODEFAULTLIB:libc.lib (DDK objects carry
     /DEFAULTLIB:LIBC) and the variant output names.
"""
from __future__ import annotations

import argparse
import collections
import csv
import hashlib
import json
import os
import pathlib
import re
import shutil
import struct
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent / "match"))
import coffedit as CE  # noqa: E402
import match as M      # noqa: E402

ROOT = HERE.parents[2]
MOD = ROOT / "mashedmod"
SRC = MOD / "src" / "mashed_re"
BUILD = MOD / "build"
DEF_OBJ = BUILD / "obj" / "exe"
BINDINGS = ROOT / "verify" / "porttop_20261108" / "BINDINGS.tsv"
VCVARS = r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"

INC_RE = re.compile(r'^\s*#\s*include\s+"([./\\]*re[/\\]match[/\\][^"]+\.c)"', re.M)
DEF_RE = re.compile(r"^\s*#\s*define\s+([A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*(?://.*)?$")
DEFABS_RE = re.compile(r"^\s*#\s*define\s+([A-Za-z_]\w*)\s+\(.*0x(00[4-9a-fA-F][0-9a-fA-F]{5})u?.*\)\s*(?://.*)?$")
UNDEF_RE = re.compile(r"^\s*#\s*undef\s+([A-Za-z_]\w*)")
MATCH_RE = re.compile(r"^\s*//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x([0-9a-fA-F]+)\s+(\S+)", re.M)
FLAGS_RE = re.compile(r"^\s*//\s*FLAGS:\s*(.+?)\s*$", re.M)
LANG_RE = re.compile(r"^\s*//\s*LANG\s+(\S+)", re.M)
TC_RE = re.compile(r"^\s*//\s*TOOLCHAIN\s+(\S+)", re.M)
MGLOBAL_RE = re.compile(r"^\s*MGLOBAL\s*\(\s*(.+?)\s*,\s*([A-Za-z_]\w*)\s*,\s*(0x[0-9a-fA-F]+)\s*\)", re.M)
MGLOBALA_RE = re.compile(r"^\s*MGLOBALA\s*\(\s*(.+?)\s*,\s*([A-Za-z_]\w*)\s*,\s*[^,]+,\s*(0x[0-9a-fA-F]+)\s*\)", re.M)


# ------------------------------------------------------------------ inventory
_COND_RE = re.compile(r"^\s*#\s*(ifdef|ifndef|if|elif|else|endif|define|undef)\b\s*(.*)$")


def _eval_if(expr: str, defined: set) -> bool:
    e = re.sub(r"/\*.*?\*/|//.*$", "", expr).strip()
    e = re.sub(r"defined\s*\(\s*(\w+)\s*\)|defined\s+(\w+)",
               lambda m: "1" if (m.group(1) or m.group(2)) in defined else "0", e)
    e = e.replace("&&", " and ").replace("||", " or ").replace("!", " not ")
    e = re.sub(r"\b[A-Za-z_]\w*\b", lambda m: m.group(0) if m.group(0) in ("and", "or", "not") else
               ("1" if m.group(0) in defined else "0"), e)
    try:
        return bool(eval(e, {"__builtins__": {}}))
    except Exception:
        return True


def active_lines(txt: str, defined: set):
    """The lines of txt that a C preprocessor keeps, given the macro names in `defined` (a #define in the text adds to
    it). Only #if/#ifdef/#ifndef/#elif/#else/#endif and defined()/name truth; good enough for wrappers and sources."""
    defined = set(defined)
    stack = []   # (this branch active, some branch already taken, parent active)
    active = True
    for line in txt.splitlines():
        m = _COND_RE.match(line)
        if m:
            kw, rest = m.group(1), m.group(2)
            if kw in ("ifdef", "ifndef", "if"):
                if kw == "ifdef":
                    c = rest.split()[0] in defined if rest.split() else False
                elif kw == "ifndef":
                    c = rest.split()[0] not in defined if rest.split() else True
                else:
                    c = _eval_if(rest, defined)
                stack.append((active and c, c, active))
                active = active and c
                continue
            if kw == "elif" and stack:
                a, taken, parent = stack.pop()
                c = (not taken) and _eval_if(rest, defined)
                stack.append((parent and c, taken or c, parent))
                active = parent and c
                continue
            if kw == "else" and stack:
                a, taken, parent = stack.pop()
                stack.append((parent and not taken, True, parent))
                active = parent and not taken
                continue
            if kw == "endif" and stack:
                _a, _t, parent = stack.pop()
                active = parent
                continue
            if active and kw == "define" and rest.split():
                defined.add(re.match(r"\w+", rest).group(0))
            if active and kw == "undef" and rest.split():
                defined.discard(rest.split()[0])
        if active:
            yield line


def exe_wrappers():
    """[(wrapper path, [(source path, defines-in-effect dict)])] for every exe TU that includes a re/match source."""
    rsp = [l.strip().strip('"') for l in (MOD / "exe_sources.rsp").read_text().splitlines() if l.strip()]
    out = []
    for rel in rsp:
        p = SRC / rel
        if not p.exists() or p.suffix.lower() not in (".cpp", ".c"):
            continue
        txt = p.read_text(encoding="utf-8", errors="replace")
        if "re/match/" not in txt and "re\\match\\" not in txt:
            continue
        defs, incs, ptrabs = {}, [], {}
        for line in active_lines(txt, {"MASHED_STANDALONE", "__cplusplus", "_MSC_VER", "_WIN32"}):
            if re.match(r"^\s*#\s*define\s+MGLOBALA?\s*\(", line):
                # the wrapper binds the source's globals to its own cells (MATCH_CELL_*, exe_main.cpp:3575), not to
                # the original addresses: a DDK object cannot follow a runtime pointer, so its units are refused
                defs["@mglobal"] = line.strip()
                continue
            m = DEF_RE.match(line)
            if m:
                defs[m.group(1)] = m.group(2)
                continue
            m = DEFQ_RE.match(line)
            if m:   # `#define FUN_x ::Ns::Name`: a C++ function, resolved through the default build's map
                defs[m.group(1)] = "@cxx:" + m.group(2).lstrip(":")
                continue
            m = DEFFN_RE.match(line)
            if m:   # `#define FUN_x(a, b) Target((a), (T)(b))`: same arguments in the same order -> Target
                tgt = fnlike_target(m.group(2), m.group(3))
                if tgt:
                    defs[m.group(1)] = tgt
                    continue
            m = DEFABS_RE.match(line)
            if m:   # `#define FUN_x ((T)0x00xxxxxxu)`: the port calls the original address (arena) directly
                defs[m.group(1)] = "@abs:" + m.group(2)
                continue
            m = PTRABS_RE.match(line)
            if m:   # `static auto* const p = reinterpret_cast<T>(0x00xxxxxx);` then `#define FUN_x p`
                ptrabs[m.group(1)] = m.group(2)
                continue
            m = UNDEF_RE.match(line)
            if m:
                defs.pop(m.group(1), None)
                continue
            m = INC_RE.match(line)
            if m:
                snap = {k: ("@abs:" + ptrabs[v] if v in ptrabs else v) for k, v in defs.items()}
                incs.append(((p.parent / m.group(1)).resolve(), snap))
        if incs:
            out.append((p, rel, incs))
    return out


DEFQ_RE = re.compile(r"^\s*#\s*define\s+([A-Za-z_]\w*)\s+((?:::)?[A-Za-z_]\w*(?:::[A-Za-z_]\w*)+)\s*(?://.*)?$")
DEFFN_RE = re.compile(r"^\s*#\s*define\s+([A-Za-z_]\w*)\(([^)]*)\)\s+(.+?)\s*(?://.*)?$")
PTRABS_RE = re.compile(r"^\s*static\s+.*\bconst\s+([A-Za-z_]\w*)\s*=\s*reinterpret_cast<.*>\s*\(\s*0x(00[4-9a-fA-F][0-9a-fA-F]{5})u?\s*\)\s*;")
_TYPEWORDS = {"void", "int", "unsigned", "signed", "char", "short", "long", "float", "double", "const", "std",
              "uint8_t", "uint16_t", "uint32_t", "int8_t", "int16_t", "int32_t", "uintptr_t", "reinterpret_cast",
              "static_cast", "bool", "__cdecl", "__stdcall", "__fastcall"}


_WSTATIC_RE = re.compile(r"^static[^=;(]*?[\s*&]([A-Za-z_]\w*)\s*\(")


def wrapper_statics(wpath: pathlib.Path):
    """Names of static functions the wrapper defines (exe view): DDK objects may bind to them (promoted later)."""
    txt = wpath.read_text(encoding="utf-8", errors="replace")
    return {m.group(1) for line in active_lines(txt, {"MASHED_STANDALONE", "__cplusplus", "_MSC_VER", "_WIN32"})
            for m in [_WSTATIC_RE.match(line)] if m}


def fnlike_target(params: str, body: str):
    """`Target(args)` when the macro body calls one function with the macro's parameters in order (casts allowed)."""
    ps = [x.strip() for x in params.split(",") if x.strip()]
    m = None
    for mm in re.finditer(r"([A-Za-z_]\w*)\s*\(", body):
        if mm.group(1) not in _TYPEWORDS:
            m = mm
            break
    if m is None:
        return None
    args = body[m.end():]
    order = [t for t in re.findall(r"[A-Za-z_]\w*", args) if t in ps]
    if order != ps:
        return None
    return m.group(1)

def load_bindings():
    rows = {}
    with open(BINDINGS, newline="", encoding="utf-8") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            try:
                rows[int(r["rva"], 16)] = r
            except (ValueError, KeyError):
                pass
    return rows


def parse_source(path: pathlib.Path):
    t = path.read_text(encoding="utf-8", errors="replace")
    m = FLAGS_RE.search(t)
    flags = m.group(1) if m else M.DEFAULT_FLAGS
    lang = (LANG_RE.search(t).group(1) if LANG_RE.search(t) else "c")
    tc = (TC_RE.search(t).group(1) if TC_RE.search(t) else "ddk")
    matches = [(int(a, 16), n) for a, n in MATCH_RE.findall(t)]
    # the MATCH functions the port compiles: those outside `#ifndef MATCH_PORT` blocks (C++ + MATCH_PORT + exe)
    ported = {CE.undecorate(n) for _a, n in MATCH_RE.findall(chr(10).join(
        active_lines(t, {"MATCH_PORT", "__cplusplus", "MASHED_STANDALONE", "_MSC_VER", "_WIN32"})))}
    glob = {}
    for rx in (MGLOBAL_RE, MGLOBALA_RE):
        for _T, n, a in rx.findall(t):
            glob[n] = int(a, 16)
    cview = list(active_lines(t, {"_MSC_VER", "_WIN32"}))
    pview = list(active_lines(t, {"MATCH_PORT", "__cplusplus", "MASHED_STANDALONE", "_MSC_VER", "_WIN32"}))
    excluded = func_defs(cview) - func_defs(pview)
    # data the port defines itself with an initializer (0x0048ff20: `static char s_Tri_dff_005cf4f8[8] = "Tri.dff";`
    # under MATCH_PORT, MGLOBALA in C). The exe arena is zero there, so the DDK object reads a copy of the original bytes.
    port_copies = {}
    for line in pview:
        m = PORTCOPY_RE.match(line)
        if m and m.group(1) in glob and "0x" not in m.group(3):
            # size: the declared element count (bytes for char arrays, the only form present today), else one dword
            port_copies[m.group(1)] = int(m.group(2)) if m.group(2) else 4
    return {"text": t, "flags": flags, "lang": lang, "toolchain": tc, "matches": matches, "ported": ported,
            "globals": glob, "port_excluded": excluded, "port_copies": port_copies}


# `static T NAME[N] = <init>;` / `static T NAME = <init>;` at file scope: groups (N, NAME, init)
PORTCOPY_RE = re.compile(r"^static\s+(?:const\s+)?\w[\w\s\*]*?\b([A-Za-z_]\w*)\s*(?:\[\s*(\d*)\s*\])?\s*=\s*([^;]*);")
_FDEF_RE = re.compile(r"^(?!\s*(?:if|for|while|switch|return|else|do|case)\b)\s*[A-Za-z_][\w\s\*]*?\b([A-Za-z_]\w*)\s*\(")


def func_defs(lines):
    """Names of function definitions in preprocessed lines: `T name(...)` whose declarator is followed by `{`."""
    out = set()
    n = len(lines)
    for i, line in enumerate(lines):
        if line.startswith((" ", "\t", "#", "/")):
            continue
        m = _FDEF_RE.match(line)
        if not m:
            continue
        for j in range(i, min(i + 8, n)):
            s = re.sub(r"/\*.*?\*/|//.*$", "", lines[j]).strip()
            if s.endswith(";") or (j > i and s.endswith(";")):
                break
            if "{" in s or (j + 1 < n and lines[j + 1].strip().startswith("{") and s.endswith(")")):
                out.add(m.group(1))
                break
    return out


# ------------------------------------------------------------------ CRT symbol inventory
_CRT = None


def vs_crt_symbols():
    """Public symbols of the VS2022 static CRT the exe links (libcmt + libvcruntime + libucrt + oldnames)."""
    global _CRT
    if _CRT is not None:
        return _CRT
    cache = BUILD / "ddkobj" / "vs_crt_symbols.txt"
    if cache.exists():
        _CRT = set(cache.read_text().split())
        return _CRT
    vc = pathlib.Path(r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC")
    msvc = sorted(vc.iterdir())[-1]
    sdk = sorted(pathlib.Path(r"C:\Program Files (x86)\Windows Kits\10\Lib").iterdir())[-1]
    libs = [msvc / "lib" / "x86" / n for n in ("libcmt.lib", "libvcruntime.lib", "oldnames.lib")] + \
           [sdk / "ucrt" / "x86" / "libucrt.lib", sdk / "um" / "x86" / "kernel32.lib"]
    syms = set()
    for lib in libs:
        syms |= ar_symbols(lib)
    cache.parent.mkdir(parents=True, exist_ok=True)
    cache.write_text("\n".join(sorted(syms)))
    _CRT = syms
    return syms


def default_link_symbols():
    """Externals defined by the default exe link's objects (link.rsp + the isolated bridge objects) and its libs."""
    syms = set()
    objs = [l.strip().strip('"') for l in (DEF_OBJ / "link.rsp").read_text().splitlines() if l.strip()]
    objs += [str(BUILD / n) for n in ("QhullBridge_exe.obj", "RwBridge_exe.obj", "RwRasterBridge_exe.obj",
                                      "RwSceneBuild_exe.obj", "RwRaceSubmit_exe.obj", "RwRaceFx_exe.obj",
                                      "RwApiBind_exe.obj")]
    for o in objs:
        try:
            co = CE.Coff.load(o)
        except Exception:
            continue
        for s in co.syms:
            if s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL and s.sec != 0:
                syms.add(s.name)
    sdk = sorted(pathlib.Path(r"C:\Program Files (x86)\Windows Kits\10\Lib").iterdir())[-1] / "um" / "x86"
    imps = [sdk / (n + ".lib") for n in ("user32", "gdi32", "d3d9", "advapi32", "ole32", "oleaut32", "uuid",
                                         "winmm", "shell32", "dinput8", "dsound", "comdlg32", "winspool")]
    for lib in [MOD / "deps" / "qhull-2002.1" / "qhull_2002_1.lib", MOD / "deps" / "librw" / "librw_d3d9.lib"] + imps:
        if lib.exists():
            syms |= ar_symbols(lib)
    return syms


def ar_symbols(lib: pathlib.Path):
    """Names in the first linker member of an archive (big-endian count + offsets + names)."""
    d = lib.read_bytes()
    assert d[:8] == b"!<arch>\n", lib
    o = 8
    name = d[o:o + 16]
    size = int(d[o + 48:o + 58].strip())
    body = d[o + 60:o + 60 + size]
    n = struct.unpack_from(">I", body, 0)[0]
    names = body[4 + 4 * n:].split(b"\0")[:n]
    return {x.decode("latin-1") for x in names}


# ------------------------------------------------------------------ build
def ddk_compile(src: pathlib.Path, info, objdir: pathlib.Path, wstem: str):
    r = M.compile_obj(src, info["flags"], info["lang"], info["toolchain"], errors_ok=True)
    obj, log = r
    if obj is None:
        return None, log
    dst = objdir / ("ddk_" + src.parent.name + "_" + src.stem + "_" + wstem + ".obj")
    shutil.copyfile(obj, dst)
    return dst, log


def main_build(a):
    out = pathlib.Path(a.out).resolve()
    objdir = out / "obj"
    if objdir.exists():
        shutil.rmtree(objdir)
    objdir.mkdir(parents=True)
    B = load_bindings()
    crt = vs_crt_symbols()
    # every external the default link can resolve: defined in a default object, or in a library on the link line
    defmap = default_link_symbols() | crt
    abs_syms = {}
    abs_calls = {}
    data_copies = {}

    def bind_to(s, target, label):
        """Rename undefined symbol s to what a wrapper binds its name to; returns the class label."""
        if target.startswith("@abs:"):
            fa = int(target[5:], 16)
            nm = f"__ddkabsf_{fa:08x}"
            abs_calls[nm] = fa     # a `push imm32 / ret` thunk (ddk_thunks.obj): REL32 cannot target an absolute
            s.name = nm
            return "abs-call"
        if target.startswith("@cxx:"):
            parts = target[5:].split("::")
            pref = "?" + "@".join(reversed(parts)) + "@@"
            hits = [n for n in defmap if n.startswith(pref)]
            if len(hits) == 1:
                s.name = hits[0]
                return "cxx-bound"
            return "cxx-unresolved"
        s.name = CE.redecorate(s.name, target)
        return "in-set" if target in exe_names else label
    wr = exe_wrappers()

    # candidate units: (source, wrapper, defines)
    cand = []
    for wpath, wrel, incs in wr:
        for src, defs in incs:
            cand.append((src, wpath, wrel, defs))
    want = None
    if a.units and a.units != "all":
        want = {int(x, 16) for x in a.units.replace(";", ",").split(",") if x.strip()}
    if a.units_file:
        want = {int(l.split()[0], 16) for l in open(a.units_file) if l.strip().startswith("0x")}

    units = []
    seen_src = set()
    for src, wpath, wrel, defs in cand:
        info = parse_source(src)
        rvas = [r for r, _ in info["matches"]]
        if not rvas:
            continue
        if want is not None and not (set(rvas) & want):
            continue
        if a.matched_only and not any(B.get(r, {}).get("kind") == "matched" for r in rvas):
            continue
        key = (str(src), str(wpath))
        if key in seen_src:
            continue
        seen_src.add(key)
        units.append({"src": src, "wrapper": wpath, "wrel": wrel, "defs": defs, "info": info, "rvas": rvas,
                      "wstatics": wrapper_statics(wpath)})

    report = {"units": [], "wrappers": {}}
    # global binding table: a name every exe wrapper that defines it binds to the same target
    gseen = collections.defaultdict(set)
    for _w, _r, incs in wr:
        for _s, d in incs:
            for k, v in d.items():
                if k.startswith(("FUN_", "DAT_", "thunk_")):
                    gseen[k].add(v)
    gdefs = {k: next(iter(v)) for k, v in gseen.items() if len(v) == 1}
    report["global_bindings"] = {"consistent": len(gdefs), "conflicting": sum(1 for v in gseen.values() if len(v) > 1)}
    exe_names = {}      # exe name (plain) -> unit index, for the in-set classification
    for u in units:
        for r, mname in u["info"]["matches"]:
            plain = CE.undecorate(mname)
            if plain in u["info"]["ported"]:
                exe_names[u["defs"].get(plain, plain)] = u
    wrapper_edits = collections.defaultdict(list)   # wrapper obj path -> [(exe plain name, rva)]

    def process_unit(co, u, rec_base, ext_statics):
        """Symbol rewrite of one DDK object (see the module docstring). ext_statics: also send port-excluded STATIC
        helpers to the exe body by name (second attempt, used when the local copies pull in callees the exe lacks)."""
        info, defs = u["info"], u["defs"]
        rec = rec_base
        rec["undef"], rec["defs"], rec["_edits"] = [], [], []
        rec["ext_statics"] = ext_statics
        match_plain = {CE.undecorate(n): r for r, n in info["matches"]}
        ported = info["ported"]
        excluded = info["port_excluded"]
        keep_secs = set()
        # 1. definitions
        for s in list(co.syms):
            if s.sec <= 0 or s.name.startswith((".", "@feat", "@comp")):
                continue
            plain = CE.undecorate(s.name)
            is_port_match = plain in match_plain and plain in ported
            ext_ok = s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL or (ext_statics and s.cls == CE.IMAGE_SYM_CLASS_STATIC)
            if ext_ok and s.is_func and plain in excluded and not is_port_match:
                # a helper copy the port leaves out (`#ifndef MATCH_PORT`): the port calls the one exe body by name,
                # so the DDK object does too. The local copy goes dead; its callers get an external reference.
                deco = s.name if s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL else "_" + plain
                s.name = s.name + f"__ddkx{u['rvas'][0]:08x}"
                s.cls = CE.IMAGE_SYM_CLASS_STATIC
                ref = co.add_symbol(deco, 0, 0, s.type, CE.IMAGE_SYM_CLASS_EXTERNAL)
                skip = {s.sec} | {x.index for x in co.sections if x.name.startswith(".debug")}
                co.repoint(s.index, ref.index, skip_sections=skip)
                rec.setdefault("externalized", []).append(deco)
                continue
            if s.cls == CE.IMAGE_SYM_CLASS_STATIC and not (s.is_func and is_port_match):
                continue
            if s.cls not in (CE.IMAGE_SYM_CLASS_EXTERNAL, CE.IMAGE_SYM_CLASS_STATIC):
                continue
            if is_port_match:
                if s.cls == CE.IMAGE_SYM_CLASS_STATIC:
                    ok, why = static_convention_ok(u["src"], info, plain, co.section(s.sec))
                    rec.setdefault("static_promotions", []).append({"name": plain, "standard_convention": ok,
                                                                    "evidence": why})
                    if not ok:
                        rec["status"] = "fail:static-register-convention"
                        rec["unbindable"] = []
                        return rec
                s.cls = CE.IMAGE_SYM_CLASS_EXTERNAL   # MATCH_STATIC: static under C, external under the port
                tgt = defs.get(plain, plain)
                new = CE.redecorate(s.name, tgt)
                rec["defs"].append({"rva": f"0x{match_plain[plain]:08x}", "ddk": s.name, "exe": new, "sec": s.sec})
                s.name = new
                keep_secs.add(s.sec)
                rec["_edits"].append((new, match_plain[plain]))
            elif s.name.startswith(("__real@", "??_C@", "__xmm@", "__mask@")):
                pass
            else:
                rec.setdefault("dropped_defs", []).append(s.name)
                s.name = s.name + f"__ddk{u['rvas'][0]:08x}"
        if not keep_secs:
            rec["status"] = "skip:no-ported-match-function"
            rec["unbindable"] = []
            return rec
        # 2. reachability inside the object from the kept functions (relocations to defined symbols)
        reach, work = set(keep_secs), list(keep_secs)
        while work:
            sec = co.section(work.pop())
            for _off, si, _typ in sec.relocs:
                t = co.slots[si]
                if t is not None and t.sec > 0 and t.sec not in reach:
                    reach.add(t.sec)
                    work.append(t.sec)
        used = set()
        for sn in reach:
            for _off, si, _typ in co.section(sn).relocs:
                used.add(si)
        rec["local_helpers"] = sorted({co.slots[si].name for sn in reach for _o, si, _t in co.section(sn).relocs
                                       if co.slots[si] is not None and co.slots[si].sec > 0 and co.slots[si].is_func
                                       and co.slots[si].sec not in keep_secs})
        # 3. undefined symbols
        unb = []
        for s in co.syms:
            if s.cls != CE.IMAGE_SYM_CLASS_EXTERNAL or s.sec != 0 or s.name.startswith((".", "@feat", "@comp")):
                continue
            plain = CE.undecorate(s.name)
            if s.index not in used:
                rec.setdefault("dropped_refs", []).append(s.name)
                s.name = "__ddk_dropped"
                continue
            addr = info["globals"].get(plain)
            if addr is None and not s.is_func:
                mm = re.search(r"_([0-9a-fA-F]{8})$", plain)
                if mm and 0x00400000 <= int(mm.group(1), 16) < 0x00a00000:
                    addr = int(mm.group(1), 16)
            if s.value != 0:
                cls = "common"
            elif addr is not None and not s.is_func and plain in info["port_copies"]:
                nm = f"__ddkcopy_{addr:08x}"
                data_copies[nm] = M.pe().get_data(addr - 0x00400000, info["port_copies"][plain])
                s.name = nm
                cls = "data-copy"
            elif addr is not None and not s.is_func:
                nm = f"__ddkabs_{addr:08x}"
                abs_syms[nm] = addr
                s.name = nm
                cls = "data-abs"
            elif plain in defs:
                cls = bind_to(s, defs[plain], "wrapper-bound")
            elif plain in exe_names:
                cls = "in-set"
            elif s.name in crt:
                cls = "crt"
            elif plain in gdefs:
                cls = bind_to(s, gdefs[plain], "global-bound")
            else:
                cls = "exe-by-name"
            if cls in ("wrapper-bound", "global-bound") and CE.undecorate(s.name) in u["wstatics"]:
                cls = "wrapper-static"   # promoted to an external in the wrapper surgery
            elif cls in ("wrapper-bound", "global-bound", "exe-by-name") and defmap and s.name not in defmap:
                cls = "unbound:" + cls
                unb.append(s.name)
            elif cls == "cxx-unresolved":
                unb.append(s.name)
            mm = re.match(r"FUN_([0-9a-fA-F]{8})$", plain)
            bkind = B.get(int(mm.group(1), 16), {}).get("kind", "") if mm else ""
            rec["undef"].append({"name": s.name, "orig": plain, "class": cls, "bind": bkind})
        rec["unbindable"] = unb
        rec["status"] = "ok"
        return rec

    for u in units:
        src, info, defs = u["src"], u["info"], u["defs"]
        rec = {"src": str(src.relative_to(ROOT)).replace("\\", "/"), "wrapper": u["wrel"], "rvas": [f"0x{r:08x}" for r in u["rvas"]],
               "status": "ok", "undef": [], "defs": []}
        if info["lang"] != "c":
            rec["status"] = "skip:lang-" + info["lang"]
            report["units"].append(rec)
            continue
        if "@mglobal" in defs:
            rec["status"] = "fail:wrapper-redefines-globals"
            rec["evidence"] = defs["@mglobal"]
            report["units"].append(rec)
            continue
        obj, log = ddk_compile(src, info, objdir, u["wrapper"].stem)
        if obj is None:
            rec["status"] = "fail:ddk-compile"
            rec["log"] = log[-600:]
            report["units"].append(rec)
            continue
        pristine = obj.read_bytes()
        attempt = None
        for ext_statics in (False, True):
            co = CE.Coff(pristine)
            r = process_unit(co, u, rec_base=dict(rec), ext_statics=ext_statics)
            attempt = (co, r)
            if r["status"] != "ok" or not r["unbindable"]:
                break
        co, rec = attempt
        if rec["status"] == "ok" and rec["unbindable"]:
            rec["status"] = "fail:unbindable-callee"   # stays on its VS2022 body in the variant
        edits = rec.pop("_edits", [])
        if rec["status"] != "ok":
            report["units"].append(rec)
            continue
        for new, rva in edits:
            wrapper_edits[u["wrapper"]].append((new, rva))
        # SafeSEH: cl 13.00 predates the @feat.00 flag, and one unmarked object makes the linker drop the exe's whole
        # safe-handler table. An object that registers no SEH handler (no __except_handler* reference, no .sxdata)
        # is SafeSEH-compatible by definition, so it is marked; one that does is left unmarked and reported.
        seh = any(s.name.startswith(("__except_handler", "___except_handler")) for s in co.syms) or \
            any(x.name == ".sxdata" for x in co.sections)
        if not seh and not any(s.name == "@feat.00" for s in co.syms):
            co.add_symbol("@feat.00", 1, CE.IMAGE_SYM_ABSOLUTE, 0, CE.IMAGE_SYM_CLASS_STATIC)
        rec["safeseh_marked"] = not seh
        co.save(obj)
        rec["obj"] = str(obj)
        rec["drectve"] = co.drectve().strip()
        u["obj"] = obj
        report["units"].append(rec)

    # wrapper recompile (variant only): same flags as build_objs.ps1 plus /Ob0, so VS2022 cannot inline a matched body
    # into another function of the wrapper (an inlined copy would keep running VS2022 code where the DDK body is meant)
    vsdir = objdir / "vsrc"
    vsdir.mkdir()
    recompile_wrappers(list(wrapper_edits), vsdir, out)

    # wrapper surgery
    for wpath, edits in wrapper_edits.items():
        base = vsdir / (wpath.stem + ".obj")
        dst = objdir / ("vs_" + wpath.stem + ".obj")
        wrec = {"default_obj": str(base), "edited_obj": str(dst), "names": []}
        if not base.exists():
            wrec["error"] = "wrapper object missing after the variant recompile"
            report["wrappers"][str(wpath)] = wrec
            continue
        co = CE.Coff.load(base)
        for name, rva in edits:
            hits = [s for s in co.syms if s.name == name and s.cls in (CE.IMAGE_SYM_CLASS_EXTERNAL, CE.IMAGE_SYM_CLASS_STATIC)]
            dfn = [s for s in hits if s.sec > 0 and s.is_func]   # a MATCH_STATIC copy is STATIC in the wrapper
            und = [s for s in hits if s.sec == 0 and s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL]
            ent = {"name": name, "rva": f"0x{rva:08x}"}
            if dfn:
                d = dfn[0]
                d.name = name + "__vs"
                if und:
                    newi = und[0].index
                else:
                    newi = co.add_symbol(name, 0, 0, d.type, CE.IMAGE_SYM_CLASS_EXTERNAL).index
                skip = {d.sec} | {s.index for s in co.sections if s.name.startswith(".debug")}
                ent["repointed"] = co.repoint(d.index, newi, skip_sections=skip)
                ent["vs_definition"] = "renamed"
            else:
                ent["vs_definition"] = "absent (inlined or not emitted)"
            wrec["names"].append(ent)
        # promote wrapper statics that DDK objects of this wrapper reference (C names, or C++ mangled ?name@@...):
        # the static gets a unique external name and the DDK objects' references are renamed to it
        mine = [u for u in units if u["wrapper"] == wpath and "obj" in u]
        needed = set()
        for u in mine:
            for s in CE.Coff.load(u["obj"]).syms:
                if s.sec == 0 and s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL:
                    needed.add(s.name)
        defined_ext = {s.name for s in co.syms if s.sec > 0 and s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL}
        prom, ren = [], {}
        for need in sorted(needed - defined_ext):
            plain = CE.undecorate(need)
            hits = [s for s in co.syms if s.cls == CE.IMAGE_SYM_CLASS_STATIC and s.sec > 0 and s.is_func and
                    (s.name == need or s.name.startswith("?" + plain + "@@"))]
            if len(hits) != 1:
                continue
            h = hits[0]
            newname = CE.redecorate(need, f"{plain}__w_{wpath.stem}")
            prom.append(f"{h.name} -> {newname}")
            h.name, h.cls = newname, CE.IMAGE_SYM_CLASS_EXTERNAL
            ren[need] = newname
        if ren:
            for u in mine:
                dco = CE.Coff.load(u["obj"])
                for s in dco.syms:
                    if s.sec == 0 and s.name in ren:
                        s.name = ren[s.name]
                dco.save(u["obj"])
        wrec["promoted_statics"] = prom
        co.save(dst)
        report["wrappers"][str(wpath)] = wrec

    absobj = objdir / "ddk_abs.obj"
    abs_syms["__ddk_dropped"] = 0
    CE.build_absolute_obj(absobj, abs_syms)
    copyobj = objdir / "ddk_copies.obj"
    CE.build_data_obj(copyobj, data_copies)
    report["data_copies"] = {k: v.hex() for k, v in data_copies.items()}
    thunkobj = objdir / "ddk_thunks.obj"
    CE.build_thunk_obj(thunkobj, abs_calls)
    report["n_abs_call_thunks"] = len(abs_calls)

    # link response: default link.rsp with edited wrapper objects swapped, plus the DDK objects
    deflink = [l.strip().strip('"') for l in (DEF_OBJ / "link.rsp").read_text().splitlines() if l.strip()]
    swap = {str((DEF_OBJ / (pathlib.Path(w).stem + ".obj"))).lower(): r["edited_obj"]
            for w, r in report["wrappers"].items() if "error" not in r}
    lines = []
    for l in deflink:
        lines.append(swap.get(str(pathlib.Path(l)).lower(), l))
    ddk_objs = [str(u["obj"]) for u in units if "obj" in u]
    lines += ddk_objs + [str(absobj), str(thunkobj), str(copyobj)]
    (out / "link_ddk.rsp").write_text("\n".join('"' + l + '"' for l in lines) + "\n")
    report["n_units"] = len(units)
    report["n_ddk_objs"] = len(ddk_objs)
    report["n_abs"] = len(abs_syms)
    (out / "ddkobj_build.json").write_text(json.dumps(report, indent=1, default=str))
    print(f"[ddkobj] units {len(units)}  ddk objects {len(ddk_objs)}  wrappers edited {len(report['wrappers'])}  "
          f"absolute data symbols {len(abs_syms)}")
    bad = [u for u in report["units"] if u["status"] != "ok"]
    for u in bad[:20]:
        print(f"[ddkobj]   {u['status']}: {u['src']}")
    if a.link:
        return do_link(out)
    return 0


_SC_CACHE = {}


def static_convention_ok(src: pathlib.Path, info, plain: str, sec):
    """A MATCH function that is `static` under C (MATCH_STATIC) may get a compiler-chosen register convention from the
    DDK (re/match/s08/004325c0.c: 0x0042ac50 takes EAX/ECX). Promoting such a body to an external that VS2022 code
    calls with stack arguments would break the call. Check: recompile the source with that function non-static and
    compare its code bytes (relocation fields masked). Same bytes = standard convention = safe to promote."""
    key = (str(src), plain)
    if key in _SC_CACHE:
        return _SC_CACHE[key]
    import tempfile
    t = info["text"]
    t2 = re.sub(r"#\s*define\s+MATCH_STATIC\s+static\b", "#define MATCH_STATIC", t)
    t2 = re.sub(r"(^|\n)([ \t]*)static(\s+[^;{(]*\b" + re.escape(plain) + r"\s*\()", r"\1\2\3", t2)
    tmp = pathlib.Path(tempfile.mkdtemp(prefix="ddksc_")) / src.name
    tmp.write_text(t2, encoding="utf-8")
    r = M.compile_obj(tmp, info["flags"], info["lang"], info["toolchain"], errors_ok=True)
    if r[0] is None:
        _SC_CACHE[key] = (False, "non-static recompile failed")
        return _SC_CACHE[key]
    co2 = CE.Coff.load(r[0])
    hit = [s for s in co2.syms if s.sec > 0 and s.is_func and CE.undecorate(s.name) == plain]
    if not hit:
        _SC_CACHE[key] = (False, "function absent from the non-static compile")
        return _SC_CACHE[key]
    s2 = co2.section(hit[0].sec)

    def masked(sc):
        b = bytearray(sc.data)
        for off, _si, _ty in sc.relocs:
            b[off:off + 4] = b"\0\0\0\0"
        return bytes(b)
    same = masked(sec) == masked(s2)
    _SC_CACHE[key] = (same, "identical bytes when non-static" if same else
                      f"bytes differ when non-static ({len(sec.data)} vs {len(s2.data)} B)")
    return _SC_CACHE[key]


def x87_set():
    s = set()
    for line in (MOD / "x87_tus.txt").read_text().splitlines():
        line = line.strip()
        if line and not line.startswith("#"):
            s.add(pathlib.Path(line).stem.lower())
    return s


def recompile_wrappers(wrappers, vsdir: pathlib.Path, out: pathlib.Path):
    x87 = x87_set()
    groups = {"sse2": [w for w in wrappers if w.stem.lower() not in x87],
              "x87": [w for w in wrappers if w.stem.lower() in x87]}
    NL = "\n"
    pre = "" if os.environ.get("VCINSTALLDIR") else f'call "{VCVARS}" >nul' + NL
    body = "@echo off" + NL + pre + f'pushd "{SRC}"' + NL
    for g, ws in groups.items():
        if not ws:
            continue
        rsp = out / f"vsrc_{g}.rsp"
        rsp.write_text(NL.join('"' + str(w) + '"' for w in ws) + NL)
        extra = "/arch:IA32 " if g == "x87" else ""
        fo = str(vsdir) + "\\\\"
        body += (f'cl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE {extra}/Ob0 /c /MP "/Fo{fo}" "@{rsp}"' + NL +
                 "if errorlevel 1 (popd & exit /b 1)" + NL)
    body += "popd" + NL + "exit /b 0" + NL
    bat = out / "vsrc_compile.bat"
    bat.write_text(body)
    r = subprocess.run(["cmd", "/c", str(bat)], capture_output=True, text=True)
    (out / "vsrc_compile.log").write_text(r.stdout + r.stderr)
    if r.returncode != 0:
        raise SystemExit("wrapper recompile failed:" + NL + r.stdout[-3000:] + r.stderr[-2000:])


def do_link(out: pathlib.Path):
    exe = out / "mashed_re_ddk.exe"
    mp = out / "mashed_re_ddk.map"
    o = BUILD
    q = '"'
    extra = " ".join(q + str(o / n) + q for n in ("mashed_re.res", "QhullBridge_exe.obj", "RwBridge_exe.obj",
                                                  "RwRasterBridge_exe.obj", "RwSceneBuild_exe.obj",
                                                  "RwRaceSubmit_exe.obj", "RwRaceFx_exe.obj", "RwApiBind_exe.obj"))
    libs = f'user32.lib d3d9.lib gdi32.lib {os.environ.get("DDKOBJ_EXTRA_LIBS", "")} "{MOD / "deps" / "qhull-2002.1" / "qhull_2002_1.lib"}" ' \
           f'"{MOD / "deps" / "librw" / "librw_d3d9.lib"}"'
    linkcmd = (f'cl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE /Fe"{exe}" @"{out / "link_ddk.rsp"}" {extra} '
               f'/link /SUBSYSTEM:WINDOWS /BASE:0x10000 /FIXED:NO /DYNAMICBASE:NO /MAP:"{mp}" '
               f'/DELAYLOAD:d3d9.dll /DELAYLOAD:dinput8.dll /NODEFAULTLIB:libc.lib {libs}')
    bat = out / "link_ddk.bat"
    pre = "" if os.environ.get("VCINSTALLDIR") else f'call "{VCVARS}" >nul\n'
    bat.write_text("@echo off\n" + pre + f'pushd "{SRC}"\n' + linkcmd + "\nset RC=%errorlevel%\npopd\nexit /b %RC%\n")
    r = subprocess.run(["cmd", "/c", str(bat)], capture_output=True, text=True)
    (out / "link_ddk.log").write_text(r.stdout + r.stderr)
    print(r.stdout[-6000:] + r.stderr[-2000:])
    print(f"[ddkobj] link rc={r.returncode} -> {exe if r.returncode == 0 else '(failed)'}")
    return r.returncode


# ------------------------------------------------------------------ post-link byte check
def read_map(mp: pathlib.Path):
    syms = {}
    pref = None
    for line in mp.read_text(errors="replace").splitlines():
        m = re.match(r"\s*Preferred load address is ([0-9a-fA-F]+)", line)
        if m:
            pref = int(m.group(1), 16)
        m = re.match(r"\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s", line)
        if m:
            syms.setdefault(m.group(1), int(m.group(2), 16))
            objm = re.search(r"\s(\S+\.obj)\s*$", line)
            if objm:
                syms.setdefault((m.group(1), objm.group(1).lower()), int(m.group(2), 16))
    return syms, pref


def main_check(a):
    import pefile
    out = pathlib.Path(a.out).resolve()
    rep = json.loads((out / "ddkobj_build.json").read_text())
    syms, _pref = read_map(out / "mashed_re_ddk.map")
    pe = pefile.PE(str(out / "mashed_re_ddk.exe"), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    ope = M.pe()
    obase = ope.OPTIONAL_HEADER.ImageBase
    oimg = ope.get_memory_mapped_image()
    rows = []
    for u in rep["units"]:
        if u["status"] != "ok":
            continue
        co = CE.Coff.load(u["obj"])
        for d in u["defs"]:
            rva = int(d["rva"], 16)
            sec = co.section(d["sec"])
            n = len(sec.data)
            va = syms.get((d["exe"], pathlib.Path(u["obj"]).name.lower()))
            row = {"rva": d["rva"], "exe_symbol": d["exe"], "src": u["src"], "size": n}
            if va is None:
                row["result"] = "not-in-map"
                rows.append(row)
                continue
            got = img[va - base:va - base + n]
            want = oimg[rva - obase:rva - obase + n]
            mask = bytearray(n)
            exact_abs = 0
            for (off, si, typ) in sec.relocs:
                tsym = co.slots[si]
                w = 4
                if tsym is not None and tsym.name.startswith("__ddkabs_") and typ == CE.REL_I386_DIR32:
                    exact_abs += 1       # absolute data address: compared, not masked
                    continue
                for k in range(off, min(off + w, n)):
                    mask[k] = 1
            diff = [k for k in range(n) if not mask[k] and got[k] != want[k]]
            row.update({"exe_va": f"0x{va:08x}", "relocs_masked": sum(1 for r in sec.relocs) - exact_abs,
                        "abs_data_refs_exact": exact_abs, "diff_bytes": len(diff),
                        "result": "IDENTICAL" if not diff else "DIFFERENT",
                        "first_diff": f"+0x{diff[0]:x}" if diff else ""})
            # the wrapper's VS copy must be gone from the image (dead, /OPT:REF) or at least not be the bound name
            row["vs_copy_in_map"] = (d["exe"] + "__vs") in syms
            rows.append(row)
    with open(out / "BYTECHECK.tsv", "w", newline="", encoding="utf-8") as fh:
        cols = ["rva", "exe_symbol", "exe_va", "size", "relocs_masked", "abs_data_refs_exact", "diff_bytes", "result",
                "first_diff", "vs_copy_in_map", "src"]
        w = csv.DictWriter(fh, cols, delimiter="\t", extrasaction="ignore")
        w.writeheader()
        for r in rows:
            w.writerow(r)
    c = collections.Counter(r["result"] for r in rows)
    print(f"[ddkobj check] {dict(c)} -> {out / 'BYTECHECK.tsv'}")
    return 0


def main(argv=None):
    p = argparse.ArgumentParser()
    sub = p.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("build")
    b.add_argument("--units", default="")
    b.add_argument("--units-file", default="")
    b.add_argument("--matched-only", action="store_true")
    b.add_argument("--out", default=str(BUILD / "ddkobj"))
    b.add_argument("--link", action="store_true")
    c = sub.add_parser("check")
    c.add_argument("--out", default=str(BUILD / "ddkobj"))
    a = p.parse_args(argv)
    return main_build(a) if a.cmd == "build" else main_check(a)


if __name__ == "__main__":
    sys.exit(main())
