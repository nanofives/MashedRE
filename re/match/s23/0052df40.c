// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052df40: 88.9% (regs 88.9%). Copies the pixel payload (+0x4c, size dst+0x2c * dst+0x24) from src to
   dst with the memcpy intrinsic, returns 1 (no library source; libpair's inflate_set_dictionary pairing is wrong).
   Residual: the original loads src ([esp+8]) into edx in the prologue and forms esi with lea; ours reloads it after
   the pushes as mov+add. Tried size/pointer locals in both orders, &data[0], operand order, a src copy, flags
   (/O2 /GS /Ob1 66.7, /G3-/G5 same), permuter 85 compiles no improvement. Class (b). */
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
void *memcpy(void *dst, const void *src, unsigned int n);
#pragma intrinsic(memcpy)

typedef struct { char pad[0x24]; int a; char pad2[4]; int b; char pad3[0x1c]; unsigned char data[1]; } surf_t;

// MATCH: 0x0052df40 FUN_0052df40
int FUN_0052df40(surf_t *dst, surf_t *src)
{
    memcpy(dst->data, src->data, dst->b * dst->a);
    return 1;
}
