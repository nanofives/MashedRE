// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00480100 (h8): 100% (regs 100%, ops 100%), 154/154 ins.
   Hand-written from the decomp (raw 36.7%): a 200-iteration `for` over parallel arrays indexed by i (the
   compiler strength-reduces to the 0x006c9438 pointer bound), identity RwMatrix local (flags |= 0x20003, stores
   in Ghidra's order), 0x0055bd80 fills a 0x20-byte two-RwV3d-with-pad local (copied .b then .a into the 0x18
   stride table at 0x006c9a88), the 32-byte scratch for 0x0057c420 as int[8] (a char array adds a /GS cookie). */
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
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2;
  RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct { RwV3d sup; RwV3d inf; } RwBBox;
typedef struct { RwV3d a; int pa; RwV3d b; int pb; } BBox4;
typedef struct { int pad[6]; int f18; int pad1c[3]; } Ent6c74f8;   /* 0x28 bytes */
MGLOBAL(int, DAT_0080332c, 0x0080332c);
MGLOBALA(int, DAT_006c3030, 200, 0x006c3030);
MGLOBALA(int, DAT_006c3350, 200, 0x006c3350);
MGLOBALA(RwMatrix, DAT_006c3670, 200, 0x006c3670);
MGLOBALA(int, DAT_006c6eb8, 200, 0x006c6eb8);
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
MGLOBALA(Ent6c74f8, DAT_006c74f8, 200, 0x006c74f8);
MGLOBALA(int, DAT_006c9438, 200, 0x006c9438);
MGLOBALA(RwBBox, DAT_006c9a88, 200, 0x006c9a88);
MGLOBALA(RwMatrix, DAT_006cb070, 200, 0x006cb070);
void FUN_0047d080(int p0, int p1);
void FUN_0047d100(int p0, int p1);
int FUN_00487280(int p0, RwMatrix *p1, RwBBox *p2, RwMatrix *p3, Ent6c74f8 *p4);
void FUN_004c0e50(int p0);
void FUN_004c45f0(RwMatrix *p0);
int FUN_0055ab30(int p0, int p1, int p2);
void FUN_0055ac00(int p0, int p1, int p2);
int FUN_0055ade0(int p0, int p1);
void FUN_0055bd80(int p0, RwMatrix *p1, int p2, BBox4 *p3);
void FUN_0055dff0(int p0, int p1);
int FUN_0057c210(int p0);
int FUN_0057c420(int p0, int *p1);
// MATCH: 0x00480100 FUN_00480100
void FUN_00480100(void)
{
  int i;
  int r;
  int w;
  RwMatrix *m;
  RwMatrix mat;
  BBox4 bb;
  int buf[8];

  mat.flags |= 0x20003;
  mat.at.z = 1.0f;
  mat.up.y = 1.0f;
  mat.right.x = 1.0f;
  mat.up.x = 0;
  mat.right.z = 0;
  mat.right.y = 0;
  mat.at.y = 0;
  mat.at.x = 0;
  mat.up.z = 0;
  mat.pos.z = 0;
  mat.pos.y = 0;
  mat.pos.x = 0;
  for (i = 0; i < 200; i++) {
    DAT_006c74f8[i].f18 = -1;
    DAT_006c3030[i] = 0;
    DAT_006c3350[i] = 0;
    if (DAT_006c6eb8[i] != 0) {
      DAT_006c9438[i] = 0;
      if (DAT_006c71d8[i] != 0) {
        FUN_0047d080(i,1);
        FUN_0047d100(i,1);
        r = FUN_0057c210(DAT_006c71d8[i]);
        w = *(int *)(r + 0x24);
        if (w != 0 &&
            (*(unsigned int *)(*(int *)(w + 0x60) + (*(unsigned short *)(r + 0x20) >> 5) * 4) &
             1 << (*(unsigned short *)(r + 0x20) & 0x1f)) != 0) {
          FUN_0055ade0(w,r);
        }
        m = *(RwMatrix **)(*(int *)(r + 0x10) + 8);
        *m = DAT_006cb070[i];
        FUN_004c45f0(m);
        FUN_004c0e50(*(int *)(DAT_006c71d8[i] + 4));
        FUN_0055dff0(r,0);
        if (*(int *)(r + 0x24) != 0) {
          FUN_0055ab30(*(int *)(r + 0x24),r,FUN_0057c420(r,buf));
          FUN_0055ac00(*(int *)(r + 0x24),r,0);
        }
        FUN_0055bd80(**(int **)(r + 0x10),&mat,0,&bb);
        DAT_006c9a88[i].inf = bb.b;
        DAT_006c9a88[i].sup = bb.a;
        if (DAT_0080332c != 0) {
          FUN_00487280(DAT_0080332c,&DAT_006cb070[i],&DAT_006c9a88[i],&DAT_006c3670[i],&DAT_006c74f8[i]);
        }
      }
    }
  }
}
