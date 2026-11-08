// FLAGS: /O1
/* MATCH-S22 0x004fb5a0 (h7): best 74.8% (regs 82.7%, ops 94.2%) under /O1 (raw converter 5.7% under /G5). Flags
   are the main finding: this __stdcall 12-arg matrix-palette skinning loop is NOT built like its /O2 /Ob1 /G5 /Oy-
   neighbours. /O2 /G5 gives fild qword for the unsigned->float weight split and splits the read-modify-write adds,
   /O2 /G6 gives fild dword + 2^32 fixup but also splits RMW adds and pads the loop; only /O1 (= /Os /Og /Oy /Ob1
   /Gs /GF /Gy; it keeps the ebp frame here) gives fild dword + fixup, test ah,1, RMW add [m],ebx and the movsd
   struct copies together: 15.4% -> 76.2% from the flag alone. Shape: one path for numN == 1 and one general path,
   V3 struct copies, memset/memcpy intrinsics, while (*w >= DAT_005d757c) weight loop. The permuter reached 80.6%
   only through semantics-changing moves (call before the index shift, a multiply after its use); rejected.
   Residual class (b): esi/edi swapped between the weight pointer and the matrix offset in both paths, the path-2
   temporaries at the other stack slot (-0x1c/-0x28), fldz;fstp zero stores where ours stores mov 0 (no form
   tried, including chained assignment, an __inline zero helper and (float)0, gave fldz). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
void *memset(void *d, int c, unsigned int n);
void *memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memset, memcpy)
typedef struct { float x, y, z; } V3_4fb5a0;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void * __stdcall FUN_004ecf29(V3_4fb5a0 *out, void *in, void *m);
void * __stdcall FUN_004ecf53(V3_4fb5a0 *out, void *in, void *m);
// MATCH: 0x004fb5a0 FUN_004fb5a0
void __stdcall FUN_004fb5a0(char *mats, char *nmats, char *srcPos, char *dstPos, unsigned int numVerts,
                            int stride, char **srcN, char **dstN, unsigned int numN, int offset,
                            float *weights, int order)
{
  V3_4fb5a0 nacc[65];
  unsigned int nsize;
  unsigned int count;
  char *nmat;
  V3_4fb5a0 tn;
  V3_4fb5a0 tp;
  V3_4fb5a0 pacc;
  char *srcExtra;
  unsigned int idx;
  unsigned int k;
  float w;
  int i2;
  char *dstExtra;
  unsigned int extra;

  srcExtra = srcPos + offset;
  dstExtra = dstPos + offset;
  extra = stride - offset;
  if (nmats == 0) {
    nmats = mats;
  }
  if (numN == 1) {
    for (count = numVerts; count > 0; count--) {
      pacc.x = 0.0f;
      pacc.y = 0.0f;
      pacc.z = 0.0f;
      nacc[0].x = 0.0f;
      nacc[0].y = 0.0f;
      nacc[0].z = 0.0f;
      while (*weights >= DAT_005d757c) {
        idx = (unsigned int)*weights >> 1;
        w = *weights - (float)(idx * 2);
        idx <<= 6;
        FUN_004ecf29(&tp, srcPos, mats + idx);
        FUN_004ecf53(&tn, *srcN, nmats + idx);
        weights++;
        tp.x *= w;
        tp.y *= w;
        tp.z *= w;
        tn.x *= w;
        tn.y *= w;
        tn.z *= w;
        pacc.x += tp.x;
        pacc.y += tp.y;
        pacc.z += tp.z;
        nacc[0].x += tn.x;
        nacc[0].y += tn.y;
        nacc[0].z += tn.z;
      }
      if (order != 0) {
        memcpy(dstExtra, srcExtra, extra);
        *(V3_4fb5a0 *)dstPos = pacc;
        *(V3_4fb5a0 *)*dstN = nacc[0];
      } else {
        *(V3_4fb5a0 *)dstPos = pacc;
        *(V3_4fb5a0 *)*dstN = nacc[0];
        memcpy(dstExtra, srcExtra, extra);
      }
      *dstN += stride;
      dstExtra += stride;
      srcExtra += stride;
      dstPos += stride;
      srcPos += stride;
      *srcN += stride;
      weights++;
    }
  } else if (numVerts > 0) {
    nsize = numN * sizeof(V3_4fb5a0);
    count = numVerts;
    do {
      pacc.x = 0.0f;
      pacc.y = 0.0f;
      memset(nacc, 0, nsize);
      pacc.z = 0.0f;
      while (*weights >= DAT_005d757c) {
        idx = (unsigned int)*weights >> 1;
        w = *weights - (float)(idx * 2);
        idx <<= 6;
        FUN_004ecf29(&tp, srcPos, mats + idx);
        tp.x *= w;
        tp.y *= w;
        tp.z *= w;
        pacc.x += tp.x;
        pacc.y += tp.y;
        pacc.z += tp.z;
        nmat = nmats + idx;
        for (k = 0; k < numN; k++) {
          FUN_004ecf53(&tn, srcN[k], nmat);
          tn.y *= w;
          tn.x *= w;
          tn.z *= w;
          nacc[k].x += tn.x;
          nacc[k].y += tn.y;
          nacc[k].z += tn.z;
        }
        weights++;
      }
      if (order != 0) {
        memcpy(dstExtra, srcExtra, extra);
        *(V3_4fb5a0 *)dstPos = pacc;
        for (k = 0; k < numN; k++) {
          *(V3_4fb5a0 *)dstN[k] = nacc[k];
          dstN[k] += stride;
          srcN[k] += stride;
        }
      } else {
        *(V3_4fb5a0 *)dstPos = pacc;
        for (k = 0; k < numN; k++) {
          *(V3_4fb5a0 *)dstN[k] = nacc[k];
          dstN[k] += stride;
          srcN[k] += stride;
        }
        memcpy(dstExtra, srcExtra, extra);
      }
      dstExtra += stride;
      srcExtra += stride;
      dstPos += stride;
      srcPos += stride;
      weights++;
    } while (--count != 0);
  }
}
