// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044c490 (94 B): 95.5% (regs 95.5%, ops 95.5%), by hand. Raw converter 13.6%.
   Sibling of 0x0044bbc0: same 0x8c-byte descriptor, position from *(*(param+4)+4) + 0x40, m_10 from
   param+0x38, callback FUN_0044c380 (a .text label; prototype [UNCERTAIN]). 0x3f59999a = 0.85f.
   Residual (b), 1 instruction: the original materializes the vector pointer (add eax,0x40 then [eax]) and this
   source folds the first load into [eax+0x40]. A direct struct copy gives the add form but with eax/ecx swapped
   for the parameter and the pointer (31.8%, regs 100%). Tried int-pointer, void-pointer and struct-typed param, field-store
   orders, frame local (50%), permuter 240 s + 200 s: no gain. relocheck not applicable (one instruction shorter, offsets shift), cxx ok. */
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
typedef struct V3_c490 {
  float x;
  float y;
  float z;
} V3_c490;
typedef struct Spawn_c490 {
  V3_c490 m_pos;
  float m_0c;
  int m_10;
  int m_14;
  int m_18;
  void (*m_1c)(void);
  int m_20[27];
} Spawn_c490;
void FUN_0044c380(void);
int FUN_00484cf0(Spawn_c490 *p0);
// MATCH: 0x0044c490 FUN_0044c490
void FUN_0044c490(int param_1)
{
  Spawn_c490 s;
  V3_c490 *p;

  p = (V3_c490 *)(*(int *)(*(int *)(param_1 + 4) + 4) + 0x40);
  s.m_pos.x = p->x;
  s.m_pos.y = p->y;
  s.m_pos.z = p->z;
  s.m_0c = 0.85f;
  s.m_10 = *(int *)(param_1 + 0x38);
  s.m_14 = param_1;
  s.m_18 = 0;
  s.m_1c = FUN_0044c380;
  FUN_00484cf0(&s);
}
