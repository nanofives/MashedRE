# RESULT DDKOBJ-REGCONV (2026-11-12) - all five arms PASS

PREREG: `verify/ddkobj_20261112/PREREG_DDKOBJ-REGCONV.md` (commit e932b5a21, before any arm). Branch
`orch/DDKOBJ-REGCONV`. Tool commit c308607fb. Row kind: top-down tool row (only `re/tools/ddkobj/`, the object build).
Arms 1-3 ran before the LIVING-PC reboot. The VS2022 knob-ON log diff, arms 4 and 5 and a repeat self-test ran after
it, on claude3, against the same variant (sha256 `d284df63...`, checked unchanged after the reboot).

## Verdict

**The 38 register-convention units are recovered by generated adapters. The object build has 0
`fail:static-register-convention` refusals.** The route chosen is the first option in the row title (a generated
adapter per function), compiled by the DDK itself rather than written as naked asm. No convention is read by hand.
The second option, a non-static source that still byte-matches, is ruled out by the spike's own check: every one of
the 38 changes bytes when compiled non-static.

| arm | result |
|---|---|
| 1 byte check | 0 DIFFERENT at 100% match score. 40/43 `__ddkreg_*` bodies IDENTICAL, 1 DIFFERENT (60.2% source), 2 not linked |
| 2 tool self-test | 0 failures (before the build, and again after the reboot: `selftest2.log`) |
| 3 boots + knob-ON log diff | OFF rc 0, ON rc 0 `returned 0 after 3000 frames`. Log diff against VS2022: **0 of 1134** `[ORIG_TOP]`/`[RW_BIND]` lines differ. 2 of 177 other lines differ, both intro-movie frame counts (section 3) |
| 4 12-capture walk | **4/4 class E** (OFF, ON, OFF, ON), 12/12 equal each, freeze `pinned pos=55 hash=28538c74` |
| 5 bisect | **0x0042ad90 alone** crashed the first build (`0xC0000005`, knob OFF and ON). The other 37 units boot under both knobs (section 4) |

## 1. Method (ddkobj.py)

For a unit whose static MATCH function fails the spike's non-static check, the tool recompiles the source once with
one adapter per such function appended:

    <ret> <cc> __ddkadapt_<name>(<params>) { return <name>(<args>); }

- `<cc>` comes from the decoration the default VS2022 wrapper object gives the exe name (`_x` cdecl, `_x@N` stdcall,
  `@x@N` fastcall). Example: `0x0041b770` is `MATCH_ECX` = `__fastcall` under C++, so its adapter is `__fastcall`
  (`@Copy41b770@4`).
- The DDK emits the call from the adapter into the private convention.
- The adapter takes the exe name, and the wrapper surgery repoints the VS2022 callers to it.
- The body is renamed `__ddkreg_<rva>` (external), keeps its in-object callers, and is byte-checked at its RVA.
- **Guard:** every function of the plain DDK compile must keep its bytes and relocation targets in the adapted compile.
  Compiler-local `$L` labels are compared by position: their numbering shifts when code is appended, which was the
  only difference in 5 of the 38 units, for example the 0x0042f020 jump table.
- **Refusals:** a declaration it cannot adapt (unnamed or variadic parameter) or any changed byte keeps the VS2022
  body, with the reason in the build JSON.
- `DDKOBJ_REGCONV=refuse` restores the spike behaviour. `DDKOBJ_REGCONV=promote` reproduces the unguarded first
  build, for the bisect.

Offline probe (`REGCONV_offline_probe.tsv`): 38/38 units adapt, 43 static functions.

## 2. Arm 1: byte check (object build of the whole tree, `build1.log`)

`ddkobj.py check` (`BYTECHECK_all.tsv`, `ddkobj_summary.json`):

| | count |
|---|---|
| units / linked | 834 / 824 (9 `wrapper-redefines-globals`, 1 `skip:provisional`; 0 `static-register-convention`) |
| functions checked | 864: IDENTICAL 775, DIFFERENT 80, not-in-map 9 |

The tree has grown since the spike (583 to 834 units, mostly RW-core sources).

**Every DIFFERENT row comes from a source below 100% in match.py** (`DIFFERENT_scores.tsv`, scored in this
worktree). The highest is 0x004d4a20 at 99.4%. No 100% source is DIFFERENT, so the PREREG bar holds.

The 38 recovered units in detail:

- **43 static bodies (`__ddkreg_*`):** 40 IDENTICAL. 1 DIFFERENT: 0x00401690, a source at 60.2% (the vertex-loop
  unroll residual in its header). 2 not linked (0x0045bac0, 0x00458080: no caller remains, `/OPT:REF`).
- **Other matched functions of those units:** 21 IDENTICAL, 4 not linked (no caller).
- **43 adapters:** each one replaced the wrapper's VS2022 copy. 23 are linked. The 20 that are not linked have no live
  caller, which agrees with the static scan in section 4.

## 3. Arms 2 and 3

- **Tool self-test** (`ddkobj.py selftest`): 0 failures. Run before the build against the merge train's default
  objects, with a new register-convention fixture (body linked as `__ddkreg_00400060`, refused under
  `DDKOBJ_REGCONV=refuse`) and adapter-text checks (fastcall, array param, unnamed and variadic refused).
- **Knob boots** of the variant `mashed_re_ddk.exe` (sha256 `d284df63...`), root `D:/MashedWT/DDKOBJ-REGCONV_root`,
  `MASHED_DET_FRAMES=3000`:
  - knob OFF: rc 0, 95.7 s.
  - knob ON: rc 0, 94.9 s, `[ORIG_TOP] WinMain 0x00492370 returned 0 after 3000 frames`.

  The spike's 583-unit build crashed knob OFF and knob ON at 1.5 / 5.1 s when it promoted these statics without
  adapters.
- **Repeat self-test after the reboot** (`selftest2.log`): `[ddkobj selftest] OK: 0 failure(s)`, 25 PASS lines,
  including `register-convention static -> ok, kept as its own body` and `DDKOBJ_REGCONV=refuse -> the DDKOBJ-SPIKE
  refusal`.
- **Knob-ON log diff** (`logdiff_on.py`, output `LOGDIFF_on.txt`). VS2022 `mashed_re.exe` knob ON (`vs_on`, after the
  reboot): rc 0, 56.7 s, `returned 0 after 3000 frames`. It is diffed against **`ddk_on2`**, a second knob-ON boot of
  the variant (rc 0, `returned 0 after 3000 frames`) taken after the reboot.

  The pre-reboot `ddk_on.log` is not used for the diff, because two inputs changed between it and `vs_on`. The
  root's `mashed_re_settings.bin` was rewritten to the walk's baseline at 12:18 (`ddk_on` read `music=7`, `vs_on`
  `music=10`). The display reported `refresh=120` before the reboot and `refresh=60` after it. `ddk_on2` restores
  the baseline settings inside the machine lock, so both runs read the same file.

  | lines | vs_on | ddk_on2 | differing |
  |---|---|---|---|
  | `[ORIG_TOP]` / `[RW_BIND]` (heap pointers, timings, thread ids masked, RVAs and original-address cells kept) | 1134 | 1134 | **0** |
  | other lines (B17 region-map bodies dropped, pointers / hex / timings masked) | 177 | 177 | 2 |

  The 2 other lines are `INTRO movie idx=1 empire.mpg` (frames 297 vs 312) and `idx=3 renderware.mpg` (141 vs 265),
  both counts of frames presented during movie playback.

  The B17 region map (`0x00400000..0x009FFFFF`, inside the exe's `.data`) is left out. Its shape flips between runs of
  one exe. In the spike's data the VS2022 exe shows one RW region in `boot_vs_on.log` and a fragmented
  WRITECOPY/RW map in `vs_on.log` / `vs2_on.log` / `vs3_on.log`, and the DDK exe shows the same split
  (`boot_ddk_on.log` vs `fin_on.log`). Here `vs_on` has one RW region and `ddk_on` the fragmented form.

## 4. Arm 5: the static that crashed the first boot is 0x0042ad90

**Bisect** (`BISECT_promote38.tsv`, `bisect_units.py` under `DDKOBJ_REGCONV=promote`, knob OFF, 40 s per probe).
FAIL = an exception exit code. OK = no exception before `knob_boot.py`'s 40 s timeout kill (exit `0x00000001`).

| probe | units | result |
|---|---|---|
| full1_off | 38 | FAIL `0xC0000005` (log ends after `R2-2 gamesave load`, the spike's crash point) |
| b1_1 .. b1_6 | 19, 9, 4, 2, 1, 1 | OK, FAIL, OK, OK, OK, OK -> culprit 0x0042ad90 by elimination |
| full2_off | 37 (all but 0x0042ad90) | OK |

**Confirmation** (`confirm_probes.py`, same build and rule, `confirm.log`). The halving reached 0x0042ad90 by
elimination, so it was booted alone:

| probe | units | result |
|---|---|---|
| solo_off | 0x0042ad90 | FAIL `0xC0000005` |
| solo_on | 0x0042ad90 | FAIL `0xC0000005` |
| rest37_on | the other 37 | OK |

So **0x0042ad90** (`re/match/s06/0042ad90.c`, `MatchS06_0042ad90`, exe name `_Find42ad90`, `__cdecl` in the VS2022
wrapper) is the one unit whose unguarded promotion crashes, under both knobs. No second culprit exists among the 38.

Mechanism, from the static scan below: the VS2022 `MenuNavSM` functions call `Find42ad90` with stack arguments. The
first build bound that name straight to the DDK static body, whose bytes fail the non-static check (register
convention). In the guarded build the cdecl adapter takes the name, and the body `__ddkreg_0042ad90` is byte-checked
IDENTICAL (64 bytes, `BYTECHECK_all.tsv`). The other statics in the table do not crash a 40 s boot when promoted
unguarded. [UNCERTAIN: whether any of them would fail on a path a 40 s knob-OFF/ON boot does not reach.
The guarded build gives every one of them an adapter, so this does not affect the shipped route.]

### Static scan (before the bisect)

`REGCONV_callers.tsv` scans the default VS2022 objects for references to each static's exe name. References from the
VS2022 copies of substituted matched functions are excluded, because those copies are dead in the variant.

The first build gave these exe names straight to the DDK register-convention body. So a live reference meant a
stack-argument call into a register-argument body.

16 statics have live callers:

| static | exe name | live caller(s) |
|---|---|---|
| 0x0041f710 | CR_41f710Body | `_CR_41f710` |
| 0x00413bc0 | FUN_00413bc0 | `_HudUvRect413bc0` |
| 0x0041b770 | Copy41b770 | address taken by `AutoRegister` (install), no call |
| 0x0041bc50 | HudRec29DispatchBody | `_FUN_0041bc50` |
| 0x0041c2d0 | HudDispatchDraw4Body | `_FUN_0041c2d0` |
| 0x0041c320 | ParticleEmitterCtorC_impl | `_ParticleEmitterCtorC` |
| 0x0041cd20 | ParticleEmitterCtorD_impl | `_ParticleEmitterCtorD` |
| 0x0041ad60 | ParticleEmitterCtorA_impl | `_ParticleEmitterCtorA` |
| 0x004277a0 | TextCtrlCodeRemapBody | `_TextCtrlCodeRemap` |
| 0x00427840 | FontText_UTF16WidenCopyBody | `_FontText_UTF16WidenCopy` |
| 0x0042aad0 | MenuDimSetBody | `_MenuDimSet` |
| 0x0042ad90 | Find42ad90 | `MenuNavSM`: `Nav_Init`, `Nav`, `BuildRecords`, `KvLookup`, `NavCountItems`, `Nav_PromptId`, `Nav_DevGoto` |
| 0x0048cee0 | Orig_0048cee0 | `_PortTop_PS3_Cee0_0048cee0` |
| 0x0046c5f0 | FUN_0046c5f0 | `Collision::TriangleFaceNormal` |
| 0x0045ba10 | FUN_0045ba10 | `_PortTop_PWI_Call0045bae0` |
| 0x00458020, 0x00458080 | FUN_00458020/80 | `_PortTop_PWC_NotRun_004582f0` |

## 5. Arm 4: 12-capture walk

`CAPS_runs.tsv` (`capflake_runs.py` through `run_arms.sh`, variant exe, root restored to the settings baseline per run):

| run | knob | rc | freeze | class | equal |
|---|---|---|---|---|---|
| ddk_OFF_25216 | OFF | 0 | `pinned pos=55 hash=28538c74` | E | 12/12 |
| ddk_ON_17747 | ON | 0 | same | E | 12/12 |
| ddk_OFF_24603 | OFF | 0 | same | E | 12/12 |
| ddk_ON_12461 | ON | 0 | same | E | 12/12 |

**4/4 class E, the PREREG pass.** All four runs give the same 12 capture hashes (`83c8cbb9...` to `6d41bc3a...`). A
fifth OFF run from before the reboot (`ddk_OFF_9390`) also gave E 12/12 with the same hashes. The freeze hash
`28538c74` is the one `RESULT_PORT-TOP-KNOBON-CAPFLAKE.md` arm A3 recorded for the default build.

## 6. Files

- **Committed** (`verify/ddkobj_20261112/`): PREREG, this RESULT, `BYTECHECK_all.tsv`, `ddkobj_summary.json`,
  `DIFFERENT_scores.tsv`, `REGCONV_callers.tsv`, `REGCONV_offline_probe.tsv`, `run_arms.sh`, `logdiff_on.py`,
  `LOGDIFF_on.txt`, `BISECT_promote38.tsv`, `CAPS_runs.tsv`, `confirm_probes.py`.
- **Tools:** `re/tools/ddkobj/ddkobj.py`, `re/tools/ddkobj/bisect_units.py` (`BISECT_ROOT`, `BISECT_DATA`,
  `BISECT_LABEL`).
- **Raw files** (`D:/MashedWT/DDKOBJ-REGCONV_data`), sha256:
  - `build1.log` `b81c3574007d8dc93bb93b7edaabe742b60d67ac349e484cd1a2b14861930e02`
  - `ddk_off.log` `f7c94a9a48d6a7c43e0479f6001a25035feb2d43f9b9aa87323d6e46adb09213`
  - `ddk_on.log` `1ae2e297b01bca9f764f0220e6dc2ee29815fe55332ae4f442ed633c37c9669f`
  - `arms1.log` `150725c74c00f515bc17009f5846f94e0701e996bfd7f5af1f00c7d3b3c0d381`
  - `vs_on.log` `e87cf5d9cf49f4c199e4d92052834d3a5af74e477f3a6cb8b58ae882d2e8cd54`
  - `ddk_on2.log` `10db3cf2c84e501888d4b8ace1973de62b3374caec005f4edde92a1ec5c2f73f`
  - `arms2.log` (vs_on, walk, bisect) `bab8de0384c8bc32764b4c47f5d836c3dba668d9a1c74d6fdd4baf817bb50214`
  - `confirm.log` `bc972e229b890ad146749791acd7532470662fcf553ac6f1fd8fc6ccd0950bbe`
  - `selftest2.log` `b53892780f6d1036bfef28552a0879c357f0e27b076315539b7f0e5986d4f125`
  - `bisect/full1_off.log` `b1246ed364619e82cf27fa4e1d44234c2aa761ebb0f32d28d71620c4ba007b9c`
  - `mashed_re_ddk.exe` (worktree `mashedmod/build/ddkobj/`) `d284df63d970ff4ee7b4444abb63a3d3df4acb9d3cb10a5807d1c3770b36b406`
- **Default build:** unchanged. The tool runs only under `MASHED_DDKOBJ=1`, and no source or `build.bat` line changed.
  Default gates not run (top-down row).
