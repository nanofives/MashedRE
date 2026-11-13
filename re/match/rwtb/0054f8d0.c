// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: g2s (56.7% rw9044) shaped to 64.3%: RwError struct, variadic FUN_004d7ff0, stream header read as two unsigned shorts {num, dev}. Remaining: the frame layout differs (locals 4 bytes apart from the first spill on), which shifts most stack operands. */
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

extern int DAT_007d3ff8;
extern undefined4 DAT_00618138;
undefined4 * FUN_004c5860(undefined4 * p0, int p1);
undefined1 * FUN_004c5890(void);
void FUN_004c5bc0(int p0, int p1);
void FUN_004c5c80(undefined4 p0);
undefined4 FUN_004c5ca0(void);
undefined4 FUN_004c7600(undefined4 p0);
undefined4 FUN_004c7650(undefined4 p0);
int FUN_004c76f0(int p0);
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
uint FUN_004c7860(undefined4 p0, uint p1, int p2);
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004cdd00(uint * p0);
undefined4 * FUN_004cee90(int p0);
uint * FUN_004cefd0(uint * p0);
int FUN_004d5310(int p0, byte * p1);
undefined4 FUN_004d5340(undefined4 p0, undefined4 p1, undefined4 * p2, undefined4 * p3, undefined4 * p4, uint * p5);
int FUN_004d7ff0(int p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_004db2e0(int p0, undefined4 p1, undefined4 p2);
int FUN_004e1b60(int p0, int p1, int p2);
undefined4 FUN_0054fd60(int p0, undefined4 * p1);
int FUN_00550130(undefined4 p0);
// MATCH: 0x0054f8d0 FUN_0054f8d0

typedef struct { int pluginID; int errorCode; } RwErrF8;
typedef struct { unsigned short num; unsigned short dev; } HdrF8;
int FUN_0054f8d0(undefined4 param_1)
{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int local_40;
  int iStack_3c;
  HdrF8 local_38;
  int iStack_34;
  int local_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  uint local_24;
  uint local_20;
  undefined4 local_1c;
  uint uStack_18;
  int iStack_14;
  int iStack_10;
  int iStack_c;
  RwErrF8 err;
  uVar1 = FUN_004cbd30(param_1,&local_38,4);
  if (uVar1 < 4) {
    return 0;
  }
  local_30 = FUN_004c5890();
  uVar1 = 0;
  if (local_30 == 0) {
    return 0;
  }
  if (local_38.dev < 1) {
    if (local_38.num > 0) {
      do {
        FUN_0054fd60(param_1,&local_30);
        uVar1 = uVar1 + 1;
      } while (uVar1 < local_38.num);
      return local_30;
    }
  }
  else {
    local_1c = FUN_004c5ca0();
    FUN_004c5c80(local_30);
    local_20 = 0;
    if (local_38.num > 0) {
      do {
        local_24 = 0;
        iVar2 = FUN_004cbd30(param_1,&local_40,4);
        if (iVar2 != 4) goto LAB_0054fd22;
        piVar3 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(local_40 * 4,0x101b3);
        if (piVar3 == (int *)0x0) {
          err.pluginID = 0x1b3;
          err.errorCode = FUN_004d7ff0(0x80000013,local_40 * 4);
          FUN_004d8480((undefined4 *)&err);
          FUN_004c5c80(local_1c);
          return 0;
        }
        iVar2 = 0;
        if (0 < local_40) {
          do {
            iVar4 = FUN_004cc5e0(param_1,0x18,0,0);
            if (iVar4 == 0) goto LAB_0054fd22;
            puVar5 = (uint *)FUN_004cee90(param_1);
            piVar3[iVar2] = (int)puVar5;
            if (puVar5 == (uint *)0x0) goto LAB_0054fd22;
            if ((*puVar5 & 2) == 0) {
              FUN_004cefd0(puVar5);
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 < local_40);
        }
        iVar2 = FUN_004cc5e0(param_1,6,0,0);
        if ((iVar2 == 0) || (iStack_c = FUN_00550130(param_1), iStack_c == 0)) goto LAB_0054fd22;
        uVar1 = 4;
        uStack_28 = *(undefined4 *)(*piVar3 + 4);
        uStack_2c = *(undefined4 *)(*piVar3 + 8);
        iStack_34 = *(int *)(*piVar3 + 0xc);
        if (local_40 < 2) {
          uVar6 = *(uint *)(iStack_c + 0x50) & 0xff;
          if ((uVar6 != 1) && (uVar6 != 2)) {
            uVar1 = 0x9004;
          }
        }
        else {
          uVar1 = 0x8004;
        }
        FUN_004d5340(*piVar3,uVar1,&uStack_28,&uStack_2c,&iStack_34,&uStack_18);
        iVar2 = FUN_004c77c0(uStack_28,uStack_2c,iStack_34,uStack_18 | uVar1);
        if (iVar2 == 0) {
          iVar2 = 0;
          if (0 < local_40) {
            do {
              FUN_004cdd00(piVar3[iVar2]);
              iVar2 = iVar2 + 1;
            } while (iVar2 < local_40);
          }
          (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
LAB_0054fd22:
          FUN_004c5c80(local_1c);
          return 0;
        }
        iVar4 = *(int *)(iVar2 + 0xc);
        if ((*(int *)(*piVar3 + 4) != iVar4) || (*(int *)(*piVar3 + 8) != *(int *)(iVar2 + 0x10))) {
          iStack_3c = *(int *)(iVar2 + 0x10);
          if (iStack_34 == 8) {
            FUN_004c7650(iVar2);
            iVar2 = FUN_004c77c0(iVar4,iStack_3c,0x20,uVar1 | 0x500);
            if (iVar2 == 0) {
              iVar2 = FUN_004c77c0(iVar4,iStack_3c,0x10,uVar1 | 0x100);
            }
          }
          iStack_10 = FUN_004c76f0(iVar2);
          iVar8 = 0;
          if (0 < iStack_10) {
            do {
              iStack_14 = FUN_004db2e0(piVar3[iVar8],iVar4,iStack_3c);
              if (iStack_14 == 0) goto LAB_0054fd22;
              FUN_004cdd00(piVar3[iVar8]);
              piVar3[iVar8] = iStack_14;
              if (1 < iVar4) {
                iVar4 = iVar4 >> 1;
              }
              if (1 < iStack_3c) {
                iStack_3c = iStack_3c >> 1;
              }
              iVar8 = iVar8 + 1;
            } while (iVar8 < iStack_10);
          }
        }
        if (1 < local_40) {
          local_24 = *(byte *)(iVar2 + 0x23) & 0x10;
          *(byte *)(iVar2 + 0x23) = ~(byte)local_24 & *(byte *)(iVar2 + 0x23);
        }
        iVar4 = FUN_004c76f0(iVar2);
        iVar8 = 0;
        if (0 < iVar4) {
          do {
            if (iVar8 == local_40) break;
            iVar7 = FUN_004c7860(iVar2,iVar8,1);
            if (((iVar7 == 0) || (iVar7 = FUN_004d5310(iVar2,piVar3[iVar8]), iVar7 == 0)) ||
               (iVar7 = FUN_004c7600(iVar2), iVar7 == 0)) goto LAB_0054fd22;
            FUN_004cdd00(piVar3[iVar8]);
            iVar8 = iVar8 + 1;
          } while (iVar8 < iVar4);
        }
        if (1 < local_40) {
          *(byte *)(iVar2 + 0x23) = *(byte *)(iVar2 + 0x23) | (byte)local_24;
        }
        if (iVar8 < local_40) {
          do {
            FUN_004cdd00(piVar3[iVar8]);
            iVar8 = iVar8 + 1;
          } while (iVar8 < local_40);
        }
        (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
        iVar4 = iStack_c;
        FUN_004c5860(iStack_c,iVar2);
        iVar2 = FUN_004e1b60(&DAT_00618138,param_1,iVar4);
        if (iVar2 == 0) goto LAB_0054fd22;
        iVar2 = FUN_004c5ca0();
        if (iVar2 != 0) {
          FUN_004c5bc0(iVar2,iVar4);
        }
        local_20 = local_20 + 1;
      } while (local_20 < local_38.num);
    }
    FUN_004c5c80(local_1c);
  }
  return local_30;
}
