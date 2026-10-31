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

extern int DAT_008a94a0;
extern undefined4 * DAT_0063bc54;
extern int DAT_007d3ff8;
extern undefined1 DAT_00828320;
extern undefined2 DAT_0089d4a0;
// MATCH: 0x00413a40 FUN_00413a40
void FUN_00413a40(void)



{

  undefined4 uStack_8;

  undefined4 uStack_4;

  

  if (DAT_008a94a0 != 0) {

    if (!(DAT_0063bc54 == (undefined4 *)0x0)) {

      (**(code **)(DAT_007d3ff8 + 0x20))(1,*DAT_0063bc54);

    } else {

      (**(code **)(DAT_007d3ff8 + 0x20))(1,0);

    }

    (**(code **)(DAT_007d3ff8 + 0x20))(9,2);

    (**(code **)(DAT_007d3ff8 + 0x20))(0xc,1);

    (**(code **)(DAT_007d3ff8 + 0x24))(0x14,&uStack_8);

    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,1);

    (**(code **)(DAT_007d3ff8 + 0x24))(0xe,&uStack_4);

    (**(code **)(DAT_007d3ff8 + 0x20))(0xe,0);

    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);

    (**(code **)(DAT_007d3ff8 + 0x20))(8,0);

    (**(code **)(DAT_007d3ff8 + 0x20))(10,5);

    (**(code **)(DAT_007d3ff8 + 0x20))(0xb,2);

    (**(code **)(DAT_007d3ff8 + 0x34))

              (3,&DAT_00828320,DAT_008a94a0 * 4,&DAT_0089d4a0,DAT_008a94a0 * 6);

    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);

    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,uStack_8);

    (**(code **)(DAT_007d3ff8 + 0x20))(0xe,uStack_4);

    (**(code **)(DAT_007d3ff8 + 0x20))(10,5);

    (**(code **)(DAT_007d3ff8 + 0x20))(0xb,6);

  }

  return;

}



