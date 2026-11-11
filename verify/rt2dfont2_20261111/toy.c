void *g(void *s, void *b, int n);
int *mk(void);
void kill(int *p);
int *f(void *s)
{
  int *p; int v; int i;
  p = mk();
  if (p == 0) return 0;
  if (!g(s, &v, 4)) return 0;
  if ((int)(v >> 24) < 1) return 0;
  if (!g(s, p, 4)) return 0;
  if (!g(s, p+1, 4)) return 0;
  if (p[3]) { kill(p); return 0; }
  for (i = 0; i < p[2]; i++) { if (!g(s, p + i, 4)) return 0; }
  return p;
}
