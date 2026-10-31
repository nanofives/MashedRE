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

extern int DAT_0068f4c8;
extern int DAT_008aa254;
extern int DAT_0088e670;
extern undefined1 DAT_005ce6b8;
extern undefined1 DAT_005ccd6c;
extern undefined1 DAT_005ccd0c;
extern undefined4 DAT_0068f640;
extern undefined4 DAT_0068f648;
void FUN_0045e0f0(int p0, float p1);
float10 __fastcall FUN_00472650();
int FUN_004b44f0(int p0, int p1);
void FUN_005a66d0(int p0, int p1);
uint FUN_005a6d60(undefined4 * p0, int p1, undefined4 p2, undefined4 p3);
void FUN_005a6dc0(int p0, undefined4 p1, undefined4 p2);
extern int _DAT_005ce6b8;
extern int _DAT_005ccd6c;
extern int _DAT_005ccd0c;
// MATCH: 0x0045e990 FUN_0045e990
/* [C2 2026-06-01] No parameters. Body 0x0045e990..0x0045efd9. Audio-dispatcher idiom (see
   0045e460.md). 8-channel per-frame… */

void FUN_0045e990(void)

{
  float fVar1;
  int iVar2;
  int iVar3;
  float10 fVar4;
  float local_c [3];
  
  if ((DAT_0068f4c8 == 0) || (DAT_008aa254 < 2)) {
    iVar2 = FUN_004b44f0(0,0x32);
    if (iVar2 == 0x19) {
      iVar2 = FUN_004b44f0(0,3);
      iVar2 = *(int *)(iVar2 * 0x90 + 0x470 + DAT_0088e670);
      if ((*(byte *)((&DAT_0068f640)[iVar2 * 2] + 0x34) & 2) == 0) {
        FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
      }
      fVar4 = (float10)FUN_00472650(0,0x461c4000);
      *(float *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = (float)(fVar4 + (float10)_DAT_005ce6b8);
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        fVar4 = (float10)FUN_00472650(0,0x461c4000);
        FUN_005a6dc0(*(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,
                     (float)(fVar4 + (float10)_DAT_005ce6b8));
      }
      FUN_0045e0f0(iVar2,0x3f800000);
      fVar4 = (float10)FUN_00472650(0,0x42200000);
      fVar1 = (float)(fVar4 - (float10)_DAT_005ccd6c);
      fVar4 = (float10)FUN_00472650(0,0x430c0000);
      iVar3 = (&DAT_0068f640)[iVar2 * 2];
      local_c[2] = (float)(fVar4 - (float10)_DAT_005ccd0c);
      local_c[1] = -1.0;
      *(float *)(iVar3 + 0x58) = fVar1;
      *(undefined4 *)(iVar3 + 0x5c) = 0xbf800000;
      *(float *)(iVar3 + 0x60) = local_c[2];
      local_c[0] = fVar1;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,local_c);
      }
    }
    iVar2 = FUN_004b44f0(0,0x19);
    if (iVar2 == 0x19) {
      iVar2 = FUN_004b44f0(0,2);
      iVar2 = *(int *)(iVar2 * 0x90 + 0x6b0 + DAT_0088e670);
      if ((*(byte *)((&DAT_0068f640)[iVar2 * 2] + 0x34) & 2) == 0) {
        FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
      }
      *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x46ac4400;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,0x46ac4400);
      }
      FUN_0045e0f0(iVar2,0x3f800000);
      fVar4 = (float10)FUN_00472650(0,0x42200000);
      iVar3 = (&DAT_0068f640)[iVar2 * 2];
      local_c[1] = 2.0;
      local_c[2] = -70.0;
      local_c[0] = (float)(fVar4 - (float10)_DAT_005ccd6c);
      *(float *)(iVar3 + 0x58) = local_c[0];
      *(undefined4 *)(iVar3 + 0x5c) = 0x40000000;
      *(undefined4 *)(iVar3 + 0x60) = 0xc28c0000;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,local_c);
      }
    }
    iVar2 = *(int *)(DAT_0088e670 + 0x860);
    if ((*(byte *)((&DAT_0068f640)[iVar2 * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
    }
    *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x463c4800;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,0x463c4800);
    }
    FUN_0045e0f0(iVar2,0x3f800000);
    iVar3 = (&DAT_0068f640)[iVar2 * 2];
    local_c[0] = -13.5;
    local_c[1] = 0.6;
    local_c[2] = -3.22;
    *(undefined4 *)(iVar3 + 0x58) = 0xc1580000;
    *(undefined4 *)(iVar3 + 0x5c) = 0x3f19999a;
    *(undefined4 *)(iVar3 + 0x60) = 0xc04e147b;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,local_c);
    }
    iVar2 = *(int *)(DAT_0088e670 + 0x8f0);
    if ((*(byte *)((&DAT_0068f640)[iVar2 * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
    }
    *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,0x46ac4400);
    }
    FUN_0045e0f0(iVar2,0x3f800000);
    iVar3 = (&DAT_0068f640)[iVar2 * 2];
    local_c[0] = 8.9;
    local_c[1] = 1.3;
    local_c[2] = 69.6;
    *(undefined4 *)(iVar3 + 0x58) = 0x410e6666;
    *(undefined4 *)(iVar3 + 0x5c) = 0x3fa66666;
    *(undefined4 *)(iVar3 + 0x60) = 0x428b3333;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,local_c);
    }
    iVar2 = *(int *)(DAT_0088e670 + 0x980);
    if ((*(byte *)((&DAT_0068f640)[iVar2 * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
    }
    *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,0x46ac4400);
    }
    FUN_0045e0f0(iVar2,0x3f800000);
    iVar3 = (&DAT_0068f640)[iVar2 * 2];
    local_c[0] = 0.0;
    local_c[1] = 2.0;
    local_c[2] = -74.0;
    *(undefined4 *)(iVar3 + 0x58) = 0;
    *(undefined4 *)(iVar3 + 0x5c) = 0x40000000;
    *(undefined4 *)(iVar3 + 0x60) = 0xc2940000;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,local_c);
    }
    iVar3 = iVar2 + 1;
    if ((*(byte *)((&DAT_0068f648)[iVar2 * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f648)[iVar2 * 2],1);
    }
    *(undefined4 *)((&DAT_0068f640)[iVar3 * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[iVar3 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar3 * 2] + 0x30),4,1,0x46ac4400);
    }
    FUN_0045e0f0(iVar3,0x3f800000);
    iVar2 = (&DAT_0068f640)[iVar3 * 2];
    local_c[0] = 8.9;
    local_c[1] = 1.3;
    local_c[2] = 69.6;
    *(undefined4 *)(iVar2 + 0x58) = 0x410e6666;
    *(undefined4 *)(iVar2 + 0x5c) = 0x3fa66666;
    *(undefined4 *)(iVar2 + 0x60) = 0x428b3333;
    if (*(int *)((&DAT_0068f640)[iVar3 * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar3 * 2] + 0x30),0,1,local_c);
    }
    iVar2 = *(int *)(DAT_0088e670 + 0xa10);
    if ((*(byte *)((&DAT_0068f640)[iVar2 * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
    }
    *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,0x46ac4400);
    }
    FUN_0045e0f0(iVar2,0x3f800000);
    iVar3 = (&DAT_0068f640)[iVar2 * 2];
    local_c[0] = -3.86;
    local_c[1] = 6.716;
    local_c[2] = 24.4;
    *(undefined4 *)(iVar3 + 0x58) = 0xc0770a3d;
    *(undefined4 *)(iVar3 + 0x5c) = 0x40d6e979;
    *(undefined4 *)(iVar3 + 0x60) = 0x41c33333;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,local_c);
    }
    iVar2 = *(int *)(DAT_0088e670 + 0xaa0);
    if ((*(byte *)((&DAT_0068f640)[iVar2 * 2] + 0x34) & 2) == 0) {
      FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
    }
    *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,0x46ac4400);
    }
    FUN_0045e0f0(iVar2,0x3f800000);
    iVar3 = (&DAT_0068f640)[iVar2 * 2];
    local_c[0] = 4.7;
    local_c[1] = 12.6;
    local_c[2] = -71.5;
    *(undefined4 *)(iVar3 + 0x58) = 0x40966666;
    *(undefined4 *)(iVar3 + 0x5c) = 0x4149999a;
    *(undefined4 *)(iVar3 + 0x60) = 0xc28f0000;
    if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,local_c);
    }
  }
  return;
}

