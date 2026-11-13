// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWT-A 0x00549710: g2s.py output (decomp_pc.py --port --callees), edits: the four split locals are one undefined4 a[4] passed to FUN_00549900.
   Byte-identical under rw9044 and ddk. verify/match_20261112/RESULT_MATCH-RWT-A.md */
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
extern int DAT_007dc730;
extern undefined4 DAT_00623d7c;
extern undefined4 DAT_00623d78;
extern undefined4 DAT_00623d84;
extern undefined4 DAT_007dc734;
extern undefined4 DAT_00623d80;
extern undefined4 DAT_007dc738;
extern undefined1 DAT_007dc6d8;
extern undefined1 DAT_007dc6fc;
undefined4 FUN_004c2dc0(undefined4 p0);
undefined4 FUN_00549900(uint * p0);
undefined4 FUN_00549970(void);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x00549710 FUN_00549710
undefined4 FUN_00549710(undefined4 param_1)
{
  int iVar1;
  uint uVar2;
  undefined4 a[4];
  if (DAT_007dc730 == 0) {
    uVar2 = 1;
    iVar1 = FUN_004c2dc0(0x116);
    if (iVar1 != -1) {
      uVar2 = 3;
    }
    iVar1 = FUN_004c2dc0(0x120);
    if (iVar1 != -1) {
      uVar2 = uVar2 | 4;
    }
    FUN_00549970(uVar2);
    DAT_007dc734 = thunk_FUN_004cc820(0xc,DAT_00623d78,4,DAT_00623d7c,&DAT_007dc6d8,0x40123);
    DAT_007dc738 = thunk_FUN_004cc820(8,DAT_00623d80,4,DAT_00623d84,&DAT_007dc6fc,0x40123);
  }
  DAT_007dc730 = DAT_007dc730 + 1;
  a[0] = 4;
  a[1] = 0xc;
  a[2] = 0x40800000;
  a[3] = 0x43480000;
  FUN_00549900(a);
  return param_1;
}
