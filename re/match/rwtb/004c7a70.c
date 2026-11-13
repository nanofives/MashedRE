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

extern uint DAT_00911ff8;
extern uint DAT_00911ffc;
extern int DAT_007d458c;
extern int DAT_007d4590;
extern int DAT_007d4594;
extern uint DAT_007d410c;
extern int DAT_00911f80;
extern int * DAT_007d4108;
extern uint DAT_00911f98;
extern uint DAT_006181b4;
extern undefined4 DAT_006181b8;
extern undefined1 DAT_006181bc;
extern undefined4 DAT_006181c0;
extern int DAT_00911f90;
extern undefined4 DAT_006181b0;
extern int DAT_007d3ff8;
extern int *DAT_007d4110;
extern uint DAT_007d4100;
extern int DAT_00912100;
extern HWND DAT_007d4104;
extern int DAT_007d4120;
extern uint DAT_007d4128;
extern int DAT_007d4130;
extern undefined4 DAT_00911f84;
extern int * DAT_00911f88;
extern undefined4 DAT_00911f8c;
extern undefined4 DAT_00911f94;
extern undefined1 DAT_00911d64;
extern int DAT_007d413c;
extern undefined1 DAT_009120fc;
extern undefined1 DAT_0091210c;
extern uint DAT_00912074;
extern int DAT_007d4134;
extern undefined1 RwV3dTransformPoints;
extern int DAT_007d4138;
extern uint DAT_00911fbc;
extern undefined1 RwV3dTransformPoint;
extern undefined1 DAT_0091208c;
extern int *DAT_007d4118;
extern int *DAT_007d4114;
extern undefined4 DAT_007d4574;
extern undefined4 DAT_007d4578;
extern undefined4 DAT_007d457c;
extern undefined4 DAT_007d4580;
extern undefined1 DAT_007d4584;
extern int DAT_007d4570;
extern undefined4 DAT_007d456c;
extern undefined1 DAT_007d40b0;
extern undefined1 DAT_007d40b4;
extern undefined1 DAT_007d40b8;
extern undefined1 DAT_007d40bc;
extern undefined1 DAT_007d4588;
extern undefined4 DAT_005d8b80;
extern undefined4 DAT_00618220;
extern undefined1 DAT_00911d80;
extern undefined1 DAT_00911fa0;
extern undefined4 DAT_009120e0;
extern uint ;
extern undefined1 FUN_004dc160;
extern undefined1 FUN_004dc280;
extern undefined1 FUN_004dc370;
extern undefined1 FUN_004dc4a0;
int *Direct3DCreate9(void);
undefined4 FUN_004c35f0();
undefined4 FUN_004c40b0();
void FUN_004c8650(void);
void FUN_004c8690(void);
void FUN_004c8740(void);
void FUN_004c8800();
undefined4 FUN_004c8c70(void);
undefined4 FUN_004c8e50();
void FUN_004c90a0(void);
void FUN_004cfe20(void);
void FUN_004d5bc0(void);
undefined4 FUN_004db3e0(void);
undefined4 FUN_004dcd50(void);
uint FUN_004dcf90(void);
bool FUN_004dcff0(void);
uint FUN_004dd050(void);
longlong __fastcall FUN_004dd0b0();
undefined4 FUN_005cb404(void);
typedef struct tagRECT { long left, top, right, bottom; } tagRECT, *LPRECT;
__declspec(dllimport) BOOL __stdcall GetClientRect(HWND p0, LPRECT p1);
__declspec(dllimport) BOOL __stdcall ShowWindow(HWND p0, int p1);
extern int pcStack_48c;
extern int pcStack_488;
extern int uVar7;
extern int uStack_470;
extern int auStack_468;
extern int uStack_45c;
extern int auStack_25c;
extern int uStack_8;
extern int piStack_494;
extern int piStack_498;
extern int piStack_490;
extern int _DAT_007d40b0;
extern int _DAT_007d40b4;
extern int _DAT_007d40b8;
extern int _DAT_007d40bc;
extern int _DAT_007d4588;
extern int _DAT_009120fc;
extern int _DAT_0091210c;
extern int iStack_4a0;
extern int piStack_4a4;
extern int _DAT_0091208c;
extern int _DAT_007d4584;
extern int _DAT_00911d64;
extern int _DAT_006181bc;
extern int stack0xfffffb84;
extern int MXCSR;
// MATCH: 0x004c7a70 FUN_004c7a70
/* [C1 2026-05-12] Switch dispatch on param_1 — 23 cases covering RenderWare `rwDEVICESYSTEM_*`
   opcodes. Each case… */

undefined4 FUN_004c7a70(int param_1,uint *param_2,undefined4 *param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  uint *puVar4;
  undefined4 *puVar5;
  uint uVar6;
  tagRECT tStack_4b4;
  int *piStack_4a4;
  int iStack_4a0;
  undefined1 *puStack_49c;
  int *piStack_498;
  int *piStack_494;
  int *piStack_490;
  code *pcStack_48c;
  code *pcStack_488;
  uint uVar7;
  uint uStack_470;
  undefined1 auStack_468 [12];
  undefined4 uStack_45c;
  undefined1 auStack_25c [524];
  int iStack_50;
  int iStack_4c;
  undefined4 uStack_8;
  
  switch(param_1) {
  case 0x0:
    DAT_007d4104 = (HWND)*param_3;
    pcStack_488 = (code *)0x4c7e10;
    DAT_007d4108 = (int *)Direct3DCreate9();
    if (DAT_007d4108 == (int *)0x0) {
      return 0;
    }
    pcStack_488 = (code *)&DAT_00911d80;
    pcStack_48c = (code *)0x4c7e3e;
    (**(code **)(DAT_007d3ff8 + 0xcc))();
    pcStack_488 = (code *)0x4c7e4c;
    uVar7 = (**(code **)(*DAT_007d4108 + 0x10))();
    DAT_007d410c = 0;
    DAT_006181b0 = 1;
    if (uVar7 != 0) {
      do {
        pcStack_488 = (code *)&DAT_00911fa0;
        pcStack_48c = (code *)DAT_006181b0;
        piStack_494 = DAT_007d4108;
        piStack_498 = (int *)0x4c7e7d;
        piStack_490 = (int *)DAT_007d410c;
        iVar1 = (**(code **)(*DAT_007d4108 + 0x38))();
        if (-1 < iVar1) {
          if (DAT_007d410c < uVar7) {
            DAT_00911f80 = 0;
            puVar5 = &DAT_005d8b80;
            do {
              pcStack_488 = (code *)*puVar5;
              pcStack_48c = (code *)DAT_007d410c;
              piStack_490 = DAT_007d4108;
              piStack_494 = (int *)0x4c7ed4;
              iVar1 = (**(code **)(*DAT_007d4108 + 0x18))();
              puVar5 = puVar5 + 1;
              DAT_00911f80 = DAT_00911f80 + iVar1;
            } while ((int)puVar5 < 0x5d8b8c);
            pcStack_488 = (code *)&DAT_00911f84;
            pcStack_48c = (code *)DAT_007d410c;
            piStack_490 = DAT_007d4108;
            piStack_494 = (int *)0x4c7f04;
            (**(code **)(*DAT_007d4108 + 0x20))();
            switch(DAT_00911f90) {
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
            DAT_00911f98 = 0;
            _DAT_007d40b0 = DAT_00911f84;
            _DAT_007d40b4 = DAT_00911f88;
            _DAT_007d40b8 = DAT_00911f8c;
            _DAT_007d40bc = DAT_00911f90;
            DAT_007d4100 = 0;
            piStack_494 = (int *)0x4c7f6d;
            FUN_004c8740();
            piStack_494 = (int *)0x4c7f72;
            _DAT_007d4588 = FUN_004dcf90();
            piStack_494 = (int *)0x4c7f7c;
            DAT_007d458c = FUN_004dcff0();
            piStack_494 = (int *)0x4c7f86;
            DAT_007d4590 = FUN_004dd050();
            piStack_494 = (int *)0x4c7f90;
            DAT_007d4594 = FUN_004dd0b0();
            return 1;
          }
          break;
        }
        DAT_007d410c = DAT_007d410c + 1;
      } while (DAT_007d410c < uVar7);
    }
    pcStack_488 = (code *)DAT_007d4108;
    pcStack_48c = (code *)0x4c7ea1;
    (**(code **)(*DAT_007d4108 + 8))();
    DAT_007d4108 = (int *)0x0;
    return 0;
  case 0x1:
    if (DAT_007d4130 != 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))();
      DAT_007d4130 = 0;
      DAT_007d4128 = 0;
    }
    if (DAT_007d4108 == (int *)0x0) {
      return 1;
    }
    (**(code **)(*DAT_007d4108 + 8))();
    DAT_007d4108 = (int *)0x0;
    return 1;
  case 0x2:
    pcStack_488 = (code *)DAT_007d4108;
    pcStack_48c = (code *)0x4c819c;
    (**(code **)(*DAT_007d4108 + 0x20))();
    puVar5 = &DAT_009120e0;
    for (iVar1 = 0xe; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    pcStack_48c = (code *)uStack_45c;
    piStack_490 = (int *)DAT_00911f98;
    piStack_494 = (int *)&DAT_00911f84;
    piStack_498 = (int *)0x4c81c0;
    FUN_004c8800();
    _DAT_009120fc = DAT_007d4104;
    _DAT_0091210c = (uint)(DAT_007d413c != 0);
    pcStack_48c = (code *)&DAT_00911fa0;
    piStack_490 = (int *)DAT_006181b0;
    piStack_494 = (int *)DAT_007d410c;
    piStack_498 = DAT_007d4108;
    puStack_49c = (undefined1 *)0x4c8201;
    (**(code **)(*DAT_007d4108 + 0x38))();
    puStack_49c = auStack_468;
    iStack_4a0 = 0;
    piStack_4a4 = (int *)DAT_007d410c;
    tStack_4b4.bottom = (LONG)DAT_007d4108;
    tStack_4b4.right = 0x4c8219;
    (**(code **)(*DAT_007d4108 + 0x14))();
    if ((iStack_50 == 0x1002) && (iStack_4c == 0x5157)) {
      DAT_00912074 = DAT_00912074 & 0xfffffffe;
    }
    tStack_4b4.left = 0;
    if (DAT_007d4134 != 0) {
      tStack_4b4.left = 4;
    }
    if (DAT_007d4138 == 0) {
      if ((DAT_00911fbc & 0x10000) == 0) {
        tStack_4b4.left = tStack_4b4.left | 0x20;
      }
      else if ((DAT_00911fbc & 0x100000) == 0) {
        tStack_4b4.left = tStack_4b4.left | 0x40;
      }
      else {
        tStack_4b4.left = tStack_4b4.left | 0x50;
      }
    }
    else {
      tStack_4b4.left = tStack_4b4.left | 0x20;
      DAT_00911fbc = DAT_00911fbc & 0xfffeffff;
      _DAT_0091208c = 0;
    }
    tStack_4b4.right = (LONG)&DAT_007d4110;
    tStack_4b4.top = (LONG)&DAT_009120e0;
    iVar1 = (**(code **)(*DAT_007d4108 + 0x40))(DAT_007d4108,DAT_007d410c,DAT_006181b0,DAT_007d4104)
    ;
    if (-1 < iVar1) {
      if (DAT_007d413c != 0) {
        (**(code **)(*DAT_007d4110 + 0x50))(DAT_007d4110,1);
      }
      if (DAT_007d4104 != (HWND)0x0) {
        GetClientRect(DAT_007d4104,&tStack_4b4);
        DAT_00911f84 = (undefined4 *)tStack_4b4.right;
        DAT_00911f88 = (int *)tStack_4b4.bottom;
      }
      (**(code **)(*DAT_007d4110 + 0x98))(DAT_007d4110,0,&DAT_007d4118);
      (**(code **)(*DAT_007d4118 + 8))(DAT_007d4118);
      (**(code **)(*DAT_007d4110 + 0xa0))(DAT_007d4110,&DAT_007d4114);
      (**(code **)(*DAT_007d4114 + 8))(DAT_007d4114);
      DAT_007d4574 = 0;
      DAT_007d4578 = 0;
      DAT_007d457c = 0;
      DAT_007d4580 = 0;
      _DAT_007d4584 = 0;
      FUN_004c8650();
      FUN_004c8690();
      DAT_007d456c = 0;
      if (DAT_007d4570 != 0) {
        (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d4570);
        DAT_007d4570 = 0;
      }
      FUN_004cfe20();
      FUN_004db3e0();
      FUN_004d5bc0();
      FUN_004dcd50();
      DAT_007d4120 = 1;
      return 1;
    }
    DAT_007d4120 = 0;
    return 0;
  case 0x3:
    pcStack_488 = (code *)param_2;
    pcStack_48c = (code *)0x4c83dd;
    uVar3 = FUN_004c8c70();
    return uVar3;
  case 0x4:
    puVar4 = &DAT_00618220;
    for (iVar1 = 0xe; iVar1 != 0; iVar1 = iVar1 + -1) {
      *param_2 = *puVar4;
      puVar4 = puVar4 + 1;
      param_2 = param_2 + 1;
    }
    _DAT_00911d64 = param_3;
    return 1;
  case 0x5:
    if (DAT_007d4130 == 0) {
      FUN_004c90a0();
    }
    *param_2 = DAT_007d4128;
    return 1;
  case 0x6:
    if (DAT_007d4130 == 0) {
      FUN_004c90a0();
    }
    if ((-1 < param_4) && (param_4 < (int)DAT_007d4128)) {
      param_4 = param_4 * 0x14;
      *param_2 = *(uint *)(param_4 + DAT_007d4130);
      param_2[1] = *(uint *)(param_4 + 4 + DAT_007d4130);
      switch(*(undefined4 *)(param_4 + 0xc + DAT_007d4130)) {
      case 0x15:
      case 0x16:
      case 0x23:
        uVar7 = 0x20;
        break;
      case 0x17:
      case 0x18:
      case 0x19:
        uVar7 = 0x10;
        break;
      default:
        uVar7 = 0;
      }
      param_2[2] = uVar7;
      param_2[3] = *(uint *)(param_4 + 0x10 + DAT_007d4130);
      param_2[4] = *(uint *)(param_4 + 8 + DAT_007d4130);
      switch(*(undefined4 *)(param_4 + 0xc + DAT_007d4130)) {
      case 0x15:
      case 0x23:
        param_2[5] = 0x500;
        return 1;
      case 0x16:
        param_2[5] = 0x600;
        return 1;
      case 0x17:
        param_2[5] = 0x200;
        return 1;
      case 0x18:
        param_2[5] = 0xa00;
        return 1;
      case 0x19:
        param_2[5] = 0x100;
        return 1;
      default:
        param_2[5] = 0;
        return 1;
      }
    }
    break;
  case 0x7:
    if (((DAT_007d4120 == 0) && (-1 < param_4)) && (param_4 < (int)DAT_007d4128)) {
      DAT_007d4100 = param_4;
      puVar5 = (undefined4 *)(DAT_007d4130 + param_4 * 0x14);
      DAT_00911f84 = (undefined4 *)*puVar5;
      DAT_00911f88 = (int *)puVar5[1];
      DAT_00911f8c = puVar5[2];
      DAT_00911f90 = puVar5[3];
      DAT_00911f98 = puVar5[4];
      switch(puVar5[3]) {
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
      uVar7 = ~DAT_00911f98;
      DAT_006181b4 = 1;
      DAT_006181b8 = 1;
      _DAT_006181bc = 1;
      DAT_006181c0 = 1;
      uStack_470 = 2;
      do {
        pcStack_48c = (code *)DAT_00911f90;
        piStack_490 = (int *)DAT_006181b0;
        piStack_494 = (int *)DAT_007d410c;
        piStack_498 = DAT_007d4108;
        puStack_49c = (undefined1 *)0x4c7b72;
        pcStack_488 = (code *)(uVar7 & 1);
        iVar1 = (**(code **)(*DAT_007d4108 + 0x2c))();
        if (-1 < iVar1) {
          DAT_006181b4 = uStack_470;
        }
        uStack_470 = uStack_470 + 1;
      } while (uStack_470 < 0x11);
      pcStack_48c = (code *)DAT_00911f90;
      piStack_490 = (int *)DAT_006181b0;
      piStack_494 = (int *)DAT_007d410c;
      piStack_498 = DAT_007d4108;
      puStack_49c = (undefined1 *)0x4c7bb0;
      pcStack_488 = (code *)(uVar7 & 1);
      iVar1 = (**(code **)(*DAT_007d4108 + 0x2c))();
      if (iVar1 < 0) {
        return 1;
      }
      _DAT_006181bc = (int)pcStack_48c + 1;
      if (_DAT_006181bc <= DAT_006181b4) {
        return 1;
      }
      DAT_006181b4 = _DAT_006181bc;
      return 1;
    }
    break;
  case 0x8:
    if (DAT_007d4110 != (int *)0x0) {
      if (DAT_00912100 != 0) {
        return 1;
      }
      if (param_4 == 0) {
        pcStack_488 = (code *)0x4c7dbc;
        ShowWindow(DAT_007d4104,0);
        return 1;
      }
      pcStack_488 = (code *)0x4c7da0;
      ShowWindow(DAT_007d4104,9);
      return 1;
    }
    break;
  case 0xa:
    *param_2 = DAT_007d4100;
    return 1;
  case 0xb:
    pcStack_488 = (code *)param_2;
    pcStack_48c = (code *)0x4c8407;
    uVar3 = FUN_004c8e50();
    return uVar3;
  case 0xc:
    piVar2 = DAT_007d4110;
    goto LAB_004c8419;
  case 0xd:
    piVar2 = DAT_007d4108;
LAB_004c8419:
    uVar3 = (**(code **)(*piVar2 + 0x10))();
    *(undefined4 *)param_1 = uVar3;
    return 1;
  case 0xe:
    pcStack_488 = (code *)param_4;
    pcStack_48c = (code *)DAT_007d4108;
    piStack_490 = (int *)0x4c8015;
    (**(code **)(*DAT_007d4108 + 0x14))();
    piStack_490 = (int *)auStack_25c;
    piStack_494 = (int *)uStack_8;
    piStack_498 = (int *)0x4c8031;
    (**(code **)(DAT_007d3ff8 + 0xcc))();
    return 1;
  case 0xf:
    *param_2 = DAT_007d410c;
    return 1;
  case 0x10:
    DAT_007d410c = param_4;
    DAT_00911f80 = 0;
    puVar5 = &DAT_005d8b80;
    do {
      pcStack_488 = (code *)DAT_007d4108;
      pcStack_48c = (code *)0x4c8090;
      iVar1 = (**(code **)(*DAT_007d4108 + 0x18))();
      puVar5 = puVar5 + 1;
      DAT_00911f80 = DAT_00911f80 + iVar1;
    } while ((int)puVar5 < 0x5d8b8c);
    pcStack_488 = (code *)DAT_007d4108;
    pcStack_48c = (code *)0x4c80c0;
    (**(code **)(*DAT_007d4108 + 0x20))();
    pcStack_48c = (code *)0x4c80c5;
    FUN_004c90a0();
    uVar6 = ~DAT_00911f98;
    DAT_006181b4 = 1;
    DAT_006181b8 = 1;
    _DAT_006181bc = 1;
    DAT_006181c0 = 1;
    uVar7 = 2;
    do {
      pcStack_48c = (code *)0x0;
      piStack_498 = (int *)DAT_00911f90;
      puStack_49c = (undefined1 *)DAT_006181b0;
      iStack_4a0 = DAT_007d410c;
      piStack_4a4 = DAT_007d4108;
      tStack_4b4.bottom = 0x4c8116;
      piStack_494 = (int *)(uVar6 & 1);
      piStack_490 = (int *)uVar7;
      iVar1 = (**(code **)(*DAT_007d4108 + 0x2c))();
      if (-1 < iVar1) {
        DAT_006181b4 = uVar7;
      }
      uVar7 = uVar7 + 1;
    } while (uVar7 < 0x11);
    pcStack_48c = (code *)&stack0xfffffb84;
    piStack_490 = (int *)0x1;
    piStack_498 = (int *)DAT_00911f90;
    puStack_49c = (undefined1 *)DAT_006181b0;
    iStack_4a0 = DAT_007d410c;
    piStack_4a4 = DAT_007d4108;
    tStack_4b4.bottom = 0x4c8154;
    piStack_494 = (int *)(uVar6 & 1);
    iVar1 = (**(code **)(*DAT_007d4108 + 0x2c))();
    if (-1 < iVar1) {
      _DAT_006181bc = (int)piStack_498 + 1;
      if (DAT_006181b4 < _DAT_006181bc) {
        DAT_006181b4 = _DAT_006181bc;
        return 1;
      }
    }
    return 1;
  case 0x11:
    FUN_005cb404();
    if (DAT_007d458c != 0) {
      MXCSR = MXCSR | 0x8000;
    }
    FUN_004c40b0();
    if (DAT_007d458c == 0) {
      pcStack_488 = RwV3dTransformPoints;
      pcStack_48c = RwV3dTransformPoint;
      piStack_490 = (int *)0x4c84f3;
      FUN_004c35f0();
      return 1;
    }
    if ((DAT_007d4590 != 0) && (DAT_007d4594 == 0)) {
      pcStack_488 = FUN_004dc370;
      pcStack_48c = FUN_004dc4a0;
      piStack_490 = (int *)0x4c849d;
      FUN_004c35f0();
      return 1;
    }
    pcStack_488 = FUN_004dc160;
    pcStack_48c = FUN_004dc280;
    piStack_490 = (int *)0x4c84c8;
    FUN_004c35f0();
    return 1;
  case 0x13:
    if (DAT_00911ff8 < DAT_00911ffc) {
      *param_2 = DAT_00911ffc;
      return 1;
    }
    *param_2 = DAT_00911ff8;
    return 1;
  case 0x16:
    *(undefined2 *)param_2 = 2;
    return 1;
  }
  return 0;
}

