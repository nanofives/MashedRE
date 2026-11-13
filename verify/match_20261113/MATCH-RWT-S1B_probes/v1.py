T = r'''int FUN_004cb4c0(int *param_1)
{
  int i;
  long r;
  if (param_1 != 0 && (param_1[3] & 0x20000) == 0) {
    int *cache = DAT_007d4558->m;
    DAT_00618258.m[0] = param_1[0];
    DAT_00618258.m[1] = param_1[1];
    DAT_00618258.m[2] = param_1[2];
    DAT_00618258.m[4] = param_1[4];
    DAT_00618258.m[5] = param_1[5];
    DAT_00618258.m[6] = param_1[6];
    DAT_00618258.m[8] = param_1[8];
    DAT_00618258.m[9] = param_1[9];
    DAT_00618258.m[10] = param_1[10];
    DAT_00618258.m[12] = param_1[12];
    DAT_00618258.m[13] = param_1[13];
    DAT_00618258.m[14] = param_1[14];
    if (DAT_007d4154 == 0) {
@@LOOP@@
    }
set:
    *(Mat16 *)cache = DAT_00618258;
    r = (*(setxf_fn *)(*DAT_007d4110 + 0xb0))(DAT_007d4110, 0x100, &DAT_00618258);
    DAT_007d4154 = 0;
    return r >= 0;
  }
  if (DAT_007d4154 == 0) {
    *DAT_007d4558 = DAT_005d8b90;
    r = (*(setxf_fn *)(*DAT_007d4110 + 0xb0))(DAT_007d4110, 0x100, &DAT_005d8b90);
    DAT_007d4154 = 1;
    return r >= 0;
  }
  return 1;
}
'''
LOOPS = {
 "idx": "      for (i = 0; i < 16; i++) { if (DAT_00618258.m[i] != cache[i]) goto set; }\n      return 1;",
 "idxr": "      for (i = 0; i < 16; i++) { if (cache[i] != DAT_00618258.m[i]) goto set; }\n      return 1;",
 "dn": "      { const int *p = DAT_00618258.m, *q = cache; int n = 16;\n      do { if (*q != *p) goto set; p++; q++; } while (--n); }\n      return 1;",
 "dn2": "      { const int *p = DAT_00618258.m, *q = cache; int n = 16;\n      do { if (*p != *q) goto set; p++; q++; } while (--n); }\n      return 1;",
 "dnw": "      { const int *p = DAT_00618258.m, *q = cache; int n = 16;\n      while (n--) { if (*q++ != *p++) goto set; } }\n      return 1;",
 "dni": "      { int n = 16; i = 0;\n      do { if (cache[i] != DAT_00618258.m[i]) goto set; i++; } while (--n); }\n      return 1;",
}
VARIANTS = {k: T.replace("@@LOOP@@", v) for k, v in LOOPS.items()}
COMBOS = [(tc, fl) for tc in ("ddk", "rw9044") for fl in ("/O2 /Ob1 /G5 /Oy-", "/O2 /Ob1 /G6 /Oy-", "/O2 /Ob1 /Oy-", "/O2 /Ob1 /GB /Oy-")]
