// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ae800 (h3): 38.2% exact, regs 56.1%. Residual class (b) memset intrinsic + natural loops (38.2%); original uses blk+j indexing and a [esp+0x18] bits-left counter that ours strength-reduces. The permuter reached 63.6% only by turning (k + j*8) into (j + k*8), a different value, so it was rejected. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
uint FUN_005aea00(uint a, uint b);
#ifdef __cplusplus
extern "C" void *memset(void *, int, unsigned int);
#else
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
#endif
// MATCH: 0x005ae800 FUN_005ae800
uint FUN_005ae800(int *param_1,undefined4 param_2)
{
  uint res;
  int *blk;
  int *newblk;
  uint nbytes;
  uint j;
  uint k;
  uint left;
  byte b;
  int m;

  res = 0;
  nbytes = param_1[2];
  blk = (int *)param_1[4];
  if (blk != param_1 + 4) {
    do {
      if (res != 0) {
        return res;
      }
      left = param_1[1];
      j = 0;
      if (nbytes > 0) {
        do {
          b = *((byte *)blk + 8 + j);
          if (b == 0xff) {
            left = left - 8;
          }
          else {
            k = 0;
            if (left > 0) {
              do {
                m = 0x80 >> k;
                if ((b & m) == 0) {
                  *((byte *)blk + 8 + j) = (byte)(b | m);
                  res = (((uint)blk + nbytes + param_1[3] + 7) & ~(param_1[3] - 1U)) + (k + j * 8) * *param_1;
                  if (res != 0) goto done;
                  break;
                }
                k = k + 1;
                left = left - 1;
              } while (k < 8 && left > 0);
            }
          }
          j = j + 1;
        } while (j < nbytes);
      }
done:
      blk = (int *)*blk;
    } while (blk != param_1 + 4);
    if (res != 0) {
      return res;
    }
  }
  newblk = (int *)FUN_005aea00((uint)(param_1[1] * *param_1 + param_1[3] + 8 + nbytes),param_2);
  res = 0;
  if (newblk != 0) {
    memset(newblk + 2, 0, nbytes);
    *newblk = param_1[4];
    newblk[1] = (int)(param_1 + 4);
    *(int **)(param_1[4] + 4) = newblk;
    param_1[4] = (int)newblk;
    *(byte *)(newblk + 2) = 0x80;
    res = ((uint)newblk + nbytes + param_1[3] + 7) & ~(param_1[3] - 1U);
  }
  return res;
}
