/* S1B attempt 2: an aligned local in a function with a parameter, where the local is optimized away.
   Want: frame kept, no and esp, no ebx, folded compare.
   compile: asm.py t24.c rw9044 /O2 /Ob1 /G5 */
typedef struct { int m[16]; } Mat16;
typedef __declspec(align(16)) struct { float f[4]; } A4;
typedef __declspec(align(16)) struct { int m[16]; } AMat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

#define LOOP \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int g0(int *x) { int *q = C->m; G.m[0] = x[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g1(int *x) { A4 a; int *q = C->m; G.m[0] = x[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g2(int *x) { A4 a; int *q = C->m; a.f[0] = 1.0f; G.m[0] = x[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g3(int *x) { A4 a; int *q = C->m; a.f[0] = *(float *)&x[0]; *(float *)&G.m[0] = a.f[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g4(int *x) { AMat16 a; int *q = C->m; a.m[0] = x[0]; G.m[0] = a.m[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g5(int *x) { __declspec(align(16)) int t; int *q = C->m; t = x[0]; G.m[0] = t; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g6(int *x) { __declspec(align(8)) int t; int *q = C->m; t = x[0]; G.m[0] = t; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g7(int *x) { __declspec(align(16)) float t; int *q = C->m; t = *(float *)&x[0]; *(float *)&G.m[0] = t; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int g8(A4 *x) { int *q = C->m; G.m[0] = *(int *)&x->f[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
