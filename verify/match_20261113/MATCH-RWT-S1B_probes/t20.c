/* S1B attempt 2: constructs that give an ebp frame without /Oy-; does any of them also keep ebx out?
   compile: asm.py t20.c rw9044 /O2 /Ob1 /G5   (no /Oy-) */
typedef struct { int m[16]; } Mat16;
typedef struct { double d; int k; } WithD;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;
extern double D;
extern __int64 I64;
extern WithD WD;

#define LOOP \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int a0(void) { int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int a1(void) { int *q = C->m; double d = D; LOOP F = (int)d; return 1; set: *(Mat16 *)q = G; return 0; }
int a2(void) { int *q = C->m; double d = D; LOOP return 1; set: *(Mat16 *)q = G; D = d * 2.0; return 0; }
int a3(void) { int *q = C->m; __int64 v = I64; LOOP return 1; set: *(Mat16 *)q = G; I64 = v + 1; return 0; }
int a4(void) { int *q = C->m; WithD w = WD; LOOP return 1; set: *(Mat16 *)q = G; F = w.k; return 0; }
int a5(void) { int *q = C->m; double d; LOOP return 1; set: *(Mat16 *)q = G; d = D; D = d; return 0; }
int a6(void) { int *q = C->m; int arr[4]; arr[0] = H; LOOP return 1; set: *(Mat16 *)q = G; F = arr[0]; return 0; }
int a7(void) { int *q = C->m; Mat16 t = G; LOOP return 1; set: *(Mat16 *)q = t; return 0; }
int a8(void) { int *q = C->m; float f = (float)H; LOOP return 1; set: *(Mat16 *)q = G; F = (int)f; return 0; }
