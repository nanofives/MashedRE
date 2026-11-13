// FLAGS: /O2 /GS
// TOOLCHAIN rw9044
typedef struct tagRECT { long left, top, right, bottom; } tagRECT, RECT, *LPRECT;
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

extern int DAT_00911ae4;
extern int * DAT_007d4578;
extern int * DAT_007d4110;
extern int * DAT_007d4574;
extern undefined4 DAT_007d412c;
extern HWND DAT_007d4104;
extern int DAT_00911f84;
extern int DAT_00911f88;
extern int DAT_00912100;
extern int DAT_009120e0;
extern int DAT_009120e4;
extern int * DAT_007d4118;
extern int *DAT_007d4114;
extern int DAT_007d456c;
extern undefined4 DAT_007d4570;
extern code *DAT_007d459c;
extern uint DAT_00912094;
extern int DAT_007d4124;
extern undefined1 DAT_005d8bd0;
__declspec(dllimport) BOOL __stdcall AdjustWindowRectEx(LPRECT p0, DWORD p1, BOOL p2, DWORD p3);
undefined4 FUN_004c7650();
int FUN_004c77c0();
undefined4 FUN_004c9ad0(void);
undefined4 FUN_004c9fb0();
undefined4 FUN_004d1e30(void);
void FUN_004d6200(void);
undefined4 FUN_004db3e0(void);
undefined4 FUN_004dc970(void);
undefined4 FUN_004e0920(void);
__declspec(dllimport) BOOL __stdcall GetClientRect(HWND p0, LPRECT p1);
__declspec(dllimport) HMENU __stdcall GetMenu(HWND p0);
__declspec(dllimport) LONG __stdcall GetWindowLongA(HWND p0, int p1);
__declspec(dllimport) BOOL __stdcall IsIconic(HWND p0);
__declspec(dllimport) LONG __stdcall SetWindowLongA(HWND p0, int p1, LONG p2);
__declspec(dllimport) BOOL __stdcall SetWindowPos(HWND p0, HWND p1, int p2, int p3, int p4, int p5, UINT p6);
extern int stack0xffffff84;
extern int stack0xffffff74;
// MATCH: 0x004c9290 FUN_004c9290


undefined4 FUN_004c9290(uint param_1,undefined1 *param_2,uint param_3)



{

  bool bVar1;

  int *piVar2;

  BOOL BVar3;

  int iVar4;

  int iVar5;

  uint uVar6;

  DWORD dwExStyle;

  HMENU pHVar7;

  DWORD dwStyle;

  int unaff_EBX;

  int *piVar8;

  int *piVar9;

  undefined1 auStack_68 [12];

  int iStack_5c;

  int iStack_58;

  int iStack_54;

  int iStack_50;

  int iStack_4c;

  int iStack_48;

  int iStack_44;

  undefined4 uStack_40;

  undefined4 uStack_3c;

  int iStack_38;

  int *piStack_34;

  uint uStack_30;

  int iStack_2c;

  undefined4 uStack_28;

  undefined4 uStack_24;

  int iStack_1c;

  int iStack_14;

  int *piStack_10;

  int *piStack_8;

  

  (*(uint *)((char *)&auStack_68 + 0)) = 0;

  (*(uint *)((char *)&auStack_68 + 4)) = param_3 & 1;

  if ((*(uint *)((char *)&auStack_68 + 4)) != 0) {

    (*(uint *)((char *)&auStack_68 + 0)) = CONCAT31(CONCAT21(CONCAT11(param_2[3],*param_2),param_2[1]),param_2[2]);

  }

  piVar9 = *(int **)(param_1 + 0x60);

  piVar2 = (int *)*piVar9;

  piVar8 = (int *)(DAT_00911ae4 + (int)piVar2);

  if (*(int *)(DAT_00911ae4 + 0x1c + (int)piVar2) != 0) {

    GetClientRect((HWND)piVar8[8],(LPRECT)(auStack_68 + 8));

    if (iStack_58 == 0) {

      return 0;

    }

    if (iStack_54 == 0) {

      return 0;

    }

    if ((iStack_58 == piVar2[10]) && (iStack_54 == piVar2[0xb])) {

      piVar2 = *(int **)(param_1 + 100);

      if (((piVar2 != (int *)0x0) && (piVar2 == (int *)*piVar2)) &&

         ((iStack_58 != piVar2[3] || (iStack_54 != piVar2[4])))) {

        *(undefined4 *)(param_1 + 100) = 0;

        FUN_004c7650();

        piVar2 = (int *)FUN_004c77c0();

        if (piVar2 != (int *)0x0) {

          *(int **)(param_1 + 100) = piVar2;

        }

      }

    }

    else {

      if (piVar9 == piVar2) {

        FUN_004c9fb0();

        piVar9 = *(int **)(param_1 + 0x60);

        piVar8 = (int *)(*piVar9 + DAT_00911ae4);

      }

      piVar2 = *(int **)(param_1 + 100);

    }

    if (DAT_007d4578 != (int *)*piVar8) {

      DAT_007d4578 = (int *)*piVar8;

      (**(code **)(*DAT_007d4110 + 0x94))();

    }

    if (piVar2 == (int *)0x0) {

      if (DAT_007d4574 != (int *)0x0) {

        DAT_007d4574 = (int *)0x0;

        (**(code **)(*DAT_007d4110 + 0x9c))();

      }

    }

    else if (DAT_007d4574 != *(int **)(DAT_00911ae4 + *piVar2)) {

      DAT_007d4574 = *(int **)(DAT_00911ae4 + *piVar2);

      (**(code **)(*DAT_007d4110 + 0x9c))();

    }

    iStack_50 = 0;

    iStack_4c = 0;

    iStack_48 = piVar9[3];

    iStack_44 = piVar9[4];

    uStack_40 = 0;

    uStack_3c = 0x3f800000;

    (**(code **)(*DAT_007d4110 + 0xbc))();

    uVar6 = *(uint *)(&DAT_005d8bd0 + param_1 * 4);

    goto LAB_004c9aa6;

  }

  GetClientRect(DAT_007d4104,(LPRECT)(auStack_68 + 8));

  if (iStack_58 == 0) {

    return 0;

  }

  if (iStack_54 == 0) {

    return 0;

  }

  BVar3 = IsIconic(DAT_007d4104);

  if (BVar3 != 0) {

    return 0;

  }

  if ((iStack_58 != DAT_00911f84) || (iStack_54 != DAT_00911f88)) {

    FUN_004c9ad0();

    iVar4 = (**(code **)(*DAT_007d4110 + 0xc))();

    if ((-1 < iVar4) && (DAT_00912100 != 0)) {

      DAT_009120e0 = iStack_5c;

      DAT_009120e4 = iStack_58;

    }

    iVar4 = (**(code **)(*DAT_007d4110 + 0x40))();

    if (-1 < iVar4) {

      (**(code **)(*DAT_007d4110 + 0x98))();

      (**(code **)(*DAT_007d4118 + 8))();

      (**(code **)(*DAT_007d4110 + 0xa0))(DAT_007d4110);

      (**(code **)(*DAT_007d4114 + 8))(DAT_007d4114);

      iVar4 = FUN_004d1e30();

      if ((iVar4 == 0) || (iVar4 = FUN_004dc970(), iVar4 == 0)) {

LAB_004c95c2:

        bVar1 = false;

      }

      else {

        FUN_004d6200();

        iVar4 = 0;

        if (0 < DAT_007d456c) {

          do {

            (**(code **)(*DAT_007d4110 + 0xcc))();

            iVar5 = (**(code **)(*DAT_007d4110 + 0xd4))(DAT_007d4110);

            iVar4 = iVar4 + 1;

          } while (iVar4 < DAT_007d456c);

          if (iVar5 < 0) goto LAB_004c95c2;

        }

        iVar4 = FUN_004db3e0();

        if ((iVar4 == 0) || (iVar4 = FUN_004e0920(), iVar4 == 0)) goto LAB_004c95c2;

        bVar1 = true;

      }

      if (DAT_007d459c != (code *)0x0) {

        (*DAT_007d459c)();

      }

      if (bVar1) {

        DAT_00911f84 = iStack_58;

        DAT_00911f88 = iStack_54;

        goto LAB_004c97a1;

      }

      FUN_004c9ad0();

    }

    if (DAT_00912100 == 0) {

      return 0;

    }

    DAT_009120e0 = DAT_00911f84;

    DAT_009120e4 = DAT_00911f88;

    iVar4 = (**(code **)(*DAT_007d4110 + 0x40))();

    if (iVar4 < 0) {

      return 0;

    }

    (*(uint *)((char *)&auStack_68 + 8)) = DAT_00911f84 + (*(uint *)((char *)&auStack_68 + 0));

    iStack_5c = DAT_00911f88 + (*(uint *)((char *)&auStack_68 + 4));

    uVar6 = GetWindowLongA(DAT_007d4104,-0x10);

    if ((uVar6 & 0x1000000) != 0) {

      uVar6 = GetWindowLongA(DAT_007d4104,-0x10);

      SetWindowLongA(DAT_007d4104,-0x10,uVar6 & 0xfeffffff);

    }

    dwExStyle = GetWindowLongA(DAT_007d4104,-0x14);

    pHVar7 = GetMenu(DAT_007d4104);

    uVar6 = (uint)(pHVar7 != (HMENU)0x0);

    dwStyle = GetWindowLongA(DAT_007d4104,-0x10);

    AdjustWindowRectEx((LPRECT)auStack_68,dwStyle,uVar6,dwExStyle);

    SetWindowPos(DAT_007d4104,(HWND)0x0,(*(uint *)((char *)&auStack_68 + 0)),iStack_5c,

                 (*(uint *)((char *)&auStack_68 + 8)) - (*(uint *)((char *)&auStack_68 + 0)),iStack_5c - (*(uint *)((char *)&auStack_68 + 4)),6);

    (**(code **)(*DAT_007d4110 + 0x98))();

    (**(code **)(*DAT_007d4118 + 8))(DAT_007d4118);

    (**(code **)(*DAT_007d4110 + 0xa0))(DAT_007d4110,&DAT_007d4114);

    (**(code **)(*DAT_007d4114 + 8))(DAT_007d4114);

    iVar4 = FUN_004d1e30();

    if ((iVar4 != 0) && (iVar4 = FUN_004dc970(), iVar4 != 0)) {

      FUN_004d6200();

      iVar4 = 0;

      if (0 < DAT_007d456c) {

        do {

          (**(code **)(*DAT_007d4110 + 0xcc))();

          iVar5 = (**(code **)(*DAT_007d4110 + 0xd4))(DAT_007d4110);

          iVar4 = iVar4 + 1;

        } while (iVar4 < DAT_007d456c);

        if (iVar5 < 0) goto LAB_004c9781;

      }

      iVar4 = FUN_004db3e0();

      if (iVar4 != 0) {

        FUN_004e0920();

      }

    }

LAB_004c9781:

    if (DAT_007d459c != (code *)0x0) {

      (*DAT_007d459c)();

    }

  }

LAB_004c97a1:

  if (*(char *)(piVar2 + 8) == '\x05') {

    if ((*(byte *)((int)piVar8 + 9) & 0xf) == 0) {

      piVar2 = (int *)0x4c97e8;

      (**(code **)(*(int *)*piVar8 + 0x48))();

    }

    else {

      piVar2 = (int *)*piVar8;

      (**(code **)(*piVar2 + 0x48))();

    }

    (**(code **)(*piStack_8 + 0x30))(piStack_8);

    if ((uStack_30 & 1) == 0) {

      iVar4 = -0x7fffbffb;

      goto LAB_004c98d9;

    }

    if (DAT_007d4578 != piStack_10) {

      DAT_007d4578 = piStack_10;

      (**(code **)(*DAT_007d4110 + 0x94))(DAT_007d4110,0,piStack_10);

    }

    if (*(int **)(iStack_14 + 100) == (int *)0x0) {

      if (DAT_007d4574 != (int *)0x0) {

        DAT_007d4574 = (int *)0x0;

        (**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110,0);

      }

    }

    else {

      iVar4 = **(int **)(iStack_14 + 100);

      piVar8 = *(int **)(DAT_00911ae4 + iVar4);

      if ((*(byte *)(iVar4 + 0x20) & 7) != 1) {

        (**(code **)(*piVar8 + 0x48))(piVar8,0,&stack0xffffff84);

        if (DAT_007d4574 != piVar2) {

          DAT_007d4574 = piVar2;

          (**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110,piVar2);

        }

        (**(code **)(*piVar2 + 8))(piVar2);

        iVar4 = 0;

        goto LAB_004c98d9;

      }

      if (DAT_007d4574 != piVar8) {

        DAT_007d4574 = piVar8;

        (**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110,piVar8);

        iVar4 = 0;

        goto LAB_004c98d9;

      }

    }

    iVar4 = 0;

LAB_004c98d9:

    iStack_50 = (int)(short)piVar9[7];

    iStack_4c = (int)*(short *)((int)piVar9 + 0x1e);

    iStack_48 = piVar9[3];

    iStack_44 = piVar9[4];

    uStack_40 = 0;

    uStack_3c = 0x3f800000;

    (**(code **)(*DAT_007d4110 + 0xbc))(DAT_007d4110,&iStack_50);

    uVar6 = *(uint *)(&DAT_005d8bd0 + iStack_14 * 4);

    if (((uVar6 & 6) != 0) && (*(int *)(iStack_1c + 100) == 0)) {

      uVar6 = uVar6 & 0xfffffff9;

    }

    (**(code **)(*DAT_007d4110 + 0xac))(DAT_007d4110,0,0,uVar6,piVar2,0x3f800000,DAT_007d412c);

    if (iVar4 < 0) {

      (**(code **)(*DAT_007d4110 + 0x88))

                (DAT_007d4110,DAT_007d4118,&stack0xffffff74,piStack_34,&stack0xffffff74,

                 (DAT_00912094 & 0x1000100) != 0);

    }

    (**(code **)(*piStack_34 + 8))(piStack_34);

    return 1;

  }

  if (DAT_007d4578 != DAT_007d4118) {

    DAT_007d4578 = DAT_007d4118;

    (**(code **)(*DAT_007d4110 + 0x94))();

  }

  if (DAT_007d4574 != DAT_007d4114) {

    DAT_007d4574 = DAT_007d4114;

    (**(code **)(*DAT_007d4110 + 0x9c))();

  }

  iStack_38 = (int)(short)piVar9[7];

  piStack_34 = (int *)(int)*(short *)((int)piVar9 + 0x1e);

  uStack_30 = piVar9[3];

  iStack_2c = piVar9[4];

  uStack_28 = 0;

  uStack_24 = 0x3f800000;

  (**(code **)(*DAT_007d4110 + 0xbc))();

  uVar6 = (uint)(unaff_EBX != 0);

  if ((param_1 & 2) == 0) {

    if ((param_1 & 4) == 0) goto LAB_004c9aa6;

  }

  else {

    uVar6 = uVar6 | 2;

  }

  if (DAT_007d4124 != 0) {

    uVar6 = uVar6 | 4;

  }

LAB_004c9aa6:

  (**(code **)(*DAT_007d4110 + 0xac))(DAT_007d4110,0,0,uVar6);

  return 1;

}



