typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
int memcmp(const void *, const void *, unsigned int);
#pragma intrinsic(memcmp)

int w1(void)
{
  int *q = C->m;
  if (memcmp(q, &G, sizeof(Mat16)) == 0) return 1;
  *(Mat16 *)q = G;
  return 0;
}

int w2(void)
{
  int *q = C->m;
  if (!memcmp(&G, q, sizeof(Mat16))) return 1;
  *(Mat16 *)q = G;
  return 0;
}
