"""Build guard: no bare absolute memory operand in MSVC inline asm.

MSVC's inline assembler reads a bare numeric `[imm]` as the IMMEDIATE, not a memory access:

    mov esi, dword ptr [771A0Ch]        ->  be 0c 1a 77 00        mov esi, 0x771a0c        (WRONG)
    mov esi, dword ptr ds:[771A0Ch]     ->  8b 35 0c 1a 77 00     mov esi, [0x771a0c]      (right)

`Fwd494fd0_DoublePlusHalf` (0x00495080, Render/PromoLoop_round256.cpp) shipped the first form as C3: the C3 A/B
stubs the callee and records stack arguments only, so a wrong REGISTER input (ESI = the address instead of the
device pointer stored there) was invisible, and with the full hook set installed MASHED.exe jumped to 0x1 during the
nav boot-to-menu (re/diag/KNOWN_ISSUES.md AUTOHOOK-INLINE-ASM-IMMEDIATE, 2026-10-16). Every other absolute operand in
the tree is already spelled `ds:[...]`; this keeps it that way.

What it flags: inside an `__asm` block (or anywhere in a `.inc`, which only ever holds asm), a memory operand whose
bracket contains ONLY a number (`[0x771A0C]`, `[771A0Ch]`), with no segment override (`ds:` `cs:` `es:` `ss:` `fs:`
`gs:`) directly before the `[`. `[ecx]`, `[esp+4]`, `[eax+0x1c]`, `ds:[0x771A0C]` and comments are fine.

    py -3.12 scripts/lint_inline_asm_abs.py            # scan mashedmod/src, exit 1 on any finding
    py -3.12 scripts/lint_inline_asm_abs.py --selftest # the built-in positive/negative samples only

`mashedmod/build.bat` runs it before the compile step; set MASHED_SKIP_ASM_LINT=1 to skip (and say so in the commit).
"""
from __future__ import annotations

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
SRC = ROOT / "mashedmod" / "src"

# [0x771A0C] / [771A0Ch] / [0771A0Ch]: nothing but a number inside the brackets
BARE = re.compile(r"\[\s*(?:0[xX][0-9A-Fa-f]+|[0-9][0-9A-Fa-f]*[hH])\s*\]")
SEG = re.compile(r"(?:\b[dcefgs]s\s*:\s*)$", re.I)
# an asm line starts with a mnemonic (after an optional `label:`); a C++ line such as `body[0x1e] = 0` does not.
# Needed because some .inc files are C++ data tables, not asm (Vehicle/CarMaterialTables_dirt.inc).
MNEM = re.compile(
    r"^\s*(?:[A-Za-z_]\w*\s*:\s*)?(?:rep[a-z]*\s+|lock\s+)?"
    r"(?:mov|movzx|movsx|movss|movsd|movaps|movups|movq|movd|lea|xchg|push|pop|call|jmp|j[a-z]{1,3}|inc|dec|neg|not|"
    r"add|adc|sub|sbb|and|or|xor|cmp|test|imul|mul|idiv|div|shl|shr|sar|sal|rol|ror|cmov[a-z]+|set[a-z]+|"
    r"fld|fst|fstp|fild|fist|fistp|fadd|faddp|fiadd|fsub|fsubp|fsubr|fsubrp|fisub|fmul|fmulp|fimul|fdiv|fdivp|fdivr|"
    r"fdivrp|fcom|fcomp|fcompp|ficom|ficomp|fucom|fucomp|fldcw|fnstcw|fnstsw|fxch|"
    r"addss|subss|mulss|divss|addsd|subsd|mulsd|divsd|ucomiss|comiss|ucomisd|comisd|cvt[a-z0-9]+|"
    r"cmpsb|cmpsd|movsb|movsw|movsd|stosb|stosd|scasb|lodsb)\b", re.I)


def strip_comments(text: str, asm: bool) -> list[str]:
    """Per-line code with /* */, // and (in asm) `;` comments removed. Line count preserved."""
    out, in_block = [], False
    for ln in text.splitlines():
        res, i = "", 0
        while i < len(ln):
            if in_block:
                j = ln.find("*/", i)
                if j < 0:
                    i = len(ln)
                else:
                    in_block, i = False, j + 2
                continue
            two = ln[i:i + 2]
            if two == "/*":
                in_block, i = True, i + 2
            elif two == "//":
                break
            elif asm and ln[i] == ";":
                break
            else:
                res += ln[i]
                i += 1
        out.append(res)
    return out


def findings_in(text: str, is_inc: bool) -> list[tuple[int, str]]:
    """[(1-based line, stripped source line)] of bare absolute operands in asm context."""
    out: list[tuple[int, str]] = []
    lines = strip_comments(text, asm=is_inc)
    depth = 0                 # brace depth inside the current `__asm { ... }` block (0 = not inside one)
    for n, ln in enumerate(lines, 1):
        code, check = ln, is_inc
        if not is_inc:
            if depth > 0:                                  # inside a block: every line is asm
                check = True
                depth = max(depth + ln.count("{") - ln.count("}"), 0)
            else:
                m = re.search(r"\b__asm\b", ln)
                if m:                                      # `__asm {` opens a block, `__asm mov ...` is one line
                    check = True
                    code = ln[m.end():]
                    depth = max(code.count("{") - code.count("}"), 0)
        if not check:
            continue
        code = code.split(";", 1)[0]                       # in asm text `;` starts a comment
        if not MNEM.match(code):                           # not an instruction line (C++ data in a .inc, a brace, ...)
            continue
        for b in BARE.finditer(code):
            if not SEG.search(code[:b.start()]):
                out.append((n, ln.strip()))
                break
    return out


SELFTEST = [
    # (source, is_inc, expected number of findings, label)
    ("void f() {\n  __asm {\n    mov esi, dword ptr [771A0Ch]\n    ret\n  }\n}\n", False, 1, "bare hex-suffix inside __asm block"),
    ("void f() {\n  __asm {\n    mov esi, dword ptr [0x771A0C]\n  }\n}\n", False, 1, "bare 0x-prefix inside __asm block"),
    ("void f() {\n  __asm fadd dword ptr [5CC32Ch]\n}\n", False, 1, "bare operand on a single-line __asm"),
    ("        fld dword ptr [0x005cc32c]\n", True, 1, "bare operand in a .inc"),
    ("void f() {\n  __asm {\n    mov esi, dword ptr ds:[771A0Ch]\n    mov eax, ds:[0x771A0C]\n  }\n}\n", False, 0, "ds: override is a memory operand"),
    ("void f() {\n  __asm {\n    mov eax, [ecx]\n    mov edx, [esp+4]\n    lea eax, [eax+0x1c]\n    mov ecx, 771A0Ch\n  }\n}\n", False, 0, "register forms and an explicit immediate"),
    ("void f() {\n  __asm {\n    // mov esi, dword ptr [771A0Ch]\n    mov esi, dword ptr ds:[771A0Ch]  ; was [771A0Ch]\n  }\n}\n", False, 0, "comments"),
    ("int g(int* arr) {\n  return arr[0x10] + tbl[0x100];\n}\n", False, 0, "C++ indexing outside __asm"),
    ("void f() {\n  __asm {\n    mov eax, 1\n  }\n  int v = t[0x20];\n}\n", False, 0, "C++ indexing after the block closed"),
    ("        mov eax, dword ptr ds:[0x0066d6d8]  // 00426bb0\n", True, 0, ".inc with ds: and a trailing comment"),
    ("    body[0x1e] = 0;\n    body[0x00] = 2;\n", True, 0, ".inc that is a C++ data table, not asm"),
    ("L_top:  mov esi, dword ptr [771A0Ch]\n        rep movsd\n", True, 1, "label prefix before the mnemonic"),
]


def selftest() -> bool:
    ok = True
    for src, inc, want, label in SELFTEST:
        got = len(findings_in(src, inc))
        if got != want:
            print(f"[asm-lint] SELFTEST FAIL: {label}: expected {want} finding(s), got {got}")
            ok = False
    return ok


def main(argv) -> int:
    if not selftest():
        return 2
    if "--selftest" in argv:
        print(f"[asm-lint] selftest OK ({len(SELFTEST)} samples)")
        return 0
    bad = []
    n_files = 0
    for p in sorted(SRC.rglob("*")):
        if p.suffix.lower() not in (".cpp", ".h", ".hpp", ".inc"):
            continue
        n_files += 1
        text = p.read_text(encoding="utf-8", errors="replace")
        for ln, src in findings_in(text, p.suffix.lower() == ".inc"):
            bad.append((p.relative_to(ROOT).as_posix(), ln, src))
    if bad:
        for f, ln, src in bad:
            print(f"[asm-lint] ERROR {f}:{ln}: bare absolute operand (assembles as an IMMEDIATE): {src}")
        print(f"[asm-lint] FAILED: {len(bad)} finding(s). Write the absolute address as `ds:[...]`, e.g. "
              f"`mov esi, dword ptr ds:[771A0Ch]`. Why: re/diag/KNOWN_ISSUES.md AUTOHOOK-INLINE-ASM-IMMEDIATE.")
        return 1
    print(f"[asm-lint] OK -- no bare absolute asm operand in {n_files} source files")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
