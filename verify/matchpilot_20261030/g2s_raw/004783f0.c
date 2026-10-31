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

extern float DAT_005d757c;
extern undefined1 DAT_005cd0d0;
extern undefined1 DAT_005cc33c;
extern undefined1 DAT_005cd0c8;
extern undefined1 DAT_005cc320;
extern undefined1 DAT_005cc98c;
extern undefined1 DAT_005ccac4;
int FUN_00409300();
int FUN_00409330(int p0, int p1);
uint FUN_004a3384();
float10 FUN_004c39b0(float * p0, float * p1);
float10 FUN_004c3b30();
extern int _DAT_005cc98c;
extern int _DAT_005ccac4;
// MATCH: 0x004783f0 FUN_004783f0
/* [C2 2026-06-02] One arg `param_1` (AI-polygon group data base pointer). No return. Body
   `0x004783f0..0x004785da` (490 bytes). Computes… */

void FUN_004783f0(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  float10 fVar5;
  int local_24;
  int local_20;
  float local_14;
  float local_10;
  undefined4 local_c;
  
  local_24 = 0;
  if (0 < *(int *)(param_1 + 0x16244)) {
    local_20 = 0;
    do {
      bVar1 = true;
      iVar2 = FUN_00409300();
      do {
        iVar3 = FUN_00409330(iVar2,local_24);
        iVar4 = FUN_00409300(local_24);
        if (iVar3 == iVar4) {
          bVar1 = false;
        }
        iVar4 = iVar2 * 0x4c + param_1;
        fVar5 = (float10)FUN_004c3b30();
        *(float *)(param_1 + (local_20 + iVar2) * 4) = (float)fVar5;
        local_14 = *(float *)(iVar4 + 0xc1c0);
        local_10 = *(float *)(iVar4 + 0xc1c8);
        local_c = 0;
        FUN_004c39b0(&local_14,&local_14);
        fVar5 = (float10)FUN_004a3384();
        fVar5 = fVar5 * (float10)_DAT_005cc98c;
        if (local_14 * DAT_005d757c - local_10 < DAT_005d757c) {
          fVar5 = -fVar5;
        }
        if (fVar5 < (float10)DAT_005d757c != (fVar5 == (float10)DAT_005d757c)) {
          do {
            fVar5 = fVar5 + (float10)_DAT_005ccac4;
          } while (fVar5 < (float10)DAT_005d757c != (fVar5 == (float10)DAT_005d757c));
        }
        *(float *)(param_1 + (local_20 + 0x1800 + iVar2) * 4) = (float)fVar5;
        iVar2 = iVar3;
      } while (bVar1);
      local_24 = local_24 + 1;
      local_20 = local_20 + 0x200;
    } while (local_24 < *(int *)(param_1 + 0x16244));
  }
  return;
}

