# RESULT PORT-TOP-AUDIOSTOP-5ADC10: 0x005adc10 as a provisional body on the image path

## Verdict: GREEN (0x005adc10 bound as a provisional body; the init arms get past 0x005ADC10 and stop at the next named stop, 0x005AD770)

- **Self-test: PASS in both arms, both builds.** portstate-image PASS (0 of 8 checks failed), OT_CODEPTR PASS (0 of 367
  checks failed; 188 slots, 11 bound to an exe body, 177 to a stop thunk), code-trap PASS (0 of 6 checks failed,
  459/459 pages).
- **Arms** (2026-11-15, test-slot slot 1/8 after the 10 min memory wait, then queued on mashed_lock behind
  RO-REF-CONTROL-NOISE, lock taken by this run as pid 21072 and released):

  | arm | build | exit | where | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005AD770** after 11.0 s | named stop, return 0x0023AFF0 = `_PortTop_Bound_005adc10+0xe0` (`mashedmod/build/mashed_re.map`) | de946b834b083b84 |
  | ddk_init_audio | object | **0x005AD770** after 7.5 s | named stop, return 0x00238D90 = `_PortTop_Bound_005adc10+0xe0` (`mashedmod/build/ddkobj/mashed_re_ddk.map`) | 63910cf103e0aabb |

  - No stop at 0x005ADC10 any more in either build. The stop line reads
    `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005AD770 (DAT_006904e8 = 0, DAT_006904ec = 0)`.
  - The return lands in 0x005adc10's body in both builds, so the call is the source's final `else` arm
    (`FUN_005ad770(...)`, entry type not 0x2000 / 0x4000 / 0x1000). Which entry type the table held is not logged.
  - Last line before the stop in both logs: `0x004627f0 sound engine HW init: provisional exe body`, the same as the
    previous row.
- **New rows filed** (`roadmap.ps1 add`, section D4): `PORT-TOP-AUDIOSTOP-5AD770` (the new stop; `re/match/s26/005ad770.c`
  is 69.9%) and `TOOL-EQUIV-PUSHECX-SLOT` (the equiv gap in section 1).
- Gates: not run (top-down row). No menu walk: no menu code changed.

## 1. equiv and shaping (evidence in `audiostop_5adc10/`)
- `re/tools/match/equiv.py` @ ca7473532, no diff to orch/integration. Verdict on the committed source (`equiv.txt`):
  `0x005adc10	NOT-PROVEN	call-regargs: region ours +0x23 / orig 0x005adc33 call 0x5ad910: ecx ...[p+0x8] | ...[p+0x18]`.
  The 78.9% source gave the same verdict at the same call.
  - The difference is the value left in ecx at the call to 0x005ad910 (a dead scratch load in both bodies).
  - 0x005ad910 starts with `push ecx` (0x005ad910). After two more pushes it takes that slot's address
    (`lea ecx,[esp+8]` at 0x005ad927) and writes 0 into it (`mov dword ptr [esp+0x14],0` at 0x005ad932) before the call
    to 0x005ad770 at 0x005ad93a. So the incoming ecx is not read.
    **[UNCERTAIN]: this comes from reading the disassembly (`odis.py 0x5ad910`). No tool proved it.** Its test is row
    `TOOL-EQUIV-PUSHECX-SLOT`.
  - Not admitted. Listed **provisional** in `re/match/PROVISIONAL.tsv` (L-126), knob path only, never C4.
- **Shaping, one attempt** (`shaping.tsv`, sources in `variants/`): 13 source forms, best 92.2% (regs 99.2%, ops 99.2%,
  128/128 instructions), was 78.9%.
  - The ternary for the 0x1000 arm gives the original's `jmp` + `xor eax,eax` (v1, 85.9%).
  - The index-form rollback loop (`((int *)param_1)[uVar4 * 8 - 4]`, counting uVar4 down) gives the original's separate
    counter `mov edi,ebx` (v2, 92.2%).
  - Residual (`match_after.txt`): the entry is `mov esi,[esp+0x14]; add esi,0x1c` where the original has
    `mov eax,[esp+0x14]; lea esi,[eax+0x1c]` (0x005adc21..), and eax/ecx/edx are renamed in the 0x2000 arm
    (0x005adc33..) and in the 0x4000 arm. A struct-index form, a pointer-typed param, a named base copy and a pointer
    computed per iteration did not move it (64.1% to 92.2%).
  - Behaviour of the committed source against the 78.9% one: the 0x1000 ternary is the same expression. The rollback
    loop reads byte offset `uVar4 * 0x20 - 0x10` from param_1 for uVar4 = n..1, which are the same addresses in the same
    order as the old pointer walk.
- `match.py --bytes`: not byte-identical (92.2%), so this body is not called matched and there is no promotion TSV.

## 2. Binding
- `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`: `PROV_BIND |= {'005adc10'}`.
- **Generator fix.** The generator read pending match promotions only from `re/promotions/*.tsv`. The merge train
  moves an applied TSV to `re/promotions/applied/`, and `hooks.csv` row 0x004cd060 keeps its earlier frida-route C4 note
  with no `c4=match`. So the re-run turned 0x005a5fd0's callee 0x004cd060 from `PortTop_Bound_004cd060` into a named
  stop. The generator now also reads `re/promotions/applied/*.tsv`. With that, 0x004cd060 binds again and
  `PortTop_005a5fd0.cpp` is unchanged.
- Generator re-run with the same roots as the table row (`D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_data/roots.txt`).
  - Plan: 245 bind (216 match, 15 admitted, 7 provisional, 7 replay), 59 reuse, 39 stops. At the previous row it was
    242 / 59 / 37.
  - 0x005adc10 binds (provisional). Its callees 0x005ad980, 0x005ada40, 0x005adfe0, 0x005ae010 and 0x005b3620 bind
    (C4 match). New TUs: `PortTop_005adc10.cpp`, `PortTop_005ad980.cpp`, `PortTop_005b3620.cpp`.
  - New named stops: 0x005ad770, 0x005ad910, 0x005ad9d0 (`PortTop_AudioStops.cpp`). 0x005a61b0 was already a stop.
    The 0x005adc10 stop was removed.
  - `PortTop_005add60.cpp` now calls `PortTop_Bound_005adc10`.
- The generator rewrote other wrappers with LF line endings. Their content is unchanged and they were not committed.
- `newfiles.txt` +3, `sync_rsp_allowlist.py` (ARMS_STEP=this row): rsp +3, allowlist +0. `bindings_update.py`:
  BINDINGS.tsv updated 37, added 5.

## 3. Builds and checks
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK. rva-lint 313 known, NEW 0.
- Object build (`MASHED_DDKOBJ=1`): Build OK. rva-lint NEW 0.
- `ddkobj.py check` on this branch: IDENTICAL 1549/1592, DIFFERENT 28, not-in-map 15, units linked 1535/1744,
  refusals 9 (externalized-static-register-convention 5, wrapper-redefines-globals 4).
- **Fresh check of the same base** (38a6f3ae5, detached worktree, object build, removed after with `diag.py wt-remove`):
  IDENTICAL 1547/1590, DIFFERENT 28, units linked 1533/1741.
  - The DIFFERENT set is the same 28 rvas on both. No unit is newly DIFFERENT.
  - IDENTICAL +2 and units +2: the two new C4 match TUs (0x005ad980, 0x005b3620).
  - 0x005adc10 is provisional, so it keeps its VS2022 body in the object build.
- `mashed_image.bin` in the private root: sha256 81cc7775...9afcd8, equal to the previous row's image.

## 4. Arms command
`ARMS_STEP=PORT-TOP-AUDIOSTOP-5ADC10 ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIOSTOP-5ADC10_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIOSTOP-5ADC10 -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
- Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIOPOOL-5AE650_root` (the previous row's root, `original\` with the
  image), its old `mashed_re.log` removed.
- The runs stayed local: their paths are outside the repo.
- Return addresses symbolized against the build maps (nearest preceding symbol).

## 5. Next
- `PORT-TOP-AUDIOSTOP-5AD770`: a body for 0x005ad770, then the init arms again on top of this branch.
- `TOOL-EQUIV-PUSHECX-SLOT`: with the gap closed, re-run equiv on 0x005adc10 (it may then be admitted).
- The audio stack still merges as one: AUDIOTICK-STOPS, HWINIT-4627F0, RWAUDIO-TABLE, AUDIOPOOL, then this row.
  orch/integration 38a6f3ae5 already contains AUDIOPOOL.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIOSTOP-5ADC10_data/`, sha256)
- `vs_init_audio.log` de946b834b083b845417033cc38cca7cb365035ba83cdc00c107de30a913ef0e
- `ddk_init_audio.log` 63910cf103e0aabba861f28c7bd457495e961f93642a1236d77f9f69fa2ae1e3
- `arms1.out` 59d5c6d102d9ecc5402ce17dfffa61f8c7eba226257390286eec96c48e13ec08
- `build_vs.log` 7e773497fc4dbd9bdc2611e32e962b1ee2b28fa08963ed3a73eaac266d8f6d24
- `build_ddk.log` 54e0f0d318320a82a9ae462c24affc9a156c2c528bfce787935b831fec3d509d
- `ddkcheck.log` 6c3f03028851f037468a9b0e650ecc4a5c953ed407d24fd00bf127faafa5d65a
- `base_build_ddk.log` dcd2d1bce9cd04ed49bf840e42f2185b4128e73fa4fd7f8c4f69fd7915a16abd
- `base_ddkcheck.log` 7c8fc3f74e98a01d2ce7f3e1d21db28dc1ed136e60ea8a0f18bf84366f974d79
- `plan_write.txt` 763a34d692cf4f3872d6a4f3ab3f0d9b7bbfc8094e0e51bf3de274286e50666f
