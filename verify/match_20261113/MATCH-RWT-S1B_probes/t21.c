/* S1B attempt 2: __asm in dead or unreachable code. Does the "function has asm" state survive the removal of the
   asm (frame kept, asm registers reserved) while no asm instruction is emitted?
   compile: asm.py t21.c rw9044 /O2 /Ob1 /G5   (no /Oy-) */
typedef struct { int m[16]; } Mat16;
extern Mat16 G;
extern Mat16 *C;
extern int F, H;

#define LOOP \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int d0(void) { int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int d1(void) { int *q = C->m; if (0) { __asm nop } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int d2(void) { int *q = C->m; if (0) { __asm mov ebx, 1 } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int d3(void) { int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; return 0; __asm mov ebx, 1 }
int d4(void) { int *q = C->m; LOOP return 1; set: *(Mat16 *)q = G; return 0; __asm nop }
int d5(void) { int *q = C->m; if (0) { __asm { push ebx } __asm { pop ebx } } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int d6(void) { int *q = C->m; if (0) { __asm cpuid } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int d7(void) { int *q = C->m; __asm { } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
int d8(void) { int *q = C->m; if (F == 12345 && 0) { __asm mov ebx, 1 } LOOP return 1; set: *(Mat16 *)q = G; return 0; }
