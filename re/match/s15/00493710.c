// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493710 (h8): best 94.2% (regs 94.2%, ops 99.2%). Engine boot sequence. Locals: int at +0,
   int[4] at +4, a 6-int video-mode struct at +0x14 (the 0x2c frame). FUN_005c9d00 is the shared xor eax,eax stub
   (called with 1 arg three times). Block order needed early `return 0`s and a shared `fail:` block for the
   video-mode / engine-start failures with the success path last (Ghidra's nested form: 88.3%).
   Residual (b): the original pops the first three call arguments (add esp,0xc at 0x00493734) before
   FUN_005c9d00(0x200000); ours merges them into one add esp,0x18 after FUN_004c32b0, which also shifts every
   later jump displacement. Tried: nested/separate statement for the 0x200000 call, temp for the result, scoped
   block for the int[4], unprototyped / int-arg callee, varargs print, __inline helpers around the first calls,
   permuter (no gain). */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct RwVideoMode_ { int width; int height; int depth; int flags; int refRate; int format; } RwVideoMode_;
MGLOBALA(char, s_Calling_RwEngineInit_005cf884, 22, 0x005cf884);
MGLOBALA(char, s_Calling_RtFSManagerOpen_005cf868, 25, 0x005cf868);
MGLOBALA(char, s_Calling_HardwareInstallFileSystem_005cf844, 35, 0x005cf844);
MGLOBALA(char, s_Calling_RenderwareAttachPlugins_005cf820, 33, 0x005cf820);
MGLOBALA(char, s_Calling_RwEngineOpen_005cf808, 22, 0x005cf808);
MGLOBALA(char, s_Calling_HardwareSetVideoMode_005cf7e8, 30, 0x005cf7e8);
MGLOBALA(char, s_Calling_RwEngineStart_005cf7d0, 23, 0x005cf7d0);
MGLOBALA(char, s_Using_video_mode_005cf7b0, 30, 0x005cf7b0);
MGLOBALA(char, s_At_10_bytes_per_pixel_005cf750, 95, 0x005cf750);
MGLOBAL(int, DAT_007719d8, 0x007719d8);
MGLOBAL(int, DAT_007719dc, 0x007719dc);
int FUN_005c9d00(void *p);
void FUN_004963e0(char *msg);
int FUN_004c32b0(int a, int b);
int FUN_00550350(int a);
void thunk_FUN_00495780(void);
void FUN_00495270(int *p);
int FUN_00493640(void);
int FUN_004c30b0(int *p);
void FUN_00550390(void);
void FUN_004c3270(void);
void FUN_004c3040(void);
int FUN_004951f0(void);
void FUN_004c9f50(int a);
void FUN_004c9eb0(int a);
void FUN_004c9f60(int a);
int FUN_004c2fb0(void);
void FUN_0045b350(void);
short FUN_004c2f00(void);
void FUN_004c2ed0(RwVideoMode_ *mode, int index);
int FUN_004a2cbd(char *fmt, ...);
int FUN_00493600(void);
void FUN_005584c0(void);
void FUN_004c5c80(int a);
int FUN_004cbc90(void);
void FUN_004cbc80(void (*cb)(void));
int FUN_004cbc70(void);
void FUN_004cbc60(void (*cb)(void));
void FUN_004936f0(void);
void FUN_00493700(void);
// MATCH: 0x00493710 FUN_00493710
undefined4 FUN_00493710(void)
{
  int sVar1;
  int uVar2;
  int uVar3;
  int local_2c;
  int local_28[4];
  RwVideoMode_ mode;

  FUN_005c9d00(local_28);
  uVar2 = FUN_005c9d00(local_28);
  FUN_004963e0(s_Calling_RwEngineInit_005cf884);
  uVar3 = FUN_005c9d00((void *)0x200000);
  if (FUN_004c32b0(uVar2, uVar3) == 0) {
    return 0;
  }
  FUN_004963e0(s_Calling_RtFSManagerOpen_005cf868);
  if (FUN_00550350(-1) == 0) {
    return 0;
  }
  FUN_004963e0(s_Calling_HardwareInstallFileSystem_005cf844);
  thunk_FUN_00495780();
  FUN_00495270(&local_2c);
  FUN_004963e0(s_Calling_RenderwareAttachPlugins_005cf820);
  FUN_00493640();
  FUN_004963e0(s_Calling_RwEngineOpen_005cf808);
  if (FUN_004c30b0(&local_2c) == 0) {
    FUN_00550390();
    FUN_004c3270();
    return 0;
  }
  FUN_004963e0(s_Calling_HardwareSetVideoMode_005cf7e8);
  if (FUN_004951f0() == 0) {
    goto fail;
  }
  FUN_004c9f50(1);
  FUN_004c9eb0(0x3c);
  FUN_004c9f60(1);
  FUN_004963e0(s_Calling_RwEngineStart_005cf7d0);
  if (FUN_004c2fb0() == 0) {
fail:
    FUN_00550390();
    FUN_004c3040();
    FUN_004c3270();
    return 0;
  }
  FUN_0045b350();
  FUN_0045b350();
  sVar1 = FUN_004c2f00();
  FUN_004c2ed0(&mode, sVar1);
  FUN_004a2cbd(s_Using_video_mode_005cf7b0, sVar1, mode.width, mode.height);
  FUN_004a2cbd(s_At_10_bytes_per_pixel_005cf750, 0x400000, mode.width * mode.height * 12,
               0x400000 - mode.width * mode.height * 10);
  FUN_00493600();
  FUN_005584c0();
  FUN_004c5c80(0);
  DAT_007719d8 = FUN_004cbc90();
  FUN_004cbc80(FUN_004936f0);
  DAT_007719dc = FUN_004cbc70();
  FUN_004cbc60(FUN_00493700);
  return 1;
}
