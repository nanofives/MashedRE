/* a7 0x00459400 (128 B): best 100.0% exact (regs 100, ops 100).
   Tried: g2s raw 20.3%; natural for-loops 83% (cos*k recomputed per iteration, inner loop re-tests n);
   hoisting d before the loop 81%; hoisting inside if(n>0) + do-while outer 87.8%; do-while inner too -> 100%.
   Residual: none. Constants: 0.01f @0x005cc328, 1.0f @0x005cc320, 2.0f @0x005cc574.
   FUN_004e89e0(geom,0x10) / FUN_004e8a10(geom) are the geometry lock/unlock pair (callee bodies not re-read here). */
// FLAGS: /O2 /GS /Ob1
double cos(double);
#pragma intrinsic(cos)
typedef struct { float u, v; } TexCoord;
typedef struct { char pad0[0x14]; int numVerts; char pad1[0x34 - 0x18]; TexCoord *uv; } Geom;
typedef struct { char pad0[0x18]; Geom *geom; } Atomic;
extern float DAT_0068bb5c;
void *FUN_004e89e0(Geom *g, int flags);
void *FUN_004e8a10(Geom *g);


// MATCH: 0x00459400 _FUN_00459400
Atomic *FUN_00459400(Atomic *a)
{
    Geom *g = a->geom;
    int n, i, j;
    TexCoord *uv;
    DAT_0068bb5c += 0.01f;
    n = g->numVerts;
    FUN_004e89e0(g, 0x10);
    uv = g->uv;
    i = 0;
    if (n > 0) {
        float d = (float)cos(DAT_0068bb5c) * 0.01f;
        do {
            uv[i].u += d;
            if (uv[i].u > 1.0f) {
                j = 0;
                do {
                    uv[j].u -= 2.0f;
                    j++;
                } while (j < n);
            }
            i++;
        } while (i < n);
    }
    FUN_004e8a10(g);
    return a;
}
