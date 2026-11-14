PRO = "int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)\n"
DECL = """{
  int *vb;
  int hr;
  int *pn;
  uint cur;
  uint n;
  uint offset;
  uint end;
  uint size;
"""
TAIL = r"""
  if (size > DAT_007d70dc[cur]) {
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == DAT_007d70ec[cur]) {
        pn[1] = 0;
        pn[3] = 0;
        break;
      }
    }
    cur = DAT_007d70c8;
    DAT_007d70dc[cur] = size;
    FUN_004dcaa0(size,&DAT_007d70ec[cur]);
    cur = DAT_007d70c8;
  }
  vb = DAT_007d70ec[cur];
  if (offset == 0) {
    hr = (*(vfn_t *)(*vb + 0x2c))(vb,0,size,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = size;
    *param_5 = 0;
  }
  else {
    hr = (*(vfn_t *)(*vb + 0x2c))(vb,offset,size,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = end;
    *param_5 = offset / *(volatile uint *)&param_1;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return hr >= 0;
}
"""
LOOPBODY = """        uint a = *(volatile uint *)&param_1;
        offset = ((DAT_007d70cc[n] + a - 1) / a) * a;
        end = offset + size;"""

# h1: g2 structure, local cur
H1 = PRO + DECL + r"""
  cur = DAT_007d70c8;
  size = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    offset = ((DAT_007d70cc[cur] + param_1 - 1) / param_1) * param_1;
    end = offset + size;
    if (end > DAT_007d70dc[cur]) {
      for (n = 0; n < 4; n++) {
        if (n != cur) {
LOOPBODY
          if (end <= DAT_007d70dc[n]) {
            cur = n;
            DAT_007d70c8 = cur;
            break;
          }
        }
      }
      if (n >= 4) {
        cur = cur + 1;
        DAT_007d70c8 = cur;
        if (cur >= 4) {
          cur = 0;
          DAT_007d70c8 = cur;
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
""".replace("LOOPBODY\n", LOOPBODY + "\n") + TAIL

# h2: h1 with goto-shaped found exit like the current source
H2 = PRO + DECL + r"""
  cur = DAT_007d70c8;
  size = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    offset = ((DAT_007d70cc[cur] + param_1 - 1) / param_1) * param_1;
    end = offset + size;
    if (end <= DAT_007d70dc[cur]) goto done;
    for (n = 0; n < 4; n++) {
      if (n != cur) {
LOOPBODY
        if (end <= DAT_007d70dc[n]) {
          cur = n;
          DAT_007d70c8 = cur;
          break;
        }
      }
    }
    if (n < 4) goto done;
    cur = cur + 1;
    DAT_007d70c8 = cur;
    if (cur >= 4) {
      cur = 0;
      DAT_007d70c8 = cur;
    }
  }
  offset = 0;
  end = size;
done:
""".replace("LOOPBODY\n", LOOPBODY + "\n") + TAIL

VARIANTS = {"h1": H1, "h2": H2}
