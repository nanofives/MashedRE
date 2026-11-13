# RESULT DDKOBJ-42AD90 (2026-11-12): 0x0042ad90 root cause, and the register-convention statics a boot never reaches

Branch `orch/DDKOBJ-42AD90` from `orch/integration` @ b34672ee2. Row kind: top-down tool row (only `re/tools/ddkobj/`
and this folder). Follow-up of `RESULT_DDKOBJ-REGCONV.md` section 4.

## Verdict

1. **Root cause of the 0x0042ad90 crash: a calling-convention mismatch, and only the unguarded build has it.** The
   original body reads its three arguments from EDX, EBX and EDI. VS2022 code (`MenuNavSM`) calls the exe name
   `Find42ad90` as `__cdecl` with three stack arguments. `DDKOBJ_REGCONV=promote` bound that name straight to the DDK
   body, so the body walked EDX, a value the VS2022 caller left behind, until it read unmapped memory. **The fix is
   already in `re/tools/ddkobj/`:** the DDKOBJ-REGCONV generated adapter (tool commit c308607fb), the default mode.
   Proven again here on the solo unit and on the whole tree (section 1). No change to `ddkobj.py` is needed, and nothing
   stays on the VS2022 body for this reason.
2. **Why only 0x0042ad90 crashed:** in a 3000-frame knob-OFF boot it is the **only** register-convention static that
   runs at all (first hit 1288 ms, from `Nav_Init`). Knob ON adds 11, every one called in-object from DDK `Orig_*`
   code, which uses the private convention correctly. So the other 37 units booted under `promote` because their
   mismatched VS2022 call sites never ran, not because those call sites are compatible.
3. **Untested at runtime (section 2):** of the 44 register-convention statics in today's tree, a 40 s boot reaches
   **12** (1 knob OFF and ON, 11 knob ON only), **29** are linked but never run, and **3** are not linked. The
   nav-demo walk reaches 3 of the 29. **26 statics have never run in any boot, and 20 of the 22 linked adapters
   have never run** (only `_Find42ad90` and `_CR_41f710Body` did).

Self-test: `ddkobj.py selftest` **OK, 0 failures** (`selftest.log`). `ddkobj.py check` on the full object build:
IDENTICAL 789 / 878, DIFFERENT 79, not-in-map 10, refusals 9 (`wrapper-redefines-globals`). The solo adapter build:
IDENTICAL 1/1. Default gates not run (top-down row). Default build unchanged: no source or `build.bat` line changed.

## 1. 0x0042ad90

### 1a. Static evidence (original, `MASHED.exe.unpatched`, capstone)

The body takes no stack argument:

| address | instruction | reads |
|---|---|---|
| `0x0042ad90` | `test edx, edx` | list pointer in EDX |
| `0x0042ad98` | `mov eax, dword ptr [edx]` | first element |
| `0x0042ada6` | `cmp eax, ebx` | key in EBX |
| `0x0042adaa` | `cmp edi, esi` | nth in EDI |
| `0x0042adaf` | `mov eax, dword ptr [edx + ecx*4 + 4]` | loop load |
| `0x0042ad9f`, `0x0042adb4` | `cmp eax, 0xff070000` | terminator (`0xff070000`, decimal -16318464 as int32) |

Its five original call sites (`0x0043d4b5`, `0x0043d4c0`, `0x0043d639`, `0x0043d644`, `0x0043d788`) load the
registers just before the call, for example `0x0043d4aa mov edx,[esp+0x18]`, `0x0043d4ae xor edi,edi`,
`0x0043d4b0 mov ebx,0xff000000`, `0x0043d4b5 call 0x42ad90`.

### 1b. The two object builds of the unit alone

`ddkobj.py build --units 0x0042ad90` (disassembly of the linked exes, base `0x00010000`):

| build | `_Find42ad90` (the exe name VS2022 calls) | VS2022 call site in `Nav_Init` |
|---|---|---|
| `DDKOBJ_REGCONV=promote` (`ddkobj_promote_solo`) | **is** the DDK body at `0x00011000`, first instruction `test edx, edx` | `push 0` / `push 0xff080000` / `push [0xf5a394]` / `call 0x11000` (`0x0016d692`..`0x0016d69f`) |
| default, adapter (`ddkobj_adapt_solo`) | the adapter at `0x00011040`: `mov edx,[esp+4]`, `push ebx`, `mov ebx,[esp+0xc]`, `push edi`, `mov edi,[esp+0x14]`, `call 0x11000`, `pop edi`, `pop ebx`, `ret`. Body `___ddkreg_0042ad90` at `0x00011000` | same pushes, `call 0x11040` (`0x0016d6bf`) |

The adapter moves the three stack arguments into EDX/EBX/EDI and saves the two callee-saved registers it loads.

### 1c. The crash (unguarded solo build, knob OFF, `catch_crash.py`, `crash_promote_solo_off.log`)

    EXCEPTION access-violation at 0x0001101f -> _Find42ad90+0x1f
      mem read 0x75bcd000   edx 0x75b6d8d0   ecx 0x17dcb   ebx 0x00f5a390   edi 0x75b6d8d0
      stack: BuildRecords+0xd7, Nav_Init+0xa2, HandTop_InitFrontendAssets+0xca9 (exe_main.obj)

- `+0x1f` is the loop load, the original's `0x0042adaf` (`0x0042adaf - 0x0042ad90 = 0x1f`).
- EDX holds `0x75b6d8d0`, which is not the list. The caller passed the list on the stack. ECX = `0x17dcb`: the loop
  read 0x17dcb dwords from EDX without meeting `0xff070000` and stopped at the first unmapped page `0x75bcd000`.
- The log ends after `R2-2 gamesave load` as in the REGCONV bisect, so this is the same crash.

### 1d. The fix holds

| run | exe | result |
|---|---|---|
| `adapt_solo_off` (`knob_boot.py`, 40 s, `MASHED_DET_FRAMES=3000`) | adapter solo | rc 0 after 29.6 s, `B17-SUMMARY reached-main-loop` |
| `adapt_solo_on` (same, `MASHED_ORIG_TOP=1`) | adapter solo | rc 0 after 26.9 s, `[ORIG_TOP] WinMain 0x00492370 returned 0 after 3000 frames` |
| `reach_run_off` | full object build (837 units) | process ended at 26.5 s, `_Find42ad90` adapter and `___ddkreg_0042ad90` both hit at 1288 ms |
| `reach_run_on` | full object build | `WinMain ... returned 0 after 3000 frames`, adapter and body hit at 1968 ms |
| `ddkobj.py check` | adapter solo / full | `___ddkreg_0042ad90` IDENTICAL (64 bytes) in both |

So the body that crashed under `promote` now runs on every boot through its adapter, under both knobs, with no
exception. The reach runs record 5 Frida exceptions of type `system` at an address outside the exe in the knob-ON run
(same kind in the nav run). Neither run stopped on them: both exited by themselves, knob ON with `WinMain ... returned 0`.
[UNCERTAIN: what raises them. The handler records type and address only. They do not come from the exe's code range.]

## 2. Register-convention statics a boot does not reach

### Method

- **Static leg** (`regconv_reach.py static`, `reach_static.tsv`): every E8/E9 rel32 and every imm32 in the full object
  build's code that points at a `___ddkreg_*` body or its adapter, with the containing map symbol. imm32 hits are
  unverified raw matches (for example `RaceSubmit_Render -> _MenuGroupCountBody` has not been decoded).
- **Runtime leg** (`regconv_reach.py run`): Frida on the full object build, a one-shot listener (detached on first hit)
  on all 63 linked bodies and adapters and on their 52 direct code callers. Four runs, private root, inside
  `mashed_lock`:
  - `reach_run_off` / `reach_run_on`: the bisect's boot (`MASHED_DET_FRAMES=3000`, 40 s limit, no input), knob OFF / ON.
  - `reach_nav_off` / `reach_nav_on`: the nav-demo walk (`MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1
    MASHED_DET_MPG_PIN=0`), both `NAV_DEMO phase=28 done ok=1`.
- **Original callers** (`orig_callers_43.py`, `REGCONV_orig_callers.tsv`): E8/E9 sites to each RVA in
  `MASHED.exe.unpatched`, named by the nearest `hooks.csv` row at or below the site (that row is the containing function
  only when `hooks.csv` has it, so the caller column is a pointer for reading, not a proof).
- Joined in `REACH_44.tsv`.

The tree now has **44** statics (REGCONV had 43): `0x00498810` (`re/match/s16/00498810.c`, 86.7% source) is new. It is
byte-checked DIFFERENT (4 bytes at `+0xc`), which the PREREG bar allows for a source below 100%.

### Reached in the 40 s boot (12)

`0x0042ad90` under both knobs (VS2022 `Nav_Init` / `BuildRecords` / `NavCountItems` / `Nav` through the adapter).
Knob ON only, each called in-object from its DDK unit: `0x004670a0` (`Orig_00467110`), `0x0041beb0` (`Orig_0041c0e0`),
`0x0041cb00` (`Orig_0041ccf0`), `0x0041d6d0` (`Orig_0041d890`), `0x00487150` (`Orig_00487e00`), `0x00401690`
(`Orig_004025f0`), `0x0045ba10` (`Orig_0045bae0`), `0x00457ba0` and `0x00458020` (`Orig_00458360`), `0x00454170` and
`0x004547c0` (`Orig_004548a0`). The first three of the `0x0041xxxx` group are hit at 27.0 s, the others before 2 s.

### Reached only by the nav-demo walk (3)

| rva | exe caller path | original caller |
|---|---|---|
| `0x0041f710` | VS2022 `_CR_41f710` -> adapter `_CR_41f710Body` | `0x00420870` [gameplay] |
| `0x0042ac50` | DDK `_MenuAnimTickTwin` (in-object) | `0x0042fb70` MenuSpriteDispatchB and 9 more [frontend] |
| `0x00431b80` | DDK `_CarSelectReset` (in-object) | `0x00431d00`, `0x0043dfd0` [frontend] |

`0x0041f710` is called from VS2022 code, so under `promote` the walk would have sent stack arguments into its register
body. It was never in the bisect's 40 s boot.

### Never reached in any run (26 linked) and not linked (3)

"VS2022" = a VS2022 caller reaches the body through the adapter. "in-object" = a DDK function of the same unit calls the
body directly. "What a boot needs" names the exe caller that has to run, which none of the four runs did, and the kind
of boot the original callers' `hooks.csv` subsystem points to. No race was booted in this row.

| rva | exe caller path (not reached) | original caller(s) | what a boot needs |
|---|---|---|---|
| `0x0047c230` | in-object `_CameraPathAllNodesEq2` | `0x0047c270` [camera] | a race that runs the matched camera-path code |
| `0x0047c1f0` | in-object `_CameraPathAnyNodeNonzero` | `0x0047c2d0` [camera] | same |
| `0x00413bc0` | VS2022 `_HudUvRect413bc0` | `0x00413cb0` [hud] | a race with HUD drawing |
| `0x0041b770` | in-object `_HudConstTableInitAndSweep`, adapter address taken by `AutoRegister_Copy41b770` (.asi install) | `0x0041c010` HudConstTableInitAndSweep [hud] | a race with HUD |
| `0x0041bc50` | in-object `_HudDispatchSlot2`, VS2022 `_FUN_0041bc50` | `0x0041c0c0` HudDispatchSlot2 [hud] | a race with HUD |
| `0x0041c2d0` | in-object `_HudDispatchMode5`, VS2022 `_FUN_0041c2d0` | `0x0041a3e0` HudDispatchMode10, `0x0041c300` HudDispatchMode5 [hud] | a race with HUD |
| `0x0041c9a0` | in-object `_HudSlotLoopCcc0` | `0x0041ccc0` HudSlotLoopCcc0 [hud] | a race with HUD |
| `0x0041d410` | in-object `_PortBatch_0041d870` | `0x0041d870` [hud] | a race with HUD |
| `0x0041e630` | in-object `_PortBatch_0041e850` | `0x0041e850` [hud] | a race with HUD |
| `0x0041c320` | VS2022 `_ParticleEmitterCtorC` | `0x0041cb10` [boot] | the call that constructs that emitter. [UNCERTAIN: which screen or race step; neither the boot nor the walk does] |
| `0x0041cd20` | VS2022 `_ParticleEmitterCtorD` | `0x0041d6e0` [boot] | same [UNCERTAIN] |
| `0x0041ad60` | VS2022 `_ParticleEmitterCtorA` | `0x0041b450` [boot] | same [UNCERTAIN] |
| `0x0041ae20` | in-object `_Update41b520`, VS2022 `@Copy41ae20@4` | `0x0041b520` [render] | [UNCERTAIN: no runtime evidence of the trigger] |
| `0x0040fc00` | in-object `_FUN_004111c0` (7 sites), adapter has no code reference | `0x004111c0` [util] | a run that calls the matched `0x004111c0` [UNCERTAIN: trigger] |
| `0x004102f0` | in-object `_FUN_004111c0`, adapter has no code reference | `0x004111c0` [util] | same |
| `0x00498810` | in-object `_Orig_00498860` (knob ON path) | `0x00498860` [util] | a knob-ON run that reaches `0x00498860` [UNCERTAIN: trigger] |
| `0x0042f020` | only from its own adapter, which has no code reference | `0x0043dfd0` [frontend] | the matched frontend dispatcher `0x0043dfd0` on the knob path |
| `0x00427680` | VS2022 `_FUN_00427e00`, `_FUN_00427f00`, `_FUN_00427ff0`, `_FUN_00428140` | `0x00427e00`, `0x00427f00`, `0x00427ff0` FontText_DrawTextRotated, `0x00428140` [frontend, hud] | menu or HUD text drawn through those exe functions |
| `0x004277a0` | in-object `_FUN_004278d0`, `_FUN_00427990`, `_MenuMenusBB`; VS2022 `_TextCtrlCodeRemap` and three of the above | 7 sites [render, frontend, hud] | same |
| `0x00427840` | in-object `_FUN_00427be0`, `_TextWidthMeasureB`; VS2022 `_FontText_UTF16WidenCopy`, `_FUN_00427f00` | `0x00427be0`, `0x00427f00`, `0x00428320` | same |
| `0x00427880` | only from its own adapter, which has no code reference | `0x00427ca0` FontText_HudInit [hud] | a run that reaches `0x00427ca0` |
| `0x00429240` | in-object `_FUN_00429290`, imm32 in `ModalGo` (unverified) | `0x00429290` [frontend] | [UNCERTAIN: trigger] |
| `0x0042aad0` | VS2022 `_MenuDimSet` | `0x0043af10`, `0x0043c5b0` MenuDrawLoop [frontend] | a menu frame that calls `MenuDimSet` |
| `0x0042ac00` | in-object `_MenuSpriteDispatchB`, VS2022 `@MenuGroupCount@8` | 11 sites in `0x0042fb70`..`0x0043d2a0` [frontend] | a menu frame that calls them |
| `0x0042ad10` | VS2022 `_PromptRowInit0042ad10` (jmp) | `0x00432b30` MenuPromptStrip [frontend] | the menu prompt strip |
| `0x0046c5f0` | VS2022 `Collision::TriangleFaceNormal`, imm32 in `_RwCore_004cdb60` (unverified) | `0x0046da80`, `0x0046ddb0`, `0x0046f6c0` [vehicle] | a race (vehicle-track contact) |
| `0x0048cee0` | **not linked** (no caller left, `/OPT:REF`) | `0x0048e820` [particle] | a linked caller first |
| `0x0045bac0` | **not linked** | `0x0045bba0`, `0x0045bed0` PowerupRoundCleanup, `0x0045bf30` PowerupTeardownAll | a linked caller first |
| `0x00458080` | **not linked** | `0x00458080` [gameplay] (self-referencing site) | a linked caller first |

Of the 26, **14 have a VS2022 caller**, which means a path a promotion without adapters would have broken the same way
as `0x0042ad90`: `0x00413bc0`, `0x0041bc50`, `0x0041c2d0`, `0x0041c320`, `0x0041cd20`, `0x0041ad60`, `0x0041ae20`,
`0x00427680`, `0x004277a0`, `0x00427840`, `0x0042aad0`, `0x0042ac00`, `0x0042ad10`, `0x0046c5f0`. A 15th,
`0x0041f710`, is reached by the walk only. In the adapter build each goes through its adapter. None of those adapters has run yet. Each
adapter is the same generated three-line shape that `_Find42ad90` proves at runtime, and each body is byte-checked.
[UNCERTAIN: runtime behaviour of the 20 never-run adapters. Evidence missing: a race boot and a top-down menu frame that
reach the callers above.]

## 3. Files

- **Committed** (`verify/ddkobj_20261112/`): this RESULT, `REACH_44.tsv`, `reach_static.tsv`, `reach_run_off.tsv`,
  `reach_run_on.tsv`, `reach_nav_off.tsv`, `reach_nav_on.tsv`, `REGCONV_orig_callers.tsv`, `orig_callers_43.py`,
  `reach_table.py`, `run_42ad90.sh`.
- **Tool:** `re/tools/ddkobj/regconv_reach.py` (new: static and runtime reach of `__ddkreg_*` bodies and adapters).
  `ddkobj.py` unchanged.
- **Raw files** (`D:/MashedWT/DDKOBJ-42AD90_data`), sha256:
  - `crash_promote_solo_off.log` `3cd80671b5fb4f4de0579b742a1f8d44bf4ef7fd92111927d8f411d5c556e220`
  - `run_42ad90.log` (adapter solo boots, reach runs) `ac5fc3ab0550201eb84a19ad24cb12021e5c866e70b477d953bf0194edcbf274`
  - `run_42ad90_nav.log` `c4163c55ff43d753c9e8616bfa3b138df95c2fcd625a77b083067f412b98a720`
  - `adapt_solo_off.log` `1c387af421524e4821dce2f729ebbfb9b8510c315d5da56772ce4d23e802939a`
  - `adapt_solo_on.log` `02a2150a5b1403d13fa65032bba61b5cb2d96e09dc679bbf28ec0a0f0a5b824a`
  - `reach_run_off.log` `fcbe1714d351cdf8db6707b963067bfdc6eae7b246dc739a4b63d7d20853b87e`
  - `reach_run_on.log` `9612cfffa0ccf15d4d937659e44e1cf7d9fc2f552aa2340f1c6b078fe64be3d0`
  - `reach_nav_off.log` `40dcb465a529cb60d91f5c5236ef7c949de269ecfd0424bd0dbce32b495591e7`
  - `reach_nav_on.log` `98750d24d6309a8376d4d32e095a89f629604b48b418de60fa00a72b01b29c65`
  - `selftest.log` `a715fc210d83398789a5f945d88d3110f7a48cb3c9096531c4e9c788c2e238ac`
  - `build_full.log` `16c052af2177c3bc375e499f13672678de849455256ebbdec613485bb9c80d98`
  - `build_solo.log` `7b280ec4838067e690ef5e5e8db4481340c41d57a72cbc9839897a124f0ce678`
  - `BYTECHECK_full.tsv` `f0d2c3f3454b0b40f64bf2117d51a2acc4198d68588a140b937a57d6b8923096`
  - `ddkobj_mashed_re_ddk.exe` (full, + `.map`) `7d4d1aeb9fdac7bc82bf4b34a04c9bd3e2bac928a6ff82fe237b19f957cef0f3`
  - `ddkobj_adapt_solo_mashed_re_ddk.exe` `7a31dcddadb788a4e786939dce75e0c896014122413455c8e04e5d2c3af41e9a`
  - `ddkobj_promote_solo_mashed_re_ddk.exe` `1bd4040d1f72687a00d9ff7b1f8290f2f0472492944de1d31a9003d32157ae8c`
- Root: `D:/MashedWT/DDKOBJ-42AD90_root` (robocopy of `PORT-TOP-04J_rootK/original`, settings baseline restored before
  every run).
