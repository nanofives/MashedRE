// FLAGS: /O1 /GS
/* MATCH-S17 0x004aefd0: CRT _setmbcp_lk (VC7 crt/src/mbctype.c), hand-written from the CRT source under /O1 /GS.
   Holds the file-static CPtoLCID 0x004aed77 (codepage arrives in EAX: VC7 static-function register convention).
   Score: 100% (regs 100, ops 100) 135/135 ins, CPtoLCID 100% 18/18. memset intrinsic needs the declaration BEFORE #pragma intrinsic (pragma first gave a call, 61.5%). relocheck OK. C++: warning C4162 only. */
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
typedef struct {
  int code_page;
  unsigned short mbulinfo[6];
  unsigned char rgrange[4][8];
} code_page_info;
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
MGLOBAL(int, DAT_008aa444, 0x008aa444);
MGLOBAL(int, DAT_008aa330, 0x008aa330);
MGLOBAL(int, DAT_008aa338, 0x008aa338);
MGLOBAL(int, DAT_00773d70, 0x00773d70);
MGLOBALA(unsigned char, DAT_008aa340, 257, 0x008aa340);
MGLOBALA(unsigned short, DAT_008aa450, 6, 0x008aa450);
MGLOBALA(unsigned char, DAT_00616d68, 4, 0x00616d68);
MGLOBALA(code_page_info, DAT_00616d70, 5, 0x00616d70);
__declspec(dllimport) int __stdcall GetCPInfo(unsigned int CodePage, MB_CPINFO *lpCPInfo);
void FUN_004aeda6(void);
void FUN_004aedcf(void);

// MATCH: 0x004aed77 FUN_004aed77
static int FUN_004aed77(int codepage)
{
  switch (codepage) {
  case 932: return 0x411;
  case 936: return 0x804;
  case 949: return 0x412;
  case 950: return 0x404;
  }
  return 0;
}

// MATCH: 0x004aefd0 FUN_004aefd0
int FUN_004aefd0(int codepage)
{
  unsigned int icp;
  unsigned int irg;
  unsigned int ich;
  unsigned char *rgptr;
  MB_CPINFO cpinfo;

  if (codepage == 0)
  {
    FUN_004aeda6();
    goto setmbcp_exit;
  }

  for (icp = 0; icp < 5; icp++)
  {
    if (DAT_00616d70[icp].code_page == codepage)
    {
      memset(DAT_008aa340, 0, 257);

      for (irg = 0; irg < 4; irg++)
      {
        for (rgptr = DAT_00616d70[icp].rgrange[irg]; rgptr[0] && rgptr[1]; rgptr += 2)
        {
          for (ich = rgptr[0]; ich <= rgptr[1]; ich++)
            DAT_008aa340[ich + 1] |= DAT_00616d68[irg];
        }
      }

      DAT_008aa444 = codepage;
      DAT_008aa338 = 1;
      DAT_008aa330 = FUN_004aed77(DAT_008aa444);

      for (irg = 0; irg < 6; irg++)
        DAT_008aa450[irg] = DAT_00616d70[icp].mbulinfo[irg];

      goto setmbcp_exit;
    }
  }

  if (GetCPInfo(codepage, &cpinfo) == 1)
  {
    memset(DAT_008aa340, 0, 257);
    DAT_008aa444 = codepage;
    DAT_008aa330 = 0;

    if (cpinfo.MaxCharSize > 1)
    {
      for (rgptr = cpinfo.LeadByte; *rgptr && *(rgptr + 1); rgptr += 2)
        for (ich = *rgptr; ich <= *(rgptr + 1); ich++)
          DAT_008aa340[ich + 1] |= 4;

      for (ich = 1; ich < 0xff; ich++)
        DAT_008aa340[ich + 1] |= 8;

      DAT_008aa330 = FUN_004aed77(DAT_008aa444);
      DAT_008aa338 = 1;
    }
    else
      DAT_008aa338 = 0;

    for (irg = 0; irg < 6; irg++)
      DAT_008aa450[irg] = 0;

    goto setmbcp_exit;
  }

  if (DAT_00773d70)
  {
    FUN_004aeda6();
    goto setmbcp_exit;
  }

  return -1;

setmbcp_exit:
  FUN_004aedcf();
  return 0;
}
