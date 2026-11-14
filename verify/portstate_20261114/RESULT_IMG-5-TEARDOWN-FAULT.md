# RESULT IMG-5-TEARDOWN-FAULT: the entry-2 teardown AV, captured (evidence for L-153)

Row `IMG-5-TEARDOWN-FAULT` (D4, run 45). Worker cmwfo3pjd3el0lh1nhu2z36xa (claude2, Opus 5.5), lead
cmwfo2r6s3eh2lh1ni98xzy3k. Kind: diagnosis only. No build, no gates, no port, no tracker change. Branch
`orch/IMG-5-TEARDOWN-FAULT` from `orch/IMG-5-TEARDOWN-BODIES` @ `ab68c1f06`.

**Finding:** the AV is a read of address 0x00000000 at `Orig_00489c60+0xa` (`push dword ptr [esi]`, esi = 0), reached
from the matched teardown 0x004555d0 on its first pass. The null it dereferences is the dword at 0x00688418, and the
only instruction in the original that stores to that dword by absolute address is `mov [edi+0x688418], eax` at
0x00455517 inside the init 0x00455400 (eax = return value of the call 0x00489940 at 0x0045550e). That init is the
stand-in on this path. **The faulting read IS state that 0x00455400 should have set.**

## Verdict: GREEN (diagnosis delivered; the fault is the init stand-in's missing store)

## 1. Boot

- **exe:** `D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/default/mashed_re.exe`, the default build of IMG-5-TEARDOWN-BODIES at
  `ab68c1f06` (sha256 `523ca877...`, equal to that RESULT's section 7). Not rebuilt.
- **knobs:** the arm `def_on_img1_cp_trap1` of RESULT_IMG-5-TEARDOWN-BODIES.md, unchanged: `MASHED_DET_FRAMES=3000
  MASHED_ORIG_TOP=1 MASHED_PORTSTATE_IMAGE=1 MASHED_OT_CODEPTR=1 MASHED_CODE_TRAP=1`.
- **root:** private `D:/MashedWT/IMG-5-TEARDOWN-FAULT_root` (robocopy of IMG-5-TEARDOWN-BODIES_root, `origtop_rw`
  excluded and removed before the boot, `original/mashed_image.bin` sha256 `81cc7775...`, the same image).
- **launch:** `PROYECTOS_TEST_HUB=off test-slot` outside (Frida and the D: root need this PC), `mashed_lock.py run
  --label IMG-5-TEARDOWN-FAULT` inside, then `img5_fault_catch.py`. That script `frida.spawn`s the exe, attaches to
  **the PID it spawned (13204)** and arms `Process.setExceptionHandler` only. There is no Interceptor. The handler
  records each exception, waits for the host's ack, then returns false, so the exception goes on to the exe's own
  handlers unchanged. `poll_attach_catch_crash.py` was not used: it finds its target by the name `MASHED.exe`.
- **end:** the process terminated by itself 28.3 s after resume (Frida `process-terminated`). No kill was needed.

### Same path as the IMG-5 arm

From `Calling AppDestroy` to the end of the log, this boot's 89 lines equal the 89 lines of
`IMG-5-TEARDOWN-BODIES_data/boots/def_on_img1_cp_trap1.log` once hex values are masked (md5 `7c5192dc...` on both).
That covers entries 0 and 1 and the last two lines, `RpClumpDestroy(0x00000000)` x2. Both logs print `OT_CODEPTR`
applied, self-test PASS (0 of 335), image slots 75 written / 0 refused, PortStateSeed 272/272 PASS, and `type table entry
2 (+0x14 = 0x00455400 in the original): init stand-in, not run`.

One difference, caused by Frida: `[code-trap] self-test execute 0x00401000` reads `kind read ... FAIL` under Frida
(1 of 6). Without Frida, in the IMG-5 log, it reads `kind execute ... ok`. Frida's exception path reports that DEP fault
to the exe's VEH with a different `ExceptionInformation[0]`. The other 5 code-trap checks pass. This check runs at
t = 0.1 s. It is not on the teardown path, and the teardown lines are identical.

## 2. The fault (exception 9 of 9, t = 28.3 s, thread 16300)

Module base `0x00010000` (the exe's preferred base, so map `Rva+Base` = runtime VA).

| field | value |
|---|---|
| type | access-violation, **read** |
| fault address (memory) | `0x00000000` |
| pc / eip | `0x0022a1aa` = `mashed_re.exe+0x21a1aa` = `_Orig_00489c60+0xa` (map) |
| instruction | `ff 36`  `push dword ptr [esi]` |
| eax | `0x007d3ff8` (the address of DAT_007d3ff8, loaded from `[0x395e44]` at `0x0022a1a0`) |
| ebx | `0x00000000` |
| ecx | `0x014ffdd0` |
| edx | `0x01150240` |
| **esi** | **`0x00000000`** (`param_1`, loaded at `0x0022a1a6` from `[esp+8]`) |
| edi | `0x006885e8` (the caller's launcher-record pointer, first pass) |
| ebp | `0x00000000` |
| esp | `0x014ffe30` |

Stack from esp: `[esp+0]` = `0x00688418` (the caller's esi, saved by `push esi` at `0x0022a1a5`). `[esp+4]` = `0x00229fb6`
(return address, `_Orig_004555d0+0x26`). **`[esp+8]` = `0x00000000`, the argument.**

Backtrace (fuzzy, symbolized with the build's map): `_Orig_004555d0+0x26` ← `_Orig_0045b930+0x24e` (0x0018241e) ←
`_AppDestroy+0x9a` ← `_Orig_00492370@16+0x1d1` ← `_PortTop_WinMain+0x5f` ← `_WinMain@16+0xad`.

The memory read at the fault: at t = 28.3 s, `0x006883b8 .. 0x006883b8+5*0x6c` (135 dwords) and `0x006885d4 ..
0x006885d4+5*0x2c` (55 dwords) are all `0x00000000`. The type table words at `0x005f9a2c` (entry 2, +0x14) =
`0x001bca10` = `_PortTop_PWI_InitStandIn_2`, and `0x005f9a30` (entry 2 teardown) = `0x00229f90` = `_Orig_004555d0`.

The other 8 exceptions: t = 0.1 s to 0.3 s. Five are code-trap self-test probes, all from `RunProbe` / `SelfTest` in
`Compat/CodeTrap.cpp`: reads at `0x00500000` and `0x00500004`, executes at `0x00401000` and `0x00000000`, and the
breakpoint at `0x005cb010`. Three are `system` exceptions at `KERNELBASE.dll+0x16d444`. None of them is in teardown.

### Where it is in the port and in the original

In the port (default build, VS2022 compile of `re/match/s09/004555d0.c` and `re/match/s14/00489c60.c`):
```
0x00229f91  mov esi, [0x395df8]          ; esi = 0x00688418 (MGLOBAL ref DAT_00688418)
0x00229f98  mov edi, [0x395dfc]          ; edi = 0x006885e8
0x00229fa0  push [edi] ; call 0x24be10  ; RpClumpDestroy(*p) -> log "RpClumpDestroy(0x00000000)"
0x00229fa7  push [esi-0xc] ; call ...    ; RpClumpDestroy(q[-3]) -> log "RpClumpDestroy(0x00000000)"
0x00229faf  push [esi] ; call 0x22a1a0   ; Orig_00489c60(*q = [0x00688418] = 0)
0x0022a1a6  mov esi, [esp+8]             ; esi = 0
0x0022a1aa  push [esi]                   ; AV: read 0x00000000
```
In the original (`MASHED.exe.unpatched`, SHA-256 BDCAE093...C0E, capstone):
```
0x004555d7  be 18 84 68 00   mov esi, 0x688418
0x004555f1  8b 16            mov edx, [esi]        ; *q
0x004555f3  52               push edx
0x004555f4  e8 67 46 03 00   call 0x489c60
0x00489c61  8b 74 24 08      mov esi, [esp+8]      ; param_1
0x00489c65  8b 06            mov eax, [esi]        ; first access through param_1: read of param_1[0]
0x00489c67  c7 46 0c 00..    mov [esi+0xc], 0      ; then the store param_1[3] = 0
```
In both binaries the first access through `param_1` is the read of `param_1[0]`: `0x00489c65` in the original and
`0x0022a1aa` in the port. The store `param_1[3] = 0` comes after it. IMG-5-TEARDOWN-BODIES section 5 named
`param_1[3] = 0` as the first statement. That is the order of the decompiled text, not of the instructions. Its
[UNCERTAIN] (EIP not captured) is closed by this row.

## 3. Is the faulting read state that 0x00455400 should have set? Yes.

The null value is the dword at **0x00688418**. That is record `0x006883b8 + i*0x6c`, field `+0x60`, with i = 0. It is
the `pp` field of `Msl_00455400` in `re/match/s29/00455400.c`.

0x00455400 in the original (`MASHED.exe.unpatched`, full listing in `D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/d455400.txt`):

| rva | bytes | instruction | what it does |
|---|---|---|---|
| 0x00455460 | `bf 1c 02 00 00` | `mov edi, 0x21c` | missile-record index start |
| 0x00455473 | `83 ef 6c` | `sub edi, 0x6c` | per pass: edi = 0x1b0, 0x144, 0xd8, 0x6c, 0x0 |
| 0x0045550e | `e8 2d 44 03 00` | `call 0x489940` | args pushed at 0x004554ef..0x0045550c: `0x40000000`, `0x3f000000`, `&col`, `1`, `0x817`, `tex`, `0xa` |
| **0x00455517** | **`89 87 18 84 68 00`** | **`mov dword ptr [edi + 0x688418], eax`** | **stores 0x00489940's return value at 0x00688418 + edi** |
| 0x0045559c | `0f 85 c3 fe ff ff` | `jne 0x455465` | loop while edi != 0 (`cmp edi, ebp` at 0x00455588, ebp = 0 from 0x00455419) |

Five passes give stores at 0x006885c8, 0x0068855c, 0x006884f0, 0x00688484, and **0x00688418 (last pass, edi = 0)**.
Those are the five dwords that 0x004555d0 reads as `*q` (esi from 0x00688418, `add esi, 0x6c`, `cmp esi, 0x688634`).

The same init also stores the two values that the two preceding `RpClumpDestroy(0x00000000)` calls received. Neither
of those calls faulted, because the RW binding skips a null clump:
- `*p` at 0x006885e8 is stored by `mov [esi+0x6885e8], eax` at **0x00455484** (eax = return of `call 0x4e6ab0` at
  0x00455479). esi starts at 0xdc (0x0045545b) and drops by 0x2c per pass (0x00455476), so the last pass has esi = 0.
- `q[-3]` at 0x0068840c is stored by `mov [edi+0x68840c], eax` at **0x004554f9** (eax = return of `call 0x4e6ab0` at
  0x004554e6).

**Other writers.** A scan of every section of `MASHED.exe.unpatched` for the dword `0x00688418` (`dwref.py`) gives
exactly two hits. One is the store at 0x00455517. The other is `mov esi, 0x688418` at 0x004555d7 (teardown, loads the
address). No other instruction holds that absolute address. [UNCERTAIN] The scan does not cover a writer that reaches
0x00688418 through a computed base plus a different displacement (for example `[reg + 0x6883b8]` with reg = 0x60). That
would need a write watch on 0x00688418 in an original-side run. The finding above does not depend on it: on this path
the stand-in `PortTop_PWI_InitStandIn_2` (`Match/PortTop_0045bae0.cpp:156`) replaces the store at 0x00455517, and the
dword is 0 at the fault.

## 4. Consequence for L-153

The entry-2 AV is not a fault in the teardown bodies. 0x004555d0 and 0x00489c60 are byte-matched and run as the
original would on a record that 0x00455400 never filled. Option (a) of RESULT_IMG-5-TEARDOWN-BODIES.md stands: build
entry 2's records with a body for 0x00455400 (shape to 100%, or a PROVISIONAL.tsv body). The arms then need re-running.
This row does not port 0x00455400.

## 5. Raw files (not committed), sha256

```
19b082afbf4857f5cf5872af0cfa1458f57c3e36d751231cff6eac7fb84ce9b0  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/img5_fault_catch.py
327895513774669d02c6438eaafa3d140bc9d6950758a6b5285a8e7d71b83fa9  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/run.sh
8ebecc472fe2566b63c7f392c61c2ea645c0f7fc0f8cb1e41085fcf19096ba79  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/run.out
5bf77092b02af059a4ea3ca46a01ab35b15dc6fd19914447c45459bd6c18948d  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/run_full.out
1376ef4084bd50faab4bd8e707d1b22a9d8473bc512a31cdf66df2e5659c7b3b  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/boots/def_on_img1_cp_trap1_catch.json
82cf790074005bf8e65cc2cc8f731c3a4f015a16df94c8872e6c6c02c434a693  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/boots/def_on_img1_cp_trap1_catch.log
3fcddfbc9f4710d0e12a799a2bc11dec9c93329cb1545a653d903e22bae96d3e  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/pedis.py
ec71b6a02856e38d352209cd9eba0c74765ab70a6fe7d2aa9f279ddf5cab00e2  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/dwref.py
4b015f68316f6512d5ca95525064bd664a3e65b61dc1e24c8559bc95e31aad74  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/maplook.py
ceb2342e277b478e5dd9faf52e158bd5aa469b3a0c82a07a9c929c22bf46d412  D:/MashedWT/IMG-5-TEARDOWN-FAULT_data/d455400.txt
523ca8779db10843acb6f342695763596afbbc266467afdcc07516ca7421d157  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/default/mashed_re.exe
e35f8bbc8e6882b8e46bebd1f1019323733531d5f737fc5b7898e14f6ee2c215  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/default/mashed_re.map
c967c827f43acfdba9ff437344f3a71b19f5ed2cc3d6ab534755734ba88425d2  D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/boots/def_on_img1_cp_trap1.log
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  D:/MashedWT/IMG-5-TEARDOWN-FAULT_root/original/mashed_image.bin
```

Gates: not run (diagnosis-only row, nothing built or changed). `original\` untouched. No tracker change.
