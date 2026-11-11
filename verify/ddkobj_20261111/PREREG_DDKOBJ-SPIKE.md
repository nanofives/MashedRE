# PREREG DDKOBJ-SPIKE (2026-11-11)

Written before the runtime arms. Spike row, brief `log/orch/briefs/DDKOBJ-SPIKE.md`.

## Change under test

An opt-in **build variant** (`MASHED_DDKOBJ=1`, `mashedmod/build.bat` block after the exe link, tool
`re/tools/ddkobj/ddkobj.py`) that writes a second exe, `mashedmod/build/ddkobj/mashed_re_ddk.exe`. It links the XP DDK
objects of the byte-matched `re/match` sources in place of their VS2022 compile. Not a runtime knob. With
`MASHED_DDKOBJ` unset the default build is the same: same sources, objects and link line.

## Arms and pass bars

1. **Default unchanged.** Rebuild the worktree with the variant block present and `MASHED_DDKOBJ` unset. Pass: the
   generated `build/obj/exe/link.rsp` is identical to the pre-change one (sha256), the per-object sha256 list is identical,
   and the exe's section bytes are identical (header timestamp/checksum excluded). Then the default gates
   (`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`) must equal `merge_train.json` refs before any enqueue.
2. **Byte check (static).** `ddkobj.py check` on the variant: every substituted function's bytes in the linked image
   compared with `MASHED.exe.unpatched` at its RVA, relocation fields masked, absolute data operands compared exactly.
   Expected: IDENTICAL for every source that `match.py` scores 100%. Any other result is a finding, not a pass.
3. **Runtime, knob OFF and ON.** Variant exe, `MASHED_DET_FRAMES=3000`, knob OFF and `MASHED_ORIG_TOP=1`, against the
   VS2022 exe from the same tree. Pass: both run to their own exit (ON: `WinMain 0x00492370 returned 0 after 3000
   frames`). Then the 12-capture nav walk (`capflake_runs.py`, OFF@dflt ON@dflt OFF@dflt ON@dflt) on both exes. Each
   difference is explained (which side matches the original) or listed as open.
4. **Float (offline).** 0x00443300 in the linked variant exe vs the original's bytes under Unicorn, the inputs of
   `verify/portstate_20261109/aispline_ddk/fuzz_00443300.py` (seed 7, 20000). Expected 0 differences at PC_24.

A crash in arm 3 is bisected by unit set (`re/tools/ddkobj/bisect_units.py`) and reported with its cause.
