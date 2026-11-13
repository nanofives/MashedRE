/* S1B attempt 2: aligned locals under the Processor Pack c2 (dynamic stack alignment). Does a frame or an ebx
   reservation survive when the aligned local is optimized away?
   compile: asm.py t23.c rw9044 /O2 /Ob1 /G5   (no /Oy-) */
typedef struct { int m[16]; } Mat16;
typedef __declspec(align(16)) struct { int m[16]; } AMat16;
typedef __declspec(align(16)) struct { float f[4]; } A4;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;
extern AMat16 AG;

#define LOOP \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int f0(void) { int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int f1(void) { __declspec(align(16)) int t[4]; int *q = C->m; t[0] = H; LOOP F = t[0]; return 1; set: *(Mat16 *)q = G; return 0; }
int f2(void) { __declspec(align(16)) int t[4]; int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int f3(void) { A4 a; int *q = C->m; a.f[0] = (float)H; LOOP F = (int)a.f[0]; return 1; set: *(Mat16 *)q = G; return 0; }
int f4(void) { AMat16 t; int *q = C->m; LOOP return 1; set: t = AG; *(AMat16 *)q = t; return 0; }
int f5(void) { int *q = C->m; LOOP return 1; set: *(AMat16 *)q = AG; return 0; }
int f6(int *x) { A4 a; int *q = C->m; a.f[0] = (float)x[0]; a.f[1] = (float)x[1]; LOOP return 1; set: *(Mat16 *)q = G; F = (int)(a.f[0] + a.f[1]); return 0; }
