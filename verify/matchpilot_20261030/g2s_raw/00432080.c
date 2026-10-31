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

extern int DAT_0067eca4;
extern undefined4 DAT_0067f19c;
extern int DAT_007f1a0c;
extern undefined1 DAT_007f1041;
extern undefined1 DAT_007f1501;
extern char DAT_007f108d;
extern char DAT_007f154d;
extern char DAT_007f10d9;
extern char DAT_007f1599;
extern char DAT_007f1125;
extern char DAT_007f15e5;
extern char DAT_007f1171;
extern char DAT_007f1631;
extern char DAT_007f11bd;
extern char DAT_007f167d;
extern char DAT_007f1209;
extern char DAT_007f16c9;
extern char DAT_007f1255;
extern char DAT_007f1715;
extern undefined4 DAT_0067ecac;
extern undefined4 DAT_0067e844;
extern undefined4 DAT_0067e9f8;
extern undefined4 DAT_0067e914;
extern undefined4 DAT_007f1a14;
undefined4 FUN_0040e470(int p0);
void FUN_0042d3a0(void);
// MATCH: 0x00432080 FUN_00432080
undefined4 FUN_00432080(undefined4 param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  DAT_0067f19c = 0;
  if (DAT_0067eca4 == 0) {
    if (DAT_007f1a0c == 1) {
      if (((DAT_007f1a14 != -1) && ((&DAT_007f1041)[DAT_007f1a14 * 0x4c] != '\0')) &&
         ((&DAT_007f1501)[DAT_007f1a14 * 0x4c] == '\0')) {
        DAT_0067eca4 = 1;
        goto LAB_004321c3;
      }
    }
    else {
      if (DAT_007f1a0c == 2) {
        iVar3 = 0;
        piVar2 = &DAT_007f1a14;
        do {
          iVar1 = FUN_0040e470(iVar3);
          if (((iVar1 == 1) && (*piVar2 != -1)) &&
             ((iVar1 = *piVar2 * 0x4c, (&DAT_007f1041)[iVar1] != '\0' &&
              ((&DAT_007f1501)[iVar1] == '\0')))) {
            DAT_0067eca4 = 1;
          }
          piVar2 = piVar2 + 4;
          iVar3 = iVar3 + 1;
        } while ((int)piVar2 < 0x7f1a54);
      }
      else {
        if (DAT_007f1a0c != 0x1000) {
          DAT_0067f19c = 0;
          return 0;
        }
        if ((DAT_007f1041 != '\0') && (DAT_007f1501 == '\0')) {
          DAT_0067eca4 = 1;
        }
        if ((DAT_007f108d != '\0') && (DAT_007f154d == '\0')) {
          DAT_0067eca4 = 1;
        }
        if ((DAT_007f10d9 != '\0') && (DAT_007f1599 == '\0')) {
          DAT_0067eca4 = 1;
        }
        if ((DAT_007f1125 != '\0') && (DAT_007f15e5 == '\0')) {
          DAT_0067eca4 = 1;
        }
        if ((DAT_007f1171 != '\0') && (DAT_007f1631 == '\0')) {
          DAT_0067eca4 = 1;
        }
        if ((DAT_007f11bd != '\0') && (DAT_007f167d == '\0')) {
          DAT_0067eca4 = 1;
        }
        if ((DAT_007f1209 != '\0') && (DAT_007f16c9 == '\0')) {
          DAT_0067eca4 = 1;
        }
        if ((DAT_007f1255 != '\0') && (DAT_007f1715 == '\0')) {
          DAT_0067eca4 = 1;
          goto LAB_004321c3;
        }
      }
      if (DAT_0067eca4 == 1) {
LAB_004321c3:
        FUN_0042d3a0();
        DAT_0067ecac = param_1;
        DAT_0067e844 = 0;
        DAT_0067e9f8 = 0;
        DAT_0067e914 = 0;
        DAT_0067f19c = 2;
        return 1;
      }
    }
  }
  return 0;
}

