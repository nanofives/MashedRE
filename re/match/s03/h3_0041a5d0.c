// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041a5d0 FUN_0041a5d0
/* 0x0041a5d0 (617 B). 100.0% byte-identical (--bytes, relocations masked). Converter failed to compile
   (C2106 from the CONCAT13 byte splice). FUN_004a2c48 is the CRT float->int helper (_ftol): the byte
   splice is `col = mat->color; col.a = (unsigned char)(fade * 255.0f); mat->color = col;`, and cl
   places that RGBA local in the dead idx parameter slot ([esp+0x38]). The 7 material blocks are one
   `for (j = 0; j < 7; j++)` loop that cl unrolls. The three axes are initialised V3 locals.
   Constant 0x005cd04c = 255.0f written as a literal. Class: none left.
   Port: RecC4 / Obj / Geometry field names are mechanical (offsets). Obj+0x18 is read as geometry and
   Geometry+0x20 as the material list, only as far as this function reads them. */
typedef struct V3 { float x, y, z; } V3;
typedef struct RGBA { unsigned char r, g, b, a; } RGBA;
typedef struct Material { int texture; RGBA color; } Material;
typedef struct MatList { Material **materials; } MatList;
typedef struct Geometry { int pad[8]; MatList matList; } Geometry;     /* matList.materials at +0x20 */
typedef struct Obj { int type; struct Obj *parent; int pad[4]; Geometry *geometry; } Obj; /* +0x18 */
typedef struct RecC4 {
  int    m[16];       /* +0x00 matrix */
  Obj   *a[7];        /* +0x40 */
  int    pad5c[9];
  float  ang0[4];     /* +0x80 */
  float  ang1[4];     /* +0x90 */
  float  rot[3];      /* +0xa0 */
  int    padac;
  void  *frame;       /* +0xb0 */
  int    padb4[2];
  float  fade;        /* +0xbc */
  int    on;          /* +0xc0 */
} RecC4;

#ifndef MATCH_PORT
extern RecC4 DAT_0063c630[];
void FUN_004c1480(void *frame, void *m, int c);
void FUN_004c1520(void *frame, V3 *axis, float angle, int c);
#endif

void FUN_0041a5d0(int idx)
{
  V3 right = { 1.0f, 0.0f, 0.0f };
  V3 up    = { 0.0f, 1.0f, 0.0f };
  V3 at    = { 0.0f, 0.0f, 1.0f };
  RecC4 *rec;
  Obj *f;
  Material *mat;
  RGBA col;
  int j;

  rec = &DAT_0063c630[idx];
  if (rec->on == 0)
    return;
  FUN_004c1480(rec->frame, rec->m, 0);
  f = rec->a[4]->parent;
  FUN_004c1520(f, &at, rec->rot[2], 0);
  FUN_004c1520(f, &up, rec->rot[1], 2);
  FUN_004c1520(f, &right, rec->rot[0], 2);
  for (j = 0; j < 4; j++) {
    f = rec->a[j]->parent;
    FUN_004c1520(f, &right, rec->ang0[j], 0);
    FUN_004c1520(f->parent, &up, rec->ang1[j], 0);
  }
  for (j = 0; j < 7; j++) {
    if (rec->a[j] != 0) {
      mat = rec->a[j]->geometry->matList.materials[0];
      col = mat->color;
      col.a = (unsigned char)(rec->fade * 255.0f);
      mat->color = col;
    }
  }
}
