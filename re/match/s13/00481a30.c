// FLAGS: /O2 /GS /Ob1
// h11: 63.3% (regs 65.4%, ops 95.4%). Hand from decomp: do-while loop with three strided record pointers, zone query struct, float acos result kept on the x87 stack (fcom form), parenthesized x/y/z subtractions fix the fld/fsubr order.
/* MATCH-S13 0x00481a30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* PROV-FIX-9 2026-11-12 (verify/provfix9_20261112/RESULT_PROV-FIX-9.md, diagnosis verify/provcrash_20261112/RESULT_PROV-CRASH-12.md): FUN_004c1b40(FUN_004671a0(0), &sph), as 0x00481bb5 (push &sph; push 0; call 0x4671a0; add esp,4; push eax; call 0x4c1b40); &sph was given to the wrong callee. Best now 69.7%. */
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
typedef struct { float x; float y; float z; float radius; } Sphere;
typedef struct { char pad[0x30]; float x; float y; float z; } CamFrame;
typedef void (*Callback)(void);
typedef struct {
  Sphere sph;
  int type;
  int obj;
  int index;
  Callback cb;
  int pad[0x1e];
} ZoneQuery;
typedef struct { char b[0x40]; } Rec40;
typedef struct { int w[6]; } Rec18;
typedef struct { char b[0x28]; } Rec28;
MGLOBAL(int, DAT_006c6eb0, 0x006c6eb0);
MGLOBAL(int, DAT_007f101c, 0x007f101c);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_005cc978, 0x005cc978);
MGLOBAL(int, DAT_0080332c, 0x0080332c);
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
MGLOBALA(int, DAT_006c6b90, 200, 0x006c6b90);
MGLOBALA(int, DAT_006c3030, 200, 0x006c3030);
MGLOBALA(int, DAT_006c3350, 200, 0x006c3350);
MGLOBALA(int, DAT_006c9438, 200, 0x006c9438);
MGLOBALA(int, DAT_006cad50, 200, 0x006cad50);
MGLOBALA(Rec40, DAT_006c3670, 200, 0x006c3670);
MGLOBALA(Rec18, DAT_006c9a88, 200, 0x006c9a88);
MGLOBALA(Rec28, DAT_006c74f8, 200, 0x006c74f8);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
MGLOBAL(float, DAT_005cc55c, 0x005cc55c);
MGLOBAL(float, DAT_005cd098, 0x005cd098);
MGLOBAL(float, DAT_005cc56c, 0x005cc56c);
MGLOBAL(float, DAT_005cd07c, 0x005cd07c);
MGLOBAL(float, DAT_005cf234, 0x005cf234);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cd6d4, 0x005cd6d4);
int FUN_004260c0(void);
int FUN_004c0ed0(int p0);
int FUN_0057c210(int p0);
Sphere *FUN_004e6100(int p0);
CamFrame *FUN_00467210(int p0);
int FUN_004671a0(int p0);
int FUN_004c1b40(int p0, Sphere *p1);
void FUN_00484cf0(ZoneQuery *p0);
void FUN_004818d0(int p0);
int FUN_00487280(int p0, int p1, Rec18 *p2, Rec40 *p3, Rec28 *p4);
float FUN_004726f0(float *p0, float *p1);
double FUN_004a3384(double p0);
void FUN_00487d00(Rec40 *p0, int p1);
void FUN_00487d60(void);
void FUN_0047cd50(void);
void FUN_00407670(void);
// MATCH: 0x00481a30 FUN_00481a30
void FUN_00481a30(void)
{
  int i;
  int obj;
  int atom;
  int same;
  int enabled;
  int allow;
  int tex;
  int val;
  Rec40 *r40;
  Rec18 *r18;
  Rec28 *r28;
  Sphere sph;
  float dx;
  float dy;
  float dz;
  ZoneQuery q;
  float a;

  i = 0;
  allow = FUN_004260c0();
  enabled = 1;
  if (DAT_006c6eb0 <= 0) goto done;
  r40 = DAT_006c3670;
  r28 = DAT_006c74f8;
  r18 = DAT_006c9a88;
  do {
    same = (i % 2) == (DAT_007f101c % 2);
    obj = DAT_006c71d8[i];
    if (DAT_007f0fd0 == 5) {
      if (DAT_006c6b90[i] >= DAT_005cc978) {
        same = 1;
        enabled = 0;
      } else {
        enabled = allow;
      }
    }
    if (obj == 0 || (*(unsigned char *)(obj + 2) & 4) == 0) goto next;
    tex = FUN_004c0ed0(*(int *)(obj + 4));
    atom = FUN_0057c210(obj);
    if (atom == 0) goto next;
    sph = *FUN_004e6100(obj);
    sph.radius = (sph.radius) * (DAT_005cc55c);
    dx = (sph.x) - (FUN_00467210(0)->x);
    dy = (sph.y) - (FUN_00467210(0)->y);
    dz = (sph.z) - (FUN_00467210(0)->z);
    if (!(dx * dx + dy * dy + dz * dz < DAT_005cd098)) goto next;
    DAT_006c3030[i] = DAT_006c3350[i];
    if (same) {
      DAT_006c3350[i] = FUN_004c1b40(FUN_004671a0(0), &sph);
    }
    if (DAT_006c3350[i] != 2 && DAT_006c3350[i] != 1) goto next;
    if (same) {
      q.sph.x = sph.x;
      q.sph.y = sph.y;
      q.sph.radius = sph.radius * DAT_005cc56c;
      val = DAT_006c6b90[i];
      q.obj = atom;
      q.index = i;
      q.sph.z = sph.z;
      q.cb = 0;
      q.type = 10;
      if (val >= 150000 && val < 0x24a22) {
        q.type = 0x1a;
      } else if (val >= 1000 && val <= 0x802) {
        if ((DAT_006c9438[i] & 1) == 0) {
          q.cb = FUN_0047cd50;
        }
      } else if (val >= DAT_005cc978 && val <= DAT_005cc978 + 0x32) {
        q.cb = FUN_00407670;
        q.type = 0x1c;
      }
      FUN_00484cf0(&q);
      FUN_004818d0(i);
    }
    if (enabled == 0) goto next;
    if ((DAT_006c3030[i] != 2 && DAT_006c3030[i] != 1) || *(int *)(*(int *)(atom + 0x10) + 4) != 0) {
      DAT_006cad50[i] = FUN_00487280(DAT_0080332c, tex, r18, r40, r28);
    }
    if (DAT_006cad50[i] != 0) {
      a = FUN_004a3384(FUN_004726f0((float *)((char *)r40 + 0x20), DAT_006146fc));
      if (a < DAT_005cd07c) {
        FUN_00487d00(r40, (int)((DAT_005cc320 - a * DAT_005cf234 + DAT_005cc320) * DAT_005cd6d4));
      }
    }
next:
    i++;
    r28++;
    r18++;
    r40++;
  } while (i < DAT_006c6eb0);
done:
  FUN_00487d60();
}
