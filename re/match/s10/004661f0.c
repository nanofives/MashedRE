// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004661f0: 100% (regs 100%, whole symbol byte-identical), by hand from raw g2s 51.7%. What moved it:
   FUN_00465a30 / FUN_00465b20 / FUN_00472650 are plain cdecl (Ghidra's __fastcall was wrong; FUN_00472650 is
   (int, float) returning float); FUN_004417e0 fills a 3-float struct (x and y used); pos as a 3-float struct
   copied into entry+0x58; the 22050.0f frequency stored as the int 0x46ac4400 (the compiler keeps the bits in edi
   for the stores and the pushes); float globals for the .rdata constants except the (in.y + 1.0f) literal, which
   gives the fld in.y / fadd order; the 1.0/0.0 flag at 0x00690594 as if/else (the compiler hoists the 1.0 store);
   p[0] == 116.41855f compiles to the integer compare. DAT_0069058c is file-static (pilot finding 2: the
   compiler keeps it in eax across the 500 block's calls): static under #ifndef __cplusplus, MGLOBAL under #else. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { float x, y, z; } Vec3_4661f0;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(float, DAT_005ccd0c, 0x005ccd0c);
MGLOBAL(float, DAT_005ce9ec, 0x005ce9ec);
MGLOBAL(float, DAT_005cc99c, 0x005cc99c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(int, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_005cc56c, 0x005cc56c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
#ifndef __cplusplus
static int DAT_0069058c;
#else
MGLOBAL(int, DAT_0069058c, 0x0069058c);
#endif
MGLOBAL(int, DAT_00690590, 0x00690590);
MGLOBAL(float, DAT_005cda34, 0x005cda34);
MGLOBAL(float, DAT_00690594, 0x00690594);
MGLOBAL(float, DAT_005ce9e8, 0x005ce9e8);
MGLOBAL(float, DAT_005cd0b8, 0x005cd0b8);
MGLOBAL(float, DAT_005ce6b8, 0x005ce6b8);
MGLOBAL(float, DAT_005cc568, 0x005cc568);
MGLOBAL(float, DAT_005ce9e4, 0x005ce9e4);
MGLOBAL(float, DAT_005cd96c, 0x005cd96c);
MGLOBAL(float, DAT_005ce9e0, 0x005ce9e0);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
void FUN_004417e0(Vec3_4661f0 *p0);
float *FUN_00452160(void);
void FUN_0045e0f0(int p0, float p1);
void FUN_00465a30(Vec3_4661f0 *p0);
void FUN_00465b20(Vec3_4661f0 *p0, int p1);
float FUN_00472650(int p0, float p1);
int FUN_004b44f0(int p0, int p1);
void FUN_005a66d0(int p0, int p1);
uint FUN_005a6d60(int p0, int p1, int p2, Vec3_4661f0 *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x004661f0 FUN_004661f0
void FUN_004661f0(void)
{
  float vol;
  Vec3_4661f0 pos;
  Vec3_4661f0 in;
  float *p;
  int idx;
  int i;
  float t;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1) {
    return;
  }
  FUN_004417e0(&in);
  if (in.x > DAT_005ccd0c) {
    vol = (in.x - DAT_005ccd0c) * DAT_005ce9ec + DAT_005cc99c;
    if (vol > DAT_005cc320) {
      vol = 1.0f;
    }
  }
  else {
    vol = 0.3f;
  }
  idx = *(int *)(DAT_0088e670 + 0x470);
  if ((*(byte *)((&DAT_0068f640)[idx * 2] + 0x34) & 2) == 0) {
    FUN_005a66d0((&DAT_0068f640)[idx * 2],1);
  }
  *(int *)((&DAT_0068f640)[idx * 2] + 0x3c) = 0x46ac4400;
  if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
    FUN_005a6dc0(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),4,1,22050.0f);
  }
  t = (in.y + 1.0f) * DAT_005cc56c + DAT_005cc32c;
  if (t < DAT_005d757c) {
    t = DAT_005d757c;
  }
  else if (t > DAT_005cc320) {
    t = DAT_005cc320;
  }
  FUN_0045e0f0(idx,t * vol);
  if (DAT_0069058c == 500) {
    idx = *(int *)(DAT_0088e670 + 0x620);
    if ((*(byte *)((&DAT_0068f640)[idx * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f640)[idx * 2],1);
    }
    *(int *)((&DAT_0068f640)[idx * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),4,1,22050.0f);
    }
    FUN_0045e0f0(idx,1.0f);
    pos.x = 52.0f;
    pos.y = 5.0f;
    pos.z = 31.0f;
    *(Vec3_4661f0 *)((&DAT_0068f640)[idx * 2] + 0x58) = pos;
    if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),0,1,&pos);
    }
    DAT_0069058c = 0;
  }
  idx = *(int *)(DAT_0088e670 + 0x500);
  DAT_0069058c++;
  if ((*(byte *)((&DAT_0068f640)[idx * 2] + 0x34) & 2) == 0) {
    FUN_005a66d0((&DAT_0068f640)[idx * 2],1);
  }
  *(int *)((&DAT_0068f640)[idx * 2] + 0x3c) = 0x46fa6400;
  if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
    FUN_005a6dc0(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),4,1,32050.0f);
  }
  FUN_0045e0f0(idx,1.0f);
  p = FUN_00452160();
  pos.x = p[0];
  pos.y = 1.5f;
  pos.z = p[2];
  *(Vec3_4661f0 *)((&DAT_0068f640)[idx * 2] + 0x58) = pos;
  if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
    FUN_005a6d60(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),0,1,&pos);
  }
  if (p[2] > DAT_005cda34 && DAT_00690590 == 0) {
    DAT_00690594 = 1.0f;
  }
  else {
    DAT_00690594 = 0.0f;
  }
  if (p[2] > DAT_005ce9e8 && DAT_00690590 == 0 && p[0] == 116.41855f) {
    for (i = 0; i < 3; i++) {
      idx = *(int *)(DAT_0088e670 + 0x590) + i;
      if ((*(byte *)((&DAT_0068f640)[idx * 2] + 0x34) & 2) == 0) {
        FUN_005a66d0((&DAT_0068f640)[idx * 2],1);
      }
      t = (float)i * DAT_005cd0b8 + DAT_005ce6b8;
      *(float *)((&DAT_0068f640)[idx * 2] + 0x3c) = t;
      if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
        FUN_005a6dc0(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),4,1,t);
      }
      FUN_0045e0f0(idx,1.0f);
      pos.x = 115.0f;
      pos.y = 3.0f;
      pos.z = 99.0f;
      *(Vec3_4661f0 *)((&DAT_0068f640)[idx * 2] + 0x58) = pos;
      if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
        FUN_005a6d60(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),0,1,&pos);
      }
    }
    DAT_00690590 = 1;
  }
  if (p[2] < DAT_005cc568) {
    DAT_00690590 = 0;
  }
  if (FUN_004b44f0(0,0x32) == 0x19) {
    pos.x = FUN_00472650(0,84.0f);
    pos.y = 0.0f;
    pos.z = DAT_005ce9e4 - FUN_00472650(0,16.0f);
    FUN_00465b20(&pos,0);
  }
  if (FUN_004b44f0(0,0xfa) == 0x19) {
    pos.x = FUN_00472650(0,30.0f) + DAT_005cd96c;
    pos.y = 0.0f;
    pos.z = FUN_00472650(0,34.0f) + DAT_005ce9e0;
    FUN_00465a30(&pos);
  }
  if (FUN_004b44f0(0,0xfa) == 0x19) {
    pos.x = FUN_00472650(0,30.0f) + DAT_005cd96c;
    pos.y = 0.0f;
    pos.z = FUN_00472650(0,34.0f) + DAT_005ce9e0;
    FUN_00465b20(&pos,1);
  }
  if (FUN_004b44f0(0,200) == 0x19) {
    pos.x = 152.0f;
    pos.y = 0.0f;
    pos.z = -7.0f;
    FUN_00465b20(&pos,1);
  }
  if (FUN_004b44f0(0,200) == 0x19) {
    pos.x = 81.0f;
    pos.y = 0.0f;
    pos.z = 60.0f;
    FUN_00465b20(&pos,1);
  }
  if (FUN_004b44f0(0,200) == 0x19) {
    pos.x = 151.0f;
    pos.y = 1.0f;
    pos.z = 35.0f;
    FUN_00465b20(&pos,1);
  }
  if (FUN_004b44f0(0,200) == 0x19) {
    pos.x = 159.0f;
    pos.y = 0.0f;
    pos.z = 5.0f;
    FUN_00465b20(&pos,1);
  }
  if (FUN_004b44f0(0,200) == 0x19) {
    pos.x = 168.0f;
    pos.y = 0.0f;
    pos.z = 22.0f;
    FUN_00465b20(&pos,1);
  }
  if (FUN_004b44f0(0,200) == 0x19) {
    pos.x = 61.0f;
    pos.y = 0.0f;
    pos.z = 60.0f;
    FUN_00465b20(&pos,1);
  }
  if (FUN_004b44f0(0,0xfa) == 0x19) {
    pos.x = FUN_00472650(0,30.0f) + DAT_005cd96c;
    pos.y = 0.0f;
    pos.z = FUN_00472650(0,34.0f) + DAT_005ce9e0;
    FUN_00465b20(&pos,FUN_004b44f0(0,1) + 2);
  }
  if (FUN_004b44f0(0,0xfa) == 0x19) {
    pos.x = 52.0f;
    pos.y = 0.0f;
    pos.z = 23.0f;
    FUN_00465b20(&pos,FUN_004b44f0(0,1) + 2);
  }
  idx = *(int *)(DAT_0088e670 + 0x8f0);
  if (DAT_00690594 != DAT_005d757c) {
    if ((*(byte *)((&DAT_0068f640)[idx * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f640)[idx * 2],1);
    }
    *(int *)((&DAT_0068f640)[idx * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),4,1,22050.0f);
    }
    FUN_0045e0f0(idx,1.0f);
    pos.x = p[0];
    pos.y = 1.5f;
    pos.z = p[2];
    *(Vec3_4661f0 *)((&DAT_0068f640)[idx * 2] + 0x58) = pos;
    if (*(int *)((&DAT_0068f640)[idx * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[idx * 2] + 0x30),0,1,&pos);
    }
  }
}
