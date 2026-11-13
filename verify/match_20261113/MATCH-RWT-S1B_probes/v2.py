exec(open("v1.py").read().split("LOOPS = {")[0])
LOOPS = {
 "vg_idx": "      for (i = 0; i < 16; i++) { if (((volatile int *)DAT_00618258.m)[i] != cache[i]) goto set; }\n      return 1;",
 "vc_idx": "      for (i = 0; i < 16; i++) { if (DAT_00618258.m[i] != ((volatile int *)cache)[i]) goto set; }\n      return 1;",
 "vg_dn": "      { const volatile int *p = DAT_00618258.m; const int *q = cache; int n = 16;\n      do { if (*q != *p) goto set; p++; q++; } while (--n); }\n      return 1;",
 "vc_dn": "      { const int *p = DAT_00618258.m; const volatile int *q = cache; int n = 16;\n      do { if (*q != *p) goto set; p++; q++; } while (--n); }\n      return 1;",
 "vc_dn2": "      { const int *p = DAT_00618258.m; const volatile int *q = cache; int n = 16;\n      do { if (*p != *q) goto set; p++; q++; } while (--n); }\n      return 1;",
 "vg_dn2": "      { const volatile int *p = DAT_00618258.m; const int *q = cache; int n = 16;\n      do { if (*p != *q) goto set; p++; q++; } while (--n); }\n      return 1;",
}
VARIANTS = {k: T.replace("@@LOOP@@", v) for k, v in LOOPS.items()}
COMBOS = [(tc, "/O2 /Ob1 /G5 /Oy-") for tc in ("ddk", "rw9044")]
