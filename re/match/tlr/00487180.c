// FLAGS: /O2 /GS
/* TL-MATCH-REST 0x00487180 (no hooks.csv row before this slice): raw g2s.py output (decomp_pc.py --port --callees, Mashed_pool, 2026-11-10), body unchanged. Starting point only, not matched; score in verify/tlmatchrest_20261110/scores.tsv. */
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

extern undefined1 DAT_005ce18c;
extern undefined1 DAT_005cf47c;
undefined4 FUN_004e55d0(int * p0);
extern int _DAT_005ce18c;
extern int _DAT_005cf47c;
// MATCH: 0x00487180 FUN_00487180
undefined4 FUN_00487180(float *param_1,int param_2,float *param_3,float param_4,int *param_5)



{

  float fVar1;

  float fVar2;

  float fVar3;

  float fVar4;

  int iVar5;

  float *pfVar6;

  float *pfVar7;

  

  fVar1 = param_1[4];

  fVar2 = param_1[1];

  fVar3 = param_1[5];

  fVar4 = param_1[2];

  param_5[5] = (int)((param_1[3] - *param_1) * param_4 + *param_1);

  param_5[6] = (int)((fVar1 - fVar2) * param_4 + param_1[1]);

  param_5[7] = (int)((fVar3 - fVar4) * param_4 + param_1[2]);

  param_5[2] = (int)*param_3;

  param_5[3] = (int)param_3[1];

  param_5[4] = (int)param_3[2];

  fVar2 = *param_3 * _DAT_005ce18c;

  fVar3 = param_3[1] * _DAT_005ce18c;

  fVar1 = param_3[2];

  param_5[8] = 1;

  fVar1 = fVar1 * _DAT_005ce18c;

  param_5[5] = (int)(fVar2 + (float)param_5[5]);

  param_5[6] = (int)(fVar3 + (float)param_5[6]);

  param_5[7] = (int)(fVar1 + (float)param_5[7]);

  param_5[0x13] = (int)(_DAT_005cf47c * param_4);

  pfVar6 = param_3;

  pfVar7 = (float *)(param_5 + 9);

  for (iVar5 = 10; iVar5 != 0; iVar5 = iVar5 + -1) {

    *pfVar7 = *pfVar6;

    pfVar6 = pfVar6 + 1;

    pfVar7 = pfVar7 + 1;

  }

  *param_5 = param_2;

  iVar5 = FUN_004e55d0(param_2);

  param_5[1] = *(int *)(*(int *)(iVar5 + 0x10) +

                       ((uint)*(ushort *)(*(int *)(param_2 + 4) + 6 + (int)param_3[6] * 8) +

                       (uint)*(ushort *)(param_2 + 0x80)) * 4);

  return 0;

}



