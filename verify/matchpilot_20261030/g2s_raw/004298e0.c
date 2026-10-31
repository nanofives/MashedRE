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

extern undefined1 DAT_0067d99c;
extern undefined4 DAT_0067d994;
extern undefined4 DAT_0067d98c;
extern undefined4 DAT_008991bc;
undefined4 FUN_00411580(int p0, undefined4 p1, undefined4 p2, undefined4 p3);
void FUN_00430b00(int p0, int p1, undefined4 p2, undefined4 p3, undefined4 p4);
extern int _DAT_0067d99c;
// MATCH: 0x004298e0 FUN_004298e0
/* [C1 2026-06-01] Resets the HUD lap-time display to a max/sentinel split for two display slots (0
   and 1). */

void FUN_004298e0(void)

{
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  _DAT_0067d99c = 0x3f7d70a4;
  DAT_0067d994 = 0x3b;
  DAT_0067d98c = 9;
  DAT_008991bc = 0;
  local_4 = 0x3f7d70a4;
  local_8 = 0x3b;
  local_c = 9;
  FUN_00430b00(0,0,0x3f7d70a4,0x3b,9);
  FUN_00411580(0,&local_4,&local_8,&local_c);
  FUN_00430b00(0,1,local_4,local_8,local_c);
  local_4 = 0x3f7d70a4;
  local_8 = 0x3b;
  local_c = 9;
  FUN_00430b00(1,0,0x3f7d70a4,0x3b,9);
  FUN_00411580(1,&local_4,&local_8,&local_c);
  FUN_00430b00(1,1,local_4,local_8,local_c);
  return;
}

