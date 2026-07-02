---
rva: 0x00417da0
name_in_ghidra: FUN_00417da0
size_bytes: 1979
slot: Mashed_pool13
date: 2026-07-02
purpose: "exact decomp snapshot for WS-R6 verbatim port"
---

# FUN_00417da0 (0x00417da0) -- mode-8 variant

## Decompilation (mcp__ghidra__decomp_function, verbatim)

```c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-02] Signature: `void FUN_00417da0(undefined4 param_1, int param_2, undefined1
   *param_3)` — identical… */

void FUN_00417da0(undefined4 param_1,int param_2,undefined1 *param_3)

{
  float fVar1;
  bool bVar2;
  float fVar3;
  undefined1 uVar4;
  int iVar5;
  undefined4 uVar6;
  float *pfVar7;
  int iVar8;
  float10 fVar9;
  float10 fVar10;
  float10 extraout_ST0;
  float10 extraout_ST0_00;
  float10 extraout_ST0_01;
  float10 extraout_ST0_02;
  float10 extraout_ST1;
  float10 extraout_ST1_00;
  float10 extraout_ST1_01;
  float10 extraout_ST1_02;
  int local_48;
  float local_40;
  float local_3c;
  float local_38;
  int local_34;
  int local_30;
  int local_2c;
  undefined4 local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  float local_c;

  local_34 = FUN_0040e350();
  FUN_0046d6a0(&local_38,param_2);
  FUN_0046d6d0(&local_3c,param_2);
  FUN_0046d4a0(&local_30,param_2);
  local_1c = *(undefined4 *)(local_30 + 0x30);
  local_18 = *(undefined4 *)(local_30 + 0x38);
  FUN_00443440(param_1,&local_1c,0x41200000,&local_40,0);
  if (_DAT_005cd09c < local_40) {
    local_40 = _DAT_005ccac4 - local_40;
  }
  FUN_004161e0(param_1,&local_24,param_2);
  local_48 = 0;
  iVar5 = FUN_00414570(param_1,&local_24,&local_2c,param_2);
  if ((iVar5 == 0) || (iVar5 = FUN_00416060(&local_1c,&local_2c), iVar5 == 0)) {
    iVar5 = FUN_00415880(param_1,&local_24,&local_2c,param_2);
    if ((iVar5 == 0) ||
       ((iVar5 = FUN_00416060(&local_1c,&local_2c), iVar5 == 0 ||
        (iVar5 = FUN_00415d00(param_2), iVar5 != 0)))) {
      iVar5 = FUN_00414a70(param_1,&local_24,&local_2c,param_2);
      if (iVar5 == 1) {
        iVar5 = FUN_00416060(&local_1c,&local_2c);
        if (iVar5 == 0) goto LAB_00417f07;
LAB_00417f36:
        local_24 = local_2c;
        local_20 = local_28;
        local_48 = 3;
      }
      else {
        if (iVar5 == 2) {
          param_3[4] = 0;
          param_3[5] = 0xff;
          return;
        }
LAB_00417f07:
        iVar5 = FUN_00414c30(param_1,&local_24,&local_2c,param_2);
        if (iVar5 == 1) {
          iVar5 = FUN_00416060(&local_1c,&local_2c);
          if (iVar5 != 0) goto LAB_00417f36;
        }
        else if ((iVar5 == 2) && (iVar5 = FUN_00416060(&local_1c,&local_2c), iVar5 != 0)) {
          local_24 = local_2c;
          local_20 = local_28;
          local_48 = 7;
          goto LAB_00417fb2;
        }
        iVar5 = FUN_004150e0(&local_1c,param_2);
        if (iVar5 == 1) {
          local_48 = 9;
        }
      }
    }
    else {
      local_24 = local_2c;
      local_20 = local_28;
      local_48 = 2;
    }
  }
  else {
    local_24 = local_2c;
    local_20 = local_28;
    local_48 = 1;
  }
LAB_00417fb2:
  iVar5 = FUN_00426c00();
  if ((iVar5 == 0x21) && (iVar5 = FUN_00414f00(param_1,&local_24,&local_2c,param_2), iVar5 != 0)) {
    local_24 = local_2c;
    local_20 = local_28;
    local_48 = 10;
  }
  if ((local_34 == 6) && (iVar5 = FUN_00443080(), iVar5 == 0)) {
    if (local_48 == 0) {
      iVar5 = FUN_00417cf0(param_1,&local_24,&local_2c,param_2);
      if ((iVar5 != 0) && (iVar5 = FUN_00416060(&local_1c,&local_2c), iVar5 != 0)) {
        param_3[5] = 0xff;
        *param_3 = 0;
        param_3[1] = 0;
        return;
      }
      iVar5 = FUN_00415020(param_2);
      if (iVar5 != 0) {
        local_48 = 5;
      }
    }
    if ((&DAT_0088fc88)[param_2 * 0x2d] == 0) {
      (&DAT_0089a51c)[param_2 * 0x1d] = 0;
      (&DAT_0089a520)[param_2 * 0x1d] = 0;
      (&DAT_0089a524)[param_2 * 0x1d] = 0;
    }
    else {
      uVar6 = FUN_004a2c48();
      iVar5 = FUN_00415220(param_1,&local_24,&local_2c,param_2,uVar6);
      if ((iVar5 != 0) && (iVar5 = FUN_00416060(&local_1c,&local_2c), iVar5 != 0)) {
        local_48 = 8;
      }
    }
  }
  iVar5 = param_2 * 0x74;
  *(int *)(&DAT_0089a52c + iVar5) = local_48;
  fVar9 = (float10)FUN_00415e20(param_2,local_24,local_20);
  fVar1 = (float)fVar9;
  fVar10 = (float10)DAT_005d757c;
  if (fVar1 < _DAT_005cd09c) {
    iVar8 = param_2 * 0x14;
    *(undefined4 *)(&DAT_008032d8 + iVar8) = 0x43b40000;
    fVar3 = _DAT_005cc320;
    if (*(float *)(&DAT_008032dc + iVar8) < fVar1) {
      fVar10 = (float10)*(float *)(&DAT_008032dc + iVar8);
    }
    *(float *)(&DAT_008032dc + iVar8) = fVar1;
    if (fVar3 < fVar1) {
      if (((&DAT_0089a4ec)[param_2 * 0x1d] == 1) ||
         (199 < DAT_007f0ff4 - (&DAT_0089a4f0)[param_2 * 0x1d])) {
        uVar4 = FUN_004a2c48();
        *param_3 = uVar4;
        uVar6 = FUN_004a2c48();
        iVar8 = DAT_007f0ff4;
        (&DAT_0089a4f4)[param_2 * 0x1d] = uVar6;
        (&DAT_0089a4ec)[param_2 * 0x1d] = 1;
        (&DAT_0089a4f0)[param_2 * 0x1d] = iVar8;
        fVar10 = extraout_ST0_00;
        fVar9 = extraout_ST1_00;
      }
      else {
        local_2c = 200 - (DAT_007f0ff4 - (&DAT_0089a4f0)[param_2 * 0x1d]);
        uVar4 = FUN_004a2c48();
        param_3[1] = uVar4;
        fVar10 = extraout_ST0;
        fVar9 = extraout_ST1;
      }
    }
  }
  if (_DAT_005cd09c < fVar1) {
    iVar8 = param_2 * 0x14;
    *(undefined4 *)(&DAT_008032dc + iVar8) = 0;
    if (fVar1 < *(float *)(&DAT_008032d8 + iVar8)) {
      fVar10 = (float10)_DAT_005ccac4 - (float10)*(float *)(&DAT_008032d8 + iVar8);
    }
    bVar2 = fVar1 < _DAT_005cd0e4;
    *(float *)(&DAT_008032d8 + iVar8) = fVar1;
    if (bVar2) {
      if (((&DAT_0089a4ec)[param_2 * 0x1d] == 2) ||
         (199 < DAT_007f0ff4 - (&DAT_0089a4f0)[param_2 * 0x1d])) {
        uVar4 = FUN_004a2c48();
        param_3[1] = uVar4;
        uVar6 = FUN_004a2c48();
        iVar8 = DAT_007f0ff4;
        (&DAT_0089a4f4)[param_2 * 0x1d] = uVar6;
        (&DAT_0089a4ec)[param_2 * 0x1d] = 2;
        (&DAT_0089a4f0)[param_2 * 0x1d] = iVar8;
        fVar10 = extraout_ST0_02;
        fVar9 = extraout_ST1_02;
      }
      else {
        local_2c = 200 - (DAT_007f0ff4 - (&DAT_0089a4f0)[param_2 * 0x1d]);
        uVar4 = FUN_004a2c48();
        *param_3 = uVar4;
        fVar10 = extraout_ST0_01;
        fVar9 = extraout_ST1_01;
      }
    }
  }
  param_3[4] = 0xff;
  fVar1 = *(float *)(&DAT_008032e0 + param_2 * 0x14);
  *(float *)(&DAT_008032e0 + param_2 * 0x14) = local_38;
  if ((_DAT_005cc9b0 < local_38 - fVar1) && (_DAT_005cc55c < local_38)) {
    param_3[4] = 0;
    param_3[5] = 0xff;
  }
  if (((float10)_DAT_005ccd6c < fVar10) && (_DAT_005cd0b8 < local_3c)) {
    param_3[4] = 0;
    param_3[5] = 0xff;
  }
  if ((fVar9 < (float10)_DAT_005cd09c) && ((float10)_DAT_005cc72c < fVar9)) {
    param_3[4] = 0xff;
    param_3[5] = 0xff;
    *param_3 = 0xff;
  }
  if (((float10)_DAT_005cd09c < fVar9) && (fVar9 < (float10)_DAT_005cd0e0)) {
    param_3[4] = 0xff;
    param_3[5] = 0xff;
    param_3[1] = 0xff;
  }
  if (local_48 == 7) {
    param_3[4] = 0x40;
    goto LAB_00418530;
  }
  if (local_48 == 5) {
    if (_DAT_005cc35c < *(float *)(&DAT_0089a4e8 + iVar5)) {
      param_3[4] = 0;
    }
    if (_DAT_005cd0a0 < *(float *)(&DAT_0089a4e8 + iVar5)) {
      param_3[5] = 0x40;
    }
    if ((DAT_007f0ff8 / 0x3c & 0x20U) != 0) {
      *param_3 = 0xff;
      goto LAB_00418530;
    }
  }
  else {
    if (local_48 == 9) {
      if (_DAT_005cd0dc < local_3c) {
        param_3[4] = 0;
        param_3[5] = 0xff;
      }
      if (_DAT_005cd0d8 < local_3c) {
        param_3[4] = 0;
      }
      goto LAB_00418530;
    }
    if (local_48 != 2) goto LAB_00418530;
    pfVar7 = (float *)FUN_00408af0(param_2);
    FUN_0046d510(&local_14,param_2);
    fVar1 = local_c * pfVar7[2] + local_14 * *pfVar7 + pfVar7[1] * local_10;
    if (fVar1 < DAT_005d757c) {
      fVar1 = -fVar1;
    }
    if (local_14 * pfVar7[2] - local_c * *pfVar7 < DAT_005d757c) {
      if (fVar1 < _DAT_005cc9c8) {
        param_3[1] = 0;
      }
      if (fVar1 < _DAT_005cc9bc) {
        *param_3 = 0xff;
      }
      goto LAB_00418530;
    }
    if (fVar1 < _DAT_005cc9c8) {
      *param_3 = 0;
    }
    if (_DAT_005cc9bc <= fVar1) goto LAB_00418530;
  }
  param_3[1] = 0xff;
LAB_00418530:
  if ((((local_34 != 6) && (local_34 != 5)) && (local_34 != 9)) &&
     ((local_34 != 10 && (local_34 != 0xb)))) {
    param_3[4] = 0;
    param_3[5] = 0;
  }
  return;
}
```

## Disassembly (mcp__ghidra__listing_disassemble_function, verbatim)

```
00417da0  55                   PUSH EBP
00417da1  8bec                 MOV EBP,ESP
00417da3  83e4f8               AND ESP,0xfffffff8
00417da6  83ec44               SUB ESP,0x44
00417da9  53                   PUSH EBX
00417daa  56                   PUSH ESI
00417dab  57                   PUSH EDI
00417dac  e89f65ffff           CALL 0x0040e350
00417db1  8b750c               MOV ESI,dword ptr [EBP + 0xc]
00417db4  89442424             MOV dword ptr [ESP + 0x24],EAX
00417db8  8d442420             LEA EAX,[ESP + 0x20]
00417dbc  56                   PUSH ESI
00417dbd  50                   PUSH EAX
00417dbe  e8dd580500           CALL 0x0046d6a0
00417dc3  8d4c2424             LEA ECX,[ESP + 0x24]
00417dc7  56                   PUSH ESI
00417dc8  51                   PUSH ECX
00417dc9  e802590500           CALL 0x0046d6d0
00417dce  8d542438             LEA EDX,[ESP + 0x38]
00417dd2  56                   PUSH ESI
00417dd3  52                   PUSH EDX
00417dd4  e8c7560500           CALL 0x0046d4a0
00417dd9  8b442440             MOV EAX,dword ptr [ESP + 0x40]
00417ddd  8b4830               MOV ECX,dword ptr [EAX + 0x30]
00417de0  8b7d08               MOV EDI,dword ptr [EBP + 0x8]
00417de3  894c2454             MOV dword ptr [ESP + 0x54],ECX
00417de7  8b5038               MOV EDX,dword ptr [EAX + 0x38]
00417dea  6a00                 PUSH 0x0
00417dec  8d442434             LEA EAX,[ESP + 0x34]
00417df0  50                   PUSH EAX
00417df1  6800002041           PUSH 0x41200000
00417df6  8d4c2460             LEA ECX,[ESP + 0x60]
00417dfa  51                   PUSH ECX
00417dfb  57                   PUSH EDI
00417dfc  8954246c             MOV dword ptr [ESP + 0x6c],EDX
00417e00  e83bb60200           CALL 0x00443440
00417e05  ddd8                 FSTP ST0
00417e07  d9442444             FLD float ptr [ESP + 0x44]
00417e0b  83c42c               ADD ESP,0x2c
00417e0e  d81d9cd05c00         FCOMP float ptr [0x005cd09c]
00417e14  dfe0                 FNSTSW AX
00417e16  f6c441               TEST AH,0x41
00417e19  750e                 JNZ 0x00417e29
00417e1b  d905c4ca5c00         FLD float ptr [0x005ccac4]
00417e21  d8642418             FSUB float ptr [ESP + 0x18]
00417e25  d95c2418             FSTP float ptr [ESP + 0x18]
00417e29  56                   PUSH ESI
00417e2a  8d542438             LEA EDX,[ESP + 0x38]
00417e2e  52                   PUSH EDX
00417e2f  57                   PUSH EDI
00417e30  e8abe3ffff           CALL 0x004161e0
00417e35  56                   PUSH ESI
00417e36  8d44243c             LEA EAX,[ESP + 0x3c]
00417e3a  50                   PUSH EAX
00417e3b  8d4c2448             LEA ECX,[ESP + 0x48]
00417e3f  51                   PUSH ECX
00417e40  33db                 XOR EBX,EBX
00417e42  57                   PUSH EDI
00417e43  895c242c             MOV dword ptr [ESP + 0x2c],EBX
00417e47  e824c7ffff           CALL 0x00414570
00417e4c  83c41c               ADD ESP,0x1c
00417e4f  85c0                 TEST EAX,EAX
00417e51  7430                 JZ 0x00417e83
00417e53  8d54242c             LEA EDX,[ESP + 0x2c]
00417e57  52                   PUSH EDX
00417e58  8d442440             LEA EAX,[ESP + 0x40]
00417e5c  50                   PUSH EAX
00417e5d  e8fee1ffff           CALL 0x00416060
00417e62  83c408               ADD ESP,0x8
00417e65  85c0                 TEST EAX,EAX
00417e67  741a                 JZ 0x00417e83
00417e69  8b4c242c             MOV ECX,dword ptr [ESP + 0x2c]
00417e6d  8b542430             MOV EDX,dword ptr [ESP + 0x30]
00417e71  894c2434             MOV dword ptr [ESP + 0x34],ECX
00417e75  89542438             MOV dword ptr [ESP + 0x38],EDX
00417e79  bb01000000           MOV EBX,0x1
00417e7e  e92b010000           JMP 0x00417fae
00417e83  56                   PUSH ESI
00417e84  8d442430             LEA EAX,[ESP + 0x30]
00417e88  50                   PUSH EAX
00417e89  8d4c243c             LEA ECX,[ESP + 0x3c]
00417e8d  51                   PUSH ECX
00417e8e  57                   PUSH EDI
00417e8f  e8ecd9ffff           CALL 0x00415880
00417e94  83c410               ADD ESP,0x10
00417e97  85c0                 TEST EAX,EAX
00417e99  743d                 JZ 0x00417ed8
00417e9b  8d54242c             LEA EDX,[ESP + 0x2c]
00417e9f  52                   PUSH EDX
00417ea0  8d442440             LEA EAX,[ESP + 0x40]
00417ea4  50                   PUSH EAX
00417ea5  e8b6e1ffff           CALL 0x00416060
00417eaa  83c408               ADD ESP,0x8
00417ead  85c0                 TEST EAX,EAX
00417eaf  7427                 JZ 0x00417ed8
00417eb1  56                   PUSH ESI
00417eb2  e849deffff           CALL 0x00415d00
00417eb7  83c404               ADD ESP,0x4
00417eba  85c0                 TEST EAX,EAX
00417ebc  751a                 JNZ 0x00417ed8
00417ebe  8b4c242c             MOV ECX,dword ptr [ESP + 0x2c]
00417ec2  8b542430             MOV EDX,dword ptr [ESP + 0x30]
00417ec6  894c2434             MOV dword ptr [ESP + 0x34],ECX
00417eca  89542438             MOV dword ptr [ESP + 0x38],EDX
00417ece  bb02000000           MOV EBX,0x2
00417ed3  e9d6000000           JMP 0x00417fae
00417ed8  56                   PUSH ESI
00417ed9  8d442430             LEA EAX,[ESP + 0x30]
00417edd  50                   PUSH EAX
00417ede  8d4c243c             LEA ECX,[ESP + 0x3c]
00417ee2  51                   PUSH ECX
00417ee3  57                   PUSH EDI
00417ee4  e887cbffff           CALL 0x00414a70
00417ee9  83c410               ADD ESP,0x10
00417eec  83f801               CMP EAX,0x1
00417eef  755c                 JNZ 0x00417f4d
00417ef1  8d54242c             LEA EDX,[ESP + 0x2c]
00417ef5  52                   PUSH EDX
00417ef6  8d442440             LEA EAX,[ESP + 0x40]
00417efa  50                   PUSH EAX
00417efb  e860e1ffff           CALL 0x00416060
00417f00  83c408               ADD ESP,0x8
00417f03  85c0                 TEST EAX,EAX
00417f05  752f                 JNZ 0x00417f36
00417f07  56                   PUSH ESI
00417f08  8d442430             LEA EAX,[ESP + 0x30]
00417f0c  50                   PUSH EAX
00417f0d  8d4c243c             LEA ECX,[ESP + 0x3c]
00417f11  51                   PUSH ECX
00417f12  57                   PUSH EDI
00417f13  e818cdffff           CALL 0x00414c30
00417f18  83c410               ADD ESP,0x10
00417f1b  83f801               CMP EAX,0x1
00417f1e  7544                 JNZ 0x00417f64
00417f20  8d54242c             LEA EDX,[ESP + 0x2c]
00417f24  52                   PUSH EDX
00417f25  8d442440             LEA EAX,[ESP + 0x40]
00417f29  50                   PUSH EAX
00417f2a  e831e1ffff           CALL 0x00416060
00417f2f  83c408               ADD ESP,0x8
00417f32  85c0                 TEST EAX,EAX
00417f34  7460                 JZ 0x00417f96
00417f36  8b4c242c             MOV ECX,dword ptr [ESP + 0x2c]
00417f3a  8b542430             MOV EDX,dword ptr [ESP + 0x30]
00417f3e  894c2434             MOV dword ptr [ESP + 0x34],ECX
00417f42  89542438             MOV dword ptr [ESP + 0x38],EDX
00417f46  bb03000000           MOV EBX,0x3
00417f4b  eb61                 JMP 0x00417fae
00417f4d  83f802               CMP EAX,0x2
00417f50  75b5                 JNZ 0x00417f07
00417f52  8b4510               MOV EAX,dword ptr [EBP + 0x10]
00417f55  c6400400             MOV byte ptr [EAX + 0x4],0x0
00417f59  c64005ff             MOV byte ptr [EAX + 0x5],0xff
00417f5d  5f                   POP EDI
00417f5e  5e                   POP ESI
00417f5f  5b                   POP EBX
00417f60  8be5                 MOV ESP,EBP
00417f62  5d                   POP EBP
00417f63  c3                   RET
00417f64  83f802               CMP EAX,0x2
00417f67  752d                 JNZ 0x00417f96
00417f69  8d44242c             LEA EAX,[ESP + 0x2c]
00417f6d  50                   PUSH EAX
00417f6e  8d4c2440             LEA ECX,[ESP + 0x40]
00417f72  51                   PUSH ECX
00417f73  e8e8e0ffff           CALL 0x00416060
00417f78  83c408               ADD ESP,0x8
00417f7b  85c0                 TEST EAX,EAX
00417f7d  7417                 JZ 0x00417f96
00417f7f  8b54242c             MOV EDX,dword ptr [ESP + 0x2c]
00417f83  8b442430             MOV EAX,dword ptr [ESP + 0x30]
00417f87  89542434             MOV dword ptr [ESP + 0x34],EDX
00417f8b  89442438             MOV dword ptr [ESP + 0x38],EAX
00417f8f  bb07000000           MOV EBX,0x7
00417f94  eb18                 JMP 0x00417fae
00417f96  8d4c243c             LEA ECX,[ESP + 0x3c]
00417f9a  56                   PUSH ESI
00417f9b  51                   PUSH ECX
00417f9c  e83fd1ffff           CALL 0x004150e0
00417fa1  83c408               ADD ESP,0x8
00417fa4  83f801               CMP EAX,0x1
00417fa7  7509                 JNZ 0x00417fb2
00417fa9  bb09000000           MOV EBX,0x9
00417fae  895c2410             MOV dword ptr [ESP + 0x10],EBX
00417fb2  e849ec0000           CALL 0x00426c00
00417fb7  83f821               CMP EAX,0x21
00417fba  7531                 JNZ 0x00417fed
00417fbc  56                   PUSH ESI
00417fbd  8d542430             LEA EDX,[ESP + 0x30]
00417fc1  52                   PUSH EDX
00417fc2  8d44243c             LEA EAX,[ESP + 0x3c]
00417fc6  50                   PUSH EAX
00417fc7  57                   PUSH EDI
00417fc8  e833cfffff           CALL 0x00414f00
00417fcd  83c410               ADD ESP,0x10
00417fd0  85c0                 TEST EAX,EAX
00417fd2  7419                 JZ 0x00417fed
00417fd4  8b4c242c             MOV ECX,dword ptr [ESP + 0x2c]
00417fd8  8b542430             MOV EDX,dword ptr [ESP + 0x30]
00417fdc  bb0a000000           MOV EBX,0xa
00417fe1  894c2434             MOV dword ptr [ESP + 0x34],ECX
00417fe5  89542438             MOV dword ptr [ESP + 0x38],EDX
00417fe9  895c2410             MOV dword ptr [ESP + 0x10],EBX
00417fed  837c242406           CMP dword ptr [ESP + 0x24],0x6
00417ff2  0f85d4000000         JNZ 0x004180cc
00417ff8  e883b00200           CALL 0x00443080
00417ffd  85c0                 TEST EAX,EAX
00417fff  0f85c7000000         JNZ 0x004180cc
00418005  85db                 TEST EBX,EBX
00418007  7556                 JNZ 0x0041805f
00418009  56                   PUSH ESI
0041800a  8d442430             LEA EAX,[ESP + 0x30]
0041800e  50                   PUSH EAX
0041800f  8d4c243c             LEA ECX,[ESP + 0x3c]
00418013  51                   PUSH ECX
00418014  57                   PUSH EDI
00418015  e8d6fcffff           CALL 0x00417cf0
0041801a  83c410               ADD ESP,0x10
0041801d  85c0                 TEST EAX,EAX
0041801f  7429                 JZ 0x0041804a
00418021  8d54242c             LEA EDX,[ESP + 0x2c]
00418025  52                   PUSH EDX
00418026  8d442440             LEA EAX,[ESP + 0x40]
0041802a  50                   PUSH EAX
0041802b  e830e0ffff           CALL 0x00416060
00418030  83c408               ADD ESP,0x8
00418033  85c0                 TEST EAX,EAX
00418035  7413                 JZ 0x0041804a
00418037  8b4510               MOV EAX,dword ptr [EBP + 0x10]
0041803a  c64005ff             MOV byte ptr [EAX + 0x5],0xff
0041803e  8818                 MOV byte ptr [EAX],BL
00418040  885801               MOV byte ptr [EAX + 0x1],BL
00418043  5f                   POP EDI
00418044  5e                   POP ESI
00418045  5b                   POP EBX
00418046  8be5                 MOV ESP,EBP
00418048  5d                   POP EBP
00418049  c3                   RET
0041804a  56                   PUSH ESI
0041804b  e8d0cfffff           CALL 0x00415020
00418050  83c404               ADD ESP,0x4
00418053  85c0                 TEST EAX,EAX
00418055  7408                 JZ 0x0041805f
00418057  c744241005000000     MOV dword ptr [ESP + 0x10],0x5
0041805f  8bd6                 MOV EDX,ESI
00418061  69d2b4000000         IMUL EDX,EDX,0xb4
00418067  8b8288fc8800         MOV EAX,dword ptr [EDX + 0x88fc88]
0041806d  33c9                 XOR ECX,ECX
0041806f  3bc1                 CMP EAX,ECX
00418071  7442                 JZ 0x004180b5
00418073  d9442418             FLD float ptr [ESP + 0x18]
00418077  e8ccab0800           CALL 0x004a2c48
0041807c  50                   PUSH EAX
0041807d  56                   PUSH ESI
0041807e  8d442434             LEA EAX,[ESP + 0x34]
00418082  50                   PUSH EAX
00418083  8d4c2440             LEA ECX,[ESP + 0x40]
00418087  51                   PUSH ECX
00418088  57                   PUSH EDI
00418089  e892d1ffff           CALL 0x00415220
0041808e  83c414               ADD ESP,0x14
00418091  85c0                 TEST EAX,EAX
00418093  7437                 JZ 0x004180cc
00418095  8d54242c             LEA EDX,[ESP + 0x2c]
00418099  52                   PUSH EDX
0041809a  8d442440             LEA EAX,[ESP + 0x40]
0041809e  50                   PUSH EAX
0041809f  e8bcdfffff           CALL 0x00416060
004180a4  83c408               ADD ESP,0x8
004180a7  85c0                 TEST EAX,EAX
004180a9  7421                 JZ 0x004180cc
004180ab  c744241008000000     MOV dword ptr [ESP + 0x10],0x8
004180b3  eb17                 JMP 0x004180cc
004180b5  8bc6                 MOV EAX,ESI
004180b7  6bc074               IMUL EAX,EAX,0x74
004180ba  89881ca58900         MOV dword ptr [EAX + 0x89a51c],ECX
004180c0  898820a58900         MOV dword ptr [EAX + 0x89a520],ECX
004180c6  898824a58900         MOV dword ptr [EAX + 0x89a524],ECX
004180cc  8b542438             MOV EDX,dword ptr [ESP + 0x38]
004180d0  8b442434             MOV EAX,dword ptr [ESP + 0x34]
004180d4  8b4c2410             MOV ECX,dword ptr [ESP + 0x10]
004180d8  8bfe                 MOV EDI,ESI
004180da  6bff74               IMUL EDI,EDI,0x74
004180dd  52                   PUSH EDX
004180de  50                   PUSH EAX
004180df  56                   PUSH ESI
004180e0  898f2ca58900         MOV dword ptr [EDI + 0x89a52c],ECX
004180e6  e835ddffff           CALL 0x00415e20
004180eb  8b5d10               MOV EBX,dword ptr [EBP + 0x10]
004180ee  d9542420             FST float ptr [ESP + 0x20]
004180f2  83c40c               ADD ESP,0xc
004180f5  d9057c755d00         FLD float ptr [0x005d757c]
004180fb  d9442414             FLD float ptr [ESP + 0x14]
004180ff  d81d9cd05c00         FCOMP float ptr [0x005cd09c]
00418105  dfe0                 FNSTSW AX
00418107  f6c405               TEST AH,0x5
0041810a  0f8afa000000         JP 0x0041820a
00418110  8d0cb6               LEA ECX,[ESI + ESI*0x4]
00418113  c1e102               SHL ECX,0x2
00418116  d981dc328000         FLD float ptr [ECX + 0x8032dc]
0041811c  c781d83280000000b443 MOV dword ptr [ECX + 0x8032d8],0x43b40000
00418126  d85c2414             FCOMP float ptr [ESP + 0x14]
0041812a  dfe0                 FNSTSW AX
0041812c  f6c405               TEST AH,0x5
0041812f  7a08                 JP 0x00418139
00418131  ddd8                 FSTP ST0
00418133  d981dc328000         FLD float ptr [ECX + 0x8032dc]
00418139  d9442414             FLD float ptr [ESP + 0x14]
0041813d  8b542414             MOV EDX,dword ptr [ESP + 0x14]
00418141  d81d20c35c00         FCOMP float ptr [0x005cc320]
00418147  8991dc328000         MOV dword ptr [ECX + 0x8032dc],EDX
0041814d  dfe0                 FNSTSW AX
0041814f  f6c441               TEST AH,0x41
00418152  0f85b2000000         JNZ 0x0041820a
00418158  83bfeca4890001       CMP dword ptr [EDI + 0x89a4ec],0x1
0041815f  7437                 JZ 0x00418198
00418161  a1f40f7f00           MOV EAX,[0x007f0ff4]
00418166  2b87f0a48900         SUB EAX,dword ptr [EDI + 0x89a4f0]
0041816c  3dc8000000           CMP EAX,0xc8
00418171  7d25                 JGE 0x00418198
00418173  b9c8000000           MOV ECX,0xc8
00418178  2bc8                 SUB ECX,EAX
0041817a  894c242c             MOV dword ptr [ESP + 0x2c],ECX
0041817e  db44242c             FILD dword ptr [ESP + 0x2c]
00418182  d80decd05c00         FMUL float ptr [0x005cd0ec]
00418188  da8ff4a48900         FIMUL dword ptr [EDI + 0x89a4f4]
0041818e  e8b5aa0800           CALL 0x004a2c48
00418193  884301               MOV byte ptr [EBX + 0x1],AL
00418196  eb72                 JMP 0x0041820a
00418198  d9442414             FLD float ptr [ESP + 0x14]
0041819c  8b442410             MOV EAX,dword ptr [ESP + 0x10]
004181a0  85c0                 TEST EAX,EAX
004181a2  d84c241c             FMUL float ptr [ESP + 0x1c]
004181a6  d80de8d05c00         FMUL float ptr [0x005cd0e8]
004181ac  751d                 JNZ 0x004181cb
004181ae  d9442418             FLD float ptr [ESP + 0x18]
004181b2  d81d6ccd5c00         FCOMP float ptr [0x005ccd6c]
004181b8  dfe0                 FNSTSW AX
004181ba  f6c441               TEST AH,0x41
004181bd  750c                 JNZ 0x004181cb
004181bf  d9442418             FLD float ptr [ESP + 0x18]
004181c3  d80da0c95c00         FMUL float ptr [0x005cc9a0]
004181c9  dec9                 FMULP
004181cb  d8154cd05c00         FCOM float ptr [0x005cd04c]
004181d1  dfe0                 FNSTSW AX
004181d3  f6c441               TEST AH,0x41
004181d6  7508                 JNZ 0x004181e0
004181d8  ddd8                 FSTP ST0
004181da  d9054cd05c00         FLD float ptr [0x005cd04c]
004181e0  d9c0                 FLD ST0
004181e2  e861aa0800           CALL 0x004a2c48
004181e7  8803                 MOV byte ptr [EBX],AL
004181e9  e85aaa0800           CALL 0x004a2c48
004181ee  8b15f40f7f00         MOV EDX,dword ptr [0x007f0ff4]
004181f4  8987f4a48900         MOV dword ptr [EDI + 0x89a4f4],EAX
004181fa  c787eca4890001000000 MOV dword ptr [EDI + 0x89a4ec],0x1
00418204  8997f0a48900         MOV dword ptr [EDI + 0x89a4f0],EDX
0041820a  d9442414             FLD float ptr [ESP + 0x14]
0041820e  d81d9cd05c00         FCOMP float ptr [0x005cd09c]
00418214  dfe0                 FNSTSW AX
00418216  f6c441               TEST AH,0x41
00418219  0f8506010000         JNZ 0x00418325
0041821f  8d0cb6               LEA ECX,[ESI + ESI*0x4]
00418222  c1e102               SHL ECX,0x2
00418225  d981d8328000         FLD float ptr [ECX + 0x8032d8]
0041822b  c781dc32800000000000 MOV dword ptr [ECX + 0x8032dc],0x0
00418235  d85c2414             FCOMP float ptr [ESP + 0x14]
00418239  dfe0                 FNSTSW AX
0041823b  f6c441               TEST AH,0x41
0041823e  750e                 JNZ 0x0041824e
00418240  ddd8                 FSTP ST0
00418242  d905c4ca5c00         FLD float ptr [0x005ccac4]
00418248  d8a1d8328000         FSUB float ptr [ECX + 0x8032d8]
0041824e  d9442414             FLD float ptr [ESP + 0x14]
00418252  8b442414             MOV EAX,dword ptr [ESP + 0x14]
00418256  d81de4d05c00         FCOMP float ptr [0x005cd0e4]
0041825c  8981d8328000         MOV dword ptr [ECX + 0x8032d8],EAX
00418262  dfe0                 FNSTSW AX
00418264  f6c405               TEST AH,0x5
00418267  0f8ab8000000         JP 0x00418325
0041826d  83bfeca4890002       CMP dword ptr [EDI + 0x89a4ec],0x2
00418274  7436                 JZ 0x004182ac
00418276  a1f40f7f00           MOV EAX,[0x007f0ff4]
0041827b  2b87f0a48900         SUB EAX,dword ptr [EDI + 0x89a4f0]
00418281  3dc8000000           CMP EAX,0xc8
00418286  7d24                 JGE 0x004182ac
00418288  b9c8000000           MOV ECX,0xc8
0041828d  2bc8                 SUB ECX,EAX
0041828f  894c242c             MOV dword ptr [ESP + 0x2c],ECX
00418293  db44242c             FILD dword ptr [ESP + 0x2c]
00418297  d80decd05c00         FMUL float ptr [0x005cd0ec]
0041829d  da8ff4a48900         FIMUL dword ptr [EDI + 0x89a4f4]
004182a3  e8a0a90800           CALL 0x004a2c48
004182a8  8803                 MOV byte ptr [EBX],AL
004182aa  eb79                 JMP 0x00418325
004182ac  d905c4ca5c00         FLD float ptr [0x005ccac4]
004182b2  8b442410             MOV EAX,dword ptr [ESP + 0x10]
004182b6  85c0                 TEST EAX,EAX
004182b8  d8642414             FSUB float ptr [ESP + 0x14]
004182bc  d84c241c             FMUL float ptr [ESP + 0x1c]
004182c0  d80de8d05c00         FMUL float ptr [0x005cd0e8]
004182c6  751d                 JNZ 0x004182e5
004182c8  d9442418             FLD float ptr [ESP + 0x18]
004182cc  d81d6ccd5c00         FCOMP float ptr [0x005ccd6c]
004182d2  dfe0                 FNSTSW AX
004182d4  f6c441               TEST AH,0x41
004182d7  750c                 JNZ 0x004182e5
004182d9  d9442418             FLD float ptr [ESP + 0x18]
004182dd  d80da0c95c00         FMUL float ptr [0x005cc9a0]
004182e3  dec9                 FMULP
004182e5  d8154cd05c00         FCOM float ptr [0x005cd04c]
004182eb  dfe0                 FNSTSW AX
004182ed  f6c441               TEST AH,0x41
004182f0  7508                 JNZ 0x004182fa
004182f2  ddd8                 FSTP ST0
004182f4  d9054cd05c00         FLD float ptr [0x005cd04c]
004182fa  d9c0                 FLD ST0
004182fc  e847a90800           CALL 0x004a2c48
00418301  884301               MOV byte ptr [EBX + 0x1],AL
00418304  e83fa90800           CALL 0x004a2c48
00418309  8b15f40f7f00         MOV EDX,dword ptr [0x007f0ff4]
0041830f  8987f4a48900         MOV dword ptr [EDI + 0x89a4f4],EAX
00418315  c787eca4890002000000 MOV dword ptr [EDI + 0x89a4ec],0x2
0041831f  8997f0a48900         MOV dword ptr [EDI + 0x89a4f0],EDX
00418325  d9442420             FLD float ptr [ESP + 0x20]
00418329  8d04b6               LEA EAX,[ESI + ESI*0x4]
0041832c  c64304ff             MOV byte ptr [EBX + 0x4],0xff
00418330  d82485e0328000       FSUB float ptr [EAX*0x4 + 0x8032e0]
00418337  d9442420             FLD float ptr [ESP + 0x20]
0041833b  d91c85e0328000       FSTP float ptr [EAX*0x4 + 0x8032e0]
00418342  d81db0c95c00         FCOMP float ptr [0x005cc9b0]
00418348  dfe0                 FNSTSW AX
0041834a  f6c441               TEST AH,0x41
0041834d  7519                 JNZ 0x00418368
0041834f  d9442420             FLD float ptr [ESP + 0x20]
00418353  d81d5cc55c00         FCOMP float ptr [0x005cc55c]
00418359  dfe0                 FNSTSW AX
0041835b  f6c441               TEST AH,0x41
0041835e  7508                 JNZ 0x00418368
00418360  c6430400             MOV byte ptr [EBX + 0x4],0x0
00418364  c64305ff             MOV byte ptr [EBX + 0x5],0xff
00418368  d81d6ccd5c00         FCOMP float ptr [0x005ccd6c]
0041836e  dfe0                 FNSTSW AX
00418370  f6c441               TEST AH,0x41
00418373  7519                 JNZ 0x0041838e
00418375  d944241c             FLD float ptr [ESP + 0x1c]
00418379  d81db8d05c00         FCOMP float ptr [0x005cd0b8]
0041837f  dfe0                 FNSTSW AX
00418381  f6c441               TEST AH,0x41
00418384  7508                 JNZ 0x0041838e
00418386  c6430400             MOV byte ptr [EBX + 0x4],0x0
0041838a  c64305ff             MOV byte ptr [EBX + 0x5],0xff
0041838e  d8159cd05c00         FCOM float ptr [0x005cd09c]
00418394  dfe0                 FNSTSW AX
00418396  f6c405               TEST AH,0x5
00418399  7a18                 JP 0x004183b3
0041839b  d8152cc75c00         FCOM float ptr [0x005cc72c]
004183a1  dfe0                 FNSTSW AX
004183a3  f6c441               TEST AH,0x41
004183a6  750b                 JNZ 0x004183b3
004183a8  c64304ff             MOV byte ptr [EBX + 0x4],0xff
004183ac  c64305ff             MOV byte ptr [EBX + 0x5],0xff
004183b0  c603ff               MOV byte ptr [EBX],0xff
004183b3  d8159cd05c00         FCOM float ptr [0x005cd09c]
004183b9  dfe0                 FNSTSW AX
004183bb  f6c441               TEST AH,0x41
004183be  751b                 JNZ 0x004183db
004183c0  d81de0d05c00         FCOMP float ptr [0x005cd0e0]
004183c6  dfe0                 FNSTSW AX
004183c8  f6c405               TEST AH,0x5
004183cb  7a10                 JP 0x004183dd
004183cd  c64304ff             MOV byte ptr [EBX + 0x4],0xff
004183d1  c64305ff             MOV byte ptr [EBX + 0x5],0xff
004183d5  c64301ff             MOV byte ptr [EBX + 0x1],0xff
004183d9  eb02                 JMP 0x004183dd
004183db  ddd8                 FSTP ST0
004183dd  8b442410             MOV EAX,dword ptr [ESP + 0x10]
004183e1  83f807               CMP EAX,0x7
004183e4  7509                 JNZ 0x004183ef
004183e6  c6430440             MOV byte ptr [EBX + 0x4],0x40
004183ea  e941010000           JMP 0x00418530
004183ef  83f805               CMP EAX,0x5
004183f2  7558                 JNZ 0x0041844c
004183f4  d987e8a48900         FLD float ptr [EDI + 0x89a4e8]
004183fa  d81d5cc35c00         FCOMP float ptr [0x005cc35c]
00418400  dfe0                 FNSTSW AX
00418402  f6c441               TEST AH,0x41
00418405  7504                 JNZ 0x0041840b
00418407  c6430400             MOV byte ptr [EBX + 0x4],0x0
0041840b  d987e8a48900         FLD float ptr [EDI + 0x89a4e8]
00418411  d81da0d05c00         FCOMP float ptr [0x005cd0a0]
00418417  dfe0                 FNSTSW AX
00418419  f6c441               TEST AH,0x41
0041841c  7504                 JNZ 0x00418422
0041841e  c6430540             MOV byte ptr [EBX + 0x5],0x40
00418422  8b0df80f7f00         MOV ECX,dword ptr [0x007f0ff8]
00418428  b889888888           MOV EAX,0x88888889
0041842d  f7e9                 IMUL ECX
0041842f  03d1                 ADD EDX,ECX
00418431  c1fa05               SAR EDX,0x5
00418434  8bc2                 MOV EAX,EDX
00418436  c1e81f               SHR EAX,0x1f
00418439  03d0                 ADD EDX,EAX
0041843b  f6c220               TEST DL,0x20
0041843e  0f84e8000000         JZ 0x0041852c
00418444  c603ff               MOV byte ptr [EBX],0xff
00418447  e9e4000000           JMP 0x00418530
0041844c  83f809               CMP EAX,0x9
0041844f  7537                 JNZ 0x00418488
00418451  d944241c             FLD float ptr [ESP + 0x1c]
00418455  d81ddcd05c00         FCOMP float ptr [0x005cd0dc]
0041845b  dfe0                 FNSTSW AX
0041845d  f6c441               TEST AH,0x41
00418460  7508                 JNZ 0x0041846a
00418462  c6430400             MOV byte ptr [EBX + 0x4],0x0
00418466  c64305ff             MOV byte ptr [EBX + 0x5],0xff
0041846a  d944241c             FLD float ptr [ESP + 0x1c]
0041846e  d81dd8d05c00         FCOMP float ptr [0x005cd0d8]
00418474  dfe0                 FNSTSW AX
00418476  f6c441               TEST AH,0x41
00418479  0f85b1000000         JNZ 0x00418530
0041847f  c6430400             MOV byte ptr [EBX + 0x4],0x0
00418483  e9a8000000           JMP 0x00418530
00418488  83f802               CMP EAX,0x2
0041848b  0f859f000000         JNZ 0x00418530
00418491  56                   PUSH ESI
00418492  e85906ffff           CALL 0x00408af0
00418497  8d4c2448             LEA ECX,[ESP + 0x48]
0041849b  56                   PUSH ESI
0041849c  51                   PUSH ECX
0041849d  8bf8                 MOV EDI,EAX
0041849f  e86c500500           CALL 0x0046d510
004184a4  d94704               FLD float ptr [EDI + 0x4]
004184a7  d84c2454             FMUL float ptr [ESP + 0x54]
004184ab  83c40c               ADD ESP,0xc
004184ae  d9442444             FLD float ptr [ESP + 0x44]
004184b2  d80f                 FMUL float ptr [EDI]
004184b4  dec1                 FADDP
004184b6  d944244c             FLD float ptr [ESP + 0x4c]
004184ba  d84f08               FMUL float ptr [EDI + 0x8]
004184bd  dec1                 FADDP
004184bf  d9542418             FST float ptr [ESP + 0x18]
004184c3  d9442444             FLD float ptr [ESP + 0x44]
004184c7  d84f08               FMUL float ptr [EDI + 0x8]
004184ca  d944244c             FLD float ptr [ESP + 0x4c]
004184ce  d80f                 FMUL float ptr [EDI]
004184d0  dee9                 FSUBP
004184d2  d9c1                 FLD ST1
004184d4  d81d7c755d00         FCOMP float ptr [0x005d757c]
004184da  dfe0                 FNSTSW AX
004184dc  f6c405               TEST AH,0x5
004184df  7a06                 JP 0x004184e7
004184e1  d9c9                 FXCH
004184e3  d9e0                 FCHS
004184e5  d9c9                 FXCH
004184e7  d81d7c755d00         FCOMP float ptr [0x005d757c]
004184ed  dfe0                 FNSTSW AX
004184ef  d815c8c95c00         FCOM float ptr [0x005cc9c8]
004184f5  f6c405               TEST AH,0x5
004184f8  dfe0                 FNSTSW AX
004184fa  7a1b                 JP 0x00418517
004184fc  f6c405               TEST AH,0x5
004184ff  7a04                 JP 0x00418505
00418501  c6430100             MOV byte ptr [EBX + 0x1],0x0
00418505  d81dbcc95c00         FCOMP float ptr [0x005cc9bc]
0041850b  dfe0                 FNSTSW AX
0041850d  f6c405               TEST AH,0x5
00418510  7a1e                 JP 0x00418530
00418512  c603ff               MOV byte ptr [EBX],0xff
00418515  eb19                 JMP 0x00418530
00418517  f6c405               TEST AH,0x5
0041851a  7a03                 JP 0x0041851f
0041851c  c60300               MOV byte ptr [EBX],0x0
0041851f  d81dbcc95c00         FCOMP float ptr [0x005cc9bc]
00418525  dfe0                 FNSTSW AX
00418527  f6c405               TEST AH,0x5
0041852a  7a04                 JP 0x00418530
0041852c  c64301ff             MOV byte ptr [EBX + 0x1],0xff
00418530  8b442424             MOV EAX,dword ptr [ESP + 0x24]
00418534  83f806               CMP EAX,0x6
00418537  741c                 JZ 0x00418555
00418539  83f805               CMP EAX,0x5
0041853c  7417                 JZ 0x00418555
0041853e  83f809               CMP EAX,0x9
00418541  7412                 JZ 0x00418555
00418543  83f80a               CMP EAX,0xa
00418546  740d                 JZ 0x00418555
00418548  83f80b               CMP EAX,0xb
0041854b  7408                 JZ 0x00418555
0041854d  c6430400             MOV byte ptr [EBX + 0x4],0x0
00418551  c6430500             MOV byte ptr [EBX + 0x5],0x0
00418555  5f                   POP EDI
00418556  5e                   POP ESI
00418557  5b                   POP EBX
00418558  8be5                 MOV ESP,EBP
0041855a  5d                   POP EBP
0041855b  c3                   RET
```

## Depth-1 callees (mcp__ghidra__function_callees)

- 0x00408af0 `FUN_00408af0`
- 0x0040e350 `FUN_0040e350`
- 0x00414570 `FUN_00414570`
- 0x00414a70 `FUN_00414a70`
- 0x00414c30 `FUN_00414c30`
- 0x00414f00 `FUN_00414f00`
- 0x00415020 `FUN_00415020`
- 0x004150e0 `FUN_004150e0`
- 0x00415220 `FUN_00415220`
- 0x00415880 `FUN_00415880`
- 0x00415d00 `FUN_00415d00`
- 0x00415e20 `FUN_00415e20`
- 0x00416060 `FUN_00416060`
- 0x004161e0 `FUN_004161e0`
- 0x00417cf0 `FUN_00417cf0`
- 0x00426c00 `FUN_00426c00`
- 0x00443080 `FUN_00443080`
- 0x00443440 `FUN_00443440`
- 0x0046d4a0 `FUN_0046d4a0`
- 0x0046d510 `FUN_0046d510`
- 0x0046d6a0 `FUN_0046d6a0`
- 0x0046d6d0 `FUN_0046d6d0`
- 0x004a2c48 `FUN_004a2c48`
