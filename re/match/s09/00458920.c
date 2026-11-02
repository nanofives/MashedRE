// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458920: 100% by hand (raw 32.5%). The file holds the static callee 0x00458570 (register args:
   out vector in ESI, index in EAX, record on the stack), defined first so cl 9176 gives it that convention; the
   parameter order (out, idx, rec) is what puts idx in ESI in the caller (the other 5 orders give EBX, 70.1%).
   0x00458570 itself is at 90.0% (regs 90.0%): the only difference is the x87 squared-length sum, where the
   original spills y into the dead `rec` argument slot (fst [esp+0x24]; fmul [esp+0x24]) and ours squares on
   the x87 stack. Tried 7 spellings of the sum (out-> reads, dx/dy/dz locals, order of terms, len temp) and the
   permuter (no improvement): class (b). */
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
typedef struct V3_00458920 {
  float x;
  float y;
  float z;
} V3_00458920;
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(float, DAT_005ce548, 0x005ce548);
MGLOBAL(float, DAT_005ce54c, 0x005ce54c);
int FUN_0046d4a0(int *p0, int p1);
float FUN_004c3ac0(V3_00458920 *p0);
float FUN_004c39b0(V3_00458920 *p0, V3_00458920 *p1);
int FUN_00405890(void);
int FUN_00407580(int p0);
int FUN_0040e350(void);
void FUN_004588c0(int p0);
void FUN_0045c010(int p0, undefined4 p1);
int FUN_0045c330(int p0);
void FUN_00465ca0(int p0, int p1);
// MATCH: 0x00458570 FUN_00458570
static float FUN_00458570(V3_00458920 *out, int idx, int *rec)
{
  float len;
  float dx;
  float dy;
  float dz;
  int frame;
  V3_00458920 a;
  V3_00458920 b;

  len = 0.0f;
  FUN_0046d4a0(&frame, idx);
  a = *(V3_00458920 *)(frame + 0x30);
  b = *(V3_00458920 *)(*(int *)(*rec + 4) + 0x40);
  dx = a.x - b.x;
  out->x = dx;
  dy = a.y - b.y;
  out->y = dy;
  dz = a.z - b.z;
  out->z = dz;
  if (dy * dy + dx * dx + dz * dz > DAT_005ce54c) {
    len = FUN_004c3ac0(out);
    FUN_004c39b0(out, out);
  }
  return len;
}

// MATCH: 0x00458920 FUN_00458920
void FUN_00458920(int param_1, int param_2)
{
  int rec;
  int id;
  int idx;
  int e;
  V3_00458920 v;
  V3_00458920 d;

  if (*(int *)(param_1 + 0x10) != 7) return;
  rec = *(int *)(param_1 + 0x14);
  if (*(int *)(rec + 0x20) != 1) return;
  if (DAT_007f0fd0 == 5 && FUN_00405890() != 0 && *(int *)(param_2 + 0x10) == 10 &&
      *(int *)(param_2 + 0x10) == FUN_00407580(0)) {
    *(int *)(rec + 0x20) = 2;
    *(float *)(rec + 0x1c) = *(float *)(rec + 0x18) - DAT_005ce548;
    return;
  }
  id = *(int *)(param_2 + 0x10);
  if (id < 0xff00 || id > 0xff07) return;
  idx = id - 0xff00;
  if (FUN_0040e350() != 6) {
    *(int *)(rec + 0x20) = 2;
    *(float *)(rec + 0x1c) = *(float *)(rec + 0x18) - DAT_005ce548;
    return;
  }
  if (FUN_0045c330(idx) != 0) {
    v.x = 0.0f;
    v.y = 0.04f;
    v.z = 0.0f;
    FUN_00458570(&d, idx, (int *)rec);
    *(V3_00458920 *)(rec + 0x44) = v;
    return;
  }
  *(int *)(rec + 0x20) = 2;
  *(float *)(rec + 0x1c) = *(float *)(rec + 0x18);
  FUN_004588c0(rec);
  FUN_0045c010(idx, *(undefined4 *)(rec + 0x24));
  FUN_00465ca0(0x1c, rec + 0x38);
}
