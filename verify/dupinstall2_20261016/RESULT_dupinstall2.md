# RESULT dupinstall2 (2026-10-16)

Follow-up to `verify/dupinstall_20261016/RESULT_dupinstall.md`. Branch `work/dupinstall2` from e9a425db, worktree
`.worktrees/dupinstall2`. Evidence in this directory; every window and probe below is a file here.

## Verdict

| # | Item | Result |
|---|---|---|
| 1 | 3 queued race path2 runs | All three PASS (E9 + rel32 to the survivor, entries match) **once hook `0x00495080` is skipped**. The nav drive was dying because of that hook: **own finding, below.** |
| 2 | `0x0046cbb0` | Done. `CarStatePairGet` deleted from `Util/PromoLoop_round25.cpp`, key `car_state_pair_get` pruned, no `Vehicle/` file touched. Survivor `VehicleCarStateRead`. DUP-INSTALL findings: **0**. |
| 3 | stale DUP-IN-TARGET | Lint confirmed `0x0042b8c0` and `0x005aea00` stale (removed). `0x00417640` and `0x00418860` each had a stale 2-file entry (removed) AND a live 3-file entry (`Ai/AiControllerAB.cpp` joined the set; kept, still a real finding). `0x0046cbb0`'s DUP-IN-TARGET went with the fix. 6 allowlist lines out, lint 66 → 64, NEW=0. |
| 4 | path2 vector shapes, 8 keys + 2 | All 8 now PASS call-through, plus the two race plugin getters (10 keys). Needed a template change, see below. |
| 5 | strict C4 check, 5 rows | 3 keep C4 with fresh canonical-scenario evidence on the survivor, **2 demoted C4 → C3** (`0x0040b9a0`, `0x0040ba60`). |

Also demoted: **`0x00495080` `Fwd494fd0_DoublePlusHalf` C3 → C2** (the finding).
Install manifest after, 27 RVAs: **27 of 27 single install**, survivor named, first byte `0xE9`.
path2 on 22 survivor keys (menu + race): **22 of 22 install proven, 22 of 22 call-through PASS.**
G-CTRL on the rebuilt exe: **c2e4d60d / 603ab244 / ba90ed88**, equal to the default trio on this base (u9683
changed the gates hash from `b50bd39b`, `verify/d3_u9683_20261016/RESULT_u9683.md`). No exe TU was touched.

## 1. The nav-drive finding (own finding)

**Signature:** race-scenario path2 prints `[nav] booting to menu...` then `frida.InvalidOperationError: script has been
destroyed`. Not the audio endpoint: it reproduced with audio back, and with the unmodified deployed .asi.

**Root cause:** hook `0x00495080` `Fwd494fd0_DoublePlusHalf` (`Render/PromoLoop_round256.cpp:172`) assembles
`mov esi, dword ptr [771A0Ch]` as **`be 0c 1a 77 00  mov esi, 0x771a0c`** (an immediate) instead of
`8b 35 0c 1a 77 00  mov esi,[0x771a0c]`. `FUN_00494fd0` takes its DirectInput device pointer in ESI (`mov eax,[esi]` at
`0x00494fee`, `call [eax+0x1c]` at `0x00494ff1`, return `0x00494ff4`), so the call goes to `0x1`.

**Chain of evidence**
1. `nav_probe.py` (crash handler + exit code): full hook set, with or without `DETACHED_PROCESS`: AV read at **EIP 0x1**,
   stack `0x00494ff4, 0x00771a0c, ...`, exit `0xC0000005`. `MASHED_HOOK_ONLY=__none__`: reaches the race, both spawn
   styles. (`nav_probe_variants.txt`, four spawn variants, re-run for the record.)
2. `bisect_nav.py` over `MASHED_HOOK_LO/HI`, 20 boots, `bisect_nav_run1.log`: culprit registry index **1335** of 1354
   (deployed Oct-13 .asi), name read from `verify/dupinstall_20261016/install_before/hook_install_manifest.txt`.
3. Window 2, worktree build: the full set dies the same way (`win2/nav_probe_full_hookset.txt`); with
   `MASHED_HOOK_SKIP=Fwd494fd0_DoublePlusHalf` the drive reaches the race (`win2/nav_probe_skip_fwd494fd0.txt`).
4. Built export disassembled (`disasm_asi_export.py`): `be 0c 1a 77 00`. A compile probe (`asm_probe/probe.cpp`):
   `[771A0Ch]` and `[0x771A0C]` assemble to the immediate, `ds:[771A0Ch]` / `ds:[0x771A0C]` and the register route to a
   memory load.
5. Scope: a grep over `mashedmod/src` for the bare spelling finds exactly this one line; every other absolute operand
   (about 60) is `ds:[...]`.

**Why it stayed hidden:** the C3 A/B stubs the callee and records stack arguments; ESI is a register input.
**What I did:** KNOWN_ISSUES entry `AUTOHOOK-INLINE-ASM-IMMEDIATE`, row demoted C3 → C2 with the one-line fix recorded
(`mov esi, dword ptr ds:[771A0Ch]`). **I did not change the source**: it needs its own re-earn (path1 plus a nav boot with
the hook installed), your call. Until then every race-scenario path2 or nav boot with the full hook set needs
`MASHED_HOOK_SKIP=Fwd494fd0_DoublePlusHalf`.

## 4. path2 template and vector fixes

`re/frida/verify_hook_install_template.js` (path2 only, `diff_template.js` untouched):
- shape-keyed `{p2bufs: [size...], p2args: [{buf: i} | number ...]}`: zero-filled buffers, pointer passed as a pointer
  (or as a number where the signature declares a scalar, as for `Bool0Out8a94e0`), FNV-1a of the buffers returned;
- `freeze_main` now also freezes the game thread around the generic test loop (it did so only for `call_log`), which is
  what the draw entry `HudRectEmitter5` needed.

`re/frida/hooks_registry.py` `path2_tests` rewritten for `copter_find_639dc4`, `bool0_out_8a94e0`, `boolmin_out_8a94e0`,
`maxscoreflags_40b9a0`, `active4slots_40ba60`, `audio_list_init`, `frontend_page_id_dispatch`, `plugin_data_dword_a`,
`plugin_data_dword_b` (`fix_path2_vectors.py`); `hud_rect_emitter5` needed only the freeze (its fixture already set
`freeze_main`). path1 vectors untouched.

| key | entries | | key | entries |
|---|---|---|---|---|
| copter_find_639dc4 | 3/3 | | audio_list_init | 3/3 |
| bool0_out_8a94e0 | 3/3 | | frontend_page_id_dispatch | 3/3 |
| boolmin_out_8a94e0 | 3/3 | | hud_rect_emitter5 | 1/1 |
| maxscoreflags_40b9a0 | 2/2 | | plugin_data_dword_a / _b | 1/1 each (SKIP) |
| active4slots_40ba60 | 1/1 | | table_773030_get | 2/2 (SKIP) |

## 5. Strict C4 check against `re/CONFIDENCE.md`

Bar applied: a clean diff on a canonical scenario **with the inline JMP live and the body actually executed** (CONFIDENCE
C4 + CLAUDE.md "Confidence promotion"; the repo's own tools for it are `canonical_c4_verify.py` = exercised AND installed
AND survived, and `canonical_c4_racediff.py` = Stalker diff OFF vs ON, frame-synced, in a Quick Battle race) and the
evidence must be on the surviving body ("Which copy the evidence covers"). Install-observe alone does not qualify:
`install-observe-may-never-execute-the-body`.

| row | C4 basis before | evidence on the survivor now | verdict |
|---|---|---|---|
| `0x00407a20` GetAiLapCounter | race racediff, installed body = survivor | racediff in-race **C4-CLEAN**, 1 sig each side, JMP 0xE9; committed 2026-08-15 result agrees | **C4 kept** |
| `0x004098a0` Ret63a5f0 | race racediff CLEAN, but on the deleted GetLedEntryArrayBase | racediff in-race **C4-CLEAN** on the survivor (1 sig each side) | **C4 kept** (basis now recorded) |
| `0x00498bf0` DisplayActiveFlagGet | `canonical_c4_verify` 2026-06-06 by RVA, deleted DisplayGetCursorGate installed | `canonical_c4_verify`: OFF run 955 calls at boot-to-menu, ON JMP 0xE9, survived: **C4-READY** | **C4 kept** |
| `0x0046cbb0` VehicleCarStateRead | racediff 2026-08-15, survivor installed | racediff in-race **C4-CLEAN** again | **C4 kept** |
| `0x0040b9a0` MaxScoreFlags40b9a0 | 2026-07-24 install-observe, ran the deleted PlayerScoreMaxTest | menu boot **0 calls**; Quick Battle race frames 300-1200: **original not observed, 0 sigs** both sides | **DEMOTED C4 → C3** |
| `0x0040ba60` Active4Slots40ba60 | same | same | **DEMOTED C4 → C3** |

Why the two never run: their caller `FUN_0041b540` is the Race::Tick non-ghost branch
(`re/analysis/leaderboard_d3/0x0041b540.md`, `parent_rva 0x0040fc00`), not on the Quick Battle path. Both keep C3 (path1
GREEN, path2 PASS 2/2 and 1/1). Re-promotion needs a scenario that reaches `FUN_0041b540` with the JMP live.
Caveat on the three kept leaves: the racediff signature of a leaf is one `eax=` value per side, thin; it is the project's
canonical standard and sits on top of a bit-identical path1 over the full vector set.

## Tooling notes

- `canonical_c4_verify.py` reads `survived` from `psutil`; `py -3.12` here has none, so it can only print HOLD and it
  never kills its MASHED. `make_c4_tools.py` generates worktree-safe copies (`c4_verify.py` with a ctypes pid check,
  `c4_racediff.py` with the main install path); upstream is unchanged.
- Deploy windows (`window.py`, backup + no-MASHED poll + restore in a `finally`): 10:30:14-10:38:44, 11:02:16-11:05:15,
  11:06:02-11:07:09. Window 2 waited 9 minutes for other sessions' MASHED.exe to exit before it opened. Shared .asi
  sha256 `270e036f...` before and after each, no backup left. One transient `WinError 32` on the first restore attempt of
  window 1 (the .asi was still being unmapped), the retry succeeded.
- No MASHED.exe of mine was left running; only pids spawned by my scripts were touched.

## Files

`window.py`, `steps_window{1,2,3}.json`, `make_steps*.py`, `nav_probe.py`, `bisect_nav.py`, `bisect_logs/`,
`disasm.py`, `disasm_asi_export.py`, `asm_probe/`, `make_c4_tools.py` (+ generated `c4_verify.py`, `c4_racediff.py`),
`fix_path2_vectors.py`, `drop_carstatepairget.py`, `allowlist_prune.py`, `tracker2.py` (the hooks.csv + CHANGELOG
transaction), `win1/`, `win2/`, `win3/`, `path2_summary*.txt`, `install_after_summary.txt`, `gctrl_after.log`.
