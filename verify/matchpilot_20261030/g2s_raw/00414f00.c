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

extern undefined1 DAT_005cc32c;
extern undefined1 DAT_005cc72c;
extern undefined1 DAT_005cc348;
extern undefined1 DAT_005cc564;
int FUN_00452160(void);
undefined4 FUN_00452ea0(int p0);
float10 FUN_00452eb0(void);
undefined4 FUN_0046c7b0(uint p0);
undefined4 FUN_0046d4a0(undefined4 * p0, uint p1);
undefined4 FUN_0046d570(float * p0, uint p1);
undefined4 FUN_0046d6d0(undefined4 * p0, uint p1);
float10 FUN_004c3ac0(float * p0);
extern int _DAT_005cc32c;
extern int _DAT_005cc72c;
extern int _DAT_005cc348;
extern int _DAT_005cc564;
// MATCH: 0x00414f00 FUN_00414f00
/* [C2 2026-06-02] Powerup-target seek. Body `0x00414f00..0x00415011` (0x111 = 273 bytes). */

undefined4 FUN_00414f00(undefined4 param_1,undefined4 param_2,float *param_3,undefined4 param_4)

{
  int iVar1;
  float *pfVar2;
  float10 fVar3;
  int local_18;
  undefined1 local_14 [4];
  undefined1 local_10 [4];
  float local_c;
  float local_8;
  float local_4;
  
  fVar3 = (float10)FUN_00452eb0();
  if ((float10)_DAT_005cc32c <= fVar3) {
    fVar3 = (float10)FUN_00452eb0();
    if (fVar3 <= (float10)_DAT_005cc72c) {
      iVar1 = FUN_00452ea0(param_4);
      if (iVar1 != 0) {
        pfVar2 = (float *)FUN_00452160();
        FUN_0046d6d0(local_14,param_4);
        FUN_0046d570(local_10,param_4);
        FUN_0046d4a0(&local_18,param_4);
        local_c = *pfVar2 - *(float *)(local_18 + 0x30);
        local_8 = pfVar2[1] - *(float *)(local_18 + 0x34);
        local_4 = pfVar2[2] - *(float *)(local_18 + 0x38);
        fVar3 = (float10)FUN_004c3ac0(&local_c);
        if ((float10)_DAT_005cc348 < fVar3) {
          *param_3 = *pfVar2;
          param_3[1] = pfVar2[2];
          return 1;
        }
        if ((float10)_DAT_005cc564 < fVar3) {
          iVar1 = FUN_0046c7b0(0);
          if (iVar1 != 0) {
            FUN_0046d4a0(&local_18,0);
            *param_3 = *(float *)(local_18 + 0x30);
            param_3[1] = *(float *)(local_18 + 0x38);
            return 1;
          }
        }
      }
      return 0;
    }
  }
  return 0;
}

