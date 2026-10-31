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

extern undefined4 DAT_007f1a1c;
extern undefined4 DAT_0088f680;
extern undefined1 DAT_0088f6a0;
void FUN_0045c720(int p0, int p1);
int FUN_0045c820(int p0);
int FUN_004b44f0(int p0, int p1);
// MATCH: 0x0045cd30 FUN_0045cd30
void FUN_0045cd30(int param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined8 uVar9;
  undefined4 uVar10;
  int local_14;
  int local_c;
  int local_8;
  
  iVar5 = *(int *)(&DAT_0088f6a0 + param_1 * 4);
  if (*(int *)(iVar5 + 0x18) < 1) {
    iVar4 = *(int *)(iVar5 + 0x20);
    iVar8 = *(int *)(iVar5 + 0x1c);
  }
  else {
    iVar4 = *(int *)(iVar5 + 0x20) / *(int *)(iVar5 + 0x18);
    iVar8 = param_1;
    switch((&DAT_007f1a1c)[*(int *)(iVar5 + 4) * 4]) {
    case 0:
      switch((&DAT_007f1a1c)[*(int *)(iVar5 + 8) * 4]) {
      case 0:
      case 3:
      case 4:
        goto switchD_0045cd6a_caseD_3;
      case 1:
switchD_0045cdd1_caseD_0:
        iVar8 = *(int *)(iVar5 + 0x1c);
        break;
      case 2:
switchD_0045cdd1_caseD_1:
        iVar8 = *(int *)(iVar5 + 0x1c) + iVar4;
        break;
      case 5:
switchD_0045cdd1_caseD_2:
        iVar8 = *(int *)(iVar5 + 0x1c) + iVar4 * 2;
      }
      break;
    case 1:
      switch((&DAT_007f1a1c)[*(int *)(iVar5 + 8) * 4]) {
      case 0:
        goto switchD_0045cdd1_caseD_0;
      case 1:
      case 3:
      case 4:
        goto switchD_0045cd6a_caseD_3;
      case 2:
        goto switchD_0045cdd1_caseD_1;
      case 5:
        goto switchD_0045cdd1_caseD_2;
      }
      break;
    case 2:
      switch((&DAT_007f1a1c)[*(int *)(iVar5 + 8) * 4]) {
      case 0:
        goto switchD_0045cdd1_caseD_0;
      case 1:
        goto switchD_0045cdd1_caseD_1;
      case 2:
      case 3:
      case 4:
        goto switchD_0045cd6a_caseD_3;
      case 5:
        goto switchD_0045cdd1_caseD_2;
      }
      break;
    case 3:
    case 4:
      goto switchD_0045cd6a_caseD_3;
    case 5:
      switch((&DAT_007f1a1c)[*(int *)(iVar5 + 8) * 4]) {
      case 0:
        goto switchD_0045cdd1_caseD_0;
      case 1:
        goto switchD_0045cdd1_caseD_1;
      case 2:
        goto switchD_0045cdd1_caseD_2;
      case 3:
      case 4:
      case 5:
        goto switchD_0045cd6a_caseD_3;
      }
    }
  }
  if (iVar4 == 0xd) {
    iVar5 = FUN_004b44f0(0,6);
    uVar10 = 6;
LAB_0045ce25:
    iVar6 = FUN_004b44f0(0,uVar10);
    iVar7 = FUN_004b44f0(0,1);
    iVar5 = iVar5 + iVar6 + iVar7;
  }
  else {
    if (iVar4 == 0x19) {
      iVar5 = FUN_004b44f0(0,0xc);
      uVar10 = 0xc;
      goto LAB_0045ce25;
    }
    iVar5 = param_1;
    if (iVar4 == 0x23) {
      iVar5 = FUN_004b44f0(0,0x11);
      uVar10 = 0x11;
      goto LAB_0045ce25;
    }
  }
  if (iVar8 != -1) {
    FUN_0045c720(param_1,iVar8);
    piVar1 = &DAT_0088f680 + param_1;
    if ((&DAT_0088f680)[param_1] != 0) {
      local_c = -1;
      if (0 < iVar4) {
        iVar8 = iVar5 - iVar4;
        local_14 = iVar5 * 0x2c;
        local_8 = iVar4;
        do {
          iVar6 = local_14;
          iVar7 = iVar5;
          if (iVar4 <= iVar5) {
            iVar6 = local_14 + iVar4 * -0x2c;
            iVar7 = iVar8;
          }
          iVar3 = *(int *)(*piVar1 + 0x10 + iVar6);
          if (iVar3 == 0) {
            if (local_c == -1) {
              *(undefined4 *)(*piVar1 + 0x10 + iVar6) = 1;
              local_c = iVar7;
            }
          }
          else {
            *(int *)(*piVar1 + 0x10 + iVar6) = iVar3 + 1;
            piVar2 = (int *)(*piVar1 + 0x10 + iVar6);
            if (iVar4 <= *piVar2) {
              *piVar2 = 0;
            }
          }
          local_14 = local_14 + 0x2c;
          iVar5 = iVar5 + 1;
          iVar8 = iVar8 + 1;
          local_8 = local_8 + -1;
        } while (local_8 != 0);
        if (local_c != -1) {
          iVar5 = *piVar1 + local_c * 0x2c;
          *piVar1 = iVar5;
          *(undefined4 *)(iVar5 + 0x14) = *(undefined4 *)(*(int *)(&DAT_0088f6a0 + param_1 * 4) + 8)
          ;
          *(undefined4 *)(*piVar1 + 4) = *(undefined4 *)(*(int *)(&DAT_0088f6a0 + param_1 * 4) + 4);
        }
      }
      *(undefined4 *)(*piVar1 + 0x28) = **(undefined4 **)(&DAT_0088f6a0 + param_1 * 4);
      uVar9 = FUN_0045c820(param_1);
      *(int *)(*(int *)((ulonglong)uVar9 >> 0x20) + 0x24) = (int)uVar9;
    }
  }
switchD_0045cd6a_caseD_3:
  return;
}

