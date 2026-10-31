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

extern undefined1 DAT_005cc320;
extern undefined1 DAT_005cc32c;
extern undefined1 DAT_005cd0b4;
extern undefined4 DAT_005f9730;
float10 FUN_004728f0(undefined4 p0);
void FUN_0047cdc0(int p0, float p1);
int FUN_0047fad0();
undefined4 FUN_004b3f90(int p0);
extern int _DAT_005cc320;
extern int _DAT_005cc32c;
extern int _DAT_005cd0b4;
// MATCH: 0x0044c4f0 FUN_0044c4f0
/* [C2 2026-06-03] Initializer for one `0x58`-byte slot in the 3-slot array at `&DAT_00896120`.
   Seeds the slot from a 6-float template… */

void FUN_0044c4f0(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 int param_5)

{
  float *pfVar1;
  float fVar2;
  undefined4 uVar3;
  int iVar4;
  float *pfVar5;
  float *pfVar6;
  float10 fVar7;
  
  *param_1 = param_2;
  param_1[1] = param_3;
  param_1[2] = param_4;
  param_1[3] = 0;
  fVar7 = (float10)FUN_004728f0(param_2);
  fVar7 = (float10)_DAT_005cc320 / fVar7;
  pfVar1 = (float *)(param_1 + 8);
  pfVar5 = (float *)&DAT_005f9730;
  pfVar6 = pfVar1;
  for (iVar4 = 6; iVar4 != 0; iVar4 = iVar4 + -1) {
    *pfVar6 = *pfVar5;
    pfVar5 = pfVar5 + 1;
    pfVar6 = pfVar6 + 1;
  }
  param_1[0xe] = 9;
  param_1[0x13] = 0;
  param_1[0x15] = 0;
  param_1[4] = (float)(fVar7 * (float10)(float)param_1[10]);
  param_1[5] = (float)(-(float10)(float)param_1[0xd] * fVar7);
  param_1[6] = (float)(fVar7 * (float10)*pfVar1);
  param_1[7] = (float)(-(float10)(float)param_1[0xb] * fVar7);
  fVar2 = ((float)param_1[9] - (float)param_1[0xc]) * _DAT_005cc32c;
  param_1[0x14] = fVar2;
  param_1[0x10] = (*pfVar1 - (float)param_1[0xb]) * _DAT_005cc32c * _DAT_005cd0b4;
  param_1[0x11] = fVar2 * _DAT_005cd0b4;
  param_1[0x12] = ((float)param_1[10] - (float)param_1[0xd]) * _DAT_005cc32c * _DAT_005cd0b4;
  uVar3 = FUN_004b3f90(param_3,param_1 + 0x10,0,0x3dcccccd,0x3f7ff972,0x42a00000,param_5 + 0x75ab);
  uVar3 = FUN_0047fad0(uVar3);
  param_1[0xf] = uVar3;
  FUN_0047cdc0(uVar3,0x42c80000);
  return;
}

