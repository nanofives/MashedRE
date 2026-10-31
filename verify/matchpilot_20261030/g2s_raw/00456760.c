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

extern undefined4 DAT_00688670;
extern undefined1 DAT_00688680;
extern undefined4 DAT_00688694;
extern undefined4 DAT_00688698;
extern undefined4 DAT_0068869c;
extern undefined4 DAT_006886a0;
extern undefined4 DAT_006886a4;
extern undefined1 DAT_006886ac;
extern undefined1 DAT_006887d0;
undefined4 FUN_0040bb30(undefined4 p0);
undefined4 FUN_0042a5d0(char * p0);
void FUN_00474d60(int p0);
void FUN_00476cb0(int p0, undefined4 p1, undefined4 p2);
void FUN_004770c0(uint * p0, uint p1, uint p2, undefined4 p3);
undefined4 FUN_004b3fc0(undefined4 p0, undefined4 p1);
undefined4 FUN_004b5190(int p0, undefined4 p1, undefined4 p2);
void FUN_004c5c80(undefined4 p0);
int FUN_004e6ab0(int p0);
undefined4 RpClumpDestroy(int p0);
// MATCH: 0x00456760 FUN_00456760
void FUN_00456760(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  undefined4 local_28 [10];
  int iVar6;
  
  uVar2 = FUN_0040bb30("flash");
  FUN_004770c0(&DAT_006887d0,0x817,0x32,uVar2);
  FUN_00476cb0(&DAT_006887d0,5,6);
  FUN_004c5c80(0);
  uVar2 = FUN_0042a5d0("GatlingGun.dff",0,0);
  iVar6 = 0x120;
  do {
    iVar5 = iVar6 + -0x48;
    *(undefined4 *)(&DAT_00688680 + iVar6) = 0;
    uVar3 = FUN_004e6ab0(uVar2);
    *(undefined4 *)((int)&DAT_00688670 + iVar6) = uVar3;
    iVar4 = FUN_004b3fc0(uVar3,local_28);
    while (iVar4 != 0) {
      iVar1 = iVar4 + -1;
      iVar4 = iVar4 + -1;
      uVar3 = FUN_004b5190(local_28[iVar1],0,0);
      switch(uVar3) {
      case 0:
        *(undefined4 *)((int)&DAT_0068869c + iVar6) = local_28[iVar4];
        break;
      case 1:
        *(undefined4 *)((int)&DAT_006886a0 + iVar6) = local_28[iVar4];
        break;
      case 2:
        *(undefined4 *)((int)&DAT_006886a4 + iVar6) = local_28[iVar4];
        break;
      case 3:
        *(undefined4 *)(&DAT_006886ac + iVar6) = local_28[iVar4];
        break;
      case 4:
        *(undefined4 *)((int)&DAT_00688698 + iVar6) = local_28[iVar4];
        break;
      case 5:
        *(undefined4 *)(iVar6 + 0x6886a8) = local_28[iVar4];
        break;
      case 6:
        *(undefined4 *)((int)&DAT_00688694 + iVar6) = local_28[iVar4];
      }
    }
    FUN_00474d60(uVar2,0);
    iVar6 = iVar5;
  } while (iVar5 != 0);
  RpClumpDestroy(uVar2);
  return;
}

