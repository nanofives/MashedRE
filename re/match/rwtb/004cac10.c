// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWT-B 2026-11-12: g2s source made to compile, then shaped 75.7 -> 92.6% (rw9044): COM vtable calls typed __stdcall, flag branches with the bit-set arm first, swap-chain Present with its 5 args, light array as 0x6c-byte records, return 0 <= hr. Remaining: the raster-extension pointer (*param_1 + DAT_00911ae4) is formed by add then [eax+0x1c] in the original, folded into [ecx+edx+0x1c] here (6 forms tried, permuter 798 compiles gave no real gain). */
/* MATCH-NM-5518F0-4CAC10 2026-11-13: 92.6 -> 100% (rw9044): the raster extension is a named struct pointer (ext = (RX *)((char *)*param_1 + DAT_00911ae4)) read as ext->sc three times (test, vtable, this), as the COM call macro does. A single load into a local folds the add into [ecx+edx+0x1c]. */
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

extern int DAT_007d4140;
extern int * DAT_007d4110;
extern int DAT_00912114;
extern int *DAT_007d4118;
extern int *DAT_007d4114;
extern int DAT_007d456c;
extern int DAT_007d4570;
extern code *DAT_007d459c;
extern int DAT_00911ae4;
extern undefined4 DAT_009120e0;
undefined4 FUN_004c9ad0(void);
undefined4 FUN_004d1e30(void);
void FUN_004d6200(void);
undefined4 FUN_004db3e0(void);
void FUN_004dc8a0(void);
undefined4 FUN_004dc970(void);
undefined4 FUN_004e0920(void);
typedef int __stdcall scode();
typedef struct { char d[0x68]; int en; } L6c;
typedef struct { char p[0x1c]; int *sc; } RX4cac10;
// MATCH: 0x004cac10 FUN_004cac10

bool FUN_004cac10(int *param_1,undefined4 param_2,byte param_3)
{
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  if (DAT_007d4140 != 0) {
    (**(scode **)(*DAT_007d4110 + 0xa8))(DAT_007d4110);
    DAT_007d4140 = 0;
  }
  if ((param_3 & 1) != 0) {
    if (DAT_00912114 == -0x80000000) {
      DAT_00912114 = 1;
      goto LAB_004cac6c;
    }
  }
  else if (DAT_00912114 == 1) {
    DAT_00912114 = -0x80000000;
LAB_004cac6c:
    FUN_004c9ad0();
    iVar2 = (**(scode **)(*DAT_007d4110 + 0x40))(DAT_007d4110,&DAT_009120e0);
    if (0 <= iVar2) {
      (**(scode **)(*DAT_007d4110 + 0x98))(DAT_007d4110,0,&DAT_007d4118);
      (**(scode **)(*DAT_007d4118 + 8))(DAT_007d4118);
      (**(scode **)(*DAT_007d4110 + 0xa0))(DAT_007d4110,&DAT_007d4114);
      (**(scode **)(*DAT_007d4114 + 8))(DAT_007d4114);
      iVar2 = FUN_004d1e30();
      if (iVar2 != 0) {
        iVar2 = FUN_004dc970();
        if (iVar2 != 0) {
          FUN_004d6200();
          iVar2 = 0;
          if (0 < DAT_007d456c) {
            iVar5 = 0;
            do {
              (**(scode **)(*DAT_007d4110 + 0xcc))(DAT_007d4110,iVar2,(int)&((L6c *)DAT_007d4570)[iVar2]);
              iVar3 = (**(scode **)(*DAT_007d4110 + 0xd4))
                                (DAT_007d4110,iVar2,((L6c *)DAT_007d4570)[iVar2].en);
              iVar2 = iVar2 + 1;
              iVar5 = iVar5 + 0x6c;
            } while (iVar2 < DAT_007d456c);
            if (iVar3 < 0) goto LAB_004cad3d;
          }
          iVar2 = FUN_004db3e0();
          if (iVar2 != 0) {
            FUN_004e0920();
          }
        }
      }
LAB_004cad3d:
      if (DAT_007d459c != (code *)0x0) {
        (*DAT_007d459c)();
      }
    }
  }
  {
  RX4cac10 *ext = (RX4cac10 *)((char *)*param_1 + DAT_00911ae4);
  if (ext->sc) {
    iVar2 = (**(scode **)(*ext->sc + 0xc))(ext->sc,0,0,0,0,0);
  }
  else {
    iVar2 = (**(scode **)(*DAT_007d4110 + 0x44))(DAT_007d4110,0,0,0,0);
  }
  }
  if (iVar2 != -0x7789f798) goto LAB_004cae7f;
  iVar2 = (**(scode **)(*DAT_007d4110 + 0xc))(DAT_007d4110);
  if (iVar2 != -0x7789f797) goto LAB_004cae7f;
  FUN_004c9ad0();
  iVar2 = (**(scode **)(*DAT_007d4110 + 0x40))(DAT_007d4110,&DAT_009120e0);
  if (iVar2 < 0) goto LAB_004cae7f;
  (**(scode **)(*DAT_007d4110 + 0x98))(DAT_007d4110,0,&DAT_007d4118);
  (**(scode **)(*DAT_007d4118 + 8))(DAT_007d4118);
  (**(scode **)(*DAT_007d4110 + 0xa0))(DAT_007d4110,&DAT_007d4114);
  (**(scode **)(*DAT_007d4114 + 8))(DAT_007d4114);
  iVar5 = FUN_004d1e30();
  if (iVar5 != 0) {
    iVar5 = FUN_004dc970();
    if (iVar5 != 0) {
      FUN_004d6200();
      iVar5 = 0;
      if (0 < DAT_007d456c) {
        iVar3 = 0;
        do {
          (**(scode **)(*DAT_007d4110 + 0xcc))(DAT_007d4110,iVar5,(int)&((L6c *)DAT_007d4570)[iVar5]);
          iVar4 = (**(scode **)(*DAT_007d4110 + 0xd4))
                            (DAT_007d4110,iVar5,((L6c *)DAT_007d4570)[iVar5].en);
          iVar5 = iVar5 + 1;
          iVar3 = iVar3 + 0x6c;
        } while (iVar5 < DAT_007d456c);
        if (iVar4 < 0) goto LAB_004cae74;
      }
      iVar5 = FUN_004db3e0();
      if (iVar5 != 0) {
        FUN_004e0920();
      }
    }
  }
LAB_004cae74:
  if (DAT_007d459c != (code *)0x0) {
    (*DAT_007d459c)();
  }
LAB_004cae7f:
  FUN_004dc8a0();
  return 0 <= iVar2;
}
