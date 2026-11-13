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
