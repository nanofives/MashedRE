"""One-off: make scripts/lint_inline_asm_abs.py mnemonic-aware (the first scan flagged C++ data in
Vehicle/CarMaterialTables_dirt.inc, which is a .inc but not asm)."""
import pathlib

p = pathlib.Path(__file__).resolve().parent.parent.parent / "scripts" / "lint_inline_asm_abs.py"
t = p.read_text(encoding="utf-8")
old_re = 'SEG = re.compile(r"(?:\\b[dcefgs]s\\s*:\\s*)$", re.I)\n'
assert old_re in t
new_re = old_re + '''# an asm line starts with a mnemonic (after an optional `label:`); a C++ line such as `body[0x1e] = 0` does not.
# Needed because some .inc files are C++ data tables, not asm (Vehicle/CarMaterialTables_dirt.inc).
MNEM = re.compile(
    r"^\\s*(?:[A-Za-z_]\\w*\\s*:\\s*)?(?:rep[a-z]*\\s+|lock\\s+)?"
    r"(?:mov|movzx|movsx|movss|movsd|movaps|movups|movq|movd|lea|xchg|push|pop|call|jmp|j[a-z]{1,3}|inc|dec|neg|not|"
    r"add|adc|sub|sbb|and|or|xor|cmp|test|imul|mul|idiv|div|shl|shr|sar|sal|rol|ror|cmov[a-z]+|set[a-z]+|"
    r"fld|fst|fstp|fild|fist|fistp|fadd|faddp|fiadd|fsub|fsubp|fsubr|fsubrp|fisub|fmul|fmulp|fimul|fdiv|fdivp|fdivr|"
    r"fdivrp|fcom|fcomp|fcompp|ficom|ficomp|fucom|fucomp|fldcw|fnstcw|fnstsw|fxch|"
    r"addss|subss|mulss|divss|addsd|subsd|mulsd|divsd|ucomiss|comiss|ucomisd|comisd|cvt[a-z0-9]+|"
    r"cmpsb|cmpsd|movsb|movsw|movsd|stosb|stosd|scasb|lodsb)\\b", re.I)
'''
t = t.replace(old_re, new_re)
old_loop = '''        code = code.split(";", 1)[0]                       # in asm text `;` starts a comment
        for b in BARE.finditer(code):'''
assert old_loop in t
new_loop = '''        code = code.split(";", 1)[0]                       # in asm text `;` starts a comment
        if not MNEM.match(code):                           # not an instruction line (C++ data in a .inc, a brace, ...)
            continue
        for b in BARE.finditer(code):'''
t = t.replace(old_loop, new_loop)
old_st = '''    ("        mov eax, dword ptr ds:[0x0066d6d8]  // 00426bb0\\n", True, 0, ".inc with ds: and a trailing comment"),
]'''
assert old_st in t
new_st = '''    ("        mov eax, dword ptr ds:[0x0066d6d8]  // 00426bb0\\n", True, 0, ".inc with ds: and a trailing comment"),
    ("    body[0x1e] = 0;\\n    body[0x00] = 2;\\n", True, 0, ".inc that is a C++ data table, not asm"),
    ("L_top:  mov esi, dword ptr [771A0Ch]\\n        rep movsd\\n", True, 1, "label prefix before the mnemonic"),
]'''
t = t.replace(old_st, new_st)
p.write_text(t, encoding="utf-8")
print("patched")
