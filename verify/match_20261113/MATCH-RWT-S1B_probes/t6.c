typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

int f1(int k)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  do { if (*qq != *p) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  F = k;
  return 0;
}

int f2(int k, int j)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  do { if (*qq != *p) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  F = k; H = j;
  return 0;
}
