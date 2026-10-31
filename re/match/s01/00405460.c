// FLAGS: /O2 /GS /Ob1
/* a3 MATCH-S01: 0x00405460 (223 B) 100% (g2s did not compile). The 64-byte local is an RwMatrix-shaped
   struct of floats (a char[64] buys a /GS cookie). param_1 converts as unsigned (fild + jge + fadd 2^32 fixup,
   0x00405507..0x00405516). The range test is `!(d74 > x)`: `d74 <= x` gives jp instead of je. Callees cdecl;
   FUN_00404fa0 is in this range (separate file 00404fa0.c).
   0x005cc950 (0.75), 0x005d757c (0.0), 0x005cc948 (1/3000) are .rdata float literals. */
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
extern int DAT_00639d70;
extern int DAT_00639d78;
extern float DAT_00803344;
extern float DAT_00639d74;
void FUN_00404fa0(void *p0, int p1, float p2);
int FUN_004c1480(int p0, void *p1, int p2);
int FUN_004c1c80(int p0, float *p1);
// MATCH: 0x00405460 FUN_00405460
int FUN_00405460(int param_1)
{
  int obj;
  float scale[2];
  RwMatrix mat;

  if (DAT_00639d70 != 0 && DAT_00639d78 != 0) {
    scale[0] = 0.75f / DAT_00803344;
    scale[1] = 0.75f;
    FUN_004c1c80(DAT_00639d78, scale);
    obj = *(int *)(DAT_00639d78 + 4);
    if (!(DAT_00639d74 > *(float *)(DAT_00639d70 + 0xc))) {
      FUN_00404fa0(&mat, DAT_00639d70, DAT_00639d74);
      if (0.0f == DAT_00639d74) {
        FUN_004c1480(obj, &mat, 0);
      }
      FUN_004c1480(obj, &mat, 0);
      DAT_00639d74 = (float)(unsigned int)param_1 * 0.00033333333f + DAT_00639d74;
      return 1;
    }
  }
  return 0;
}
