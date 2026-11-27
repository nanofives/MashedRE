// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b8c40: RO-AUDIOINIT-45D460-REACH-5, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27): rewritten from the listing 0x005b8c40..0x005b92cb (1676 B) and the Ghidra
   decompilation. The MATCH-S27 h1 source it replaces (30.0%) declared the record as a float, read the device pointer before
   loading it, and called 0x005aeb90 / 0x005bc020 / 0x005c75b0 without their arguments. Called by 0x005b8910 on every path
   that returns the record (0x005b8bac / 0x005b8bca). rec = param_1, dev = *(rec+0x94) (0x005b8c4d), hw = dev+0x50 & 8
   (0x005b8c59..0x005b8c5c). Interface calls are __stdcall through the object's vtable (no add esp after them). Each
   float-to-int conversion stores the value as a float first, then fistp under control word | 0xc00 (truncation)
   (0x005b8df5..0x005b8e16, 0x005b8eec..0x005b8f0d, 0x005b8fec..0x005b900d). The x87 chains before each store are written
   in double so the VS2022 build does not round every step to float. */
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005ce990, 0x005ce990);
MGLOBAL(float, DAT_005ce268, 0x005ce268);
MGLOBAL(undefined1, DAT_005e7100, 0x005e7100);
MGLOBAL(undefined1, DAT_005e70f0, 0x005e70f0);
MGLOBAL(undefined1, DAT_005e7090, 0x005e7090);
MGLOBAL(undefined1, DAT_005e70d0, 0x005e70d0);
void FUN_005a9280(int *param_1, undefined4 param_2, float *param_3, float *param_4, int param_5);
int FUN_005aeb90(float param_1);
uint FUN_005bc020(undefined4 *param_1);
undefined4 FUN_005c75b0(int param_1);
double FUN_004a2d30(double param_1, double param_2);
typedef long (__stdcall *VF_I)(int self, int v);
typedef long (__stdcall *VF_0)(int self);
typedef long (__stdcall *VF_3)(int self, int a, int b, int c);
typedef long (__stdcall *VF_P2)(int self, void *a, int b);
typedef long (__stdcall *VF_SET)(int self, void *guid, int a, int b, int c, void *data, int n);
#define VT(o, off) (*(void **)(*(int *)(o) + (off)))
#define FLAGS(r) (*(uint *)((r) + 0x28))

// MATCH: 0x005b8c40 FUN_005b8c40
int FUN_005b8c40(int param_1)
{
  int dev;
  int hw;
  int iv;
  int obj;
  int ok;
  int rc;
  uint fl;
  uint q;
  float f;
  float out[3];
  undefined4 buf[13];
  int *posp;

  dev = *(int *)(param_1 + 0x94);
  hw = *(uint *)(dev + 0x50) & 8;
  if (*(int *)(param_1 + 0x84) == 0) {
    *(undefined4 *)(param_1 + 0x88) = 0;
    FLAGS(param_1) = FLAGS(param_1) & 0xfffffa03;
    return param_1;
  }
  if ((*(byte *)(*(int *)(param_1 + 0x84) + 0x54) & 2) != 0 && *(int *)(dev + 0x14) != 0) {
    if (hw != 0) {
      if ((*(byte *)(param_1 + 0x28) & 4) == 0) goto L_4d;
      iv = *(int *)(param_1 + 0x11c);
      *(float *)(iv + 0x64) = *(float *)(param_1 + 0x98);
      *(float *)(iv + 0x68) = *(float *)(param_1 + 0xd4);
      ((float *)buf)[0] = -*(float *)(param_1 + 0xa0);
      buf[1] = *(undefined4 *)(param_1 + 0xa4);
      buf[2] = *(undefined4 *)(param_1 + 0xa8);
      ((float *)buf)[3] = -*(float *)(param_1 + 0xac);
      buf[4] = *(undefined4 *)(param_1 + 0xb0);
      buf[5] = *(undefined4 *)(param_1 + 0xb4);
      *(undefined4 *)(iv + 0x4c) = buf[0];
      *(undefined4 *)(iv + 0x50) = buf[1];
      *(undefined4 *)(iv + 0x54) = buf[2];
      *(undefined4 *)(iv + 0x58) = buf[3];
      *(undefined4 *)(iv + 0x5c) = buf[4];
      *(undefined4 *)(iv + 0x60) = buf[5];
    }
    else {
      ((float *)buf)[0] = -*(float *)(param_1 + 0xa0);
      buf[1] = *(undefined4 *)(param_1 + 0xa4);
      buf[2] = *(undefined4 *)(param_1 + 0xa8);
      ((float *)buf)[3] = -*(float *)(param_1 + 0xac);
      buf[4] = *(undefined4 *)(param_1 + 0xb0);
      buf[5] = *(undefined4 *)(param_1 + 0xb4);
      ((float *)buf)[6] = *(float *)(param_1 + 0x98);
      ((float *)buf)[7] = *(float *)(param_1 + 0xd4);
      FUN_005a9280((int *)(dev + 0x14), 1, (float *)buf, out, *(int *)(dev + 0x4c));
      fl = FLAGS(param_1);
      if ((fl & 0x1000) != 0) {
        *(float *)(param_1 + 0x38) = out[0];
      }
      else {
        *(float *)(param_1 + 0x38) = (float)(((double)out[0] - *(float *)(param_1 + 0x38)) * DAT_005cc32c + *(float *)(param_1 + 0x38));
      }
      fl = fl | 0x40;
      FLAGS(param_1) = fl;
      if ((*(byte *)(*(int *)(param_1 + 0x94) + 0x50) & 0x40) != 0) {
        *(float *)(param_1 + 0x3c) = out[1];
        FLAGS(param_1) = fl | 0x20;
        f = (float)(((((double)out[2] + out[2]) - DAT_005cc320) * DAT_005cc32c + DAT_005cc320) * DAT_005ce990 - DAT_005ce990);
        obj = *(int *)(param_1 + 0xdc);
        ((VF_I)VT(obj, 0x40))(obj, (int)f);
        goto L_4d;
      }
      if ((fl & 4) == 0) goto L_4d;
      obj = *(int *)(param_1 + 0xe0);
      ((VF_P2)VT(obj, 0x30))(obj, (void *)(param_1 + 0x9c), 0);
    }
    FLAGS(param_1) = FLAGS(param_1) & 0xfffffffb;
  }
L_4d:
  if ((FLAGS(param_1) & 0x1000) != 0) {
    if ((FLAGS(param_1) & 0x20) == 0) {
      q = *(uint *)(*(int *)(param_1 + 0x84) + 0x2c);
      FLAGS(param_1) = FLAGS(param_1) | 0x20;
      *(float *)(param_1 + 0x2c) = (float)q;
    }
    FLAGS(param_1) = FLAGS(param_1) & 0xffffefff;
  }
  if ((*(byte *)(*(int *)(param_1 + 0x94) + 0x50) & 7) != 0 && (FLAGS(param_1) & 8) != 0 &&
      (FLAGS(param_1) & 1) != 0 && (obj = *(int *)(param_1 + 0xe4)) != 0) {
    ((VF_SET)VT(obj, 0x10))(obj, &DAT_005e7100, 0, 0, 0, (void *)(param_1 + 0xe8), 0x34);
    FLAGS(param_1) = FLAGS(param_1) & 0xfffffff7;
  }
  if ((*(byte *)(param_1 + 0x28) & 0x20) != 0) {
    if (hw != 0) {
      *(float *)(*(int *)(param_1 + 0x11c) + 0x34) = *(float *)(param_1 + 0x2c);
      *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) = *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) | 4;
    }
    else {
      f = (float)((double)*(float *)(param_1 + 0x3c) * *(float *)(param_1 + 0x2c));
      obj = *(int *)(param_1 + 0xdc);
      ((VF_I)VT(obj, 0x44))(obj, (int)f);
    }
    FLAGS(param_1) = FLAGS(param_1) & 0xffffffdf;
  }
  if ((*(byte *)(param_1 + 0x28) & 0x40) != 0) {
    if (hw != 0) {
      *(float *)(*(int *)(param_1 + 0x11c) + 0x3c) = *(float *)(param_1 + 0x30);
      *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) = *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) | 0x10;
    }
    else {
      f = (float)((double)*(float *)(*(int *)(param_1 + 0x94) + 0x10) * *(float *)(param_1 + 0x38) * *(float *)(param_1 + 0x30));
      rc = FUN_005aeb90(f);
      obj = *(int *)(param_1 + 0xdc);
      ((VF_I)VT(obj, 0x3c))(obj, rc);
    }
    FLAGS(param_1) = FLAGS(param_1) & 0xffffffbf;
  }
  if ((*(byte *)(param_1 + 0x28) & 0x80) != 0 && *(int *)(param_1 + 0x84) != 0 &&
      (~*(uint *)(*(int *)(param_1 + 0x84) + 0x54) >> 1 & 1) != 0) {
    if (hw != 0) {
      *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) = *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) | 2;
      *(float *)(*(int *)(param_1 + 0x11c) + 0x48) = *(float *)(param_1 + 0x34);
    }
    else {
      f = (float)(((double)*(float *)(param_1 + 0x34) * DAT_005cc32c + DAT_005cc320) * DAT_005ce990 - DAT_005ce990);
      obj = *(int *)(param_1 + 0xdc);
      ((VF_I)VT(obj, 0x40))(obj, (int)f);
    }
    FLAGS(param_1) = FLAGS(param_1) & 0xffffff7f;
  }
  if (hw != 0) {
    *(undefined4 *)(param_1 + 0x88) = FUN_005c75b0(*(int *)(param_1 + 0x11c));
    FLAGS(param_1) = FLAGS(param_1) & 0xfffffaef;
    return param_1;
  }
  ok = FUN_005bc020((undefined4 *)(param_1 + 0xdc));
  if ((FLAGS(param_1) & 0x100) != 0) {
    if (ok != 0) {
      obj = *(int *)(param_1 + 0xdc);
      ((VF_0)VT(obj, 0x48))(obj);
      ok = 0;
    }
    obj = *(int *)(param_1 + 0xdc);
    ((VF_I)VT(obj, 0x34))(obj, *(int *)(param_1 + 0x90));
    *(undefined4 *)(param_1 + 0x8c) = *(undefined4 *)(param_1 + 0x90);
    fl = FLAGS(param_1) & 0xfffffbff;
    FLAGS(param_1) = fl;
    if ((fl & 0x200) != 0) {
      obj = *(int *)(param_1 + 0xdc);
      rc = ((VF_3)VT(obj, 0x30))(obj, 0, 0, fl >> 0xb & 1);
      if (rc >= 0) {
        ok = 1;
      }
      *(undefined4 *)(param_1 + 0x90) = 0;
    }
    FLAGS(param_1) = FLAGS(param_1) & 0xfffffcff;
  }
  if ((FLAGS(param_1) & 0x400) != 0) {
    obj = *(int *)(param_1 + 0xdc);
    ((VF_I)VT(obj, 0x34))(obj, *(int *)(param_1 + 0x8c));
    FLAGS(param_1) = FLAGS(param_1) & 0xfffffbff;
  }
  posp = (int *)(param_1 + 0x8c);
  if (ok != 0) {
    obj = *(int *)(param_1 + 0xdc);
    ((VF_P2)VT(obj, 0x10))(obj, posp, 0);
  }
  else {
    *posp = *(int *)(param_1 + 0x90);
  }
  *(uint *)(param_1 + 0x88) = ((uint)*posp / (uint)(*(byte *)(*(int *)(param_1 + 0x84) + 0x38) >> 3)) /
                              (uint)*(byte *)(*(int *)(param_1 + 0x84) + 0x39);
  if ((*(byte *)(param_1 + 0x28) & 0x10) == 0) {
    return param_1;
  }
  obj = *(int *)(param_1 + 0xe4);
  if (obj != 0) {
    dev = *(int *)(param_1 + 0x94);
    if ((*(byte *)(dev + 0x50) & 1) != 0) {
      buf[0] = *(undefined4 *)(param_1 + 0x70);
      buf[1] = 0;
      buf[2] = *(undefined4 *)(param_1 + 0x40);
      buf[3] = *(undefined4 *)(param_1 + 0x44);
      buf[4] = *(undefined4 *)(param_1 + 0x48);
      buf[5] = *(undefined4 *)(param_1 + 0x74);
      buf[6] = *(undefined4 *)(param_1 + 0x78);
      buf[7] = *(undefined4 *)(param_1 + 0x7c);
      buf[8] = *(undefined4 *)(param_1 + 0x80);
      ((VF_SET)VT(obj, 0x10))(obj, &DAT_005e70f0, 0, 0, 0, buf, 0x24);
      FLAGS(param_1) = FLAGS(param_1) & 0xffffffef;
      return param_1;
    }
    if ((*(byte *)(dev + 0x50) & 2) != 0) {
      f = (float)FUN_004a2d30(10.0, (double)*(int *)(param_1 + 0x40) * (double)DAT_005ce268);
      obj = *(int *)(param_1 + 0xe4);
      ((VF_SET)VT(obj, 0x10))(obj, &DAT_005e7090, 0, 0, 0, &f, 4);
      FLAGS(param_1) = FLAGS(param_1) & 0xffffffef;
      return param_1;
    }
    if ((*(byte *)(dev + 0x50) & 4) != 0) {
      buf[0] = *(undefined4 *)(param_1 + 0x70);
      buf[1] = 0;
      buf[2] = *(undefined4 *)(param_1 + 0x40);
      buf[3] = *(undefined4 *)(param_1 + 0x44);
      buf[4] = *(undefined4 *)(param_1 + 0x48);
      buf[5] = *(undefined4 *)(param_1 + 0x74);
      buf[6] = *(undefined4 *)(param_1 + 0x78);
      buf[7] = *(undefined4 *)(param_1 + 0x7c);
      buf[8] = *(undefined4 *)(param_1 + 0x80);
      buf[9] = 0x3f000000;
      buf[10] = *(undefined4 *)(param_1 + 0x44);
      buf[11] = 0x3f800000;
      buf[12] = 1;
      ((VF_SET)VT(obj, 0x10))(obj, &DAT_005e70d0, 1, 0, 0, buf, 0x34);
    }
  }
  FLAGS(param_1) = FLAGS(param_1) & 0xffffffef;
  return param_1;
}
#undef VT
#undef FLAGS
