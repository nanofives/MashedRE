typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F;

int a1(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (*q != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int a2(void)
{
  const int *p = G.m; const int *q = C->m; int n = 16;
  do { if (*q != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int a3(void)
{
  int i; int *q = C->m;
  for (i = 0; i < 16; i++) if (q[i] != G.m[i]) return 0;
  return 1;
}

int a4(void)
{
  const int *p = G.m; int *q = C->m; int n = 16;
  do { if (*q != *p) goto set; p++; q++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}

int a5(void)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  do { if (*qq != *p) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}
