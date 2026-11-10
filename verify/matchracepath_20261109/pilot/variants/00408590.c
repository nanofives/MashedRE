// FLAGS: /O2 /GS /Ob1
/* a6 0x00408590 (121 B): best 94.9% (bad12/121), 39/39 instructions, g2s 50.0%.
   Called from 0x00408610 as FUN_00408590(rec, obj): rec = 0x30c record (+0x20 handle, +0xa8 counter,
   +0xac dot), o+0x20 = V3. Key step: the dot product as three statements `d = y*y; d += x*x; d += z*z;`
   (a single expression makes the compiler call the z getter first, 57-66%).
   Residual (2 spots): instruction scheduling only. Original puts `mov ecx,[esi+0x20]` between
   `mov edi,[esp+0x14]` and `fmul [edi+0x24]`, and `fmul [edi+0x28]` before the deferred `add esp,0xc`;
   ours swaps both pairs. Tried: 6 statement forms (+=, d = a + d, d = d + a, temps), operand order of
   each product, struct / float* / int* typings of both params, V3* temp for o->v, pointer temps for
   each getter result, literal 0.0f vs extern DAT_005d757c, permuter 151 compiles: no change.
   Class (a) candidate: x87/integer interleave scheduling around the first and last call. */
typedef struct { float x, y, z; } V3;
typedef struct { char pad[0x20]; V3 v; } O2;
extern float DAT_005d757c;
V3 *FUN_00426cc0(int h);
// MATCH: 0x00408590 FUN_00408590
void FUN_00408590(char *obj, O2 *o)
{
    float d;
    d = FUN_00426cc0(*(int *)(obj + 0x20))->y * o->v.y;
    d = (float)(d + (double)(o->v.x * FUN_00426cc0(*(int *)(obj + 0x20))->x));
    d += FUN_00426cc0(*(int *)(obj + 0x20))->z * o->v.z;
    *(float *)(obj + 0xac) = d;
    if (d < DAT_005d757c)
        (*(int *)(obj + 0xa8))++;
    else
        *(int *)(obj + 0xa8) = 0;
}
