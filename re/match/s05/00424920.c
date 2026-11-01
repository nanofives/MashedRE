// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00424920: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs05_20261031/results.tsv. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(int, DAT_00899a40, 0x00899a40);
MGLOBAL(int, DAT_00899f20, 0x00899f20);
MGLOBAL(int, DAT_00899f74, 0x00899f74);
MGLOBAL(int, DAT_00899f24, 0x00899f24);
MGLOBAL(int, DAT_00899a94, 0x00899a94);
MGLOBAL(int, DAT_00899f78, 0x00899f78);
MGLOBAL(int, DAT_00899a44, 0x00899a44);
MGLOBAL(int, DAT_00899f28, 0x00899f28);
MGLOBAL(int, DAT_00899a98, 0x00899a98);
MGLOBAL(int, DAT_00899f70, 0x00899f70);
MGLOBAL(int, DAT_00899a48, 0x00899a48);
MGLOBAL(int, DAT_00899f7c, 0x00899f7c);
MGLOBAL(int, DAT_00899a90, 0x00899a90);
MGLOBAL(int, DAT_00899e80, 0x00899e80);
MGLOBAL(int, DAT_00899a9c, 0x00899a9c);
MGLOBAL(int, DAT_0089a058, 0x0089a058);
MGLOBAL(int, DAT_008999a0, 0x008999a0);
MGLOBAL(int, DAT_0089a0ac, 0x0089a0ac);
MGLOBAL(int, DAT_00899b78, 0x00899b78);
MGLOBAL(int, DAT_0089a05c, 0x0089a05c);
MGLOBAL(int, DAT_00899bcc, 0x00899bcc);
MGLOBAL(int, DAT_0089a0b0, 0x0089a0b0);
MGLOBAL(int, DAT_00899b7c, 0x00899b7c);
MGLOBAL(int, DAT_0089a060, 0x0089a060);
MGLOBAL(int, DAT_00899bd0, 0x00899bd0);
MGLOBAL(int, DAT_0089a0a8, 0x0089a0a8);
MGLOBAL(int, DAT_00899b80, 0x00899b80);
MGLOBAL(int, DAT_0089a0b4, 0x0089a0b4);
MGLOBAL(int, DAT_00899bc8, 0x00899bc8);
MGLOBAL(int, DAT_00899fb8, 0x00899fb8);
MGLOBAL(int, DAT_00899bd4, 0x00899bd4);
MGLOBAL(int, DAT_0089a190, 0x0089a190);
MGLOBAL(int, DAT_00899ad8, 0x00899ad8);
MGLOBAL(int, DAT_0089a1e4, 0x0089a1e4);
MGLOBAL(int, DAT_00899cb0, 0x00899cb0);
MGLOBAL(int, DAT_0089a194, 0x0089a194);
MGLOBAL(int, DAT_00899d04, 0x00899d04);
MGLOBAL(int, DAT_0089a1e8, 0x0089a1e8);
MGLOBAL(int, DAT_00899cb4, 0x00899cb4);
MGLOBAL(int, DAT_00899d08, 0x00899d08);
MGLOBAL(int, DAT_00899cb8, 0x00899cb8);
MGLOBAL(int, DAT_0089a198, 0x0089a198);
MGLOBAL(int, DAT_0089a1e0, 0x0089a1e0);
MGLOBAL(int, DAT_0089a1ec, 0x0089a1ec);
MGLOBAL(int, DAT_00899d00, 0x00899d00);
MGLOBAL(int, DAT_0089a0f0, 0x0089a0f0);
MGLOBAL(int, DAT_00899d0c, 0x00899d0c);
MGLOBAL(int, DAT_0089a2c8, 0x0089a2c8);
MGLOBAL(int, DAT_00899c10, 0x00899c10);
MGLOBAL(int, DAT_0089a31c, 0x0089a31c);
MGLOBAL(int, DAT_00899de8, 0x00899de8);
MGLOBAL(int, DAT_0089a2cc, 0x0089a2cc);
MGLOBAL(int, DAT_00899e3c, 0x00899e3c);
MGLOBAL(int, DAT_0089a320, 0x0089a320);
MGLOBAL(int, DAT_00899dec, 0x00899dec);
MGLOBAL(int, DAT_0089a2d0, 0x0089a2d0);
MGLOBAL(int, DAT_00899e40, 0x00899e40);
MGLOBAL(int, DAT_0089a318, 0x0089a318);
MGLOBAL(int, DAT_00899df0, 0x00899df0);
MGLOBAL(int, DAT_0089a324, 0x0089a324);
MGLOBAL(int, DAT_00899e38, 0x00899e38);
MGLOBAL(int, DAT_00899e44, 0x00899e44);
MGLOBAL(int, DAT_0089a228, 0x0089a228);
MGLOBAL(int, DAT_00899d48, 0x00899d48);
// MATCH: 0x00424920 FUN_00424920
void FUN_00424920(void)

{
  DAT_00899f20 = DAT_00899f20 + DAT_00899a40;
  DAT_00899f74 = DAT_00899f74 + DAT_00899a94;
  DAT_00899f24 = DAT_00899f24 + DAT_00899a44;
  DAT_00899f78 = DAT_00899f78 + DAT_00899a98;
  DAT_00899f28 = DAT_00899f28 + DAT_00899a48;
  DAT_00899f70 = DAT_00899f70 + DAT_00899a90;
  DAT_00899f7c = DAT_00899f7c + DAT_00899a9c;
  DAT_00899e80 = DAT_00899e80 + DAT_008999a0;
  DAT_0089a058 = DAT_0089a058 + DAT_00899b78;
  DAT_0089a0ac = DAT_0089a0ac + DAT_00899bcc;
  DAT_0089a05c = DAT_0089a05c + DAT_00899b7c;
  DAT_0089a0b0 = DAT_0089a0b0 + DAT_00899bd0;
  DAT_0089a060 = DAT_0089a060 + DAT_00899b80;
  DAT_0089a0a8 = DAT_0089a0a8 + DAT_00899bc8;
  DAT_0089a0b4 = DAT_0089a0b4 + DAT_00899bd4;
  DAT_00899fb8 = DAT_00899fb8 + DAT_00899ad8;
  DAT_0089a190 = DAT_0089a190 + DAT_00899cb0;
  DAT_0089a1e4 = DAT_0089a1e4 + DAT_00899d04;
  DAT_0089a194 = DAT_0089a194 + DAT_00899cb4;
  DAT_0089a1e8 = DAT_0089a1e8 + DAT_00899d08;
  DAT_0089a198 = DAT_0089a198 + DAT_00899cb8;
  DAT_0089a1e0 = DAT_0089a1e0 + DAT_00899d00;
  DAT_0089a1ec = DAT_0089a1ec + DAT_00899d0c;
  DAT_0089a0f0 = DAT_0089a0f0 + DAT_00899c10;
  DAT_0089a2c8 = DAT_0089a2c8 + DAT_00899de8;
  DAT_0089a31c = DAT_0089a31c + DAT_00899e3c;
  DAT_0089a2cc = DAT_0089a2cc + DAT_00899dec;
  DAT_0089a320 = DAT_0089a320 + DAT_00899e40;
  DAT_0089a2d0 = DAT_0089a2d0 + DAT_00899df0;
  DAT_0089a318 = DAT_0089a318 + DAT_00899e38;
  DAT_0089a324 = DAT_0089a324 + DAT_00899e44;
  DAT_0089a228 = DAT_0089a228 + DAT_00899d48;
  return;
}
