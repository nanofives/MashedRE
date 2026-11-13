"""S1B attempt 2: the full function with DAT_00618258 declared as an 8-aligned type (Mat16A), loop forms x toolchains
x flags. usage: py -3.12 v14.py [show <name> <tc> <flags...>]"""
import pathlib, sys, re
WT = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-RWT-S1B")
sys.path.insert(0, str(WT / "re/tools/match"))
import match
HERE = pathlib.Path(__file__).parent
SRC = (WT / "re/match/s20/004cb4c0.c").read_text()
head, rest = SRC.split("// MATCH: 0x004cb4c0 FUN_004cb4c0\n", 1)
head = re.sub(r"^// FLAGS:.*\n", "", head)
ALIGN = {
    "a8": "typedef __declspec(align(8)) struct { int m[16]; } Mat16A;\nMGLOBAL(Mat16A, DAT_00618258, 0x00618258);\n",
    "a16": "typedef __declspec(align(16)) struct { int m[16]; } Mat16A;\nMGLOBAL(Mat16A, DAT_00618258, 0x00618258);\n",
}
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
    *(Mat16A *)cache = DAT_00618258;
    r = (*(setxf_fn *)(*DAT_007d4110 + 0xb0))(DAT_007d4110, 0x100, (Mat16 *)&DAT_00618258);
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
}


def build(al, lp):
    old = "MGLOBAL(Mat16, DAT_00618258, 0x00618258);\n"
    assert old in head
    return head.replace(old, ALIGN[al]) + "// MATCH: 0x004cb4c0 FUN_004cb4c0\n" + T.replace("@@LOOP@@", LOOPS[lp])


if len(sys.argv) > 1 and sys.argv[1] == "show":
    al, lp = sys.argv[2].split("_", 1)
    tc, fl = sys.argv[3], " ".join(sys.argv[4:])
    p = HERE / f"w_{al}_{lp}.c"
    p.write_text(f"// TOOLCHAIN {tc}\n" + build(al, lp))
    match.compare(p, fl, show=True, toolchain=tc, bytes_mode=True)
    sys.exit()
for al in ALIGN:
    for lp in LOOPS:
        for tc in ("rw9044", "ddk"):
            for fl in ("/O2 /Ob1 /G5", "/O2 /Ob1 /G5 /Oy-"):
                p = HERE / f"w_{al}_{lp}.c"
                p.write_text(f"// TOOLCHAIN {tc}\n" + build(al, lp))
                try:
                    r = match.compare(p, fl, quiet=True, toolchain=tc, bytes_mode=True)[0]
                except SystemExit as e:
                    print(f"{al}_{lp:6} {tc:7} {fl:20} FAIL {str(e)[:300]}"); continue
                print(f"{al}_{lp:6} {tc:7} {fl:20} {100*r['acc']:5.1f} regs {100*r['regs']:5.1f} ops {100*r['ops']:5.1f} "
                      f"{r['n_mine']}/{r['n_orig']} badbytes {len(r['bytes_bad'])}", flush=True)
