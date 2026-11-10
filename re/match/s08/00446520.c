// FLAGS: /Od
/* MATCH-S08 0x00446520: 100% BYTE-IDENTICAL 7412 B (MATCH-RACEPATH-BIG 2026-11-09; was 94.9% / 2725 bad B).
   FLAGS ARE /Od, NOT /O2 /GS /Ob1: this function is unoptimized code (every local in [ebp-x], for loops as
   jmp/inc/cmp, `xor eax,eax; test eax,eax; jne` tails = do { } while (0) vector macros, _CIsin/_CIcos/_CIatan
   calls). The caller has to decide whether a per-file /Od FLAGS line is acceptable for the port.
   Two findings that made it match:
   1. 9176 /Od lays locals out by a hash of the variable NAME (buckets in a fixed order, declaration order inside
      a bucket), not by declaration order. The locals here are named from one hash bucket sequence
      (re-derive with a probe compile) and declared in the original's address order, which reproduces the
      original 0x1d0 frame exactly. Name = Ghidra local (ebp offset + 4):
   vcl=l1d4, vco=l1d0, vcv=l1cc, vdh=l1c8, vdj=l1c4, vdk=l1c0, veo=l1bc, vfx=l1b8, vhk=l1b4, vik=l1b0, viw=l1ac,
   vjy=l1a8, vkj=l1a4, vkq=l1a0, vln=v19c, vmu=v190, vmw=v184, vog=v178, vop=l16c, vad=v12c, vah=v120, var=l114,
   vav=l110, vbc=v10c, vbe=l100, vbw=lfc, vby=vf8, vdz=lec, vfc=le8, vfe=le4, vfn=le0, vgk=ldc, vgl=ld8, vgy=ld4,
   vhe=ld0, vhg=vcc, vhh=lc0, vim=lbc, viq=lb8, vjl=lb4, vjo=lb0, vkg=lac, vku=l68, vlf=l64, vlh=l60, vly=l5c,
   vmb=l58, vmr=v54, vnf=l48, vnh=l44, vnl=l40, vno=l3c, vns=l38, vnu=l34, vpj=l30, vae=l2c, vay=l28, vbr=v24,
   vch=l18, vcn=v14, vcy=l8
   2. /Od keeps ST0 across consecutive statements that store the same variable (x = a; x /= b; x = sin(x) ...),
      and `x * 2.0f` compiles to fadd st(0), st(0). FUN_004b4cd0 takes (FUN_0045bfe0(), &seg, hit).
   3. (MATCH-RACEPATH-BIG) The 2725 differing bytes were one 6-byte growth at +0x2f1 shifting every later byte:
      `vcl = ...; vay = 90 - (180 - vcl);` reloaded vcl. The original keeps it in ST0 (fst [ebp-0x1d0];
      fsubr [180.0]), which is the assignment expression `vay = 90 - (180 - (vcl = ...))`.
   4. (MATCH-RACEPATH-BIG) The old "residual (a)" (param_1 field loaded before the frame operand, 16 statements,
      216 B) is reproduced by 9176, so the 9176/9466 hypothesis is not needed: written as an rvalue,
      `(0, param_1->f)`, the field is evaluated into ST0 first, as in the original. Same value, same type; only the
      lvalue-ness of the operand changes. Tried and not used: (double) casts (they fix the pointer+pointer and the
      compound forms, but move the frame operand first in frame-op-pointer, and (float)(double) adds a spill and
      grows the frame), `(&p->a)->x`, `(&p->a.x)[0]`, volatile, unary +, and C++ (/Tp breaks finding 2).
      Matrix local is float[16] (64 B, 8-aligned: the original's 4-byte gap). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } V3_00446520;
typedef struct {
  int active;              /* +0x000 */
  V3_00446520 p4;          /* +0x004 */
  V3_00446520 p10;         /* +0x010 */
  int f1c;                 /* +0x01c */
  int f20;                 /* +0x020 */
  int f24;                 /* +0x024 */
  int pad28[3];            /* +0x028 */
  float f34;               /* +0x034 */
  float f38;               /* +0x038 */
  float f3c;               /* +0x03c */
  V3_00446520 pos;         /* +0x040 */
  V3_00446520 dir;         /* +0x04c */
  float f58;               /* +0x058 */
  int pad5c[578];          /* +0x05c */
  V3_00446520 a964;        /* +0x964 */
  V3_00446520 a970;        /* +0x970 */
  V3_00446520 a97c;        /* +0x97c */
  V3_00446520 a988;        /* +0x988 */
  int i994;                /* +0x994 */
  int i998;                /* +0x998 */
  float f99c;              /* +0x99c */
  float f9a0;              /* +0x9a0 */
  float f9a4;              /* +0x9a4 */
} Cam_00446520;
MGLOBAL(int, DAT_00803344, 0x00803344);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0ff0, 0x007f0ff0);
MGLOBAL(V3_00446520, DAT_006146f0, 0x006146f0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
MGLOBAL(int, DAT_007f0f38, 0x007f0f38);
MGLOBAL(unsigned int, DAT_007f1030, 0x007f1030);
MGLOBAL(float, DAT_007f0fc8, 0x007f0fc8);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
double sin(double);
double cos(double);
double atan(double);
#pragma intrinsic(sin, cos, atan)
double acos(double);
void FUN_0041ef60(int i, int p1);
void FUN_00442600(void);
int FUN_0040e350(void);
int FUN_00405890(void);
V3_00446520 *FUN_00407600(int p0);
int FUN_004671a0(int p0);
int FUN_00467210(int p0);
float FUN_004c39b0(V3_00446520 *out, V3_00446520 *in);
int FUN_0045bfe0(void);
int FUN_004b4cd0(int p0, V3_00446520 *seg, float *hit);
int FUN_0045c350(float *hit, V3_00446520 *out);
void FUN_004c51a0(int m, V3_00446520 *v, int op);
void FUN_004b4430(int m, V3_00446520 *a, V3_00446520 *b);
void FUN_00441700(Cam_00446520 *cam);
int FUN_0042f6a0(void);
void FUN_004464c0(Cam_00446520 *cam);
void FUN_004427c0(Cam_00446520 *cam);
void FUN_0041f120(int i, int p1, float *out);
void FUN_00442a60(void);
void FUN_0040e180(int *a, int *b);
float FUN_00417730(int i);
int FUN_0046c7b0(int i);
void FUN_0046d4a0(int *out, int i);
void FUN_0046cb30(V3_00446520 *out, int i);
float FUN_004c3ac0(V3_00446520 *v);
void FUN_0046cbb0(int i, int *a, int *b);
float FUN_00408a50(int i);
void FUN_00441820(int i, V3_00446520 *v, float *f);
int FUN_00426bb0(int p0);
int FUN_0040e370(int i);
int FUN_00426c00(void);
void FUN_004c4d20(float *m, V3_00446520 *axis, float angle, int op);
void FUN_004c3df0(V3_00446520 *out, V3_00446520 *in, int n, float *m);
float FUN_00472650(float lo, float hi);
float FUN_004c3b30(float x);
void FUN_00441760(Cam_00446520 *cam);
void FUN_00442a20(Cam_00446520 *cam);
// MATCH: 0x00446520 FUN_00446520
void FUN_00446520(Cam_00446520 *param_1, int param_2)
{
  float vcl;
  float vco;
  float vcv;
  float vdh;
  int vdj;
  float vdk;
  float veo;
  int vfx;
  float vhk;
  float vik;
  float viw;
  int vjy;
  float vkj;
  int vkq;
  V3_00446520 vln;
  V3_00446520 vmu;
  V3_00446520 vmw;
  V3_00446520 vog;
  float vop[16];
  V3_00446520 vad;
  V3_00446520 vah;
  int var;
  int vav;
  V3_00446520 vbc;
  float vbe;
  float vbw;
  V3_00446520 vby;
  int vdz;
  float vfc;
  float vfe;
  int vfn;
  float vgk;
  float vgl;
  int vgy;
  int vhe;
  V3_00446520 vhg;
  float vhh;
  float vim;
  float viq;
  float vjl;
  float vjo;
  float vkg[16];
  int vku;
  int vlf;
  float vlh;
  int vly;
  int vmb;
  V3_00446520 vmr;
  float vnf;
  int vnh;
  float vnl;
  float vno;
  float vns;
  float vnu;
  float vpj;
  int vae;
  float vay;
  V3_00446520 vbr;
  float vch;
  V3_00446520 vcn;
  int vcy;

  for (vmb = 0; vmb < 4; vmb++) {
    FUN_0041ef60(vmb, 0);
  }
  param_1->f24 = DAT_00803344;
  FUN_00442600();
  vhe = FUN_0040e350();
  if (DAT_007f0fd0 == 5 && FUN_00405890() != 0) {
    vad = *FUN_00407600(0);
    var = FUN_004671a0(0);
    vkq = FUN_00467210(0);
    vjy = 0;
    vkj = 5.0f;
    vog = DAT_006146f0;
    vah.x = 0.0f;
    vah.y = (float)(cos((float)DAT_007f0ff0 * 0.0001f) * 0.3f + 0.5);
    vah.z = 0.0f;
    FUN_004c39b0(&vah, &vah);
    vmu = vad;
    vmw = vad;
    do {
      vmw.x += vah.x * vkj;
      vmw.y += vah.y * vkj;
      vmw.z += vah.z * vkj;
    } while (0);
    vln = vmw;
    vjy = FUN_004b4cd0(FUN_0045bfe0(), &vmu, vop);
    if (vjy != 0) {
      viw = 0.0f;
      if (FUN_0045c350(vop, &vad) == 1) {
        vjy = 0;
      }
      viw = vop[14] * vkj;
      param_1->f9a4 = viw - 0.1f;
    } else {
      if (param_1->f9a4 <= 2.5f) {
        param_1->f9a4 = 2.5f;
      }
      if (param_1->f9a4 < vkj) {
        param_1->f9a4 = param_1->f9a4 * 1.01f;
      } else {
        param_1->f9a4 = vkj;
      }
    }
    vln = vad;
    do {
      vln.x += vah.x * (0, param_1->f9a4);
      vln.y += vah.y * (0, param_1->f9a4);
      vln.z += vah.z * (0, param_1->f9a4);
    } while (0);
    FUN_004c51a0(vkq, &vln, 0);
    vog.x = (float)sin((float)DAT_007f0ff0 * 0.0001f);
    vog.z = (float)cos((float)DAT_007f0ff0 * 0.0001f);
    FUN_004b4430(vkq, &vad, &vog);
    param_1->f58 = 0.8f;
    FUN_00441700(param_1);
    return;
  }
  if (vhe != 6 || FUN_0042f6a0() == 0xb) {
    FUN_004464c0(param_1);
    if (param_1->f1c != 0) {
      FUN_004427c0(param_1);
      return;
    }
  }
  for (vmb = 0; vmb < 4; vmb++) {
    vik = 0.0f;
    FUN_0041f120(vmb, 0, &vik);
    if (vik < 144.0f) {
      FUN_0041ef60(vmb, 1);
    } else {
      FUN_0041ef60(vmb, 2);
    }
  }
  if (FUN_0042f6a0() == 2 || DAT_007f0fd0 == 5 || DAT_007f0fd0 == 10 || DAT_007f0fd0 == 9) {
    FUN_0041ef60(0, 0);
  }
  FUN_00442a60();
  FUN_0040e180(&vae, &vku);
  if (DAT_007f0fd0 == 7) {
    vfx = 0;
    vdj = 0;
    vhk = -1.0f;
    veo = -1.0f;
    vdk = 0.03f;
    vmb = 0;
    while (vmb < 4) {
      vdh = FUN_00417730(vmb);
      if (vdh > vhk) {
        vfx = vmb;
        vhk = vdh;
      }
      vmb++;
    }
    vdj = vfx;
    vmb = 0;
    while (vmb < 4) {
      if (vmb != vfx && FUN_0046c7b0(0) == 1) {
        vco = FUN_00417730(vmb);
        vcv = vhk - vco;
        if (vcv < vdk && vcv > veo) {
          veo = vcv;
          vdj = vmb;
        }
      }
      vmb++;
    }
    vae = vfx;
    vku = vdj;
  }
  if (DAT_007f0fd0 == 4) {
    vku = 0;
    vae = vku;
  }
  if (DAT_007f0fd0 == 9) {
    vku = 0;
    vae = vku;
  }
  if (DAT_007f0fd0 == 8) {
    vku = 0;
    vae = vku;
  }
  FUN_0046d4a0(&vlf, vae);
  vnl = *(float *)(vlf + 0x30);
  vno = *(float *)(vlf + 0x34);
  vhh = *(float *)(vlf + 0x38);
  FUN_0046cb30(&vbr, vae);
  do {
    vbr.x *= 0.00015f;
    vbr.y *= 0.00015f;
    vbr.z *= 0.00015f;
  } while (0);
  vnl += vbr.x;
  vhh += vbr.z;
  FUN_0046d4a0(&vlf, vku);
  vbr.x = vnl - *(float *)(vlf + 0x30);
  vbr.y = vno - *(float *)(vlf + 0x34);
  vbr.z = vhh - *(float *)(vlf + 0x38);
  FUN_0046cb30(&vmr, vae);
  do {
    vmr.x *= 0.00015f;
    vmr.y *= 0.00015f;
    vmr.z *= 0.00015f;
  } while (0);
  vbr.x -= vmr.x;
  vbr.z -= vmr.z;
  vhg.x = vbr.x;
  vhg.y = 0.0f;
  vhg.z = vbr.z;
  vgl = FUN_004c3ac0(&vhg) * 0.8f;
  vpj = vgl;
  if (vae == vku && vgl < 1.0f) {
    FUN_0046cbb0(vae, &vfn, &vly);
    if (vfn != 0) {
      vgl = 8.0f;
      vpj = vgl;
    }
  }
  vnl -= vbr.x / 2.0f;
  vnf = vnl;
  vno -= vbr.y / 2.0f;
  vbw = vno;
  vhh -= vbr.z / 2.0f;
  vfe = vhh;
  vgy = 0;
  if (vae == param_1->i994 && vku == param_1->i998) {
    vgy = 1;
  }
  if (vae == param_1->i998 && vku == param_1->i994) {
    vgy = 1;
  }
  if (param_2 != 0) {
    vgy = 0;
  }
  if (vgy == 0) {
    param_1->f99c += DAT_007f100c * 2.0f;
    if (param_1->f99c >= 1.0f) {
      param_1->i994 = vae;
      param_1->i998 = vku;
    } else {
      FUN_0046d4a0(&vlf, param_1->i994);
      vns = *(float *)(vlf + 0x30);
      vnu = *(float *)(vlf + 0x34);
      vjo = *(float *)(vlf + 0x38);
      FUN_0046d4a0(&vlf, param_1->i998);
      vbr.x = vns - *(float *)(vlf + 0x30);
      vbr.y = vnu - *(float *)(vlf + 0x34);
      vbr.z = vjo - *(float *)(vlf + 0x38);
      vhg.x = vbr.x;
      vhg.y = 0.0f;
      vhg.z = vbr.z;
      vfc = FUN_004c3ac0(&vhg) * 0.8f;
      vns -= vbr.x / 2.0f;
      vnu -= vbr.y / 2.0f;
      vjo -= vbr.z / 2.0f;
      viq = param_1->f99c;
      viq = viq * 2.0f;
      viq = viq - 1.0f;
      if (viq < 0.0f) {
        viq = 0.0f;
      }
      vgk = 1.0f - viq;
      vnl = vnl * viq + vns * vgk;
      vno = vno * viq + vnu * vgk;
      vhh = vhh * viq + vjo * vgk;
      vgl = vgl * viq + vfc * vgk;
    }
  } else {
    param_1->f99c = 0.0f;
  }
  vjl = FUN_00408a50(vae);
  vmb = (int)vjl;
  FUN_00441820(vmb, &vbr, &vch);
  vav = vmb + 1;
  if (vav >= FUN_00426bb0(0)) {
    vav = 0;
  }
  FUN_00441820(vav, &vmr, &vlh);
  vjl -= (float)vmb;
  viq = 1.0f - vjl;
  vbc.x = vbr.x * viq + vmr.x * vjl;
  vbc.y = vbr.y * viq + vmr.y * vjl;
  vbc.z = vbr.z * viq + vmr.z * vjl;
  vim = vch * viq + vlh * vjl;
  vjl = FUN_00408a50(vku);
  vmb = (int)vjl;
  FUN_00441820(vmb, &vbr, &vch);
  vav = vmb + 1;
  if (vav >= FUN_00426bb0(0)) {
    vav = 0;
  }
  FUN_00441820(vav, &vmr, &vlh);
  vjl -= (float)vmb;
  viq = 1.0f - vjl;
  vhg.x = vbr.x * viq + vmr.x * vjl;
  vhg.y = vbr.y * viq + vmr.y * vjl;
  vhg.z = vbr.z * viq + vmr.z * vjl;
  vbe = vch * viq + vlh * vjl;
  vim = vim + vbe;
  vim = vim * 0.5f;
  do {
    vbr.x = vbc.x + vhg.x;
    vbr.y = vbc.y + vhg.y;
    vbr.z = vbc.z + vhg.z;
  } while (0);
  FUN_004c39b0(&vbr, &vbr);
  vnh = 0;
  vdz = 0;
  for (vcy = 0; vcy < 4; vcy++) {
    if (FUN_0040e370(vcy) != 0 && FUN_0046c7b0(vcy) == 1) {
      vnh++;
      FUN_0046cbb0(vcy, &vfn, &vly);
      if (vfn != 0) {
        vdz++;
      }
    }
  }
  if (vnh == 1 && vdz == 1) {
    viq = (float)vly / 1000.0f;
    viq = viq + vim;
    if (viq > 10.0f) {
      viq = 10.0f;
    }
    vim = viq;
  }
  vim *= 1.1f;
  if (DAT_007f0fd0 == 8) {
    vim = 4.0f;
  }
  if (DAT_007f0fd0 == 4) {
    vim = 4.0f;
  }
  viq = vim;
  if (vim < vgl) {
    vim = vgl;
  }
  if (vim > 10.0f) {
    vim = 10.0f;
  }
  if (DAT_007f0f38 != 0) {
    vim = 10.0f;
  }
  if (viq < vpj) {
    viq = vpj;
  }
  if (viq > 10.0f) {
    viq = 10.0f;
  }
  param_1->f9a0 = viq;
  vay = DAT_006146fc[0] * vbr.x + DAT_006146fc[1] * vbr.y + DAT_006146fc[2] * vbr.z;
  if (vay < -1.0f) {
    vay = -1.0f;
  }
  if (vay > 1.0f) {
    vay = 1.0f;
  }
  vay = 90.0f - (180.0f - (vcl = (float)(acos(vay) * 57.2958f)));
  viq = (float)(-3.95f - vim * 0.25 / 5.0);
  do {
    vbr.x *= viq;
    vbr.y *= viq;
    vbr.z *= viq;
  } while (0);
  FUN_004c39b0(&vmr, &vbr);
  do {
    vbc.x = vmr.y * DAT_006146fc[2] - vmr.z * DAT_006146fc[1];
    vbc.y = vmr.z * DAT_006146fc[0] - vmr.x * DAT_006146fc[2];
    vbc.z = vmr.x * DAT_006146fc[1] - vmr.y * DAT_006146fc[0];
  } while (0);
  viq = 50.0f * vim / 10.0f;
  if (FUN_00426c00() == 0x1a) {
    viq = 67.5f * vim / 10.0f;
    vim *= 1.25f;
  }
  viq = viq - 5.0f;
  if (viq + vay >= 89.9f) {
    viq = 89.9f - vay;
  }
  if (DAT_007f0f38 != 0) {
    viq = 90.0f;
  }
  FUN_004c4d20(vkg, &vbc, viq, 0);
  FUN_004c3df0(&vbr, &vbr, 1, vkg);
  viq = vim / 0.8f / 5.0f + 1.0f;
  do {
    vbr.x *= viq;
    vbr.y *= viq;
    vbr.z *= viq;
  } while (0);
  vby.x = vbr.x + vnl;
  vby.y = vbr.y + vno;
  vby.z = vbr.z + vhh;
  if (DAT_007f0f38 != 0) {
    vby.x = vnl;
    vby.y = vno + 15.0f;
    vby.z = vhh;
  }
  vbr.y = 0.0f;
  FUN_004c39b0(&vbr, &vbr);
  viq = (10.0f - vim) * -2.5f / 10.0f;
  viq = 2.5f * vim / 10.0f + viq;
  if (FUN_00426c00() == 0x1a) {
    viq = viq - 2.25f * vim / 10.0f;
  }
  do {
    vbr.x *= viq;
    vbr.y *= viq;
    vbr.z *= viq;
  } while (0);
  if (DAT_007f0f38 == 0) {
    vnl += vbr.x;
    vno += vbr.y;
    vhh += vbr.z;
    vby.x += vbr.x;
    vby.y += vbr.y;
    vby.z += vbr.z;
  }
  vgy = 0;
  do {
    vbr.x = vby.x - (0, param_1->a964.x);
    vbr.y = vby.y - (0, param_1->a964.y);
    vbr.z = vby.z - (0, param_1->a964.z);
  } while (0);
  viq = FUN_004c3ac0(&vbr);
  if (viq > 5.0f) {
    vgy = 1;
  }
  if (param_2 != 0 || vgy != 0) {
    param_1->a964 = vby;
    param_1->a970.z = 0.0f;
    param_1->a970.y = 0.0f;
    param_1->a970.x = 0.0f;
    vnf = vnl;
    param_1->a97c.x = vnf;
    vbw = vno;
    param_1->a97c.y = vbw;
    vfe = vhh;
    param_1->a97c.z = vfe;
    param_1->a988.z = 0.0f;
    param_1->a988.y = 0.0f;
    param_1->a988.x = 0.0f;
    param_1->i994 = vae;
    param_1->i998 = vku;
    param_1->f99c = 0.0f;
  }
  do {
    vbr.x = vby.x - (0, param_1->a964.x);
    vbr.y = vby.y - (0, param_1->a964.y);
    vbr.z = vby.z - (0, param_1->a964.z);
  } while (0);
  do {
    vbr.x *= 0.05f;
    vbr.y *= 0.05f;
    vbr.z *= 0.05f;
  } while (0);
  do {
    param_1->a970.x = (0, param_1->a970.x) + vbr.x;
    param_1->a970.y = (0, param_1->a970.y) + vbr.y;
    param_1->a970.z = (0, param_1->a970.z) + vbr.z;
  } while (0);
  do {
    param_1->a970.x *= 0.8f;
    param_1->a970.y *= 0.8f;
    param_1->a970.z *= 0.8f;
  } while (0);
  do {
    vbr.x = (0, param_1->a964.x) + param_1->a970.x;
    vbr.y = (0, param_1->a964.y) + param_1->a970.y;
    vbr.z = (0, param_1->a964.z) + param_1->a970.z;
  } while (0);
  do {
    vmr.x = vby.x - vbr.x;
    vmr.y = vby.y - vbr.y;
    vmr.z = vby.z - vbr.z;
  } while (0);
  do {
    vmr.x *= (10.0f - vim) / 10.0f;
    vmr.y *= (10.0f - vim) / 10.0f;
    vmr.z *= (10.0f - vim) / 10.0f;
  } while (0);
  vby.x -= vmr.x;
  vby.y -= vmr.y;
  vby.z -= vmr.z;
  vcn.x = vnl;
  vcn.y = vno;
  vcn.z = vhh;
  do {
    vbr.x = vcn.x - (0, param_1->a97c.x);
    vbr.y = vcn.y - (0, param_1->a97c.y);
    vbr.z = vcn.z - (0, param_1->a97c.z);
  } while (0);
  do {
    vbr.x *= 0.05f;
    vbr.y *= 0.05f;
    vbr.z *= 0.05f;
  } while (0);
  do {
    param_1->a988.x = (0, param_1->a988.x) + vbr.x;
    param_1->a988.y = (0, param_1->a988.y) + vbr.y;
    param_1->a988.z = (0, param_1->a988.z) + vbr.z;
  } while (0);
  do {
    param_1->a988.x *= 0.8f;
    param_1->a988.y *= 0.8f;
    param_1->a988.z *= 0.8f;
  } while (0);
  do {
    vbr.x = (0, param_1->a97c.x) + param_1->a988.x;
    vbr.y = (0, param_1->a97c.y) + param_1->a988.y;
    vbr.z = (0, param_1->a97c.z) + param_1->a988.z;
  } while (0);
  do {
    vmr.x = vcn.x - vbr.x;
    vmr.y = vcn.y - vbr.y;
    vmr.z = vcn.z - vbr.z;
  } while (0);
  do {
    vmr.x *= (10.0f - vim) / 10.0f;
    vmr.y *= (10.0f - vim) / 10.0f;
    vmr.z *= (10.0f - vim) / 10.0f;
  } while (0);
  vcn.x -= vmr.x;
  vcn.y -= vmr.y;
  vcn.z -= vmr.z;
  if (DAT_007f0f38 == 0) {
    vmb = DAT_007f1030 / 60000;
    vgk = (vim + 0.2f) / 5.0f;
    viq = (float)vmb;
    viq /= 420.0f;
    viq = (float)sin(viq);
    viq /= 10.0f;
    viq *= vgk;
    vcn.x += viq;
    viq = (float)vmb;
    viq /= 635.0f;
    viq = (float)sin(viq);
    viq /= 10.0f;
    viq *= vgk;
    vcn.z += viq;
    viq = (float)vmb;
    viq /= 400.0f;
    viq = (float)sin(viq);
    viq /= 5.0f;
    viq *= vgk;
    vby.x += viq;
    viq = (float)vmb;
    viq /= 155.0f;
    viq = (float)sin(viq);
    viq /= 5.0f;
    viq *= vgk;
    vby.y += viq;
    viq = (float)vmb;
    viq /= 175.0f;
    viq = (float)sin(viq);
    viq /= 5.0f;
    viq *= vgk;
    vby.z += viq;
    viq = (float)vmb;
    viq /= 83.0f;
    viq = (float)sin(viq);
    viq /= 10.0f;
    viq *= vgk;
    vby.x += viq;
    viq = (float)vmb;
    viq /= 139.5f;
    viq = (float)sin(viq);
    viq /= 10.0f;
    viq *= vgk;
    vby.z += viq;
    viq = (float)vmb;
    viq /= 33.9f;
    viq = (float)sin(viq);
    viq /= 200.0f;
    viq *= vgk;
    vby.x += viq;
    viq = (float)vmb;
    viq /= 16.5f;
    viq = (float)sin(viq);
    viq /= 200.0f;
    viq *= vgk;
    vby.z += viq;
  }
  vcn.x += FUN_00472650(-DAT_007f0fc8, DAT_007f0fc8);
  vcn.y += FUN_00472650(-DAT_007f0fc8, DAT_007f0fc8);
  vcn.z += FUN_00472650(-DAT_007f0fc8, DAT_007f0fc8);
  vnl = vcn.x;
  vno = vcn.y;
  vhh = vcn.z;
  param_1->pos.x = vby.x;
  param_1->pos.y = vby.y;
  param_1->pos.z = vby.z;
  param_1->a964 = vby;
  param_1->a97c = vcn;
  vnl -= param_1->pos.x;
  vno -= param_1->pos.y;
  vhh -= param_1->pos.z;
  param_1->dir.x = vnl;
  param_1->dir.y = vno;
  param_1->dir.z = vhh;
  param_1->f58 = 0.6f;
  if (vhh != 0.0f) {
    vjl = vnl / vhh;
    vjl = (float)-(atan(vjl) * 57.2958f);
    vjl = 180.0f - vjl;
    if (vhh > 0.0f) {
      vjl = 180.0f + vjl;
    }
  } else if (vnl < 0.0f) {
    vjl = 270.0f;
  } else {
    vjl = 90.0f;
  }
  param_1->f38 = vjl;
  vnl *= vnl;
  vhh = vhh * vhh;
  vhh = vhh + vnl;
  if (vhh != 0.0f) {
    vhh = FUN_004c3b30(vhh);
  }
  if (vhh != 0.0f) {
    vjl = vno / vhh;
    vjl = (float)-(atan(vjl) * 57.2958f);
    vjl = 180.0f - vjl;
    if (vhh > 0.0f) {
      vjl = 180.0f + vjl;
    }
  } else if (vno < 0.0f) {
    vjl = 270.0f;
  } else {
    vjl = 90.0f;
  }
  param_1->f34 = 360.0f - vjl;
  param_1->f3c = 0.0f;
  if (DAT_007f0f38 != 0) {
    param_1->f38 = 0.0f;
  }
  FUN_00441760(param_1);
  param_1->p4 = param_1->pos;
  param_1->p10 = vcn;
  if (param_1->active != 0) {
    FUN_00442a20(param_1);
  }
}
