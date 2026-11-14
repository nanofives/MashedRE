/* MATCH-RWT-S5 (Fable 5.2 high, one attempt) 2026-11-13: 100.0% byte match (119/119 ins, regs 100, ops 100) under
   ddk (cl 13.00.9176 / c2 13.00.9178) and rw9044, flags /O2 /Ob1 /G5. Source shape that closed it:
   (1) the slot index DAT_007d70c8 is used directly at every site, with no local copy. c2 then forms the index web
       itself: load at 0x004dcbbb (mov ebp,[0x7d70c8]), found path 0x004dcc27 mov ebp,ecx / 0x004dcc2c
       mov [0x7d70c8],ebp, reloads after the pn stores (0x004dcc7b) and after the call (0x004dcc96).
   (2) end is a variable (off + size, or size when off is 0: 0x004dcc4a mov ebx,edi), kept in ebx.
   (3) the pn loop key is a local loaded before the loop (0x004dcc5f mov ecx,[ebp*4+0x7d70ec] precedes the null
       test at 0x004dcc68); the tail uses DAT_007d70ec[DAT_007d70c8] inline in each branch (0x004dcc9f
       mov ebp,[ebp*4+0x7d70ec], then mov ecx,[ebp] / mov eax,[ebp] per branch).
   (4) the entry block reads param_1 once through `*(volatile uint *)&param_1` (0x004dcbb0 mov ecx,[esp+4]) and uses
       that copy for the size product and the first round-up; the loop and the tail read param_1 itself, which c2
       leaves in its home slot (0x004dcc00 mov ebx,[esp+0x14] per pass, 0x004dccf2 div dword ptr [esp+0x14]).
       param_1 is never written, so the copy holds the same value. With a plain copy or param_1 everywhere, c2 gives
       param_1 a callee-saved register and the index stays in memory (41.2%).
   History: RWL01 77.5% and the permuter's 77.5% measured a source that stored the OLD index on the found path
   (void, S3). S3 fixed the store (62.0%), S5 attempt 2 reached 83.2% with a local index copy.
   Probes: verify/match_20261113/MATCH-RWT-S5_probes/ (f1..f4), RESULT: verify/match_20261113/RESULT_MATCH-RWT-S5.md */
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dcbb0: real prototype is cdecl 5 args (the PC image reads [esp+4]..[esp+0x14]); the vtable call at +0x2c is __stdcall (callee pops, no add esp after 0x004dccbb / 0x004dccde). */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int (__stdcall *vfn_t)(int *, uint, uint, undefined4, uint);
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(uint, DAT_007d70c8, 0x007d70c8);
MGLOBAL(int *, DAT_007d70c0, 0x007d70c0);
MGLOBALA(uint, DAT_007d70cc, 4, 0x007d70cc);
MGLOBALA(uint, DAT_007d70dc, 4, 0x007d70dc);
MGLOBALA(int *, DAT_007d70ec, 4, 0x007d70ec);
undefined4 FUN_004dcaa0(uint p0, int ** p1);
// MATCH: 0x004dcbb0 FUN_004dcbb0
int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)
{
  int *vb;
  int hr;
  int *pn;
  uint n;
  uint off;
  uint size;
  uint end;
  uint a0;

  a0 = *(volatile uint *)&param_1;
  size = a0 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    off = ((DAT_007d70cc[DAT_007d70c8] + a0 - 1) / a0) * a0;
    end = off + size;
    if (end > DAT_007d70dc[DAT_007d70c8]) {
      for (n = 0; n < 4; n++) {
        if (n != DAT_007d70c8) {
          off = ((DAT_007d70cc[n] + param_1 - 1) / param_1) * param_1;
          end = off + size;
          if (end <= DAT_007d70dc[n]) {
            DAT_007d70c8 = n;
            break;
          }
        }
      }
      if (n >= 4) {
        DAT_007d70c8++;
        if (DAT_007d70c8 >= 4) {
          DAT_007d70c8 = 0;
        }
        off = 0;
        end = size;
      }
    }
  }
  else {
    off = 0;
    end = size;
  }
  if (size > DAT_007d70dc[DAT_007d70c8]) {
    vb = DAT_007d70ec[DAT_007d70c8];
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == vb) {
        pn[1] = 0;
        pn[3] = 0;
        break;
      }
    }
    DAT_007d70dc[DAT_007d70c8] = size;
    FUN_004dcaa0(size,&DAT_007d70ec[DAT_007d70c8]);
  }
  if (off == 0) {
    hr = (*(vfn_t *)(*DAT_007d70ec[DAT_007d70c8] + 0x2c))(DAT_007d70ec[DAT_007d70c8],0,size,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = size;
    *param_5 = 0;
  }
  else {
    hr = (*(vfn_t *)(*DAT_007d70ec[DAT_007d70c8] + 0x2c))(DAT_007d70ec[DAT_007d70c8],off,size,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = end;
    *param_5 = off / param_1;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return hr >= 0;
}
