U-5302 | NO-NEW-EVIDENCE | pass-1 `UNCONDITIONAL_CALL 0x0054364a in (data)` | reference_to the fn-pointer entry at 0x0054364a (what installs/reads it) ÔÇö no new body
U-0010 | PARTIAL | new caller body: `if (DAT_00636ac8 != 0) { FUN_004c5930(DAT_00636ac8); DAT_00636ac8 = 0; }` | struct type/name of the param_1 record (+0x8/+0x10/+0x14) still unnamed
U-0011 | NO-NEW-EVIDENCE | pass-1 `*(int *)(DAT_007d4054 + 0x10 + DAT_007d3ff8)` | writers/types of DAT_007d3ff8 & DAT_007d4054 ÔÇö no new body in this block
U-0091 | NO-NEW-EVIDENCE | pass-1 `DAT_007d3ff8[0x49] = 1` | struct layout/type + writers of DAT_007d3ff8 ÔÇö no new body
U-4312 | NO-NEW-EVIDENCE | pass-1 `local_48 = DAT_00636ac0[0xd]` | all WRITES to DAT_00636ac0 ÔÇö no new body
U-4313 | NO-NEW-EVIDENCE | pass-1 `local_60 = &DAT_007f1a1c` | initialization/writers of DAT_007f1a1c ÔÇö no new body
U-4398 | NO-NEW-EVIDENCE | pass-1 `xrefs (2): ... 0x00450a87 in (data), ... 0x00452e5d in (data)` | what installs/reads those two table entries ÔÇö no new body
U-4426 | PARTIAL | new caller loops `puVar1 = &DAT_00692bb8` stride 0x5c, `if (*(int *)(puVar1 + 0x58) != 0) { FUN_00477490(); }` | no explicit ESI=puVar1 load shown (decomp hides the register arg)
U-4429 | RESOLVED | `*param_1 = (float)(fVar3 * fVar2);` with `param_1[2]=`/`param_1[1]=` ÔÇö fills a vec3 into param_1 | -
U-4430 | RESOLVED | `if ((param_2 & 0x10) != 0) { uVar3 = uVar3 | 0x20; }` and `if ((param_2 & 0x10) != 0) { uVar3 = FUN_00474db0(param_3,4); param_1[8] = uVar3; }` | -
U-4500 | PARTIAL | new caller loops `puVar1 = &DAT_00692bb8` stride 0x5c calling `FUN_00477490()` guarded by `*(int *)(puVar1 + 0x58) != 0` | no literal ESI-as-this load shown (register arg hidden)
U-4502 | PARTIAL | `uVar1 = (ulonglong)ROUND(in_ST0);` ÔÇö rounds the x87 ST0 float to int (not a constant) | whether the rounded byte is an alpha depends on ST0 input at each call site (not in body)
U-4504 | PARTIAL | unlinks via `*(undefined4 *)(param_1 + 4) = 0` + `(**(code **)(DAT_007d3ff8 + 0x11c))(...)` | destroy-vs-unlock hinges on vtable slot +0x11c (target not in file); no free() visible
U-4506 | RESOLVED | `*(undefined4 *)(iVar1 + iVar4 * 4) = *param_2;` over `iVar2 = *(int *)(iVar3 + 0x30)` entries ÔÇö fill routine (default 0xffffffff) | -
U-4507 | PARTIAL | `in_EAX[0x13] = param_3;` `in_EAX[0x14] = param_4;` (caller's param_4/param_5 stored raw at dword idx 0x13/0x14) | meaning of record fields 0x13/0x14 not shown
U-4509 | PARTIAL | refcount `iVar2 = *(int *)(param_1 + 0x44) + -1` then `(**(code **)(DAT_007d3ff8 + 0x10c))(param_1)` at zero | geometry-vs-instance needs struct type / +0x10c target (not in file)
U-4510 | PARTIAL | both callers loop `&DAT_00693448` stride 0xb0 dwords (0x2c0 bytes) calling `FUN_004778e0()` | neither literally loads ESI with the record index/ptr (register arg hidden)
U-4512 | NO-NEW-EVIDENCE | pass-1 `(float)(int)(&DAT_00693448)[iVar1 * 0xb0]` | the pool consumer/renderer (reader of &DAT_00693198) ÔÇö no new body
U-4514 | PARTIAL | `FUN_004c5c00(DAT_0063b8f8,param_1);` ÔÇö thin wrapper forwarding param_1 | resource type needs FUN_004c5c00; DAT_005ceb10 string value needs a data read
U-4515 | PARTIAL | callers pass 3 consecutive vertices in order, e.g. `FUN_00477e60(local_1050,puVar6,puVar6 + 3,puVar6 + 6)` | CW/CCW winding convention not literally stated in either caller
U-4517 | PARTIAL | `if (iVar1 == 0) { return 0; } ... return 1;` over `FUN_00478030(in_EAX,param_1)` ÔÇö all-items-AND predicate | frustum-clip vs portal-vis not named; depends on FUN_00478030/FUN_00477f50 (not in body)
U-4577 | NO-NEW-EVIDENCE | pass-1 `(**(code **)(DAT_007d3ff8 + 0x20))(8,0)` | the function at DAT_007d3ff8+0x20 (meaning of 8/6 and 0/1) ÔÇö no new body
U-4578 | RESOLVED | `void FUN_0048a850(undefined4 *param_1,int param_2,undefined4 *param_3,undefined4 *param_4,undefined4 *param_5,float param_6)`; param_2=loop count, `if (param_6 <= fVar3)`, param_3/4/5 NULLÔåÆrandom/default | -
U-4580 | PARTIAL | consumer body present: `local_28 = (float *)&DAT_0076d99c` stride 0x122, gate `if (local_28[-7] != 0.0)` | does not literally reference &DAT_0076ddc8 or &DAT_0076d9b0; those globals' semantic names unassigned
TOTALS: RESOLVED=4 PARTIAL=12 NO-NEW-EVIDENCE=8 IDS=24
U-4583 | RESOLVED | `(**(code **)(*(int *)(param_1 + 4) + 0x48))();` in FUN_004770a0; full FUN_00490500 body 0x00490500-0x00490e43 | -
U-4584 | PARTIAL | `switch(param_2[6])` dispatches to FUN_00538d60/FUN_00539ec0/FUN_00539900, case 5 RpAtomicGetWorldBoundingSphere | LAB_0048fe30 callback body still not decompiled
U-4586 | RESOLVED | `FUN_004c0ed0(*(undefined4 *)(*(int *)(&DAT_0063d9e0 + (param_1 * 0xab + param_2) * 4) + 4));` | -
U-4602 | RESOLVED | `fVar1 = (pfVar2[2]*param_2[2] + *pfVar2**param_2 + pfVar2[1]*param_2[1]) - pfVar2[3];` looped `iVar3 = 6`, radius `param_2[3]` both-sided | -
U-4603 | PARTIAL | `*(undefined4 *)(param_1 + 0x84) = param_2;` then `FUN_004c1a70(param_1)` and `FUN_004c0e50` | no D3DRENDERSTATE set here; needs FUN_004c1a70 body
U-4605 | NO-NEW-EVIDENCE | (pass-1 `if (DAT_007f108b == '\0')`) | writers of DAT_007f108b
U-4606 | NO-NEW-EVIDENCE | (pass-1 `(**(code **)(DAT_007d3ff8 + 0x20))(6,1)`) | the DAT_007d3ff8+0x20 setter body
U-4607 | RESOLVED | `(**(code **)(*DAT_007d4110 + 0x14c))(DAT_007d4110,2,param_2,&DAT_007e2580,0x10);` ÔÇö prim-type arg raw 2, stride 0x10 | -
U-4628 | RESOLVED | consumer writes floats `*local_1c = pfVar6[3] + fVar2;` and packed bytes `*local_24 = (char)((int)pfVar6[-1] / 2) + -0x40;` | -
U-4700 | NO-NEW-EVIDENCE | (pass-1 compares vs &DAT_005d0c1c/&DAT_005d0cac/&DAT_005d0c7c) | values/producers at those three addresses
U-4701 | NO-NEW-EVIDENCE | (pass-1 xref `CONDITIONAL_JUMP 0x00494b5b in (data)`) | refs to 0x00494b65
U-4702 | NO-NEW-EVIDENCE | (pass-1 `DATA 0x005cfd7c in (data)`) | owning class/vtable at 0x005cfd7c / refs to 0x00494c60
U-4703 | NO-NEW-EVIDENCE | new FUN_00495080 body is only `FUN_00494fd0(param_1 + param_1 + _DAT_005cc32c,param_2);` ÔÇö no device/gamma call | FUN_00494fd0 body
U-4704 | NO-NEW-EVIDENCE | same 2-line FUN_00495080 body; no unaff_ESI or +0x1c/+0x54/+0x58 vtable use | FUN_00494fd0 body / register origin
U-4705 | NO-NEW-EVIDENCE | same 2-line FUN_00495080 body; no uStack_10/uStack_c set, no FUN_00494f40 call | the actual function setting uStack_10/uStack_c
U-4707 | RESOLVED | `iVar2 = (**(code **)(DAT_007d3ff8 + 0x118))(...,0x30407);` then field init + `(*pcVar1)(0,iVar2,param_4)` + FUN_004d8000 register | -
U-4708 | NO-NEW-EVIDENCE | (pass-1 `(**(code **)(DAT_007d3ff8 + 0x20))(2,3)`ÔÇª`(7,2)`) | the +0x20/+0x2c target function bodies
U-4709 | PARTIAL | caller shows `FUN_004938e0(DAT_0077307c,DAT_00636b70);` and `FUN_004969a0(0,DAT_0077307c)` | which callee's in_EAX/unaff_EBX is traced + its convention not identified
U-4711 | NO-NEW-EVIDENCE | (pass-1 `if (DAT_00636b70 != (int *)0x0)`) | writers of DAT_00636b70
U-4712 | RESOLVED | `return param_1 + 0x50;` (guarded by `*(byte *)(*(int *)(param_1 + 0xa0) + 3) & 1`) | -
U-4713 | NO-NEW-EVIDENCE | new body is FUN_00497060 itself (producer), not its consumers | decomp of caller at xref 0x0049a817
U-4715 | NO-NEW-EVIDENCE | new body is FUN_00497060 itself; no view/proj/VP labels | decomp of caller at xref 0x0049a817
U-4716 | PARTIAL | three helpers shown call it as getter e.g. `FUN_00498a00(&uStack_44);`, manipulate DAT_007733xx subsystem globals | none writes bits 1/2/4/0x100/0x200 of the EBX[3] dword; writer still unfound
U-4717 | NO-NEW-EVIDENCE | (pass-1 `_DAT_00773208 = 5;`) | readers/consumers of DAT_00773208
U-4718 | RESOLVED | `DAT_007731fc = _malloc(DAT_00773410 * 0x50);` (also DAT_007731f8/DAT_00773408 mallocs) | -
U-4725 | PARTIAL | caller passes `FUN_00499d90(DAT_00771530,0x380);` ÔÇö max param_2 = 0x380 | byte size of block at 0x007e2580 not stated in this body
U-4726 | NO-NEW-EVIDENCE | (pass-1 `(*(uint *)(DAT_00771e58 + 0xc4) & 0xffff)`) | writers of DAT_00771e58
U-4729 | RESOLVED | `return *(undefined4 *)(DAT_007dc724 + 8 + param_1);` | -
U-4751 | NO-NEW-EVIDENCE | (pass-1 `FUN_00538c80(param_1,param_2,&LAB_004b49b0,&local_10);`) | decomp of LAB_004b49b0
U-4752 | NO-NEW-EVIDENCE | (pass-1 `FUN_00538c80(param_1,param_2,&LAB_004b4bb0,&local_10);`) | decomp of LAB_004b4bb0
U-4806 | PARTIAL | caller iterates `FUN_004b61c0((&DAT_006cf888)[iVar1],&uStack_4);` count DAT_006cfc90 | the param_1+4 / +8-stride index-format construction not shown in this body
U-4811 | PARTIAL | body stores path `_strncpy((char *)&DAT_008ab7e0,param_1,0x80);` then `iVar4 = FUN_004b6710();` (no explicit arg) | raw disasm at 0x004b69f6 confirming EAX=path not shown (decomp elides register arg)
U-4900 | NO-NEW-EVIDENCE | (pass-1 references _DAT_005cc320/_DAT_005cd03c/_DAT_005cc32c) | memory_read of 0x005cc320/0x005cc32c/0x005cd03c
U-4929 | NO-NEW-EVIDENCE | (pass-1 FUN_004c42d0/FUN_004c4270/FUN_004c4360 return float10) | the three callee bodies
U-4978 | PARTIAL | decomp shown: `*(byte *)(param_1 + 0x22) = *(byte *)(param_1 + 0x22) & 0xe7;` after `(**(code **)(DAT_007d3ff8 + 0xa8))(0,param_1,0)` | layout_struct_get to name field +0x22 (no callers/layout)
U-4984 | PARTIAL | +0x21 sign-bit gates copy `param_1[3] = *(undefined4 *)(param_3 + 4);` + device call at +0x78 | layout_struct_get to name field +0x21 (no callers/layout)
U-5025 | NO-NEW-EVIDENCE | new body is FUN_004cb490 itself (copies 16 dwords from (&DAT_007d4158)[param_1]) | caller FUN_005422c0 body / data ref 0x0049a381 to name the block
U-5038 | PARTIAL | caller: `param_2 == 1` ÔåÆ RpAtomicGetWorldBoundingSphereÔåÆFUN_004cbb70; else `FUN_004cbbd0(uVar1,param_1 + 0x60);` | plane/AABB byte layout needs FUN_004cbbd0/FUN_004cbb70 body
U-5103 | NO-NEW-EVIDENCE | (pass-1 reads DAT_007d45fc only) | writers of DAT_007d45fc
TOTALS: RESOLVED=9 PARTIAL=10 NO-NEW-EVIDENCE=20 IDS=39
U-5105 | NO-NEW-EVIDENCE | 0x004cce20 installs `&LAB_004ccef0`/`&LAB_005aead0`/`&LAB_004ccf00` | decompiled bodies of 0x004ccef0 / 0x005aead0 / 0x004ccf00
U-5129 | RESOLVED | `if ((cVar1 != '\\') && (((...'A'..'Z'...'a'..'z'...) || (param_1[1] != ':'))) ) return 0; return 1;` | -
U-5130 | NO-NEW-EVIDENCE | `(**(code**)(DAT_007d3ff8+0xcc))(dst,src)` | writer/identity of the FUN_ at slot +0xcc (strcat vs strcpy)
U-5150 | RESOLVED | `if (param_1[uVar5] == ':') { _strncpy(&cStack_64,param_1,uVar5 + 1); ... (**(code **)(DAT_007d3ff8 + 0xf0))(&cStack_64,puVar4[3])` | -
U-5151 | NO-NEW-EVIDENCE | `FUN_004ce480(param_1,0x14,&LAB_004ce6f0,&local_4)` | decompiled body of LAB_004ce6f0
U-5152 | NO-NEW-EVIDENCE | slot 0xfc (line 853) / slot 0xf8 (line 887) | writers/identity of DAT_007d3ff8+0xf8 / +0xfc (uppercase vs CRC)
U-5154 | PARTIAL | `FUN_004ce8e0(iVar2,param_1);` (FUN_00498810 passes its param_1 as 2nd arg) | +0x2c's role as write callback still not shown; body never mentions +0x2c
U-5155 | NO-NEW-EVIDENCE | switch handles only 0x808/0x404/0x408/0x420/0x820/0x2020 | binary-wide scan of RwImageCreate depth args for any depth-1 site
U-5157 | RESOLVED | `*pfVar6 = pfVar5[2] * pfVar4[1] - pfVar5[1] * pfVar4[2];` then normalize via `FUN_004c3b90(...)` (matrix cross-product/orthonormalize) | -
U-5158 | RESOLVED | `if (puVar1[2] == param_2) break; ... puVar1[3]=param_3; puVar1[4]=param_4; puVar1[5]=param_5; return *puVar1;` (id-matched 3-slot setter) | -
U-5159 | RESOLVED | `if (param_1==0) param_1=&DAT_005d8d70; iVar1=(**(code**)(DAT_007d3ff8+0xf4))(param_1); return iVar1 + 4U & 0xfffffffc;` (rounded strlen) | -
U-5160 | NO-NEW-EVIDENCE | `(*(byte*)(*piVar2+0x23) & 0x10)` (read only) | writers of raster+0x23 to fix bit 0x10 (mipmaps vs RT)
U-5162 | RESOLVED | `uVar5 = 0x80; if (uVar6 < 0x81) { uVar5 = uVar6; } uVar3 = FUN_004cbd30(param_2,local_80,uVar5);` ÔÇö stream chunked to 0x80 into scratch, dest bounded by local_9c not 128 | -
U-5375 | NO-NEW-EVIDENCE | xrefs 8 `UNCONDITIONAL_CALL ... in (data)` 0x0053f4d1..0x0053f81d | identity of the data table/registry at 0x0053f4d1..
U-5378 | RESOLVED | `piVar1[3] = piVar1[3] + *(int *)(param_1 + -0x14) + 0x20; piVar4 = piVar1;` (coalesces free block with neighbors) | -
U-5379 | NO-NEW-EVIDENCE | `if((code*)piVar5[5]!=0) (*(code*)piVar5[5])(piVar5)` | writers of node+0x14 (piVar5[5]) / node object type
U-5383 | RESOLVED | `DAT_007d4710 = FUN_004e17e0(DAT_00618410); ... DAT_007d470c = 1; return 1;` (module init) | -
U-5385 | RESOLVED | `if (DAT_007d470c != 0) { FUN_004cc9f0(...); ... DAT_007d4710 = 0; DAT_007d470c = 0; } return 1;` (module teardown) | -
U-5387 | RESOLVED | `puVar1 = (int)param_1 + 0x27U & 0xffffffe0; iVar2 = ...; if (iVar2 < 0x20) return 0; ... return 1;` (arena init 0/1) | -
U-5389 | RESOLVED | `return 1;` (6-byte always-true stub) | -
U-5391 | RESOLVED | `uint * thunk_FUN_004cc820(int param_1,uint param_2,uint param_3,int param_4,uint *param_5,uint param_6)` full 6-arg free-list pool allocator body shown | -
U-5392 | NO-NEW-EVIDENCE | DAT_00618428 / DAT_0061842c (loaded not written) | writers of DAT_00618428 / DAT_0061842c
U-5403 | NO-NEW-EVIDENCE | `*(undefined1 *)(local_5c + 6) = 0;` (write) | readers of node+6 byte (in-use flag vs sentinel)
U-5427 | RESOLVED | `FUN_004dba60(4,&DAT_007d4658,4);` (caller passes prim-type first arg = 4) | -
U-5584 | RESOLVED | `puVar1 = &DAT_0063d298; do { FUN_0041d410(); puVar1 = puVar1 + 0x160; } while ((int)puVar1 < 0x63d558);` (caller keeps object base in reg, stride 0x160) | -
U-5585 | NO-NEW-EVIDENCE | `(**(code **)(*(int *)(in_EAX + 0xbc) + 0x48))(...)` | constructor/vtable that populates slot +0x48
U-5586 | PARTIAL | `if (DAT_0063d5e8 != 0) { FUN_0041de80(param_1); }` ÔÇö caller forwards param_1 on stack | ESI write site / base object type still not shown (body never sets ESI)
U-5587 | NO-NEW-EVIDENCE | `unaff_ESI[DAT_007f1a1c + 7]` | writers of DAT_007f1a1c
U-5589 | PARTIAL | `if (DAT_0063d7e0 != 0) { FUN_0041e630(); }` ÔÇö bare guarded call | EDI not set here; origin of EDI-this value still unconfirmed
U-5590 | NO-NEW-EVIDENCE | `*(int *)(unaff_EDI + 0x188 + ((int)uVar4 / 2) * 4)` (read) | stores at EDI+0x188..0x190
U-5591 | NO-NEW-EVIDENCE | `(**(code **)(DAT_007d3ff8 + 0x20))(9,2)` | initializer/type of object at DAT_007d3ff8 to name slot +0x20
U-5592 | RESOLVED | `float * FUN_00556e90(float *param_1,byte *param_2,byte *param_3,byte *param_4,float *param_5)` ÔÇö five separate pointer params, each deref'd separately | -
U-5595 | NO-NEW-EVIDENCE | `FUN_004c5c00(DAT_0068b9ac,"flamethrower")` | writers of DAT_0068b9ac
U-5596 | RESOLVED | `undefined4 * FUN_004c5c00(int param_1,char *param_2)` returns `puVar1 + -2` or `0x0` (case-insensitive name search of doubly-linked list) | -
U-5614 | RESOLVED | `(**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d4028 + DAT_007d3ff8),param_1); return 1;` (single free-to-pool, no decref) | -
U-5615 | RESOLVED | `if (param_1 == 0) { DAT_00912c0c = 0; } ... return 0;` (arg 0 clears DAT_00912c0c, returns 0) | -
U-5616 | RESOLVED | `iVar2 = (**(code **)(DAT_007d3ff8 + 0x108))(param_1 * 0x14,param_2 | 0x1000000);` (general count├ù0x14 array alloc; 0x301a1 is param_2 flags) | -
U-5617 | RESOLVED | `iVar2 = (**(code **)(DAT_007d3ff8 + 0x110))(*param_1,...); ... *param_1 = iVar2;` (realloc in-place growth, not fresh alloc) | -
U-5618 | NO-NEW-EVIDENCE | `FUN_00553f40(piVar1[5]);` | writers of node+5 for chain depth
U-5619 | RESOLVED | `if ((*param_1 != 0) && (param_1[2] != 0)) { ...(*param_1); *param_1=0; param_1[2]=0; } ...(param_1); return 1;` | -
U-5620 | RESOLVED | `iVar1 = param_1[5]; ... FUN_00553e80(iVar1);` (recursive free of node+5 sub-list chain) | -
U-5621 | RESOLVED | `(**(code **)(DAT_007d3ff8 + 0x108))(0x24,param_6 & 0xff0000)` ÔÇö 0x301a1/0x401a1 are flag words (masked, passed as alloc flags), not separate arenas | -
U-5622 | NO-NEW-EVIDENCE | `thunk_FUN_004cc820(0x1c,DAT_00623e8c,4,DAT_00623e90,...)` | writers of DAT_00623e8c / DAT_00623e90
U-5625 | RESOLVED | `undefined4 FUN_00555830(int param_1)` frees +0x134/+300/+0x14[4] then `(**(code **)(DAT_007d3ff8 + 0x11c))(DAT_00912a08,param_1); return 1;` | -
U-5626 | RESOLVED | `iVar3 = FUN_004c5a60(puVar2 + -2,0);` per node then `(**(code **)(DAT_007d3ff8 + 0x11c))(...,param_1); return 1;` (list destructor + free) | -
U-5646 | RESOLVED | `uint * FUN_004cc820(int param_1,uint param_2,uint param_3,int param_4,uint *param_5,uint param_6)` ÔÇö free-list pool allocator body shown | -
U-5647 | NO-NEW-EVIDENCE | stride-6 init `psVar3[-2]=sVar4-3 ... psVar3+=6; sVar4+=4` | readers of DAT_00912704 in rendering functions
U-5650 | RESOLVED | `iVar5 = FUN_00555af0(iVar3,auStack_80,iVar7,auStack_180);` with `undefined1 auStack_80 [128];` ÔÇö param_2 is a local 128-byte stack buffer ptr | -
TOTALS: RESOLVED=27 PARTIAL=3 NO-NEW-EVIDENCE=18 IDS=48
U-5651 | RESOLVED | line 28 `iVar1 = (**(code **)(*(int *)(param_3 + 0x38) + 0x30))(param_3,param_1,param_2 + -1);` | -
U-5652 | PARTIAL | line 99 `(**(code **)(DAT_007d3ff8 + 0xf4))(param_1)` + line 102 realloc `+0x110` ÔÇö a path-string copy into grown global buffer | body never names DAT_007d3ff8 as RtFSManager vs custom VFS; vtable identity unresolved
U-5653 | NO-NEW-EVIDENCE | line 165 `iVar2 = *(int *)(DAT_0091325c + iVar7 * 4);` (still the reader only, no null-check) | decomp of callers of FUN_00555f20 or of any writer of DAT_0091325c to prove valid-on-entry
U-5656 | NO-NEW-EVIDENCE | block states "NO NEW EVIDENCE AVAILABLE" | a reader at stride-0x20+0x1c to fix [+0x1c] meaning
U-5674 | NO-NEW-EVIDENCE | block states "NO NEW EVIDENCE AVAILABLE" | type of DAT_0067d838 and decomp of the other 3 callers
U-5678 | NO-NEW-EVIDENCE | block states "NO NEW EVIDENCE AVAILABLE" | initializer/vtable of DAT_007d3ff8 to identify slot +0x10c
U-5679 | NO-NEW-EVIDENCE | block states "NO NEW EVIDENCE AVAILABLE" | initializer/vtable of DAT_007d3ff8 to identify slot +0x108
U-5680 | RESOLVED | maps to `param_2`=DAT_00623f60 (`param_5[1] = param_2;` / `uVar5 = param_2 + 7 >> 3;`) and `param_4`=DAT_00623f64 (`if (param_4 != 0) { do {...} while (param_4 != 0); }` block-preallocation count) | -
U-5681 | RESOLVED | param_5=&DAT_007dc7e8: `if (param_5 == (uint *)0x0){...}else{ param_5[6] = 3; }` then `*param_5 = uVar2; param_5[1] = param_2;` ÔÇö caller-supplied pool control block written in place | -
U-5682 | NO-NEW-EVIDENCE | block states "NO NEW EVIDENCE AVAILABLE" | writers of DAT_007d3ff8 to identify the object/vtable type
U-5685 | PARTIAL | `iVar1 = param_1[0x15]; iVar2 = iVar1 + -1; ... if (iVar2 < 1)` then `(**(code **)(DAT_007d3ff8 + 0x11c))(...)` ÔÇö refcount-at-[0x15] release via vtable dtor | body does not name the concrete resource type of param_1
U-5704 | NO-NEW-EVIDENCE | block states "NO NEW EVIDENCE AVAILABLE" | writers/contents of DAT_00623f68 and DAT_00623f6c
U-5705 | NO-NEW-EVIDENCE | block states "NO NEW EVIDENCE AVAILABLE" | writers/contents of DAT_00623f70/f74/f78/f7c
TOTALS: RESOLVED=3 PARTIAL=2 NO-NEW-EVIDENCE=8 IDS=13

## STATUS OF THIS FILE - ADJUDICATED 2026-09-11, do not re-run

All 43 RESOLVED claims here have been reviewed and dispositioned. Do not treat this
file as a work queue any more; UNCERTAINTIES.md is authoritative.

- 39 landed as resolved (37 in this pass, plus U-5427 and U-5584 earlier).
- 4 refused, each kept gating with the reason written into its row:
  U-4583 (names not established - data-xref lane), U-4586 (addressing settled, bone
  IDs still unmapped), U-4707 (behaviour read, RenderWare name unconfirmed),
  U-5162 (CONFIRMED as a real memory-safety item - see
  re/analysis/rw_native_raster_name_buffer_20260911.md).

The chunk-3 asymmetry that originally caused the parking was measured and explained:
chunk 3 held 46% identity-shaped rows vs 8-29% elsewhere, and those are the rows a
callee body answers. It was composition, not leniency - the earlier suspicion in this
file was wrong.

Two review gates were used and both earned their keep. A mechanical one (does every
quoted symbol actually occur in the decomp corpus?) passed 41/41. A judgment one
(does the evidence answer the question asked?) failed 4 - including U-5162, where the
quoted code was not in the cited function at all.
