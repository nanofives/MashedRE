# RESULT PORT-TOP-RWAUDIO-TABLE-632BA8: the RW-audio init/shutdown table 0x00632ba8 on the image path

## Verdict: GREEN (table bound; the init arms get past 0x005a5e70 and stop at the next named body, 0x005ae650)

- **Self-tests (both init arms, both builds):** OT_CODEPTR self-test PASS (0 of 359 checks failed, image slots 99 written,
  99 bound to an exe body, 0 refused), code-trap self-test PASS (0 of 6), portstate-image self-test PASS (0 of 8).
- **Arms** (`MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1`, section 5): `vs_init_audio` and
  `ddk_init_audio` no longer trap at 0x005a5e70. Both exit at the named stop **0x005AE650** (AudioPoolConstruct, C3 hand
  body, no matched or admitted source), return site `PortTop_Bound_005ae5b0` (+0x30 object build, +0x32 VS2022). That is
  table slot 0x00632bb0, the second init entry, so entry 0 (0x005a5e70) ran and returned nonzero and its callback 0x005a5fd0
  ran (0x005a5f60 only reaches entry 1 when both return nonzero, re/match/s25/005a5f60.c).
- Builds: VS2022 Build OK, object build Build OK, `ddkobj.py check` IDENTICAL 1411/1460, DIFFERENT 34, all expected
  (section 4). rva-lint NEW 0.
- Gates: not run (top-down row). No menu walk: no menu code changed.
- Branch: `orch/integration` @ ba39eeb6b merged in (IMG-3 and IMG-4 are in it now); the IMG-3 dependency of the handoff is
  closed.

## 1. Master Ghidra: 32 functions created (step 1)
Recipe of `verify/c4rmatchbatch5_20261108/RESULT_C4R-MATCHBATCH-4.md` (guards of `re/tools/ghidra_scripts/CreateLibFunctions.java`).
- Before: no `java.exe`, no `Mashed.lock*`. Backup `Mashed.rep.bak-20261113-pre-rwaudiofn` + `Mashed.gpr.bak-20261113-pre-rwaudiofn`
  (81 MB, gitignored).
- Rehearsal on master `-readOnly -noanalysis` (`rwaudio_632ba8/ghidra/rehearsal_readonly.tsv`): ok=32 skipped=0 rejected=0,
  all `CREATED_EXACT` at the `--create` sizes of `table_632ba8.tsv`, "Discarding changes".
- Write without `-readOnly` (`apply_master.tsv`): ok=32, all `CREATED_EXACT`, "Save succeeded".
- Dumps (DumpFunctions.java): before 6,481 functions, sha256 `c14368ef37cd18b1...`; rehearsal and after-write both 6,513,
  sha256 `7d51466f55aec70e...` (`ghidra/dumps.sha256`). 0x005aafe0 is named `thunk_FUN_005ab010` by Ghidra (5-byte
  `E9` jump to 0x005ab010, read from MASHED.exe.unpatched).
- `re/console/pc_funcs.csv` +32 rows, nothing else changed (`ghidra/add_pcfuncs.py` adds only the apply-list entries;
  `ghidra/pcfuncs_added.csv`).
- Pool clones not synced (no pool-slot writes); `decomp_pc.py --create` still covers reads.

## 2. Sources (step 2), `re/match/s30/`
Decompilation: `decomp_pc.py --create --port --callees --json` on the 34 + the 4 callees the handoff listed
(D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_data/decomp38.json, sha256 6570ba6eaeae7ebe...), then `g2s.py`. 0x005ae330 already
has a C4 match source (`re/match/s26/005ae330.c`); the s30 copy was dropped. Outcome for the 37 sources:

| class | rvas |
|---|---|
| **byte-identical + relocheck OK (24)**, `re/promotions/PORT-TOP-RWAUDIO-TABLE-632BA8.tsv`, `c4_reclass.py batch` dry run 22 PASS, 2 SKIP (0x0045b350, 0x004cd060 already C4) | 0045b350 004cd060 005a5ee0 005a5fd0 005a6010 005a6af0 005a6e90 005a9a70 005aa520 005aafb0 005aafe0 005ab4a0 005ab510 005ab9d0 005aba00 005ac550 005ac590 005acff0 005ada90 005add80 005addb0 005adf90 005adfc0 005ae1d0 |
| **L-126 admitted, plain EQUIV (12)**, `re/match/ADMITTED.tsv` | 005a6b30 005a6e50 005a7810 005a78b0 005a9970 005aa480 005ad030 005adb70 005ae210 005ae5b0 005ae610 005aea60 |
| **L-126 provisional (1)**, `re/match/PROVISIONAL.tsv` | 005a5e70 |

Shaping, by the disassembly (match.py --show):
- 0x005a6b30 / 0x005a78b0 / 0x005ad030 / 0x005ae210 / 0x005ae610: the list-walk argument is `head - N*4` bytes (Ghidra wrote
  `head + -N` on an `undefined4`); now EQUIV.
- 0x005a5fd0 / 0x005a6010: the compare is against the function address (`*e == (int)FUN_005a5e70`,
  `*(int*)(e+4) == (int)FUN_005a5ee0`); 0x005a5fd0 returns int (`mov eax,1`, not `mov al,1`). Both byte-identical.
- 0x005a6af0: `return 0` / `return 1` (int); byte-identical.
- 0x005aafb0: LARGE_INTEGER typedef (g2s compile fix); byte-identical.
- 0x005aa480: the callback argument is `FUN_005aa510` (was `&LAB_005aa510`, unresolved for equiv); EQUIV.
- 0x005ada90: rewritten from the disassembly: 8-byte descriptor rows at 0x006336f8 (+0 pool record, +4 size); a failed
  create unwinds rows i-1..0 and the loop **continues** (`inc esi; cmp esi,5; jb`), the tail unwinds again on failure.
  Byte-identical.
- 0x005adb70: same rows walked down from 0x00633720; EQUIV [ENC,ORDER,LAYOUT,FRAME,VALUE-FLOW] (the original keeps the
  `while` loop unrotated; 4 forms tried).
- 0x005aea60: 16-byte struct copy to RwGlobals +0x108; the free pointer named `FUN_004a460d` (CRT `_free`, pc_funcs) so
  equiv resolves it; EQUIV [ORDER].
- 0x005a5e70: **provisional**. EQUIV-IF-CALLERS-IGNORE(EAX) [LAYOUT,VALUE-FLOW], and the only caller 0x005a5f60 tests the
  result, so it does not admit. The DDK cross-jumps the two failure tails the original keeps apart; four source forms give
  the same 69.4%.
- All 37 then put into the MATCH preamble form (`rwaudio_632ba8/tools/to_s30.py`: MGLOBAL globals), and given C++-only
  casts so the exe can include them (79 VS2022 errors fixed by casts only). After each step, match.py --bytes is unchanged
  for all 37, relocheck OK for the 24, equiv EQUIV for the 12. 0x005ac590 guards its `extern LAB_005ac5c0` with
  `#ifndef MATCH_PORT` (the port reads the label at 0x005ac5c0).
- equiv.py on the branch = orch/integration's (fea5b5483, no diff), so the verdicts hold on the merged tool.
- Cross-check (`equiv_dyn.py --runs 16`, default and `--seeded`, `equiv/dyn12.tsv`): SAME 16/16 both modes for 6
  (005a6e50 005a7810 005a9970 005aa480 005ae5b0 005aea60); INCONCLUSIVE 16/16 for 6 (the circular-list walks 005a6b30
  005a78b0 005ad030 005adb70 005ae210 005ae610), diff=0 everywhere. The cross-check is a witness; the admission rests on the
  proof (L-126).

## 3. Binding (step 3)
Generator `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`, roots = the 4 earlier roots + the 34
(`D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_data/roots.txt`). Generator changes:
- a match promotion committed on the branch (`re/promotions/*.tsv`, route match) binds like a hooks.csv c4=match row;
- `PROV_BIND` += 005a5e70;
- `reuse()` takes an existing RW core body (`RwCore/RwCore_<rva>.cpp`, L-124), so 0x004c2f60 / 0x004c2fb0 / 0x004c3040 /
  0x004c30b0 / 0x004c3270 / 0x004c32b0 get no second exe copy. Side effect: three older audio-tick stops now call their RW
  core bodies (0x0042b8d0, 0x00493f70, 0x004c3ac0);
- existing wrappers keep their leading header (re-runs used to rewrite the HWINIT headers and append two lines twice);
- `EXTRA_PRE` for 005aafe0 (`thunk_FUN_005ab010`), 005ac590 (`LAB_005ac5c0` at its original address), 005ae330
  (`thunk_FUN_004522d0` -> `RwCore_004522d0`; 0x005ae4b0 is `E9` jmp 0x004522d0, read from MASHED.exe.unpatched).

Plan: 240 bind (213 match, 15 admitted, 7 replay, 5 provisional), 59 reuse, 38 stops (was 167 / 42 / 27 at HWINIT). 73 new
TUs in `exe_sources.rsp`, 13 new ORIGTOP-ONLY allowlist lines, `BINDINGS.tsv` updated 25 + added 93.

Table: `re/tools/image/codeptr_image.py --write` now binds all 32 slots 0x00632ba8..0x00632c24 to their exe bodies
(`PortTopCodePtrImage.inc`: 99 CP_IMG_FIX, was 67). **Pointer-equality guard (step 4):** the 0x005a5fd0 wrapper defines
`FUN_005a5e70` as `PortTop_Bound_005a5e70` and the 0x005a6010 wrapper `FUN_005a5ee0` as `PortTop_Bound_005a5ee0`, the same
symbols the fix-up writes into 0x00632ba8 / 0x00632bac. The OT_CODEPTR self-test reads every image slot back against
`&sym` (0 of 359 failed). The arms show the compare is true: entry 1 is reached only after the 0x005a5fd0 callback
returns nonzero.

Remaining named stops reached through the table closure: 0x004ccef0, 0x005a61b0, 0x005a6340, 0x005a6cb0, 0x005a9ab0, 0x005a9d00,
0x005aa200, 0x005aa510, 0x005ab010, 0x005ad420, 0x005adc10, **0x005ae650**, 0x005ae800, 0x005aead0, 0x005b35e0, 0x005b36b0.

## 4. Builds and checks
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK, rva-lint 300 known, NEW 0. Log `D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_build_vs.log`
  (sha256 e12eca2a17279244...).
- Object build (`MASHED_DDKOBJ=1`): Build OK. Log `..._build_ddk.log` (d30916d9fb4c7151...).
- `ddkobj.py check`: IDENTICAL 1411/1460, DIFFERENT 34, not-in-map 15, refusals 4 (wrapper-redefines-globals, not this row).
  Log `..._ddkcheck.log` (5d3b0ca50fdee2c0...). The DIFFERENT set = the 20 of the handoff + this row's 12 admitted bodies +
  0x005ad320 / 0x005ae920 (admitted in EQUIV-ADMIT-1, newly linked on this path). **All 24 byte-identical bodies of this row
  are IDENTICAL.** 0x005a5e70 is provisional (VS2022 body).

## 5. Arms (PORT-TOP-AUDIO-RWA's four)
`test-slot` outside (`PROYECTOS_TEST_HUB=off`), `mashed_lock.py run --label PORT-TOP-RWAUDIO-TABLE-632BA8` inside, private
root `D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_root` (mashed_image.bin sha256 81cc7775..., equal to the build's).
`ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1"`. Output `rwaudio_632ba8/arms_bind1.out`.

| arm | build | exit | where | log sha256 (16) |
|---|---|---|---|---|
| vs_audio | VS2022 | 0x004568A0 after 8.0 s | trap, as in the handoff (control-arm finding, not this row) | 32e43b2ce1a23470 |
| ddk_audio | object | 0x004568A0 after 7.5 s | same | ad10caf3ce98cbe5 |
| vs_init_audio | VS2022 | **0x005AE650** after 1.6 s | named stop, return `PortTop_Bound_005ae5b0+0x32` | 4f42eceb3834a7aa |
| ddk_init_audio | object | **0x005AE650** after 1.5 s | named stop, return `PortTop_Bound_005ae5b0+0x30` | 755032303008fea2 |

Before this row both init arms trapped at 0x005A5E70 (handoff section 3).

## 6. Next
1. **0x005ae650 (AudioPoolConstruct, C3 hand body, called by the init side of the 13 pool pairs in the table):** source `re/match/s26/005ae650.c` is 74.1%,
   equiv NOT-PROVEN (call-args at 0x005ae672, arg 3 `ebx_in | esi_in`). Shape or admit it, re-run the generator (it binds
   on the next run), re-run the init arms. Its callee 0x005ae800 (C3) is a stop too.
2. The control-arm trap 0x004568A0 from `_Orig_0045b930` (handoff section 3) still needs its own row.
3. Merge train: apply `re/promotions/PORT-TOP-RWAUDIO-TABLE-632BA8.tsv` (22 rows to C4 by match).

## Files (this part)
- `re/console/pc_funcs.csv` (+32); `re/match/s30/` (37 sources); `re/match/ADMITTED.tsv` (+12), `re/match/PROVISIONAL.tsv` (+1);
  `re/promotions/PORT-TOP-RWAUDIO-TABLE-632BA8.tsv`
- `verify/portorigtop_20261113/rwaudio_632ba8/ghidra/` (lists, reports, logs, `add_pcfuncs.py`), `equiv/` (`equiv12.json`,
  `dyn12.tsv`, `gen_admitted12.py`, `match_round1.txt`, `equiv_round1.txt`, `c4_dryrun.tsv`), `tools/to_s30.py`, `arms_bind1.out`
- `verify/portorigtop_20261113/tools/` (generator, plan.json, exeidx.json, newfiles.txt), wrappers under
  `mashedmod/src/mashed_re/Match/`, `PortTopCodePtrImage.inc`, `CODEPTR_IMAGE.tsv`, `BINDINGS.tsv`, `exe_sources.rsp`,
  `dual_copy_allowlist.txt`

---

# Earlier part of this row (handoff @ 0bf54ae18, kept verbatim)

# RESULT PORT-TOP-RWAUDIO-TABLE-632BA8: the RW-audio init/shutdown table 0x00632ba8 on the image path

## Verdict: HANDOFF (path decided: image code-pointer fix-up, not a per-address seed)

- Self-tests: **portstate-image self-test PASS (0 of 8 checks failed)** and **code-trap self-test PASS (0 of 6 checks
  failed)** on all 4 arms, both builds. No arm reaches the OT_TIMER self-test summary: every arm stops at a trap first
  (section 3).
- With `MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1` the table holds the original's 32 code addresses. The trap catches
  the first call, to 0x005a5e70, before anything else runs. On the object build it also names the slot:
  `execute original 0x005A5E70 slot 0x00632BA8 via "call [edi]"`. This is the expected outcome in the L0 amendment, so
  the row is about **binding the 32 pointers through the image's code-pointer fix-up** (IMG-3 / IMG-4 design,
  `verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md` section 3). It is **not** about a new per-address seed.
- Not done in this worker, so not GREEN: sources, match and binding of the 34 functions. The 32 table targets are
  functions Ghidra never created: none is in `re/console/pc_funcs.csv`, so `g2s.py` / `match.py` refuse to score them
  ("no Ghidra size ... refusing to guess the original's length"). That needs a master-Ghidra step first (section 5).
- **Dependency: this branch merges `orch/IMG-3-TEXT-TRAP` @ `477ab4407`**, which is not in `orch/integration` yet (its
  ledger row is still `working`, and it has no RESULT). This branch must merge after IMG-3.

## 1. Branch
- Branch `orch/PORT-TOP-RWAUDIO-TABLE-632BA8` was created from `orch/PORT-TOP-AUDIO-HWINIT-4627F0` @ `7e5466a15`.
- Merges, both with `git -c merge.autoStash=false`:
  - `orch/integration` @ `cdac1121d` (brings IMG-1 and IMG-2) gives `bca851b25`. Two conflicts, both append-only (rows
    from both sides kept): `re/match/PROVISIONAL.tsv` (HWINIT's 0x004627f0 against integration's 0x00498510) and
    `re/tools/dual_copy_allowlist.txt` (HWINIT's 5 lines against BIND-CRT-498510's 4).
  - `orch/IMG-3-TEXT-TRAP` @ `477ab4407` gives `704f0ed58`. No conflicts.
- No source change of this row is in the exe.
- One harness change: `audiotick_stops_arms.py` takes `ARMS_EXTRA`, extra knobs added to every arm.

## 2. The table and its 34 functions (C0 to C1 discovery)
`rwaudio_632ba8/read_table.py` reads 0x00632ba8 from `MASHED.exe.unpatched` and from `mashed_image.bin` (sha256
`81cc77758ffe93ac...`, read through IMG-1's `mashed_image.parse`). Output is in `rwaudio_632ba8/read_table.txt`. Result:
**`image equals file at all 32 slots: True`**. The 16 {init, shutdown} pairs are the ones in the HWINIT RESULT.

Discovery used `re/tools/decomp_pc.py --create` on pool slot Mashed_pool0. It is read-only: the created functions are
discarded on close. Outputs:
- `rwaudio_632ba8/decomp34_create.txt`: decompilation, callees, callers.
- `rwaudio_632ba8/table_632ba8.tsv`: rva, size Ghidra gives the created function, pc_funcs presence, table slot,
  callees.

Facts read from that decompilation:

- **All 32 table targets report "not a function entry".** None is in `pc_funcs.csv` and none has a caller. The table is
  their only reference. 0x005a5fd0 and 0x005a6010 are in `pc_funcs.csv`, with sizes 53 and 19.
- **Init/shutdown pairs.** 26 of the 32 entries are pool constructor / destructor pairs.
  - The init side calls 0x005ae650 (hooks.csv `AudioPoolConstruct`, audio, C3) with a pool record address and a flags
    word 0x408xx. Example: 0x005adf90 is `FUN_005ae650(0x10,8,4,0,&DAT_007dda28,0x40802)`.
  - The shutdown side calls 0x005ae7e0 on the same record.
- **Pairs without that pattern:**
  - (0x005a5e70, 0x005a5ee0): RW core calls 0x004c32b0 / 0x004c30b0 / 0x004c2fb0 and 0x004c2f60 / 0x004c3040 /
    0x004c3270, gated on `DAT_007dc960 == 0`.
  - (0x005a6e50, 0x005a6e90): list heads at 0x007dca24 / 0x007dca18, then 0x005a62b0. The shutdown side calls 0x005a65a0.
  - (0x005ab4a0, 0x005ab510): three 0x005aeea0 calls on 0x007dd618 / 0x007dd620 / 0x007dd61c. The shutdown side calls
    `WaitForSingleObject(...,0xffffffff)` and `CloseHandle` on the same three dwords.
  - (0x005aafb0, 0x005aafe0): `QueryPerformanceCounter` into 0x007dcdf0. If it returns 1, `QueryPerformanceFrequency`
    into 0x007dcdf8. Then 0x005aaff0. 0x005aafe0 is a 5-byte thunk to 0x005ab010.
- **The per-entry callbacks compare code pointers (pointer equality):**
  - `FUN_005a5fd0(int *e)`: `if (*e == FUN_005a5e70) { if (DAT_007dc954 == 0) DAT_007dc954 = FUN_004cd060(); return
    FUN_005aea60(DAT_007dc954) == 1; } return 1;`
  - `FUN_005a6010(int e)`: `if (*(e + 4) == FUN_005a5ee0) FUN_0045b350();`
  - This is the exception `RESULT_PORTSTATE-IMAGE.md` section 3 marks [UNCERTAIN]: "a matched body that compares a slot
    against `&FUN_x`". The fix-up and the bound 0x005a5fd0 / 0x005a6010 bodies must agree on the value of slot
    0x00632ba8 / 0x00632bac:
    - If 0x005a5e70 / 0x005a5ee0 are bound, the exe bodies compare against the same exe symbols the fix-up writes.
    - If they stay unbound, the slot keeps the original value, and the sources must compare against the original
      constant.
    - Mixing the two makes the compare false, and that silently skips the 0x004cd060 / 0x005aea60 call (init) or the
      0x0045b350 call (shutdown).
- **Callees.** Every callee of the 34 except 0x004cd060, 0x005aea60, 0x0045b350 and 0x005ae330 has a `re/match` source
  already (s19 / s25 / s26 / s27 / rwl01). Not checked here: their match percentage, and whether they are in the exe's
  bindings.

**hooks.csv:** 34 C1 rows were added with `re/tools/c1_add.py` (re-classify skill, "C1 row for a game function that has
none"), subsystem `audio`, tag `PORT-TOP-RWAUDIO-TABLE-632BA8`. Their xref evidence:
- the 32 table targets: their slot in 0x00632ba8, which 0x005a5f00 passes to 0x005a5f60 (count 0x10);
- 0x005a5fd0 / 0x005a6010: the callbacks 0x005a5f00 passes to 0x005a5f60.

No row is above C1, and no name was invented: the names stay `FUN_<rva>`.

**g2s first pass**, `re/match/s30/` (34 drafts plus `results.tsv`), with `decomp_pc.py --create --port --callees --json`:
- 0x005a5fd0: 68.4%.
- 0x005a6010: 33.3%.
- 0x005aafb0: compile-fail C2143 (the `LARGE_INTEGER *` cast).
- The other 31: no score, "no Ghidra size". Their calling convention also defaulted, because `ret_pop` needs the size.
  Decompiled, all 32 take no arguments.
- None is admitted, provisional or bound. These are drafts for the next row.

## 3. Arms (PORT-TOP-AUDIO-RWA's four, with the image and the trap)
Setup:
- Wrapping: `test-slot` outside (`PROYECTOS_TEST_HUB=off`), `mashed_lock.py run --label PORT-TOP-RWAUDIO-TABLE-632BA8`
  inside.
- Private root `D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_root`. Its `original\mashed_image.bin` is a copy of
  `mashedmod/build/mashed_image.bin`, needed for the object exe's lookup.
- Knobs: `ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1"`, plus the arm script's
  `MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 MASHED_DET_FRAMES=600`.
- Output `rwaudio_632ba8/arms_trap1.out`.

| arm | build | extra knobs | exit code | trap line | return site (map) | log sha256 (16) |
|---|---|---|---|---|---|---|
| vs_audio | VS2022 | AUDIOTICK | 0x004568A0 after 9.7 s | `execute original 0x004568A0 slot ? via "call eax"` | `_Orig_0045b930+0x24e` | 4de900eab178cd9b |
| ddk_audio | object | AUDIOTICK | 0x004568A0 after 58.0 s | same | `_Orig_0045b930+0x38` | a185d10830419127 |
| vs_init_audio | VS2022 | AUDIOINIT + AUDIOTICK | **0x005A5E70** after 10.3 s | `execute original 0x005A5E70 slot ? via "call eax"` | `_PortTop_Bound_005a5f60+0x1a` | 64b6f3cfde772e02 |
| ddk_init_audio | object | AUDIOINIT + AUDIOTICK | **0x005A5E70** after 2.7 s | `execute original 0x005A5E70 slot 0x00632BA8 via "call [edi]"` | `_PortTop_Bound_005a5f60+0x18` | e4cbb4f2d26c9563 |

What decided the path:
- In the HWINIT RESULT, the same two init arms faulted on an AV at address 0, at `_PortTop_Bound_005a5f60+0x1a`,
  because the table held 0.
- With the image, the same call site calls 0x005a5e70, the original value from the image, and the trap reports it.
- The VS2022 body calls through a register (`call eax`), so the trap cannot decode the slot there. The object build
  calls through `[edi]` and names `0x00632BA8`.

**Control-arm finding (not this row).** Without the image and the trap, the two AUDIOTICK-only controls ran 600 frames
with rc 0 (HWINIT RESULT). With them, both stop at 0x004568A0, called through `eax` from `_Orig_0045b930`. The image
supplies a code value the per-address seed did not, and nothing binds it yet. That is a new trap report for the IMG-3 /
IMG-4 code-pointer list. It is unrelated to 0x00632ba8.

Logs, in `D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_data/`: `vs_audio.log`, `ddk_audio.log`, `vs_init_audio.log`,
`ddk_init_audio.log` (sha256 prefixes in the table).

## 4. Builds and checks
- **VS2022** (`MASHED_NO_DEPLOY=1`, `MASHED_ROOT` = the private root): Build OK.
  - `build.bat` wrote `mashedmod/build/mashed_image.bin`: 434,400 B, sha256 `81CC7775...9AFCD8`, equal to IMG-1's.
  - Log `D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_build_vs.log`.
- **Object build** (`MASHED_DDKOBJ=1`): Build OK. Log `D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_build_ddk.log`.
- **`ddkobj.py check`**: IDENTICAL 1352/1387, DIFFERENT 20, not-in-map 15, refusals 4 (wrapper-redefines-globals).
  - The DIFFERENT set is the HWINIT branch's 19 plus 0x00498510.
  - 0x00498510 is integration's provisional BIND-CRT-498510 body (`PROVISIONAL.tsv`).
  - This row adds no exe body.
  - Log `D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_ddkcheck.log`.
- No G-CTRL, no B, no captures: this is a top-down row. No menu walk: no menu code changed.

## 5. Next (precise)
1. **Make the 32 targets functions** (master Ghidra, account3, through `ghidra-sweep` / SCRIBE_QUEUE, not a pool slot).
   - Create functions at the 32 rvas of `rwaudio_632ba8/table_632ba8.tsv`.
   - Regenerate `re/console/pc_funcs.csv`, so `match.ghidra_size` sees them.
   - The `--create` sizes in the TSV (101, 25, 94, ...) are what Ghidra gives on a pool clone. Check them against the
     master after creation.
2. **Sources**:
   - Re-run `g2s.py` on the 34 (drafts in `re/match/s30/`).
   - Then `match.py --bytes` and `relocheck.py`.
   - Bodies below 100% go through `equiv.py` on the merged copy (L-126): ADMITTED.tsv or PROVISIONAL.tsv.
   - Also bring in the 4 callees with no source: 0x004cd060, 0x005aea60, 0x0045b350, 0x005ae330.
3. **Bind through the image fix-up**, not a seed:
   - Add the 32 slots 0x00632ba8..0x00632c24 to the code-pointer list (`CODEPTR_SLOTS.tsv` today, `CODEPTR_IMAGE.tsv`
     with IMG-4), with exe symbols from the closure generator (`tools/audiotick_bind_gen.py`, roots += the 34).
   - Per design (b), a slot with no exe body keeps its original value, and the trap names it.
   - Run the arms with `MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1` (and `MASHED_OT_CODEPTR=1` if CP_FIX is still the
     knob).
   - Expected: no trap at 0x005a5e70. Either a later trap names the next unbound target, or `[OT_AUDIOINIT]` logs the
     0x004627f0 return.
4. **Pointer-equality guard**: bind 0x005a5fd0 / 0x005a6010 together with 0x005a5e70 / 0x005a5ee0, or compile them
   against the original constants (section 2). Add a check that slot 0x00632ba8 equals the 0x005a5fd0 comparand after
   the fix-up.
5. **Control arms**: file a row for the 0x004568A0 trap from `_Orig_0045b930` (section 3). It blocks the AUDIOTICK
   controls on the image path.

## Files
- `verify/portorigtop_20261113/RESULT_PORT-TOP-RWAUDIO-TABLE-632BA8.md` (this file)
- `verify/portorigtop_20261113/rwaudio_632ba8/`: `read_table.py`, `read_table.txt`, `decomp34_create.txt`,
  `table_632ba8.tsv`, `map_ret.py`, `arms_trap1.out`
- `verify/portorigtop_20261113/audiotick_stops_arms.py` (`ARMS_EXTRA`)
- `re/match/s30/` (g2s drafts, not bound)
- `hooks.csv`, `re/analysis/CHANGELOG.md` (34 C1 rows through `c1_add.py`)
