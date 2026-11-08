// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054c870 h1: best 56.6% (regs 58.3%, ops 71.2%); structure only (1590 B, 4-channel byte-quantised forward-difference evaluator). Ghidra's float locals folded into float L[0x50] + q[64] arrays, ROUND replaced by (unsigned char)(int)x (orig uses fnstcw/fldcw truncation). Frame 0x244 vs orig 0x264 and orig pushes ebx. Class (b). */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_0054dbd0(float *p0, int p1);
void FUN_0054eb90(float *p0, float *p1, float p2, float p3);
// MATCH: 0x0054c870 FUN_0054c870


unsigned char * FUN_0054c870(unsigned char *param_1,int param_2,int param_3,int param_4)

{
  float fVar1;
  unsigned char *puVar2;
  int iVar3;
  int iVar4;
  float L[0x50];
  unsigned char c0, c1, c2, c3;
  float q[64];
  
  fVar1 = DAT_005cc320 / (float)param_4;
  FUN_0054dbd0(q,param_3);
  FUN_0054eb90(&L[16],q,fVar1,fVar1);
  if (-1 < param_4) {
    puVar2 = param_1;
    iVar4 = param_4 + 1;
    do {
      L[0] = L[16];
      L[1] = L[17];
      L[2] = L[18];
      L[3] = L[19];
      L[4] = L[20];
      L[5] = L[21];
      L[6] = L[22];
      L[7] = L[23];
      L[8] = L[24];
      L[9] = L[25];
      L[10] = L[26];
      L[11] = L[27];
      iVar3 = param_4 + 1;
      do {
        c0 = (unsigned char)(int)(L[0]);
        *puVar2 = c0;
        c1 = (unsigned char)(int)(L[1]);
        puVar2[1] = c1;
        c2 = (unsigned char)(int)(L[2]);
        puVar2[2] = c2;
        L[0] = L[4] + L[0];
        c3 = (unsigned char)(int)(L[3]);
        puVar2[3] = c3;
        puVar2 = puVar2 + param_2;
        iVar3 = iVar3 + -1;
        L[1] = L[5] + L[1];
        L[2] = L[6] + L[2];
        L[3] = L[7] + L[3];
        L[4] = L[8] + L[4];
        L[5] = L[9] + L[5];
        L[6] = L[10] + L[6];
        L[7] = L[11] + L[7];
        L[8] = L[28] + L[8];
        L[9] = L[29] + L[9];
        L[10] = L[30] + L[10];
        L[11] = L[31] + L[11];
      } while (iVar3 != 0);
      L[16] = L[32] + L[16];
      L[17] = L[33] + L[17];
      L[18] = L[34] + L[18];
      L[19] = L[35] + L[19];
      L[20] = L[36] + L[20];
      L[21] = L[37] + L[21];
      L[22] = L[38] + L[22];
      L[23] = L[39] + L[23];
      L[24] = L[40] + L[24];
      L[25] = L[41] + L[25];
      L[26] = L[42] + L[26];
      L[27] = L[43] + L[27];
      L[28] = L[44] + L[28];
      L[29] = L[45] + L[29];
      L[30] = L[46] + L[30];
      L[31] = L[47] + L[31];
      L[32] = L[48] + L[32];
      L[33] = L[49] + L[33];
      L[34] = L[50] + L[34];
      L[35] = L[51] + L[35];
      L[36] = L[52] + L[36];
      L[37] = L[53] + L[37];
      L[38] = L[54] + L[38];
      L[39] = L[55] + L[39];
      L[40] = L[56] + L[40];
      L[41] = L[57] + L[41];
      L[42] = L[58] + L[42];
      L[43] = L[59] + L[43];
      iVar4 = iVar4 + -1;
      L[44] = L[60] + L[44];
      L[45] = L[61] + L[45];
      L[46] = L[62] + L[46];
      L[47] = L[63] + L[47];
      L[48] = L[64] + L[48];
      L[49] = L[65] + L[49];
      L[50] = L[66] + L[50];
      L[51] = L[67] + L[51];
      L[52] = L[68] + L[52];
      L[53] = L[69] + L[53];
      L[54] = L[70] + L[54];
      L[55] = L[71] + L[55];
      L[56] = L[72] + L[56];
      L[57] = L[73] + L[57];
      L[58] = L[74] + L[58];
      L[59] = L[75] + L[59];
      L[60] = L[76] + L[60];
      L[61] = L[77] + L[61];
      L[62] = L[78] + L[62];
      L[63] = L[79] + L[63];
    } while (iVar4 != 0);
  }
  return param_1;
}
