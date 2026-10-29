# RESULT D4-WSB-PORT-SOLV-KV: 4 rows ported from disassembly, gates unchanged, Arm U INVALID (M4 site only), no promotion

PREREG `PREREG_D4-WSB-PORT-SOLV-KV.md` committed alone at `54b88250b`, amendment 1 committed alone at `5eddde319`, each
before the arm pass it governs. Branch `orch/D4-WSB-PORT-SOLV-KV` off `orch/integration` @ `a087d5bd9` (L-56 flip refs).
No original launch in this step (audio endpoint absent).

## Per-row outcome

| row | before | after | port | Arm U (unmutated port vs original bytes) |
|---|---|---|---|---|
| `0x0056a7a0` | C2 mapped | C2 **impl** | `Collision/RwpVtableKV12.cpp` `FUN_0056a7a0_impl`, reg `L2_FUN_0056a7a0` | 300/300 |
| `0x0056aae0` | C2 mapped | C2 **impl** | `FUN_0056aae0_impl`, reg `L2_FUN_0056aae0` | 300/300 (with its callee) |
| `0x0056ac40` | C2 mapped | C2 **impl** | `FUN_0056ac40_impl`, reg `L2_FUN_0056ac40` | 300/300 (arena + float return) |
| `0x0056adb0` | C2 mapped | C2 **impl** | `FUN_0056adb0_impl`, reg `L2_FUN_0056adb0` | 300/300 + 300/300 targeted (whole chain) |

No C3: there is no Frida / shadow A/B diff (`re/CONFIDENCE.md`). The original-side shadow leg is the next step.

## Frames (from disassembly, `re/tools/disasm_fn.py` over `MASHED.exe.unpatched`)

The decomp mis-models every call in this chain. The frames come from the pushes and the caller's cleanup:

| call | words | built at | cleanup |
|---|---|---|---|
| `0x0056b310` -> `0x0056adb0` (`call [esp+0x384]` at `0x0056b630`) | 69 | `sub esp,0x10c` `0x0056b57c`, `rep movsd` 0x43, 2 pushes | `add esp,0x114` `0x0056b645` |
| `0x0056adb0` -> `0x0056ac40` (x2) | 15 | `0x0056aedf..0x0056af63`, `0x0056b0dd..0x0056b167` | `add esp,0x3c` `0x0056af84` / `0x0056b185` |
| `0x0056adb0` -> `0x0056aae0` | 56 | `0x0056b000..0x0056b0d7` (14 words, `rep movsd` 0x22 of W3..W36 from `[ebp+0x10]`, 8 pushes) | `add esp,0xe0` `0x0056b0e3` |
| `0x0056aae0` -> `0x0056a7a0` | 39 | `0x0056abc8..0x0056abf8` (3 stored, `rep movsd` 0x22 of w15..w48 from entry+0x3c, push delta, push idx) | inside `add esp,0x9c` `0x0056ac13` |

The two `0x56af69` / `0x56b16d` "locals" in the `0x0056adb0` decomp are those calls' return addresses. The 39-word
count settles the U-9589 / U-9590 frame question from the call site itself: `in_stack_00000098` = v38 = delta,
`in_stack_0000009c` = v39 = idx (the word count the PORT-SOLV RESULT gave from decomp text). The U rows stay open
anyway: the PREREG tied their closure to Arm U GREEN (see below).

Constants, all `.rdata`, embedded as bits (read 2026-10-29): `0x005e5a40` `3f800000`x4, `0x005e5a70` `80000000`x4,
`0x005d757c` `00000000`, `0x005cc32c` `3f000000` (0.5), `0x005cc320` `3f800000` (1.0), `0x005cc568` `42c80000` (100.0).

Port rules carried in the TU header: SSE parts with the same intrinsics lane for lane, x87 parts as single-precision
SSE2 ops (equal to PC_24 x87 except out-of-float-range intermediates and two-NaN payloads), `fld/fstp` moves with
signalling-NaN quieting (`X87Move`), `fistp` by inline x87 (`X87Fistp`), callees called through their original RVA.

## Default-OFF

- `L2_` names: installed only by `MASHED_HOOK_ONLY` or `MASHED_HOOK_LANE2=1` (`Core/HookSystem.cpp:244-253`).
- `asi_sources.rsp` only. `exe_sources.rsp` unchanged, so `mashed_re.exe` compiles no new code. The other source edit
  is a 2-line comment in `RwpVtableKV1.cpp` (also `.asi` only).
- Lints: `scripts/lint_rva_bodies.py` OK (16 known CROSS-TARGET, 0 NEW), `scripts/lint_inline_asm_abs.py` OK.

Build (`MASHED_NO_DEPLOY=1`, worktree): clean, no warning from the new TU. `mashed_re.exe`
`fede294b91e7d940fd5e8ddc46ea63b5a30ac5bab6d85b7ae555f2c7ee7ac635`, `mashed_re_dev.asi`
`1a162f7c092d0df4d3ee0c530aa7c13dfd8ac78771177ddb9dc3e5045d03fa6d`. `original/mashed_re_dev.asi` not touched, the
`.asi` not deployed anywhere (no original-side run).

## Arm U (offline Unicorn)

`uc_arm.py`: Unicorn runs the original's own bytes (CW `0x007f`, MXCSR `0x1f80`). The harness compiles the TU text
between `namespace mashed_re {` and `// --- registrations` verbatim (or one mutant), with thunks at the four RVAs to the
port bodies. Same arena at `0x20000000` on both sides, compared bitwise. Summary: `uc_summary.tsv`, `uc_summary.json`.

| variant | 0056a7a0 | 0056aae0 | 0056ac40 | 0056adb0 | 0056adb0A | read-out |
|---|---|---|---|---|---|---|
| port | 300/300 | 300/300 | 300/300 | 300/300 | 300/300 | **PASS, 1500/1500** |
| M1 (a7a0 add order) | 156/300 | 211/300 | 300/300 | 68/300 | 70/300 | detected (695) |
| M2 (ac40 sum in double) | 300/300 | 300/300 | 216/300 | 300/300 | 299/300 | detected (85) |
| M3 (aae0 update in double) | 300/300 | 259/300 | 300/300 | 120/300 | 134/300 | detected (387) |
| M4 (adb0 limit: round -> trunc) | 300/300 | 300/300 | 300/300 | 300/300 | 300/300 | **not detected** |

Cells are matching cases. Verdict as preregistered: **INVALID** for the M4 site. Port side 0 mismatches, Unicorn 0 errors.

Why M4 cannot show here (pass 1 led to amendment 1, pass 2 kept it at 0): a Unicorn code hook on the original saw
the `fistp` at `0x0056b264` run 15 times over the 600 `0x0056adb0` cases, every operand in (0, 0.29), where round and
truncation agree. That follows from the code: the new limit is computed only when `(double)limit > est*100 + 1`
(`fcompp` at `0x0056b249`), and the operand is `est * (float)P1_4` (`0x0056b254` / `0x0056b256`). With the initial
`limit = P1_4 + 1` (`0x0056afda`) this gives operand < `P1_4^2 / 100`, under 0.5 for every `P1_4 <= 7`. The
generator draws `P1_4` (vec4 group count) in 1..6. After one update the limit is 1, and the bound is then 0. So the
rounding at `0x0056b264` is untested by this arm. It needs `P1_4 >= 8` cases. The amendment ruled out a second
amendment in this step.

Pass 1 (1200 cases, before the amendment) gave the same port 1200/1200 and M4 0. Kept raw as `uc_summary_pass1.tsv`.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this build's `mashed_re.exe`
(`fede294b`), test slot + `mashed_lock.py` (queued about 70 min behind the D3-SUBSTATE-SETTLE-ORDER solo window and two
merge trains), `A4_ROOT=D:/MashedWT/D4-WSB-PORT-SOLV-KV_root`. All four arms self-exited, exit 0.

| arm | file | sha8 | pre-flip ref | post-flip ref (`merge_train.json` @ base) |
|---|---|---|---|---|
| H_DEF | step.csv | `5943c5f1` | `5943c5f1` | `5943c5f1` |
| H_DEF | gates.csv | `364350ae` | `364350ae` | `306186d4` |
| H_DEF | ptrace.log | `52052890` | `52052890` | `52052890` |
| B_DEF_K0 | step.csv | `3a03670c` | `3a03670c` | `3a03670c` |
| B_DEF_K1 | step.csv | `9fcdbd3a` | `9fcdbd3a` | `9fcdbd3a` |
| B_DEF_K26 | step.csv | `53778664` | `53778664` | `53778664` |

All six equal the **pre-flip** refs. That is the expected set for this branch: the base `a087d5bd9` carries only the
L-56 ref change (`merge_train.json`), and the flip code itself (`f30f7401d`, `MASHED_PICKUPS_PREGO` / `MASHED_ICONCUBE`
default ON) reached `orch/integration` later, at `1b70ca47a`. The exe source set is unchanged by this step, so after the
merge the gates are the flip's (`306186d4`), and the merge train's own gate run on the merged tree is the check for that.

## Tracker transaction (re-classify)

- `hooks.csv`: the 4 rows stay **C2**, status `mapped` -> `impl`, `file` = the port, dated note (port, frames, Arm U
  numbers, "not a Frida diff, stays C2", the U row that stays open).
- `re/analysis/CHANGELOG.md`: 4 lines below `<!-- ENTRIES -->` (`C2->C2 mapped->impl`).
- U-9589 / U-9590 / U-9591 / U-9592 stay open (PREREG: closure only on Arm U GREEN). No new U or S rows.

## Next

1. Original-side shadow A/B, census scenario (track 0, mode 10, 4 cars), one `L2_` hook alone per boot, as in
   `D4-WSB-PORT-SOLV` (`drive_ps.py` / `run_shadow6.py`), needs the audio endpoint. `0x0056ac40` returns float10 on
   the original side, read with the `DIVERGENT_FLOAT10` limits of `shadow_ab_report.py`. CLEAN -> C3 through
   re-classify.
2. Arm U M4 site: add `P1_4 >= 8` `0x0056adb0` cases (new PREREG), then close U-9589..U-9592 with the disassembly cites
   above if Arm U is GREEN.

## Files

Committed: `PREREG_D4-WSB-PORT-SOLV-KV.md`, this RESULT, `uc_arm.py`, `uc_harness_main.inc`, `uc_summary.tsv`,
`uc_summary.json`.

Raw (not committed), `D:/MashedWT/D4-WSB-PORT-SOLV-KV_data/`:

| file | SHA-256 |
|---|---|
| `decomp4.txt` | `dfa0bd0f4b236564b7a54059ffc51cc956f74703a46cf617db1d6c5433e02049` |
| `dis_0056a7a0.txt` | `a888aa5a5372495bdd818159baf44d9cc025397c27cbac34e03622a4489f057b` |
| `dis_0056aae0.txt` | `c97fcfbc30650d5795a45dd3d0a728a5793706cc2bf93168f4e0ff433f5f23d6` |
| `dis_0056ac40.txt` | `c95ea4b48d72b8137c07d64e744d617ce6778cb0d104b4800b0367abca3b6838` |
| `dis_0056adb0.txt` | `2d069b3138d6f2082e8511bfa70f2d92634b5b63a85407e3b93af8f3a8b80fe2` |
| `dis_0056b310.txt` | `81661cb545a752f295d51532417615fa9e8e35068cddd7e808b44ffe09ae5fe6` |
| `uc/cases.bin` | `7167c1d61e0f71756ac9b0d86fec65d2f3c2e40b5b8c4730962b257e1e0a6725` |
| `uc/out_port.bin` | `f6a492c6f38fdf97520016338b48623d6738cb6a760b7874a4b88496a87ba129` |
| `uc/out_M1.bin` | `6c6b02e026e727bca164f0a9309117f880580d24a0674d92e89232c8baf5e2ee` |
| `uc/out_M2.bin` | `4238c63b49d357d68546122038fb97eb5d17ecf9a5bbec39bb28bcfa56fcba14` |
| `uc/out_M3.bin` | `68f6647d30d450a62564d9c5ec2935254506319846eeeffee23c62a24ffe9b14` |
| `uc/out_M4.bin` | `f6a492c6f38fdf97520016338b48623d6738cb6a760b7874a4b88496a87ba129` (= port) |
| `uc_run.log` (pass 1) | `0bdb04d7c76b45f421c7f3d26e0d90316a19636c4ab35e3b5313ae5b5fbe7c28` |
| `uc_run_pass2.log` | `309ee5b20bd445cd068cdb894b8169fc0fe52dec25876f7bac81212dd8721d89` |
| `uc_summary_pass1.tsv` | `54bf53a75a68ce6da589cf9c1ac4e2a4330463070c718e592e3f96a4d61dbf81` |
| `build1.log` | `aaf86371ae5abed8ca585cd06839086fb2ae08d6d63d5c3b4409146d95c3cc11` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/RUNLOG.txt` | `d7fea771352f0b99714ff5e54f99265e0eee94315640148f440db60e5483ab53` |

Processes spawned (only these, all self-exited): gate arms 14104, 12088, 4748, 20864 (`mashed_re.exe`); the Unicorn
harness exes (one per variant, suspended-launched by `uc_arm.py`, waited on). No MASHED.exe started. No watcher left
running.
