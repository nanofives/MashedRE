// FLAGS: /O2 /GS
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
typedef struct { void *pool; int size; } S30_PoolDesc;   /* 8-byte rows at 0x006336f8: +0 pool record, +4 size (0x005adaa2 / 0x005adaa9) */
extern S30_PoolDesc DAT_006336f8[5];
extern int DAT_007dd6c4;
extern int DAT_009146f0;
extern int _DAT_007dd6b8;
extern int _DAT_007dda20;
int FUN_005add60(void);
int FUN_005ae650(int p0, uint p1, uint p2, int p3, void * p4, undefined4 p5);
void FUN_005ae7e0(void * p0);
void FUN_005b3580(void * p0);
// MATCH: 0x005ada90 FUN_005ada90
int FUN_005ada90(void)
{
  uint i;
  FUN_005b3580(&DAT_009146f0);
  for (i = 0; i < 5; i++) {
    if (FUN_005ae650(DAT_006336f8[i].size,8,4,0,DAT_006336f8[i].pool,0x40803) == 0) {
      while (i > 0) {
        FUN_005ae7e0(DAT_006336f8[i - 1].pool);
        i--;
      }
    }
  }
  _DAT_007dd6b8 = 0;
  _DAT_007dda20 = 1;
  if (FUN_005ae650(0x28,0x10,4,0,&DAT_007dd6c4,0x40803) != 0) {
    if (FUN_005add60() == 1) return 1;
    FUN_005ae7e0(&DAT_007dd6c4);
  }
  while (i > 0) {
    FUN_005ae7e0(DAT_006336f8[i - 1].pool);
    i--;
  }
  _DAT_007dda20 = 0;
  return 0;
}
