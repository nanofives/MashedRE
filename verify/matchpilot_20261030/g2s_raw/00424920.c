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

extern int DAT_00899a40;
extern int DAT_00899f20;
extern int DAT_00899f74;
extern int DAT_00899f24;
extern int DAT_00899a94;
extern int DAT_00899f78;
extern int DAT_00899a44;
extern int DAT_00899f28;
extern int DAT_00899a98;
extern int DAT_00899f70;
extern int DAT_00899a48;
extern int DAT_00899f7c;
extern int DAT_00899a90;
extern int DAT_00899e80;
extern int DAT_00899a9c;
extern int DAT_0089a058;
extern int DAT_008999a0;
extern int DAT_0089a0ac;
extern int DAT_00899b78;
extern int DAT_0089a05c;
extern int DAT_00899bcc;
extern int DAT_0089a0b0;
extern int DAT_00899b7c;
extern int DAT_0089a060;
extern int DAT_00899bd0;
extern int DAT_0089a0a8;
extern int DAT_00899b80;
extern int DAT_0089a0b4;
extern int DAT_00899bc8;
extern int DAT_00899fb8;
extern int DAT_00899bd4;
extern int DAT_0089a190;
extern int DAT_00899ad8;
extern int DAT_0089a1e4;
extern int DAT_00899cb0;
extern int DAT_0089a194;
extern int DAT_00899d04;
extern int DAT_0089a1e8;
extern int DAT_00899cb4;
extern int DAT_00899d08;
extern int DAT_00899cb8;
extern int DAT_0089a198;
extern int DAT_0089a1e0;
extern int DAT_0089a1ec;
extern int DAT_00899d00;
extern int DAT_0089a0f0;
extern int DAT_00899d0c;
extern int DAT_0089a2c8;
extern int DAT_00899c10;
extern int DAT_0089a31c;
extern int DAT_00899de8;
extern int DAT_0089a2cc;
extern int DAT_00899e3c;
extern int DAT_0089a320;
extern int DAT_00899dec;
extern int DAT_0089a2d0;
extern int DAT_00899e40;
extern int DAT_0089a318;
extern int DAT_00899df0;
extern int DAT_0089a324;
extern int DAT_00899e38;
extern int DAT_00899e44;
extern int DAT_0089a228;
extern int DAT_00899d48;
// MATCH: 0x00424920 FUN_00424920
void FUN_00424920(void)

{
  DAT_00899f20 = DAT_00899f20 + DAT_00899a40;
  DAT_00899f74 = DAT_00899f74 + DAT_00899a94;
  DAT_00899f24 = DAT_00899f24 + DAT_00899a44;
  DAT_00899f78 = DAT_00899f78 + DAT_00899a98;
  DAT_00899f28 = DAT_00899f28 + DAT_00899a48;
  DAT_00899f70 = DAT_00899f70 + DAT_00899a90;
  DAT_00899f7c = DAT_00899f7c + DAT_00899a9c;
  DAT_00899e80 = DAT_00899e80 + DAT_008999a0;
  DAT_0089a058 = DAT_0089a058 + DAT_00899b78;
  DAT_0089a0ac = DAT_0089a0ac + DAT_00899bcc;
  DAT_0089a05c = DAT_0089a05c + DAT_00899b7c;
  DAT_0089a0b0 = DAT_0089a0b0 + DAT_00899bd0;
  DAT_0089a060 = DAT_0089a060 + DAT_00899b80;
  DAT_0089a0a8 = DAT_0089a0a8 + DAT_00899bc8;
  DAT_0089a0b4 = DAT_0089a0b4 + DAT_00899bd4;
  DAT_00899fb8 = DAT_00899fb8 + DAT_00899ad8;
  DAT_0089a190 = DAT_0089a190 + DAT_00899cb0;
  DAT_0089a1e4 = DAT_0089a1e4 + DAT_00899d04;
  DAT_0089a194 = DAT_0089a194 + DAT_00899cb4;
  DAT_0089a1e8 = DAT_0089a1e8 + DAT_00899d08;
  DAT_0089a198 = DAT_0089a198 + DAT_00899cb8;
  DAT_0089a1e0 = DAT_0089a1e0 + DAT_00899d00;
  DAT_0089a1ec = DAT_0089a1ec + DAT_00899d0c;
  DAT_0089a0f0 = DAT_0089a0f0 + DAT_00899c10;
  DAT_0089a2c8 = DAT_0089a2c8 + DAT_00899de8;
  DAT_0089a31c = DAT_0089a31c + DAT_00899e3c;
  DAT_0089a2cc = DAT_0089a2cc + DAT_00899dec;
  DAT_0089a320 = DAT_0089a320 + DAT_00899e40;
  DAT_0089a2d0 = DAT_0089a2d0 + DAT_00899df0;
  DAT_0089a318 = DAT_0089a318 + DAT_00899e38;
  DAT_0089a324 = DAT_0089a324 + DAT_00899e44;
  DAT_0089a228 = DAT_0089a228 + DAT_00899d48;
  return;
}

