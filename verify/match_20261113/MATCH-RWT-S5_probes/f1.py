"""MATCH-RWT-S5 Fable attempt, step 1: natural source, the global index DAT_007d70c8 used directly (no local copy),
end either a variable (E) or the expression lockOffset + lockSize at each site (X)."""
BODY = r"""int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)
{
  int *vb;
  int hr;
  int *pn;
  uint n;
  uint off;
  uint size;
@DECL@

  size = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    off = ((DAT_007d70cc[DAT_007d70c8] + param_1 - 1) / param_1) * param_1;
@E1@
    if (@C1@ > DAT_007d70dc[DAT_007d70c8]) {
      for (n = 0; n < 4; n++) {
        if (n != DAT_007d70c8) {
          off = ((DAT_007d70cc[n] + param_1 - 1) / param_1) * param_1;
@E2@
          if (@C1@ <= DAT_007d70dc[n]) {
            DAT_007d70c8 = n;
            break;
          }
        }
      }
      if (n >= 4) {
        DAT_007d70c8++;
        if (DAT_007d70c8 >= 4) {
          DAT_007d70c8 = 0;
        }
        off = 0;
@E3@
      }
    }
  }
  else {
    off = 0;
@E3@
  }
  if (size > DAT_007d70dc[DAT_007d70c8]) {
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == DAT_007d70ec[DAT_007d70c8]) {
        pn[1] = 0;
        pn[3] = 0;
        break;
      }
    }
    DAT_007d70dc[DAT_007d70c8] = size;
    FUN_004dcaa0(size,&DAT_007d70ec[DAT_007d70c8]);
  }
@VB@
  if (off == 0) {
    hr = (*(vfn_t *)(*@V@ + 0x2c))(@V@,@Z@,size,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = size;
    *param_5 = 0;
  }
  else {
    hr = (*(vfn_t *)(*@V@ + 0x2c))(@V@,off,size,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = @C1@;
    *param_5 = off / param_1;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return hr >= 0;
}
"""
VARIANTS = {}
for en, (decl, e1, e2, e3, c1) in {
    "E": ("  uint end;", "    end = off + size;", "          end = off + size;", "    end = size;", "end"),
    "X": ("", "", "", "", "off + size"),
}.items():
    for vn, (vb, v) in {
        "v": ("  vb = DAT_007d70ec[DAT_007d70c8];", "vb"),
        "g": ("", "DAT_007d70ec[DAT_007d70c8]"),
    }.items():
        for zn, z in {"z0": "0", "zo": "off"}.items():
            b = (BODY.replace("@DECL@", decl).replace("@E1@", e1).replace("@E2@", e2).replace("@E3@", e3)
                 .replace("@C1@", c1).replace("@VB@", vb).replace("@V@", v).replace("@Z@", z))
            VARIANTS[f"{en}{vn}{zn}"] = b
