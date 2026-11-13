// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (62.8% rw9044) to 100%: one RwMalloc(size, 0x30028) block, size from param_1 = {flags, nv, nb, na, nt} as in the decompiler, error (0x123, FUN_004d7ff0(0x80000013, size)) on NULL; memset 0; then sub-arrays after a 0x60-byte header: na records of 0x44 at +0x58, nb records of 0x2c at +0x5c, 12-byte vertex arrays at +0 / +4 for flag bits 1 / 2, 4-byte entries at +8 for bit 4, nt arrays of 8-byte entries at +0xc.., counts copied to +0x2c..+0x3c, +0x44 = 1, FUN_004f3bc0(+0x48) (failure frees the block and raises the same error), +0x40 = 0xffff. The record type names in this source are placeholders for the sizes, not identities. Levers: typed pointer steps, memset intrinsic, error struct, loop guard if (nt > 0) do..while. */
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
int FUN_004d7ff0(int p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined4 FUN_004f3bc0(undefined4 * p0);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x005493e0 FUN_005493e0

typedef struct { float x, y, z; } V3_93;
typedef struct { unsigned char r, g, b, a; } C4_93;
typedef struct { float u, v; } T2_93;
typedef struct { char d[0x44]; } A44_93;
typedef struct { char d[0x2c]; } B2c_93;
typedef struct { V3_93 *v0; V3_93 *v1; C4_93 *col; T2_93 *tex[8]; uint flags, nv, nb, na, nt; int f40; int one; int sub[4]; A44_93 *a; B2c_93 *bb; } Obj93;
typedef struct { int pluginID; int errorCode; } RwErr93;
undefined4 * FUN_005493e0(uint *param_1)
{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  RwErr93 err;
  uVar1 = *param_1;
  uVar1 = ((((uVar1 >> 1 & 1) + (uVar1 & 1)) * 3 + (uVar1 >> 2 & 1) + param_1[4] * 2) * param_1[1] +
           param_1[3] * 0x11 + param_1[2] * 0xb) * 4 + 0x60;
  puVar2 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(uVar1,0x30028);
  if (puVar2 == (undefined4 *)0x0) {
    err.pluginID = 0x123;
    err.errorCode = FUN_004d7ff0(0x80000013,uVar1);
    FUN_004d8480((undefined4 *)&err);
    return (undefined4 *)0x0;
  }
  memset(puVar2,0,uVar1);
  {
    Obj93 *o = (Obj93 *)puVar2;
    unsigned char *cur = (unsigned char *)(o + 1);
    o->a = (A44_93 *)cur;
    cur += param_1[3] * sizeof(A44_93);
    o->bb = (B2c_93 *)cur;
    cur += param_1[2] * sizeof(B2c_93);
    if (*param_1 & 1) {
      o->v0 = (V3_93 *)cur;
      cur += param_1[1] * sizeof(V3_93);
    }
    if (*param_1 & 2) {
      o->v1 = (V3_93 *)cur;
      cur += param_1[1] * sizeof(V3_93);
    }
    if (*param_1 & 4) {
      o->col = (C4_93 *)cur;
      cur += param_1[1] * sizeof(C4_93);
    }
    if (param_1[4] > 0) {
      uVar4 = 0;
      do {
        o->tex[uVar4] = (T2_93 *)cur;
        cur += param_1[1] * sizeof(T2_93);
        uVar4++;
      } while (uVar4 < param_1[4]);
    }
    o->flags = *param_1;
    o->na = param_1[3];
    o->nb = param_1[2];
    o->nv = param_1[1];
    o->nt = param_1[4];
    o->one = 1;
  }
  iVar5 = FUN_004f3bc0(puVar2 + 0x12);
  if (iVar5 == 0) {
    err.pluginID = 0x123;
    err.errorCode = FUN_004d7ff0(0x80000013,uVar1);
    FUN_004d8480((undefined4 *)&err);
    (**(code **)(DAT_007d3ff8 + 0x10c))(puVar2);
    return (undefined4 *)0x0;
  }
  puVar2[0x10] = 0xffff;
  return puVar2;
}
