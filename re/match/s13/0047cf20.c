// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047cf20 (h4): best 56.5% (regs 58%, ops 72.5%). By hand from raw (52.2%). Residual: the original evaluates the world bitset test (w->0x60 bits[obj->0x20 >> 5] & 1 << (obj->0x20 & 31)) twice, once for the remove path and again for the add path, with ebx/ebp/edi saved at entry. 9176 CSEs the two tests into one in every form tried: macro, __inline / __forceinline helper, inlined add-path helper, if/else-if chains, / and % spellings (signed or unsigned), int/uint/long arrays, 1u. Only a signed-% spelling broke the CSE (wrong code). Permuter (96 compiles) no gain. Class (b) source shape (the form that defeats CSE not found). */
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
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
int FUN_0055ac50(int p0, int p1, int p2);
int FUN_0055ad30(int p0, int p1);
int FUN_0055ade0(int p0, int p1);
int FUN_0057c210(int p0);
#define F4CF20_SET(w, o) ((w) != 0 && (((unsigned int *)*(int *)((w) + 0x60))[*(unsigned short *)((o) + 0x20) >> 5] & (1 << (*(unsigned short *)((o) + 0x20) & 0x1f))) != 0)
// MATCH: 0x0047cf20 FUN_0047cf20
void FUN_0047cf20(int param_1, int param_2)
{
  int obj;
  int w;

  if (param_1 >= 0 && param_1 < 200 && DAT_006c71d8[param_1] != 0) {
    obj = FUN_0057c210(DAT_006c71d8[param_1]);
    if (obj != 0) {
      w = *(int *)(obj + 0x24);
      if (F4CF20_SET(w, obj) && param_2 == 0) {
        FUN_0055ade0(w, obj);
        return;
      }
      if (F4CF20_SET(w, obj)) {
        return;
      }
      if (param_2 != 0) {
        FUN_0055ac50(w, obj, 1);
        FUN_0055ad30(*(int *)(obj + 0x24), obj);
      }
    }
  }
}
