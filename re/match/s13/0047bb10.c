// FLAGS: /O2 /GS /Ob1
/* 0x0047bb10 (per-record plane builder: for each edge i < b->ne that FUN_0047ba20 accepts, writes a plane via
   FUN_00477e60 to the next 16-byte slot and sums the chosen point; then one more plane via FUN_00477f00 from
   pt - mean(points) and the record's +0x138 vector; *count = planes written): best 79.8% (regs 87.4%), raw 27.7%.
   Done: BoxShape layout (n[12][3], p[8][3] @0x90, e[18][4] @0xf0, c[4] @0x138, ne @0x148, record 0x158 B),
   for loop guarded only by i < ne (jbe), running plane pointer cur = *pl++ before n++, pa + ib in the
   branches, avg[3] temp (frame 0x2c), cur = planes[n]; n++ before the last call, sum zeroing before i = 0
   (permuter). n[]/p[] vs one v[20][3] with +12 indexes compile the same. Remaining: pb address form
   (ours lea idx*3+0x24, original add 0xc / lea / lea) and the pa/ib register swap, operand order of the
   y/z sum adds, n+1 store after the push of &d. Class (b). */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct BoxShape {
  float n[12][3];         /* +0x000 */
  float p[8][3];          /* +0x090 */
  unsigned char e[18][4]; /* +0x0f0 */
  float c[4];             /* +0x138 */
  unsigned int ne;        /* +0x148 */
} BoxShape;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_00477e60(float *plane, float *pt, float *a, float *b);
void FUN_00477f00(float *plane, float *c, float *d);
int FUN_0047ba20(BoxShape *b, int e, float *pt, int *side);
// MATCH: 0x0047bb10 FUN_0047bb10
void FUN_0047bb10(BoxShape *b, float *pt, float (*planes)[4], int *count)
{
  float sum[3];
  float avg[3];
  float d[3];
  int n;
  int side;
  unsigned int i;
  float *pa;
  float *cur;
  float *pb;
  unsigned int ib;
  float (*pl)[4];
  float s;
  sum[0] = 0.0f;
  sum[1] = 0.0f;
  sum[2] = 0.0f;
  i = 0;
  n = 0;
  if (i < b->ne) {
    pl = planes;
    do {
      if (FUN_0047ba20(b, i, pt, &side)) {
        cur = *pl++;
        n++;
        if (side == 0) {
          pa = b->p[b->e[i][0]];
          ib = b->e[i][1];
        } else {
          pa = b->p[b->e[i][1]];
          ib = b->e[i][0];
        }
        pb = b->p[ib];
        sum[0] += pa[0];
        sum[1] += pa[1];
        sum[2] += pa[2];
        FUN_00477e60(cur, pt, pa, pb);
      }
      i++;
    } while (i < b->ne);
    if (n != 0) {
      s = DAT_005cc320 / (float)n;
      cur = planes[n];
      n++;
      avg[0] = sum[0] * s;
      avg[1] = sum[1] * s;
      avg[2] = sum[2] * s;
      d[0] = pt[0] - avg[0];
      d[1] = pt[1] - avg[1];
      d[2] = pt[2] - avg[2];
      FUN_00477f00(cur, b->c, d);
    }
  }
  if (count != 0) {
    *count = n;
  }
}
