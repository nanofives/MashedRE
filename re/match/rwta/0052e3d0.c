// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWT-A 0x0052e3d0: g2s.py output (decomp_pc.py --port --callees), one edit: Ghidra's rep-stos loop is the
   intrinsic memset(p, 0, 0x400f) (rep stosd + stosw + stosb). Byte-identical under rw9044 and ddk.
   verify/match_20261112/RESULT_MATCH-RWT-A.md
   RWC-LIFT-EXEC-52E3D0 (2026-11-13): MATCH-S11 preamble, globals moved to MGLOBAL, the g2s typedef block cut to the
   names the body uses, the RW memory-function call typed (RWMALLOCFN) and the thunk result cast, so the port compiles
   it as C++ (MATCH_PORT). Body statements unchanged; re-scored, verify/rwclift_20261113/RESULT_RWC-LIFT-EXEC-52E3D0.md */
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
typedef void *(*RWMALLOCFN)(uint size, uint hint);

MGLOBAL(int, DAT_007dc520, 0x007dc520);
MGLOBAL(undefined4, DAT_00623c08, 0x00623c08);
MGLOBAL(undefined4, DAT_00623c04, 0x00623c04);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_007dc518, 0x007dc518);
MGLOBAL(undefined4 *, DAT_007dc510, 0x007dc510);
MGLOBAL(uint, DAT_007dbcdc, 0x007dbcdc);
MGLOBAL(int, _DAT_007dc50c, 0x007dc50c);
undefined4 FUN_004c2dc0(undefined4 p0);
bool FUN_0052ffd0(byte p0);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x0052e3d0 FUN_0052e3d0
undefined4 FUN_0052e3d0(undefined4 param_1)

{
  int iVar1;
  uint uVar2;

  if (DAT_007dc520 == 0) {
    uVar2 = 1;
    iVar1 = FUN_004c2dc0(0x120);
    if (iVar1 != -1) {
      uVar2 = 3;
    }
    iVar1 = FUN_004c2dc0(0x12e);
    if (iVar1 != -1) {
      uVar2 = uVar2 | 4;
    }
    FUN_0052ffd0((byte)uVar2);
    DAT_007dc518 = (undefined4)thunk_FUN_004cc820(0x40,DAT_00623c04,4,DAT_00623c08,&DAT_007dbcdc,0x40116);
    DAT_007dc510 = (undefined4 *)(*(RWMALLOCFN *)(DAT_007d3ff8 + 0x108))(0x400f,0x40116);
    memset(DAT_007dc510, 0, 0x400f);
    _DAT_007dc50c = (int)DAT_007dc510 + 0xfU & 0xfffffff0;
  }
  DAT_007dc520 = DAT_007dc520 + 1;
  return param_1;
}
