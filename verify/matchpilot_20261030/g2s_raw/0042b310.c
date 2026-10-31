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

extern int DAT_007f1a0c;
extern undefined1 DAT_007f1044;
extern char DAT_007f1090;
extern char DAT_007f10dc;
extern char DAT_007f1128;
extern char DAT_007f1174;
extern char DAT_007f11c0;
extern char DAT_007f120c;
extern char DAT_007f1258;
extern int DAT_0067e9f8;
extern undefined1 DAT_007f1504;
extern char DAT_007f1550;
extern char DAT_007f159c;
extern char DAT_007f15e8;
extern char DAT_007f1634;
extern char DAT_007f1680;
extern char DAT_007f16cc;
extern char DAT_007f1718;
extern undefined1 DAT_0067f1b8;
extern undefined1 DAT_0067ed3c;
extern undefined1 DAT_0067ed40;
extern undefined4 DAT_007f1a14;
undefined4 FUN_0040e470(int p0);
extern int _DAT_0067f1b8;
// MATCH: 0x0042b310 FUN_0042b310
/* [C2 2026-05-11] Phase 1 — detect active input at byte-col +0: */

undefined4 FUN_0042b310(void)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  
  uVar3 = 0;
  bVar1 = false;
  if (DAT_007f1a0c == 0x1000) {
    bVar1 = DAT_007f120c != '\0' ||
            (DAT_007f11c0 != '\0' ||
            (DAT_007f1174 != '\0' ||
            (DAT_007f1128 != '\0' ||
            (DAT_007f10dc != '\0' || (DAT_007f1090 != '\0' || DAT_007f1044 != '\0')))));
    if (DAT_007f1258 == '\0') goto LAB_0042b3cb;
  }
  else {
    iVar5 = 0;
    piVar4 = &DAT_007f1a14;
    do {
      if (((*piVar4 != -1) && (iVar2 = FUN_0040e470(iVar5), iVar2 != 2)) &&
         ((&DAT_007f1044)[*piVar4 * 0x4c] != '\0')) {
        bVar1 = true;
      }
      piVar4 = piVar4 + 4;
      iVar5 = iVar5 + 1;
    } while ((int)piVar4 < 0x7f1a54);
LAB_0042b3cb:
    if (!bVar1) {
      _DAT_0067f1b8 = 0;
      goto LAB_0042b419;
    }
  }
  if (*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) == 0x13) {
    iVar5 = *(int *)(&DAT_0067ed40 + DAT_0067e9f8 * 0x40);
    if ((iVar5 != 0) && (iVar5 != 1)) {
      if (iVar5 == 2) {
        return 1;
      }
      goto LAB_0042b419;
    }
  }
  else if (*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) != 0x1e) {
LAB_0042b419:
    if (DAT_007f1a0c == 0x1000) {
      if ((DAT_007f1044 != '\0') && (DAT_007f1504 == '\0')) {
        uVar3 = 1;
      }
      if ((DAT_007f1090 != '\0') && (DAT_007f1550 == '\0')) {
        uVar3 = 1;
      }
      if ((DAT_007f10dc != '\0') && (DAT_007f159c == '\0')) {
        uVar3 = 1;
      }
      if ((DAT_007f1128 != '\0') && (DAT_007f15e8 == '\0')) {
        uVar3 = 1;
      }
      if ((DAT_007f1174 != '\0') && (DAT_007f1634 == '\0')) {
        uVar3 = 1;
      }
      if ((DAT_007f11c0 != '\0') && (DAT_007f1680 == '\0')) {
        uVar3 = 1;
      }
      if ((DAT_007f120c != '\0') && (DAT_007f16cc == '\0')) {
        uVar3 = 1;
      }
      if ((DAT_007f1258 != '\0') && (DAT_007f1718 == '\0')) {
        return 1;
      }
    }
    else {
      iVar5 = 0;
      piVar4 = &DAT_007f1a14;
      do {
        if (((*piVar4 != -1) && (iVar2 = FUN_0040e470(iVar5), iVar2 != 2)) &&
           (((&DAT_007f1044)[*piVar4 * 0x4c] != '\0' && ((&DAT_007f1504)[*piVar4 * 0x4c] == '\0'))))
        {
          uVar3 = 1;
        }
        piVar4 = piVar4 + 4;
        iVar5 = iVar5 + 1;
      } while ((int)piVar4 < 0x7f1a54);
    }
    return uVar3;
  }
  return 1;
}

