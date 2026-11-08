// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR06 2026-11-08: written from the original disassembly (Ghidra-missed, no pc_funcs.csv row). 99.1%: the posStride reload (mov ebp,[esp+0x14]) is scheduled 3 slots later than in the original; vertex expressions written p + (a - b) keep the original fchs/operand order. */
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
/* Ghidra-missed (no pc_funcs.csv row). Original body 0x00536e90..0x00536fda (330 B, ret then 90 padding).
   Selected at 0x00536840 (stored to +0x28). Per count step: w = size[0] * 0.5 (0x005cc32c), h = size[1] * 0.5,
   r = w * (*param_2), u = h * (*param_3), and 4 vertices at param_1[0] (stride param_1[1]):
   pos - r - u, pos + r - u, pos + u + r, pos + u - r. pos/size advance by their strides from param_5. */
typedef struct { float x, y, z; } V3;
typedef struct { unsigned char *ptr; int stride; } VtxIter;
typedef struct { int f0; unsigned char *pos; int posStride; int pad[4]; unsigned char *size; int sizeStride; } PrtArr;
// MATCH: 0x00536e90 FUN_00536e90
void FUN_00536e90(VtxIter *param_1, V3 *param_2, V3 *param_3, int param_4, PrtArr *param_5)
{
  unsigned char *pos;
  int pstride;
  float *size;
  int sstride;
  V3 *v;
  int vstride;
  float w, h;
  V3 r, u;
  V3 *p;

  pos = param_5->pos;
  pstride = param_5->posStride;
  size = (float *)param_5->size;
  sstride = param_5->sizeStride;
  v = (V3 *)param_1->ptr;
  vstride = param_1->stride;
  for (; param_4 > 0; param_4--) {
    w = size[0] * 0.5f;
    r.x = w * param_2->x;
    r.y = w * param_2->y;
    r.z = w * param_2->z;
    h = size[1] * 0.5f;
    size = (float *)((unsigned char *)size + sstride);
    u.x = h * param_3->x;
    u.y = h * param_3->y;
    u.z = h * param_3->z;
    p = (V3 *)pos;
    v->x = p->x + (-r.x - u.x);
    v->y = p->y + (-r.y - u.y);
    v->z = p->z + (-r.z - u.z);
    v = (V3 *)((unsigned char *)v + vstride);
    v->x = r.x - u.x + p->x;
    v->y = r.y - u.y + p->y;
    v->z = r.z - u.z + p->z;
    v = (V3 *)((unsigned char *)v + vstride);
    v->x = p->x + (u.x + r.x);
    v->y = p->y + (u.y + r.y);
    v->z = p->z + (u.z + r.z);
    v = (V3 *)((unsigned char *)v + vstride);
    v->x = p->x + (u.x - r.x);
    v->y = p->y + (u.y - r.y);
    v->z = p->z + (u.z - r.z);
    pos += pstride;
    v = (V3 *)((unsigned char *)v + vstride);
  }
}
