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

extern int DAT_0086ecc0;
extern undefined4 DAT_006c6eb8;
undefined4 * FUN_0055af40();
undefined4 FUN_0055dc70(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_0055dec0(undefined4 * p0);
undefined4 FUN_00562520(int p0, int p1);
void FUN_00562580(undefined4 * p0);
void FUN_005625e0(int p0, int p1);
void FUN_0057c4b0(int p0, int p1, int p2);
// MATCH: 0x0047f4c0 FUN_0047f4c0
undefined4 FUN_0047f4c0(undefined4 param_1)

{
  uint *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  undefined4 *puVar7;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  undefined1 local_7c [124];
  
  local_90 = 0;
  local_80 = 0;
  local_9c = 0x447a0000;
  local_98 = 0x447a0000;
  local_94 = 0x447a0000;
  local_8c = 0xc47a0000;
  local_88 = 0xc47a0000;
  local_84 = 0xc47a0000;
  FUN_00562580(local_7c);
  FUN_0057c4b0(local_7c,200,100);
  FUN_005625e0(local_7c,0x20);
  uVar2 = FUN_0055dc70(local_7c,param_1,&local_9c);
  DAT_0086ecc0 = FUN_00562520(10,10);
  uVar4 = *(int *)(DAT_0086ecc0 + 4) + 1;
  puVar1 = (uint *)(DAT_0086ecc0 + 0xc + (uVar4 >> 5) * 4);
  *puVar1 = *puVar1 | 1 << ((byte)uVar4 & 0x1f);
  uVar4 = *(int *)(DAT_0086ecc0 + 4) * 2 + 2;
  uVar6 = uVar4 >> 5;
  *(uint *)(DAT_0086ecc0 + 0xc + uVar6 * 4) =
       *(uint *)(DAT_0086ecc0 + 0xc + uVar6 * 4) | 1 << ((byte)uVar4 & 0x1f);
  uVar4 = (*(int *)(DAT_0086ecc0 + 4) + 1) * 3;
  puVar1 = (uint *)(DAT_0086ecc0 + 0xc + (uVar4 >> 5) * 4);
  *puVar1 = *puVar1 | 1 << ((byte)uVar4 & 0x1f);
  uVar4 = *(int *)(DAT_0086ecc0 + 4) * 4 + 4;
  puVar1 = (uint *)(DAT_0086ecc0 + 0xc + (uVar4 >> 5) * 4);
  *puVar1 = *puVar1 | 1 << ((byte)uVar4 & 0x1f);
  uVar4 = (*(int *)(DAT_0086ecc0 + 4) + 1) * 5;
  uVar6 = uVar4 >> 5;
  *(uint *)(DAT_0086ecc0 + 0xc + uVar6 * 4) =
       *(uint *)(DAT_0086ecc0 + 0xc + uVar6 * 4) | 1 << ((byte)uVar4 & 0x1f);
  uVar6 = (*(int *)(DAT_0086ecc0 + 4) + 1) * 6;
  uVar4 = uVar6 >> 5;
  *(uint *)(DAT_0086ecc0 + 0xc + uVar4 * 4) =
       *(uint *)(DAT_0086ecc0 + 0xc + uVar4 * 4) | 1 << ((byte)uVar6 & 0x1f);
  uVar4 = (*(int *)(DAT_0086ecc0 + 4) + 1) * 7;
  puVar1 = (uint *)(DAT_0086ecc0 + 0xc + (uVar4 >> 5) * 4);
  *puVar1 = *puVar1 | 1 << ((byte)uVar4 & 0x1f);
  uVar4 = *(int *)(DAT_0086ecc0 + 4) * 7 + 1;
  puVar1 = (uint *)(DAT_0086ecc0 + 0xc + (uVar4 >> 5) * 4);
  *puVar1 = *puVar1 | 1 << ((byte)uVar4 & 0x1f);
  uVar4 = *(int *)(DAT_0086ecc0 + 4) * 8 + 8;
  uVar6 = uVar4 >> 5;
  *(uint *)(DAT_0086ecc0 + 0xc + uVar6 * 4) =
       *(uint *)(DAT_0086ecc0 + 0xc + uVar6 * 4) | 1 << ((byte)uVar4 & 0x1f);
  uVar3 = FUN_0055dec0(uVar2,DAT_0086ecc0);
  FUN_0055af40(uVar3);
  puVar7 = &DAT_006c6eb8;
  for (iVar5 = 200; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar7 = 1;
    puVar7 = puVar7 + 1;
  }
  return uVar2;
}

