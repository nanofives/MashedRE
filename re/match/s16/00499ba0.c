// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00499ba0 (h6): copy command line to DAT_00773818, CoInitialize, RegisterClassA("MASHED"),
   AdjustWindowRect + CreateWindowExA, store hwnd/hinstance, FUN_00499890. DAT_00773818 length 0x100 [UNCERTAIN: size not read].
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
typedef void *HWND;
typedef void *HINSTANCE;
typedef void *HICON;
typedef void *HCURSOR;
typedef void *HBRUSH;
typedef void *HMENU;
typedef void *HGDIOBJ;
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned int WPARAM;
typedef long LPARAM;
typedef long LRESULT;
typedef long HRESULT;
typedef unsigned long DWORD;
typedef unsigned short ATOM;
typedef LRESULT (__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef struct tagWNDCLASSA {
  UINT style; WNDPROC lpfnWndProc; int cbClsExtra; int cbWndExtra; HINSTANCE hInstance;
  HICON hIcon; HCURSOR hCursor; HBRUSH hbrBackground; const char *lpszMenuName; const char *lpszClassName;
} WNDCLASSA;
typedef struct tagRECT { long left; long top; long right; long bottom; } RECT;
MGLOBAL(HWND, DAT_007e9584, 0x007e9584);
MGLOBAL(HINSTANCE, DAT_007e9580, 0x007e9580);
MGLOBALA(char, DAT_00773818, 0x100, 0x00773818);
MGLOBALA(char, s_MASHED_005cfabc, 7, 0x005cfabc);
MGLOBALA(char, s_MASHED__D3D9___Release__005d041c, 24, 0x005d041c);
LRESULT __stdcall FUN_00499820(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int FUN_00498bc0(void);
int FUN_00498bd0(void);
void FUN_00499890(void);
__declspec(dllimport) HRESULT __stdcall CoInitialize(void *pvReserved);
__declspec(dllimport) HCURSOR __stdcall LoadCursorA(HINSTANCE hInstance, const char *lpCursorName);
__declspec(dllimport) HGDIOBJ __stdcall GetStockObject(int i);
__declspec(dllimport) ATOM __stdcall RegisterClassA(const WNDCLASSA *lpWndClass);
__declspec(dllimport) BOOL __stdcall AdjustWindowRect(RECT *lpRect, DWORD dwStyle, BOOL bMenu);
__declspec(dllimport) HWND __stdcall CreateWindowExA(DWORD dwExStyle, const char *lpClassName, const char *lpWindowName,
  DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, void *lpParam);
// MATCH: 0x00499ba0 FUN_00499ba0
void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
char *__cdecl strcpy(char *, const char *);
#pragma intrinsic(strcpy)
int FUN_00499ba0(HINSTANCE param_1, int param_2, char *param_3)
{
  WNDCLASSA wc;
  RECT rc;

  strcpy(DAT_00773818, param_3);
  CoInitialize(0);
  memset(&wc, 0, sizeof(wc));
  wc.style = 0x2000;
  wc.hInstance = param_1;
  wc.lpszClassName = s_MASHED_005cfabc;
  wc.lpfnWndProc = FUN_00499820;
  wc.hIcon = 0;
  wc.hCursor = LoadCursorA(0, (const char *)0x7f00);
  wc.lpszMenuName = 0;
  wc.hbrBackground = (HBRUSH)GetStockObject(0);
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  if (RegisterClassA(&wc) == 0) {
    return 0;
  }
  rc.left = 0;
  rc.top = 0;
  rc.right = FUN_00498bc0();
  rc.bottom = FUN_00498bd0();
  AdjustWindowRect(&rc, 0xcf0000, 0);
  DAT_007e9584 = CreateWindowExA(0, s_MASHED_005cfabc, s_MASHED__D3D9___Release__005d041c, 0xcf0000,
                                 (int)0x80000000, (int)0x80000000, rc.right - rc.left, rc.bottom - rc.top,
                                 0, 0, param_1, 0);
  DAT_007e9580 = param_1;
  FUN_00499890();
  return 1;
}
