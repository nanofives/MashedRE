// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00499890 (h6): logs the OS version through FUN_00496400 and sets DAT_00773920 (1..7).
   Body has the shape of the Platform SDK GetVersionEx sample (OSVERSIONINFOEX, ProductOptions registry fallback).
   By hand: 100% exact (regs 100%), relocheck OK. Residual: none. */
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
typedef void *HKEY;
typedef int BOOL;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef long LONG;
typedef struct _OSVERSIONINFOEXA {
  DWORD dwOSVersionInfoSize; DWORD dwMajorVersion; DWORD dwMinorVersion; DWORD dwBuildNumber; DWORD dwPlatformId;
  char szCSDVersion[128]; WORD wServicePackMajor; WORD wServicePackMinor; WORD wSuiteMask; BYTE wProductType;
  BYTE wReserved;
} OSVERSIONINFOEXA;
MGLOBAL(int, DAT_00773920, 0x00773920);
void FUN_00496400(const char *fmt, ...);
__declspec(dllimport) BOOL __stdcall GetVersionExA(OSVERSIONINFOEXA *lpVersionInformation);
__declspec(dllimport) LONG __stdcall RegOpenKeyExA(HKEY hKey, const char *lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY *phkResult);
__declspec(dllimport) LONG __stdcall RegQueryValueExA(HKEY hKey, const char *lpValueName, DWORD *lpReserved, DWORD *lpType, BYTE *lpData, DWORD *lpcbData);
__declspec(dllimport) LONG __stdcall RegCloseKey(HKEY hKey);
__declspec(dllimport) int __stdcall lstrcmpiA(const char *lpString1, const char *lpString2);
void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x00499890 FUN_00499890
void FUN_00499890(void)
{
  OSVERSIONINFOEXA osvi;
  BOOL bOsVersionInfoEx;
  HKEY hKey;
  char szProductType[80];
  DWORD dwBufLen;

  FUN_00496400("Operating System:\n");
  memset(&osvi, 0, sizeof(OSVERSIONINFOEXA));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
  if (!(bOsVersionInfoEx = GetVersionExA(&osvi))) {
    osvi.dwOSVersionInfoSize = 0x94;
    if (!GetVersionExA(&osvi)) {
      FUN_00496400("Unable to get OS version\n");
      return;
    }
  }
  switch (osvi.dwPlatformId) {
  case 2:
    if (osvi.dwMajorVersion <= 4) {
      DAT_00773920 = 1;
      FUN_00496400("Microsoft Windows NT ");
    }
    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
      DAT_00773920 = 5;
      FUN_00496400("Microsoft Windows 2000 ");
    }
    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
      DAT_00773920 = 6;
      FUN_00496400("Whistler ");
    }
    if (bOsVersionInfoEx) {
      if (osvi.wProductType == 1) {
        if (osvi.wSuiteMask & 0x200)
          FUN_00496400("Personal ");
        else
          FUN_00496400("Professional ");
      } else if (osvi.wProductType == 3) {
        if (osvi.wSuiteMask & 0x80)
          FUN_00496400("DataCenter Server ");
        else if (osvi.wSuiteMask & 0x2)
          FUN_00496400("Advanced Server ");
        else
          FUN_00496400("Server ");
      }
    } else {
      RegOpenKeyExA((HKEY)0x80000002, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 0, 1, &hKey);
      RegQueryValueExA(hKey, "ProductType", 0, 0, (BYTE *)szProductType, &dwBufLen);
      RegCloseKey(hKey);
      if (lstrcmpiA("WINNT", szProductType) == 0)
        FUN_00496400("Workstation ");
      if (lstrcmpiA("SERVERNT", szProductType) == 0)
        FUN_00496400("Server ");
    }
    if (osvi.dwMajorVersion <= 4) {
      FUN_00496400("version %d.%d %s (Build %d)\n", osvi.dwMajorVersion, osvi.dwMinorVersion,
                   osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
    } else {
      FUN_00496400("%s (Build %d)\n", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
    }
    break;
  case 1:
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
      DAT_00773920 = 2;
      FUN_00496400("Microsoft Windows 95 ");
      if (osvi.szCSDVersion[1] == 'C')
        FUN_00496400("OSR2 ");
    }
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
      DAT_00773920 = 3;
      FUN_00496400("Microsoft Windows 98 ");
      if (osvi.szCSDVersion[1] == 'A')
        FUN_00496400("SE ");
    }
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) {
      DAT_00773920 = 4;
      FUN_00496400("Microsoft Windows Me ");
    }
    break;
  case 0:
    DAT_00773920 = 7;
    FUN_00496400("Microsoft Win32s ");
    break;
  default:
    FUN_00496400("Unknown OS");
    break;
  }
  FUN_00496400("\n");
}
