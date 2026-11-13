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

extern undefined4 DAT_0061864c;
extern int DAT_007d3ff8;
extern undefined4 DAT_006189e4;
extern int DAT_007d71cc;
extern undefined1 FUN_004e5190;
undefined4 FUN_004d7ff0(undefined4 p0);
int FUN_004d8000(int p0, undefined4 p1);
int FUN_004d8060(int p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined4 FUN_004f0c10(int * p0);
int * FUN_004f3b60(int * p0);
void FUN_004f3bc0(undefined4 * p0);
int RpWorldInstance(int p0);
void RwFrameDestroy(int * p0);
void RwFrameDestroyHierarchy(int * p0);
// MATCH: 0x004e5850 RpWorldCreate

undefined1 * RpWorldCreate(undefined4 *param_1)



{

  undefined1 *puVar1;

  undefined4 *puVar2;

  undefined4 uVar3;

  undefined1 *puVar4;

  undefined4 *puVar5;

  undefined4 *puVar6;

  int iVar7;

  int *piVar8;

  int *piVar9;

  undefined4 uStack_108;

  undefined4 uStack_104;

  int aiStack_100 [64];

  

  puVar4 = (undefined1 *)(**(code **)(DAT_007d3ff8 + 0x108))(DAT_0061864c,0x3000b);

  if (puVar4 == (undefined1 *)0x0) {

    uStack_108 = 2;

    uStack_104 = FUN_004d7ff0(0x80000013,DAT_0061864c);

    FUN_004d8480(&uStack_108);

    return (undefined1 *)0x0;

  }

  *puVar4 = 7;

  puVar4[1] = 0;

  puVar4[2] = 0;

  puVar4[3] = 0;

  *(undefined4 *)(puVar4 + 4) = 0;

  FUN_004f3bc0(puVar4 + 0x10);

  *(undefined4 *)(puVar4 + 8) = 0;

  *(undefined4 *)(puVar4 + 0xc) = 2;

  puVar5 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(DAT_006189e4,0x3000b);

  if (puVar5 == (undefined4 *)0x0) {

    uStack_108 = 2;

    uStack_104 = FUN_004d7ff0(0x80000013,4);

    FUN_004d8480(&uStack_108);

    (**(code **)(DAT_007d3ff8 + 0x10c))(puVar4);

    return (undefined1 *)0x0;

  }

  puVar6 = puVar5 + 0xe;

  puVar5[0xf] = puVar6;

  *puVar6 = puVar6;

  puVar6 = puVar5 + 0x10;

  puVar5[0x11] = puVar6;

  *puVar5 = 0xffffffff;

  *puVar6 = puVar6;

  puVar5[0xd] = 0;

  puVar5[0x1e] = 0;

  *(undefined2 *)((int)puVar5 + 0x82) = 0;

  *(undefined2 *)(puVar5 + 0x21) = 0;

  puVar5[2] = 0;

  puVar5[1] = 0;

  puVar5[3] = 0;

  puVar6 = puVar5 + 4;

  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {

    *puVar6 = 0;

    puVar6 = puVar6 + 1;

  }

  puVar5[0xc] = 0;

  puVar5[0x15] = param_1[3];

  puVar5[0x16] = param_1[4];

  puVar5[0x17] = param_1[5];

  puVar5[0x12] = *param_1;

  puVar5[0x13] = param_1[1];

  puVar5[0x14] = param_1[2];

  puVar5[0x1b] = param_1[3];

  puVar5[0x1c] = param_1[4];

  puVar5[0x1d] = param_1[5];

  puVar5[0x18] = *param_1;

  puVar5[0x19] = param_1[1];

  uVar3 = param_1[2];

  puVar5[0x1f] = 0;

  puVar5[0x1a] = uVar3;

  *(undefined4 **)(puVar4 + 0x1c) = puVar5;

  *(undefined4 *)(puVar4 + 0x20) = 0;

  *(undefined4 *)(puVar4 + 0x4c) = 0;

  *(undefined4 *)(puVar4 + 0x48) = 0;

  *(undefined4 *)(puVar4 + 0x44) = 0;

  *(undefined4 *)(puVar4 + 0x5c) = param_1[3];

  *(undefined4 *)(puVar4 + 0x60) = param_1[4];

  *(undefined4 *)(puVar4 + 100) = param_1[5];

  *(undefined4 *)(puVar4 + 0x50) = *param_1;

  *(undefined4 *)(puVar4 + 0x54) = param_1[1];

  uVar3 = param_1[2];

  *(undefined4 *)(puVar4 + 0x24) = 0;

  *(undefined4 *)(puVar4 + 0x58) = uVar3;

  puVar1 = puVar4 + 0x2c;

  *(undefined1 **)(puVar4 + 0x30) = puVar1;

  *(undefined1 **)(puVar4 + 0x28) = puVar1;

  *(undefined1 **)puVar1 = puVar1;

  puVar1 = puVar4 + 0x34;

  *(undefined1 **)(puVar4 + 0x38) = puVar1;

  *(undefined1 **)puVar1 = puVar1;

  puVar1 = puVar4 + 0x3c;

  *(undefined1 **)puVar1 = puVar1;

  *(undefined1 **)(puVar4 + 0x40) = puVar1;

  *(code **)(puVar4 + 0x68) = FUN_004e5190;

  *(undefined4 *)(puVar4 + 0x6c) = 0;

  uVar3 = DAT_0061864c;

  puVar6 = (undefined4 *)

           (**(code **)(DAT_007d3ff8 + 0x118))(*(undefined4 *)(DAT_007d71cc + DAT_007d3ff8),0x40507)

  ;

  if (puVar6 != (undefined4 *)0x0) {

    *puVar6 = puVar4;

    puVar6[1] = uVar3;

    puVar2 = puVar6 + 2;

    *puVar2 = *(undefined4 *)(DAT_007d71cc + 4 + DAT_007d3ff8);

    puVar6[3] = DAT_007d71cc + 4 + DAT_007d3ff8;

    *(undefined4 **)(*(int *)(DAT_007d71cc + 4 + DAT_007d3ff8) + 4) = puVar2;

    *(undefined4 **)(DAT_007d71cc + 4 + DAT_007d3ff8) = puVar2;

  }

  FUN_004d8000(&DAT_0061864c,puVar4);

  FUN_004d8000(&DAT_006189e4,puVar5);

  iVar7 = RpWorldInstance(puVar4);

  if (iVar7 != 0) {

    return puVar4;

  }

  piVar8 = *(int **)(DAT_007d71cc + 4 + DAT_007d3ff8);

  do {

    if (piVar8 == (int *)(DAT_007d71cc + 4 + DAT_007d3ff8)) {

LAB_004e5b26:

      iVar7 = 0;

      piVar8 = *(int **)(puVar4 + 0x1c);

      if (*(int **)(puVar4 + 0x1c) != (int *)0x0) {

        do {

          if (*piVar8 < 0) {

            if (piVar8[0x1e] != 0) {

              FUN_004f0c10(piVar8[0x1e]);

              piVar8[0x1e] = 0;

            }

            piVar9 = (int *)aiStack_100[iVar7];

            iVar7 = iVar7 + -1;

          }

          else {

            piVar9 = (int *)piVar8[2];

            iVar7 = iVar7 + 1;

            aiStack_100[iVar7] = piVar8[3];

          }

          piVar8 = piVar9;

        } while (-1 < iVar7);

      }

      FUN_004f3b60(puVar4 + 0x10);

      iVar7 = *(int *)(puVar4 + 0x1c);

      if ((puVar4[3] & 1) != 0) {

        if (iVar7 != 0) {

          RwFrameDestroy(iVar7);

        }

        FUN_004d8060(&DAT_0061864c,puVar4);

        (**(code **)(DAT_007d3ff8 + 0x10c))(puVar4);

        return (undefined1 *)0x0;

      }

      if (iVar7 != 0) {

        RwFrameDestroyHierarchy(iVar7);

      }

      FUN_004d8060(&DAT_0061864c,puVar4);

      (**(code **)(DAT_007d3ff8 + 0x10c))(puVar4);

      return (undefined1 *)0x0;

    }

    if ((undefined1 *)piVar8[-2] == puVar4) {

      *(int *)piVar8[1] = *piVar8;

      *(int *)(*piVar8 + 4) = piVar8[1];

      (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d71cc + DAT_007d3ff8),piVar8 + -2);

      goto LAB_004e5b26;

    }

    piVar8 = (int *)*piVar8;

  } while( true );

}



