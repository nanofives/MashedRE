typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

int h1(int *x)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  __asm { }
  do { if (*qq != *p) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}

int h2(int *x)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  do { if (*qq != *p) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}
