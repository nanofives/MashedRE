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

extern undefined1 DAT_007f100c;
extern undefined1 DAT_005ce4f8;
extern undefined1 DAT_005cc320;
extern undefined1 DAT_005cd0ec;
extern undefined1 DAT_005ce194;
extern undefined1 DAT_005cd074;
extern undefined1 DAT_005ce4f0;
extern undefined1 DAT_005ce4ec;
extern undefined1 DAT_005cd0fc;
extern undefined1 DAT_005cc32c;
extern float DAT_005d757c;
extern undefined1 DAT_005cc950;
extern undefined1 DAT_005ce4e4;
extern undefined4 DAT_00688828;
extern undefined1 DAT_0068a210;
extern undefined1 DAT_0068a2c0;
extern undefined1 DAT_0068a300;
void FUN_004570a0(void);
int FUN_00467210(int p0);
float10 __fastcall FUN_00472650();
void FUN_004769f0(undefined4 * p0);
void FUN_00476a10(undefined4 * p0);
void FUN_00476a80(undefined4 * p0);
void FUN_00476d00(int p0);
void FUN_00476df0(int p0);
void FUN_004864f0(undefined4 * p0, undefined4 p1, int p2);
void FUN_0048f780(undefined4 * p0, undefined4 p1, undefined4 * p2, float p3, undefined4 p4, undefined4 p5, int p6);
ulonglong FUN_004a2c48(void);
undefined4 FUN_004c3d90(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
void FUN_004c5010(float * p0, float * p1, int p2);
void FUN_004c51a0(float * p0, float * p1, int p2);
void thunk_FUN_004c4680(float * p0, float * p1);
extern int _DAT_007f100c;
extern int _DAT_005cc320;
extern int _DAT_005ce4f8;
extern int _DAT_005cd0ec;
extern int _DAT_005cc32c;
extern int _DAT_005cd074;
extern int _DAT_005ce194;
extern int _DAT_005ce4f0;
extern int _DAT_005cd0fc;
extern int _DAT_005cc950;
extern int _DAT_005ce4e4;
// MATCH: 0x00457100 FUN_00457100
/* [C2 2026-06-03] Per-frame **pool-H update + draw** (64 entries). Cursor `puVar5 = &DAT_00688828`
   (= pool base 0x00688810 + 0x18); slot… */

void FUN_00457100(void)

{
  byte bVar1;
  float fVar2;
  float fVar3;
  byte bVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  undefined4 *puVar8;
  float10 fVar9;
  undefined1 local_104;
  undefined1 local_103;
  undefined1 local_102;
  undefined1 local_101;
  undefined1 local_100;
  undefined1 local_ff;
  undefined1 local_fe;
  undefined1 local_fd;
  undefined1 local_fc;
  undefined1 local_fb;
  undefined1 local_fa;
  undefined1 local_f9;
  float local_f8;
  float local_f4;
  float local_f0;
  int local_ec;
  float local_e8 [13];
  undefined4 local_b4;
  undefined4 local_b0;
  float local_ac;
  float local_a8;
  float local_a4;
  float local_a0;
  float local_9c;
  float local_98;
  float local_94;
  float local_90;
  float local_8c;
  float local_88;
  float local_84;
  undefined4 local_80 [8];
  float local_60;
  float local_5c;
  float local_58;
  undefined1 local_40 [64];
  
  iVar7 = 0;
  local_ec = 0;
  puVar5 = &DAT_00688828;
  do {
    bVar1 = (&DAT_0068a300)[iVar7 >> 3];
    bVar4 = (byte)iVar7 & 7;
    if ((bVar1 & (byte)(1 << bVar4)) == 0) {
LAB_004575cb:
      iVar7 = local_ec + 1;
      puVar5 = puVar5 + 0x1a;
      local_ec = iVar7;
    }
    else {
      fVar2 = _DAT_007f100c + (float)puVar5[-6];
      local_104 = 0x80;
      local_103 = 0x80;
      local_102 = 0x80;
      puVar5[-6] = fVar2;
      fVar3 = _DAT_005cc320;
      local_101 = 0xff;
      fVar2 = fVar2 * _DAT_005ce4f8;
      puVar5[-5] = fVar2;
      if (fVar2 <= fVar3) {
        if (_DAT_005cd0ec <= fVar2) {
          fVar2 = fVar2 * _DAT_005cc32c + _DAT_005cd074;
        }
        else {
          fVar2 = fVar2 * _DAT_005ce194 * _DAT_005cd074;
        }
        puVar5[-1] = fVar2;
        local_e8[0xc] = (float)puVar5[-1];
        puVar6 = puVar5;
        puVar8 = local_80;
        for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
          *puVar8 = *puVar6;
          puVar6 = puVar6 + 1;
          puVar8 = puVar8 + 1;
        }
        local_b4 = local_e8[0xc];
        local_b0 = local_e8[0xc];
        FUN_004c5010(local_80,local_e8 + 0xc,1);
        local_101 = FUN_004a2c48();
        FUN_004769f0(&local_104);
        FUN_00476a10(local_80);
        FUN_00476d00(&DAT_0068a2c0);
        puVar6 = puVar5;
        puVar8 = local_80;
        for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
          *puVar8 = *puVar6;
          puVar6 = puVar6 + 1;
          puVar8 = puVar8 + 1;
        }
        local_ac = local_60 * _DAT_005ce4f0;
        local_a8 = local_5c * _DAT_005ce4f0;
        local_100 = 0xff;
        local_ff = 0xff;
        local_a4 = local_58 * _DAT_005ce4f0;
        local_fe = 0xff;
        local_fd = 0x80;
        local_e8[0] = 0.5;
        local_e8[1] = -0.5;
        local_e8[2] = 0.0;
        local_e8[3] = -0.5;
        local_e8[4] = -0.5;
        local_e8[5] = 0.0;
        local_e8[6] = -0.5;
        local_e8[7] = 0.5;
        local_e8[8] = 0.0;
        local_e8[9] = 0.5;
        local_e8[10] = 0.5;
        local_e8[0xb] = 0.0;
        FUN_004c51a0(local_80,&local_ac,2);
        iVar7 = FUN_00467210(0);
        local_fd = FUN_004a2c48();
        FUN_004c3d90(local_e8,local_e8,4,local_80);
        fVar2 = *(float *)(iVar7 + 0x30) * _DAT_005cd0fc - *(float *)(iVar7 + 0x20);
        fVar3 = *(float *)(iVar7 + 0x38) * _DAT_005cd0fc + *(float *)(iVar7 + 0x24);
        local_a0 = local_e8[0] * _DAT_005cc32c + fVar2 + _DAT_005cc32c;
        local_9c = local_e8[2] * _DAT_005cc32c + fVar3 + _DAT_005cc32c;
        local_98 = local_e8[3] * _DAT_005cc32c + fVar2 + _DAT_005cc32c;
        local_94 = local_e8[5] * _DAT_005cc32c + fVar3 + _DAT_005cc32c;
        local_90 = local_e8[6] * _DAT_005cc32c + fVar2 + _DAT_005cc32c;
        local_8c = local_e8[8] * _DAT_005cc32c + fVar3 + _DAT_005cc32c;
        local_88 = local_e8[9] * _DAT_005cc32c + fVar2 + _DAT_005cc32c;
        local_84 = local_e8[0xb] * _DAT_005cc32c + fVar3 + _DAT_005cc32c;
        FUN_004769f0(&local_100);
        FUN_00476a80(&local_a0);
        FUN_00476a10(local_80);
        FUN_00476d00(&DAT_0068a210);
        FUN_004570a0();
        if ((puVar5[0x12] == 0) && (puVar5[0x10] == 1)) {
          fVar2 = (float)puVar5[0x11] - _DAT_007f100c;
          puVar5[0x11] = fVar2;
          if (fVar2 < DAT_005d757c != (fVar2 == DAT_005d757c)) {
            thunk_FUN_004c4680(local_40,puVar5);
            FUN_004864f0(local_40,0x3f99999a,1);
            puVar5[0x12] = 1;
            puVar5[0x10] = 0;
            puVar5[0x11] = 0x3dcccccd;
          }
        }
        if ((puVar5[0x12] == 1) && ((float)puVar5[-5] < _DAT_005cc950)) {
          local_fc = 0x20;
          local_fb = 0x20;
          local_fa = 0x20;
          local_f9 = 200;
          fVar9 = (float10)FUN_00472650(0,0x3f800000);
          if ((float10)_DAT_005ce4e4 < fVar9) {
            local_f8 = (float)puVar5[0xc];
            local_f4 = (float)puVar5[0xd];
            local_f0 = (float)puVar5[0xe];
            fVar9 = (float10)FUN_00472650(0xbdcccccd,0x3dcccccd);
            local_f8 = (float)(fVar9 + (float10)local_f8);
            fVar9 = (float10)FUN_00472650(0xbdcccccd,0x3dcccccd);
            local_f4 = (float)(fVar9 + (float10)local_f4);
            fVar9 = (float10)FUN_00472650(0xbdcccccd,0x3dcccccd);
            local_f0 = (float)(fVar9 + (float10)local_f0);
            FUN_0048f780(&local_f8,0x3ecccccd,&local_fc,0x40a00000,0x3f000000,0,0);
          }
        }
        goto LAB_004575cb;
      }
      (&DAT_0068a300)[iVar7 >> 3] = ~('\x01' << bVar4) & bVar1;
    }
    if (0x68a227 < (int)puVar5) {
      FUN_00476df0(&DAT_0068a2c0);
      FUN_00476df0(&DAT_0068a210);
      return;
    }
  } while( true );
}

