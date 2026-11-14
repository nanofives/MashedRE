/* S1B attempt 2: __asm in a branch that only c2 can prove dead (c1 at /O2 keeps the test on a local constant).
   compile: asm.py t22.c rw9044 /O2 /Ob1 /G5   (no /Oy-) */
typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

#define LOOP \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int e0(void) { int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int e1(void) { int k = 0; int *q = C->m; if (k) { __asm mov ebx, 1 } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int e2(void) { int k = 0; int *q = C->m; if (k) { __asm nop } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int e3(void) { int k = 0; int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; if (k) { __asm mov ebx, 1 } return 0; }
int e4(void) { int k = 0; int *q = C->m; if (k) { __asm { mov esi, 1 } } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int e5(void) { int *q = C->m; goto skip; { __asm mov ebx, 1 } skip: LOOP return 1; set: *(Mat16 *)q = G; return 0; }
static int s_zero = 0;
int e6(void) { int *q = C->m; if (s_zero) { __asm mov ebx, 1 } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
