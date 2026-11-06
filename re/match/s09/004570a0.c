// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004570a0: 100% by hand (h5), relocheck OK. Raw 10.5% (in_EAX). static with one pointer parameter so cl
   passes it in EAX (pilot finding 1). The real caller (call at 0x00457495, mov eax, ebx first) is not in this file,
   so a stand-in caller sits below it (not the real TU). Builds a 0x8c-byte query record on the stack (pos copied as
   a struct from +0x48, callback 0x00457010 at +0x1c) and passes it to FUN_00484cf0. The real caller is 0x00457100;
   re/match/s09/00457100.c holds the same body (100%) next to it, so a port takes one copy, not both. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_004570a0 { float x, y, z; } V3_004570a0;
typedef struct Obj004570a0 {
  int pad0[5];
  int m_14;
  int pad1[12];
  V3_004570a0 m_48;
} Obj004570a0;
typedef struct Query004570a0 {
  V3_004570a0 pos;
  int m_c;
  int m_10;
  Obj004570a0 *m_14;
  int m_18;
  void (*m_1c)(void *q, void *hit);
  int rest[27];
} Query004570a0;
void FUN_00457010(void *q, void *hit);
int FUN_00484cf0(Query004570a0 *q);

// MATCH: 0x004570a0 FUN_004570a0
static void FUN_004570a0(Obj004570a0 *o)
{
  Query004570a0 q;

  q.pos = o->m_48;
  q.m_c = o->m_14;
  q.m_14 = o;
  q.m_18 = 0;
  q.m_10 = 0x12;
  q.m_1c = FUN_00457010;
  FUN_00484cf0(&q);
}

/* stand-in caller (not the real TU): the real caller (call at 0x00457495) does mov eax, ebx first */
void FUN_004570a0_caller(Obj004570a0 *o)
{
  FUN_004570a0(o);
}
