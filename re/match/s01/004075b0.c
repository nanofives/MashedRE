/* 0x004075b0 (70 B). Best 100.0%. Fix: out is a {V3; float} struct and the copy is a V3 struct assignment (esi/edi copy pattern); g2s per-dword form was 28%. */
// FLAGS: /O2 /GS /Ob1
typedef struct { float x, y, z; } V3;
typedef struct { V3 p; float w; } V4;
struct Obj { char pad[0x40]; V3 pos; };
struct Clump { int a; struct Obj *o; };
struct Rec { char pad0[0x10]; struct Clump *c10; char pad14[0x48 - 0x14]; int i48; char pad4c[0xec - 0x4c]; };
extern struct Rec DAT_00639d80[];

// MATCH: 0x004075b0 FUN_004075b0
int FUN_004075b0(int i, V4 *out)
{
    struct Rec *r = &DAT_00639d80[i];
    out->p = r->c10->o->pos;
    out->w = 2.0f;
    return r->i48 == 0;
}
