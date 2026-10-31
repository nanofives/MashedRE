// FLAGS: /O2 /GS /Ob1
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

extern undefined4 DAT_0063a49c;
extern undefined4 DAT_0063a498;
extern undefined4 DAT_0063a494;
extern undefined4 DAT_0063a4bc;
extern undefined4 DAT_0063a4b8;
extern undefined1 DAT_0063a4b4;
extern undefined1 DAT_0063a4dc;
extern undefined1 DAT_0063a4d8;
extern undefined1 DAT_0063a4d4;
extern undefined1 DAT_0063a4fc;
extern undefined1 DAT_0063a4f8;
extern undefined1 DAT_0063a4f4;
extern undefined1 DAT_0063a51c;
extern undefined1 DAT_0063a518;
extern undefined1 DAT_0063a514;
extern undefined1 DAT_0063a53c;
extern undefined1 DAT_0063a538;
extern undefined1 DAT_0063a534;
extern undefined1 DAT_0063a55c;
extern undefined1 DAT_0063a558;
extern undefined1 DAT_0063a554;
extern undefined1 DAT_0063a57c;
extern undefined1 DAT_0063a578;
extern undefined1 DAT_0063a574;
extern undefined1 DAT_0063a59c;
extern undefined1 DAT_0063a598;
extern undefined1 DAT_0063a594;
extern undefined1 DAT_0063a5bc;
extern undefined1 DAT_0063a5b8;
extern undefined1 DAT_0063a5b4;
extern int _DAT_0063a4b4;
extern int _DAT_0063a4dc;
extern int _DAT_0063a4d8;
extern int _DAT_0063a4d4;
extern int _DAT_0063a4fc;
extern int _DAT_0063a4f8;
extern int _DAT_0063a4f4;
extern int _DAT_0063a51c;
extern int _DAT_0063a518;
extern int _DAT_0063a514;
extern int _DAT_0063a53c;
extern int _DAT_0063a538;
extern int _DAT_0063a534;
extern int _DAT_0063a55c;
extern int _DAT_0063a558;
extern int _DAT_0063a554;
extern int _DAT_0063a57c;
extern int _DAT_0063a578;
extern int _DAT_0063a574;
extern int _DAT_0063a59c;
extern int _DAT_0063a598;
extern int _DAT_0063a594;
extern int _DAT_0063a5bc;
extern int _DAT_0063a5b8;
extern int _DAT_0063a5b4;
// MATCH: 0x00406370 FUN_00406370
/* [C2 2026-06-03] `void FUN_00406370(void)`. Body 0x00406370..0x00406408. Pure zero-init of
   subsystem B (10-slot array at 0x0063a490… */

void FUN_00406370(void)

{
  DAT_0063a49c = 0;
  DAT_0063a498 = 0;
  DAT_0063a494 = 0;
  DAT_0063a4bc = 0;
  DAT_0063a4b8 = 0;
  _DAT_0063a4b4 = 0;
  _DAT_0063a4dc = 0;
  _DAT_0063a4d8 = 0;
  _DAT_0063a4d4 = 0;
  _DAT_0063a4fc = 0;
  _DAT_0063a4f8 = 0;
  _DAT_0063a4f4 = 0;
  _DAT_0063a51c = 0;
  _DAT_0063a518 = 0;
  _DAT_0063a514 = 0;
  _DAT_0063a53c = 0;
  _DAT_0063a538 = 0;
  _DAT_0063a534 = 0;
  _DAT_0063a55c = 0;
  _DAT_0063a558 = 0;
  _DAT_0063a554 = 0;
  _DAT_0063a57c = 0;
  _DAT_0063a578 = 0;
  _DAT_0063a574 = 0;
  _DAT_0063a59c = 0;
  _DAT_0063a598 = 0;
  _DAT_0063a594 = 0;
  _DAT_0063a5bc = 0;
  _DAT_0063a5b8 = 0;
  _DAT_0063a5b4 = 0;
  return;
}

