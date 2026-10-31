// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f4f0 by hand (h9): 100% (regs 100%). Raw g2s 48.0%. The record arrives in EDI (Ghidra unaff_EDI): FUN_0041f4f0 is a file-static function and the file holds a stand-in caller (Caller_00420870, shaped like the call site in 0x00420870, NOT matched) so cl picks the register convention (pilot finding 1). Callees: 0x004c1520 (frame, axis V3*, angle, op) and 0x004c1340 (frame, V3*, op), both cdecl. Stack slots of a/b follow operand order: (a.z + b.z) and (a.x + b.x) put a at the higher slot (b + a gave 96%). The two float factors are literals -100.0f and 40.0f (pooled at 0x005cd038 and 0x005cd274; relocheck checks the values). Bytes of 0x0041f4f0 identical; the stand-in caller is not the real TU. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { char u0[0x4c]; char *clump; char u50[0x290 - 0x50]; unsigned int x290; } CarRec;
MGLOBAL(V3, DAT_006146f0, 0x006146f0);
MGLOBAL(V3, DAT_00614708, 0x00614708);
void FUN_0046bce0(V3 *out, unsigned int v);
void FUN_0046bd20(V3 *out, unsigned int v);
char *FUN_004c1340(char *frame, V3 *v, int op);
char *FUN_004c1520(char *frame, V3 *axis, float angle, int op);
// MATCH: 0x0041f4f0 FUN_0041f4f0
static void FUN_0041f4f0(CarRec *r)
{
  char *frame = *(char **)(r->clump + 4);
  V3 a, b, pos;
  float ang;
  FUN_0046bce0(&a, r->x290);
  FUN_0046bd20(&b, r->x290);
  pos = *(V3 *)(frame + 0x40);
  ang = (a.z + b.z) * -100.0f;
  FUN_004c1520(frame, &DAT_00614708, (a.x + b.x) * 40.0f, 0);
  FUN_004c1520(frame, &DAT_006146f0, ang, 2);
  FUN_004c1340(frame, &pos, 2);
}
/* stand-in caller shaped like the call site in 0x00420870 (not matched here) */
void Caller_00420870(CarRec *r, int a)
{
  FUN_0041f4f0(r);
  FUN_0041f4f0(r);
}
