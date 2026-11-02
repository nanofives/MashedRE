// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x00485e50: 100.0% (regs 100%). Raw g2s 17.8%. Same source as re/match/s14/00485c20.c:
   holds the static register-arg callee 0x00485c20 (EAX idx, ECX obj, ESI out), defined first. BoneRec struct
   (stride 0x23c, 4 records) at 0x006fde10 replaces the six flat globals; the 5-entry fill is 'i = 5; while (i-- != 0)'.
   relocheck OK. */
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
typedef struct MVec3 {
  float x;
  float y;
  float z;
} MVec3;

typedef struct MMatrix {
  MVec3 right;
  undefined4 flags;
  MVec3 up;
  undefined4 pad1;
  MVec3 at;
  undefined4 pad2;
  MVec3 pos;
  undefined4 pad3;
} MMatrix;

void FUN_0041f1e0(int obj, int id, MMatrix *out);

// MATCH: 0x00485c20 FUN_00485c20
static int FUN_00485c20(int obj, int idx, MVec3 *out)
{
  MMatrix m;

  if (idx == 0) {
    FUN_0041f1e0(obj, 0x21, &m);
    *out = m.pos;
    return 0;
  }
  if (idx == 1) {
    FUN_0041f1e0(obj, 0x22, &m);
    *out = m.pos;
    return 1;
  }
  if (idx == 2) {
    FUN_0041f1e0(obj, 0x24, &m);
    *out = m.pos;
    return 2;
  }
  if (idx == 3) {
    FUN_0041f1e0(obj, 0x23, &m);
    *out = m.pos;
    return 3;
  }
  if (idx == 4) {
    FUN_0041f1e0(obj, 0x10, &m);
    *out = m.pos;
    return 4;
  }
  if (idx == 5) {
    FUN_0041f1e0(obj, 0x35, &m);
    *out = m.pos;
    return 5;
  }
  if (idx == 6) {
    FUN_0041f1e0(obj, 0x36, &m);
    *out = m.pos;
    return 6;
  }
  return 0;
}

typedef struct BoneRec {
  int index;
  void *pool;
  MVec3 v[7][5];
  int flag[7];
  int f1c8[7];
  undefined4 f1e4[7];
  undefined4 f200[7];
  int f21c[7];
  undefined4 f238;
} BoneRec;

MGLOBALA(BoneRec, DAT_006fde10, 4, 0x006fde10);

// MATCH: 0x00485e50 FUN_00485e50
void FUN_00485e50(int a,int b,undefined4 p3,undefined4 p4)
{
  int i;
  MVec3 v;

  if (DAT_006fde10[a].flag[b] != 1) {
    FUN_00485c20(a, b, &v);
    i = 5;
    while (i-- != 0) {
      DAT_006fde10[a].v[b][i] = v;
    }
    DAT_006fde10[a].f21c[b] = b;
    DAT_006fde10[a].flag[b] = 1;
    DAT_006fde10[a].f1c8[b] = 0;
    DAT_006fde10[a].f1e4[b] = p3;
    DAT_006fde10[a].f200[b] = p4;
  }
}
