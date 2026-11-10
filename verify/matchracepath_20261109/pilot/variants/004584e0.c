// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004584e0 (h6): best 87.8% (regs 87.8%, ops 93.9%), 49/49 instructions. Record arrives in EAX,
   output vector in ESI (pilot finding 1): the function is a `static` defined before a STAND-IN caller (not
   matched, not shipped) that sets EAX/ESI the way 0x00458a40 does at 0x00458ae0..0x00458ae6 (0x00458a40 is h7's).
   Raw converter 24.5%. Tried: local V3 copy via pointer/temp, len load order, dz temp, compare operand order,
   parameter order, pointer to r->pos, permuter (112 compiles, no gain). Residual (b): the original issues
   fld [eax+0x2c] before the 12-byte frame-pos copy and interleaves the copy stores after the first fsub, and it
   pops the three squared-length operands as fstp st(2)/st(1)/st(0) where ours pops st(0) x3. */
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
typedef struct { float x, y, z; } V3;
typedef struct { char pad[0x40]; V3 pos; } Frm;
typedef struct { int f0; Frm *frame; } Obj;
typedef struct {
    Obj *obj;           /* +0x00 */
    float f04, f08;
    char pad0c[0x2c - 0x0c];
    V3 pos;             /* +0x2c */
    V3 f38;             /* +0x38 */
    V3 vel;             /* +0x44 */
} Rec;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005ce54c, 0x005ce54c);
float FUN_004c3ac0(V3 *v);
float FUN_004c39b0(V3 *out, V3 *in);
// MATCH: 0x004584e0 FUN_004584e0
static float FUN_004584e0(Rec *r, V3 *out)
{
    float len = DAT_005d757c;
    V3 a = r->obj->frame->pos;
    { float tA_ = r->pos.x; out->x = tA_ - a.x; }
    out->y = r->pos.y - a.y;
    out->z = r->pos.z - a.z;
    if (out->x * out->x + out->y * out->y + out->z * out->z > DAT_005ce54c) {
        len = FUN_004c3ac0(out);
        FUN_004c39b0(out, out);
    }
    return len;
}
/* stand-in caller (not matched, not shipped): passes the record in EAX and a local vector in ESI, as
   0x00458ae0..0x00458ae6 in 0x00458a40 does */
void FUN_004584e0_standin(Rec *r)
{
    V3 d;
    float s = FUN_004584e0(r, &d) * 2.0f;
    r->vel.x = d.x * s;
    r->vel.y = d.y * s;
    r->vel.z = s * d.z;
}
