// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00545260 (h2): 61.6% (regs 54.7). Typed hdr struct {b[32],cnt,u4}, sz via cnt local, real fn-ptr typedefs. Residue: original keeps 3 separate 'xor eax,eax + 4 pops + add esp,0x30 + ret' exits, ours tail-merges them (77 vs 86 ins) and reg allocation (ebx=cnt, esi=sz, edi=buf, ebp=file) differs; flags /Ox,/Ob2,/G6,/O1,/Os tried. class (a). */
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
typedef unsigned int uint;
typedef int (__cdecl *ALLOC_t)(int,int);
typedef void (__cdecl *COPY_t)(undefined1 *,undefined1 *,int);
typedef void (__cdecl *FREE_t)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cbd30(int p0, undefined1 *p1, int p2);
int FUN_004cc790(int p0, int p1, int p2);
int FUN_004d7ff0(int p0);
void FUN_004d8480(undefined4 * p0);
undefined1 * FUN_00546380(int p0, int p1, int p2);
// MATCH: 0x00545260 FUN_00545260
undefined1 * FUN_00545260(int param_1)
{
  int iVar1;
  undefined1 *puVar3;
  undefined4 err[2];
  int cnt;
  struct { undefined1 b[32]; int cnt; int u4; } hdr;

  if (FUN_004cbd30(param_1,(undefined1 *)&hdr,0x28) != 0x28) {
    return (undefined1 *)0;
  }
  cnt = hdr.cnt;
  
  iVar1 = (*(ALLOC_t *)(DAT_007d3ff8 + 0x108))(cnt * 0xc,0x10102);
  if (iVar1 == 0) {
    err[0] = 0x102;
    err[1] = FUN_004d7ff0(0x80000013);
    FUN_004d8480(err);
    return (undefined1 *)0;
  }
  if (FUN_004cc790(param_1,iVar1,cnt * 0xc) == 0) {
    return (undefined1 *)0;
  }
  puVar3 = FUN_00546380(cnt,hdr.u4,iVar1);
  (*(COPY_t *)(DAT_007d3ff8 + 0xd0))(puVar3 + 0x18,hdr.b,0x20);
  *puVar3 = 0xc;
  (*(FREE_t *)(DAT_007d3ff8 + 0x10c))(iVar1);
  return puVar3;
}
