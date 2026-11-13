/* S1B attempt 2: a function-scope static with __declspec(align(N)): c1 may flag the function as having an aligned
   local while no stack slot needs realignment.   compile: asm.py t26.c rw9044 /O2 /Ob1 /G5 */
typedef struct { int m[16]; } Mat16;
extern Mat16 *C;
extern int F, H;

#define LOOP(G) \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int k8(int *x) { static __declspec(align(8)) Mat16 G; int *q = C->m; G.m[0] = x[0]; LOOP(G) return 1; set: *(Mat16 *)q = G; return 0; }
int k16(int *x) { static __declspec(align(16)) Mat16 G; int *q = C->m; G.m[0] = x[0]; LOOP(G) return 1; set: *(Mat16 *)q = G; return 0; }
int k4(int *x) { static __declspec(align(4)) Mat16 G; int *q = C->m; G.m[0] = x[0]; LOOP(G) return 1; set: *(Mat16 *)q = G; return 0; }
int kn(int *x) { static Mat16 G; int *q = C->m; G.m[0] = x[0]; LOOP(G) return 1; set: *(Mat16 *)q = G; return 0; }
