typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F;

int b1(int *a, int *b) { if (*a != *b) return 0; return 1; }

int b2(int *q)
{
  int n = 16;
  do { if (*q != F) return 0; q++; } while (--n);
  return 1;
}

int b3(int *q, int v)
{
  int n = 16;
  do { if (*q != v) return 0; q++; } while (--n);
  return 1;
}

int b4(int *q)
{
  const int *p = G.m; int n = 16;
  do { int x = *q; if (x != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int b5(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (*q != *p) break; p++; q++; } while (--n);
  return n == 0;
}

int b6(unsigned int *q)
{
  const unsigned int *p = (const unsigned int *)G.m; int n = 16;
  while (*q == *p) { p++; q++; if (--n == 0) return 1; }
  return 0;
}
