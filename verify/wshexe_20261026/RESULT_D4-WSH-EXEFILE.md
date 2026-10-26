# RESULT D4-WSH-EXEFILE: C4 rows where exe_file != file

Verdict: **RED for 1 row (demoted), 4 real two-body pairs with no exe-copy evidence, 13 rows that are one shared body.**
Base `orch/integration` @ `96e7f1ad2`. PREREG `verify/wshexe_20261026/PREREG_D4-WSH-EXEFILE.md` (commit `757b9a741`, alone,
before any arm). Harness extension `0fff2aed5`. No build input touched, so no exe was built and the default gates were not run.

## 1. Text diff of the 17 rows

Method: every `0x<rva>` occurrence under `mashedmod/src/mashed_re`, then the TU's membership in
`mashedmod/exe_sources.rsp` and `mashedmod/asi_sources.rsp`, then `#if` guards in the TU (none in the six shared files below).

### 1a. One body, compiled into both targets (13 rows)

The body named in `exe_file` is the only body for the RVA, its TU is in both rsp lists, and the `.asi`
`RH_ScopedInstall` lives in that same TU. The C4 evidence measured this body. `exe_file != file` only because `file`
names a plate or a path with no body in it.

| RVA | Body (both targets) | `file` cell today | Action |
|---|---|---|---|
| `0x004026d0` | `Boot/BootLowRvaCluster.cpp` | plate `.md` | `file` := body |
| `0x0040b6d0` | `Util/UtilLeaves_ac.cpp:134` | plate `.md` | `file` := body |
| `0x0042b8d0` | `Boot/FrameDispatch.cpp:26` | `Util/StateAccessors.cpp` (does not exist on this lineage) | `file` := body |
| `0x0041c2d0` | `HUD/HudDispatch.cpp:373` | plate `.md` | `file` := body |
| `0x0040bb70` | `Frontend/SpriteGate.cpp:51` | plate `.md` | `file` := body |
| `0x00430a10` | `Frontend/SpriteGate.cpp:194` | plate `.md` | `file` := body |
| `0x00494ef0` | `Boot/Teardown.cpp:49` | `Boot/VideoConfig.cpp` (no body for this RVA) | `file` := body |
| `0x0040bb90` | `Frontend/SpriteGate.cpp` | plate `.md` | none here: in D4-WSH-ZEROEXEC's list |
| `0x00430a60` | `Frontend/SpriteGate.cpp:229` | plate `.md` | none here: ZEROEXEC |
| `0x00430ab0` | `Frontend/SpriteGate.cpp:264` | plate `.md` | none here: ZEROEXEC |
| `0x00431ae0` | `Boot/BootLowRvaCluster.cpp:322` | plate `.md` | none here: no `frida_diff`, D4-WSH-NOEVID |
| `0x00431af0` | `Boot/BootLowRvaCluster.cpp:347` | plate `.md` | none here: NOEVID |
| `0x00431b00` | `Boot/BootLowRvaCluster.cpp:372` | plate `.md` | none here: NOEVID |

The six rows held back carry a separate open question about their C4 (zero executions or no diff). Setting
`file := body` on them would extend that C4 to the exe before their own step rules on it.

The exe and `.asi` compile these TUs with different flags (`/EHa /DMASHED_STANDALONE` against `/EHsc`), same text.

### 1b. Two bodies, one per target (4 rows)

| RVA | `.asi` body (evidenced) | exe body | Difference |
|---|---|---|---|
| `0x004325c0` | `Frontend/MenuAnimTickTwin.cpp` (asi-only) | `Frontend/MenuNavSM.cpp` `Anim_Tick` (exe-only) | float-to-int: twin `std::nearbyint` (`MenuAnimTickTwin.cpp:34`), exe `(int)v`. Exe multiplies `base` by `kMenuAnimSpeedup = 6.0f` (user feedback 2026-06-13, comment in place). Exe reads `g_records` / `g_frame_dt` / `slot.screen_id`, not the live globals. |
| `0x00432b30` | `Frontend/PromptStripTwin.cpp` (asi-only) | `Frontend/MenuNavSM.cpp` `PromptStripAppend` + head in `Nav` (exe-only) | jump tables identical (normalised compare of the 10 rows: only a comment differs). Exe replaces the calls to `0x0042ad10` with `PromptRowInit`, `0x0042b920` with the constant `0x16` (decomp: `FUN_0042b920` returns `0x16`), `0x0042a9c0` with exe-built `ModeCodeLookup`, moves the mode/rel head into the caller, adds a `kMaxRecords` guard. |
| `0x0040b890` | `Gameplay/Thresholds_ah4.cpp:50` (asi-only) | inline in `exe_main.cpp` (~4125), third copy `LibRw/RwRaceFx.cpp:1426 ScoreTarget` | same mapping of (count, `DAT_0067ea64`, `DAT_007f0fd0`) as the decomp. Exe binds count to the constant 4 instead of `FUN_0040e340` (`return DAT_008a94d0`) and the rule to `g_track.race_rule()`. |
| `0x0040b8e0` | `Gameplay/Thresholds_ah4.cpp:66` (asi-only) | inline in `exe_main.cpp` (~4129), third copy `LibRw/RwRaceFx.cpp:1432 LeaderThreshold` | as `0x0040b890`, values 10/7. |

None of the four exe bodies can run under path1: each one sits in an exe-only TU and works on standalone state
(`g_records`, `HudInputs`, locals inside the HUD draw). A path1 run against the original needs the body
consolidated into one shared TU first (ROADMAP `:2930-2934`, the D4-DC recipe). Per `re/CONFIDENCE.md:108` the
rows' level already describes the `.asi` copy only, so the exe copies lose nothing they held. They stay without evidence, and the notes say so.

## 2. Frida arms: the `0x004325c0` twin against the original

Original `FUN_004a2c48` (decomp `D:/MashedWT/D4-WSH-EXEFILE_data/decomp_getters.txt`): `ROUND(ST0)`, then a sign-aware
correction that gives truncation toward zero. The exe copy's `(int)v` matches that. The twin's `nearbyint` does not. The
2026-06-10 evidence ran at one tick, where the exercised products (-33.33, -13.33) round and truncate alike.

`re/frida/menu_anim_diff.py`, private root `D:\MashedWT\D4-WSH-EXEFILE_root` (robocopy of `original\`,
`MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E`, deployed `.asi` `906F8601...A8AD`, the same file as the main install's),
under `mashed_lock.py`, one boot per arm, MASHED PID killed by the harness (its own spawn only).

| Arm | dt (`DAT_007f1004` bits) | Result | Prediction |
|---|---|---|---|
| A0 | game's own, `0x3c888888` on all 20 | GREEN 20/20 | GREEN, met |
| A1 | forced `0x3c888888` (1 tick) | GREEN 20/20 | GREEN, met |
| A2 | forced `0x3d088888` (2 ticks) | **RED 0/20**: record 0, tag `0xff000000`, `+0x10`, `v0 = 511`, original `485`, twin `484` | RED, met on mechanism |

A2 reading: `-800 * 0.033333331 = -26.67`. The original wrote `511 - 26` (truncation), the twin `511 - 27` (round).
**Deviation from the PREREG:** it predicted the first differing dword in a `0xff040000` record (`-2000*dt`, -66 against -67).
The first difference is instead record 0, the `0xff000000` slide class (`-800*dt`). The direction and the cause
(twin rounds, original truncates) are as predicted. Only the record class differs, because the PREREG did not check which
class sits first in the table. The decision rule's question, whether the original truncates at this site, is answered either way.

dt = 2 ticks can happen: a frame of 97..103 ms snaps to 100 ms at `0x0049349b`, and `FrameClockUpdate` / `FUN_00493480`
turns 100 ms into 2 ticks.

**Decision (per PREREG):** demote `0x004325c0` C4 -> C2 through `re-classify`. Its plate is
`re/analysis/frontend_c1_to_c2_followup_s2/FUN_004325c0.md`. The C3 "verified" half fails: the body the evidence measured is
not bit-identical on a reachable input.

## 3. Tracker transaction (re-classify)

- `0x004325c0` C4 -> C2, `status` `impl`, notes carry the A2 signature and the exe copy's state.
- `file := exe_file` on `0x004026d0 0x0040b6d0 0x0042b8d0 0x0041c2d0 0x0040bb70 0x00430a10 0x00494ef0` (no level change).
- Notes only on `0x00432b30 0x0040b890 0x0040b8e0`: the exe copy, its differences, and that it has no evidence of its own.
- CHANGELOG entries for each of the above.

## 4. Follow-ups (not done here)

1. `0x004325c0` twin: `Trunc48` should truncate (`(int)v`, like the exe copy). After the fix, re-run A0/A1/A2. A2 GREEN
   re-earns the C3 "verified" half for the `.asi`. C4 then needs its canonical-scenario run again.
2. `0x004325c0` exe copy: consolidation is blocked on `kMenuAnimSpeedup = 6.0f`, a user-requested non-faithful rate.
   Making the exe faithful means a knob and a decision row (keep 6x as the default or restore 1x).
3. `0x0040b890` / `0x0040b8e0`: consolidation onto `Thresholds_ah4.cpp` requires the exe to write `DAT_008a94d0` = 4. That
   was not checked here. If the exe never writes it, the shared body returns the 4-car arm's opposite (8/7 instead of 12/10)
   and the HUD changes, so it is a knob plus a PREREG.
4. `0x00432b30`: consolidation needs the `Nav` head and `0x0042ad10`'s port in the shared TU.
5. `scripts/lint_rva_bodies.py` reports 1 finding on this tree (`CROSS-TARGET:004177b0`) and none of the four pairs in 1b.
   The exe copies carry their RVA as `// FUN_<rva> (0x<rva>)` prose or inside a function, not as the `// 0x<rva>` line
   the scanner keys on, so the allowlist undercounts. Harness-backlog item, not fixed here.
6. The six rows held back in 1a get `file := body` from ZEROEXEC / NOEVID once those steps rule on them.

## Raw data (not committed)

| File | SHA-256 |
|---|---|
| `D:/MashedWT/D4-WSH-EXEFILE_data/A0.json` | `b39be41ea9345cf9f060a573035c13762dd3dc0d005df1e71d2921b27b3894ee` |
| `D:/MashedWT/D4-WSH-EXEFILE_data/A0.log` | `fb83f62dc8ee41f5dccabe6519df2c8195f583e7b3053dd2c68fa95df0fda94c` |
| `D:/MashedWT/D4-WSH-EXEFILE_data/A1.json` | `b39be41ea9345cf9f060a573035c13762dd3dc0d005df1e71d2921b27b3894ee` (same bytes as A0: same dt, same table) |
| `D:/MashedWT/D4-WSH-EXEFILE_data/A1.log` | `0eaca776975f4434aeeaadf4f509dd401dcc52f2eefba6263acfbbc9b1b78c63` |
| `D:/MashedWT/D4-WSH-EXEFILE_data/A2.json` | `049d72091c93d62862eafa775a1ab05d2b7ba6756cf78e09d8771ec054b46902` |
| `D:/MashedWT/D4-WSH-EXEFILE_data/A2.log` | `bf167c355a7b3019c447d5219ac778036c1f0f923b83a1b9229ce0cfd39aedbe` |
| `D:/MashedWT/D4-WSH-EXEFILE_data/decomp_getters.txt` | `1cfdb6e0f4b19e116f4c63301568820343ef34838ce88231b1acf8f320d0fa3d` |
