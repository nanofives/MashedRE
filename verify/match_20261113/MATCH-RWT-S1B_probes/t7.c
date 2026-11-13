typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

int g1(void)
{
  int *q = C->m;
  int t = H * 3;
  const int *p = G.m; const int *qq = q; int n = 16;
  do { if (*qq != *p) goto set; p++; qq++; } while (--n);
  return t;
set:
  *(Mat16 *)q = G;
  F = t;
  return 0;
}
