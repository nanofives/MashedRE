// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S22 0x004fb130 (h7): best 81.0% (regs 82.8%, ops 89.7%) under /O2 /Ob1 /G5 /Oy- (ebp frame; raw converter
   16.4%). What moved it: /Oy-; the 64-byte matrix as a struct copy (rep movsd x3, one per return path, from an
   if/else chain ending in one copy); the three 0 stores as float 0.0f (int 0 put 0 in edi); the chained
   DAT_007d9908 = DAT_007d9904 = &m. Residual: the original keeps the matrix flags (+0xc) in esi and tests
   0x20000 as mov ecx,esi; and ecx,0x20000; cmp ecx,0x20000 (and mov edx,esi; and edx,3; cmp dl,3); 9176 folds the
   single-bit compare to test ecx,0x20000 under every form tried (local flags, __inline getter/test helpers with the
   mask as a parameter, switch, !(f & m), int vs unsigned, xor/sub forms), which also frees esi and shifts the
   prologue push. Permuter: 113 compiles, 0 gains. Class (a) for the folded compare [UNCERTAIN: a 9466 c2 is
   needed to confirm], (b) for anything that depends on it. */
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
typedef struct { float m[16]; } M_4fb130;
MGLOBAL(M_4fb130 *, DAT_007d9904, 0x007d9904);
MGLOBAL(char *, DAT_007d9900, 0x007d9900);
MGLOBAL(M_4fb130 *, DAT_007d9908, 0x007d9908);
MGLOBAL(M_4fb130, DAT_005daf10, 0x005daf10);
MGLOBAL(M_4fb130, DAT_006199f0, 0x006199f0);
float * FUN_004c4dc0(void * p0, void * p1);
void FUN_004fb210(void * p0, void * p1);
// MATCH: 0x004fb130 FUN_004fb130
void FUN_004fb130(M_4fb130 *param_1)
{
  if (DAT_007d9904 == 0) {
    if (DAT_007d9900 != 0 && (*(unsigned int *)(DAT_007d9900 + 0xc) & 0x20000) != 0x20000) {
      if ((*(unsigned int *)(DAT_007d9900 + 0xc) & 3) == 3) {
        FUN_004fb210(&DAT_006199f0, DAT_007d9900);
        DAT_007d9908 = DAT_007d9904 = &DAT_006199f0;
      } else {
        FUN_004c4dc0(&DAT_006199f0, DAT_007d9900);
        DAT_006199f0.m[3] = 0.0f;
        DAT_006199f0.m[7] = 0.0f;
        DAT_006199f0.m[11] = 0.0f;
        DAT_006199f0.m[15] = 1.0f;
        DAT_007d9904 = &DAT_006199f0;
      }
    } else {
      DAT_007d9908 = &DAT_005daf10;
      DAT_007d9904 = &DAT_005daf10;
    }
  }
  *param_1 = *DAT_007d9904;

}
