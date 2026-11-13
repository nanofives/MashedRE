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

extern int DAT_00911d60;
extern undefined1 DAT_00911d20;
extern undefined1 DAT_00911d30;
extern undefined1 DAT_00911d40;
extern undefined1 DAT_00911d50;
extern undefined1 DAT_00911d2c;
extern undefined1 DAT_00911d3c;
extern undefined1 DAT_00911d4c;
extern undefined1 DAT_00911d5c;
extern undefined1 DAT_006181e0;
extern undefined1 DAT_006181f4;
extern undefined1 DAT_00618200;
extern undefined1 DAT_00618204;
extern undefined1 DAT_00618210;
extern undefined1 DAT_00618214;
extern undefined1 DAT_005cc320;
extern undefined1 DAT_0061820c;
extern undefined1 DAT_0061821c;
extern undefined1 DAT_00618208;
extern undefined1 DAT_00618218;
extern int DAT_00911ae4;
extern undefined4 DAT_007d4150;
extern int * DAT_007d4578;
extern int * DAT_007d4110;
extern int * DAT_007d4574;
extern int DAT_00912100;
extern int * DAT_007d4108;
extern undefined4 DAT_007d410c;
extern int DAT_00911f90;
extern int DAT_009120e8;
extern undefined4 DAT_00911f94;
extern int *DAT_007d4118;
extern int *DAT_007d4114;
extern int DAT_007d456c;
extern int DAT_007d4570;
extern code *DAT_007d459c;
extern HWND DAT_007d4104;
extern int DAT_00911f84;
extern int DAT_00911f88;
extern undefined4 DAT_009120e0;
extern int DAT_009120e4;
extern undefined4 DAT_007d411c;
__declspec(dllimport) BOOL __stdcall AdjustWindowRectEx(LPRECT p0, DWORD p1, BOOL p2, DWORD p3);
int FUN_004c0ed0();
float * FUN_004c4dc0(float * p0, float * p1);
undefined4 FUN_004c7650();
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 FUN_004c9ad0(void);
undefined4 FUN_004c9fb0();
bool FUN_004cb330();
undefined4 FUN_004d1e30(void);
void FUN_004d5480(int p0, int p1);
void FUN_004d6200(void);
undefined4 FUN_004db3e0(void);
undefined4 FUN_004dc970(void);
undefined4 FUN_004e0920(void);
void FUN_005cb432(void);
__declspec(dllimport) BOOL __stdcall GetClientRect(HWND p0, LPRECT p1);
__declspec(dllimport) HMENU __stdcall GetMenu(HWND p0);
__declspec(dllimport) LONG __stdcall GetWindowLongA(HWND p0, int p1);
__declspec(dllimport) BOOL __stdcall IsIconic(HWND p0);
__declspec(dllimport) LONG __stdcall SetWindowLongA(HWND p0, int p1, LONG p2);
__declspec(dllimport) BOOL __stdcall SetWindowPos(HWND p0, HWND p1, int p2, int p3, int p4, int p5, UINT p6);
extern int _DAT_00911d20;
extern int _DAT_00911d30;
extern int _DAT_00911d40;
extern int _DAT_00911d50;
extern int _DAT_00911d2c;
extern int _DAT_00911d3c;
extern int _DAT_00911d4c;
extern int _DAT_00911d5c;
extern int _DAT_006181e0;
extern int _DAT_006181f4;
extern int _DAT_00618200;
extern int _DAT_00618204;
extern int _DAT_00618210;
extern int _DAT_00618214;
extern int _DAT_0061820c;
extern int _DAT_0061821c;
extern int _DAT_00618208;
extern int _DAT_005cc320;
extern int _DAT_00618218;
extern int stack0xffffff9c;
extern int DAT_007d4140;
// MATCH: 0x004ca160 FUN_004ca160




int FUN_004ca160(undefined4 param_1,int param_2)



{

  bool bVar1;

  undefined4 uVar2;

  int *piVar3;

  BOOL BVar4;

  uint uVar5;

  DWORD dwExStyle;

  HMENU pHVar6;

  DWORD dwStyle;

  int iVar7;

  int iVar8;

  int iVar9;

  int *piVar10;

  int iVar11;

  int *piVar12;

  undefined1 auStack_50 [12];

  int iStack_44;

  int iStack_40;

  int iStack_3c;

  int *piStack_20;

  int iStack_18;

  int iStack_14;

  int *piStack_10;

  int iStack_c;

  int *piStack_8;

  undefined4 uStack_4;

  

  FUN_005cb432();

  DAT_00911d60 = param_2;

  uVar2 = FUN_004c0ed0();

  FUN_004c4dc0(&DAT_00911d20,uVar2);

  _DAT_00911d20 = -_DAT_00911d20;

  _DAT_00911d30 = -_DAT_00911d30;

  _DAT_00911d40 = -_DAT_00911d40;

  _DAT_00911d50 = -_DAT_00911d50;

  _DAT_00911d2c = 0;

  _DAT_00911d3c = 0;

  _DAT_00911d4c = 0;

  _DAT_00911d5c = 0x3f800000;

  FUN_004cb330(2,&DAT_00911d20);

  _DAT_006181e0 = *(undefined4 *)(param_2 + 0x70);

  _DAT_006181f4 = *(undefined4 *)(param_2 + 0x74);

  _DAT_00618200 = *(float *)(param_2 + 0x78) * *(float *)(param_2 + 0x70);

  _DAT_00618204 = *(float *)(param_2 + 0x7c) * *(float *)(param_2 + 0x74);

  _DAT_00618210 = -(*(float *)(param_2 + 0x78) * *(float *)(param_2 + 0x70));

  _DAT_00618214 = -(*(float *)(param_2 + 0x7c) * *(float *)(param_2 + 0x74));

  if (*(int *)(param_2 + 0x14) == 2) {

    _DAT_0061820c = 0;

    _DAT_0061821c = 0x3f800000;

    _DAT_00618208 = _DAT_005cc320;

  }

  else {

    _DAT_00618208 = *(float *)(param_2 + 0x84);

    _DAT_0061820c = 0x3f800000;

    _DAT_0061821c = 0;

  }

  _DAT_00618208 = _DAT_00618208 / (*(float *)(param_2 + 0x84) - *(float *)(param_2 + 0x80));

  _DAT_00618218 = -(_DAT_00618208 * *(float *)(param_2 + 0x80));

  FUN_004cb330(3);

  DAT_007d4150 = 0;

  piVar12 = *(int **)(param_2 + 0x60);

  piVar3 = (int *)*piVar12;

  piVar10 = (int *)(DAT_00911ae4 + (int)piVar3);

  (*(uint *)((char *)&auStack_50 + 0)) = piVar12;

  if (*(int *)(DAT_00911ae4 + 0x1c + (int)piVar3) == 0) {

    GetClientRect(DAT_007d4104,(LPRECT)(auStack_50 + 8));

    if (iStack_40 == 0) {

      return 0;

    }

    if (iStack_3c == 0) {

      return 0;

    }

    BVar4 = IsIconic(DAT_007d4104);

    if (BVar4 != 0) {

      return 0;

    }

    if ((iStack_40 != DAT_00911f84) || (iStack_3c != DAT_00911f88)) {

      FUN_004c9ad0();

      iVar7 = (**(code **)(*DAT_007d4110 + 0xc))();

      if ((-1 < iVar7) && (DAT_00912100 != 0)) {

        DAT_009120e0 = iStack_44;

        DAT_009120e4 = iStack_40;

      }

      iVar7 = (**(code **)(*DAT_007d4110 + 0x40))(DAT_007d4110,&DAT_009120e0);

      if (-1 < iVar7) {

        (**(code **)(*DAT_007d4110 + 0x98))(DAT_007d4110,0);

        (**(code **)(*DAT_007d4118 + 8))(DAT_007d4118);

        (**(code **)(*DAT_007d4110 + 0xa0))(DAT_007d4110,&DAT_007d4114);

        (**(code **)(*DAT_007d4114 + 8))(DAT_007d4114);

        iVar7 = FUN_004d1e30();

        if ((iVar7 == 0) || (iVar7 = FUN_004dc970(), iVar7 == 0)) {

LAB_004ca55a:

          bVar1 = false;

        }

        else {

          FUN_004d6200();

          iVar7 = 0;

          if (0 < DAT_007d456c) {

            iVar8 = 0;

            do {

              (**(code **)(*DAT_007d4110 + 0xcc))(DAT_007d4110,iVar7);

              iVar11 = (**(code **)(*DAT_007d4110 + 0xd4))

                                 (DAT_007d4110,iVar7,*(undefined4 *)(iVar8 + 0x68 + DAT_007d4570));

              iVar7 = iVar7 + 1;

              iVar8 = iVar8 + 0x6c;

            } while (iVar7 < DAT_007d456c);

            if (iVar11 < 0) goto LAB_004ca55a;

          }

          iVar7 = FUN_004db3e0();

          if ((iVar7 == 0) || (iVar7 = FUN_004e0920(), iVar7 == 0)) goto LAB_004ca55a;

          bVar1 = true;

        }

        if (DAT_007d459c != (code *)0x0) {

          (*DAT_007d459c)();

        }

        if (bVar1) {

          DAT_00911f84 = iStack_40;

          DAT_00911f88 = iStack_3c;

          goto LAB_004ca739;

        }

        FUN_004c9ad0();

      }

      if (DAT_00912100 == 0) {

        return 0;

      }

      DAT_009120e4 = DAT_00911f88;

      DAT_009120e0 = DAT_00911f84;

      iVar7 = (**(code **)(*DAT_007d4110 + 0x40))(DAT_007d4110);

      if (iVar7 < 0) {

        return 0;

      }

      (*(uint *)((char *)&auStack_50 + 8)) = DAT_00911f84 + (*(uint *)((char *)&auStack_50 + 0));

      iStack_44 = DAT_00911f88 + (*(uint *)((char *)&auStack_50 + 4));

      uVar5 = GetWindowLongA(DAT_007d4104,-0x10);

      if ((uVar5 & 0x1000000) != 0) {

        uVar5 = GetWindowLongA(DAT_007d4104,-0x10);

        SetWindowLongA(DAT_007d4104,-0x10,uVar5 & 0xfeffffff);

      }

      dwExStyle = GetWindowLongA(DAT_007d4104,-0x14);

      pHVar6 = GetMenu(DAT_007d4104);

      uVar5 = (uint)(pHVar6 != (HMENU)0x0);

      dwStyle = GetWindowLongA(DAT_007d4104,-0x10);

      AdjustWindowRectEx((LPRECT)auStack_50,dwStyle,uVar5,dwExStyle);

      SetWindowPos(DAT_007d4104,(HWND)0x0,(*(uint *)((char *)&auStack_50 + 0)),iStack_44,

                   (*(uint *)((char *)&auStack_50 + 8)) - (*(uint *)((char *)&auStack_50 + 0)),iStack_44 - (*(uint *)((char *)&auStack_50 + 4)),6);

      (**(code **)(*DAT_007d4110 + 0x98))(DAT_007d4110,0,&DAT_007d4118);

      (**(code **)(*DAT_007d4118 + 8))(DAT_007d4118);

      (**(code **)(*DAT_007d4110 + 0xa0))(DAT_007d4110,&DAT_007d4114);

      (**(code **)(*DAT_007d4114 + 8))(DAT_007d4114);

      iVar7 = FUN_004d1e30();

      if ((iVar7 != 0) && (iVar7 = FUN_004dc970(), iVar7 != 0)) {

        FUN_004d6200();

        iVar7 = 0;

        if (0 < DAT_007d456c) {

          iVar8 = 0;

          do {

            (**(code **)(*DAT_007d4110 + 0xcc))(DAT_007d4110,iVar7);

            iVar11 = (**(code **)(*DAT_007d4110 + 0xd4))

                               (DAT_007d4110,iVar7,*(undefined4 *)(iVar8 + 0x68 + DAT_007d4570));

            iVar7 = iVar7 + 1;

            iVar8 = iVar8 + 0x6c;

          } while (iVar7 < DAT_007d456c);

          if (iVar11 < 0) goto LAB_004ca719;

        }

        iVar7 = FUN_004db3e0();

        if (iVar7 != 0) {

          FUN_004e0920();

        }

      }

LAB_004ca719:

      if (DAT_007d459c != (code *)0x0) {

        (*DAT_007d459c)();

      }

    }

LAB_004ca739:

    if (*(char *)(piVar3 + 8) == '\x05') {

      if ((*(byte *)((int)piVar10 + 9) & 0xf) == 0) {

        piVar12 = (int *)0x4ca779;

        (**(code **)(*(int *)*piVar10 + 0x48))((int *)*piVar10,0);

      }

      else {

        piVar12 = (int *)*piVar10;

        (**(code **)(*piVar12 + 0x48))(piVar12,*(byte *)((int)piVar10 + 9) >> 4,0);

      }

      (**(code **)(*piStack_8 + 0x30))(piStack_8,auStack_50 + 8);

      if (((*(uint *)((char *)&auStack_50 + 8)) & 1) == 0) {

        DAT_007d411c = 1;

        piVar10 = DAT_007d4114;

        if (DAT_007d4578 != DAT_007d4118) {

          DAT_007d4578 = DAT_007d4118;

          (**(code **)(*DAT_007d4110 + 0x94))(DAT_007d4110,0,DAT_007d4118);

          piVar10 = DAT_007d4114;

        }

LAB_004ca875:

        if (DAT_007d4574 != piVar10) {

          iVar7 = *DAT_007d4110;

          DAT_007d4574 = piVar10;

LAB_004ca88c:

          (**(code **)(iVar7 + 0x9c))(DAT_007d4110,DAT_007d4574);

        }

      }

      else {

        if (DAT_007d4578 != piStack_10) {

          DAT_007d4578 = piStack_10;

          (**(code **)(*DAT_007d4110 + 0x94))(DAT_007d4110,0,piStack_10);

        }

        if (*(int **)(param_2 + 100) != (int *)0x0) {

          iVar7 = **(int **)(param_2 + 100);

          piVar10 = *(int **)(DAT_00911ae4 + iVar7);

          if ((*(byte *)(iVar7 + 0x20) & 7) != 1) {

            (**(code **)(*piVar10 + 0x48))(piVar10,0,&stack0xffffff9c);

            if (DAT_007d4574 != piVar12) {

              DAT_007d4574 = piVar12;

              (**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110,piVar12);

            }

            (**(code **)(*piVar12 + 8))(piVar12);

            (**(code **)(*piStack_20 + 8))(piStack_20);

            piVar12 = (int *)(*(uint *)((char *)&auStack_50 + 0));

            goto LAB_004ca8ec;

          }

          goto LAB_004ca875;

        }

        if (DAT_007d4574 != (int *)0x0) {

          DAT_007d4574 = (int *)0x0;

          iVar7 = *DAT_007d4110;

          goto LAB_004ca88c;

        }

      }

      (**(code **)(*piStack_10 + 8))(piStack_10);

      piVar12 = (int *)(*(uint *)((char *)&auStack_50 + 0));

    }

    else {

      if (DAT_007d4578 != DAT_007d4118) {

        DAT_007d4578 = DAT_007d4118;

        (**(code **)(*DAT_007d4110 + 0x94))(DAT_007d4110,0);

      }

      piVar12 = (int *)(*(uint *)((char *)&auStack_50 + 0));

      if (DAT_007d4574 != DAT_007d4114) {

        DAT_007d4574 = DAT_007d4114;

        (**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110);

        piVar12 = (int *)(*(uint *)((char *)&auStack_50 + 0));

      }

    }

  }

  else {

    GetClientRect((HWND)piVar10[8],(LPRECT)(auStack_50 + 8));

    if (iStack_40 == 0) {

      return 0;

    }

    if (iStack_3c == 0) {

      return 0;

    }

    if ((iStack_40 == piVar3[10]) && (iStack_3c == piVar3[0xb])) {

      piVar3 = *(int **)(param_2 + 100);

      if (((piVar3 != (int *)0x0) && (piVar3 == (int *)*piVar3)) &&

         ((iStack_40 != piVar3[3] || (iStack_3c != piVar3[4])))) {

        *(undefined4 *)(param_2 + 100) = 0;

        FUN_004c7650();

        piVar3 = (int *)FUN_004c77c0(iStack_40,iStack_3c,0,1);

        if (piVar3 != (int *)0x0) {

          *(int **)(param_2 + 100) = piVar3;

        }

      }

    }

    else {

      if (piVar12 == piVar3) {

        FUN_004c9fb0(param_2);

        piVar12 = *(int **)(param_2 + 0x60);

        piVar10 = (int *)(*piVar12 + DAT_00911ae4);

      }

      piVar3 = *(int **)(param_2 + 100);

    }

    if (DAT_007d4578 != (int *)*piVar10) {

      DAT_007d4578 = (int *)*piVar10;

      (**(code **)(*DAT_007d4110 + 0x94))(DAT_007d4110,0);

    }

    if (piVar3 == (int *)0x0) {

      if (DAT_007d4574 != (int *)0x0) {

        DAT_007d4574 = (int *)0x0;

        (**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110);

      }

    }

    else if (DAT_007d4574 != *(int **)(DAT_00911ae4 + *piVar3)) {

      DAT_007d4574 = *(int **)(DAT_00911ae4 + *piVar3);

      (**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110);

    }

  }

LAB_004ca8ec:

  iStack_18 = (int)(short)piVar12[7];

  iStack_14 = (int)*(short *)((int)piVar12 + 0x1e);

  piStack_10 = (int *)piVar12[3];

  iStack_c = piVar12[4];

  piStack_8 = (int *)0x0;

  uStack_4 = 0x3f800000;

  (**(code **)(*DAT_007d4110 + 0xbc))(DAT_007d4110);

  FUN_004d5480(0x24,*(undefined4 *)(param_2 + 0x88));

  FUN_004d5480(0x25,*(undefined4 *)(param_2 + 0x84));

  iVar7 = (**(code **)(*DAT_007d4110 + 0xc))(DAT_007d4110);

  if (iVar7 != -0x7789f797) goto LAB_004caabb;

  FUN_004c9ad0();

  iVar7 = (**(code **)(*DAT_007d4110 + 0x40))(DAT_007d4110,&DAT_009120e0);

  if (iVar7 == -0x7789f794) {

    if (DAT_00912100 == 0) {

      DAT_007d4140 = 0;

      return 0;

    }

    (**(code **)(*DAT_007d4108 + 0x20))(DAT_007d4108,DAT_007d410c,auStack_50 + 4);

    DAT_00911f90 = (*(uint *)((char *)&auStack_50 + 4));

    DAT_009120e8 = (*(uint *)((char *)&auStack_50 + 4));

    switch((*(uint *)((char *)&auStack_50 + 4))) {

    case 0x15:

    case 0x16:

    case 0x23:

      DAT_00911f94 = 0x20;

      break;

    case 0x17:

    case 0x18:

    case 0x19:

      DAT_00911f94 = 0x10;

      break;

    default:

      DAT_00911f94 = 0;

    }

    iVar7 = (**(code **)(*DAT_007d4110 + 0x40))(DAT_007d4110,&DAT_009120e0);

  }

  if (iVar7 < 0) {

    DAT_007d4140 = 0;

    return 0;

  }

  (**(code **)(*DAT_007d4110 + 0x98))(DAT_007d4110,0,&DAT_007d4118);

  (**(code **)(*DAT_007d4118 + 8))(DAT_007d4118);

  (**(code **)(*DAT_007d4110 + 0xa0))(DAT_007d4110,&DAT_007d4114);

  (**(code **)(*DAT_007d4114 + 8))(DAT_007d4114);

  iVar8 = FUN_004d1e30();

  if ((iVar8 != 0) && (iVar8 = FUN_004dc970(), iVar8 != 0)) {

    FUN_004d6200();

    iVar8 = 0;

    if (0 < DAT_007d456c) {

      iVar11 = 0;

      do {

        (**(code **)(*DAT_007d4110 + 0xcc))(DAT_007d4110,iVar8,DAT_007d4570 + iVar11);

        iVar9 = (**(code **)(*DAT_007d4110 + 0xd4))

                          (DAT_007d4110,iVar8,*(undefined4 *)(iVar11 + 0x68 + DAT_007d4570));

        iVar8 = iVar8 + 1;

        iVar11 = iVar11 + 0x6c;

      } while (iVar8 < DAT_007d456c);

      if (iVar9 < 0) goto LAB_004caab0;

    }

    iVar8 = FUN_004db3e0();

    if (iVar8 != 0) {

      FUN_004e0920();

    }

  }

LAB_004caab0:

  if (DAT_007d459c != (code *)0x0) {

    (*DAT_007d459c)();

  }

LAB_004caabb:

  if (iVar7 < 0) {

    DAT_007d4140 = 0;

    return 0;

  }

  if (DAT_007d4140 == 0) {

    iVar7 = (**(code **)(*DAT_007d4110 + 0xa4))(DAT_007d4110);

    if (-1 < iVar7) {

      DAT_007d4140 = 1;

      return 1;

    }

    return DAT_007d4140;

  }

  return DAT_007d4140;

}



