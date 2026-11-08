// FLAGS: /O1
// best 69.4 (regs 69.4). CRT _strtod, hand-written.
// Residual: original pushes eax (=0 from isspace) twice before strlen and returns fldz directly in the 0x240 branch; ours folds zeros to push 0 and keeps local_c. Class (b).
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef unsigned int undefined4;
typedef unsigned char undefined1;
MGLOBAL(double, DAT_00635c68, 0x00635c68);
int FUN_004ac7f0(unsigned int p0);
unsigned int FUN_004a9410(char *p0);
unsigned int *FUN_005c31d0(undefined1 *p0, unsigned char *p1, unsigned int p2, int p3, int p4);
int *FUN_004aa23d(void);
// MATCH: 0x005c1f97 FUN_005c1f97
double FUN_005c1f97(char *_Str, char **_EndPtr)
{
  unsigned int uVar2;
  unsigned int sVar4;
  unsigned int *puVar5;
  undefined4 *puVar6;
  unsigned char *_Str_00;
  undefined1 local_24[24];
  double local_c;

  _Str_00 = (unsigned char *)_Str;
  while (FUN_004ac7f0((unsigned int)*_Str_00) != 0) {
    _Str_00 = _Str_00 + 1;
  }
  sVar4 = FUN_004a9410((char *)_Str_00);
  puVar5 = FUN_005c31d0(local_24, _Str_00, sVar4, 0, 0);
  if (_EndPtr != (char **)0x0) {
    *_EndPtr = (char *)(_Str_00 + puVar5[1]);
  }
  uVar2 = *puVar5;
  if ((uVar2 & 0x240) == 0) {
    if ((uVar2 & 0x81) == 0) {
      if ((uVar2 & 0x100) == 0) {
        return *(double *)(puVar5 + 4);
      }
      local_c = 0.0;
    }
    else {
      local_c = DAT_00635c68;
      if (*_Str_00 == 0x2d) {
        local_c = -DAT_00635c68;
      }
    }
    puVar6 = (undefined4 *)FUN_004aa23d();
    *puVar6 = 0x22;
  }
  else {
    local_c = 0.0;
    if (_EndPtr != (char **)0x0) {
      *_EndPtr = _Str;
    }
  }
  return local_c;
}