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

extern undefined1 DAT_0068bb5c;
extern undefined1 DAT_005cc328;
extern undefined1 DAT_005cc320;
extern undefined1 DAT_005cc574;
int FUN_004e89e0(int p0, ushort p1);
int FUN_004e8a10(int p0);
extern int _DAT_0068bb5c;
extern int _DAT_005cc328;
extern int _DAT_005cc320;
extern int _DAT_005cc574;
// MATCH: 0x00459400 FUN_00459400
/* [C2 2026-06-03] `int FUN_00459400(int param_1)` — body 0x00459400..0x0045947f. **Pool J
   per-frame trail-mesh vertex animator** (cosine… */

int FUN_00459400(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  float10 fVar5;
  int iVar6;
  int iVar7;
  float10 fVar8;
  float10 fVar9;
  
  _DAT_0068bb5c = _DAT_0068bb5c + _DAT_005cc328;
  iVar2 = *(int *)(param_1 + 0x18);
  iVar3 = *(int *)(iVar2 + 0x14);
  FUN_004e89e0(iVar2,0x10);
  iVar4 = *(int *)(iVar2 + 0x34);
  iVar7 = 0;
  if (0 < iVar3) {
    fVar8 = (float10)fcos((float10)_DAT_0068bb5c);
    fVar5 = (float10)_DAT_005cc328;
    do {
      fVar9 = fVar8 * fVar5 + (float10)*(float *)(iVar4 + iVar7 * 8);
      *(float *)(iVar4 + iVar7 * 8) = (float)fVar9;
      if ((float10)_DAT_005cc320 < fVar9) {
        iVar6 = 0;
        do {
          iVar1 = iVar6 * 8;
          iVar6 = iVar6 + 1;
          *(float *)(iVar4 + -8 + iVar6 * 8) = *(float *)(iVar4 + iVar1) - _DAT_005cc574;
        } while (iVar6 < iVar3);
      }
      iVar7 = iVar7 + 1;
    } while (iVar7 < iVar3);
  }
  FUN_004e8a10(iVar2);
  return param_1;
}

