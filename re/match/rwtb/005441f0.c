// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (70.2% rw9044) to 98.7%: user-data plugin copy (offset param_3, block {count, arrays}, 16-byte arrays {name, format, numElements, data}): destroy the dst arrays (free name, free each string when format 3, free data, free arrays), then copy count, malloc count*16 hint 0x3011f, per array copy format and count, strdup name as RwMalloc(rwstrlen(s) + 1, 0x30002) + rwstrcpy, element size 4 for formats 1/2/3 else 0 (switch with three case bodies), strings strdup-ed for format 3, memcpy otherwise. Block-scoped first loop counter, iStack_8 step after iStack_c (permuter). Remaining: the lea of param_2 + param_3 + 4 is scheduled before xor ebp in the original, and the pointer difference is formed as mov ebp,eax; sub ebp,esi.
   MATCH-NM-5441F0-4C4A50 2026-11-13: 98.7 -> 99.1% (regs 99.1, ops 99.6) under rw9044: the format-3 string copy as an
   index loop `for (param_2 = 0; param_2 < piVar8[2]; ...)` over piVar5[param_2] / piVar9[param_2] closes the
   `mov ebp,eax; sub ebp,esi` pointer-difference residue (c2 strength-reduces the two bases itself). One residue
   left: `lea edx,[esi+edi+4]` (param_2 + param_3 + 4, the src array base) before `xor ebp,ebp` in the original,
   after it here. Writing the full sum as its own variable puts it first but turns on the zero register (68.9);
   10 split forms x 5 positions, index-form outer loop (62.7), permuter 2606 compiles: no change. */
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
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x005441f0 FUN_005441f0

int FUN_005441f0(int param_1,int param_2,int param_3)
{
  undefined4 *puVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  int iVar7;
  int *piVar8;
  int *piVar9;
  int iVar10;
  int iStack_c;
  int iStack_8;
  piVar2 = (int *)(param_3 + 4 + param_1);
  if (*(int *)(param_3 + 4 + param_1) != 0) {
    int iStack_d;
    iStack_d = 0;
    if (0 < *(int *)(param_3 + param_1)) {
      iVar7 = 0;
      do {
        piVar8 = (int *)(*piVar2 + iVar7);
        if (*piVar8 != 0) {
          (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar8);
          *piVar8 = 0;
        }
        if (piVar8[1] == 3) {
          iVar4 = piVar8[3];
          iVar10 = 0;
          if (0 < piVar8[2]) {
            do {
              iVar3 = *(int *)(iVar4 + iVar10 * 4);
              if (iVar3 != 0) {
                (**(code **)(DAT_007d3ff8 + 0x10c))(iVar3);
              }
              iVar10 = iVar10 + 1;
            } while (iVar10 < piVar8[2]);
          }
        }
        if (piVar8[3] != 0) {
          (**(code **)(DAT_007d3ff8 + 0x10c))(piVar8[3]);
          piVar8[3] = 0;
        }
        iStack_d = iStack_d + 1;
        iVar7 = iVar7 + 0x10;
      } while (iStack_d < *(int *)(param_1 + param_3));
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar2);
  }
  *piVar2 = 0;
  *(undefined4 *)(param_3 + param_1) = 0;
  iVar7 = *(int *)(param_2 + param_3);
  *(int *)(param_3 + param_1) = iVar7;
  if (0 < iVar7) {
    iVar7 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar7 << 4,0x3011f);
    *piVar2 = iVar7;
    iStack_c = 0;
    if (0 < *(int *)(param_3 + param_1)) {
      iVar7 = param_2 + 4;
      iStack_8 = 0;
      do {
        piVar8 = (int *)(*piVar2 + iStack_8);
        piVar9 = (int *)(*(int *)(iVar7 + param_3) + iStack_8);
        piVar8[1] = piVar9[1];
        piVar8[2] = piVar9[2];
        if (*piVar9 != 0) {
          *piVar8 = 0;
          if (*piVar9 != 0) {
            puVar1 = (undefined4 *)(DAT_007d3ff8 + 0x108);
            iVar4 = (*(code *)*puVar1)((**(code **)(DAT_007d3ff8 + 0xf4))(*piVar9) + 1,0x30002);
            *piVar8 = iVar4;
            if (iVar4 != 0) {
              (**(code **)(DAT_007d3ff8 + 0xcc))(iVar4,*piVar9);
            }
          }
        }
        if (piVar9[3] != 0) {
          iVar4 = piVar8[1];
          switch (iVar4) {
          case 1:
            iVar4 = 4;
            break;
          case 2:
            iVar4 = 4;
            break;
          case 3:
            iVar4 = 4;
            break;
          default:
            iVar4 = 0;
            break;
          }
          iVar10 = piVar8[2];
          piVar5 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(iVar10 * iVar4,0x3011f);
          piVar8[3] = (int)piVar5;
          piVar9 = (int *)piVar9[3];
          if (piVar8[1] == 3) {
            for (param_2 = 0; param_2 < piVar8[2]; param_2 = param_2 + 1) {
                iVar10 = piVar9[param_2];
                piVar5[param_2] = 0;
                if ((iVar10 != 0) && (piVar9[param_2] != 0)) {
                  puVar1 = (undefined4 *)(DAT_007d3ff8 + 0x108);
                  iVar10 = (*(code *)*puVar1)((**(code **)(DAT_007d3ff8 + 0xf4))(piVar9[param_2]) + 1,0x30002);
                  piVar5[param_2] = iVar10;
                  if (iVar10 != 0) {
                    (**(code **)(DAT_007d3ff8 + 0xcc))(iVar10,piVar9[param_2]);
                  }
                }
            }
          }
          else {
            memcpy(piVar5,piVar9,iVar10 * iVar4);
          }
        }
        iStack_c = iStack_c + 1;
        iStack_8 = iStack_8 + 0x10;
      } while (iStack_c < *(int *)(param_1 + param_3));
      return param_1;
    }
  }
  return param_1;
}
