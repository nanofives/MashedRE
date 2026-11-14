"""Writes v3.txt: outer form x mask-null form x try form for 0x004ce970 (MATCH-RWT-S6), try forms that fall through
to a shared `return image` (the original shares mov eax,esi at 0x004ceae1 between the success path and the
no-mask-name exit)."""
import os, itertools

F = "IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0;"
T = {
 'T4': "if (!FUN_004cde50(mask) || !FUN_004cdf20(image, mask)) { " + F + " }\nIMAGEDESTROY(mask);",
 'T5': "if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); }\nelse { " + F + " }",
 'Tb': ("if (FUN_004cde50(mask)) { if (FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); } else { " + F + " } }\n"
        "else { " + F + " }"),
 'Tc': ("if (FUN_004cde50(mask)) { if (!FUN_004cdf20(image, mask)) { " + F + " } }\nelse { " + F + " }\n"
        "IMAGEDESTROY(mask);"),
 'Td': ("if (!FUN_004cde50(mask)) { " + F + " }\nif (!FUN_004cdf20(image, mask)) { " + F + " }\nIMAGEDESTROY(mask);"),
 'Te': ("if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); }\n"
        "else { IMAGEDESTROY(image); IMAGEDESTROY(mask); image = 0; }"),
 'Tf': ("if (FUN_004cde50(mask)) { if (FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); return image; } }\n"
        + F),
}
M = {
 'M1': lambda t: "mask = FUN_004ce390(param_2);\nif (!mask) { IMAGEDESTROY(image); return 0; }\n" + t,
 'M3': lambda t: "mask = FUN_004ce390(param_2);\nif (mask) {\n" + t + "\n} else { IMAGEDESTROY(image); return 0; }",
 'M5': lambda t: "mask = FUN_004ce390(param_2);\nif (mask) {\n" + t + "\n} else { IMAGEDESTROY(image); image = 0; }",
}
O = {
 'O1': lambda m: "image = FUN_004ce390(param_1);\nif (image) {\nif (param_2 && *param_2) {\n" + m + "\n}\nreturn image;\n}\nreturn 0;",
 'O2': lambda m: "image = FUN_004ce390(param_1);\nif (!image) return 0;\nif (!param_2 || !*param_2) return image;\n" + m + "\nreturn image;",
 'O3': lambda m: "image = FUN_004ce390(param_1);\nif (image && param_2 && *param_2) {\n" + m + "\n}\nreturn image;",
 'O4': lambda m: "image = FUN_004ce390(param_1);\nif (image) {\nif (param_2 && *param_2) {\n" + m + "\n}\n}\nreturn image;",
 'O5': lambda m: "image = FUN_004ce390(param_1);\nif (!image) return 0;\nif (param_2 && *param_2) {\n" + m + "\n}\nreturn image;",
}
out = []
for (ok, of), (mk, mf), (tk, tv) in itertools.product(O.items(), M.items(), T.items()):
    body = of(mf(tv))
    out.append(f"@@ {ok}{mk}{tk}\n// MATCH: 0x004ce970 FUN_004ce970\nRwImage *FUN_004ce970(const char *param_1, const char *param_2)\n"
               f"{{\n  RwImage *image;\n  RwImage *mask;\n\n{body}\n}}\n")
open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'v3.txt'), 'w', encoding='utf-8').write('\n'.join(out))
print(len(out))
