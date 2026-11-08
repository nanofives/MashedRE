// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bc470: 29.7% (regs 56.5%). Hand: file did not compile (IID undefined); rebuilt from the asm: p2 is the 0x10-byte format record (rate dword +0, bits byte +0xc, channels byte +0xd), Blk12 WAVEFORMATEX at W, Desc 0x48 bytes (3 GUID copies from DAT_005d0c9c/6c/0c, &W at +0x44) passed to vtable +0x20 and +0x24, FUN_005be0f0 is (0,ptr,0). Residual (b): original schedules the Desc stores interleaved with W and keeps edi=1; store order/zero-init placement not reproduced. */
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
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef int (__stdcall *vf3)(void *, void *, void *);
typedef int (__stdcall *vf4)(void *, int, void *, int);
#define VT(o, off) (*(int *)(*(int *)(o) + (off)))
typedef struct Blk12 { uint d[4]; ushort e; } Blk12;
#define W16(b, o) (*(ushort *)((byte *)&(b) + (o)))
#define W32(b, o) (*(uint *)((byte *)&(b) + (o)))
typedef struct Desc { uint g1[4]; uint g2[4]; uint one; uint zero; uint sz; uint g3[4]; uint z2; uint cb; void *w; } Desc;
MGLOBAL(undefined4, DAT_005d0c9c, 0x005d0c9c);
MGLOBAL(undefined4, DAT_005d0ca0, 0x005d0ca0);
MGLOBAL(undefined4, DAT_005d0ca4, 0x005d0ca4);
MGLOBAL(undefined4, DAT_005d0ca8, 0x005d0ca8);
MGLOBAL(undefined4, DAT_005d0c6c, 0x005d0c6c);
MGLOBAL(undefined4, DAT_005d0c70, 0x005d0c70);
MGLOBAL(undefined4, DAT_005d0c74, 0x005d0c74);
MGLOBAL(undefined4, DAT_005d0c78, 0x005d0c78);
MGLOBAL(undefined4, DAT_005d0c0c, 0x005d0c0c);
MGLOBAL(undefined4, DAT_005d0c10, 0x005d0c10);
MGLOBAL(undefined4, DAT_005d0c14, 0x005d0c14);
MGLOBAL(undefined4, DAT_005d0c18, 0x005d0c18);
MGLOBAL(undefined1, DAT_005e7180, 0x005e7180);
MGLOBAL(undefined1, DAT_005e71a0, 0x005e71a0);
__declspec(dllimport) long __stdcall CoCreateInstance(void *p0, void *p1, unsigned long p2, void *p3, void *p4);
void FUN_005bc450(int *p0);
void FUN_005bc640(int *p0);
void FUN_005be0f0(int p0, void *p1, int p2);
// MATCH: 0x005bc470 FUN_005bc470
int *FUN_005bc470(void *param_1, byte *param_2, int *param_3)
{
  int iVar2;
  Blk12 w;
  Desc d;

  FUN_005bc450(param_3);
  FUN_005be0f0(0, param_3 + 2, 0);
  FUN_005be0f0(0, param_3 + 8, 0);
  iVar2 = CoCreateInstance(param_1, 0, 3, &DAT_005e7180, param_3);
  if (iVar2 < 0) {
    return 0;
  }
  iVar2 = ((vf3)VT(param_3[0], 0))((void *)param_3[0], &DAT_005e71a0, param_3 + 1);
  if (iVar2 < 0) {
    param_3[1] = 0;
  }
  w.d[0] = 0; w.d[1] = 0; w.d[2] = 0; w.d[3] = 0; w.e = 0;
  W16(w, 2) = param_2[0xd];
  W32(w, 4) = *(uint *)param_2;
  W16(w, 0xe) = param_2[0xc];
  W16(w, 0xc) = (ushort)((param_2[0xc] >> 3) * W16(w, 2));
  W16(w, 0) = 1;
  W16(w, 0x10) = 0x12;
  W32(w, 8) = W16(w, 0xc) * W32(w, 4);
  d.g1[0] = DAT_005d0c9c; d.g1[1] = DAT_005d0ca0; d.g1[2] = DAT_005d0ca4; d.g1[3] = DAT_005d0ca8;
  d.g2[0] = DAT_005d0c6c; d.g2[1] = DAT_005d0c70; d.g2[2] = DAT_005d0c74; d.g2[3] = DAT_005d0c78;
  d.one = 1;
  d.zero = 0;
  d.sz = param_2[0xc] >> 3;
  d.g3[0] = DAT_005d0c0c; d.g3[1] = DAT_005d0c10; d.g3[2] = DAT_005d0c14; d.g3[3] = DAT_005d0c18;
  d.z2 = 0;
  d.cb = 0x12;
  d.w = &w;
  iVar2 = ((vf4)VT(param_3[0], 0x20))((void *)param_3[0], 0, &d, 0);
  if (iVar2 < 0) {
    FUN_005bc640(param_3);
    return 0;
  }
  iVar2 = ((vf4)VT(param_3[0], 0x24))((void *)param_3[0], 0, &d, 0);
  if (iVar2 < 0) {
    FUN_005bc640(param_3);
    return 0;
  }
  return param_3;
}
