PRO = "int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)\n"

TAIL = r"""
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
  vb = DAT_007d70ec[DAT_007d70c8];
  if (offset == 0) {
    hr = (*(vfn_t *)(*vb + 0x2c))(vb,0,size,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = size;
    *param_5 = 0;
  }
  else {
    hr = (*(vfn_t *)(*vb + 0x2c))(vb,offset,size,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = end;
    *param_5 = offset / param_1;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return hr >= 0;
}
"""

DECL = """{
  int *vb;
  int hr;
  int *pn;
  uint n;
  uint offset;
  uint end;
  uint size;
"""

# g1: global used directly everywhere, goto structure of the current source
G1 = PRO + DECL + r"""
  size = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    offset = ((DAT_007d70cc[DAT_007d70c8] + param_1 - 1) / param_1) * param_1;
    end = offset + size;
    if (end <= DAT_007d70dc[DAT_007d70c8]) goto found;
    for (n = 0; n < 4; n++) {
      if (n != DAT_007d70c8) {
        offset = ((DAT_007d70cc[n] + param_1 - 1) / param_1) * param_1;
        end = offset + size;
        if (end <= DAT_007d70dc[n]) {
          DAT_007d70c8 = n;
          break;
        }
      }
    }
    if (n < 4) goto found;
    DAT_007d70c8 = DAT_007d70c8 + 1;
    if (DAT_007d70c8 >= 4) {
      DAT_007d70c8 = 0;
    }
  }
  offset = 0;
  end = size;
found:
""" + TAIL

# g2: no goto, if/else blocks
G2 = PRO + DECL + r"""
  size = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    offset = ((DAT_007d70cc[DAT_007d70c8] + param_1 - 1) / param_1) * param_1;
    end = offset + size;
    if (end > DAT_007d70dc[DAT_007d70c8]) {
      for (n = 0; n < 4; n++) {
        if (n != DAT_007d70c8) {
          offset = ((DAT_007d70cc[n] + param_1 - 1) / param_1) * param_1;
          end = offset + size;
          if (end <= DAT_007d70dc[n]) {
            DAT_007d70c8 = n;
            break;
          }
        }
      }
      if (n >= 4) {
        DAT_007d70c8 = DAT_007d70c8 + 1;
        if (DAT_007d70c8 >= 4) {
          DAT_007d70c8 = 0;
        }
        offset = 0;
        end = size;
      }
    }
  }
  else {
    offset = 0;
    end = size;
  }
""" + TAIL

# g3: like g1 but ++ form
G3 = G1.replace("DAT_007d70c8 = DAT_007d70c8 + 1;\n    if (DAT_007d70c8 >= 4)", "if (++DAT_007d70c8 >= 4)")

VARIANTS = {"g1": G1, "g2": G2, "g3": G3}
