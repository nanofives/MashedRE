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

extern int DAT_0067e9fc;
extern int DAT_0067f17c;
extern undefined4 DAT_007f0a48;
extern int DAT_007f0a78;
extern int DAT_007f0aa8;
extern int DAT_007f0ad8;
extern int DAT_007f0b08;
extern int DAT_007f0b38;
extern int DAT_007f0b68;
extern int DAT_007f0b98;
extern int DAT_007f0bc8;
extern int DAT_007f0bf8;
extern int DAT_007f0c54;
extern undefined4 DAT_007f0c5c;
extern undefined4 DAT_007f0f2c;
extern undefined4 DAT_007f0a54;
extern int DAT_007f0a84;
extern int DAT_007f0ab4;
extern int DAT_007f0ae4;
extern int DAT_007f0b14;
extern int DAT_007f0b44;
extern int DAT_007f0b74;
extern int DAT_007f0ba4;
extern int DAT_007f0bd4;
extern int DAT_007f0c04;
extern int DAT_007f0c84;
extern undefined1 DAT_007f0c8c;
extern undefined4 DAT_007f0a40;
extern undefined4 DAT_007f0a44;
extern undefined1 DAT_007f0a50;
extern undefined1 DAT_007f0a58;
extern undefined1 DAT_007f0a5c;
extern undefined4 DAT_007f0a6c;
extern undefined4 DAT_007f0a70;
extern undefined4 DAT_007f0a74;
extern undefined1 DAT_007f0a9c;
int FID_conflict__wprintf(char * p0, ...);
void FUN_004099a0(void);
uint FUN_0040dd60(void);
void FUN_0042a920(undefined4 p0);
extern int _DAT_007f0c8c;
// MATCH: 0x00430290 FUN_00430290
void FUN_00430290(void)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_EDX;
  undefined4 uVar4;
  uint uVar5;
  
  if ((((DAT_0067e9fc != 3) && (DAT_0067e9fc != 4)) && (DAT_0067e9fc != 5)) ||
     (iVar2 = FUN_0040dd60(), iVar2 == 0)) {
    return;
  }
  if (DAT_0067e9fc == 3) {
    iVar2 = DAT_0067f17c * 0x30;
    if ((&DAT_007f0a44)[DAT_0067f17c * 0xc] != 2) {
      uVar4 = 0;
      iVar2 = 0;
      piVar3 = &DAT_007f0a74;
      do {
        if (piVar3[-0xc] == 2) {
          iVar2 = iVar2 + 1;
        }
        if (*piVar3 == 2) {
          iVar2 = iVar2 + 1;
        }
        if (piVar3[0xc] == 2) {
          iVar2 = iVar2 + 1;
        }
        if (piVar3[0x18] == 2) {
          iVar2 = iVar2 + 1;
        }
        if (piVar3[0x24] == 2) {
          iVar2 = iVar2 + 1;
        }
        if (piVar3[0x30] == 2) {
          iVar2 = iVar2 + 1;
        }
        piVar3 = piVar3 + 0x48;
      } while ((int)piVar3 < 0x7f0cb4);
      if (iVar2 == 2) {
        uVar4 = 0x264;
      }
      else if (iVar2 == 9) {
        uVar4 = 0x267;
      }
      FUN_0042a920(uVar4);
      iVar2 = extraout_ECX;
    }
    *(undefined4 *)((int)&DAT_007f0a44 + iVar2) = 2;
    if (*(int *)((int)&DAT_007f0a48 + iVar2) != 2) {
      *(undefined4 *)((int)&DAT_007f0a48 + iVar2) = 1;
    }
    *(undefined4 *)(&DAT_007f0a50 + iVar2) = 2;
    *(undefined4 *)((int)&DAT_007f0a40 + iVar2) = 1;
    *(undefined4 *)((int)&DAT_007f0a70 + iVar2) = 1;
    *(undefined4 *)((int)&DAT_007f0a6c + iVar2) = 1;
    *(undefined4 *)(&DAT_007f0a9c + iVar2) = 1;
    iVar2 = 0;
    piVar3 = &DAT_007f0a74;
    do {
      if (piVar3[-0xc] == 2) {
        iVar2 = iVar2 + 1;
      }
      if (*piVar3 == 2) {
        iVar2 = iVar2 + 1;
      }
      if (piVar3[0xc] == 2) {
        iVar2 = iVar2 + 1;
      }
      if (piVar3[0x18] == 2) {
        iVar2 = iVar2 + 1;
      }
      if (piVar3[0x24] == 2) {
        iVar2 = iVar2 + 1;
      }
      if (piVar3[0x30] == 2) {
        iVar2 = iVar2 + 1;
      }
      piVar3 = piVar3 + 0x48;
    } while ((int)piVar3 < 0x7f0cb4);
    if (iVar2 != 0) {
      piVar3 = &DAT_007f0a74;
      do {
        if (((int)piVar3 < 0x7f0c54) && (*piVar3 != 2)) {
          *piVar3 = 1;
          piVar3[2] = 2;
        }
        piVar3 = piVar3 + 0xc;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      FUN_004099a0();
      return;
    }
  }
  else {
    if (DAT_0067e9fc == 4) {
      iVar2 = DAT_0067f17c * 0x30;
      if ((&DAT_007f0a48)[DAT_0067f17c * 0xc] != 2) {
        FUN_0042a920(0x265);
        iVar2 = extraout_ECX_00;
      }
      *(undefined4 *)((int)&DAT_007f0a48 + iVar2) = 2;
      if (*(int *)((int)&DAT_007f0a54 + iVar2) != 2) {
        *(undefined4 *)((int)&DAT_007f0a54 + iVar2) = 1;
      }
      *(undefined4 *)(&DAT_007f0a58 + iVar2) = 2;
      uVar5 = (uint)(DAT_007f0a48 == 2);
      if (DAT_007f0a78 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0aa8 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0ad8 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0b08 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0b38 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0b68 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0b98 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0bc8 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0bf8 == 2) {
        uVar5 = uVar5 + 1;
      }
      FID_conflict__wprintf("silver add=%d\n",uVar5);
      FID_conflict__wprintf("TOTALNUMTRACKS-3=%d\n",10);
      if ((uVar5 == 10) && (FID_conflict__wprintf("HERE!!\n"), DAT_007f0c54 != 2)) {
        DAT_007f0c54 = 1;
        DAT_007f0c5c = 2;
        FUN_0042a920(0x268);
      }
    }
    if (DAT_0067e9fc == 5) {
      iVar2 = DAT_0067f17c * 0x30;
      if ((&DAT_007f0a54)[DAT_0067f17c * 0xc] != 2) {
        uVar4 = 0x266;
        (&DAT_007f0a54)[DAT_0067f17c * 0xc] = 2;
        bVar1 = true;
        piVar3 = &DAT_007f0a48;
        do {
          if (piVar3[-1] != 2) {
            bVar1 = false;
          }
          if (*piVar3 != 2) {
            bVar1 = false;
          }
          if (piVar3[3] != 2) {
            bVar1 = false;
          }
          if (piVar3[0xb] != 2) {
            bVar1 = false;
          }
          if (piVar3[0xc] != 2) {
            bVar1 = false;
          }
          if (piVar3[0xf] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x17] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x18] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x1b] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x23] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x24] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x27] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x2f] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x30] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x33] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x3b] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x3c] != 2) {
            bVar1 = false;
          }
          if (piVar3[0x3f] != 2) {
            bVar1 = false;
          }
          piVar3 = piVar3 + 0x48;
        } while ((int)piVar3 < 0x7f0c88);
        if (bVar1) {
          uVar4 = 0x26a;
          DAT_007f0f2c = 1;
          FUN_0042a920(0x26a);
        }
        FUN_0042a920(uVar4);
        iVar2 = extraout_EDX;
      }
      *(undefined4 *)((int)&DAT_007f0a54 + iVar2) = 2;
      *(undefined4 *)(&DAT_007f0a5c + iVar2) = 2;
      uVar5 = (uint)(DAT_007f0a54 == 2);
      if (DAT_007f0a84 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0ab4 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0ae4 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0b14 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0b44 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0b74 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0ba4 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0bd4 == 2) {
        uVar5 = uVar5 + 1;
      }
      if (DAT_007f0c04 == 2) {
        uVar5 = uVar5 + 1;
      }
      if ((uVar5 == 10) && (DAT_007f0c84 != 2)) {
        DAT_007f0c84 = 1;
        _DAT_007f0c8c = 2;
        FUN_0042a920(0x269);
      }
    }
  }
  FUN_004099a0();
  return;
}

