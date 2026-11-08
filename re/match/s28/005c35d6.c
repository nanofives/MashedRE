// FLAGS: /O1 /GS
/* MATCH-S28 0x005c35d6: 14.3% (regs 24.3%). _input/_vfscanf body (2672 B), CRT flags /O1 /GS. Raw compile-fail made to compile (FILE opaque typedef, __inc as a cdecl FILE* helper, 64-bit multiply instead of __allmul call, local buffer standing in for alloca stack0xfffffe18, wchar_t typedef guarded for C++). Not matchable here: __SEH_prolog frame + the 0x5c35c0 INC helper uses the caller's ebp frame (non-standard convention) + chkstk/alloca. Class: c. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct _iobuf FILE;
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned __int64 undefined8;
typedef unsigned int uint;
typedef unsigned char byte;
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
typedef int code();
#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif
typedef unsigned int size_t;
#ifndef __cplusplus
#define true 1
#endif
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
MGLOBAL(uint, DAT_00616038, 0x00616038);
MGLOBAL(char, DAT_006169e4, 0x006169e4);
MGLOBAL(undefined *, PTR_FUN_006160e0, 0x006160e0);
MGLOBAL(undefined *, PTR_DAT_006169cc, 0x006169cc);
MGLOBAL(size_t, DAT_006169e0, 0x006169e0);
MGLOBAL(undefined1, DAT_005e72d0, 0x005e72d0);
int FID_conflict__ungetc(int p0, FILE * p1);
void FUN_004ad1e0(void);
void __SEH_epilog(void);
void __SEH_prolog(undefined4 p0, int p1);
longlong __stdcall __allmul(uint p0, int p1, uint p2, int p3);
void __chkstk(void);
uint __cdecl __inc(FILE * p1);
void _free(void * p0);
int _isdigit(int p0);
int _isspace(int p0);
int _isxdigit(int p0);
int _mbtowc(wchar_t * p0, char * p1, size_t p2);
void * _memset(void * p0, int p1, size_t p2);
// MATCH: 0x005c35d6 FUN_005c35d6
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Function: __SEH_epilog replaced with injection: EH_epilog3 */
/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* [C1 2026-05-18] Full _input/_vfscanf body. 2,672 bytes. */

int FUN_005c35d6(FILE *param_1,byte *param_2,undefined4 *param_3)

{
  byte bVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  byte bVar5;
  byte bVar6;
  uint uVar7;
  wchar_t *pwVar8;
  byte *pbVar9;
  char *pcVar10;
  char *pcVar11;
  wchar_t *pwVar12;
  uint uVar13;
  byte *pbVar14;
  bool bVar15;
  longlong lVar16;
  uint unaff_retaddr;
  char local_buf_fe18[0x200];
  wchar_t local_1dc [2];
  byte local_1d8;
  undefined1 local_1d7;
  uint local_1d4;
  byte *local_1d0;
  char local_1cc;
  char local_1cb [351];
  uint local_6c;
  wchar_t *local_68;
  undefined4 *local_64;
  undefined8 local_60;
  int local_58;
  char local_53;
  char local_52;
  char local_51;
  char local_50;
  char local_4f;
  char local_4e;
  byte local_4d;
  int local_4c;
  int local_48;
  int local_44;
  byte local_3d;
  int local_3c;
  int local_38;
  int local_34;
  char local_2d;
  uint local_2c;
  int local_28;
  undefined1 *local_24;
  uint local_20;
  undefined1 *local_1c;
  undefined4 uStack_c;
  undefined *local_8;
  
  local_8 = &DAT_005e72d0;
  uStack_c = 0x5c35e5;
  local_20 = DAT_00616038 ^ unaff_retaddr;
  local_24 = (undefined1 *)0x0;
  local_28 = 0;
  local_2c = 0;
  local_2d = '\0';
  local_34 = 0;
  local_38 = 0;
LAB_005c3604:
  do {
    lVar16 = local_60;
    if (*param_2 == 0) {
LAB_005c4043:
      if (local_28 == 1) {
        local_60 = lVar16;
        _free(local_24);
      }
      if (((local_2c == 0xffffffff) && (local_38 == 0)) && (local_2d == '\0')) {
        local_38 = -1;
      }
      return local_38;
    }
    iVar3 = _isspace((uint)*param_2);
    if (iVar3 == 0) {
      if (*param_2 == 0x25) {
        iVar3 = 0;
        local_3c = 0;
        local_3d = 0;
        local_44 = 0;
        local_48 = 0;
        local_4c = 0;
        local_4d = 0;
        local_4e = '\0';
        local_4f = '\0';
        local_50 = '\0';
        local_51 = '\0';
        local_52 = '\0';
        local_53 = '\x01';
        local_58 = 0;
        pbVar14 = param_2;
        do {
          pbVar9 = pbVar14 + 1;
          uVar7 = (uint)*pbVar9;
          iVar4 = _isdigit(uVar7);
          param_2 = pbVar9;
          if (iVar4 == 0) {
            if (uVar7 < 0x4f) {
              if (uVar7 != 0x4e) {
                if (uVar7 == 0x2a) {
                  local_4f = local_4f + '\x01';
                }
                else if (uVar7 != 0x46) {
                  if (uVar7 == 0x49) {
                    bVar6 = pbVar14[2];
                    if ((bVar6 == 0x36) && (pbVar14[3] == 0x34)) {
                      local_58 = local_58 + 1;
                      local_60 = 0;
                      param_2 = pbVar14 + 3;
                    }
                    else if (((bVar6 != 0x33) || (param_2 = pbVar14 + 3, *param_2 != 0x32)) &&
                            ((((param_2 = pbVar9, bVar6 != 100 && (bVar6 != 0x69)) &&
                              (bVar6 != 0x6f)) && ((bVar6 != 0x78 && (bVar6 != 0x58))))))
                    goto LAB_005c3745;
                  }
                  else if (uVar7 == 0x4c) {
                    local_53 = local_53 + '\x01';
                  }
                  else {
LAB_005c3745:
                    local_50 = local_50 + '\x01';
                    param_2 = pbVar9;
                  }
                }
              }
            }
            else if (uVar7 == 0x68) {
              local_53 = local_53 + -1;
              local_52 = local_52 + -1;
            }
            else {
              if (uVar7 == 0x6c) {
                local_53 = local_53 + '\x01';
              }
              else if (uVar7 != 0x77) goto LAB_005c3745;
              local_52 = local_52 + '\x01';
            }
          }
          else {
            local_48 = local_48 + 1;
            iVar3 = (uVar7 - 0x30) + iVar3 * 10;
          }
          pbVar14 = param_2;
        } while (local_50 == '\0');
        if (local_4f == '\0') {
          local_64 = param_3;
          local_68 = (wchar_t *)*param_3;
          param_3 = param_3 + 1;
        }
        pwVar8 = local_68;
        local_50 = '\0';
        if (local_52 == '\0') {
          if ((*param_2 == 0x53) || (*param_2 == 0x43)) {
            local_52 = '\x01';
          }
          else {
            local_52 = -1;
          }
        }
        uVar13 = *param_2 | 0x20;
        local_6c = uVar13;
        local_4c = iVar3;
        uVar7 = local_2c;
        lVar16 = local_60;
        if (uVar13 != 0x6e) {
          if ((uVar13 == 99) || (uVar13 == 0x7b)) {
            local_34 = local_34 + 1;
            uVar7 = __inc(param_1);
            lVar16 = local_60;
          }
          else {
            do {
              local_34 = local_34 + 1;
              uVar7 = __inc(param_1);
              iVar3 = _isspace(uVar7);
              lVar16 = local_60;
            } while (iVar3 != 0);
          }
        }
        local_2c = uVar7;
        uVar7 = local_2c;
        if ((local_48 != 0) && (local_4c == 0)) {
LAB_005c4013:
          if (uVar7 != 0xffffffff) {
            local_60 = lVar16;
            FID_conflict__ungetc(local_2c,param_1);
            lVar16 = local_60;
          }
          goto LAB_005c4043;
        }
        local_60 = lVar16;
        if (uVar13 < 0x70) {
          if (uVar13 == 0x6f) {
LAB_005c3d85:
            if (local_2c == 0x2d) {
              local_4e = '\x01';
            }
            else if (local_2c != 0x2b) goto LAB_005c3db6;
            local_4c = local_4c + -1;
            if ((local_4c == 0) && (local_48 != 0)) {
              local_50 = '\x01';
            }
            else {
              local_34 = local_34 + 1;
              local_2c = __inc(param_1);
            }
            goto LAB_005c3db6;
          }
          if (uVar13 != 99) {
            if (uVar13 == 100) goto LAB_005c3d85;
            if (uVar13 < 0x65) {
LAB_005c3a38:
              if (*param_2 != local_2c) goto LAB_005c4013;
              local_2d = local_2d + -1;
              if (local_4f == '\0') {
                param_3 = local_64;
              }
              goto LAB_005c3fb7;
            }
            if (0x67 < uVar13) {
              if (uVar13 == 0x69) {
                uVar13 = 100;
                goto LAB_005c384f;
              }
              if (uVar13 != 0x6e) goto LAB_005c3a38;
              iVar3 = local_34;
              if (local_4f != '\0') goto LAB_005c3fb7;
              goto LAB_005c3f97;
            }
            pcVar10 = &local_1cc;
            if (local_2c == 0x2d) {
              local_1cc = '-';
              pcVar10 = local_1cb;
LAB_005c3885:
              local_4c = local_4c + -1;
              local_34 = local_34 + 1;
              local_2c = __inc(param_1);
              lVar16 = local_60;
            }
            else if (local_2c == 0x2b) goto LAB_005c3885;
            local_60 = lVar16;
            if ((local_48 == 0) || (0x15d < local_4c)) {
              local_4c = 0x15d;
            }
            while( true ) {
              uVar7 = local_2c;
              iVar3 = _isdigit(local_2c);
              if ((iVar3 == 0) ||
                 (iVar3 = local_4c + -1, bVar15 = local_4c == 0, local_4c = iVar3, bVar15)) break;
              local_44 = local_44 + 1;
              *pcVar10 = (char)uVar7;
              pcVar10 = pcVar10 + 1;
              local_34 = local_34 + 1;
              local_2c = __inc(param_1);
            }
            if ((DAT_006169e4 == (char)uVar7) &&
               (iVar3 = local_4c + -1, bVar15 = local_4c != 0, local_4c = iVar3, bVar15)) {
              local_34 = local_34 + 1;
              uVar7 = __inc(param_1);
              *pcVar10 = DAT_006169e4;
              while( true ) {
                pcVar10 = pcVar10 + 1;
                local_2c = uVar7;
                iVar3 = _isdigit(uVar7);
                if ((iVar3 == 0) ||
                   (iVar3 = local_4c + -1, bVar15 = local_4c == 0, local_4c = iVar3, bVar15)) break;
                local_44 = local_44 + 1;
                *pcVar10 = (char)uVar7;
                local_34 = local_34 + 1;
                uVar7 = __inc(param_1);
              }
            }
            pcVar11 = pcVar10;
            if ((local_44 != 0) &&
               (((uVar7 == 0x65 || (uVar7 == 0x45)) &&
                (iVar3 = local_4c + -1, bVar15 = local_4c != 0, local_4c = iVar3, bVar15)))) {
              *pcVar10 = 'e';
              pcVar11 = pcVar10 + 1;
              local_34 = local_34 + 1;
              uVar7 = __inc(param_1);
              local_2c = uVar7;
              if (uVar7 == 0x2d) {
                *pcVar11 = '-';
                pcVar11 = pcVar10 + 2;
LAB_005c396f:
                bVar15 = local_4c != 0;
                local_4c = local_4c + -1;
                if (bVar15) goto LAB_005c398e;
                local_4c = 0;
              }
              else if (uVar7 == 0x2b) goto LAB_005c396f;
              while( true ) {
                iVar3 = _isdigit(uVar7);
                if ((iVar3 == 0) ||
                   (iVar3 = local_4c + -1, bVar15 = local_4c == 0, local_4c = iVar3, bVar15)) break;
                local_44 = local_44 + 1;
                *pcVar11 = (char)uVar7;
                pcVar11 = pcVar11 + 1;
LAB_005c398e:
                local_34 = local_34 + 1;
                uVar7 = __inc(param_1);
                local_2c = uVar7;
              }
            }
            local_34 = local_34 + -1;
            lVar16 = local_60;
            if (uVar7 != 0xffffffff) {
              FID_conflict__ungetc(uVar7,param_1);
              lVar16 = local_60;
            }
            if (local_44 != 0) {
              if (local_4f == '\0') {
                local_38 = local_38 + 1;
                local_60 = lVar16;
                *pcVar11 = '\0';
                ((void (__cdecl *)(int, wchar_t *, char *))PTR_FUN_006160e0)(local_53 + -1,local_68,&local_1cc);
                lVar16 = local_60;
              }
              goto LAB_005c3fb7;
            }
            goto LAB_005c4043;
          }
          if (local_48 == 0) {
            local_48 = 1;
            local_4c = local_4c + 1;
          }
LAB_005c39ff:
          if ('\0' < local_52) {
            local_51 = '\x01';
          }
LAB_005c3b8a:
          local_34 = local_34 + -1;
          pwVar12 = pwVar8;
          if (local_2c != 0xffffffff) {
            FID_conflict__ungetc(local_2c,param_1);
          }
          while( true ) {
            if ((local_48 != 0) &&
               (iVar3 = local_4c + -1, bVar15 = local_4c == 0, local_4c = iVar3, lVar16 = local_60,
               bVar15)) goto LAB_005c3d48;
            local_34 = local_34 + 1;
            local_2c = __inc(param_1);
            if ((local_2c == 0xffffffff) ||
               ((bVar6 = (byte)local_2c, uVar13 != 99 &&
                (((uVar13 != 0x73 ||
                  (((8 < (int)local_2c && ((int)local_2c < 0xe)) || (local_2c == 0x20)))) &&
                 ((uVar13 != 0x7b ||
                  (uVar13 = local_6c,
                  ((int)(char)(local_24[(int)local_2c >> 3] ^ local_4d) & 1 << (bVar6 & 7)) == 0))))
                )))) break;
            if (local_4f == '\0') {
              if (local_51 == '\0') {
                *(byte *)pwVar8 = bVar6;
                pwVar8 = (wchar_t *)((int)pwVar8 + 1);
                local_68 = pwVar8;
              }
              else {
                local_1d8 = bVar6;
                if ((PTR_DAT_006169cc[(local_2c & 0xff) * 2 + 1] & 0x80) != 0) {
                  local_34 = local_34 + 1;
                  local_1d7 = __inc(param_1);
                }
                _mbtowc(local_1dc,(char *)&local_1d8,DAT_006169e0);
                *pwVar8 = local_1dc[0];
                pwVar8 = pwVar8 + 1;
                local_68 = pwVar8;
              }
            }
            else {
              pwVar12 = (wchar_t *)((int)pwVar12 + 1);
            }
          }
          local_34 = local_34 + -1;
          lVar16 = local_60;
          if (local_2c != 0xffffffff) {
            FID_conflict__ungetc(local_2c,param_1);
            lVar16 = local_60;
          }
LAB_005c3d48:
          if (pwVar12 == pwVar8) goto LAB_005c4043;
          if ((local_4f == '\0') && (local_38 = local_38 + 1, local_6c != 99)) {
            if (local_51 == '\0') {
              *(byte *)local_68 = 0;
            }
            else {
              *local_68 = L'\0';
            }
          }
        }
        else {
          if (uVar13 == 0x70) {
            local_53 = '\x01';
            goto LAB_005c3d85;
          }
          if (uVar13 == 0x73) goto LAB_005c39ff;
          if (uVar13 == 0x75) goto LAB_005c3d85;
          if (uVar13 != 0x78) {
            if (uVar13 != 0x7b) goto LAB_005c3a38;
            if ('\0' < local_52) {
              local_51 = '\x01';
            }
            pbVar9 = param_2 + 1;
            pbVar14 = pbVar9;
            if (*pbVar9 == 0x5e) {
              pbVar14 = param_2 + 2;
              local_4d = 0xff;
            }
            if (local_24 == (undefined1 *)0x0) {
              local_1c = (undefined1 *)local_buf_fe18;
              local_24 = (undefined1 *)local_buf_fe18;
              local_8 = (undefined *)0xffffffff;
            }
            puVar2 = local_24;
            local_1d0 = pbVar14;
            _memset(local_24,0,0x20);
            bVar6 = local_3d;
            if ((local_6c == 0x7b) && (*pbVar14 == 0x5d)) {
              puVar2[0xb] = 0x20;
              pbVar14 = pbVar14 + 1;
              bVar6 = 0x5d;
            }
            while (bVar5 = *pbVar14, bVar5 != 0x5d) {
              if (((bVar5 == 0x2d) && (bVar6 != 0)) && (bVar1 = pbVar14[1], bVar1 != 0x5d)) {
                bVar5 = bVar1;
                if (bVar6 < bVar1) {
                  bVar5 = bVar6;
                  bVar6 = bVar1;
                }
                if (bVar5 <= bVar6) {
                  uVar7 = (uint)bVar5;
                  local_1d4 = (uint)(byte)((bVar6 - bVar5) + 1);
                  do {
                    puVar2[uVar7 >> 3] = puVar2[uVar7 >> 3] | '\x01' << ((byte)uVar7 & 7);
                    uVar7 = uVar7 + 1;
                    local_1d4 = local_1d4 - 1;
                  } while (local_1d4 != 0);
                }
                pbVar14 = pbVar14 + 2;
                bVar6 = 0;
              }
              else {
                local_3d = bVar5;
                puVar2[bVar5 >> 3] = puVar2[bVar5 >> 3] | '\x01' << (bVar5 & 7);
                pbVar14 = pbVar14 + 1;
                bVar6 = local_3d;
              }
            }
            pwVar8 = local_68;
            uVar13 = local_6c;
            param_2 = pbVar9;
            if (local_6c == 0x7b) {
              param_2 = pbVar14;
            }
            goto LAB_005c3b8a;
          }
LAB_005c384f:
          if (local_2c == 0x2d) {
            local_4e = '\x01';
LAB_005c3c83:
            local_4c = local_4c + -1;
            if ((local_4c == 0) && (local_48 != 0)) {
              local_50 = '\x01';
            }
            else {
              local_34 = local_34 + 1;
              local_2c = __inc(param_1);
              lVar16 = local_60;
            }
          }
          else if (local_2c == 0x2b) goto LAB_005c3c83;
          local_60 = lVar16;
          if (local_2c == 0x30) {
            local_34 = local_34 + 1;
            local_2c = __inc(param_1);
            if (((char)local_2c == 'x') || ((char)local_2c == 'X')) {
              local_34 = local_34 + 1;
              local_2c = __inc(param_1);
              if ((local_48 != 0) && (local_4c = local_4c + -2, local_4c < 1)) {
                local_50 = local_50 + '\x01';
              }
              uVar13 = 0x78;
            }
            else {
              local_44 = 1;
              if (uVar13 == 0x78) {
                local_34 = local_34 + -1;
                if (local_2c != 0xffffffff) {
                  FID_conflict__ungetc(local_2c,param_1);
                }
                local_2c = 0x30;
              }
              else {
                if ((local_48 != 0) && (local_4c = local_4c + -1, local_4c == 0)) {
                  local_50 = local_50 + '\x01';
                }
                uVar13 = 0x6f;
              }
            }
          }
LAB_005c3db6:
          uVar7 = local_2c;
          lVar16 = local_60;
          if (local_58 == 0) {
            while (local_50 == '\0') {
              local_60 = lVar16;
              if ((uVar13 == 0x78) || (uVar13 == 0x70)) {
                iVar3 = _isxdigit(uVar7);
                if (iVar3 != 0) {
                  local_3c = local_3c << 4;
                  iVar3 = _isdigit(uVar7);
                  if (iVar3 == 0) {
                    uVar7 = (uVar7 & 0xffffffdf) - 7;
                  }
                  goto LAB_005c3f1c;
                }
LAB_005c3f19:
                local_50 = local_50 + '\x01';
              }
              else {
                iVar3 = _isdigit(uVar7);
                if (iVar3 == 0) goto LAB_005c3f19;
                if (uVar13 == 0x6f) {
                  if (0x37 < (int)uVar7) goto LAB_005c3f19;
                  local_3c = local_3c << 3;
                }
                else {
                  local_3c = local_3c * 10;
                }
              }
LAB_005c3f1c:
              lVar16 = local_60;
              if (local_50 == '\0') {
                local_44 = local_44 + 1;
                local_3c = local_3c + -0x30 + uVar7;
                if ((local_48 == 0) || (local_4c = local_4c + -1, local_4c != 0)) {
                  local_34 = local_34 + 1;
                  uVar7 = __inc(param_1);
                  lVar16 = local_60;
                }
                else {
                  local_50 = '\x01';
                }
              }
              else {
                local_34 = local_34 + -1;
                if (uVar7 != 0xffffffff) {
                  FID_conflict__ungetc(uVar7,param_1);
                  lVar16 = local_60;
                }
              }
            }
            local_2c = uVar7;
            if (local_4e != '\0') {
              local_3c = -local_3c;
            }
          }
          else {
            while (local_50 == '\0') {
              local_60 = lVar16;
              if ((uVar13 == 0x78) || (uVar13 == 0x70)) {
                iVar3 = _isxdigit(uVar7);
                if (iVar3 != 0) {
                  local_60 = local_60 << 4;
                  iVar3 = _isdigit(uVar7);
                  if (iVar3 == 0) {
                    uVar7 = (uVar7 & 0xffffffdf) - 7;
                  }
                  goto LAB_005c3e49;
                }
LAB_005c3e46:
                local_50 = local_50 + '\x01';
              }
              else {
                iVar3 = _isdigit(uVar7);
                if (iVar3 == 0) goto LAB_005c3e46;
                if (uVar13 == 0x6f) {
                  if (0x37 < (int)uVar7) goto LAB_005c3e46;
                  local_60 = local_60 << 3;
                }
                else {
                  lVar16 = local_60 * 10;
                  local_60 = lVar16;
                }
              }
LAB_005c3e49:
              if (local_50 == '\0') {
                local_44 = local_44 + 1;
                if ((local_48 == 0) || (local_4c = local_4c + -1, local_4c != 0)) {
                  local_34 = local_34 + 1;
                  local_60 = local_60 + (int)(uVar7 - 0x30);
                  uVar7 = __inc(param_1);
                  lVar16 = local_60;
                }
                else {
                  local_50 = '\x01';
                  lVar16 = local_60 + (int)(uVar7 - 0x30);
                }
              }
              else {
                local_34 = local_34 + -1;
                lVar16 = local_60;
                if (uVar7 != 0xffffffff) {
                  FID_conflict__ungetc(uVar7,param_1);
                  lVar16 = local_60;
                }
              }
            }
            (*(uint *)((char *)&local_60 + 4)) = (int)((ulonglong)lVar16 >> 0x20);
            (*(uint *)((char *)&local_60 + 0)) = (int)lVar16;
            local_2c = uVar7;
            if (local_4e != '\0') {
              lVar16 = CONCAT44(-((*(uint *)((char *)&local_60 + 4)) + (uint)((int)local_60 != 0)),-(int)local_60);
            }
          }
          if (uVar13 == 0x46) {
            local_44 = 0;
          }
          if (local_44 == 0) goto LAB_005c4043;
          if (local_4f == '\0') {
            local_38 = local_38 + 1;
            iVar3 = local_3c;
            pwVar8 = local_68;
LAB_005c3f97:
            if (local_58 == 0) {
              if (local_53 == '\0') {
                *pwVar8 = (wchar_t)iVar3;
              }
              else {
                *(int *)pwVar8 = iVar3;
              }
            }
            else {
              *(longlong *)pwVar8 = lVar16;
            }
          }
        }
LAB_005c3fb7:
        local_2d = local_2d + '\x01';
        pbVar14 = param_2 + 1;
        local_60 = lVar16;
      }
      else {
        local_34 = local_34 + 1;
        uVar7 = __inc(param_1);
        local_2c = uVar7;
        lVar16 = local_60;
        if (*param_2 != uVar7) goto LAB_005c4013;
        pbVar14 = param_2 + 1;
        if ((PTR_DAT_006169cc[(uVar7 & 0xff) * 2 + 1] & 0x80) != 0) {
          local_34 = local_34 + 1;
          uVar13 = __inc(param_1);
          if (param_2[1] != uVar13) {
            lVar16 = local_60;
            if (uVar13 != 0xffffffff) {
              FID_conflict__ungetc(uVar13,param_1);
              lVar16 = local_60;
            }
            goto LAB_005c4013;
          }
          local_34 = local_34 + -1;
          pbVar14 = param_2 + 2;
        }
      }
      param_2 = pbVar14;
      if ((local_2c == 0xffffffff) &&
         ((lVar16 = local_60, *param_2 != 0x25 || (param_2[1] != 0x6e)))) goto LAB_005c4043;
      goto LAB_005c3604;
    }
    local_34 = local_34 + -1;
    do {
      local_34 = local_34 + 1;
      iVar3 = __inc(param_1);
      iVar4 = _isspace(iVar3);
    } while (iVar4 != 0);
    if (iVar3 != -1) {
      FID_conflict__ungetc(iVar3,param_1);
    }
    do {
      param_2 = param_2 + 1;
      iVar3 = _isspace((uint)*param_2);
    } while (iVar3 != 0);
  } while( true );
}
