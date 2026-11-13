/* S1B attempt 2: is an aligned extern enough for the aligned-frame path (frame, ebx reserved)?
   compile: asm.py t27.c rw9044 /O2 /Ob1 /G5 */
typedef struct { int m[16]; } Mat16;
extern Mat16 *C;
extern __declspec(align(8)) Mat16 GF8;
extern Mat16 GN;

#define LOOP(G) \
  { const int *p = G.m; const int *qq = q; int n = 16; \
    do { if (*qq != *p) goto set; p++; qq++; } while (--n); }

int m_blk(int *x) { extern __declspec(align(8)) Mat16 GB8; int *q = C->m; GB8.m[0] = x[0]; LOOP(GB8) return 1; set: *(Mat16 *)q = GB8; return 0; }
int m_file(int *x) { int *q = C->m; GF8.m[0] = x[0]; LOOP(GF8) return 1; set: *(Mat16 *)q = GF8; return 0; }
int m_none(int *x) { int *q = C->m; GN.m[0] = x[0]; LOOP(GN) return 1; set: *(Mat16 *)q = GN; return 0; }
int m_stat(int *x) { static __declspec(align(8)) Mat16 GS8; int *q = C->m; GS8.m[0] = x[0]; LOOP(GS8) return 1; set: *(Mat16 *)q = GS8; return 0; }
