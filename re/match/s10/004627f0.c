// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004627f0 (h6): 96.8% (regs 96.8%). Raw did not compile. Fixed arities from the pops: 5aa060
   takes 3 args and 5a9e40 takes (handle, &caps) (the &caps push precedes the inner call); caps is a 0x28-byte
   struct (int at +8, ushort at +0x1a); FUN_004a2b60 is varargs (sprintf shape).
   Residual (b): the original keeps `or [eax+0x70],0x10` and `or [eax+0x70],0x40` separate across the float
   stores; ours folds them into one `or 0x50`. Tried: `|=` vs `x = x | c`, bitfields, volatile flags (all
   load/or/store), __inline setters (all three folded), comma statements, the last `|=` via the global, the
   permuter (219 compiles, no gain). */
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
typedef struct {
  int f0;
  int f4;
  int useSoftware;
  char pad0c[0xe];
  unsigned short maxVoices;
  char pad1c[0xc];
} S10SndCaps;
MGLOBAL(int, DAT_00632ba0, 0x00632ba0);
MGLOBAL(int, DAT_0069049c, 0x0069049c);
MGLOBAL(int, DAT_006904a0, 0x006904a0);
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
typedef struct {
  char pad00[0x64];
  float f64;
  float f68;
  float f6c;
  unsigned int flags;
} S10SndObj;
MGLOBAL(S10SndObj *, DAT_0068f61c, 0x0068f61c);
MGLOBALA(char, DAT_005e6e50, 1, 0x005e6e50);
void FUN_004624c0(void);
void FUN_004963e0(char *p0);
int FUN_004a2b60(char *p0, const char *p1, ...);
int FUN_005a5f00(int p0, int p1);
int FUN_005a6110(void);
S10SndObj *FUN_005a8e70(int p0, int p1, int p2);
int FUN_005a9e40(int p0, S10SndCaps *p1);
int FUN_005aa060(int p0, char *p1, int p2);
int FUN_005b9e30(int p0, int p1, int p2, int p3, int p4);
int FUN_005baf40(int p0, S10SndObj *p1);
int FUN_005baf60(int p0, int p1);
int FUN_005bbb20(void);
__declspec(dllimport) void __stdcall OutputDebugStringA(const char *lpOutputString);
// MATCH: 0x004627f0 FUN_004627f0
void FUN_004627f0(void)
{
  S10SndCaps caps;
  char buf[256];
  S10SndObj *p;

  DAT_00632ba0 = FUN_005a5f00(0, 1);
  FUN_005a6110();
  FUN_005bbb20();
  FUN_004624c0();
  DAT_0069049c = FUN_005b9e30(0, 0, 6, 0, 0);
  FUN_005a9e40(FUN_005aa060(0, DAT_005e6e50, 0), &caps);
  FUN_004a2b60(buf, "SOUND CARD - MAX voices %d \n", caps.maxVoices);
  OutputDebugStringA(buf);
  FUN_004963e0(buf);
  FUN_004a2b60(buf, "SOUND CARD - useSoftware flag = %d \n", caps.useSoftware);
  OutputDebugStringA(buf);
  FUN_004963e0(buf);
  DAT_006904a0 = caps.maxVoices - 4;
  DAT_0068f4c8 = 0;
  if (DAT_006904a0 < 0x30 || caps.useSoftware != 0) {
    DAT_0068f4c8 = 1;
  }
  p = FUN_005a8e70(4, 0, 0);
  p->f64 = 10.0f;
  p->flags |= 0x10;
  p->f6c = 4.0f;
  p->flags |= 0x40;
  p->f68 = 0.0001f;
  DAT_0068f61c = p;
  p->flags |= 0x20;
  FUN_005baf40(DAT_0069049c, p);
  FUN_005baf60(DAT_0069049c, 4);
}
