// FLAGS: /O2 /GS
/* TL-MATCH-REST 0x004b4eb0 (no hooks.csv row before this slice): raw g2s.py output (decomp_pc.py --port --callees, Mashed_pool, 2026-11-10), body unchanged. Starting point only, not matched; score in verify/tlmatchrest_20261110/scores.tsv. */
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

undefined4 FUN_004e55d0(int * p0);
// MATCH: 0x004b4eb0 FUN_004b4eb0


int FUN_004b4eb0(int param_1,undefined4 *param_2)



{

  ushort *puVar1;

  int *piVar2;

  int iVar3;

  int *piVar4;

  int iVar5;

  int iVar6;

  int iVar7;

  int iVar8;

  undefined4 *puVar9;

  

  piVar2 = (int *)*param_2;

  iVar3 = param_2[3];

  piVar4 = (int *)param_2[2];

  iVar5 = param_2[1];

  iVar6 = *piVar4;

  iVar7 = FUN_004e55d0(param_1);

  iVar8 = 0;

  if (*(short *)(param_1 + 0x84) != 0) {

    puVar9 = (undefined4 *)(iVar5 + iVar6 * 0x30);

    do {

      puVar1 = (ushort *)(*(int *)(param_1 + 4) + iVar8 * 8);

      if (*(int *)(*(int *)(*(int *)(iVar7 + 0x10) +

                           ((uint)*(ushort *)(*(int *)(param_1 + 4) + 6 + iVar8 * 8) +

                           (uint)*(ushort *)(param_1 + 0x80)) * 4) + 4) == *piVar2) {

        if (iVar5 != 0) {

          *puVar9 = *(undefined4 *)(*(int *)(param_1 + 8) + (uint)*puVar1 * 0xc);

          puVar9[1] = *(undefined4 *)(*(int *)(param_1 + 8) + 4 + (uint)*puVar1 * 0xc);

          puVar9[2] = *(undefined4 *)(*(int *)(param_1 + 8) + 8 + (uint)*puVar1 * 0xc);

          puVar9[3] = *(undefined4 *)(*(int *)(param_1 + 8) + (uint)puVar1[1] * 0xc);

          puVar9[4] = *(undefined4 *)(*(int *)(param_1 + 8) + 4 + (uint)puVar1[1] * 0xc);

          puVar9[5] = *(undefined4 *)(*(int *)(param_1 + 8) + 8 + (uint)puVar1[1] * 0xc);

          puVar9[6] = *(undefined4 *)(*(int *)(param_1 + 8) + (uint)puVar1[2] * 0xc);

          puVar9[7] = *(undefined4 *)(*(int *)(param_1 + 8) + 4 + (uint)puVar1[2] * 0xc);

          puVar9[8] = *(undefined4 *)(*(int *)(param_1 + 8) + 8 + (uint)puVar1[2] * 0xc);

          puVar9[9] = param_1;

          puVar9[10] = *(undefined4 *)puVar1;

          puVar9[0xb] = *(undefined4 *)(puVar1 + 2);

          puVar9 = puVar9 + 0xc;

        }

        iVar6 = *piVar4;

        *piVar4 = iVar6 + 1;

        if (iVar6 + 1 == iVar3) {

          return 0;

        }

      }

      iVar8 = iVar8 + 1;

    } while (iVar8 < (int)(uint)*(ushort *)(param_1 + 0x84));

  }

  return param_1;

}



