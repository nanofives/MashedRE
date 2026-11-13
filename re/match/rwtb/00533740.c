// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (57.9%) to 100%: plugin attach thunk_FUN_004cc820 (0x4cc9e0) (0x24, [0x623c70], 4, [0x623c74], &DAT_007dc554, 0x4011e) stored to DAT_0091246c, param_1 = 0 on failure, then a 12-dword registration record {1, 0x24, 0x24, 0x532a60, 0x532fa0, 0x532b80, 0x533600, 0x5334a0, 0x5333a0, 0x533420, 0x533490, 0} passed to FUN_0052d980; returns param_1. */ The five Ghidra LAB_ addresses are named FUN_<rva> so relocheck checks all 14 relocations.
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

extern undefined4 DAT_00623c74;
extern undefined4 DAT_00623c70;
extern int DAT_0091246c;
extern undefined1 FUN_00532fa0;
extern undefined1 FUN_00533420;
extern undefined1 FUN_00533490;
extern undefined1 FUN_005334a0;
extern undefined1 FUN_00533600;
extern undefined1 DAT_007dc554;
extern undefined1 FUN_005333a0;
extern undefined1 FUN_00532a60;
extern undefined1 FUN_00532b80;
undefined4 FUN_0052d980(int * p0);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x00533740 FUN_00533740

typedef struct { int a, b, c; void *f[8]; int z; } Reg3740;
undefined4 FUN_00533740(undefined4 param_1)
{
  Reg3740 r;
  DAT_0091246c = (int)thunk_FUN_004cc820(0x24,DAT_00623c70,4,DAT_00623c74,(uint *)&DAT_007dc554,0x4011e);
  if (DAT_0091246c == 0) {
    param_1 = 0;
  }
  r.a = 1;
  r.b = 0x24;
  r.c = 0x24;
  r.f[0] = &FUN_00532a60;
  r.f[1] = &FUN_00532fa0;
  r.f[2] = &FUN_00532b80;
  r.f[3] = &FUN_00533600;
  r.f[4] = &FUN_005334a0;
  r.f[5] = &FUN_005333a0;
  r.f[6] = &FUN_00533420;
  r.f[7] = &FUN_00533490;
  r.z = 0;
  FUN_0052d980((int *)&r);
  return param_1;
}
