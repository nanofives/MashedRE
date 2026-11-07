// FLAGS: /O1
/* MATCH-S17 0x004a4bb7: CRT WinMainCRTStartup (VC7 crt/src/crt0.c, _WINMAIN_ _MT _RTC), hand-written from the CRT
   source under /O1 with a real __try/__except. The DDK libcmt build opens the __try before _RTC_Initialize, the exe's
   after it (first diff +0xec). check_managed_app is inlined (static __inline), exit/_exit noreturn, filter and
   handler use _exception_code()/_exception_info(). Matched: whole obj symbol BYTE-IDENTICAL over 466 B (relocations
   masked); match.py prints 89.3% only because the Ghidra size (423 B) stops inside the __except filter (0x004a4d4f).
   relocheck reports BAD on the scope table $T (label relocations unresolved: tool limit, same as 004a2bb8.c); the
   filter/handler offsets agree because the code is byte-identical. */
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
typedef struct {
  unsigned long dwOSVersionInfoSize, dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformId;
  char szCSDVersion[128];
} CRT_OSVERSIONINFOA;
typedef struct {
  unsigned long cb; char *lpReserved, *lpDesktop, *lpTitle;
  unsigned long dwX, dwY, dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute, dwFlags;
  unsigned short wShowWindow, cbReserved2; unsigned char *lpReserved2; void *hStdInput, *hStdOutput, *hStdError;
} CRT_STARTUPINFOA;
typedef struct { unsigned short e_magic; unsigned short e_pad[29]; long e_lfanew; } CRT_DOS_HEADER;
typedef struct { unsigned long VirtualAddress; unsigned long Size; } CRT_DATA_DIRECTORY;
typedef struct { unsigned short Magic; unsigned char pad[0x5a]; unsigned long NumberOfRvaAndSizes; CRT_DATA_DIRECTORY DataDirectory[16]; } CRT_OPT32;
typedef struct { unsigned short Magic; unsigned char pad[0x6a]; unsigned long NumberOfRvaAndSizes; CRT_DATA_DIRECTORY DataDirectory[16]; } CRT_OPT64;
typedef struct { unsigned long Signature; unsigned char FileHeader[20]; CRT_OPT32 OptionalHeader; } CRT_NT_HEADERS;
typedef struct { unsigned long ExceptionCode; } CRT_EXCEPTION_RECORD;
typedef struct { CRT_EXCEPTION_RECORD *ExceptionRecord; void *ContextRecord; } CRT_EXCEPTION_POINTERS;

void *_alloca(unsigned int);
unsigned long _exception_code(void);
void *_exception_info(void);
MGLOBAL(unsigned int, DAT_0077399c, 0x0077399c);
MGLOBAL(unsigned int, DAT_007739a8, 0x007739a8);
MGLOBAL(unsigned int, DAT_007739ac, 0x007739ac);
MGLOBAL(unsigned int, DAT_007739a0, 0x007739a0);
MGLOBAL(unsigned int, DAT_007739a4, 0x007739a4);
MGLOBAL(char *, DAT_008ab6c4, 0x008ab6c4);
MGLOBAL(char *, DAT_007739e8, 0x007739e8);
__declspec(dllimport) int __stdcall GetVersionExA(CRT_OSVERSIONINFOA *lpVersionInformation);
__declspec(dllimport) void *__stdcall GetModuleHandleA(const char *lpModuleName);
__declspec(dllimport) char *__stdcall GetCommandLineA(void);
__declspec(dllimport) void __stdcall GetStartupInfoA(CRT_STARTUPINFOA *lpStartupInfo);
int FUN_004aa3fe(int);
void FUN_004a4b93(int);
int FUN_004a8a04(void);
void FUN_004a78b0(void);
int FUN_004ac04a(void);
void FUN_004a4b6e(int);
char *FUN_004abf28(void);
int FUN_004abe86(void);
int FUN_004abc53(void);
int FUN_004a31f3(void);
char *FUN_004abbea(void);
int __stdcall FUN_00492370(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nShowCmd);
__declspec(noreturn) void FUN_004a332b(int);
void FUN_004a334d(void);
int FUN_004aba86(unsigned long, CRT_EXCEPTION_POINTERS *);
__declspec(noreturn) void FUN_004a333c(int);
void FUN_004a335c(void);

static __inline int check_managed_app(void)
{
  CRT_DOS_HEADER *pDOSHeader;
  CRT_NT_HEADERS *pPEHeader;
  CRT_OPT32 *pNTHeader32;
  CRT_OPT64 *pNTHeader64;

  pDOSHeader = (CRT_DOS_HEADER *)GetModuleHandleA(0);
  if (pDOSHeader->e_magic != 0x5a4d)
    return 0;

  pPEHeader = (CRT_NT_HEADERS *)((char *)pDOSHeader + pDOSHeader->e_lfanew);
  if (pPEHeader->Signature != 0x4550)
    return 0;

  pNTHeader32 = &pPEHeader->OptionalHeader;
  switch (pNTHeader32->Magic) {
  case 0x10b:
    if (pNTHeader32->NumberOfRvaAndSizes <= 14)
      return 0;
    return !!pNTHeader32->DataDirectory[14].VirtualAddress;
  case 0x20b:
    pNTHeader64 = (CRT_OPT64 *)pNTHeader32;
    if (pNTHeader64->NumberOfRvaAndSizes <= 14)
      return 0;
    return !!pNTHeader64->DataDirectory[14].VirtualAddress;
  }
  return 0;
}

// MATCH: 0x004a4bb7 FUN_004a4bb7
int FUN_004a4bb7(void)
{
  int initret;
  int mainret;
  CRT_OSVERSIONINFOA *posvi;
  int managedapp;
  char *lpszCommandLine;
  CRT_STARTUPINFOA StartupInfo;

  posvi = (CRT_OSVERSIONINFOA *)_alloca(sizeof(CRT_OSVERSIONINFOA));
  posvi->dwOSVersionInfoSize = sizeof(CRT_OSVERSIONINFOA);
  (void)GetVersionExA(posvi);

  DAT_0077399c = posvi->dwPlatformId;
  DAT_007739a8 = posvi->dwMajorVersion;
  DAT_007739ac = posvi->dwMinorVersion;

  DAT_007739a0 = (posvi->dwBuildNumber) & 0x07fff;
  if (DAT_0077399c != 2)
    DAT_007739a0 |= 0x08000;
  DAT_007739a4 = (DAT_007739a8 << 8) + DAT_007739ac;

  managedapp = check_managed_app();

  if (!FUN_004aa3fe(1))
    FUN_004a4b93(0x1c);

  if (!FUN_004a8a04())
    FUN_004a4b93(0x10);

  FUN_004a78b0();

  __try {
    if (FUN_004ac04a() < 0)
      FUN_004a4b6e(0x1b);

    DAT_008ab6c4 = GetCommandLineA();
    DAT_007739e8 = FUN_004abf28();

    if (FUN_004abe86() < 0)
      FUN_004a4b6e(8);
    if (FUN_004abc53() < 0)
      FUN_004a4b6e(9);

    initret = FUN_004a31f3();
    if (initret != 0)
      FUN_004a4b6e(initret);

    StartupInfo.dwFlags = 0;
    GetStartupInfoA(&StartupInfo);

    lpszCommandLine = FUN_004abbea();
    mainret = FUN_00492370(GetModuleHandleA(0), 0, lpszCommandLine,
                           StartupInfo.dwFlags & 1 ? StartupInfo.wShowWindow : 10);

    if (!managedapp)
      FUN_004a332b(mainret);

    FUN_004a334d();
  }
  __except (FUN_004aba86(_exception_code(), (CRT_EXCEPTION_POINTERS *)_exception_info()))
  {
    mainret = _exception_code();
    if (!managedapp)
      FUN_004a333c(mainret);
    FUN_004a335c();
  }
  return mainret;
}
