typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F;

static __inline int eq(int a, int b) { return a == b; }

int c1(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (!eq(*q, *p)) return 0; p++; q++; } while (--n);
  return 1;
}

int c2(int *q)
{
  const int *p = G.m; int n = 16; int x;
  do { x = *q; if (x != *p) return 0; p++; q++; } while (--n);
  return x;
}

int c3(int *q)
{
  const int *p = G.m; int n = 16; register int x;
  do { x = *q; if (x != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int c4(int *q)
{
  const int *p = G.m; int n = 16; int x;
  x = F;
  do { x = *q; if (x != *p) return 0; p++; q++; } while (--n);
  F = x;
  return 1;
}

int c5(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (*q != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int c6(int *q)
{
  const int *p = G.m; int n = 16;
  do { int x = *q++; if (x != *p++) return 0; } while (--n);
  return 1;
}
