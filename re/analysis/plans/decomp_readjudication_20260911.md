U-0007 | PARTIAL | FUN_004caea0: `undefined4 FUN_004caea0(void)` ... `return DAT_007d4110;` (return type = undefined4) | object type at DAT_00771a0c and constants 0xe4/0x5/0x19/0x18 still unidentified; body doesn't touch the vtable call
U-0010 | PARTIAL | FUN_00402750: `DAT_00636ac8 = FUN_004b3d80("toastart\\pc\\pc.txd",0,0);` and FUN_00402a40: `FUN_004c5930(DAT_00636ac8);` (param_1 = pc.txd load return) | offsets +0x8/+0x10/+0x14 layout and struct type name (needs RW ref) still open
U-4388 | PARTIAL | FUN_004c2d90: `FUN_004d7de0(&DAT_00617fe0,param_1,param_2,param_3,param_4,0);` + FUN_004d7ff0: `return param_1;` (callback is identity passthrough) | RW-plugin-API meaning of args/size=0 needs FUN_004d7de0 + external ref
U-4401 | NO-NEW-EVIDENCE | no body provided | listing at 0x00475616
U-4412 | NO-NEW-EVIDENCE | no body provided | listing at 0x004756e0..0x004756f0
U-4414 | NO-NEW-EVIDENCE | no body provided | listing at 0x00475770
U-4415 | RESOLVED | FUN_00557ec0: `if (param_3 != 0) { FUN_004e8e90(param_3); }` ... `if (piVar2[param_2 + 6] != 0) { FUN_004e8ea0(piVar2[param_2 + 6]); }` ... `piVar2[param_2 + 6] = param_3;` (inc-ref new, release old, store into slot) | -
U-4419 | NO-NEW-EVIDENCE | no body provided | listing at 0x00475ab0
U-4421 | NO-NEW-EVIDENCE | no body provided | decomp of *(DAT_007d3ff8 + 0x10c)
U-4423 | NO-NEW-EVIDENCE | FUN_004770c0 body has no reference to offset 0xc0 (searched); DAT_00613254 value read is not from any provided body | reader of +0xc0 field still unidentified
U-4426 | PARTIAL | FUN_00475f90: `FUN_00475f50(*(undefined4 *)(param_1 + 200));` (call site + arg shown) | decomp abstracts registers: ESI init and RGBA [0..2]=0/[3] interpretation not confirmed
U-4427 | NO-NEW-EVIDENCE | no body provided | listing at 0x00475f90
U-4500 | PARTIAL | FUN_004777d0: `if (*(int *)(puVar1 + 0x58) != 0) { FUN_00477490(); }` in loop over puVar1 stride 0x5c | ESI="this" register load not literally shown in decomp
U-4502 | RESOLVED | FUN_004a2c48: `uVar1 = (ulonglong)ROUND(in_ST0);` (rounds the x87 ST0 input; input-dependent, not a constant read) | -
U-4503 | NO-NEW-EVIDENCE | no body provided; needs external RW3 vtable ref (not resolvable here) | RW3 vtable cross-check
U-4504 | PARTIAL | FUN_004c0c20: linked-list unlink over +0x9c/+0x98/+0xa0 then `(**(code **)(DAT_007d3ff8 + 0x11c))(...)` | destroy-vs-unlock is a semantic label the code does not carry
U-4507 | PARTIAL | FUN_00477450: `in_EAX[0x13] = param_3;` `in_EAX[0x14] = param_4;` (stored raw, no transform) | scale/lifetime meaning not shown; no 5th param present to map "param_5"
U-4509 | PARTIAL | FUN_005495b0: `iVar2 = *(int *)(param_1 + 0x44) + -1;` then conditional `(**(code **)(DAT_007d3ff8 + 0x10c))(param_1);` (refcount-dec + release) | destroy geometry-vs-instance label not carried
U-4510 | PARTIAL | FUN_00477920 / FUN_00477a10: `FUN_004778e0();` inside loops indexed by iVar6/iVar2 over &DAT_00693448 stride 0xb0 | ESI=slot-index register load not literally shown
U-4514 | PARTIAL | FUN_0040bb30: `FUN_004c5c00(DAT_0063b8f8,param_1);` (forwarder) | resource type (TXD vs other) needs FUN_004c5c00; body never references DAT_005ceb10
U-4515 | PARTIAL | FUN_00422570: `FUN_00477e60(local_1050,puVar6,puVar6 + 3,puVar6 + 6);`; FUN_0047bb10 passes bVar1/bVar2-ordered verts by local_28 | CW/CCW winding direction not literally determined
U-4517 | PARTIAL | FUN_0047c1f0: `iVar1 = FUN_00478030(in_EAX,param_1); if (iVar1 == 0) return 0;` (all-items-pass predicate, stride 0x10) | frustum-vs-portal semantic not carried
U-4575 | PARTIAL | FUN_00535700: mask switch e.g. `case 1: *param_2 = *(int *)(iVar2 + 0x4c); iVar3 = *(int *)(iVar2 + 0x50);` (maskÔåÆoffset-pair map) | RWS-stream-descriptor identity needs external RW header ref
U-4580 | PARTIAL | FUN_00490500: three parallel arrays `local_28=&DAT_0076d99c`, `local_2c=&DAT_0076dafc`, `local_30=&DAT_0076db04`, stride 0x122 | semantic names not establishable from decomp without guessing
TOTALS: RESOLVED=2 PARTIAL=14 NO-NEW-EVIDENCE=8 IDS=24
U-4583 | NO-NEW-EVIDENCE | FUN_00490500 uses each block only as arg to `FUN_00476d00(&DAT_007706d8)` / `...&DAT_0076d900` etc; no semantic name given | the six blocks' distinguishing names (needs writers/external ref)
U-4584 | PARTIAL | FUN_00538c80 is `switch(param_2[6])` dispatching FUN_00538d60/FUN_00539ec0/FUN_00539900 + case5 RpAtomicGetWorldBoundingSphere | LAB_0048fe30 callback body still not provided
U-4586 | PARTIAL | FUN_0041f290: `FUN_004c0ed0(*(...)(&DAT_0063d9e0 + (param_1*0xab+param_2)*4)+4))` | which skeleton joints IDs 0x33/0x34 denote (table contents/runtime)
U-4600 | NO-NEW-EVIDENCE | Only the setter FUN_00490fb0 given (`DAT_00771538 = param_3; DAT_0077153c = param_4;`), no consumer | how the Y-range pair is used at runtime
U-4601 | PARTIAL | FUN_00491590: `FUN_00472650(DAT_00771538,DAT_0077153c); pfVar4[-2] = (float)fVar6;` = range for middle of 3 position floats | which world axis the middle component maps to (Y/height not literal)
U-4603 | PARTIAL | FUN_004c1b10: `*(undefined4 *)(param_1 + 0x84) = param_2;` then FUN_004c1a70/FUN_004c0e50 | the D3D state identity (stores to +0x84, not a named render state)
U-4604 | NO-NEW-EVIDENCE | FUN_0042b930 `return DAT_0067ecb0;`, FUN_0042f510 `return DAT_0067f190;`; neither touches a coordinate/space | whether DAT_0077152c entries are object-local or world
U-4703 | NO-NEW-EVIDENCE | FUN_00494fd0 shows `(**(code **)(*unaff_ESI + 0x1c))()` etc, no struct-type name | D3DGAMMARAMP identity (needs external D3D9 name)
U-4704 | NO-NEW-EVIDENCE | FUN_00495080: `FUN_00494fd0(param_1 + param_1 + _DAT_005cc32c,param_2);` never sets/reveals ESI | unaff_ESI object type (identity needs external ref)
U-4705 | PARTIAL | FUN_00495080 passes `param_1 + param_1 + _DAT_005cc32c` and `param_2` (the args feeding uStack_10/uStack_c) | confirmation these are gamma float params
U-4708 | NO-NEW-EVIDENCE | NO BODY PROVIDED | vtable +0x20/+0x2c D3D render-state mnemonics
U-4709 | NO-NEW-EVIDENCE | FUN_00496c10: `FUN_004969a0(0,DAT_0077307c);` passes literal 0, no type shown | this-object (EAX) type (identity needs external ref)
U-4713 | NO-NEW-EVIDENCE | FUN_00497060 copies matrices to param_1/2/3 but does not distinguish inverse-transpose vs view | whether FUN_00496ec0 negation = inverse-transpose or view matrix
U-4714 | NO-NEW-EVIDENCE | NO BODY PROVIDED | prologue asm to show hidden 4th param / ESI
U-4715 | NO-NEW-EVIDENCE | NO BODY PROVIDED | matrix identity of auStack_4c/auStack_8c/puStack_10c (view/proj/VP)
U-4716 | RESOLVED | All 3 callers pass one pointer, no flag: `FUN_00498a00(local_44)` (FUN_00498d60), `FUN_00498a00(&DAT_0077330c)` (FUN_00498ea0), `FUN_00498a00(&uStack_44)` (FUN_00499400) | -
U-4719 | NO-NEW-EVIDENCE | NO BODY PROVIDED | raw asm to confirm height offset piVar2[1] vs *piVar2
U-4727 | PARTIAL | FUN_004cbad0/FUN_004cbb20 are bool wrappers: `(**(code **)(*DAT_007d4110 + 0x16c))(...)` / `+0x1a8`, not allocators | the D3D/RW vtable-slot identity
U-4728 | PARTIAL | FUN_004e99b0 reads stream, builds world: `RpWorldInstance(puVar3)` / `RpWorldDestroy(puVar3)` | RW chunk ID 0x0b confirmation vs RW3 table (external)
U-4751 | NO-NEW-EVIDENCE | NO BODY PROVIDED | LAB_004b49b0 callback body
U-4752 | NO-NEW-EVIDENCE | NO BODY PROVIDED | LAB_004b4bb0 callback body
U-4780 | NO-NEW-EVIDENCE | NO BODY PROVIDED | raw asm for actual arg count / param_3
U-4806 | NO-NEW-EVIDENCE | NO BODY PROVIDED | index-buffer stride / format from builders
U-4811 | NO-NEW-EVIDENCE | FUN_004b6940 decomp shows `iVar4 = FUN_004b6710();` but a decomp cannot show register-level EAX passing | raw asm confirming file path in EAX
U-4825 | NO-NEW-EVIDENCE | NO BODY PROVIDED | ctor body bytes at plugin-data offset
U-4826 | NO-NEW-EVIDENCE | NO BODY PROVIDED | dtor body field clear/free at offset
U-4929 | NO-NEW-EVIDENCE | NO BODY PROVIDED | byte-level decomp of callees 004c42d0/004c4270/004c4360
U-4931 | NO-NEW-EVIDENCE | NO BODY PROVIDED | FUN_00406ce0 decomp + RwMatrixQueryRotate cross-check
U-4978 | NO-NEW-EVIDENCE | NO BODY PROVIDED | field name/purpose of bits 3,4 at param_1+0x22
U-4980 | NO-NEW-EVIDENCE | NO BODY PROVIDED | vtable slot DAT_007d3ff8+0x11c name
U-4984 | NO-NEW-EVIDENCE | NO BODY PROVIDED | field name of sign bit at param_1+0x21
TOTALS: RESOLVED=1 PARTIAL=7 NO-NEW-EVIDENCE=23 IDS=31
U-5025 | NO-NEW-EVIDENCE | FUN_005422c0 calls `FUN_004cb490(0x100,local_40)`; body never mentions DAT_007d4158 | table name/purpose of the 16-dword block; only 1 of 2 named callers given
U-5029 | PARTIAL | FUN_004f13d0: `iVar5 = FUN_004cb840(*(int *)(iVar2 + 0x50),piVar7)` at load time, `iVar5 == 0`ÔåÆcleanup | internal literal 8 / 0x65 primitive-type identity (needs external RW/D3D ref)
U-5033 | NO-NEW-EVIDENCE | no body provided | full arg list from caller FUN_0049a750; param_3 role
U-5034 | RESOLVED | FUN_00543710 `FUN_004cbb20(&DAT_005e4580,&DAT_007dc5fc);` + FUN_0049aae0 `FUN_004cbb20(&DAT_005d0518,&DAT_0077397c);` ÔÇö 2 ptr args at every site, not folded | SetTextureStageState identity (external, but discredited by 2-ptr sig)
U-5038 | NO-NEW-EVIDENCE | FUN_004ec130 only passes `FUN_004cbbd0(uVar1,param_1 + 0x60)`; no +0x11/+0x12 or plane bytes | D3DPLANE 5-float layout and AABB stride/base byte layout
U-5105 | NO-NEW-EVIDENCE | no body provided | disassembly of LAB_004ccef0 / LAB_005aead0 / LAB_004ccf00
U-5106 | PARTIAL | FUN_004cc820: `param_5[6] = 2;`(alloc) / `param_5[6] = 3;`(provided); `if ((param_5[6] & 1) == 0)`ÔåÆ`+0x11c`/`+0x10c` free | pool_base-1 byte in FUN_004ccf20 not shown; its identity to param_5[6] unconfirmed
U-5125 | NO-NEW-EVIDENCE | no body provided | caller context / RW IM3D prim enum for PrimType=6
U-5127 | NO-NEW-EVIDENCE | no body provided | flag bit 0x02 meaning; RwImageCopy callers
U-5128 | PARTIAL | FUN_004ce690: `FUN_00550b00(param_1)` then `(*(code *)*param_2)(param_1)`, stores `param_2[1]`, returns 0 on success (stop) | RW API name (RwImageReadCallback vs RwImageFindFileTypeCallback) needs external ref
U-5151 | NO-NEW-EVIDENCE | no body provided | function_at 0x004ce6f0 split
U-5152 | NO-NEW-EVIDENCE | no body provided | RW slot name (statement itself says needs external RW ref)
U-5154 | NO-NEW-EVIDENCE | FUN_00498810 calls `FUN_004ce8e0(iVar2,param_1)`; never mentions node+0x2c | whether node[+0x2c] is a write callback
U-5156 | NO-NEW-EVIDENCE | no body provided | bit 0x02 read/write sites on RwImage
U-5190 | NO-NEW-EVIDENCE | no body provided | D3D9 vtable offsets vs COM listing (external)
U-5375 | NO-NEW-EVIDENCE | no body provided | any caller/call-site context
U-5377 | PARTIAL | FUN_004cc580: `local_4 = (param_4 + 0x10000 & 0x3ff00) << 0xe | (param_4 & 0x3f) << 0x10 | param_5 & 0xffff;` packs param_4 as chunk header, writes 12 bytes | semantic name of value 0x37002 (needs RW chunk enum)
U-5380 | NO-NEW-EVIDENCE | no body provided | Ghidra function-boundary at 0x004d8430
U-5403 | NO-NEW-EVIDENCE | no body provided | any reader testing byte at puVar[6]
U-5425 | PARTIAL | FUN_004cdca0 sets `puVar1[1]=param_1; puVar1[2]=param_2; puVar1[3]=param_3;` (w/h/depth new); FUN_004cdd00 frees; FUN_004cecb0 copies rows | exported RW RtImage* names (needs external ref)
U-5428 | NO-NEW-EVIDENCE | no body provided | RW3 matrix-transform reference for SSE branch
U-5585 | NO-NEW-EVIDENCE | no body provided | child-object ctor / vtable slot 0x48 semantics
TOTALS: RESOLVED=1 PARTIAL=5 NO-NEW-EVIDENCE=16 IDS=22
U-5586 | NO-NEW-EVIDENCE | FUN_0041de80 body not provided; FUN_0041ded0 only does `if (DAT_0063d5e8 != 0) FUN_0041de80(param_1)`, FUN_0041d410 shows an EAX struct but neither reveals FUN_0041de80's ESI base type | ESI object type / struct comparison for 0x0041de80
U-5588 | NO-NEW-EVIDENCE | FUN_0041ded0: `FUN_0041de80(param_1)` ÔÇö passes param_1 through unchanged; does not enumerate param_1 values, ESI[0x14], or the dispatch at 0x0041dec0 | param_1 value set + ESI[0x14] write sites
U-5589 | NO-NEW-EVIDENCE | FUN_0041e850: `FUN_0041e630();` called with no visible args, guarded by DAT_0063d7e0; nothing shows EDI register setup | confirmation of this-in-EDI convention
U-5591 | NO-NEW-EVIDENCE | NO BODY PROVIDED; statement itself says slot NAME needs RW headers/librw (external reference), not resolvable from binary | RenderWare API name for the DAT_007d3ff8 slot
U-5623 | NO-NEW-EVIDENCE | NO BODY PROVIDED; raw disasm at 0x00554138 not given | whether the second FontGlyph_UploadData call is real or artifact
U-5644 | NO-NEW-EVIDENCE | Caller FUN_00554390 calls `FUN_005551d0()` with no args and never references DAT_00912a08 (only DAT_007d3ff8/DAT_00912a20) | type of DAT_00912a08
U-5652 | NO-NEW-EVIDENCE | FUN_004ce2d0 body copies param_1 string into a growable buffer via DAT_007d3ff8 vtable, but names no API; VFS API identity needs external reference, not resolvable here | RtFSManager-vs-custom API name
U-5674 | NO-NEW-EVIDENCE | NO BODY PROVIDED; none of the 4 callers decompiled | param_1 origin/type at offset 0x138
U-5675 | NO-NEW-EVIDENCE | NO BODY PROVIDED; raw disasm 0x00556ca0ÔÇô0x00556cb5 not given | calling convention / arg count of the indirect target
U-5685 | PARTIAL | FUN_004c5a60: `iVar1 = param_1[0x15]; ... param_1[0x15] = iVar2;` refcount decr and `(**(code **)(DAT_007d3ff8 + 0x11c))(...)` release ÔÇö shows refcounted object with vtable dtor | concrete type NAME of the freed resource (external RW reference)
TOTALS: RESOLVED=0 PARTIAL=1 NO-NEW-EVIDENCE=9 IDS=10

## STATUS - ADJUDICATED 2026-09-11
4 claimed RESOLVED; 3 landed (U-4415, U-4502, U-5034) and 1 rejected. U-4716 was claimed
RESOLVED on caller enumeration alone, but the row asks what the FLAG BITS mean and
enumerating callers does not reveal that - it stays narrowed.

56 of 87 came back NO-NEW-EVIDENCE. That is the honest shape of this bucket now: the
rows say "decompile X", X has been decompiled, and X does not contain the answer.
Further progress needs a different instrument.
