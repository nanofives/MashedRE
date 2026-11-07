// FLAGS: /O1 /GS
/* MATCH-S17 0x004a43d2: CRT _ftime64 (VC7 crt/src/ftime64.c), hand-written from the CRT source under /O1 /GS
   (the DDK libcmt build lacks the cookie: its frame is 0xbc, the exe's 0xc0). Score: see header line below. */
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
typedef struct { unsigned long dwLowDateTime; unsigned long dwHighDateTime; } FT_FILETIME;
typedef union { unsigned __int64 ft_scalar; FT_FILETIME ft_struct; } FT;
typedef struct { unsigned short wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds; } FT_SYSTEMTIME;
typedef struct {
  long Bias; unsigned short StandardName[32]; FT_SYSTEMTIME StandardDate; long StandardBias;
  unsigned short DaylightName[32]; FT_SYSTEMTIME DaylightDate; long DaylightBias;
} FT_TZINFO;
struct __timeb64 { __int64 time; unsigned short millitm; short timezone; short dstflag; };
MGLOBAL(long, DAT_00616670, 0x00616670);
MGLOBAL(__int64, DAT_007739e0, 0x007739e0);
MGLOBAL(int, DAT_006160c0, 0x006160c0);
void FUN_004aa00c(void);
__declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(FT_FILETIME *lpSystemTimeAsFileTime);
__declspec(dllimport) unsigned long __stdcall GetTimeZoneInformation(FT_TZINFO *lpTimeZoneInformation);
// MATCH: 0x004a43d2 FUN_004a43d2
void FUN_004a43d2(struct __timeb64 *tp)
{
  FT nt_time;
  __int64 t;
  FT_TZINFO tzinfo;
  unsigned long tzstate;

  FUN_004aa00c();

  tp->timezone = (short)(DAT_00616670 / 60);

  GetSystemTimeAsFileTime(&(nt_time.ft_struct));

  if ((t = (__int64)(nt_time.ft_scalar / 600000000i64)) != DAT_007739e0) {
    if ((tzstate = GetTimeZoneInformation(&tzinfo)) != 0xFFFFFFFF) {
      if ((tzstate == 2) && (tzinfo.DaylightDate.wMonth != 0) && (tzinfo.DaylightBias != 0))
        DAT_006160c0 = 1;
      else
        DAT_006160c0 = 0;
    }
    else
      DAT_006160c0 = -1;
    DAT_007739e0 = t;
  }

  tp->dstflag = (short)DAT_006160c0;
  tp->millitm = (unsigned short)((nt_time.ft_scalar / 10000i64) % 1000i64);
  tp->time = (__int64)((nt_time.ft_scalar - 116444736000000000i64) / 10000000i64);
}
