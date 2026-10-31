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
extern undefined1 DAT_005cc320;
float10 FUN_004c3b30(float p0);
extern int _DAT_005cc320;
// MATCH: 0x0046c5f0 FUN_0046c5f0
/* [C1 2026-05-12] Computes the unit normal of a triangle given three vertices (EAX, param_1,
   param_2). */

void __fastcall __fastcall FUN_0046c5f0(float *param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *in_EAX;
  float *unaff_ESI;
  float10 fVar5;
  
  fVar2 = (in_EAX[2] - param_1[2]) * (in_EAX[1] - param_2[1]) -
          (in_EAX[1] - param_1[1]) * (in_EAX[2] - param_2[2]);
  fVar3 = (*in_EAX - *param_1) * (in_EAX[2] - param_2[2]) -
          (in_EAX[2] - param_1[2]) * (*in_EAX - *param_2);
  fVar1 = (in_EAX[1] - param_1[1]) * (*in_EAX - *param_2) -
          (*in_EAX - *param_1) * (in_EAX[1] - param_2[1]);
  fVar4 = fVar2 * fVar2 + fVar3 * fVar3 + fVar1 * fVar1;
  if ((DAT_005d757c <= fVar4) &&
     (fVar5 = (float10)FUN_004c3b30(fVar4), fVar5 != (float10)DAT_005d757c)) {
    fVar5 = (float10)_DAT_005cc320 / fVar5;
    *unaff_ESI = (float)((float10)fVar2 * fVar5);
    unaff_ESI[1] = (float)((float10)fVar3 * fVar5);
    unaff_ESI[2] = (float)(fVar5 * (float10)fVar1);
    return;
  }
  return;
}

