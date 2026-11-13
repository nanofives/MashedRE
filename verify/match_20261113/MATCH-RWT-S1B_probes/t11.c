typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
typedef enum { E0, E1 } En;
typedef struct { int v; } W;

#define BODY(T) \
  T *q = (T *)C->m; \
  const T *p = (const T *)G.m; const T *qq = q; int n = 16; \
  do { if (*qq != *p) goto set; p++; qq++; } while (--n); \
  return 1; \
set: \
  *(Mat16 *)q = G; \
  return 0;

int n1(void) { BODY(void *) }
int n2(void) { BODY(long) }
int n3(void) { BODY(unsigned long) }
int n4(void) { BODY(En) }
int n5(void)
{
  W *q = (W *)C->m;
  const W *p = (const W *)G.m; const W *qq = q; int n = 16;
  do { if (qq->v != p->v) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}
int n6(void)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  do { if ((*qq ^ *p) != 0) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}
int n7(void)
{
  int *q = C->m;
  const int *p = G.m; const int *qq = q; int n = 16;
  do { if (*qq > *p || *qq < *p) goto set; p++; qq++; } while (--n);
  return 1;
set:
  *(Mat16 *)q = G;
  return 0;
}
