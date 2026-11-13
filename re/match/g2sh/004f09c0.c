// FLAGS: /O2 /GS
// TOOLCHAIN rw9044
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

extern int DAT_00912118;
extern int DAT_0091211c;
extern int DAT_007d7290;
extern int DAT_007d3ff8;
undefined4 FUN_004cc7f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
// MATCH: 0x004f09c0 FUN_004f09c0

undefined4 FUN_004f09c0(undefined4 param_1,int param_2)

{
  DAT_00912118 = param_2;
  if (DAT_0091211c == 0) {
    DAT_007d7290 = FUN_004cc7f0(0xc,0x32,4,0x40502);
    if (DAT_007d7290 == 0) {
      return 0;
    }
  }
  *(undefined2 *)(DAT_007d3ff8 + DAT_00912118) = 1;
  DAT_0091211c = DAT_0091211c + 1;
  *(undefined1 *)(DAT_007d3ff8 + 8 + DAT_00912118) = 3;
  *(undefined1 *)(DAT_007d3ff8 + 9 + DAT_00912118) = 4;
  *(undefined1 *)(DAT_007d3ff8 + 10 + DAT_00912118) = 5;
  *(undefined1 *)(DAT_007d3ff8 + 0xc + DAT_00912118) = 1;
  *(undefined1 *)(DAT_007d3ff8 + 0x10 + DAT_00912118) = 2;
  *(undefined1 *)(DAT_007d3ff8 + 0x18 + DAT_00912118) = 6;
  *(undefined1 *)(DAT_007d3ff8 + 0x28 + DAT_00912118) = 4;
  *(undefined1 *)(DAT_007d3ff8 + 0x29 + DAT_00912118) = 8;
  *(undefined1 *)(DAT_007d3ff8 + 0x2a + DAT_00912118) = 0;
  *(undefined1 *)(DAT_007d3ff8 + 0x2b + DAT_00912118) = 1;
  *(undefined1 *)(DAT_007d3ff8 + 0x2c + DAT_00912118) = 2;
  *(undefined1 *)(DAT_007d3ff8 + 0x2d + DAT_00912118) = 0x10;
  return param_1;
}

