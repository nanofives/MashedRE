// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044bbc0 (106 B): 100% (regs 100%), by hand. Raw converter 15.4%.
   What moved it: a 0x8c-byte local descriptor struct (only the first 8 dwords are written here; the tail
   m_20[27] is sized from sub esp,0x8c) and a 12-byte struct copy from (FUN_004c0ed0(...) + 0x30), which gives
   the add eax,0x30 + three dword moves. 0x3f59999a = 0.85f. FUN_0044ba30 is a .text label used as a callback;
   its real prototype is [UNCERTAIN] (only its address is taken). relocheck OK, cxx ok. */
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
typedef struct V3_bbc0 {
  float x;
  float y;
  float z;
} V3_bbc0;
typedef struct Spawn_bbc0 {
  V3_bbc0 m_pos;
  float m_0c;
  int m_10;
  int m_14;
  int m_18;
  void (*m_1c)(void);
  int m_20[27];
} Spawn_bbc0;
void FUN_0044ba30(void);
int FUN_004c0ed0(int p0);
int FUN_00484cf0(Spawn_bbc0 *p0);
// MATCH: 0x0044bbc0 FUN_0044bbc0
void FUN_0044bbc0(int param_1)
{
  Spawn_bbc0 s;

  s.m_pos = *(V3_bbc0 *)(FUN_004c0ed0(*(int *)(*(int *)(param_1 + 8) + 4)) + 0x30);
  s.m_0c = 0.85f;
  s.m_10 = 8;
  s.m_14 = param_1;
  s.m_18 = 0;
  s.m_1c = FUN_0044ba30;
  FUN_00484cf0(&s);
}
