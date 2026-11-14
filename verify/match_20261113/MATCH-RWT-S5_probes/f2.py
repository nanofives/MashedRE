"""MATCH-RWT-S5 Fable attempt, step 2: global index used directly + end variable + entry copy of param_1 (a0).
a0 forms: volatile read (t), plain copy (c), none (n). key: local before the pn loop (k) or inline (i)."""
BODY = r"""int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)
{
  int *vb;
  int hr;
  int *pn;
  uint n;
  uint off;
  uint size;
  uint end;
  uint a0;

@TOP@
  size = @A@ * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    off = ((DAT_007d70cc[DAT_007d70c8] + @A@ - 1) / @A@) * @A@;
    end = off + size;
    if (end > DAT_007d70dc[DAT_007d70c8]) {
      for (n = 0; n < 4; n++) {
        if (n != DAT_007d70c8) {
          off = ((DAT_007d70cc[n] + param_1 - 1) / param_1) * param_1;
          end = off + size;
          if (end <= DAT_007d70dc[n]) {
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
        end = size;
      }
    }
  }
  else {
    off = 0;
    end = size;
  }
  if (size > DAT_007d70dc[DAT_007d70c8]) {
@KEY@
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == @K@) {
        pn[1] = 0;
        pn[3] = 0;
        break;
      }
    }
    DAT_007d70dc[DAT_007d70c8] = size;
    FUN_004dcaa0(size,&DAT_007d70ec[DAT_007d70c8]);
  }
  vb = DAT_007d70ec[DAT_007d70c8];
  if (off == 0) {
    hr = (*(vfn_t *)(*vb + 0x2c))(vb,0,size,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = size;
    *param_5 = 0;
  }
  else {
    hr = (*(vfn_t *)(*vb + 0x2c))(vb,off,size,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = end;
    *param_5 = off / param_1;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return hr >= 0;
}
"""
VARIANTS = {}
for an, (top, a) in {
    "t": ("  a0 = *(volatile uint *)&param_1;", "a0"),
    "c": ("  a0 = param_1;", "a0"),
    "n": ("", "param_1"),
}.items():
    for kn, (key, k) in {
        "k": ("    vb = DAT_007d70ec[DAT_007d70c8];", "vb"),
        "i": ("", "DAT_007d70ec[DAT_007d70c8]"),
    }.items():
        VARIANTS[an + kn] = (BODY.replace("@TOP@", top).replace("@A@", a).replace("@KEY@", key).replace("@K@", k))
