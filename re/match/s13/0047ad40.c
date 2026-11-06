// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047ad40 (h1): 100% (regs 100%). Lua C function: appends an entry (two bytes, one int) to the
   12-byte array at copter record +0x70, count at +0xd0; with 3 args the int takes (char)arg1. Raw 84.3%. What moved
   it: an entry struct indexed by the count, DAT_006bf1cc file-static. Field names are placeholders. Residual: none. */
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
typedef struct KtcEntry_ { unsigned char a; unsigned char b; char pad[6]; int c; } KtcEntry;
typedef struct KtcRec_ { char pad0[0x70]; KtcEntry e[8]; int count; } KtcRec;
#ifndef __cplusplus
static KtcRec *DAT_006bf1cc;
#else
MGLOBAL(KtcRec *, DAT_006bf1cc, 0x006bf1cc);
#endif
int FUN_004b6fc0(void *L);
double FUN_004b7090(void *L, int idx);

// MATCH: 0x0047ad40 FUN_0047ad40
int FUN_0047ad40(void *L)
{
  int top;
  int i;

  top = FUN_004b6fc0(L);
  i = DAT_006bf1cc->count;
  DAT_006bf1cc->e[i].c = 0;
  DAT_006bf1cc->e[i].a = (unsigned char)FUN_004b7090(L, top - 1);
  DAT_006bf1cc->e[i].b = (unsigned char)FUN_004b7090(L, top);
  DAT_006bf1cc->count++;
  if (top == 3) {
    DAT_006bf1cc->e[i].c = (char)FUN_004b7090(L, 1);
  }
  return 0;
}
