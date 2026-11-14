"""Design space over semantics-preserving source choices for 0x004dcbb0 (all variants compute the same function)."""
import itertools

PRO = """int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)
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

  idx = DAT_007d70c8;
  uVar7 = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
"""
FIRST = {
    "f0": """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;
""",
    "f1": """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    if (uVar6 + uVar7 <= DAT_007d70dc[idx]) {
      uVar5 = uVar6 + uVar7;
      goto LAB_004dcc4c;
    }
""",
}
LOOPHEAD = """    uVar4 = 0;
    do {
      if (uVar4 != idx) {
        uint a = *(volatile uint *)&param_1;
        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;
"""
LOOPEND = {
    "l0": """        uVar5 = uVar6 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
FOUND
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
""",
    "l1": """        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {
          uVar5 = uVar6 + uVar7;
FOUND
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
""",
    "l2": """        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {
FOUND
          uVar5 = uVar6 + uVar7;
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
""",
}
FOUND = {
    "d0": "          idx = uVar4;\n          DAT_007d70c8 = idx;",
    "d1": "          DAT_007d70c8 = uVar4;\n          idx = uVar4;",
    "d2": "          idx = uVar4;\n          DAT_007d70c8 = uVar4;",
}
MID = """      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < 4);
    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = idx;
    }
  }
"""
NF = {"n0": "  uVar6 = 0;\n  uVar5 = uVar7;\n", "n1": "  uVar5 = uVar7;\n  uVar6 = 0;\n"}
INNER = {
    "i0": """LAB_004dcc4c:
  if (uVar7 > DAT_007d70dc[idx]) {
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == DAT_007d70ec[idx]) {
        pn[1] = 0;
        pn[3] = 0;
        idx = DAT_007d70c8;
        break;
      }
    }
""",
    "i1": """LAB_004dcc4c:
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
""",
    "i2": """LAB_004dcc4c:
  if (uVar7 > DAT_007d70dc[idx]) {
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == DAT_007d70ec[idx]) {
        pn[1] = 0;
        pn[3] = 0;
        break;
      }
    }
    idx = DAT_007d70c8;
""",
}
TAIL = {
    "t0": """    DAT_007d70dc[idx] = uVar7;
    FUN_004dcaa0(uVar7,&DAT_007d70ec[idx]);
    idx = DAT_007d70c8;
  }
  piVar2 = DAT_007d70ec[idx];
""",
}
END = """  if (uVar6 == 0) {
    iVar3 = (*(vfn_t *)(*piVar2 + 0x2c))(piVar2,0,uVar7,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = uVar7;
    *param_5 = 0;
  }
  else {
    iVar3 = (*(vfn_t *)(*piVar2 + 0x2c))(piVar2,uVar6,uVar7,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = uVar5;
    *param_5 = uVar6 / *(volatile uint *)&param_1;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return iVar3 >= 0;
}
"""
VARIANTS = {}
for f, l, d, n, i, t in itertools.product(FIRST, LOOPEND, FOUND, NF, INNER, TAIL):
    body = PRO + FIRST[f] + LOOPHEAD + LOOPEND[l].replace("FOUND", FOUND[d]) + MID + NF[n] + INNER[i] + TAIL[t] + END
    VARIANTS[f"{f}{l}{d}{n}{i}{t}"] = body
