// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00552d10 (h2): 100% (regs 100%), byte-identical. Matrix-stack push: the stack is a struct
   global at 0x00912a80 { 4-byte field, Matrix64 *stack[32] at +4 (0x00912a84), int top at +0x84
   (0x00912b04) } (the +0 field is not read here; [UNCERTAIN] its type). With two separate globals the
   compiler kept &stack[top] in edi across the FUN_004c57a0 call (70.4%); as struct members it reloads top
   after the call like the original. Game flags /O2 /GS /Ob1 66.7%, /O1 55.6% on the separate-globals form. */
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
typedef struct { int m[16]; } Matrix64;
typedef struct { int pad; Matrix64 *stack[32]; int top; } MatStack;
MGLOBAL(MatStack, DAT_00912a80, 0x00912a80);
Matrix64 *FUN_004c57a0(void);
// MATCH: 0x00552d10 FUN_00552d10
int FUN_00552d10(void)
{
  int r = DAT_00912a80.top < 0x1f;
  if (r) {
    DAT_00912a80.top++;
    if (!DAT_00912a80.stack[DAT_00912a80.top]) {
      DAT_00912a80.stack[DAT_00912a80.top] = FUN_004c57a0();
    }
    *DAT_00912a80.stack[DAT_00912a80.top] = *DAT_00912a80.stack[DAT_00912a80.top - 1];
  }
  return r;
}
