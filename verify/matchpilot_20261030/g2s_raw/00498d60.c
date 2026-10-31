// FLAGS: /O2 /GS
// TOOLCHAIN ddk
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef int code();
typedef unsigned char bool; typedef unsigned int WPARAM; typedef long LPARAM; typedef long LRESULT; typedef void *HWND;
typedef void *HINSTANCE; typedef void *HMODULE; typedef void *HANDLE; typedef int BOOL; typedef unsigned int UINT;
typedef unsigned long DWORD; typedef char *LPSTR; typedef const char *LPCSTR; typedef void *LPVOID; typedef unsigned short WORD;
typedef unsigned char BYTE; typedef long LONG; typedef void *HDC; typedef void *HMENU; typedef void *HICON;
typedef unsigned short wchar_t; typedef wchar_t *LPWSTR; typedef const wchar_t *LPCWSTR; typedef int INT_PTR;
typedef unsigned int UINT_PTR; typedef long LONG_PTR; typedef unsigned long ULONG_PTR; typedef long HRESULT;
typedef long double unkbyte10; typedef unsigned int uintptr_t; typedef unsigned int size_t;
#define true 1
#define false 0
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT12(a,b) ((uint)(((uint)(uchar)(a)<<16)|(ushort)(b)))
#define CONCAT13(a,b) ((uint)(((uint)(uchar)(a)<<24)|((uint)(b)&0xffffff)))
#define CONCAT21(a,b) ((uint)(((uint)(ushort)(a)<<8)|(uchar)(b)))
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
#define SUB41(a,n) ((uchar)((uint)(a)>>((n)*8)))
#define SUB42(a,n) ((ushort)((uint)(a)>>((n)*8)))
#define SUB81(a,n) ((uchar)((ulonglong)(a)>>((n)*8)))
#define SUB84(a,n) ((uint)((ulonglong)(a)>>((n)*8)))
#define ZEXT14(a) ((uint)(uchar)(a))
#define ZEXT24(a) ((uint)(ushort)(a))
#define ZEXT48(a) ((ulonglong)(uint)(a))
#define SEXT14(a) ((int)(signed char)(a))
#define SEXT24(a) ((int)(short)(a))
#define SEXT48(a) ((longlong)(int)(a))

extern uint DAT_00616038;
extern int DAT_007731f8;
extern int DAT_0077340c;
extern int DAT_00773408;
extern undefined4 DAT_00773418;
void FUN_00498a00(undefined4 p0);
__declspec(dllimport) LRESULT __stdcall SendMessageA(HWND p0, UINT p1, WPARAM p2, LPARAM p3);
// MATCH: 0x00498d60 FUN_00498d60
/* [C1 2026-05-16] Settings dialog helper. See re/analysis/promote_c2_settings_dialog/00498d60.md
   for Why C2. */

void FUN_00498d60(HWND param_1)

{
  int iVar1;
  WPARAM wParam;
  int *piVar2;
  int lParam;
  uint unaff_retaddr;
  int local_48;
  undefined1 local_44 [64];
  uint local_4;
  
  local_4 = DAT_00616038 ^ unaff_retaddr;
  iVar1 = DAT_0077340c * 4;
  lParam = 0;
  if (0 < *(int *)(iVar1 + DAT_007731f8)) {
    local_48 = 0;
    do {
      piVar2 = (int *)(*(int *)(iVar1 + DAT_00773408) + local_48);
      if (((*(int *)(*(int *)(iVar1 + DAT_00773408) + 0xc + local_48) != 0) && (0x27f < *piVar2)) &&
         (0x1df < piVar2[1])) {
        FUN_00498a00(local_44);
        wParam = SendMessageA(param_1,0x143,0,(LPARAM)local_44);
        SendMessageA(param_1,0x151,wParam,lParam);
        (&DAT_00773418)[lParam] = wParam;
      }
      iVar1 = DAT_0077340c * 4;
      lParam = lParam + 1;
      local_48 = local_48 + 0x18;
    } while (lParam < *(int *)(iVar1 + DAT_007731f8));
  }
  return;
}

