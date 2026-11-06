// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00480340 (h8): matched. 289/297 ins at 97.4% only because Ghidra's size (983 B) stops before the
   epilogue; match.py --bytes: 992 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL.
   Hand-written from the decomp (raw 23.2%): 32-entry 0x110-byte PObj table at 0x0086cac0 indexed by i (no pointer
   local: the compiler's induction pointer lands on +0x10c count), memset 0xff for the two 200-int arrays,
   int atoms[64] for 0x004b3f50, real callee arities (0x0044d6e0 takes 1, 0x004781b0 takes 2, 0x00478110 takes 2,
   0x0047fe00/0x0047ff70 take 6), RwEngine slot +0xf4 called with the MTS name. The "mass" printf reads entry 0
   (absolute 0x0086cba4) in both branches, as the original does. printf (0x004a2cbd) is the CRT entry. */
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
typedef struct {
  int type;            /* +0x000 0x0086cac0 */
  int pad04[16];
  char dffname[64];    /* +0x044 */
  char mtsname[64];    /* +0x084 */
  RwV3d box;           /* +0x0c4 */
  float cyl[2];        /* +0x0d0 */
  float fdc[2];
  float fe0;           /* +0x0e0 */
  float mass;          /* +0x0e4 */
  RwV3d com;           /* +0x0e8 */
  float bounce;        /* +0x0f4 */
  float slide;         /* +0x0f8 */
  float fatness;       /* +0x0fc */
  int pad100[3];
  int count;           /* +0x10c */
} PObj;                /* 0x110 bytes */
MGLOBALA(PObj, DAT_0086cac0, 32, 0x0086cac0);
MGLOBAL(int, DAT_006c6eb0, 0x006c6eb0);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBALA(int, DAT_006c6b90, 200, 0x006c6b90);
MGLOBALA(int, DAT_006c9758, 200, 0x006c9758);
MGLOBALA(RwMatrix, DAT_006cb070, 200, 0x006cb070);
int printf(const char *fmt, ...);
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
int FUN_00426060(void);
int FUN_00426070(void);
void FUN_0044d6e0(int p0);
int FUN_00478110(char *p0, RwMatrix *p1);
int FUN_004781b0(char *p0, int p1);
int FUN_0047f840(int p0, int p1);
void FUN_0047f940(int p0, int p1, int p2, int p3, int p4, int p5);
void FUN_0047fc40(int p0, int p1, int p2, int p3, int p4, int p5);
void FUN_0047fe00(int p0, int p1, int p2, int p3, int p4, int p5);
void FUN_0047ff70(int p0, int p1, int p2, int p3, int p4, int p5);
void FUN_00480100(void);
int FUN_004b3f50(int p0, int *p1);
int FUN_004b3f90(int p0);
int FUN_004e6e00(int p0);
typedef int (*RwFileFn)(char *);
// MATCH: 0x00480340 FUN_00480340
void FUN_00480340(void)
{
  int a;
  int i;
  int j;
  int n;
  int sum;
  int clump;
  int geom;
  int atoms[64];

  printf("!!!!!!!!!!!!!!!!!!!! Initialize CALLED !!!!!!!!!!!!!!!!!!!!!\n");
  a = FUN_00426060();
  FUN_0047f840(a,FUN_00426070());
  memset(DAT_006c9758,0xff,sizeof(DAT_006c9758));
  DAT_006c6eb0 = 0;
  sum = 0;
  memset(DAT_006c6b90,0xff,sizeof(DAT_006c6b90));
  for (i = 0; i < 32; i++) {
    DAT_0086cac0[i].count = 0;
    FUN_0044d6e0(1);
    if (DAT_0086cac0[i].type != 0) {
      if ((*(RwFileFn *)(DAT_007d3ff8 + 0xf4))(DAT_0086cac0[i].mtsname) != 0) {
        n = FUN_00478110(DAT_0086cac0[i].mtsname,&DAT_006cb070[sum]);
        DAT_0086cac0[i].count = n;
        clump = FUN_004781b0(DAT_0086cac0[i].dffname,0);
        geom = FUN_004b3f90(clump);
      }
      else {
        clump = FUN_004781b0(DAT_0086cac0[i].dffname,0);
        geom = FUN_004b3f90(clump);
        n = FUN_004b3f50(*(int *)(geom + 4),atoms);
        DAT_0086cac0[i].count = n;
        for (j = 0; j < n; j++) {
          DAT_006cb070[sum + j] = *(RwMatrix *)(atoms[j] + 0x10);
        }
      }
      if (DAT_0086cac0[i].type == 2) {
        printf("making cylinder\n");
        printf("%d - type=%d\n",i,DAT_0086cac0[i].type);
        printf("%d - cylinderdimensions=%f,%f\n",i,DAT_0086cac0[i].cyl[0],DAT_0086cac0[i].cyl[1]);
        printf("%d - mass=%f\n",i,DAT_0086cac0[0].mass);
        printf("%d - COMoffset=%f,%f,%f\n",i,DAT_0086cac0[i].com.x,DAT_0086cac0[i].com.y,DAT_0086cac0[i].com.z);
        printf("%d - bouncefriction=%f\n",i,DAT_0086cac0[i].bounce);
        printf("%d - slidefriction=%f\n",i,DAT_0086cac0[i].slide);
        printf("%d - fatness=%f\n",i,DAT_0086cac0[i].fatness);
        printf("%d - MTSname=%s\n",i,DAT_0086cac0[i].mtsname);
        printf("%d - dffname=%s\n",i,DAT_0086cac0[i].dffname);
        FUN_0047f940(n,sum,DAT_006ce274,a,i,geom);
      }
      else if (DAT_0086cac0[i].type == 1) {
        printf("making box\n");
        printf("%d - type=%d\n",i,DAT_0086cac0[i].type);
        printf("%d - boxdimensions=%f,%f,%f\n",i,DAT_0086cac0[i].box.x,DAT_0086cac0[i].box.y,DAT_0086cac0[i].box.z);
        printf("%d - mass=%f\n",i,DAT_0086cac0[0].mass);
        printf("%d - COMoffset=%f,%f,%f\n",i,DAT_0086cac0[i].com.x,DAT_0086cac0[i].com.y,DAT_0086cac0[i].com.z);
        printf("%d - bouncefriction=%f\n",i,DAT_0086cac0[i].bounce);
        printf("%d - slidefriction=%f\n",i,DAT_0086cac0[i].slide);
        printf("%d - fatness=%f\n",i,DAT_0086cac0[i].fatness);
        printf("%d - MTSname=%s\n",i,DAT_0086cac0[i].mtsname);
        printf("%d - dffname=%s\n",i,DAT_0086cac0[i].dffname);
        FUN_0047fc40(n,sum,DAT_006ce274,a,i,geom);
      }
      else if (DAT_0086cac0[i].type == 3) {
        FUN_0047fe00(n,sum,DAT_006ce274,a,i,geom);
      }
      else if (DAT_0086cac0[i].type == 4) {
        FUN_0047ff70(n,sum,DAT_006ce274,a,i,geom);
      }
      printf("%d - number=%d\n",i,n);
      printf("%d - matnum=%d\n",i,sum);
      FUN_004e6e00(clump);
      sum += n;
    }
  }
  FUN_00480100();
  printf("numscenePobjects=%d\n",DAT_006c6eb0);
}
