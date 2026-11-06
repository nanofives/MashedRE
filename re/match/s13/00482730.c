// FLAGS: /O2 /GS /Ob1
// h11: 100% by whole-symbol bytes (match.py --bytes: 110 B BYTE-IDENTICAL). Instruction score 94.9% only because Ghidra's size (107) stops before the final add esp,0xc / ret. Hand: RW material list structs, local 3-float surface-props struct copied by struct assignment.
/* MATCH-S13 0x00482730: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct { float ambient; float specular; float diffuse; } SurfProps;
typedef struct { int texture; unsigned int color; int pipeline; SurfProps surfaceProps; } Mat;
typedef struct { char pad[0x20]; Mat **materials; int numMaterials; } MatList;
// MATCH: 0x00482730 FUN_00482730
int FUN_00482730(int param_1, unsigned int *param_2)
{
  SurfProps sp;
  MatList *list;
  int i;

  list = *(MatList **)(param_1 + 0x18);
  sp.ambient = 1.0f;
  sp.specular = 1.0f;
  sp.diffuse = 0.5f;
  for (i = 0; i < list->numMaterials; i++) {
    Mat *m = list->materials[i];
    m->color = *param_2;
    m->surfaceProps = sp;
  }
  return param_1;
}
