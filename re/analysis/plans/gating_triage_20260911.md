U-5552 | NEEDS-EXTERNAL | encoder `((param_1[1]*0x3b + param_1[2]*0xb + *param_1*0x1e)*param_1[3])/0x639c)*0xff` shows the luminance formula but ID 0x51 never appears | librw/criterion-rwg37 pixel-format enum entry for ID 0x51 (not in file)
U-5553 | NEEDS-EXTERNAL | branches `if (iVar4 == 0x1b)`, `else if (iVar4 == 0x34)`, `else if (iVar4 == 0x1c)` confirm packing shapes | RW SDK/librw image-format enum NAMES for 0x1b/0x1c/0x34 (not in file)
U-5302 | NEEDS-XREF | `xrefs (1): UNCONDITIONAL_CALL 0x0054364a in (data)` ÔÇö one call site, in a data region (fn-pointer table) | what references/installs the pointer entry at 0x0054364a (not in file)
U-0007 | PARTIAL | `(**(code **)(*(int *)DAT_00771a0c + 0xe4))(DAT_00771a0c,0x18)` plus `pcStack_38=0x5;uStack_3c=0x19;` and `DAT_00771a0c=(code *)FUN_004caea0()` ÔÇö constants pinpointed | return type of FUN_004caea0 (object type) not in file
U-0010 | NEEDS-XREF | `**(undefined4 **)(param_1 + 0x14) = *(undefined4 *)(param_1 + 0x10);` and `*(int *)(param_1+0x10)+4)=...(param_1+0x14)` confirm +0x8/+0x10/+0x14 accesses | struct type/name; caller FUN_00402a40 (xref 0x00402a6c) not in file
U-0011 | NEEDS-XREF | `*(int *)(DAT_007d4054 + 0x10 + DAT_007d3ff8)` and `(**(code **)(DAT_007d3ff8 + 0x11c))(...(DAT_007d4054+0xc+DAT_007d3ff8),param_1)` shown | writers/types of DAT_007d3ff8 & DAT_007d4054 (reference_to; not in file)
U-0091 | NEEDS-XREF | `FUN_004c2c90(DAT_007d3ff8 + 4,...)`, 0x4b-dword copy to `&DAT_007d3ec8`, `(*DAT_007d3fd4)(puVar1)`, `DAT_007d3ff8[0x49] = 1` ÔÇö all offsets confirmed | struct layout/type; writers of DAT_007d3ff8 (not in file)
U-0125 | NEEDS-RUNTIME | `(**(code **)(DAT_007d3ff8 + 0x108))();` ÔÇö pure indirect JMP; DAT_007d3ff8=0 in static image | live value of *(DAT_007d3ff8)+0x108 at boot (Frida)
U-0191 | NEEDS-EXTERNAL | plate `Library Function - Single Match ___sbh_alloc_block / Visual Studio 2003`; offsets like `param_1[4]`, `piVar5[local_8+0x31]` | MSVC VS2003 CRT small-block-heap region/group struct definitions (not in file)
U-4312 | NEEDS-XREF | `iVar8 = *(int *)(*DAT_00636ac0 + 4);` and `local_48 = DAT_00636ac0[0xd];` / `(float)DAT_00636ac0[0xd]` confirm +4-ptr and index 0xd reads | all WRITES to DAT_00636ac0 for full struct (not in file)
U-4313 | NEEDS-XREF | `local_60 = &DAT_007f1a1c;` then `iVar6 = *local_60; ... local_3c[iVar6]` uses it as an index table; consts are `local_3c[0]=-131075.98;[1]=-522240.0;[2]=-132092.0` | initialization/writers of DAT_007f1a1c (not in file)
U-4388 | PARTIAL | `iVar1 = FUN_004c2d90(0,4,FUN_004d7ff0,FUN_004d7ff0);` ÔÇö the 0 and identical callbacks are shown at the call | purpose of args needs FUN_004c2d90 body (not in file)
U-4391 | PARTIAL | `FUN_004b6520(uVar1,param_1 * param_2);` called after alloc with (ptr,size) | memset-vs-pattern role needs FUN_004b6520 body (not in file)
U-4398 | NEEDS-XREF | `xrefs (2): UNCONDITIONAL_CALL 0x00450a87 in (data), UNCONDITIONAL_CALL 0x00452e5d in (data)` ÔÇö two fn-pointer-table refs exist | what installs/reads those table entries at 0x00450a87 / 0x00452e5d (not in file)
U-4399 | PARTIAL | `uVar1 = FUN_0045bfe0(param_1,&local_40);` then `local_8 = -local_8;` shown literally | what FUN_0045bfe0 writes into local_8 (its body, not in file)
U-4401 | PARTIAL | `param_1[3] = 4.2039e-45;` shown (bit pattern 3) | bug-vs-Ghidra-artifact needs raw listing at 0x00475616 (only decomp in file)
U-4412 | PARTIAL | `if ((param_1 != 0.0)...)` and `uVar1 = *(undefined4 *)((int)param_1 + 4);` both present ÔÇö dual float/pointer use confirmed | true type/EAX resolution needs raw listing at 0x004756e0 (not in file)
U-4413 | PARTIAL | `undefined1 local_40 [64];` filled by in-file `FUN_004752f0(local_40,param_2,param_1)` (writes param_1[0..0xe] as 4x4 matrix) then `FUN_004c1480(uVar1,local_40,0)` | consumer field semantics need FUN_004c1480 (not in file)
U-4414 | PARTIAL | accesses `*param_2`,`param_2[1]`,`param_2[2]`,`param_2[4]`,`param_2[6]`,`param_2[8]`,`param_2[10]` ÔÇö indices consistent with stride-4 rows | row3 / full 4x4 layout not accessed here to verify
U-4415 | PARTIAL | slots 0-4 use `*(undefined4 *)(param_1 + 0x18)`, slots 5-9 use `*(undefined4 *)(param_2 + 0x18)` ÔÇö pattern literally shown | meaning of +0x18/slot semantics needs FUN_00557ec0 (not in file)
U-4416 | STALE | callees + call show `RpClumpForAllAtomics(param_1,FUN_00475830,param_2)` ÔÇö FUN_004e66d0 is the RW per-atomic iterator, confirming callback-dispatch role | -
U-4417 | ANSWERED | in-file FUN_00474db0: `(**(code **)(DAT_007d3ff8 + 0x108))(param_1 * param_2,0)` ÔÇö 2nd arg is multiplied by count param_1, so 0x40/4 is per-element size (not total, not alignment) | -
U-4418 | PARTIAL | `FUN_004770c0(... ,0x80c,param_2,param_1);` ÔÇö the 0x80c literal is shown at the call | bit meaning needs FUN_004770c0 body (not in file)
U-4419 | PARTIAL | stride `puVar4 = puVar4 + 0x14` (0x50B); writes `[0x10]=*param_3;[0x11]=0;[0x12]=1;(ushort)(puVar4+0x13);+0x4e=0`; `while (puVar4[0x12] != 0)` shows [0x12] as in-use flag | "counter"/"type tag" labels for [0x11]/[0x10] not shown
U-4420 | NEEDS-XREF | `fVar2 = DAT_005d757c;` seeds max-search `if (fVar2 < pfVar5[-0x28])`; value not in file (in-file `fVar2 < DAT_005d757c`ÔåÆ"tween is less than 0.0f!!" narrows but not literal) | .rdata value at 0x005d757c (memory_read; not in file)
U-4421 | NEEDS-RUNTIME | `(**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);` on heap ptrs (paired with +0x108 alloc); DAT_007d3ff8=0 static | live target of *(DAT_007d3ff8)+0x10c at boot to confirm free/destructor
U-4423 | NEEDS-XREF | `*(undefined4 *)(puVar1 + 0xc0) = DAT_00613254;` ÔÇö write shown | value/type of DAT_00613254 (memory_read 0x00613254; not in file)
U-4426 | NEEDS-XREF | `unaff_ESI[3] = uVar1; *unaff_ESI = 0; unaff_ESI[1] = 0; unaff_ESI[2] = 0;` ÔÇö ESI is a hidden caller-set ptr | caller FUN_00475f90 (call 0x00476100 / ctx 0x00476011) that sets ESI (not in file)
TOTALS: ANSWERED=1 PARTIAL=11 NEEDS-XREF=10 NEEDS-RUNTIME=2 NEEDS-EXTERNAL=3 STALE=1 IDS=28
U-4427 | PARTIAL | offsets literally read: `*(float *)(param_1 + 0xc4)` (L44), `param_1 + 0xd0` (L47), `*(int *)(param_1 + 0xd4)` (L74), `param_1 + 4` (L56), `param_1 + 0x90` (L80), `param_1 + 0x34` (L111) | decomp shows the accesses but never labels them intensity/speed_scale/flag/wheel/exhaust
U-4428 | PARTIAL | used as comparison thresholds vs fVar2(=+0xc4): `fVar2 < _DAT_0061325c` (L46), `_DAT_00613268 < fVar2` (L82), `fVar2 < _DAT_00613284` (L102) | the actual float values not shown (data addrs not in file)
U-4429 | NEEDS-XREF | `FUN_004762c0(param_1);` (L159) then `fVar2 = *param_1;` (L160) reads back | body of callee FUN_004762c0 not in file ÔÇö decompile FUN_004762c0
U-4430 | NEEDS-XREF | flames/smoke slots `FUN_004770c0(puVar2,0x817,...)` (L202,L209) vs exhaust `FUN_004770c0(&DAT_00692498,0x807,0x40,0)` (L212) | meaning of bit 0x10 needs FUN_004770c0 body (not in file)
U-4500 | NEEDS-XREF | `int unaff_ESI;` used as this: `*(float *)(unaff_ESI + 0x54)` (L247) | whether caller sets ESI as this needs FUN_004777d0 body (caller, not in file)
U-4501 | PARTIAL | `local_8 = local_c;` `local_4 = local_c;` (L254-255), only `FUN_004c13e0(...,&local_c,1)` (L257) passed | whether FUN_004c13e0 reads local_8/local_4 via &local_c not shown (callee absent)
U-4502 | NEEDS-XREF | `uVar1 = FUN_004a2c48();` byte stored via `CONCAT13(uVar1,(int3)uVar2)` (L296-297) | purpose (alpha vs const) needs FUN_004a2c48 body (not in file)
U-4503 | NEEDS-EXTERNAL | `(**(code **)(*(int *)(in_EAX + 0x44) + 0x48))(...)` (L299) vtable slot +0x48 | identity as RpAtomicRender needs gta-reversed/RW3 RpAtomic vtable layout (offset 0x48)
U-4504 | NEEDS-XREF | `FUN_004c0c20(*(undefined4 *)(*(int *)(unaff_ESI + 0x44) + 4));` (L328) | destroy-vs-unlock needs FUN_004c0c20 body (not in file)
U-4506 | NEEDS-XREF | `FUN_004b4150(DAT_00693180,0);` (L365), `FUN_004b4150(DAT_00693184,0);` (L369) | purpose needs FUN_004b4150 body (not in file)
U-4507 | NEEDS-XREF | `FUN_00477450(param_2,param_3,param_4,param_5);` (L459) | param_4/param_5 semantics need FUN_00477450 body (not in file)
U-4508 | ANSWERED | free slot found `puVar1` in walk (L439-445), then `FUN_00477450(param_2,param_3,param_4,param_5)` (L459) ÔÇö puVar1 not passed | (why callee needs no slot ptr ÔåÆ FUN_00477450, but the skip itself is shown)
U-4509 | NEEDS-XREF | `FUN_005495b0(DAT_00693188);` (L496), `FUN_005495b0(DAT_0069318c);` (L498) | geometry-vs-instance needs FUN_005495b0 body (not in file)
U-4510 | NEEDS-XREF | `int unaff_ESI;` indexes `&DAT_00693198 + unaff_ESI * 0x2c0` (L526) | callers set ESI: FUN_00477920 / FUN_00477a10 bodies not in file
U-4512 | NEEDS-XREF | count stored as float: `*(float *)(...0x6931b8...) = (float)(int)(&DAT_00693448)[iVar1 * 0xb0]` (L605-606) | V-coord vs shader-param needs the pool consumer/renderer (reader of &DAT_00693198) not in file
U-4514 | NEEDS-XREF | `DAT_00693190 = FUN_0040bb30(&DAT_005ceb10);` (L644); FUN_0040bb30 elsewhere takes a name `FUN_0040bb30("flames3")` (L199) | resource type + DAT_005ceb10 string value need FUN_0040bb30 body / data read
U-4515 | NEEDS-XREF | cross product `*param_1 = (fVar9 - fVar10) * (fVar5 - fVar6) - ...` (L696-698) | CW/CCW convention needs callers FUN_00422570 / FUN_0047bb10 (not in file)
U-4517 | NEEDS-XREF | 8-corner AABB scan calling `FUN_00477f50(param_1,&local_c,0)` (L752) | frustum-clip vs portal-vis needs caller FUN_0047c1f0 (not in file)
U-4575 | PARTIAL | switch maps param_3 flags to offset pairs: case 1ÔåÆ+0x4c/+0x50, 2ÔåÆ+0x6c/+0x70, 4ÔåÆ+0x64/+0x68, 8ÔåÆ+0x54/+0x58, 0x10ÔåÆ+0x5c/+0x60, 0x20ÔåÆ+0x7c/+0x80, 0x40ÔåÆ+0x74/+0x78, 0x80ÔåÆ+0x84/+0x88, 0x100ÔåÆ+0x8c/+0x90 (L798-838) | "RWS stream descriptor" semantic of those offsets not shown
U-4577 | NEEDS-XREF | `(**(code **)(DAT_007d3ff8 + 0x20))(8,0)` `(6,1)` `(8,1)` (L871-875) | meaning of args 8/6 and 0/1 needs the function at DAT_007d3ff8+0x20 (producer/target not in file)
U-4578 | NEEDS-XREF | `FUN_0048a850(param_1,...,param_6,0x3f800000)` (L900); 0x3f800000=1.0f 7th arg | FUN_0048a850 signature/param semantics need its body (not in file)
U-4580 | NEEDS-XREF | arrays written e.g. `(&DAT_0076ddc8)[param_3 * 0x122 + iVar4] = 0` (L951), `&DAT_0076d9b0 + ...` (L992) | semantic names need consumer FUN_00490500 (not in file)
U-4581 | PARTIAL | `FUN_00472650(0,0x43000000);` (L1099) return not assigned (adjacent calls assign to fVar8, L1102) ÔÇö confirms unused; 0x43000000=128.0 | why/meaning of the discarded call not derivable from decomp
U-4583 | NEEDS-XREF | six blocks passed to `FUN_004770a0(&DAT_0076d900)`ÔÇª`(&DAT_007706d8)` (L1158-1163) | semantic names need consumer FUN_00490500 / FUN_004770a0 (not in file)
U-4584 | NEEDS-XREF | `FUN_00538c80(DAT_0080332c,&local_1c,&LAB_0048fe30,&local_20);` (L1203) | FUN_00538c80 semantics + LAB_0048fe30 callback body not in file
U-4586 | NEEDS-XREF | `uVar3 = 0x33`/`0x34` by param_4 (L1304,L1314) ÔåÆ `FUN_0041f290(param_3,uVar3)` then reads +0x30/+0x34/+0x38 (L1316-1319) | bone-idÔåÆskeleton map needs FUN_0041f290 body (not in file)
U-4600 | PARTIAL | init `DAT_00771538 = 0; DAT_0077153c = 0;` (L1419-1420); consumer FUN_00491590 uses them as random range for pfVar4[-2], 2nd of 3-float XYZ: `FUN_00472650(DAT_00771538,DAT_0077153c)` (L1763) | which physical axis (height vs other) not labeled
U-4601 | PARTIAL | consumer path shown: `fVar6 = FUN_00472650(DAT_00771538,DAT_0077153c); pfVar4[-2] = (float)fVar6;` (L1763-1764) = min/max of middle position component | axis identity (Y/height) not proven
U-4602 | NEEDS-XREF | `iVar9 = FUN_004c1b40(uVar8); if (iVar9 == 0) {respawn}` (L1539-1540) | test type (frustum/sphere/AABB) needs FUN_004c1b40 body (not in file)
U-4603 | NEEDS-XREF | `uVar8 = FUN_004671a0(0,0x3f4ccccd); FUN_004c1b10(uVar8);` (L1520-1521) | D3D states set need FUN_004c1b10 body (not in file)
U-4604 | PARTIAL | consumer FUN_00491340 transforms the 3 floats by matrix pfVar5: `*pfVar8 = fVar3 * *pfVar5 + fVar4 * pfVar5[8] + fVar2 * pfVar5[4] + pfVar5[0xc]` (L1643) ÔåÆ pre-matrix coords | which space the matrix (from FUN_00467210) maps needs FUN_00467210
U-4605 | NEEDS-XREF | `if (DAT_007f108b == '\0') FUN_00491340(); else FUN_004910c0();` (L1678-1682) confirms table-vs-random select | what the flag represents needs writers of DAT_007f108b (not in file)
U-4606 | NEEDS-XREF | `(**(code **)(DAT_007d3ff8 + 0x20))(6,1)` `(8,1)` `(0xc,1)` `(9,2)` (L1710-1713) | 6/8/9/0xcÔåÆD3DRENDERSTATETYPE needs the DAT_007d3ff8+0x20 setter body (not in file) to confirm mapping
U-4607 | NEEDS-XREF | `FUN_00499d90(DAT_00771530,0x380);` (L1717) | primitive type + vertex format need FUN_00499d90 body (not in file)
U-4608 | PARTIAL | used as scale multipliers: `FUN_00472650(-(float)(fVar6 * _DAT_005ce010),(float)(fVar6 * _DAT_005ce010))` (L1782-1783), `*(...) * _DAT_005cc318` (L1785) | float values not shown (data addrs not in file)
U-4628 | NEEDS-XREF | `in_EAX[5] = (int)(float)fVar2;` `in_EAX[6] = ...` `in_EAX[7] = ...` (L1843-1846) | int-packed vs float-packed needs vertex consumer FUN_00491f00 (caller, not in file)
U-4700 | NEEDS-XREF | four-int equality compares vs `&DAT_005d0c1c` (L1893), `&DAT_005d0cac` (L1907), `&DAT_005d0c7c` (L1919); returns 0x80070057 on mismatch | block values/producers need reference_to each addr (not in file)
U-4701 | NEEDS-XREF | only xref `CONDITIONAL_JUMP 0x00494b5b in (data)` (L1874) | calling context needs search for refs to 0x00494b65 (bytes 65 4b 49 00), not in file
U-4702 | NEEDS-XREF | only xref `DATA 0x005cfd7c in (data)` (L1946) | owning class/vtable at 0x005cfd7c needs chase (refs to 0x00494c60), not in file
U-4703 | NEEDS-XREF | writes 3├ù 0x100-word ramps at in_EAX+0x400: `puVar2[-0x200]=uVar1; puVar2[-0x100]=uVar1; *puVar2=uVar1` over 0x100 (L1998-2002) | D3DGAMMARAMP confirmation needs producer FUN_00495080 (device call), not in file
U-4704 | NEEDS-XREF | `(**(code **)(*unaff_ESI + 0x1c))()`, `+0x58`, `+0x54` (L2038-2042) | unaff_ESI type/origin needs producer FUN_00495080 (not in file)
U-4705 | NEEDS-XREF | `FUN_00494f40(uStack_10,uStack_c);` (L2041) ÔÇö uStack_10/uStack_c declared, never set in this fn | producer of those stack vars needs caller FUN_00495080 (not in file)
U-4706 | PARTIAL | symbol `thunk_FUN_00495780` but body calls `iVar1 = FUN_004955d0();` (L2071); comment says 4-byte body 0x004951e0-4 | actual JMP-target opcode bytes at 0x004951e0 not shown (raw byte read, not in file)
U-4707 | NEEDS-XREF | `DAT_0077307c = FUN_004c77c0(0,0,0,0x84);` (L2098) | RwRasterCreate identity needs FUN_004c77c0 body/other callers (not in file)
U-4708 | NEEDS-XREF | `(**(code **)(DAT_007d3ff8 + 0x20))(2,3)`ÔÇª`(7,2)` (L2223-2234) and `(DAT_007d3ff8 + 0x2c))(local_80 + 4,4,0,1,2)` (L2229) | slot semantics need the +0x20/+0x2c target functions (not in file)
U-4709 | NEEDS-XREF | `undefined1 *in_EAX;` and `iVar1 = *(int *)(unaff_EBX + 0x60);` (L2188) this-context | EAX/ESI type origin needs caller FUN_00496c10 (not in file)
TOTALS: ANSWERED=1 PARTIAL=10 NEEDS-XREF=34 NEEDS-RUNTIME=0 NEEDS-EXTERNAL=1 STALE=0 IDS=46
U-4710 | PARTIAL | line 38 `piVar1 = (int *)0x0;` then lines 41-42 `(**(code **)(*piVar1 + 8))(piVar1);` (x2) ÔÇö decomp literally assigns null then dereferences | raw disassembly at 0x00496cbe not in file to recover the real register/object pointer
U-4711 | NEEDS-XREF | line 32 `if (DAT_00636b70 != (int *)0x0)` ÔÇö only read here | writers of DAT_00636b70 (0x00636b70), not in file
U-4712 | NEEDS-XREF | line 101 `uVar1 = FUN_004c0ed0(*(undefined4 *)(param_2 + 4));` ÔÇö return type not shown at this site | decomp of callee FUN_004c0ed0, not in file
U-4713 | NEEDS-XREF | lines 116-123 `local_40[0] = -local_80;` etc show the row negations | inverse-transpose-vs-view semantics need consumers of caller FUN_00497060 output, not in file
U-4714 | PARTIAL | line 169 `undefined4 *unaff_retaddr;` used as copy target lines 217-219 | raw disassembly of 0x00497060 to prove ESI/stack/register origin of the 4th param, not in file
U-4715 | NEEDS-XREF | copy sources shown: auStack_4cÔåÆunaff_retaddr (215), &stack0xffffff34ÔåÆparam_1 (223), auStack_8cÔåÆparam_2 (231), &puStack_10cÔåÆparam_3 (239) | view/proj/VP labels need callers of FUN_00497060 (xref 0x0049a817 in data)
U-4716 | NEEDS-XREF | line 286 `uVar1 = unaff_EBX[3];` tested `&1`(297) `&2`(316) `&4`(327) `&0x100`(341) `&0x200`(355) | writers/callers setting other bits of unaff_EBX[3]: FUN_00498d60/FUN_00498ea0/FUN_00499400
U-4717 | NEEDS-XREF | line 401 `_DAT_00773208 = 5;` | readers/consumers of DAT_00773208 (0x00773208) to give value 5 meaning, not in file
U-4718 | NEEDS-XREF | line 403 `(...)(&DAT_0077320c,DAT_0077340c * 0x50 + DAT_007731fc)` ÔÇö 0x50 stride literal | alloc path FUN_00498c00 / writers of DAT_007731fc, not in file
U-4719 | PARTIAL | line 442 `((piVar2[3] == 0 || (*piVar2 < 0x280)) || (*piVar2 < 0x1e0))` ÔÇö both compares use *piVar2 | raw disassembly of 0x00498e40 to confirm the second is piVar2[1], not in file
U-4725 | NEEDS-XREF | line 487 `puVar6 = &DAT_007e2580;` written up to param_2 iterations; line 600 uses param_2 as count | size of block 0x007e2580 + max param_2 from caller FUN_004914b0, not in file
U-4726 | NEEDS-XREF | lines 631-632 `(*(uint *)(DAT_00771e58 + 0xc4) & 0xffff)` and `+0xcc` vs `0x100 <` | writers of DAT_00771e58 (0x00771e58) to name +0xc4/+0xcc, not in file
U-4727 | PARTIAL | FUN_004cbb20 present at line 1878 `(**(code **)(*DAT_007d4110 + 0x1a8))(DAT_007d4110,param_1,param_2)` ÔÇö a device vtable wrapper, not an allocator | FUN_004cbad0 decomp not in file; D3D9 slot 0x1a8 identity
U-4728 | NEEDS-EXTERNAL | line 699 `iVar2 = FUN_004cc5e0(iVar1,0xb,param_2,param_3);` ÔÇö chunk id 0xb literal | RenderWare RW3 chunk-type table entry for id 0x0b(11); FUN_004e99b0 decomp also absent
U-4729 | NEEDS-XREF | line 736 `iVar3 = FUN_005493d0(param_1);` then `*(int *)(iVar3 + 8)` ÔÇö key type not shown | decomp of callee FUN_005493d0, not in file
U-4750 | NEEDS-XREF | line 781 `if (param_3 < DAT_005d757c != (param_3 == DAT_005d757c))` ÔÇö value not shown | static data read of DAT_005d757c (0x005d757c), not in file
U-4751 | NEEDS-XREF | line 830 `FUN_00538c80(param_1,param_2,&LAB_004b49b0,&local_10);` ÔÇö callback passed by address | decomp of LAB_004b49b0 (0x004b49b0), not in file
U-4752 | NEEDS-XREF | line 866 `FUN_00538c80(param_1,param_2,&LAB_004b4bb0,&local_10);` | decomp of LAB_004b4bb0 (0x004b4bb0), not in file
U-4780 | PARTIAL | line 888 `param_3` declared in signature; body (lines 898-908) uses only param_1/2/4/5 ÔÇö param_3 never read | raw disassembly 0x004b51d0..0x004b5231 to decide dead-arg vs spill artifact, not in file
U-4800 | NEEDS-XREF | line 942 `param_3 = DAT_005d757c;` seed ÔÇö value not shown | static data read of DAT_005d757c (0x005d757c), not in file
U-4801 | NEEDS-EXTERNAL | lines 1080-1089 `(**(code **)(DAT_007d3ff8 + 0x20))(7,1);(1,0);(0xc,1)`; line 1070 `_DAT_005cc320 / ...` | RenderWare RwRenderState enum values 7/1/0xc; plus static read of DAT_005cc320
U-4806 | NEEDS-XREF | line 1151 `puVar5 = (undefined2 *)(*piVar1 + 4);` line 1163 `puVar5 = puVar5 + 4;` ÔÇö +8-byte stride shown | index format needs param_1+4 construction via caller FUN_004844a0 (0x0048454f), not in file
U-4811 | NEEDS-XREF | line 1264 `LPCSTR in_EAX;` line 1271 `CreateFileA(in_EAX,...)` ÔÇö EAX use shown | raw disassembly of caller FUN_004b6940 at 0x004b69f6 to confirm the ABI, not in file
U-4825 | PARTIAL | line 1289 `DECOMP UNAVAILABLE: no function at or containing 0x004c07b0` ÔÇö confirms the LAB_-only claim | ctor body needs raw disassembly (listing) of 0x004c07b0, not in file
U-4826 | PARTIAL | line 1297 `DECOMP UNAVAILABLE: no function at or containing 0x004c0830` ÔÇö confirms the LAB_-only claim | dtor body needs raw disassembly (listing) of 0x004c0830, not in file
U-4900 | NEEDS-XREF | lines 1327-1335 reference `_DAT_005cc320`,`_DAT_005cd03c`,`_DAT_005cc32c` ÔÇö values not shown | static data reads at 0x005cc320/0x005cc32c/0x005cd03c, not in file
U-4929 | NEEDS-XREF | lines 1369/1371/1374 `FUN_004c42d0(param_1)`,`FUN_004c4270(param_1)`,`FUN_004c4360(param_1)` return float10 | callee bodies of the three functions to decide point-vs-object, not in file
U-4931 | NEEDS-EXTERNAL | line 1420 signature `float * FUN_004c5470(mat,axis,angle,param_4)`; xref caller 0x0040712b in FUN_00406ce0 | librw RwMatrixQueryRotate cross-ref to confirm the name; caller 0x00406ce0 identity
U-4977 | NEEDS-EXTERNAL | line 1532 `(**(code **)(DAT_007d3ff8 + 0xa8))(0,param_1,0);` | RenderWare device/raster vtable table entry at offset 0xa8
U-4978 | NEEDS-XREF | line 1533 `*(byte *)(param_1 + 0x22) = *(byte *)(param_1 + 0x22) & 0xe7;` (clears 0x18=bits 3,4) | callers of FUN_004c7620 / struct layout to name field at +0x22, not in file
U-4979 | NEEDS-EXTERNAL | line 1562 `(**(code **)(DAT_007d3ff8 + 0x5c))(0,param_1,0);` | RenderWare device vtable table entry at offset 0x5c
U-4980 | NEEDS-EXTERNAL | line 1563 `(**(code **)(DAT_007d3ff8 + 0x11c))(...,param_1);` | RenderWare device vtable table entry at offset 0x11c
U-4981 | NEEDS-EXTERNAL | line 1589 `(**(code **)(DAT_007d3ff8 + 0xa4))(&param_2,param_1,param_2);` | RenderWare device vtable table entry at offset 0xa4
U-4984 | NEEDS-XREF | line 1619 `if (*(char *)((int)param_1 + 0x21) < '\0')` ÔÇö sign-bit guard shown | callers of FUN_004c7760 / struct layout to name field at +0x21, not in file
U-4985 | NEEDS-EXTERNAL | line 1624 `iVar1 = (**(code **)(DAT_007d3ff8 + 0x78))(param_1,param_2,0);` | RenderWare device vtable table entry at offset 0x78
U-4987 | NEEDS-EXTERNAL | line 1655 `(**(code **)(*DAT_007d4110 + 0x9c))(DAT_007d4110,param_1);` | D3D9 IDirect3DDevice9 vtable method at offset 0x9c
U-4988 | NEEDS-EXTERNAL | line 1686 `iVar2 = (**(code **)(*piVar1 + 0x94))(piVar1,param_1,param_2);` | D3D9 IDirect3DDevice9 vtable method at offset 0x94
U-5025 | NEEDS-XREF | lines 1717-1725 copy 16 dwords from `(&DAT_007d4158)[param_1]` (fallback &DAT_005d8b90) to param_2 | callers of FUN_004cb490 (0x0049a381 data, 0x00542304) to name the block, not in file
U-5026 | NEEDS-EXTERNAL | lines 1764/1791 `(**(code **)(*DAT_007d4110 + 0xb0))(DAT_007d4110,0x100,&...)` | D3D9 device vtable offset 0xb0 (SetTransform) + D3DTS index 0x100 confirmation
U-5029 | NEEDS-EXTERNAL | line 1828 `(**(code **)(*DAT_007d4110 + 0x6c))(DAT_007d4110,param_1 * 2,8,0x65,1,param_2,0);` | D3D9 device vtable method at offset 0x6c + D3DPRIMITIVETYPE enum to explain literal 8
U-5033 | NEEDS-EXTERNAL | line 1852 `(**(code **)(*DAT_007d4110 + 0x178))(DAT_007d4110,param_1,param_2,param_3);` ÔÇö 3 args shown | D3D9 device vtable method at offset 0x178 and its parameter list
U-5034 | PARTIAL | line 1878 `iVar1 = (**(code **)(*DAT_007d4110 + 0x1a8))(DAT_007d4110,param_1,param_2);` ÔÇö 2 args shown | raw disasm to rule out arg-folding + D3D9 slot 0x1a8 identity, not in file
U-5038 | NEEDS-XREF | lines 1908-1911 byte extracts `*(byte *)((int)pfVar1+0x12)`,`(pfVar1+4)`,`+0x11` index `param_2 + byte*-0xc + {0x14,0xc,0x10}` | caller FUN_004ec130 (0x004ec160) to confirm plane/AABB layout, not in file
U-5100 | NEEDS-EXTERNAL | line 1948 `local_4 = (param_4 + 0x10000 & 0x3ff00) << 0xe | (param_4 & 0x3f) << 0x10 | param_5 & 0xffff;` | RenderWare SDK library/version bit-pack macro (rwLIBRARYIDPACK) definition
U-5101 | NEEDS-EXTERNAL | line 1981 `local_4 = FUN_004d7ff0(0x8000001a);` ÔÇö raw error code | RenderWare error-code definition table entry for 0x8000001a
U-5103 | NEEDS-XREF | lines 2022/2026/2058-2059 read `DAT_007d45fc` (null-check, alloc arg, compare) ÔÇö only read | writers of DAT_007d45fc (0x007d45fc) to confirm the pool/freelist role, not in file
TOTALS: ANSWERED=0 PARTIAL=8 NEEDS-XREF=23 NEEDS-RUNTIME=0 NEEDS-EXTERNAL=15 STALE=0 IDS=46
U-5104 | NEEDS-EXTERNAL | 0x004ccce0 body iterates a pool bitmap firing `(*param_2)(...)` (line 61); defined FUN_ with no symbol | SDK name of the iterator needs RW/Criterion SDK; not derivable from binary
U-5105 | NEEDS-XREF | 0x004cce20 installs `&LAB_004ccef0`ÔåÆ+0x108, `&LAB_005aead0`ÔåÆ+0x110, `&LAB_004ccf00`ÔåÆ+0x114 (lines 117-120) | bodies/names of those three addresses; chase 0x004ccef0 / 0x005aead0 / 0x004ccf00
U-5106 | PARTIAL | 0x004ccf20 line 168 `(*pbVar1 & 1)` selects free slot 0x10c (line 170) vs 0x11c (line 175) | relation to FUN_004cc820 param_5[6] (writes 2/3) needs FUN_004cc820 decomp (not in file)
U-5107 | ANSWERED | 0x004cd170 case 1 lines 244-249: `uVar2=param_3&0x80000001;` `if((int)uVar2<0) uVar2=(uVar2-1|0xfffffffe)+1;` `*(iVar1+0x2c)=param_3-uVar2;` | -
U-5108 | NEEDS-EXTERNAL | 0x004cd500 sets ptr at DAT_00911d00+DAT_007d3ff8, resets to default at +0x1c (lines 298-301) | SDK identity (RwIm3DSetTransformPipeline?) needs RW Im3D API reference
U-5125 | NEEDS-EXTERNAL | 0x004cd550 case 6 maps to offset 0x18 (line 349) / 0x34 (lines 395-402) | identity of prim-type value 6 needs RW / D3DPRIMITIVETYPE enum
U-5127 | PARTIAL | 0x004cdf20 preserves bit 2: `*puVar10 = *param_1 & 2 | uVar9 & 0xfffffffd;` (line 504; also line 563) | meaning of bit 0x02 (palette-owned vs external) not shown; needs RwImage flag definition
U-5128 | NEEDS-EXTERNAL | 0x004ce390 line 666 `FUN_004ce480(param_1,5,FUN_004ce690,&iStack_8)` | SDK callback name (RwImageReadCallback vs RwImageFindFileTypeCallback) needs RW SDK
U-5129 | NEEDS-XREF | 0x004ce480 line 708 `iVar1 = FUN_004e1a90(param_1);` gates the path-list branch | identity of FUN_004e1a90; chase FUN_004e1a90 body (not in file)
U-5130 | NEEDS-XREF | 0x004ce480 calls `(**(code**)(DAT_007d3ff8+0xcc))(dst,src)` (lines 737, 754) | strcat vs strcpy; chase writer of DAT_007d3ff8+0xcc to name the FUN_
U-5150 | NEEDS-XREF | 0x004ce690 line 783 `iVar1 = FUN_00550b00(param_1);` gates the handler call | identity of FUN_00550b00; chase FUN_00550b00 body
U-5151 | NEEDS-XREF | 0x004ce6c0 line 818 `FUN_004ce480(param_1,0x14,&LAB_004ce6f0,&local_4)` | behavior of LAB_004ce6f0; chase 0x004ce6f0
U-5152 | NEEDS-XREF | 0x004ce790 calls slot 0xfc (line 853) and slot 0xf8 (line 887) | uppercase vs CRC identity; chase writers of DAT_007d3ff8+0xf8 / +0xfc
U-5153 | NEEDS-EXTERNAL | 0x004ce790 line 872 `(**(code**)(DAT_007d3ff8+0x118))(...,0x30406)` | meaning of constant 0x30406 upper bits needs RW memory-API allocation-hint table
U-5154 | NEEDS-XREF | 0x004ce8e0 line 935 `(**(code**)(iVar1+0x2c))(param_1,param_2)` gated by `+0x2c!=0` (line 934) | whether +0x2c is the write callback needs caller FUN_00498810 context
U-5155 | NEEDS-XREF | 0x004ceba0 switch handles only 0x808/0x404/0x408/0x420/0x820/0x2020 (lines 982-1024); no depth-1 branch | whether any creation site uses depth 1; scan RwImageCreate depth args binary-wide
U-5156 | PARTIAL | 0x004cecb0 line 1183 `*puVar3 = *param_2 & 2 | uVar4 & 0xfffffffd;` preserves bit 2 | meaning of bit 0x02 (palette-shared vs other) not shown; needs RwImage flag definition
U-5157 | NEEDS-XREF | 0x004cf2b0 line 1258 `if((char)(local_4&3)!='\x03')` else line 1261 `thunk_FUN_004c4680(puVar3,puVar3)` | purpose of thunk_FUN_004c4680; chase 0x004c4680
U-5158 | NEEDS-XREF | 0x004cf430 line 1297 `FUN_004e1ac0(&DAT_00618138,param_1..param_4)` | which _rwPluginRegistry API (forAll/deInit/readData); chase FUN_004e1ac0 body
U-5159 | NEEDS-XREF | 0x004cf460 lines 1326-1327 `FUN_004d8770(param_1+0x10)`, `FUN_004d8770(param_1+0x30)` summed into size | identity of FUN_004d8770; chase its body
U-5160 | NEEDS-XREF | 0x004cf4a0 line 1374 READS `(*(byte*)(*piVar2+0x23) & 0x10)` | meaning of raster+0x23 bit 0x10 (mipmaps vs RT); chase writers of raster+0x23
U-5161 | ANSWERED | 0x004cf4a0 line 1380 `piVar2[0x14]&0xffffU` (int idx 0x14 = byte 0x50) vs 0x004cf5a0 line 1477 `*(uint*)(iVar2+0x50)=local_110&0xffff` ÔÇö same byte-0x50 16-bit field | -
U-5162 | NEEDS-XREF | 0x004cf5a0 `local_100[128]`/`local_80[128]` filled by FUN_004d8810 (lines 1460, 1466), no cap check | whether FUN_004d8810 bounds the read to 128; chase FUN_004d8810
U-5190 | NEEDS-EXTERNAL | 0x004d1e30 calls `*DAT_007d4110 + 0x5c/0x74/0x34/0x14` (lines 1558,1628,1669,1672) | confirm these are the named D3D9 methods; needs IDirect3DDevice9 vtable layout
U-5375 | NEEDS-XREF | 0x004d84e0 xrefs are 8 `UNCONDITIONAL_CALL ... in (data)` at 0x0053f4d1..0x0053f81d (line 1734) | call-site context; identify the data table/registry that invokes it
U-5377 | NEEDS-EXTERNAL | 0x004d87a0 line 1789 `FUN_004cc580(param_2,2,uVar3,0x37002,10)` | chunk-type semantic of constant 0x37002 needs RW chunk-type ID table
U-5378 | NEEDS-XREF | 0x004d8eb0 line 1849 `FUN_004e2ff0();` (zero-arg trailing call) | purpose (compact/repack/notify); chase FUN_004e2ff0 body
U-5379 | NEEDS-XREF | 0x004d8eb0 lines 1834-1835 `if((code*)piVar5[5]!=0) (*(code*)piVar5[5])(piVar5)` | node object type; chase writers of node+0x14 (piVar5[5])
U-5380 | PARTIAL | "DECOMP UNAVAILABLE: no function at or containing 0x004d8430" (line 1872) | no decomp in file; needs Ghidra function-boundary creation at 0x004d8430 (D-8560)
U-5381 | PARTIAL | "DECOMP UNAVAILABLE: no function at or containing 0x004d8470" (line 1880) | no decomp in file; needs Ghidra boundary creation at 0x004d8470 (D-8560)
U-5382 | PARTIAL | "DECOMP UNAVAILABLE: no function at or containing 0x004d8530" (line 1891) | no decomp in file; needs Ghidra boundary creation at 0x004d8530 (D-8560)
U-5383 | NEEDS-XREF | block 0x004d8530 DECOMP UNAVAILABLE; row names FUN_004d3db0 (PipeModule init via call site) | chase FUN_004d3db0 body (not in file)
U-5384 | PARTIAL | "DECOMP UNAVAILABLE: no function at or containing 0x004d8550" (line 1902) | no decomp in file; needs Ghidra boundary creation at 0x004d8550 (D-8560)
U-5385 | NEEDS-XREF | block 0x004d8550 DECOMP UNAVAILABLE; row names FUN_004d3d50 (PipeModule teardown via call site) | chase FUN_004d3d50 body
U-5386 | PARTIAL | "DECOMP UNAVAILABLE: no function at or containing 0x004d8a80" (line 1913) | no decomp in file; needs Ghidra boundary creation at 0x004d8a80 (D-8560)
U-5387 | NEEDS-XREF | block 0x004d8a80 DECOMP UNAVAILABLE; row names FUN_004e2fa0 (arena initializer via call site) | chase FUN_004e2fa0 body
U-5388 | PARTIAL | "DECOMP UNAVAILABLE: no function at or containing 0x004d8b70" (line 1924) | no decomp in file; needs Ghidra boundary creation at 0x004d8b70 (D-8560)
U-5389 | NEEDS-XREF | block 0x004d8b70 DECOMP UNAVAILABLE; row names FUN_004d8560 (6-byte stub returns 1) | chase FUN_004d8560 body
U-5390 | PARTIAL | "DECOMP UNAVAILABLE: no function at or containing 0x004d8fa0" (line 1938) | no decomp in file; needs Ghidra boundary creation at 0x004d8fa0 (D-8560)
U-5391 | NEEDS-XREF | block 0x004d8fa0 DECOMP UNAVAILABLE; row names FUN_004cc9e0 (6-arg allocator call site) | chase FUN_004cc9e0 body
U-5392 | NEEDS-XREF | block 0x004d8fa0 DECOMP UNAVAILABLE; row names DAT_00618428 / DAT_0061842c (loaded not written) | chase writers of DAT_00618428 / DAT_0061842c
TOTALS: ANSWERED=2 PARTIAL=10 NEEDS-XREF=22 NEEDS-RUNTIME=0 NEEDS-EXTERNAL=7 STALE=0 IDS=41
U-5403 | NEEDS-XREF | write `*(undefined1 *)(local_5c + 6) = 0;` (partition node byte, line 196) | readers of the node+6 byte to distinguish in-use flag vs sentinel
U-5425 | NEEDS-EXTERNAL | callees FUN_004cdca0/FUN_004cdd60/FUN_004cdd00/FUN_004cecb0 (line 274) | RenderWare RtImage/Rt2d toolkit export signatures (RtImageCreate/RtImageCopy) to match the shapes
U-5426 | NEEDS-EXTERNAL | `FUN_004d5480(0x88,0)` / `FUN_004d5480(0x89,0)` (lines 357-358) | D3D9 D3DRENDERSTATETYPE enum: confirm 0x88=D3DRS_FOGENABLE, 0x89=D3DRS_SPECULARENABLE
U-5427 | NEEDS-XREF | `*(undefined4 *)(&DAT_005d8d8c + unaff_retaddr * 4)` (line 486) | caller FUN_004d07b0 (call at 0x004d0b53) to recover the register-passed prim-type arg
U-5428 | PARTIAL | if/else branches (lines 531-542 vs 545-556) are byte-identical scalar float under guard `((uint)param_4 & 0xf) == 0` (line 530) | no SIMD in either branch; whether original source had an SSE fastpath is not derivable from decomp
U-5584 | NEEDS-XREF | `int in_EAX;` dereffed as this `*(int *)(in_EAX + 0x1c)` (lines 596-598) | caller FUN_0041d870 (call at 0x0041d878) to confirm EAX-this convention
U-5585 | NEEDS-XREF | `(**(code **)(*(int *)(in_EAX + 0xbc) + 0x48))(...)` (line 599) | constructor/vtable populating slot +0x48 to name the virtual method
U-5586 | NEEDS-XREF | `int *unaff_ESI;` (line 730) | caller FUN_0041ded0 (call at 0x0041dee4) + ESI write site for base object type
U-5587 | NEEDS-XREF | `unaff_ESI[DAT_007f1a1c + 7]` (line 736) | writers of global DAT_007f1a1c
U-5588 | PARTIAL | `(**(code **)(unaff_ESI[unaff_ESI[0x14] + 2] + 0x48))();` (line 740) confirms stated slot=ESI[ESI[0x14]+2] | value of field ESI[0x14] / actual jump targets
U-5589 | NEEDS-XREF | `int unaff_EDI;` dereffed as this (lines 774,777) | caller FUN_0041e850 (call at 0x0041e85f) to confirm EDI-this convention
U-5590 | NEEDS-XREF | `*(int *)(unaff_EDI + 0x188 + ((int)uVar4 / 2) * 4)` (line 783) | writers/stores at EDI+0x188..0x190 for semantics
U-5591 | NEEDS-XREF | `(**(code **)(DAT_007d3ff8 + 0x20))(9,2)` (line 842) | initializer/type of object at DAT_007d3ff8 to name slot +0x20
U-5592 | NEEDS-XREF | `FUN_00556e90(DAT_0067d83c,&param_4,&param_4,&param_4,&param_4)` (line 845) | callee FUN_00556e90 body to tell struct vs separate params
U-5595 | NEEDS-XREF | `FUN_004c5c00(DAT_0068b9ac,"flamethrower")` (line 882) | writers of global DAT_0068b9ac
U-5596 | NEEDS-XREF | `uVar1 = FUN_004c5c00(DAT_0068b9ac,...)` (line 882) | callee FUN_004c5c00 body/return type
U-5614 | NEEDS-XREF | `FUN_004c5770(*piVar1);` (line 955) | callee FUN_004c5770 body (destructor vs release vs decref)
U-5615 | NEEDS-XREF | `FUN_00552a60(0);` (line 971) | callee FUN_00552a60 body (role of arg 0)
U-5616 | NEEDS-XREF | `iVar2 = FUN_005c4ad0(0x18,0x301a1);` (line 1012) | callee FUN_005c4ad0 body (general vs font-specific; 0x301a1 backing)
U-5617 | NEEDS-XREF | `puVar4 = (undefined4 *)FUN_005c4c60(iVar2,iVar3,0x301a1);` (line 1027) | callee FUN_005c4c60 body (resize in-place vs fresh alloc)
U-5618 | NEEDS-XREF | recursion `FUN_00553f40(piVar1[5]);` (line 1045); entry guard `param_1==0 || param_1[1]==-1` (line 1011) | writers of node+5 for chain depth
U-5619 | NEEDS-XREF | `FUN_005c4d50();` when `DAT_00912a28[1] == -1` (lines 1081-1082) | callee FUN_005c4d50 body
U-5620 | NEEDS-XREF | `FUN_00553e80(iVar1);` (line 1090) | callee FUN_00553e80 body (recursive vs one level)
U-5621 | NEEDS-XREF | `0x301a1` (line 1128) and `0x401a1` (line 1127) both present | consumer FUN_004cc820/allocator to tell separate arenas vs flag bits
U-5622 | NEEDS-XREF | `thunk_FUN_004cc820(0x1c,DAT_00623e8c,4,DAT_00623e90,...)` (line 1127) | writers of DAT_00623e8c / DAT_00623e90
U-5623 | PARTIAL | two textual calls `FUN_00553f40(puVar1)` at lines 1137 and 1140 straddling `DAT_00912a28 = puVar1` (1139) | raw listing 0x554138..0x554149 to confirm real double-call vs decompiler artifact
U-5625 | NEEDS-XREF | `FUN_00555830(puVar2[1]);` (line 1175) | callee FUN_00555830 body
U-5626 | NEEDS-XREF | `FUN_004c5930(DAT_00912a1c);` (line 1182) | callee FUN_004c5930 body (texture release vs RW free)
U-5644 | PARTIAL | writer `DAT_00912a08 = thunk_FUN_004cc820(0x144,...,0x40199)` (line 1297); use `(**(code**)(DAT_007d3ff8+0x118))(DAT_00912a08,0x30199)` (1231) | actual type name (needs allocator FUN_004cc820 semantics)
U-5645 | ANSWERED | `puVar1[0x4e] = FUN_00554940;` (line 1252); 0x4e*4 = 0x138 stores a code address (FUN_ prefix) ÔåÆ function pointer, not data label | -
U-5646 | NEEDS-XREF | `DAT_00912a08 = thunk_FUN_004cc820(0x144,DAT_00623e94,4,...)` (line 1297) | callee FUN_004cc820 body (allocator role)
U-5647 | NEEDS-XREF | stride-6 init `psVar3[-2]=sVar4-3 ... psVar3[3]=sVar4-2; psVar3+=6; sVar4+=4` (lines 1320-1327) | readers of DAT_00912704 in rendering functions to interpret each field
U-5648 | STALE | `_DAT_00913100 = FUN_00555910;` (line 1334) ÔÇö 0x00555910 shown as a defined function (FUN_ prefix; also callee xref at 0x00555a2e) | statement "function not defined in Ghidra" is contradicted
U-5650 | NEEDS-XREF | param_2 in `(...)(param_4,s__s__s_00623ef8,param_2,...)` (line 1421) and `FUN_004c5cb0(param_2,...)` (1432) | caller FUN_00555910 (call at 0x00555a2e) for type/convention
U-5651 | NEEDS-XREF | `FUN_00550a20(param_4,0x100,param_3);` (line 1420) | callee FUN_00550a20 body (param_3 semantics)
U-5652 | NEEDS-XREF | `FUN_004ce2d0(DAT_00912a0c);` (line 1423) | callee FUN_004ce2d0 body (RtFSManager vs custom VFS)
U-5653 | NEEDS-XREF | `iVar2 = *(int *)(DAT_0091325c + iVar7 * 4);` read with no null-check (line 1579) | callers of FUN_00555f20 / writers of DAT_0091325c to prove always-valid on entry
U-5654 | NEEDS-EXTERNAL | `(float)((iStack_5c - iVar13) + -3) / (float)(((iStack_70 + iVar11) - iStack_70) + -3)` (lines 1919-1921) | font atlas assets (piz/.met) to confirm -3 = border+shadow+gap
U-5655 | PARTIAL | `_DAT_005cd088` (lines 1907,1914) and `_DAT_005cc32c` (lines 1916,1918) are distinct data addresses used left/top vs right/bottom | actual float values (memory_read) to tell if both are 0.5f
U-5656 | NEEDS-XREF | dual use: counter `local_54=0`(1697)/`+1`(1948)/`if(3<local_54)`(1951) and byte store `*(undefined1 *)(iStack_3c+0x1c+(int)local_1c)=(undefined1)local_54` (1910) | readers at stride-0x20+0x1c for [+0x1c] meaning
U-5674 | NEEDS-XREF | `(**(code **)(param_1 + 0x138))();` (line 2000); caller FUN_00427f00 passes DAT_0067d838 (line 847) | type of DAT_0067d838 / remaining 3 callers (vtable struct vs fn-ptr array)
U-5675 | PARTIAL | single indirect jmp thunk `(**(code **)(param_1 + 0x138))();` with "Treating indirect jump as call" (lines 1998-2000) | target arg count/calling convention (raw disasm + target body)
U-5678 | NEEDS-XREF | `(**(code **)(DAT_007d3ff8 + 0x10c))(DAT_00912a04)` (line 2027) | initializer/vtable of DAT_007d3ff8 to identify slot +0x10c
U-5679 | NEEDS-XREF | `DAT_00912a04 = (**(code **)(DAT_007d3ff8 + 0x108))(0x2400,0x40190)` (line 2060) | initializer/vtable of DAT_007d3ff8 to identify slot +0x108
U-5680 | NEEDS-XREF | `thunk_FUN_004cc820(0x78,DAT_00623f60,0x10,DAT_00623f64,&DAT_007dc7e8,0x401a2)` (line 2061) | callee FUN_004cc820 to resolve arg roles of DAT_00623f60 / DAT_00623f64
U-5681 | NEEDS-XREF | `thunk_FUN_004cc820(0x78,...,&DAT_007dc7e8,0x401a2)` (line 2061) | callee FUN_004cc820 for role of &DAT_007dc7e8 (hint-object vs arena vs size-limit)
TOTALS: ANSWERED=1 PARTIAL=6 NEEDS-XREF=35 NEEDS-RUNTIME=0 NEEDS-EXTERNAL=3 STALE=1 IDS=46
U-5682 | NEEDS-XREF | `puVar1 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_00912a00,0x301a2);` ÔÇö vtable is at DAT_007d3ff8, not resolved in this file | writers of global DAT_007d3ff8 to identify the object/vtable type
U-5683 | PARTIAL | call site `FUN_00556e90(puVar1,&uStack_c,&uStack_c,&uStack_c,&uStack_c);` passes 4 separate pointer args (not one struct pointer) | body of callee FUN_00556e90 (not in this file) to confirm they are channel args
U-5684 | PARTIAL | call site `FUN_00557110(puVar1,&uStack_8,&uStack_8,&uStack_8,&uStack_8);` passes 4 separate pointer args | body of callee FUN_00557110 (not in this file) for the arg meaning
U-5685 | NEEDS-XREF | `if (puVar1[0x1a] != 0) { FUN_004c5a60(puVar1[0x1a]); }` | body of callee FUN_004c5a60 (not in this file) to identify the resource type freed
U-5704 | NEEDS-XREF | read only: `thunk_FUN_004cc820(0x100,DAT_00623f68,0x10,DAT_00623f6c,&DAT_007dc81c,0x401a3);` | writers of DAT_00623f68 and DAT_00623f6c (contents not in this file)
U-5705 | NEEDS-XREF | reads only: `thunk_FUN_004cc820(8,DAT_00623f78,4,DAT_00623f7c,...)` and `(0x14,DAT_00623f70,4,DAT_00623f74,...)` | writers of DAT_00623f70/f74/f78/f7c (contents not in this file)
TOTALS: ANSWERED=0 PARTIAL=2 NEEDS-XREF=4 NEEDS-RUNTIME=0 NEEDS-EXTERNAL=0 STALE=0 IDS=6
