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

extern int DAT_0063cde4;
extern int DAT_007f0fd0;
extern undefined4 DAT_007f1a1c;
undefined4 FUN_0042f500(void);
// MATCH: 0x0041c380 FUN_0041c380
void FUN_0041c380(void)

{
  uint uVar1;
  int iVar2;
  int unaff_ESI;
  
  *(undefined4 *)(unaff_ESI + 0x10c) = 0;
  *(undefined4 *)(unaff_ESI + 0x110) = 0;
  iVar2 = DAT_0063cde4;
  uVar1 = 1 << ((byte)(&DAT_007f1a1c)[*(int *)(unaff_ESI + 0x108) * 4] & 0x1f);
  *(uint *)(unaff_ESI + 0x10c) = uVar1;
  if (3 < iVar2) {
    iVar2 = FUN_0042f500();
    if (((iVar2 == 0) && (DAT_007f0fd0 != 1)) && (DAT_007f0fd0 != 2)) {
      uVar1 = *(uint *)(unaff_ESI + 0x10c) | 0x800;
      goto LAB_0041c3e3;
    }
    uVar1 = *(uint *)(unaff_ESI + 0x10c);
  }
  uVar1 = uVar1 | 0x400;
LAB_0041c3e3:
  iVar2 = DAT_0063cde4;
  *(uint *)(unaff_ESI + 0x10c) = uVar1;
  if (*(int *)(unaff_ESI + 0x108) < iVar2) {
    *(undefined4 *)(unaff_ESI + 0x110) = 1;
  }
  return;
}

