// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (61.7% ranked, 57.3% rw9044) to 100%: FUN_004ec130(param_2, param_3 & 0xff), FUN_004d54d0(0x89, &param_2); when param_2 == 0 and bit 8 of param_4 is clear, param_3 = 1 and six setup calls, else param_3 = 0. hdr = param_1 + 0x18, mesh = hdr + 0x40: device slot +0x1a0 with hdr+8 when it differs from DAT_006181d8, FUN_004cb190(hdr + 0x10, hdr+0x30), device slot +0x15c with hdr+0x34 when it differs from DAT_006181cc (also DAT_006181c8 = -1). Per mesh (hdr+4 of them, stride 0x24): FUN_004d7100(1 or 0) written as two calls, then FUN_00540510 when param_3 != 0, else optional FUN_004d55b0 and a switch on ext+0x30 (ext at material + DAT_007dc5dc, 0 when NULL): 1/3 FUN_00541d40, 2 FUN_005412d0, 4/6 FUN_005405c0, else FUN_00540340. COM calls typed __stdcall. Loading ext through mesh+8 before mat gives the [ecx+edx] operand order. */
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

extern int DAT_006181d8;
extern int * DAT_007d4110;
extern int DAT_006181cc;
extern undefined4 DAT_006181c8;
extern int DAT_007dc5dc;
void FUN_004cb190(int p0, int p1);
void FUN_004d5480(int p0, int p1);
void FUN_004d54d0(int, int *);
void FUN_004d54f0(int p0, int p1, int p2);
bool FUN_004d55b0(float * p0, byte * p1, uint p2);
undefined4 FUN_004d6e70(int, int);
void FUN_004d7100(int p0);
void FUN_004ec130(int p0, int p1);
void FUN_00540340(int, int, int, int);
void FUN_00540510(int, int);
void FUN_005405c0(int, int, int, int, int);
void FUN_005412d0(int, int, int, int, int, int);
void FUN_00541d40(int, int, int, int, int, int);
// MATCH: 0x00543500 FUN_00543500

typedef int (__stdcall *com3500)();
void FUN_00543500(int param_1,int param_2,uint param_3,uint param_4)
{
  int hdr;
  int mesh;
  int n;
  int *ext;
  int *mat;
  int type;
  FUN_004ec130(param_2,param_3 & 0xff);
  FUN_004d54d0(0x89,&param_2);
  if (param_2 == 0 && (param_4 & 8) == 0) {
    param_3 = 1;
    FUN_004d6e70(0,0);
    FUN_004d5480(0x3c,0xff000000);
    FUN_004d54f0(0,1,3);
    FUN_004d54f0(0,3,3);
    FUN_004d54f0(0,4,3);
    FUN_004d54f0(0,6,3);
  }
  else {
    param_3 = 0;
  }
  hdr = param_1 + 0x18;
  mesh = hdr + 0x40;
  if (*(int *)(hdr + 8) != 0 && DAT_006181d8 != *(int *)(hdr + 8)) {
    DAT_006181d8 = *(int *)(hdr + 8);
    (*(com3500 *)(*DAT_007d4110 + 0x1a0))(DAT_007d4110,*(int *)(hdr + 8));
  }
  FUN_004cb190(hdr + 0x10,*(int *)(hdr + 0x30));
  if (DAT_006181cc != *(int *)(hdr + 0x34)) {
    DAT_006181cc = *(int *)(hdr + 0x34);
    DAT_006181c8 = 0xffffffff;
    (*(com3500 *)(*DAT_007d4110 + 0x15c))(DAT_007d4110,*(int *)(hdr + 0x34));
  }
  for (n = *(int *)(hdr + 4); n != 0; n--) {
    if (*(int *)(mesh + 0xc) != 0 || *(unsigned char *)(*(int *)(mesh + 8) + 7) != 0xff) FUN_004d7100(1);
    else FUN_004d7100(0);
    if (param_3 == 0) {
      if (param_2 != 0) {
        FUN_004d55b0(*(int *)(mesh + 8) + 0xc,*(int *)(mesh + 8) + 4,param_4);
      }
      ext = *(int **)(*(int *)(mesh + 8) + DAT_007dc5dc);
      mat = *(int **)(mesh + 8);
      if (ext == 0) type = 0;
      else type = ext[0xc];
      switch (type) {
      case 1:
        FUN_00541d40(hdr,mesh,param_4,*mat,ext[1],0);
        break;
      case 2:
        FUN_005412d0(hdr,mesh,param_4,0,*mat,ext[1]);
        break;
      case 3:
        FUN_00541d40(hdr,mesh,param_4,*mat,ext[1],ext[7]);
        break;
      case 4:
        FUN_005405c0(hdr,mesh,param_4,*mat,*ext);
        break;
      case 6:
        FUN_005405c0(hdr,mesh,param_4,*mat,ext[6]);
        break;
      default:
        FUN_00540340(hdr,mesh,param_4,*mat);
        break;
      }
    }
    else {
      FUN_00540510(hdr,mesh);
    }
    mesh += 0x24;
  }
}
