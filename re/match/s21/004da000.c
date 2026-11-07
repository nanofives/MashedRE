// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004da000 (h4): 91.0% (regs 91.0%, ops 100%). Rewritten from odis (raw 24.3%): Box copy, binary search on box->hi[ch], Stats {6 floats, flag} zeroed before FUN_004d9d00, rest = total - s, a mean-difference term over two V4 locals, then the s0==eps / r0==eps / split-score branches in that order. Every instruction has the original's opcode and operands. The 8 mismatches are x87 operand order in the mean products: ours fld st(i); fmul m, the original fld m; fmul st(i). Same with is*s.v[k], s.v[k]*is, and *= forms. S19 x87 class, so no permuter run. Class (a). */
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
typedef struct Stats { float v[6]; unsigned char flag; } Stats;
typedef struct Box { int lo[4]; int hi[4]; } Box;
typedef struct V4 { float x, y, z, w; } V4;
MGLOBAL(int, DAT_00618430, 0x00618430);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_004d9d00(void *ctx, int *org, int bits, Box *box, Stats *out);
// MATCH: 0x004da000 FUN_004da000
float FUN_004da000(void *ctx, Box *box, int ch, int *out, Stats *total)
{
  float best = total->v[5];
  Box b = *box;
  int hi = box->hi[ch];
  int lo = box->lo[ch];
  if (hi - lo > 1) {
    int org[4];
    org[0] = 0;
    org[1] = 0;
    org[2] = 0;
    org[3] = 0;
    do {
      int mid = lo + ((hi - lo) >> 1);
      Stats s, r;
      V4 ms, md;
      b.hi[ch] = mid;
      s.flag = 0;
      s.v[0] = 0.0f;
      s.v[1] = 0.0f;
      s.v[2] = 0.0f;
      s.v[3] = 0.0f;
      s.v[4] = 0.0f;
      s.v[5] = 0.0f;
      FUN_004d9d00(ctx, org, DAT_00618430, &b, &s);
      r.v[0] = total->v[0] - s.v[0];
      r.v[1] = total->v[1] - s.v[1];
      r.v[2] = total->v[2] - s.v[2];
      r.v[3] = total->v[3] - s.v[3];
      r.v[4] = total->v[4] - s.v[4];
      r.v[5] = total->v[5] - s.v[5];
      if (r.v[0] > DAT_005d757c && s.v[0] > DAT_005d757c) {
        float is = DAT_005cc320 / s.v[0];
        float id = DAT_005cc320 / r.v[0];
        ms.x = s.v[1] * is;
        ms.y = s.v[2] * is;
        ms.z = s.v[3] * is;
        ms.w = s.v[4] * is;
        md.x = r.v[1] * id;
        md.y = r.v[2] * id;
        md.z = r.v[3] * id;
        md.w = r.v[4] * id;
        ms.x -= md.x;
        ms.y -= md.y;
        ms.z -= md.z;
        ms.w -= md.w;
        r.v[5] -= (ms.x * ms.x + ms.y * ms.y + ms.z * ms.z + ms.w * ms.w) / (id + is);
      }
      if (s.v[0] == DAT_005d757c) {
        lo = mid;
      }
      else if (r.v[0] == DAT_005d757c) {
        hi = mid;
      }
      else {
        float t = s.v[5] + r.v[5];
        if (t < best) {
          best = t;
          out[ch] = b.hi[ch];
        }
        if (s.v[5] < r.v[5]) lo = mid;
        else hi = mid;
      }
    } while (hi - lo > 1);
  }
  return best;
}
