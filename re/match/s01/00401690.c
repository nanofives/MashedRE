// FLAGS: /O2 /GS /Ob1
/* a1 group file: 0x00401690 (820 B) + 0x004025f0 (208 B Ghidra, 211 B real).
   0x004025f0: BYTES-OK (whole symbol; score shows 96.2% only because Ghidra size 208 stops before
     `add esp,0x34 / ret`). Start (g2s) 83.3%. Needs 0x00401690 as a `static` defined earlier in the file
     (no args pushed: 9176 passes the slot pointer in ESI), the 13-entry 0x68-stride Slot array at
     0x00636578 walked by index n<13, and a local int[13] id table.
   0x00401690: best 60.2% (start g2s 20.1%). Slot bbox from the first atomic's morph-target vertices
     (FUN_004b3f90(clump)->+0x18 geometry, +0x14 numVertices, +0x5c morph target, +0x14 verts).
     Fixed: ESI register arg (static), max/min init stores in x,y,z order (separate statements), constants
     0x005cc32c/0x005cc320/0x005cc33c are compiler literals 0.5f/1.0f/-1.0f (as externs 9176 CSEs and
     reorders them), centre written through `V3 *c = &s->centre` (stops 9176 CSE'ing bmax-bmin into ext).
     With those, everything outside the vertex loop is identical (prologue, tail, remainder loop).
     Residual: the variable-trip vertex loop. Original unrolls x4 (`mov ebx,3 / cmp edx,4`, step 0x30);
     9176 unrolls the same body x8 (`mov ebx,7 / cmp edx,8`, step 0x60). Tried: index vs pointer vs
     per-element V3* access, unsigned n/i, ++i, if/else chains, max-then-min order, V3* bmax/bmin locals,
     a float* component loop, /G6 /G7 /GB; also 2, 4, 7 and 8 compares per iteration: 9176 gives x8 for
     2..7 and no unroll at 8, never x4. Class (a): compiler-version candidate (unroll factor of a
     variable-trip loop). */
typedef struct { float x, y, z; } V3;
typedef struct Slot {
  int clump;        /* +0x00 */
  V3 centre;        /* +0x04 */
  V3 bmin;          /* +0x10 */
  V3 bmax;          /* +0x1c */
  V3 ext;           /* +0x28 */
  float scale;      /* +0x34 */
  int id;           /* +0x38 */
  int pad3c;
  int a[4];         /* +0x40 */
  int b[6];         /* +0x50 */
} Slot;             /* 0x68 */
typedef struct MorphT { int pad[5]; V3 *verts; } MorphT;
typedef struct Geom { int pad[5]; int numVerts; int pad2[0x11]; MorphT *morph; } Geom;
typedef struct Atomic { int pad[6]; Geom *geom; } Atomic;

extern Slot DAT_00636578[13];
Atomic *FUN_004b3f90(int clump);
float FUN_004c3ac0(V3 *v);
int FUN_00401630(int id);
int FUN_00402240(void);
int FUN_00402590(void);
// MATCH: 0x00401690 FUN_00401690
static void FUN_00401690(Slot *s)
{
  Atomic *at;
  int n, i;
  V3 *v;
  if (s->clump != 0) {
    at = FUN_004b3f90(s->clump);
    n = at->geom->numVerts;
    v = at->geom->morph->verts;
    s->bmax.x = 1.17549435e-38f;
    s->bmax.y = 1.17549435e-38f;
    s->bmax.z = 1.17549435e-38f;
    s->bmin.x = 3.40282347e+38f;
    s->bmin.y = 3.40282347e+38f;
    s->bmin.z = 3.40282347e+38f;
    for (i = 0; i < n; i++) {
      if (v[i].x > s->bmax.x) s->bmax.x = v[i].x;
      if (v[i].x < s->bmin.x) s->bmin.x = v[i].x;
      if (v[i].y > s->bmax.y) s->bmax.y = v[i].y;
      if (v[i].y < s->bmin.y) s->bmin.y = v[i].y;
      if (v[i].z > s->bmax.z) s->bmax.z = v[i].z;
      if (v[i].z < s->bmin.z) s->bmin.z = v[i].z;
    }
    { V3 *c = &s->centre;
    c->x = (s->bmax.x - s->bmin.x) * 0.5f + s->bmin.x;
    c->y = (s->bmax.y - s->bmin.y) * 0.5f + s->bmin.y;
    c->z = (s->bmax.z - s->bmin.z) * 0.5f + s->bmin.z; }
    s->ext.x = s->bmax.x - s->bmin.x;
    s->ext.y = s->bmax.y - s->bmin.y;
    s->ext.z = s->bmax.z - s->bmin.z;
    s->scale = 1.0f / FUN_004c3ac0(&s->ext);
    s->centre.x = s->centre.x * -1.0f;
    s->centre.y = s->centre.y * -1.0f;
    s->centre.z = s->centre.z * -1.0f;
  }
}
// MATCH: 0x004025f0 FUN_004025f0
void FUN_004025f0(void)
{
  int ids[13];
  int n, i;
  ids[0] = 0; ids[1] = 2; ids[2] = 3; ids[3] = 0xb; ids[4] = 0x19; ids[5] = 0x1a; ids[6] = 0x1e;
  ids[7] = 0x21; ids[8] = 0x22; ids[9] = 0x24; ids[10] = 0x25; ids[11] = 0x26; ids[12] = 0x27;
  for (n = 0; n < 13; n++) {
    DAT_00636578[n].id = ids[n];
    DAT_00636578[n].clump = FUN_00401630(ids[n]);
    FUN_00401690(&DAT_00636578[n]);
    for (i = 0; i < 4; i++) DAT_00636578[n].a[i] = FUN_00402240();
    for (i = 0; i < 6; i++) DAT_00636578[n].b[i] = FUN_00402590();
  }
}
