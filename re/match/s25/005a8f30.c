// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a8f30: source written from the listing (PORT-TOP-AUDIO-5A8F30-STOPS, 2026-11-16). 100.0%, 589 B
   BYTE-IDENTICAL. The original expands a nested bit-length macro: HB32(v) = (v & 0xffff0000) ? HB16(v >> 16) + 16 :
   HB16(v), HB16(w) = (w & 0xff00) ? DAT_00617ff8[w >> 8] + 7 : DAT_00617ff8[w] - 1, with v = M (1 << FUN_005aee20(0x28),
   or 1 when its low 16 bits are 0) re-evaluated at every use: 14 calls to 0x005aee20 between 0x005a8f4b and 0x005a913c,
   the +7 / +0x10 pair at 0x005a9011 / 0x005a9014, sar 0x18 / 0x10 / 8 at 0x005a9007 / 0x005a9065 / 0x005a90fc. The
   (v >> 16) & 0xff00 test reads byte 3 of v from the spilled slot (mov ah,[esp+0xf] 0x005a8fd0). Store order of the
   tail follows the listing (0x005a9160 +0x28 first). Old raw g2s source (30.4%): D:/MashedWT/PORT-TOP-AUDIO-5A8F30-STOPS_data. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBALA(signed char, DAT_00617ff8, 256, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e617c, 0x005e617c);
void FUN_005a91b0(void);
undefined4 FUN_005a9e10(int p0, undefined1 * p1, undefined4 p2);
int FUN_005aee20(uint p0);
#define SH_005a8f30 (1 << FUN_005aee20(0x28))
#define M_005a8f30 (((short)SH_005a8f30 != 0) ? (SH_005a8f30 & 0xffff) : 1)
#define HB16_005a8f30(w) ((((w) & 0xff00) != 0) ? DAT_00617ff8[(w) >> 8] + 7 : DAT_00617ff8[(w)] - 1)
#define HB32_005a8f30(v) ((((v) & 0xffff0000) != 0) ? HB16_005a8f30((v) >> 16) + 16 : HB16_005a8f30(v))
// MATCH: 0x005a8f30 FUN_005a8f30
void FUN_005a8f30(int param_1)

{
  int iVar2;

  FUN_005a9e10(param_1,&DAT_005e617c,0);
  *(undefined4 *)(param_1 + 0x40) = 0x4c;
  iVar2 = HB32_005a8f30(M_005a8f30);
  *(void (**)(void))(param_1 + 0x28) = FUN_005a91b0;
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(undefined2 *)(param_1 + 0x44) = 0;
  *(undefined2 *)(param_1 + 0x46) = 1;
  return;
}
