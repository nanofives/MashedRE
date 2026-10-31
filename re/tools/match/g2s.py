"""Ghidra decompilation -> compilable C for the period compiler, then score it (Mashed, release /O2 code).

Adapted from SimGolf re/tools/ghidra2src.py (which targets VC6 debug builds); this one is much smaller because
MASHED.exe's game code is C (Rich prodid 28) and C accepts Ghidra's int/pointer mixing with warnings only.

Input: the JSON of `re/tools/decomp_pc.py --port --callees --json` (decompiled text, the decompiler's prototype,
typed globals, callee prototypes). Per function:
  * typedefs for Ghidra's types and the Win32 names it prints; CONCAT/SUB/ZEXT/SEXT helpers
  * globals as `extern <type> <name>;` from the decompiler's own global symbol map
  * calling conventions from the BYTES, not Ghidra (it stores "unknown" for every game function): a callee whose
    body has `ret N` (N > 0) is __stdcall, register params ECX(+EDX) make it __fastcall; imports are dllimport
    __stdcall; the function itself the same way
  * compiles; on errors declares what is missing (undeclared identifiers) and retries a few rounds
Writes <out>/<rva>.c and <out>/results.tsv (rva, stage, acc, regs, ops, n_mine, n_orig, note).

    py -3.12 re/tools/match/g2s.py --decomp log/matchpilot/decomp.json --out log/matchpilot/g2s [--only 0x...]
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

import capstone

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match  # noqa: E402

HEADER = r"""typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef int code();
typedef unsigned char bool; typedef unsigned int WPARAM; typedef long LPARAM; typedef long LRESULT; typedef void *HWND;
typedef void *HINSTANCE; typedef void *HMODULE; typedef void *HANDLE; typedef int BOOL; typedef unsigned int UINT;
typedef unsigned long DWORD; typedef char *LPSTR; typedef const char *LPCSTR; typedef void *LPVOID; typedef unsigned short WORD;
typedef unsigned char BYTE; typedef long LONG; typedef void *HDC; typedef void *HMENU; typedef void *HICON;
typedef unsigned short wchar_t; typedef wchar_t *LPWSTR; typedef const wchar_t *LPCWSTR; typedef int INT_PTR;
typedef unsigned int UINT_PTR; typedef long LONG_PTR; typedef unsigned long ULONG_PTR; typedef long HRESULT;
typedef long double unkbyte10; typedef unsigned int uintptr_t; typedef unsigned int size_t;
#define true 1
#define false 0
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT12(a,b) ((uint)(((uint)(uchar)(a)<<16)|(ushort)(b)))
#define CONCAT13(a,b) ((uint)(((uint)(uchar)(a)<<24)|((uint)(b)&0xffffff)))
#define CONCAT21(a,b) ((uint)(((uint)(ushort)(a)<<8)|(uchar)(b)))
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
#define SUB41(a,n) ((uchar)((uint)(a)>>((n)*8)))
#define SUB42(a,n) ((ushort)((uint)(a)>>((n)*8)))
#define SUB81(a,n) ((uchar)((ulonglong)(a)>>((n)*8)))
#define SUB84(a,n) ((uint)((ulonglong)(a)>>((n)*8)))
#define ZEXT14(a) ((uint)(uchar)(a))
#define ZEXT24(a) ((uint)(ushort)(a))
#define ZEXT48(a) ((ulonglong)(uint)(a))
#define SEXT14(a) ((int)(signed char)(a))
#define SEXT24(a) ((int)(short)(a))
#define SEXT48(a) ((longlong)(int)(a))
"""
MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
ERR = re.compile(r"\((\d+)\) : error (C\d+): (.*)")


def ctype(t: str) -> str:
    t = t.strip()
    return {"undefined": "undefined1", "bool": "bool", "pointer": "void *"}.get(t, t)


def ret_pop(va: int) -> int | None:
    """Bytes popped by the function's `ret N` (0 for plain ret), from its own body; None if no ret found."""
    n = match.ghidra_size(va)
    if not n:
        return None
    pops = set()
    for ins in MD.disasm(match.original_bytes(va, n), va):
        if ins.mnemonic == "ret":
            pops.add(int(ins.op_str, 16) if ins.op_str else 0)
    return max(pops) if pops else None


def reg_params(params) -> list[str]:
    return [p["storage"].split(":")[0] for p in params if p.get("storage") and not p["storage"].startswith("Stack")]


def convention(va: int, params) -> str:
    regs = reg_params(params)
    if "ECX" in regs:
        return "__fastcall" if ("EDX" in regs or ret_pop(va) == 0 or not params) else "__thiscall"
    pop = ret_pop(va)
    return "__stdcall" if pop else ""


def declare_callee(c) -> str:
    dp = c.get("dproto") or {}
    ret = ctype(dp.get("ret") or c.get("ret") or "int")
    if ret == "undefined1":
        ret = "int"
    params = dp.get("params") or c.get("params") or []
    ps = ", ".join(f"{ctype(p['type'])} p{i}" for i, p in enumerate(params)) or "void"
    if dp.get("varargs"):
        ps = (ps if params else "") + (", ..." if params else "...")
        if not params:
            ps = ""
    name = c["name"].replace("FID_conflict:", "FID_conflict_")
    if c.get("external"):
        return f"__declspec(dllimport) {ret} __stdcall {name}({ps});"
    cc = c.get("cc") if c.get("cc") not in (None, "unknown", "__cdecl") else convention(int(c["entry"], 16), params)
    if cc == "__thiscall":
        cc = "__fastcall"        # [tool limit] C has no __thiscall: mismatches by construction
    return f"{ret} {cc + ' ' if cc else ''}{name}({ps});"


COOKIE = "DAT_00616038"   # /GS cookie (written by the CRT init at 0x004a5825); the compiler re-emits its uses


def partial(m):
    """Ghidra's `X._A_B_` (B bytes at offset A of X) as a cast."""
    t = {1: "uchar", 2: "ushort", 4: "uint", 8: "ulonglong"}[int(m[3])]
    return f"(*({t} *)((char *)&{m[1]} + {int(m[2])}))"


def clean_body(body: str) -> str:
    body = re.sub(r"FID_conflict:", "FID_conflict_", body)
    # /GS: drop the cookie store and the check call (both come back from /GS)
    body = re.sub(r"^\s*\w+ = " + COOKIE + r" \^ [^;]*;\n", "", body, flags=re.M)
    body = re.sub(r"^\s*__security_check_cookie\([^;]*\);\n", "", body, flags=re.M)
    body = re.sub(r"\b([A-Za-z_]\w*)\._(\d+)_(\d+)_", partial, body)
    return body


def build(f) -> tuple[str, str]:
    """(source, note)."""
    va = int(f["entry"], 16)
    notes = []
    body = f["decomp"].strip("\n")
    # drop Ghidra's leading comment block
    body = re.sub(r"^\s*/\*.*?\*/\s*", "", body, flags=re.S)
    body = clean_body(body)
    lines = [HEADER]
    seen = set()
    for g in f.get("globals", []):
        n = g["name"]
        if n in seen:
            continue
        seen.add(n)
        t = ctype(g["type"])
        if n.startswith("s_") or n.startswith("u_"):
            lines.append(f"extern char {n}[];")
        elif t.endswith("]"):
            lines.append(f"extern {t.split('[')[0]} {n}[];")
        else:
            lines.append(f"extern {t} {n};")
    for c in f.get("callee_protos", []):
        if c["name"] in seen or c["name"] == f["name"] or c["name"] == "__security_check_cookie":
            continue
        seen.add(c["name"])
        lines.append(declare_callee(c))
    cc = convention(va, f["proto"]["params"])
    if cc == "__thiscall":
        notes.append("thiscall (C++ method): compiled as __fastcall, tool limit")
        cc = "__fastcall"
    # signature line: first line ending with ')' before '{'
    m = re.search(r"^(.*?)\b(" + re.escape(f["name"]) + r")\s*\(", body, re.M)
    if m and cc:
        body = body[:m.start(2)] + cc + " " + body[m.start(2):]
    for marker in ("in_", "unaff_", "extraout_"):
        if re.search(rf"\b{marker}\w+", body):
            notes.append(f"{marker}* register value")
    lines.append(f"// MATCH: 0x{va:08x} {f['name']}")
    lines.append(body)
    return "\n".join(lines) + "\n", "; ".join(notes)


def try_compile(path: pathlib.Path, text: str, flags: str, tc: str):
    for rnd in range(6):
        path.write_text(text)
        obj, out = match.compile_obj(path, flags, "c", tc, errors_ok=True)
        if obj:
            return obj, text, ""
        errs = [(int(a), b, c) for a, b, c in ERR.findall(out)]
        changed = False
        for ln, code, msg in errs:
            mm = re.match(r"'(\w+)' : undeclared identifier", msg)
            if code == "C2065" and mm:
                ident = mm[1]
                decl = f"int {ident}();" if re.search(rf"\b{ident}\s*\(", text) else f"extern int {ident};"
                if decl not in text:
                    text = text.replace("// MATCH:", decl + "\n// MATCH:", 1)
                    changed = True
            mm = re.match(r"'(\w+)' : too (?:few|many) actual parameters", msg)
            if code in ("C2198", "C2197") and mm:
                new = re.sub(rf"^(.*\b{mm[1]})\(.*\);$", r"\1();", text, count=1, flags=re.M)
                changed |= new != text
                text = new
            if code == "C2120":
                src_line = text.splitlines()[ln - 1] if ln <= len(text.splitlines()) else ""
                for callee in re.findall(r"\b(\w+)\s*\(", src_line):
                    new = re.sub(rf"^void ((?:__\w+ )?{callee}\()", r"undefined4 \1", text, count=1, flags=re.M)
                    changed |= new != text
                    text = new
        if not changed:
            return None, text, out
    return None, text, out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--decomp", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--only", default="")
    ap.add_argument("--flags", default="/O2 /GS")
    ap.add_argument("--toolchain", default="ddk")
    a = ap.parse_args()
    out = pathlib.Path(a.out)
    out.mkdir(parents=True, exist_ok=True)
    data = json.loads(pathlib.Path(a.decomp).read_text(encoding="utf-8"))
    rows = []
    for f in data["functions"]:
        va = int(f["entry"], 16)
        if a.only and va != int(a.only, 16):
            continue
        src, note = build(f)
        src = f"// FLAGS: {a.flags}\n// TOOLCHAIN {a.toolchain}\n" + src
        p = out / f"{va:08x}.c"
        obj, text, err = try_compile(p, src, a.flags, a.toolchain)
        if obj is None:
            e = ERR.search(err)
            rows.append((f"0x{va:08x}", "compile", "", "", "", "", "", (e[2] + " " + e[3])[:150] if e else err[-150:].replace("\n", " ")))
            print(f"0x{va:08x} compile-fail {rows[-1][-1]}")
            continue
        secs, syms = match.coff(obj)
        try:
            acc, regs, ops, am, bm, full, sm = match.score_function(va, secs, syms, f["name"])
        except SystemExit as e:
            rows.append((f"0x{va:08x}", "score", "", "", "", "", "", str(e)[:150]))
            print(f"0x{va:08x} score-fail {str(e)[:150]}")
            continue
        rows.append((f"0x{va:08x}", "ok", f"{100*acc:.1f}", f"{100*regs:.1f}", f"{100*ops:.1f}", len(am), len(bm), note))
        print(f"0x{va:08x} {100*acc:5.1f}% regs {100*regs:5.1f}% ops {100*ops:5.1f}%  {len(am)}/{len(bm)}  {note}")
    with (out / "results.tsv").open("w") as fh:
        fh.write("rva\tstage\tacc\tregs\tops\tn_mine\tn_orig\tnote\n")
        for r in rows:
            fh.write("\t".join(map(str, r)) + "\n")


if __name__ == "__main__":
    main()
