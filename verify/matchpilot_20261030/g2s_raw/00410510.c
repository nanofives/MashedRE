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

extern undefined4 DAT_0063b90c;
extern int DAT_008a94d0;
extern int DAT_007f0fd0;
extern undefined * PTR_PTR_005f2770;
extern undefined4 DAT_0063ba8c;
extern undefined1 DAT_0063b910;
extern undefined1 DAT_0063b918;
extern undefined1 DAT_0063b91c;
extern int DAT_0063b914;
extern uint DAT_007f0fcc;
extern undefined1 DAT_005cc574;
extern float DAT_007f0fe4;
extern float DAT_005d757c;
extern undefined1 DAT_005cc9c8;
extern undefined1 DAT_005cc31c;
bool FUN_00405890(void);
undefined4 FUN_0040b6d0(int p0);
undefined4 FUN_0040e340(void);
float10 FUN_00417730(int p0);
undefined4 FUN_00417740(void);
undefined4 FUN_00426c00(void);
undefined4 FUN_0042f500(void);
undefined4 FUN_0042f6a0(void);
void FUN_00448700(undefined4 p0, undefined4 p1);
void FUN_0044df80(int p0);
void FUN_00458f80(undefined4 p0);
void FUN_0045bed0(void);
void FUN_0045bf30(void);
undefined4 FUN_0046c7b0(uint p0);
undefined4 FUN_0046cbb0(uint p0, undefined4 * p1, undefined4 * p2);
extern int _DAT_005cc31c;
extern int _DAT_005cc9c8;
extern int _DAT_005cc574;
extern int _DAT_0063b910;
extern int _DAT_0063b918;
extern int _DAT_0063b91c;
// MATCH: 0x00410510 FUN_00410510
int FUN_00410510(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  float10 fVar7;
  int local_c;
  float fStack_8;
  undefined1 local_4 [4];
  
  iVar6 = 0;
  DAT_0063b90c = 0;
  iVar2 = FUN_0042f6a0();
  if (iVar2 == 2) {
    return 0;
  }
  iVar2 = 1;
  do {
    iVar3 = FUN_0040b6d0(iVar2 + -1);
    if ((((DAT_008a94d0 == 2) || (DAT_008a94d0 == 3)) || (iVar4 = FUN_0042f500(), iVar4 != 0)) &&
       (iVar3 == 8)) {
      iVar6 = iVar2;
    }
    if ((DAT_007f0fd0 == 2) && (iVar3 == 8)) {
      iVar6 = iVar2;
    }
    if ((DAT_008a94d0 == 4) && (0xb < iVar3)) {
      iVar6 = iVar2;
    }
    bVar1 = iVar2 < 4;
    iVar2 = iVar2 + 1;
  } while (bVar1);
  if (iVar6 != 0) {
    iVar3 = 0;
    iVar2 = 0x34;
    do {
      if ((*(int *)(PTR_PTR_005f2770 + iVar2) != 0) && (iVar4 = FUN_0046c7b0(iVar3), iVar4 == 1)) {
        iVar4 = FUN_0040b6d0(iVar3);
        if (((DAT_008a94d0 == 2) || ((DAT_008a94d0 == 3 || (iVar5 = FUN_0042f500(), iVar5 != 0))))
           && (iVar4 == 8)) {
          iVar6 = iVar3 + 1;
        }
        if ((DAT_008a94d0 == 4) && (0xb < iVar4)) {
          iVar6 = iVar3 + 1;
        }
      }
      iVar2 = iVar2 + 4;
      iVar3 = iVar3 + 1;
    } while (iVar2 < 0x44);
  }
  switch(DAT_007f0fd0) {
  case 4:
    iVar6 = FUN_00417740(0);
    iVar6 = iVar6 + 1;
    break;
  case 5:
    iVar6 = FUN_0046c7b0(0);
    if (iVar6 == 0) {
      iVar6 = -1;
      goto LAB_0041062a;
    }
    iVar6 = FUN_00405890();
    break;
  case 7:
    iVar3 = 0;
    iVar2 = FUN_0040e340();
    if (0 < iVar2) {
      do {
        fVar7 = (float10)FUN_00417730(iVar3);
        if ((float10)_DAT_005cc31c < fVar7) {
          iVar6 = iVar3 + 1;
          break;
        }
        iVar3 = iVar3 + 1;
        iVar2 = FUN_0040e340();
      } while (iVar3 < iVar2);
    }
    break;
  case 8:
    iVar6 = FUN_00417740(0);
    iVar6 = iVar6 + 1;
    if (iVar6 != 1) {
      if (iVar6 == 0) {
        iVar6 = -1;
      }
      goto LAB_0041062a;
    }
    break;
  case 9:
    iVar6 = FUN_00417740(0);
    if (iVar6 != -1) {
LAB_00410801:
      iVar6 = -1;
      DAT_007f0fcc = 0;
      goto LAB_0041062a;
    }
    FUN_0046cbb0(0,&local_c,local_4);
    if (local_c != 0) {
      iVar6 = -1;
      DAT_007f0fcc = 0;
      goto LAB_0041062a;
    }
    fVar7 = (float10)FUN_00417730(1);
    fStack_8 = (float)fVar7;
    fVar7 = (float10)FUN_00417730(0);
    if ((float10)_DAT_005cc9c8 <= (float10)fStack_8 - fVar7) {
      iVar6 = -1;
      DAT_007f0fcc = 0;
      goto LAB_0041062a;
    }
    goto LAB_004107e3;
  case 10:
    FUN_0046cbb0(0,&local_c,local_4);
    if (local_c != 0) {
      iVar6 = -1;
      DAT_007f0fcc = 0;
      goto LAB_0041062a;
    }
    fVar7 = (float10)FUN_00417730(0);
    if (fVar7 < (float10)_DAT_005cc574) {
      if (DAT_007f0fe4 < DAT_005d757c == (DAT_007f0fe4 == DAT_005d757c)) {
        return 0;
      }
      goto LAB_00410801;
    }
LAB_004107e3:
    iVar6 = 1;
    DAT_007f0fcc = 1;
    goto LAB_0041062a;
  }
  if (iVar6 == 0) {
    return 0;
  }
LAB_0041062a:
  FUN_0045bed0();
  FUN_0045bf30();
  FUN_00458f80(0);
  DAT_0063ba8c = 0xb;
  DAT_0063b90c = 1;
  _DAT_0063b910 = 0x442f0000;
  _DAT_0063b918 = 0;
  _DAT_0063b91c = 0;
  if (iVar6 == -1) {
    FUN_00448700(1,0);
    DAT_0063b914 = 1;
    DAT_007f0fcc = 0;
  }
  else {
    iVar2 = iVar6 + -1;
    FUN_00448700(1,iVar2);
    DAT_007f0fcc = (uint)(iVar2 == 0);
    DAT_0063b914 = iVar2;
  }
  iVar2 = FUN_00426c00();
  if (iVar2 == 0x26) {
    FUN_0044df80(0);
  }
  return iVar6;
}

