// FLAGS: /O1 /GS
/* MATCH-S17 0x004aedcf: CRT setSBUpLow (VC7 crt/src/mbctype.c, file-static there), hand-written from the CRT
   source under /O1 /GS (DDK libcmt frame 0x514 has no cookie, the exe's 0x518 does). Score: 100% (regs 100, ops 100), 126/126 ins; wVector is 256 USHORTs (frame 0x518), not 512. relocheck OK. */
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
typedef struct { unsigned int MaxCharSize; unsigned char DefaultChar[2]; unsigned char LeadByte[12]; } MB_CPINFO;
MGLOBAL(unsigned int, DAT_008aa444, 0x008aa444);
MGLOBAL(int, DAT_008aa330, 0x008aa330);
MGLOBALA(unsigned char, DAT_008aa340, 257, 0x008aa340);
MGLOBALA(unsigned char, DAT_008aa460, 256, 0x008aa460);
__declspec(dllimport) int __stdcall GetCPInfo(unsigned int CodePage, MB_CPINFO *lpCPInfo);
int FUN_004b24f8(unsigned long dwInfoType, const char *lpSrcStr, int cchSrc, unsigned short *lpCharType, int code_page, int lcid, int bError);
int FUN_004b008f(int Locale, unsigned long dwMapFlags, const char *lpSrcStr, int cchSrc, char *lpDestStr, int cchDest, int code_page, int bError);
// MATCH: 0x004aedcf FUN_004aedcf
void FUN_004aedcf(void)
{
  unsigned char *pbPair;
  unsigned int ich;
  MB_CPINFO cpinfo;
  unsigned char sbVector[256];
  unsigned char upVector[256];
  unsigned char lowVector[256];
  unsigned short wVector[256];

  if (GetCPInfo(DAT_008aa444, &cpinfo) == 1)
  {
    for (ich = 0; ich < 256; ich++)
      sbVector[ich] = (unsigned char)ich;

    sbVector[0] = (unsigned char)' ';
    for (pbPair = &cpinfo.LeadByte[0]; *pbPair; pbPair += 2)
      for (ich = *pbPair; ich <= *(pbPair + 1); ich++)
        sbVector[ich] = (unsigned char)' ';

    FUN_004b24f8(1, (const char *)sbVector, 256, wVector, DAT_008aa444, DAT_008aa330, 0);
    FUN_004b008f(DAT_008aa330, 0x100, (const char *)sbVector, 256, (char *)lowVector, 256, DAT_008aa444, 0);
    FUN_004b008f(DAT_008aa330, 0x200, (const char *)sbVector, 256, (char *)upVector, 256, DAT_008aa444, 0);

    for (ich = 0; ich < 256; ich++)
    {
      if (wVector[ich] & 1)
      {
        DAT_008aa340[ich + 1] |= 0x10;
        DAT_008aa460[ich] = lowVector[ich];
      }
      else if (wVector[ich] & 2)
      {
        DAT_008aa340[ich + 1] |= 0x20;
        DAT_008aa460[ich] = upVector[ich];
      }
      else
        DAT_008aa460[ich] = 0;
    }
  }
  else
  {
    for (ich = 0; ich < 256; ich++)
    {
      if (ich >= (unsigned int)'A' && ich <= (unsigned int)'Z')
      {
        DAT_008aa340[ich + 1] |= 0x10;
        DAT_008aa460[ich] = (unsigned char)(ich + ('a' - 'A'));
      }
      else if (ich >= (unsigned int)'a' && ich <= (unsigned int)'z')
      {
        DAT_008aa340[ich + 1] |= 0x20;
        DAT_008aa460[ich] = (unsigned char)(ich - ('a' - 'A'));
      }
      else
        DAT_008aa460[ich] = 0;
    }
  }
}
