// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458570 (h6): best 90.0% (regs 90.0%, ops 96.7%), 60/60 instructions. Record on the stack,
   index in EAX (passed on to 0x0046d4a0), output vector in ESI (pilot finding 1): `static` defined before a
   STAND-IN caller (not matched, not shipped) that sets them as 0x00458920 (h7's) does at 0x004589bf..0x004589dd.
   Raw converter 25.0%. Everything up to the squared length matches. Tried: all six term orders (the compiler
   canonicalizes them, identical code), out-> reads vs dx/dy/dz temps, a separate sq variable, declaration order,
   permuter (161 compiles, no gain). Residual (b): the original spills dy to the dead parameter slot
   (fxch; fst [esp+0x24]; fmul [esp+0x24]) and squares x then z from the stack where ours keeps all three on the
   x87 stack and pops one more value after fcomp. Same shape as 0x004584e0's residual. */
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
typedef unsigned int uint;
typedef struct { float x, y, z; } V3;
typedef struct { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos;
                 unsigned int pad3; } Mat;
typedef struct { char pad[0x40]; V3 pos; } Frm;
typedef struct { int f0; Frm *frame; } Obj;
typedef struct {
    Obj *obj;           /* +0x00 */
} Rec;
MGLOBAL(float, DAT_005ce54c, 0x005ce54c);
int FUN_0046d4a0(Mat **out, uint idx);
float FUN_004c3ac0(V3 *v);
float FUN_004c39b0(V3 *out, V3 *in);
// MATCH: 0x00458570 FUN_00458570
static float FUN_00458570(Rec *r, uint idx, V3 *out)
{
    float len = 0.0f;
    Mat *m;
    V3 a, b;
    float dx, dy, dz;
    FUN_0046d4a0(&m, idx);
    a = m->pos;
    b = r->obj->frame->pos;
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
/* stand-in caller (not matched, not shipped): record on the stack, index in EAX, local vector in ESI, as
   0x004589bf..0x004589dd in 0x00458920 (h7's) does */
void FUN_00458570_standin(Rec *r, uint idx, V3 *res)
{
    V3 d;
    d.x = 0.0f;
    d.y = 0.04f;
    d.z = 0.0f;
    FUN_00458570(r, idx, &d);
    *res = d;
}
