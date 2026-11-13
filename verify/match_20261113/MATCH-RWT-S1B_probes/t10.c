typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

int m1(int *x)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
top:
  if (*qq != *p) goto set;
  p++; qq++;
  if (--n) goto top;
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}

int m2(int *x)
{
  int *q = C->m;
  int i = 0;
top:
  if (q[i] != G.m[i]) goto set;
  i++;
  if (i < 16) goto top;
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}

int m3(int *x)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  goto mid;
top:
  p++; qq++;
  if (--n == 0) return 1;
mid:
  if (*qq == *p) goto top;
  *(Mat16 *)q = G;
  return 0;
}
