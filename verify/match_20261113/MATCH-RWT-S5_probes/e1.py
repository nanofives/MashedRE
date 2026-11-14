"""MATCH-RWT-S5 attempt 3: the volatile copy of param_1 moves to the entry block (a0), so the loop and tail
uses form param_1's own web, which c2 may spill by itself (reload per pass, div by memory)."""
BODY = r"""int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)
{
  int *piVar2;
  int iVar3;
  int *pn;
  int *key;
  uint idx;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint a0;
@DECL@
@TOP@
  idx = DAT_007d70c8;
  uVar7 = @SIZE@ * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
@IN@
    uVar6 = ((DAT_007d70cc[idx] + a0 - 1) / a0) * a0;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[DAT_007d70c8]) goto LAB_004dcc4c;
    uVar4 = 0;
    do {
      if (uVar4 != idx) {
@LOOP@
        uVar5 = uVar6 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
          idx = uVar4;
          DAT_007d70c8 = idx;
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < 4);
    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = idx;
    }
  }
  uVar6 = 0;
  uVar5 = uVar7;
LAB_004dcc4c:
  if (uVar7 > DAT_007d70dc[idx]) {
    key = DAT_007d70ec[idx];
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == key) {
        pn[1] = 0;
        pn[3] = 0;
        idx = DAT_007d70c8;
        break;
      }
    }
    DAT_007d70dc[idx] = uVar7;
    FUN_004dcaa0(uVar7,&DAT_007d70ec[idx]);
    idx = DAT_007d70c8;
  }
  piVar2 = DAT_007d70ec[idx];
  if (uVar6 == 0) {
    iVar3 = (*(vfn_t *)(*piVar2 + 0x2c))(piVar2,0,uVar7,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = uVar7;
    *param_5 = 0;
  }
  else {
    iVar3 = (*(vfn_t *)(*piVar2 + 0x2c))(piVar2,uVar6,uVar7,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = uVar5;
    *param_5 = uVar6 / @TAIL@;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return iVar3 >= 0;
}
"""
VOL = "*(volatile uint *)&param_1"
LP = "        uVar6 = ((DAT_007d70cc[uVar4] + param_1 - 1) / param_1) * param_1;"
LA = "        a = " + VOL + ";\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;"
TOPS = {
    "t": ("", "  a0 = " + VOL + ";", "a0", ""),          # volatile copy first statement, size from a0
    "tp": ("", "  a0 = " + VOL + ";", "param_1", ""),    # size from plain param_1
    "i": ("", "", "param_1", "    a0 = " + VOL + ";"),  # volatile copy inside the branch
    "c": ("", "  a0 = param_1;", "a0", ""),              # plain copy (control)
}
LOOPS = {"lp": LP, "la": LA}
TAILS = {"dp": "param_1", "da0": "a0"}
VARIANTS = {}
for tn, (decl, top, size, inn) in TOPS.items():
    for ln, loop in LOOPS.items():
        for dn, tail in TAILS.items():
            d = "  uint a;" if ln == "la" else ""
            b = (BODY.replace("@DECL@", d).replace("@TOP@", top).replace("@SIZE@", size).replace("@IN@", inn)
                 .replace("@LOOP@", loop).replace("@TAIL@", tail))
            VARIANTS[f"{tn}_{ln}_{dn}"] = b
