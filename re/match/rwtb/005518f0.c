// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (68.8% rw9044) to 96.8% (regs 100%, ops 100%): unsigned i < 8 with 1 << i, dictionary lookup with the create call in both arms, interp = FUN_0052ddf0(FUN_0052ddc0(anim), (*anim)+4), refcount release as r = refs - 1; store; test, return FUN_00551a50(ext) ? stream : 0. Release block eax/ecx swap left at 96.8%. */
/* MATCH-NM-5518F0-4CAC10 2026-11-13: 96.8 -> 100% (rw9044): the release block reads anim[5] through a typed struct pointer (CD *cd = anim->cd; r = --cd->ref; if (r) return 0), which gives the original's eax = pointer, ecx = count. The int-offset forms all kept the swap. */
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

extern int DAT_009124f8;
extern undefined4 DAT_007dc770;
extern int DAT_007d3ff8;
extern void * PTR_s_UVAnim_00623df0;
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
int FUN_0052ddc0(int * p0);
undefined4 FUN_0052ddf0(int p0, int p1);
undefined4 FUN_0052de60(undefined4 * p0, int * p1);
int * FUN_00551a50(int * p0);
int * FUN_00551ad0(char * p0);
undefined4 FUN_00551fd0(int p0);
undefined4 FUN_00558cb0(int * p0, undefined4 p1);
undefined4 FUN_00562560(undefined4 p0);
// MATCH: 0x005518f0 FUN_005518f0

typedef int (*memfn18f0)();
typedef struct { char p[0x40]; int ref; } CD18f0;
typedef struct { char p[0x14]; CD18f0 *cd; } AN18f0;
uint FUN_005518f0(uint param_1,undefined4 param_2,uint param_3)
{
  int ext = DAT_009124f8 + param_3;
  uint i;
  int dict;
  int *anim;
  int interp;
  char name[32];
  if (!FUN_004cc5e0(param_1,1,0,0) || !FUN_004cc790(param_1,&param_3,4)) {
    return 0;
  }
  for (i = 0; i < 8; i++) {
    if (param_3 & (1 << i)) {
      if (!FUN_004cbd30((undefined4 *)param_1,(undefined4 *)name,0x20)) {
        return 0;
      }
      dict = FUN_00551fd0((int)&PTR_s_UVAnim_00623df0);
      if (dict) {
        anim = (int *)FUN_00558cb0((int *)dict,(undefined4)name);
        if (anim) {
          *(int *)(anim[5] + 0x40) += 1;
        }
        else {
          anim = FUN_00551ad0(name);
        }
      }
      else {
        anim = FUN_00551ad0(name);
      }
      if (anim == 0) {
        return 0;
      }
      if (param_1 == 0) {
        return 0;
      }
      interp = FUN_0052ddf0(FUN_0052ddc0(anim), *(int *)(*anim + 4));
      ((int *)(ext + 8))[i] = interp;
      if (!interp || !FUN_0052de60((undefined4 *)interp, anim)) {
        CD18f0 *cd = ((AN18f0 *)anim)->cd;
        int r = --cd->ref;
        if (r) {
          return 0;
        }
        (*(memfn18f0 *)(DAT_007d3ff8 + 0x11c))(DAT_007dc770, ((AN18f0 *)anim)->cd);
        FUN_00562560((undefined4)anim);
        return 0;
      }
    }
  }
  return FUN_00551a50((int *)ext) ? param_1 : 0;
}
