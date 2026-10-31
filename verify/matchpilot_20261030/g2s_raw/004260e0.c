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

extern undefined4 DAT_008991e0;
extern undefined1 DAT_005cc4dc;
extern char s_ToastArt_005cd4a0[];
extern char s_tracks_005cd498[];
extern undefined1 DAT_005cd4ac;
// MATCH: 0x004260e0 FUN_004260e0
undefined4 * FUN_004260e0(int param_1,char *param_2,char *param_3,char *param_4)

{
  char cVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  int iVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  char *pcVar9;
  undefined2 *puVar10;
  char *pcVar11;
  undefined1 *puVar12;
  
  (*(uchar *)((char *)&DAT_008991e0 + 0)) = 0;
  if (param_1 == 0) {
    pcVar8 = "/";
  }
  else {
    pcVar8 = "\\";
    iVar4 = 0;
    do {
      pcVar5 = &DAT_005cd4ac + iVar4;
      *(char *)((int)&DAT_008991e0 + iVar4) = *pcVar5;
      iVar4 = iVar4 + 1;
    } while (*pcVar5 != '\0');
    puVar10 = (undefined2 *)0x8991df;
    do {
      pcVar5 = (char *)((int)puVar10 + 1);
      puVar10 = (undefined2 *)((int)puVar10 + 1);
    } while (*pcVar5 != '\0');
    *puVar10 = (undefined2)DAT_005cc4dc;
  }
  cVar1 = s_ToastArt_005cd4a0[8];
  uVar3 = (*(uint *)((char *)&s_ToastArt_005cd4a0 + 4));
  pcVar5 = (char *)0x8991df;
  do {
    pcVar11 = pcVar5;
    pcVar5 = pcVar11 + 1;
  } while (pcVar11[1] != '\0');
  *(undefined4 *)(pcVar11 + 1) = (*(uint *)((char *)&s_ToastArt_005cd4a0 + 0));
  *(undefined4 *)(pcVar11 + 5) = uVar3;
  pcVar11[9] = cVar1;
  pcVar5 = pcVar8;
  do {
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  pcVar11 = (char *)0x8991df;
  do {
    pcVar9 = pcVar11 + 1;
    pcVar11 = pcVar11 + 1;
  } while (*pcVar9 != '\0');
  pcVar9 = pcVar8;
  for (uVar6 = (uint)((int)pcVar5 - (int)pcVar8) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar11 = *(undefined4 *)pcVar9;
    pcVar9 = pcVar9 + 4;
    pcVar11 = pcVar11 + 4;
  }
  for (uVar6 = (int)pcVar5 - (int)pcVar8 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar11 = *pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar11 = pcVar11 + 1;
  }
  puVar12 = (undefined1 *)0x8991df;
  do {
    pcVar5 = puVar12 + 1;
    puVar12 = puVar12 + 1;
  } while (*pcVar5 != '\0');
  *puVar12 = (*(uchar *)((char *)&DAT_005cc4dc + 2));
  pcVar5 = pcVar8;
  do {
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  pcVar11 = (char *)0x8991df;
  do {
    pcVar9 = pcVar11 + 1;
    pcVar11 = pcVar11 + 1;
  } while (*pcVar9 != '\0');
  pcVar9 = pcVar8;
  for (uVar6 = (uint)((int)pcVar5 - (int)pcVar8) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar11 = *(undefined4 *)pcVar9;
    pcVar9 = pcVar9 + 4;
    pcVar11 = pcVar11 + 4;
  }
  for (uVar6 = (int)pcVar5 - (int)pcVar8 & 3; cVar1 = s_tracks_005cd498[6],
      uVar2 = (*(ushort *)((char *)&s_tracks_005cd498 + 4)), uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar11 = *pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar11 = pcVar11 + 1;
  }
  pcVar5 = (char *)0x8991df;
  do {
    pcVar11 = pcVar5;
    pcVar5 = pcVar11 + 1;
  } while (pcVar11[1] != '\0');
  *(undefined4 *)(pcVar11 + 1) = (*(uint *)((char *)&s_tracks_005cd498 + 0));
  *(undefined2 *)(pcVar11 + 5) = uVar2;
  pcVar11[7] = cVar1;
  pcVar5 = pcVar8;
  do {
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  pcVar11 = (char *)0x8991df;
  do {
    pcVar9 = pcVar11 + 1;
    pcVar11 = pcVar11 + 1;
  } while (*pcVar9 != '\0');
  pcVar9 = pcVar8;
  for (uVar6 = (uint)((int)pcVar5 - (int)pcVar8) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar11 = *(undefined4 *)pcVar9;
    pcVar9 = pcVar9 + 4;
    pcVar11 = pcVar11 + 4;
  }
  for (uVar6 = (int)pcVar5 - (int)pcVar8 & 3; pcVar5 = param_2, uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar11 = *pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar11 = pcVar11 + 1;
  }
  do {
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  uVar6 = (int)pcVar5 - (int)param_2;
  pcVar5 = (char *)0x8991df;
  do {
    pcVar11 = pcVar5 + 1;
    pcVar5 = pcVar5 + 1;
  } while (*pcVar11 != '\0');
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined4 *)pcVar5 = *(undefined4 *)param_2;
    param_2 = param_2 + 4;
    pcVar5 = pcVar5 + 4;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar5 = *param_2;
    param_2 = param_2 + 1;
    pcVar5 = pcVar5 + 1;
  }
  pcVar5 = param_3;
  pcVar11 = pcVar8;
  if (*param_3 != '\0') {
    do {
      cVar1 = *pcVar11;
      pcVar11 = pcVar11 + 1;
    } while (cVar1 != '\0');
    uVar6 = (int)pcVar11 - (int)pcVar8;
    pcVar11 = (char *)0x8991df;
    do {
      pcVar9 = pcVar11 + 1;
      pcVar11 = pcVar11 + 1;
    } while (*pcVar9 != '\0');
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar11 = *(undefined4 *)pcVar8;
      pcVar8 = pcVar8 + 4;
      pcVar11 = pcVar11 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar11 = *pcVar8;
      pcVar8 = pcVar8 + 1;
      pcVar11 = pcVar11 + 1;
    }
  }
  do {
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  uVar6 = (int)pcVar5 - (int)param_3;
  pcVar8 = (char *)0x8991df;
  do {
    pcVar5 = pcVar8 + 1;
    pcVar8 = pcVar8 + 1;
  } while (*pcVar5 != '\0');
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined4 *)pcVar8 = *(undefined4 *)param_3;
    param_3 = param_3 + 4;
    pcVar8 = pcVar8 + 4;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar8 = *param_3;
    param_3 = param_3 + 1;
    pcVar8 = pcVar8 + 1;
  }
  pcVar8 = param_4;
  if (*param_4 != '\0') {
    do {
      cVar1 = *pcVar8;
      pcVar8 = pcVar8 + 1;
    } while (cVar1 != '\0');
    uVar6 = (int)pcVar8 - (int)param_4;
    pcVar8 = (char *)0x8991df;
    do {
      pcVar5 = pcVar8 + 1;
      pcVar8 = pcVar8 + 1;
    } while (*pcVar5 != '\0');
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar8 = *(undefined4 *)param_4;
      param_4 = param_4 + 4;
      pcVar8 = pcVar8 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar8 = *param_4;
      param_4 = param_4 + 1;
      pcVar8 = pcVar8 + 1;
    }
  }
  return &DAT_008991e0;
}

