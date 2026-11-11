// FLAGS: /O2 /GS
/* TL-MATCH-REST 0x00562ac0 (no hooks.csv row before this slice): raw g2s.py output (decomp_pc.py --port --callees, Mashed_pool, 2026-11-10), body unchanged. Starting point only, not matched; score in verify/tlmatchrest_20261110/scores.tsv. */
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

extern int DAT_007dc5b4;
extern undefined1 LAB_00562fb0;
extern undefined1 FUN_00562ed0;
undefined4 FUN_00538c80(undefined4 p0, undefined4 * p1, undefined4 p2, undefined4 p3);
undefined4 FUN_0053a6c0(undefined4 p0, undefined4 * p1, undefined4 p2, undefined4 p3);
int FUN_0053c5d0(int p0, undefined4 * p1, undefined4 p2, undefined4 p3);
undefined4 FUN_00561ea0(int p0);
undefined4 FUN_00561ec0(int p0);
void FUN_00565cd0(float * p0, float * p1);
void FUN_005c4e20(float * p0, float p1, float * p2, float * p3);
void FUN_005c4fa0(int * p0, undefined2 * p1, ushort * p2, undefined4 * p3);
// MATCH: 0x00562ac0 FUN_00562ac0


int FUN_00562ac0(int param_1,float *param_2,float *param_3,undefined4 param_4,int *param_5)



{

  undefined4 *puVar1;

  undefined4 *puVar2;

  int iVar3;

  int iVar4;

  int iVar5;

  uint uVar6;

  undefined4 uVar7;

  float fVar8;

  float fVar9;

  float fVar10;

  int iVar11;

  uint uVar12;

  uint uStack_190;

  uint uStack_18c;

  uint uStack_188;

  float fStack_184;

  float fStack_180;

  float fStack_17c;

  float fStack_178;

  float fStack_174;

  float fStack_170;

  undefined4 uStack_16c;

  int iStack_168;

  float fStack_164;

  float fStack_160;

  float fStack_15c;

  float fStack_158;

  float fStack_154;

  float fStack_150;

  undefined1 auStack_14c [4];

  undefined1 auStack_148 [60];

  undefined1 auStack_10c [264];

  undefined1 *puStack_4;

  

  param_5[1] = 0;

  param_5[3] = 0;

  switch(*(undefined4 *)(param_1 + 0x44)) {

  case 0:

    uStack_16c = 4;

    fStack_178 = param_3[4];

    fStack_174 = param_3[5];

    fStack_184 = *param_3;

    fStack_170 = param_3[6];

    fStack_17c = param_3[2];

    fStack_180 = param_3[1];

    FUN_00538c80(*(undefined4 *)(param_1 + 0x40),&fStack_184,FUN_00562ed0,param_5);

    return param_1;

  case 1:

    break;

  case 2:

    uStack_16c = 3;

    FUN_00565cd0(&fStack_184,param_3);

    iVar11 = FUN_00561ec0(*(undefined4 *)(*(int *)(param_1 + 0x40) + 0x18));

    param_5[4] = iVar11;

    FUN_0053c5d0(*(undefined4 *)(param_1 + 0x40),&fStack_184,&LAB_00562fb0,param_5);

    param_5[3] = param_5[3] | 1;

    return param_1;

  case 3:

    uStack_16c = 4;

    fVar8 = *param_3 - param_2[0xc];

    fVar10 = param_3[1] - param_2[0xd];

    fVar9 = param_3[2] - param_2[0xe];

    fStack_184 = param_2[2] * fVar9 + fVar8 * *param_2 + param_2[1] * fVar10;

    fStack_180 = param_2[6] * fVar9 + param_2[4] * fVar8 + param_2[5] * fVar10;

    fStack_17c = param_2[10] * fVar9 + param_2[9] * fVar10 + param_2[8] * fVar8;

    fVar8 = param_3[4] - param_2[0xc];

    fVar10 = param_3[5] - param_2[0xd];

    fVar9 = param_3[6] - param_2[0xe];

    fStack_178 = param_2[2] * fVar9 + fVar8 * *param_2 + param_2[1] * fVar10;

    fStack_174 = param_2[6] * fVar9 + param_2[4] * fVar8 + param_2[5] * fVar10;

    fStack_170 = param_2[10] * fVar9 + param_2[9] * fVar10 + param_2[8] * fVar8;

    iVar11 = FUN_00561ec0(*(undefined4 *)(param_1 + 0x40));

    param_5[4] = iVar11;

    FUN_0053a6c0(*(undefined4 *)(param_1 + 0x40),&fStack_184,&LAB_00562fb0,param_5);

    param_5[3] = param_5[3] | 1;

  default:

    return param_1;

  }

  iVar11 = FUN_00561ea0(*(undefined4 *)(param_1 + 0x40));

  param_5[4] = iVar11;

  iStack_168 = *(int *)(param_1 + 0x40);

  iVar11 = *(int *)(iStack_168 + 4);

  iVar3 = *(int *)(iStack_168 + 8);

  iVar4 = *(int *)(iStack_168 + DAT_007dc5b4);

  if (iVar4 != 0) {

    iVar5 = *(int *)(iVar4 + 0x24);

    fStack_158 = param_3[4];

    fStack_154 = param_3[5];

    fStack_150 = param_3[6];

    fStack_164 = *param_3;

    fStack_160 = param_3[1];

    fStack_15c = param_3[2];

    FUN_005c4e20(auStack_148,iVar4,&fStack_164,&fStack_164);

    if (auStack_10c < puStack_4) {

      do {

        if ((uint)param_5[2] <= (uint)param_5[1]) break;

        FUN_005c4fa0(auStack_148,&uStack_188,&uStack_190,auStack_14c);

        uStack_18c = uStack_188;

        uVar12 = uStack_188 & 0xffff;

        if (uVar12 < (uStack_190 & 0xffff) + uVar12) {

          do {

            uVar6 = param_5[1];

            if ((uint)param_5[2] <= uVar6) break;

            if (iVar5 != 0) {

              uVar12 = (uint)*(ushort *)(iVar5 + uVar12 * 2);

            }

            param_5[1] = uVar6 + 1;

            puVar1 = (undefined4 *)(*param_5 + uVar6 * 0x4c);

            puVar2 = (undefined4 *)(iVar3 + (uint)*(ushort *)(iVar11 + uVar12 * 8) * 0xc);

            *puVar1 = *puVar2;

            puVar1[1] = puVar2[1];

            puVar1[2] = puVar2[2];

            puVar2 = (undefined4 *)(iVar3 + (uint)*(ushort *)(iVar11 + 2 + uVar12 * 8) * 0xc);

            puVar1[4] = *puVar2;

            puVar1[5] = puVar2[1];

            puVar1[6] = puVar2[2];

            puVar2 = (undefined4 *)(iVar3 + (uint)*(ushort *)(iVar11 + 4 + uVar12 * 8) * 0xc);

            puVar1[8] = *puVar2;

            puVar1[9] = puVar2[1];

            uVar7 = puVar2[2];

            puVar1[0x11] = uVar12;

            puVar1[10] = uVar7;

            puVar1[0x10] = iStack_168;

            if ((int *)param_5[4] == (int *)0x0) {

              puVar1[0x12] = 0xf;

            }

            else {

              puVar1[0x12] = *(uint *)(*(int *)param_5[4] + (uVar12 >> 3) * 4) >>

                             (sbyte)((uVar12 & 7) << 2) & 0xf;

            }

            uStack_18c = uStack_18c + 1;

            uVar12 = uStack_18c & 0xffff;

          } while (uVar12 < (uStack_188 & 0xffff) + (uStack_190 & 0xffff));

        }

      } while (auStack_10c < puStack_4);

    }

  }

  param_5[3] = param_5[3] | 2;

  return param_1;

}



