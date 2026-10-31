// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f260 by hand (h9): 100% (regs 100%). Raw g2s 31.2%. DAT_0063d9e0 is an array of 0x2ac-byte (0xab-dword) per-player records; Ghidra's DAT_0063dc4c is record+0x26c. The separate add eax,0x63d9e0 needs a local PSlot* (indexing the array directly folds base+0x26c into one operand). The 16-dword copy is a struct assignment (rep movsd). No residual. */
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
typedef struct { float m[16]; } Mat16;
typedef struct { int *ev[0x9b]; char *obj; int pad[0xf]; } PSlot;
MGLOBALA(PSlot, DAT_0063d9e0, 4, 0x0063d9e0);
// MATCH: 0x0041f260 FUN_0041f260
void FUN_0041f260(int param_1, Mat16 *param_2)
{
  PSlot *s = &DAT_0063d9e0[param_1];
  *param_2 = *(Mat16 *)(*(char **)(s->obj + 4) + 0x10);
}
