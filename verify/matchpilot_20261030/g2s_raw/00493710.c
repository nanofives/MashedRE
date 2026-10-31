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

extern undefined1 FUN_00493700;
extern undefined4 DAT_007719d8;
extern undefined4 DAT_007719dc;
extern undefined1 FUN_004936f0;
int FID_conflict__wprintf(char * p0, ...);
void FUN_0045b350(void);
bool FUN_00493600(void);
bool FUN_00493640(void);
undefined4 FUN_004951f0(void);
void FUN_00495270(undefined4 * p0);
void FUN_004963e0(char * p0);
undefined4 FUN_004c2ed0(undefined4 p0, undefined4 p1);
undefined4 FUN_004c2f00(void);
undefined4 FUN_004c2fb0(void);
int FUN_004c3040(void);
undefined4 FUN_004c30b0(int p0);
bool FUN_004c3270(void);
int FUN_004c32b0();
void FUN_004c5c80(undefined4 p0);
void FUN_004c9eb0(uint p0);
void FUN_004c9f50(undefined4 p0);
void FUN_004c9f60(int p0);
void FUN_004cbc60(undefined4 p0);
undefined4 FUN_004cbc70(void);
void FUN_004cbc80(undefined4 p0);
undefined4 FUN_004cbc90(void);
undefined4 FUN_00550350(undefined4 p0);
void FUN_00550390(void);
undefined4 FUN_005584c0(void);
undefined4 FUN_005c9d00(void);
bool thunk_FUN_00495780(void);
// MATCH: 0x00493710 FUN_00493710
undefined4 FUN_00493710(void)

{
  short sVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined1 local_2c [4];
  undefined1 local_28 [16];
  int local_18;
  int local_14;
  
  FUN_005c9d00(local_28);
  uVar2 = FUN_005c9d00(local_28);
  FUN_004963e0("Calling RwEngineInit\n");
  uVar3 = FUN_005c9d00(0x200000);
  iVar4 = FUN_004c32b0(uVar2,uVar3);
  if (iVar4 != 0) {
    FUN_004963e0("Calling RtFSManagerOpen\n");
    iVar4 = FUN_00550350(0xffffffff);
    if (iVar4 != 0) {
      FUN_004963e0("Calling HardwareInstallFileSystem\n");
      thunk_FUN_00495780();
      FUN_00495270(local_2c);
      FUN_004963e0("Calling RenderwareAttachPlugins\n");
      FUN_00493640();
      FUN_004963e0("Calling RwEngineOpen\n");
      iVar4 = FUN_004c30b0(local_2c);
      if (iVar4 == 0) {
        FUN_00550390();
        FUN_004c3270();
        return 0;
      }
      FUN_004963e0("Calling HardwareSetVideoMode\n");
      iVar4 = FUN_004951f0();
      if (iVar4 != 0) {
        FUN_004c9f50(1);
        FUN_004c9eb0(0x3c);
        FUN_004c9f60(1);
        FUN_004963e0("Calling RwEngineStart\n");
        iVar4 = FUN_004c2fb0();
        if (iVar4 != 0) {
          FUN_0045b350();
          FUN_0045b350();
          sVar1 = FUN_004c2f00();
          FUN_004c2ed0(&local_18,(int)sVar1);
          FID_conflict__wprintf("\tUsing video mode %u : %ux%u\n",(int)sVar1,local_18,local_14);
          FID_conflict__wprintf
                    ("At 10 bytes per pixel (16bit Z + 2*32bit FB) there is %d - %d = %d bytes of video memory free\n"
                     ,0x400000,local_18 * local_14 * 0xc,local_18 * local_14 * -10 + 0x400000);
          FUN_00493600();
          FUN_005584c0();
          FUN_004c5c80(0);
          DAT_007719d8 = FUN_004cbc90();
          FUN_004cbc80(FUN_004936f0);
          DAT_007719dc = FUN_004cbc70();
          FUN_004cbc60(FUN_00493700);
          return 1;
        }
      }
      FUN_00550390();
      FUN_004c3040();
      FUN_004c3270();
    }
  }
  return 0;
}

