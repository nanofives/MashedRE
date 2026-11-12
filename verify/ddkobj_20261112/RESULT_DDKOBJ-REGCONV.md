# RESULT DDKOBJ-REGCONV (2026-11-12) - HANDOFF (reboot of LIVING-PC, arms 3-5 partly run)

PREREG: `verify/ddkobj_20261112/PREREG_DDKOBJ-REGCONV.md` (commit e932b5a21, before any arm). Branch
`orch/DDKOBJ-REGCONV`. Tool commit c308607fb. Row kind: top-down tool row (only `re/tools/ddkobj/`, the object build).

## Verdict so far

**The 38 register-convention units are recovered by generated adapters. The object build has 0
`fail:static-register-convention` refusals.** The route chosen is the first option in the row title (a generated
adapter per function), compiled by the DDK itself rather than written as naked asm. No convention is read by hand.
The second option, a non-static source that still byte-matches, is ruled out by the spike's own check: every one of
the 38 changes bytes when compiled non-static.

Done: arm 1 (byte check), arm 2 (tool self-test), arm 3 for the variant (knob OFF and ON boots). Not done (reboot):
arm 3's VS2022 knob-ON log diff, arm 4 (12-capture walk), arm 5 (bisect naming the unit that crashed the first boot).
For arm 5 a static candidate list is in hand (section 4).

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
- **Not run:** the `vs_on` comparison boot. It was queued on the test slot's memory gate when the reboot notice came.

## 4. Which unit crashed the first boot: candidates (static), bisect not run

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

[UNCERTAIN: which of these crashed the first boot. The candidates come from a static scan. Arm 5 (the
`DDKOBJ_REGCONV=promote` bisect over the 38 units) has not run.]

## 5. Handoff: what remains (resume here after the reboot)

1. From the worktree root: `bash verify/ddkobj_20261112/run_arms.sh` with `STEPS="boots walk bisect"`. The script waits
   on PRIORITY-HOLD/SOLO, runs test-slot outside and mashed_lock inside. The `ddk_off` / `ddk_on` boots may be
   skipped (already rc 0). To run only the rest: `STEPS="walk bisect"`, plus one `vs_on` boot for the ON log diff
   (`boot vs_on` in the script).
2. Arm 3 log diff: compare `D:/MashedWT/DDKOBJ-REGCONV_data/ddk_on.log` against `vs_on.log`, masking
   `[ORIG_TOP]`/`[RW_BIND]` lines, pointers and timings, as the spike did.
3. Arm 4: `CAPS` rows in `D:/MashedWT/DDKOBJ-REGCONV_data/caps/runs.tsv`. Pass is 4/4 class E.
4. Arm 5: the bisect writes `D:/MashedWT/DDKOBJ-REGCONV_data/BISECT_promote38.tsv`. Name each culprit against the
   table in section 4. If the 38-unit promote build does not crash, rerun with all units (`ddkobj.py build --units all`
   under `DDKOBJ_REGCONV=promote`).
5. Then `set DDKOBJ-REGCONV review` with this RESULT.

## 6. Files

- **Committed** (`verify/ddkobj_20261112/`): PREREG, this RESULT, `BYTECHECK_all.tsv`, `ddkobj_summary.json`,
  `DIFFERENT_scores.tsv`, `REGCONV_callers.tsv`, `REGCONV_offline_probe.tsv`, `run_arms.sh`.
- **Tools:** `re/tools/ddkobj/ddkobj.py`, `re/tools/ddkobj/bisect_units.py` (`BISECT_ROOT`, `BISECT_DATA`,
  `BISECT_LABEL`).
- **Raw files** (`D:/MashedWT/DDKOBJ-REGCONV_data`), sha256:
  - `build1.log` `b81c3574007d8dc93bb93b7edaabe742b60d67ac349e484cd1a2b14861930e02`
  - `ddk_off.log` `f7c94a9a48d6a7c43e0479f6001a25035feb2d43f9b9aa87323d6e46adb09213`
  - `ddk_on.log` `1ae2e297b01bca9f764f0220e6dc2ee29815fe55332ae4f442ed633c37c9669f`
  - `arms1.log` `150725c74c00f515bc17009f5846f94e0701e996bfd7f5af1f00c7d3b3c0d381`
  - `mashed_re_ddk.exe` (worktree `mashedmod/build/ddkobj/`) `d284df63d970ff4ee7b4444abb63a3d3df4acb9d3cb10a5807d1c3770b36b406`
- **Default build:** unchanged. The tool runs only under `MASHED_DDKOBJ=1`, and no source or `build.bat` line changed.
  Default gates not run (top-down row).
