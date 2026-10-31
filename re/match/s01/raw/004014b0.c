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

extern int DAT_00636570;
extern undefined4 DAT_00636560;
extern int DAT_0063656c;
extern undefined4 DAT_00636564;
void FUN_004c0740(int p0, int p1);
undefined4 FUN_004c0c20(int p0);
undefined4 FUN_004e4d90(undefined4 p0);
undefined4 RpClumpDestroy(int p0);
undefined4 RpWorldDestroy(int p0);
undefined4 RpWorldRemoveLight(undefined4 p0, int p1);
undefined4 RwFrameRemoveChild(undefined4 p0, int p1);
// MATCH: 0x004014b0 FUN_004014b0
void FUN_004014b0(void)

{
  RpWorldRemoveLight(DAT_00636560,DAT_00636570);
  RpWorldRemoveLight(DAT_00636560,DAT_0063656c);
  if (*(int *)(DAT_00636570 + 4) != 0) {
    FUN_004c0c20(*(int *)(DAT_00636570 + 4));
  }
  FUN_004c0740(DAT_00636570,0);
  FUN_004e4d90(DAT_00636570);
  if (*(int *)(DAT_0063656c + 4) != 0) {
    FUN_004c0c20(*(int *)(DAT_0063656c + 4));
  }
  FUN_004c0740(DAT_0063656c,0);
  FUN_004e4d90(DAT_0063656c);
  RwFrameRemoveChild(DAT_00636560,DAT_00636564);
  RpWorldDestroy(DAT_00636560);
  RpClumpDestroy(DAT_00636564);
  return;
}

