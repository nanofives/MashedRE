"""Space 3: reload-temp forms x end/size weight forms x idx-ref forms (all semantics-preserving)."""
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
  uint a;

  idx = DAT_007d70c8;
  uVar7 = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
"""
FIRST = {
    "fi": """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;
""",
    "fg": """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[DAT_007d70c8]) goto LAB_004dcc4c;
""",
    "fy": """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    if (uVar6 + uVar7 <= DAT_007d70dc[DAT_007d70c8]) {
      uVar5 = uVar6 + uVar7;
      goto LAB_004dcc4c;
    }
""",
}
RELOAD = {
    "ra": ("        a = *(volatile uint *)&param_1;\n", "a"),
    "rp": ("        param_1 = *(volatile uint *)&param_1;\n", "param_1"),
    "re": ("        uVar5 = *(volatile uint *)&param_1;\n", "uVar5"),
}
LOOPEND = {
    "l0": """        uVar6 = ((DAT_007d70cc[uVar4] + @A - 1) / @A) * @A;
        uVar5 = uVar6 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
          idx = uVar4;
          DAT_007d70c8 = idx;
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
""",
    "l1": """        uVar6 = ((DAT_007d70cc[uVar4] + @A - 1) / @A) * @A;
        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {
          uVar5 = uVar6 + uVar7;
          idx = uVar4;
          DAT_007d70c8 = idx;
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
""",
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
"""
TAIL = {
    "t0": """    DAT_007d70dc[idx] = uVar7;
    FUN_004dcaa0(uVar7,&DAT_007d70ec[idx]);
    idx = DAT_007d70c8;
  }
  piVar2 = DAT_007d70ec[idx];
""",
    "t1": """    DAT_007d70dc[idx] = uVar7;
    FUN_004dcaa0(uVar7,&DAT_007d70ec[DAT_007d70c8]);
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
for f, r, l, t in itertools.product(FIRST, RELOAD, LOOPEND, TAIL):
    rl, an = RELOAD[r]
    loop = "    uVar4 = 0;\n    do {\n      if (uVar4 != idx) {\n" + rl + LOOPEND[l].replace("@A", an)
    if r == "re" and l == "l1":
        continue
    VARIANTS[f"{f}{r}{l}{t}"] = PRO + FIRST[f] + loop + MID + TAIL[t] + END
