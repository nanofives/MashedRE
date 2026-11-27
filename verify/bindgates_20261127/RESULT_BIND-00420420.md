0x00420420 body executions on the knob-on arm: **0** (3 arms, object build + staged image: race load stops at RwCore_005509b0+0x4 after "Searching piz for COURSE.LUA", before 0x0040d270 calls 0x0040d110). Bind landed: 1 unit bound, keep-alive 126 -> 127, binder selftest PASS 0 every profile, ddkobj 0 newly DIFFERENT.

# RESULT BIND-00420420 (run 56, BIND section)

Base: **orch/integration affd19dd4** (rebased from f10b8e69c after drain 1 pass 2; `git diff --name-only f10b8e69c affd19dd4` touches none of re/tools/bind, dual_copy_allowlist.txt, exe_sources.rsp, mashedmod/, re/match/, hooks.csv; BIND-TOOLS-SMALL-3 merged at 2ca0564f0). Branch `orch/BIND-00420420`, worktree
`.worktrees\BIND-00420420`. The branch was first stacked on orch/BIND-TOOLS-SMALL-3 a20f17480 (prep only, nothing counted
from it), then rebased onto f10b8e69c; builds, ddkobj, arms and knob boots below ran on f10b8e69c + this row's commits; the binder selftest and the bindings comparison were re-run on affd19dd4 + this row (selftest output byte-identical). Row kind: top-down binding
row, data only (RULE 1): `git diff orch/integration -- re/tools/bind/porttop_bind.py` is empty. G-CTRL / B / 12x4: not run (row kind).

Decision B: the done-means "body executes on a knob-on arm" is **not met: stop at RwCore_005509b0+0x4 (COURSE.LUA VFS open, owned by
RACE-VFS-COURSELUA)**. The measured count is 0; it is reported, not hidden. No hash moved (no default-path change; gates not run).

## 1. Change (profile data + generated outputs)

- `re/tools/bind/state/bulk/roots.txt`: +1 comment line, +1 root `00420420` (363 roots).
- `write --profile bulk`: `Match/PortTop_00420420.cpp` new (includes `re/match/s05/00420420.c`, its file-statics renamed
  `PortTop_Local_00420420_0041f330/_0041f360` and `0041fe10` kept file-local); `Match/PortTop_0041fe10.cpp` **removed** (profile
  `drop_local_copies`: 00420420.c carries the only exe copy of 0x0041fe10); `PortTop_0040d110.cpp` now maps `FUN_00420420` to
  `PortTop_Bound_00420420` (was the named stop); `PortTop_BulkStops.cpp` regenerated; `exe_sources.rsp` (sync).
  Plan totals: bind 97 -> 97 (0x00420420 in, 0x0041fe10 out), reuse 204 -> 211, stop 188 -> 191; match 96 + replay 1 unchanged.
- `refs --profile raceload`: `PortTop_BulkRefs.cpp` +`PortTop_Keep_00420420`, -`PortTop_Keep_0041fe10` (net 126 -> 127 bound bodies
  with no bound caller listed; the 126 -> 127 is the second refs run on the rebased tree, idempotent after).
- `moved.tsv` (via the new `moved` command, existing lines byte-identical, appended only): 0x0041fe10 (bind -> stop, cause
  "BIND-00420420 source carries 0x0041fe10", lead ruling), 0x00420420 (stop -> bind), the 10 new callee lines of 0x00420420 in the
  bulk listing (cause "BIND-00420420: new callee of 0x00420420 in the bulk listing"): stops 0x00420290, 0x00544a70, 0x00544ad0;
  reuse 0x0042a6b0, 0x004b52c0, 0x004c15c0, 0x004c5c00, 0x004c5c80, 0x0053fb70, 0x0053fd30; plus totals lines. Without the 10 the
  selftest read FAIL 10 ("moved, not named"). 0x00544a70 / 0x00544ad0 (owned by BIND-0048AE00-CALLEES) are **listed stops only**;
  nothing is bound there; if that row binds them, `bound_wins` reuses its bodies.
- `re/tools/dual_copy_allowlist.txt` +2 lines:
  - `ORIGTOP-HELD DUP-IN-TARGET:0041f330:Match/PortTop_00420230.cpp,Match/PortTop_00420420.cpp` (held pair, precedent :674/:675, lead OK).
  - `DUP-IN-TARGET:0041f360:Match/PortTop_00420420.cpp,Match/PortTop_00420870.cpp,Vehicle/CarMaterialTables.cpp`, **untagged D4**:
    2 marked held copies + 1 unmarked hand copy (GatedSwitch636ad0, CarMaterialTables.cpp:111); no lint class covers it
    (ORIGTOP-ONLY needs exactly one marked, ORIGTOP-HELD none unmarked). **Decision L-183, Mariano: option (1)**. Moves the
    **D4 burn-down 4 -> 5**; the old :730 key (two files, ORIGTOP-ONLY) no longer matches a finding and is superseded by this line.
    Follow-up lint tool row LINT-HELDPAIR-HANDCOPY (retags it) is filed separately by the lead.
- `bindings --profile bulk` **not written: head drift, not this bind** (L0 ruling). Run (written, diffed, file restored) on
  head **affd19dd4** without the bind: "updated 4 added 425"; on this branch rebased onto affd19dd4: "updated 4 added 429". The same 4
  rows change on both, each a named stop another profile owns that the bulk plan also lists as a stop, relabelled to the bulk stop symbol:
  `0x0057c210` PortTop_AStop_0057c210 (audio, PORT-TOP-AUDIOTICK-STOPS) -> PortTop_BulkStop_0057c210; `0x00401da0`, `0x00535700`,
  `0x0055dff0` PortTop_RFStop_<rva> (raceframe, PORT-TOP-06) -> PortTop_BulkStop_<rva>. The +4 added rows (429 vs 425) are the bind's
  new bulk plan entries; BINDINGS.tsv is not regenerated (L0 files a separate row). Earlier note: its dry run (`BIND-00420420_bindings_dry.txt`, "updated 4 added 429") rewrites 4 rows of
  the audio / raceframe profiles as bulk stops and adds the whole bulk plan; the profile's documented order is write -> refs -> sync,
  and BINDINGS.tsv is selftest-checked from the audio plan only. BINDINGS.tsv byte-identical to the base.
- Tools (evidence only, not wired anywhere): `tools/count_body.py` (Frida entry counter by .map address), `tools/arms.py`,
  `tools/knob_boots.py` (copy of verify/bindgates_20261121/tools/knob_boots.py with this row's data/root/label).

## 2. Binding numbers

1. **Units executing or reachable from a bound caller on the top-down path: 1 bound** (0x00420420 <- 0x0040d110, bound in bulk,
   called from 0x0040d270 under `MASHED_OT_RL_D110=1`; call site in the original 0x0040d1fb, `push esi / push ebx / call / add esp,8`).
   Its file-local copies 0x0041fe10 (called at 0x00420763 with the record in EAX), 0x0041f330, 0x0041f360 come with it.
   **Executing: 0** (section 5).
2. **Pre-bound keep-alive units in PortTop_BulkRefs.cpp: 127** (126 on the base; +0x00420420, the 0x0041fe10 keep is gone with its wrapper).
3. **Bindable rows left unbound: 0.** The callee stops of 0x00420420 have no matched or admitted body: 0x00420290 (C2), 0x004222f0
   (no hooks.csv row, `-`), 0x0053d460 (C2), 0x00544a70 / 0x00544ad0 (C2, owner BIND-0048AE00-CALLEES). 0x0041fe10 is not unbound:
   its body is in the exe as the 00420420.c file-local (ddkobj IDENTICAL, section 3).

## 3. ddkobj (object build, f10b8e69c + bind) vs log\orch\run56_headcheck\ (1379467a6)

| | head check | this tree |
|---|---|---|
| IDENTICAL | 2360 / 2526 | **2363 / 2529** |
| DIFFERENT | 78 | **78** (same 78 rvas) |
| not-in-map | 88 | 88 (same rvas) |
| refusals | 34 (externalized-static-register-convention 13, wrapper-redefines-globals 21) | 34 (same) |
| dual body | 33 | 33 (same rvas) |
| units / linked | 2694 / 2405 | 2694 / 2405 |

New BYTECHECK rows, all IDENTICAL, all from re/match/s05/00420420.c: `_PortTop_Bound_00420420` (0x00420420, 1104 B),
`___ddkreg_0041f330_00420420` (0x0041f330), `_PortTop_Local_00420420_0041f360` (0x0041f360). The 0x0041fe10 row (`___ddkreg_0041fe10`)
now comes from 00420420.c instead of 0041fe10.c, still IDENTICAL. **Newly DIFFERENT units: 0.** Pairs "listed unit -> 100% neighbour
that fell back to its VS2022 body": **none**.

## 4. match.py --bytes + relocheck.py on the included source (re/match/s05/00420420.c, on this head)

4/4 symbols 100.0% BYTE-IDENTICAL (relocations masked): 0x0041f330 47 B, 0x0041f360 97 B, 0x0041fe10 401 B, 0x00420420 1099 B
(317/317 ins). relocheck 4/4 OK (0x00420420 checked=62, unchecked=2: `@__security_check_cookie@4`, `___security_cookie`).
**Match-C4: 4** (hooks.csv C4 `c4=match` on all four). **Replay-C4: 0.** No promotion file (nothing promoted by this row).

## 5. Measured execution count of 0x00420420 on knob-on arms

Frida entry counter (`tools/count_body.py`, Interceptor on the .map VA of each symbol, cold functions), test-slot outside,
mashed_lock inside, private root `D:\MashedWT\BIND-00420420_root` (robocopy of original\ + staged `original\mashed_image.bin`
SHA-256 81cc7775...afcd8, KNOBON-BOOTS-G5's, per L0). Knob set = ROLOADRWC + `MASHED_OT_RL_D110=1`: `MASHED_ORIG_TOP=1
MASHED_OT_RACELOAD=1 MASHED_OT_RL_D110=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=100000 MASHED_DETERMINISTIC=1
MASHED_DET_FRAMES=600 MASHED_PORTSTATE_IMAGE=1 MASHED_REPLAY=verify/replayoracle_20261113/replay_trace.json
MASHED_REPLAY_TRACE_CKPT=1,150,299,301,450,600 MASHED_WIN_POS=primary-bl` (+ `MASHED_OT_RL_STOPS=count` on the C arms).

| arm | exe | Orig_0040d270 | PortTop_Bound_0047b8d0 | PortTop_Bound_0040d110 | **PortTop_Bound_00420420** | end |
|---|---|---|---|---|---|---|
| RO_G5D110_ddk | object build | 1 | 1 | 0 | **0** | AV exe+0x1E824 = RwCore_005509b0+0x4 read 0x38, log line 463 after "Searching piz for COURSE.LUA" |
| RO_G5D110C_ddk | object build | 1 | 1 | 0 | **0** | same |
| RO_G5D110C_vs | VS2022 | 1 | 1 | 0 | **0** | AV exe+0x1FD9F4 = RwCore_005509b0+0x4 read 0x18, same log point |

This is RACE-LOAD-LUA-2's end stop (COURSE.LUA VFS open returns 0, owned by RACE-VFS-COURSELUA). 0x0040d110 is called at
re/match/top07/0040d270.c:107 / :115, after that course-load path, so it and 0x00420420 are not reached. **No race-frame execution.**
Prep runs without the staged image (stacked base, not counted) stopped earlier at Orig_0040d270+0x5C / +0x109 (table 005F2728 NULL,
RO-ARM-VS2022-AV's finding); per L0 not chased.

Knob boots (menu, 3000 frames, `tools/knob_boots.py`, image staged in the root): knob_vs_base, knob_vs_all, knob_ddk_base,
knob_ddk_all all **PASS** (game rc 0, "WinMain 0x00492370 returned 0 after 3000 frames"). `BIND-00420420_knob_boots.txt`.

## 6. Checks run (all on f10b8e69c + this row)

| check | result |
|---|---|
| mashed_lock status / test-slot --status before each build, check, selftest and arm | free or PRIORITY-LINGER (waited by the wrappers); C: 54-60 GB free; commit 26.6-28.8 / 36 GB |
| VS2022 build (`MASHED_NO_DEPLOY=1`) | Build OK |
| object build (`MASHED_DDKOBJ=1`) + ddkobj check | Build OK; section 3 |
| rva-lint | 659 findings, allowlisted 659, NEW 0; D4 burn-down **5** (was 4, L-183), ORIGTOP-ONLY 644, ORIGTOP-HELD 10 (was 9). `BIND-00420420_rvalint.txt` |
| binder selftest | **PASS 0, every profile** (raceload, audio, bulk, raceframe, renderframe, gates). Diff vs log\orch\run56_selftest_head.txt: only bulk's two lines (roots 362 -> 363, plan IDENTICAL bind 97 reuse 211 stop 191, wrappers+stops IDENTICAL 98; listing 60 -> 71 moved, all named). `BIND-00420420_selftest.txt` |
| match.py --bytes / relocheck | section 4 |
| knob-on arms with counter | section 5 |
| knob boots | 4/4 PASS |
| not run | G-CTRL, B, 12x4 captures (top-down row); menu walk (no menu code changed); `bindings --profile bulk` write (section 1) |

## 7. Open items / unfinished

- Execution of 0x00420420 needs race load past the COURSE.LUA VFS open (RACE-VFS-COURSELUA) and then the 0x0040d270 path to
  0x0040d110 under `MASHED_OT_RL_D110=1`. Re-measure with `tools/arms.py` once that lands.
- LINT-HELDPAIR-HANDCOPY (lead files it): a lint class for "held pair + hand copy" that retags the 0x0041f360 D4 line.
- `bindings --profile bulk` rewrites other profiles' BINDINGS rows: a tool question for a binder row, not done here.

## 8. Raw files (D:/MashedWT/BIND-00420420_data), SHA-256

```
06f8baa1b7e1d7ad7f0007c3b2c72a5c3a646e6457dc02c651c747c22175af3a  mashed_re_vs.exe
308e42bceb9017e93a319cbccad4ed9010b2c8870b6d8646dda3ecbe8cb1f89d  mashed_re_ddk.exe
9bd6d7c15b2d40b249b79598da6faf2320ef278bb5f06a65329e4b7c07cc713b  mashed_re_vs.map
e105c018373227e4ca7eda309196e6a44200016be830297206ff6f3fc0938c9f  mashed_re_ddk.map
02a108bde7cb0cba06df20dc5ddce3f3fe7c7c12bbba0a3b26f9dd7e070a8df6  build_vs2022.log
fdb5bddd425bec928110b9150552ab7084bc71c87f7a41a490650735ae0eb503  build_ddk.log
637f07a0b0ac73c08697869825100c3282b82a8b3ff23798f69aff3902647e56  BYTECHECK.tsv
49f48939a7041c04ee3962dd505c4fae23826d320e899cc25dfe6426466c683b  ddkobj_summary.json
8b1d4a1e5b290fdb1ec73b7072569871ae0eeb26826c9a0ce01612e7c5d51b76  count_RO_G5D110_ddk.txt
381f312df1bf7923b0def5ac5928271768656d903a516788d5253630182ab474  count_RO_G5D110C_ddk.txt
0b8c9eab7fa7f4c8b400bbc217613832bbc7e0b146682d9505bffd96af3aadab  count_RO_G5D110C_vs.txt
6fa1bc1bc152de5107badf94197c04d9bd66e8ab08d9e945a5ad2b23b1f7af3b  arms/RO_G5D110_ddk.log
e1462a9d5d4171e2ec90bb2848512113794bcab3e6e17af61abe93616df68998  arms/RO_G5D110C_ddk.log
ae429941c9369682ee1c18094ec15b2cd374d3cc88af2ff98aa365772cd313d7  arms/RO_G5D110C_vs.log
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  ../BIND-00420420_root/original/mashed_image.bin
```

Earlier read-only evidence (before the bind, base 1379467a6): `BIND-00420420_plan_bulk_head.txt`, `..._add00420420.txt`,
`BIND-00420420_plan_bulk.diff`; original-binary call scan: one direct call to 0x00420420 at 0x0040d1fb, one to 0x0041fe10 at 0x00420763
(MASHED.exe.unpatched, anchor SHA-256 BDCAE093...3C0E).

## 9. frida-<hash> folders in %LOCALAPPDATA%\Temp

Before the first Frida run this row recorded only a count (698), not a name list. Six folders are present now. Without a pre-run name
list none can be attributed to this row by name, and deletion by age is excluded, so **0 deleted**. Present at the end:
frida-316dbdfaf0005d08d77d0eb3454c4657, frida-32aaf3dc06bf0ce4aefca65061e34380, frida-32c9f35d4c6d37a53c6ce5ad8e93282b,
frida-59351c55e012ce88ce5d0589b4d72cc0, frida-61c4a9f37a4d464f7d35791d1f29db00, frida-f74065303b60b62cff18cc944f43e640.
