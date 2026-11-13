"""Writes v2.txt: outer form x mask-null form x try form for 0x004ce970 (MATCH-RWT-S6)."""
import os, itertools

T = {
 'T1': ("if (!FUN_004cde50(mask) || !FUN_004cdf20(image, mask)) { IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0; }\n"
        "IMAGEDESTROY(mask); return image;"),
 'T2': ("if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); return image; }\n"
        "IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0;"),
 'T3': ("if (FUN_004cde50(mask)) { if (FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); return image; } }\n"
        "IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0;"),
 'T6': ("if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); return image; }\n"
        "else { IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0; }"),
 'T7': ("if (!FUN_004cde50(mask)) goto fail; if (!FUN_004cdf20(image, mask)) goto fail;\n"
        "IMAGEDESTROY(mask); return image;\nfail: IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0;"),
 'T9': ("if (!FUN_004cde50(mask) || !FUN_004cdf20(image, mask)) { IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0; }\n"
        "else { IMAGEDESTROY(mask); return image; }"),
 'Ta': ("if (FUN_004cde50(mask)) { if (FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); return image; } "
        "IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0; }\nIMAGEDESTROY(image); IMAGEDESTROY(mask); return 0;"),
}
M = {
 'M1': lambda t: "mask = FUN_004ce390(param_2);\nif (!mask) { IMAGEDESTROY(image); return 0; }\n" + t,
 'M2': lambda t: "mask = FUN_004ce390(param_2);\nif (mask) {\n" + t + "\n}\nIMAGEDESTROY(image); return 0;",
 'M3': lambda t: "mask = FUN_004ce390(param_2);\nif (mask) {\n" + t + "\n} else { IMAGEDESTROY(image); return 0; }",
 'M4': lambda t: "if ((mask = FUN_004ce390(param_2)) == 0) { IMAGEDESTROY(image); return 0; }\n" + t,
}
O = {
 'O1': lambda m: "image = FUN_004ce390(param_1);\nif (image) {\nif (param_2 && *param_2) {\n" + m + "\n}\nreturn image;\n}\nreturn 0;",
 'O2': lambda m: "image = FUN_004ce390(param_1);\nif (!image) return 0;\nif (!param_2 || !*param_2) return image;\n" + m,
 'O3': lambda m: "image = FUN_004ce390(param_1);\nif (image && param_2 && *param_2) {\n" + m + "\n}\nreturn image;",
 'O4': lambda m: "image = FUN_004ce390(param_1);\nif (image) {\nif (param_2 && *param_2) {\n" + m + "\n}\n}\nreturn image;",
 'O5': lambda m: "image = FUN_004ce390(param_1);\nif (!image) return 0;\nif (param_2 && *param_2) {\n" + m + "\n}\nreturn image;",
}
out = []
for (ok, of), (mk, mf), (tk, tv) in itertools.product(O.items(), M.items(), T.items()):
    body = of(mf(tv))
    out.append(f"@@ {ok}{mk}{tk}\n// MATCH: 0x004ce970 FUN_004ce970\nRwImage *FUN_004ce970(const char *param_1, const char *param_2)\n"
               f"{{\n  RwImage *image;\n  RwImage *mask;\n\n{body}\n}}\n")
open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'v2.txt'), 'w', encoding='utf-8').write('\n'.join(out))
print(len(out))
