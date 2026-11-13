// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d40d0 (h4): 89.1% (regs 97.8%). Runs a job: optional flush FUN_004e1710(DAT_007d4710) when the
   flag and owner+0x18 are set; status 0x00911ac8 = 1, current job 0x00911ac0, context {data, owner} at 0x00911ad0;
   job+0x10 = 0; job->visitor->vt[1](visitor, &ctx) (cdecl), status = 0 on a zero result; when job+0x10 > 1 it is set
   to 2 and FUN_004e2030(job+0x14); clears the globals and returns status ? job : NULL.
   Residual: the result test is `cmp eax, edi` (edi = 0) in the original, `test eax, eax` here, which also shifts the
   vtable and arg registers (ecx/edx vs edx/eax). Tried `== 0`, a visitor local, a result local, a pointer return
   type, 5 flag sets, permuter 111 compiles. Class (b). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct Visitor Visitor;
typedef struct VisitorVtbl { void *fn0; int (*fn4)(Visitor *self, void *ctx); } VisitorVtbl;
struct Visitor { VisitorVtbl *vt; };
typedef struct Job { int field_0; int field_4; Visitor *visitor; int field_c; int state; void *arg; } Job;
typedef struct JobCtx { void *data; void *owner; } JobCtx;
typedef struct Owner { int pad[6]; int pending; } Owner;
MGLOBAL(Owner *, DAT_007d4710, 0x007d4710);
MGLOBAL(int, DAT_00911ac8, 0x00911ac8);
MGLOBAL(Job *, DAT_00911ac0, 0x00911ac0);
MGLOBAL(JobCtx, DAT_00911ad0, 0x00911ad0);
void FUN_004e1710(Owner *owner);
void FUN_004e2030(void *arg);

// MATCH: 0x004d40d0 FUN_004d40d0
Job *FUN_004d40d0(Job *job, void *data, int flush)
{
  if (flush && DAT_007d4710->pending) {
    FUN_004e1710(DAT_007d4710);
  }
  DAT_00911ac8 = 1;
  DAT_00911ac0 = job;
  DAT_00911ad0.data = data;
  DAT_00911ad0.owner = DAT_007d4710;
  job->state = 0;
  if (job->visitor->vt->fn4(job->visitor, &DAT_00911ad0) == flush * 0) {
    DAT_00911ac8 = 0;
  }
  if (job->state > 1) {
    job->state = 2;
    FUN_004e2030(job->arg);
  }
  DAT_00911ac0 = 0;
  DAT_00911ad0.data = 0;
  DAT_00911ad0.owner = 0;
  return DAT_00911ac8 ? job : 0;
}
