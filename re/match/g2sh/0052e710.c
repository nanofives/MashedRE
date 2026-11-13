// FLAGS: /O2 /GS
// TOOLCHAIN rw9044
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

extern int DAT_007dc508;
undefined4 * FUN_004cbe80(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc6e0(undefined4 p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc770(undefined4 p0, undefined4 p1, undefined4 p2);
uint FUN_0052fe90(uint p0, int p1);
undefined4 FUN_005300d0(undefined4 p0, int * p1);
// MATCH: 0x0052e710 FUN_0052e710

undefined4 FUN_0052e710(undefined4 param_1,undefined4 param_2,uint param_3)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  
  puVar1 = *(uint **)(DAT_007dc508 + param_3);
  if (puVar1 != (uint *)0x0) {
    if ((*(uint *)(param_3 + 8) & 0x1000000) == 0) {
      iVar3 = *(int *)(param_3 + 0x14);
      param_3 = (uint)CONCAT11((char)puVar1[4],(char)puVar1[1]) << 8 | *puVar1 & 0xff;
      iVar2 = FUN_004cc770(param_1,&param_3,4);
      if (iVar2 == 0) {
        return 0;
      }
      iVar2 = FUN_004cbe80(param_1,puVar1[2],puVar1[1]);
      if (iVar2 == 0) {
        return 0;
      }
      iVar2 = FUN_004cc770(param_1,puVar1[5],iVar3 * 4);
      if (iVar2 == 0) {
        return 0;
      }
      iVar3 = FUN_004cc6e0(param_1,puVar1[6],iVar3 << 4);
      if (iVar3 == 0) {
        return 0;
      }
      iVar3 = FUN_004cc6e0(param_1,puVar1[3],*puVar1 << 6);
      if (iVar3 == 0) {
        return 0;
      }
      iVar3 = FUN_005300d0(param_1,puVar1);
      if (iVar3 == 0) {
        return 0;
      }
    }
    else {
      iVar3 = FUN_0052fe90(param_1,param_3);
      if (iVar3 == 0) {
        return 0;
      }
    }
  }
  return param_1;
}

