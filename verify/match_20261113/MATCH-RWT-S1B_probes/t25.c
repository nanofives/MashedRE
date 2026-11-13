/* S1B attempt 2: __declspec(align(N)) with N <= 4 on a local or a type. Does the aligned-frame path (frame, no ebx)
   still run with no and esp?   compile: asm.py t25.c rw9044 /O2 /Ob1 /G5 */
typedef struct { int m[16]; } Mat16;
typedef __declspec(align(4)) struct { int m[16]; } A4Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

#define LOOP \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int h0(int *x) { int *q = C->m; G.m[0] = x[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int h1(int *x) { __declspec(align(4)) int t; int *q = C->m; t = x[0]; G.m[0] = t; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int h2(int *x) { __declspec(align(2)) int t; int *q = C->m; t = x[0]; G.m[0] = t; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int h3(int *x) { __declspec(align(1)) int t; int *q = C->m; t = x[0]; G.m[0] = t; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int h4(int *x) { A4Mat16 *a = (A4Mat16 *)x; int *q = C->m; G.m[0] = a->m[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int h5(int *x) { __declspec(align(4)) int *q = C->m; G.m[0] = x[0]; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int h6(void) { __declspec(align(4)) int t; int *q = C->m; t = H; G.m[0] = t; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
