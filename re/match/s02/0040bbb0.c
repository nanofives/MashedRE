// FLAGS: /O2 /GS /Ob1
/* 0x0040bbb0 (333 B): 100.0% exact, regs 100.0%. Raw g2s 98.8%.
   The one difference was the printf/exit tail: FUN_004a332b (exit) declared __declspec(noreturn) makes
   9176 pop printf's argument before the exit call (add esp,4) instead of merging it after. The three
   FUN_0040bb30 results go through a local before their consumer call (nested calls in the argument list
   flush the deferred stack cleanup early: 90.9%). DAT_005ccca8 is the .rdata string "test", written as a
   literal. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0063b8f0, 0x0063b8f0);
MGLOBAL(int, DAT_0063b8f4, 0x0063b8f4);
MGLOBAL(int, DAT_0063b8f8, 0x0063b8f8);
MGLOBAL(int, DAT_0063b8fc, 0x0063b8fc);
MGLOBAL(int, DAT_0063b900, 0x0063b900);
MGLOBAL(int, DAT_0063b904, 0x0063b904);

int FID_conflict__wprintf(const char *fmt, ...);
void FUN_00401000(void);
int FUN_0040bb30(const char *name);
void FUN_00413b80(void);
void FUN_004210b0(void);
int FUN_0042a6b0(const char *name, int p1, int p2);
void FUN_004759b0(int p0);
void FUN_00475a00(int p0, int p1);
void FUN_00476390(void);
void FUN_004775b0(void);
void FUN_00477e40(void);
void FUN_00485d90(void);
void FUN_004862d0(void);
void FUN_00487e00(int p0, int p1);
void FUN_00489290(void);
void FUN_0048a460(void);
void FUN_0048a830(int p0);
void FUN_0048ae00(void);
void FUN_0048bbe0(void);
void FUN_0048e820(int p0);
void FUN_0048eac0(void);
void FUN_0048ff20(void);
void FUN_00495280(const char *path);
void FUN_004952f0(void);
__declspec(noreturn) void FUN_004a332b(int code);
void FUN_004c5bc0(int dict, int tex);
void FUN_004c5c80(int p0);
int FUN_004c5cb0(const char *name, int p1);

// MATCH: 0x0040bbb0 FUN_0040bbb0
void FUN_0040bbb0(void)
{
  int tex;

  DAT_0063b8f0 = 0;
  FUN_00495280("d:\\toastart\\common\\sfx.piz");
  DAT_0063b8f8 = FUN_0042a6b0("fx.txd", 0, 0);
  FUN_0048bbe0();
  DAT_0063b8fc = FUN_0042a6b0("badges.txd", 0, 0);
  DAT_0063b900 = FUN_0042a6b0("TrackImages.txd", 0, 0);
  DAT_0063b904 = FUN_0042a6b0("Interface.txd", 0, 0);
  tex = FUN_004c5cb0("test", 0);
  if (tex != 0) {
    FUN_004c5bc0(DAT_0063b8f8, tex);
  }
  FUN_004c5c80(0);
  FUN_00401000();
  FUN_0048ff20();
  FUN_004775b0();
  FUN_00476390();
  FUN_004759b0(2);
  tex = FUN_0040bb30("scorch");
  FUN_00475a00(tex, 0x18);
  tex = FUN_0040bb30("wfall");
  FUN_00475a00(tex, 0x10);
  FUN_00477e40();
  FUN_0048eac0();
  FUN_0048ae00();
  FUN_0048a830(0);
  FUN_004952f0();
  if (DAT_0063b8f8 == 0) {
    FID_conflict__wprintf("cant open sfx dic\n");
    FUN_004a332b(0);
  }
  FUN_00489290();
  tex = FUN_0040bb30("RWObjShad");
  FUN_00487e00(0x32, tex);
  FUN_0048e820(0x20);
  FUN_00485d90();
  FUN_004862d0();
  FUN_00413b80();
  FUN_004210b0();
  FUN_0048a460();
  DAT_0063b8f4 = 1;
}
