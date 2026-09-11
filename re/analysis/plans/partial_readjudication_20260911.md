U-0007 | PARTIAL | FUN_004caea0 body: `return DAT_007d4110;` (undefined4) | object type at DAT_00771a0c still unnamed; what DAT_007d4110 is/points to
U-4388 | PARTIAL | FUN_004d7ff0 body: `return param_1;` ÔÇö callback is identity/no-op | purpose of size arg (param_1=0) needs FUN_004d7de0 (not provided)
U-4391 | PARTIAL | FUN_004b6520 body: `FUN_004b64e0(param_1,0,param_2);` ÔÇö literal 0 passed as fill value | confirming memset-zero (arg order dst,val,len) needs FUN_004b64e0 body
U-4399 | PARTIAL | FUN_0045bfe0 body: `return DAT_0080332c;` ÔÇö pre-negation value is global DAT_0080332c | why it is negated (intent) not in decomp; body is a void getter, never touches local_8
U-4401 | NO-NEW-EVIDENCE | no new body (pass-1 decomp only) | raw listing at 0x00475616 to tell IEEE-754 bug from Ghidra cast artifact
U-4412 | NO-NEW-EVIDENCE | no new body | raw listing/EAX resolution at 0x004756e0 for true float-vs-pointer type
U-4413 | PARTIAL | FUN_004c1480: `FUN_004c52f0(param_1 + 0x10,param_2,param_3);` forwards local_40 (param_2) onward | field semantics of local_40 need FUN_004c52f0 (not provided)
U-4414 | NO-NEW-EVIDENCE | no new body | row3/full 4x4 access not present to verify stride-4 layout
U-4415 | PARTIAL | FUN_00557ec0: `iVar1 = *(int *)(param_1 + 0x18); if (iVar1 != 0) FUN_004e8e90(iVar1); piVar2[6] = iVar1;` and `piVar2[param_2 + 6] = param_3;` ÔÇö +0x18 stored to a slot with ref call | semantic label of +0x18/slot (needs FUN_004e8e90/FUN_004e8ea0)
U-4418 | RESOLVED | FUN_004770c0 decodes each param_2 bit: e.g. `if ((param_2 & 0x800) != 0) { uVar3 = uVar3 | 0x80000; }`, `& 4`ÔåÆalloc `param_1[6]=FUN_00474db0(param_3,4)`, `& 8`ÔåÆ`param_1[7]=FUN_00474db0(param_3,0x40)` | -
U-4419 | NO-NEW-EVIDENCE | no new body | "counter"/"type tag" labels for slot [0x11]/[0x10] not derivable from decomp
U-4427 | NO-NEW-EVIDENCE | no new body | decomp never labels +0xc4/+0xd0/+0xd4/+4/+0x90/+0x34 as intensity/speed/wheel/exhaust
U-4428 | NO-NEW-EVIDENCE | no new body | actual float values of _DAT_0061325c/_DAT_00613268/_DAT_00613284 (data addrs not in file)
U-4501 | PARTIAL | FUN_004c13e0: `FUN_004c5010(param_1 + 0x10,param_2,param_3);` forwards param_2 (the &local_c arg) | whether local_8/local_4 read via &local_c needs FUN_004c5010 (not provided)
U-4575 | NO-NEW-EVIDENCE | no new body | "RWS stream descriptor" semantic of the +0x4c..+0x90 offset pairs not shown
U-4581 | PARTIAL | FUN_00472650 body: `uVar1 = FUN_00534870(param_1);` then scaled-random return ÔÇö the discarded call has a side effect (invokes FUN_00534870) | whether that is an RNG advance / why discarded (needs FUN_00534870 + intent)
U-4600 | NO-NEW-EVIDENCE | no new body | which physical axis DAT_00771538/DAT_0077153c is (height vs other) not labeled
U-4601 | NO-NEW-EVIDENCE | no new body | axis identity (Y/height) of the middle position component not proven
U-4604 | PARTIAL | FUN_00467210 returns matrix ptr `*(int *)(DAT_006905b0 + 4) + 0x10` (or FUN_0042f510 result +0x10) | coordinate space (object-local vs world) still unlabeled; needs FUN_0042b930/FUN_0042f510
U-4608 | NO-NEW-EVIDENCE | no new body | actual float values of _DAT_005ce010/_DAT_005cc318 (data addrs not in file)
U-4706 | NO-NEW-EVIDENCE | provided bodies are FUN_00495780/FUN_004955d0 decomp, not thunk bytes | raw 4-byte opcode read at 0x004951e0 to confirm JMP target
U-4710 | NO-NEW-EVIDENCE | no new body | raw disassembly at 0x00496cbe to recover real object pointer behind null-vtable deref
U-4714 | NO-NEW-EVIDENCE | no new body | raw disasm of 0x00497060 to prove ESI/stack origin of 4th param
U-4719 | NO-NEW-EVIDENCE | no new body | raw disasm of 0x00498e40 to confirm 2nd compare is piVar2[1] not *piVar2
U-4727 | PARTIAL | FUN_004cbad0: `(**(code **)(*DAT_007d4110 + 0x16c))(DAT_007d4110,param_1,param_2)` ÔÇö device vtable wrapper, not an allocator | D3D9 slot 0x16c/0x1a8 method identity unresolved
U-4780 | NO-NEW-EVIDENCE | no new body | raw disasm 0x004b51d0..0x004b5231 to decide dead-arg vs spill for param_3
U-4825 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE (LAB_ only) | raw disassembly listing of ctor at 0x004c07b0
U-4826 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE (LAB_ only) | raw disassembly listing of dtor at 0x004c0830
U-5034 | NO-NEW-EVIDENCE | FUN_004cbb20 body only re-shows the 2-arg call `(*(*DAT_007d4110 + 0x1a8))(DAT_007d4110,param_1,param_2)` | raw disasm to rule out arg-folding + slot 0x1a8 identity
U-5106 | PARTIAL | FUN_004cc820: `param_5[6] = 2;` (internal alloc) vs `param_5[6] = 3;` (caller-provided), and `if ((param_5[6] & 1) == 0)` gates the free | tie between param_5[6] and FUN_004ccf20's pool_base-1 byte not literally shown
U-5127 | NO-NEW-EVIDENCE | no new body | RwImage flag definition for bit 0x02 (palette-owned vs external)
U-5156 | NO-NEW-EVIDENCE | no new body | RwImage flag definition for bit 0x02 (palette-shared vs other)
U-5380 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE at 0x004d8430 (LAB_ only) | Ghidra function-boundary creation (D-8560)
U-5381 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE at 0x004d8470 (LAB_ only) | Ghidra function-boundary creation (D-8560)
U-5382 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE at 0x004d8530 (LAB_ only) | Ghidra function-boundary creation (D-8560)
U-5384 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE at 0x004d8550 (LAB_ only) | Ghidra function-boundary creation (D-8560)
U-5386 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE at 0x004d8a80 (LAB_ only) | Ghidra function-boundary creation (D-8560)
U-5388 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE at 0x004d8b70 (LAB_ only) | Ghidra function-boundary creation (D-8560)
U-5390 | NO-NEW-EVIDENCE | DECOMP UNAVAILABLE at 0x004d8fa0 (LAB_ only) | Ghidra function-boundary creation (D-8560)
U-5428 | NO-NEW-EVIDENCE | no new body | whether original source had an SSE fastpath not derivable from decomp
U-5588 | NO-NEW-EVIDENCE | no new body | value of ESI[0x14] / actual jump-table targets
U-5623 | NO-NEW-EVIDENCE | no new body | raw listing 0x554138..0x554149 to confirm real double-call vs artifact
U-5644 | PARTIAL | FUN_004cc820 returns `param_5`, a struct it inits (fields [0..8]) and links into DAT_007d45cc list ÔÇö DAT_00912a08 is that pool struct | literal type name (only in a comment, not code)
U-5655 | NO-NEW-EVIDENCE | no new body | memory_read of _DAT_005cd088/_DAT_005cc32c to tell if both are 0.5f
U-5675 | NO-NEW-EVIDENCE | no new body | target arg count/calling convention of the +0x138 indirect jmp
U-5683 | RESOLVED | FUN_00556e90 sig `float * f(float *param_1,byte *param_2,byte *param_3,byte *param_4,float *param_5)`, each pointer read as its own [0..3] bytes | - (4 independent pointer args, not one RGBA struct)
U-5684 | RESOLVED | FUN_00557110 sig `int f(int param_1,float *param_2,float *param_3,float *param_4,float *param_5)`, each read as 2 floats and subtracted (`*param_4 - *param_5`) | - (4 independent 2-float pointer args, not padding)
TOTALS: RESOLVED=3 PARTIAL=12 NO-NEW-EVIDENCE=32 IDS=47

## STATUS - ADJUDICATED 2026-09-11

7 of the 47 landed as resolved (U-4418, U-5683, U-5684 from the first round;
U-4391, U-4413, U-4501, U-4581 after fetching nine more callee bodies). 2 narrowed
with the finding written into the row (U-4415 refcount pair, U-4604 path corrected).
The remaining 38 are genuinely blocked on evidence this lane cannot produce: 32 came
back NO-NEW-EVIDENCE because no further body exists to fetch, and the rest need a
runtime read or an external RenderWare/D3D9 reference.

This lane is now MINED OUT for the decompile-the-callee method. Further progress on
these rows needs a different instrument, not another round.
