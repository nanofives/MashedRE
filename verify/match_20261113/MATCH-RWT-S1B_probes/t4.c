typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F;

int d1(int *q, int v)
{
  int n = 16;
  do { if (v != *q) return 0; q++; } while (--n);
  return 1;
}

int d2(int *q, int v)
{
  int n = 16;
  do { if (*q != v) return 0; q++; } while (--n);
  return 1;
}

int d3(int *q, int v)
{
  int n = 16;
  do { if (*q - v) return 0; q++; } while (--n);
  return 1;
}

int d4(int *q, int v)
{
  int n = 16;
  do { if (*q < v) return 0; q++; } while (--n);
  return 1;
}

int d5(int *q)
{
  int *p = G.m; int n = 16;
  do { int x = *q; if (x != *p) return 0; if (x == 7) F = 1; p++; q++; } while (--n);
  return 1;
}
