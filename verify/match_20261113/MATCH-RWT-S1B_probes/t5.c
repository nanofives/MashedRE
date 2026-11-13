typedef struct { int m[16]; } Mat16;
typedef struct { float m[16]; } MatF;
extern Mat16 G;
extern Mat16 *C;
extern int F;

int e1(int *q)
{
  const unsigned int *p = (const unsigned int *)G.m; int n = 16;
  do { if (*q != (int)*p) return 0; p++; q++; } while (--n);
  return 1;
}

int e2(int *q)
{
  const int *p = G.m; int n = 16;
  do { if ((short)n, *q != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int e3(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (*q != *p) return 0; p++; q++; n--; } while (n);
  return 1;
}

int e4(int *q)
{
  const char *p = (const char *)G.m; int n = 16;
  do { if (*q != *(const int *)p) return 0; p += 4; q++; } while (--n);
  return 1;
}

int e5(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (*q != *p) return 0; ++p; ++q; } while (--n > 0);
  return 1;
}

int e6(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (*q != *p) return 0; p++; q++; } while (--n != 0);
  return 1;
}

int e7(int *q)
{
  const int *p = G.m; unsigned int n = 16;
  do { if (*q != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int e8(int *q)
{
  const int *p = G.m; int n = 16;
  do { if (*q != *p) return 0; p++; q++; } while (--n);
  return 1;
}

int e9(int *q)
{
  const int *p = G.m; int n;
  for (n = 16; n; n--) { if (*q != *p) return 0; p++; q++; }
  return 1;
}
