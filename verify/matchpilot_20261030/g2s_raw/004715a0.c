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

extern undefined1 DAT_0086cbc4;
extern undefined4 DAT_0086cbcc;
extern undefined4 DAT_0086ed20;
int FUN_0047ce40(int p0);
// MATCH: 0x004715a0 FUN_004715a0
void FUN_004715a0(void)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  int local_14;
  int *local_10;
  int local_c;
  int *local_4;
  
  local_4 = &DAT_0086ed20;
  do {
    local_14 = 0;
    local_4[0x41] = 0;
    if ((char)local_4[-0x10] != '\0') {
      if (0 < *local_4) {
        piVar2 = local_4 + -8;
        do {
          iVar3 = *piVar2;
          iVar5 = 0;
          if (0 < (int)(&DAT_0086cbcc)[iVar3 * 0x44]) {
            do {
              iVar1 = *(int *)(&DAT_0086cbc4 + iVar3 * 0x110);
              local_4[local_4[0x41] + iVar5 + 1] = iVar1 + iVar5;
              iVar1 = FUN_0047ce40(iVar1 + iVar5);
              local_4[local_4[0x41] + iVar5 + 0x21] = iVar1;
              iVar5 = iVar5 + 1;
            } while (iVar5 < (int)(&DAT_0086cbcc)[iVar3 * 0x44]);
          }
          local_4[0x41] = local_4[0x41] + (&DAT_0086cbcc)[iVar3 * 0x44];
          local_14 = local_14 + 1;
          piVar2 = piVar2 + 1;
        } while (local_14 < *local_4);
      }
      local_14 = 0;
      if (0 < local_4[0x8fa]) {
        piVar2 = local_4 + 0x4d;
        do {
          local_c = 0;
          if (0 < *piVar2) {
            local_10 = piVar2 + 3;
            piVar6 = piVar2 + 0x103;
            do {
              iVar3 = piVar6[-0x10b];
              iVar5 = (&DAT_0086cbcc)[iVar3 * 0x44];
              iVar1 = 0;
              *piVar6 = 0;
              piVar4 = local_10;
              if (0 < iVar5) {
                do {
                  iVar5 = FUN_0047ce40(*(int *)(&DAT_0086cbc4 + iVar3 * 0x110) + iVar1);
                  *piVar4 = iVar5;
                  *piVar6 = *piVar6 + 1;
                  iVar1 = iVar1 + 1;
                  piVar4 = piVar4 + 1;
                } while (iVar1 < (int)(&DAT_0086cbcc)[iVar3 * 0x44]);
              }
              local_c = local_c + 1;
              local_10 = local_10 + 0x20;
              piVar6 = piVar6 + 1;
            } while (local_c < *piVar2);
          }
          local_14 = local_14 + 1;
          piVar2 = piVar2 + 0x117;
        } while (local_14 < local_4[0x8fa]);
      }
      iVar3 = 0;
      if (0 < local_4[0x933]) {
        piVar2 = local_4 + 0x901;
        do {
          piVar2[-1] = 1;
          *piVar2 = 0;
          piVar2[-4] = 1;
          iVar3 = iVar3 + 1;
          piVar2 = piVar2 + 7;
        } while (iVar3 < local_4[0x933]);
      }
    }
    local_4 = local_4 + 0x944;
  } while ((int)local_4 < 0x8815a0);
  return;
}

