// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (73.7% ddk) to 100%: end pointer = param_1 + (RwGlobals slot +0xf4)(param_1), list head at DAT_007d4628 + DAT_007d3ff8 + 0x21c, next at +0x30; for each entry (slot +0xcc)(end, e) then FUN_00550b00(param_1), then the same with e + 0x14; a hit stores the entry pointer to *param_2 and returns 0; no hit returns param_1. Flags /O2 /Ob1 /G5. */
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

extern int DAT_007d3ff8;
extern int DAT_007d4628;
int FUN_00550b00(char * p0);
// MATCH: 0x004ce6f0 FUN_004ce6f0

#define DRVG(off) (*(int *)(DAT_007d4628 + DAT_007d3ff8 + (off)))
typedef int (*strfn)();
int FUN_004ce6f0(char *param_1,int *param_2)
{
  char *end = param_1 + (*(strfn *)(DAT_007d3ff8 + 0xf4))(param_1);
  int e;
  for (e = DRVG(0x21c); e != 0; e = *(int *)(e + 0x30)) {
    (*(strfn *)(DAT_007d3ff8 + 0xcc))(end, e);
    if (FUN_00550b00(param_1)) {
      *param_2 = e;
      return 0;
    }
    (*(strfn *)(DAT_007d3ff8 + 0xcc))(end, e + 0x14);
    if (FUN_00550b00(param_1)) {
      *param_2 = e + 0x14;
      return 0;
    }
  }
  return (int)param_1;
}
