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

undefined4 FUN_004850b0(int p0);
void FUN_004853f0(undefined2 p0);
void FUN_004987b0(undefined4 p0);
undefined4 FUN_004b3fc0(undefined4 p0, undefined4 p1);
void FUN_004b4270(undefined4 p0);
undefined4 FUN_004c5930(int p0);
undefined4 FUN_004e4800(int p0);
undefined4 FUN_00546320(int p0);
undefined4 FUN_00558d30(int * p0);
undefined4 FUN_00562560(undefined4 p0);
undefined4 RpClumpDestroy(int p0);
undefined4 RpWorldDestroy(int p0);
undefined4 RpWorldRemoveLight(undefined4 p0, int p1);
undefined4 RwFrameRemoveChild(undefined4 p0, int p1);
// MATCH: 0x00478cf0 FUN_00478cf0
void FUN_00478cf0(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  char *pcVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  undefined4 *puVar7;
  undefined4 local_100 [64];
  
  iVar4 = 0;
  if (0 < *(int *)(param_2 + 0x6c)) {
    pcVar3 = (char *)(param_2 + 0x2c);
    do {
      if (*(short *)(pcVar3 + 2) != -1) {
        iVar1 = FUN_004b3fc0(*(undefined4 *)(param_1 + 0x10110 + *pcVar3 * 4),local_100);
        iVar5 = 0;
        if (0 < iVar1) {
          do {
            uVar2 = FUN_004850b0(local_100[iVar5]);
            FUN_004853f0(uVar2);
            iVar5 = iVar5 + 1;
          } while (iVar5 < iVar1);
        }
      }
      iVar4 = iVar4 + 1;
      pcVar3 = pcVar3 + 4;
    } while (iVar4 < *(int *)(param_2 + 0x6c));
  }
  iVar4 = 0;
  if (0 < *(int *)(param_2 + 0x1c)) {
    piVar6 = (int *)(param_1 + 0x10090);
    do {
      if (*piVar6 != 0) {
        RpWorldDestroy(*piVar6);
        *piVar6 = 0;
      }
      iVar4 = iVar4 + 1;
      piVar6 = piVar6 + 1;
    } while (iVar4 < *(int *)(param_2 + 0x1c));
  }
  iVar4 = 0;
  if (0 < *(int *)(param_2 + 0x20)) {
    piVar6 = (int *)(param_1 + 0x100a0);
    do {
      if (*piVar6 != 0) {
        RpWorldDestroy(*piVar6);
        *piVar6 = 0;
      }
      iVar4 = iVar4 + 1;
      piVar6 = piVar6 + 1;
    } while (iVar4 < *(int *)(param_2 + 0x20));
  }
  piVar6 = (int *)(param_1 + 0x100b0);
  iVar4 = 0x10;
  do {
    if (*piVar6 != 0) {
      FUN_00546320(*piVar6);
      *piVar6 = 0;
    }
    piVar6 = piVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  piVar6 = (int *)(param_1 + 0x100f0);
  iVar4 = 8;
  do {
    if (*piVar6 != 0) {
      RpClumpDestroy(*piVar6);
      *piVar6 = 0;
    }
    piVar6 = piVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if (*(int *)(param_1 + 0x105d8) != 0) {
    RpClumpDestroy(*(int *)(param_1 + 0x105d8));
  }
  if (*(int *)(param_1 + 0x105e0) != 0) {
    RpWorldRemoveLight(*(undefined4 *)(param_1 + 0x105d4),*(int *)(param_1 + 0x105e0));
  }
  if (*(int *)(param_1 + 0x105e4) != 0) {
    RpWorldRemoveLight(*(undefined4 *)(param_1 + 0x105d4),*(int *)(param_1 + 0x105e4));
  }
  if (*(int *)(param_1 + 0x105dc) != 0) {
    RpClumpDestroy(*(int *)(param_1 + 0x105dc));
  }
  piVar6 = (int *)(param_1 + 0x10110);
  iVar4 = 0x40;
  do {
    iVar1 = *piVar6;
    if (iVar1 != 0) {
      iVar5 = FUN_004e4800(iVar1);
      if (iVar5 != 0) {
        RwFrameRemoveChild(iVar5,iVar1);
      }
      RpClumpDestroy(iVar1);
    }
    piVar6 = piVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if (*(int *)(param_1 + 0x105f0) != 0) {
    FUN_00562560(*(int *)(param_1 + 0x105f0));
  }
  piVar6 = (int *)(param_1 + 0x105b0);
  iVar4 = 8;
  do {
    if (*piVar6 != 0) {
      FUN_00562560(*piVar6);
    }
    piVar6 = piVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if (*(int *)(param_1 + 0x105d4) != 0) {
    FUN_004b4270(*(int *)(param_1 + 0x105d4));
    RpWorldDestroy(*(undefined4 *)(param_1 + 0x105d4));
    *(undefined4 *)(param_1 + 0x105d4) = 0;
    FUN_004987b0("World Destroyed\n");
  }
  iVar4 = 0;
  if (0 < *(int *)(param_2 + 4)) {
    puVar7 = (undefined4 *)(param_1 + 0x10040);
    do {
      RpWorldDestroy(*puVar7);
      iVar4 = iVar4 + 1;
      puVar7 = puVar7 + 1;
    } while (iVar4 < *(int *)(param_2 + 4));
  }
  RpWorldDestroy(*(undefined4 *)(param_1 + 0x10080));
  piVar6 = (int *)(param_1 + 0x10618);
  iVar4 = 8;
  do {
    if (*piVar6 != 0) {
      FUN_00558d30(*piVar6);
      *piVar6 = 0;
    }
    piVar6 = piVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if (*(int *)(param_1 + 0x105d0) != 0) {
    FUN_004c5930(*(int *)(param_1 + 0x105d0));
    FUN_004987b0("TextureDictionary Destroyed\n");
  }
  return;
}

