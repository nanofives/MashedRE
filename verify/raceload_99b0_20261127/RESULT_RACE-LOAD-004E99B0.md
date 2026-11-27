not met, stop moved from 0x004E99B0 to 0x005595D0 (caller 0x00559b50) on the object build: race.n 0, reached false; CUT wall count 108 -> 86; 0x00420420 executions on a knob-on arm 0

# RESULT RACE-LOAD-004E99B0 (run 57, G1, race-path row)

Branch `orch/RACE-LOAD-004E99B0`, worktree `.worktrees\RACE-LOAD-004E99B0`. Prep on c880ab12e, then rebased onto **d20fd3984**
(L0 "SYNC merged"; clean rebase, `git -c merge.autoStash=false`). Every check counted below ran on the rebased tree, commit
46ad1a749 plus this RESULT. PREREG dbe8d310b (committed alone, before any arm). Worker claude-opus-5-5 high on claude3.
No new knob, no default change: every body added here runs only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD (+ the
ROLOADRWC knobs MASHED_RWC_CORE / MASHED_RWC_FRAME). `git diff orch/integration HEAD -- re/tools/bind/porttop_bind.py`
is empty (RULE 1). G-CTRL / B / 12x4: not run (row kind).

## 0. Verdict

| item | result |
|---|---|
| G1 done-means (decision B) | **not met**: exe load length under MASHED_ORIG_TOP on the object build not above 3 (`race.n` 0, `race.reached` false, arm RO_ROLOADRWC-rlF); race.n 0 on every ROLOADRWC arm of this row (rl1..rl13, rlF) |
| stop, object build | **moved from 0x004E99B0 (caller 0x004b3c60) to 0x005595D0 (caller 0x00559b50)**, 12 stops later. The load now reads graph.bsp, collisions.bsp, Lap.bsp (three worlds through the new 0x004e99b0 body: 3 calls counted), Lake.uva, Camman.dff, Water02/03.dff, Tank1..4.dff, Lake.dff, Road.dff, Occluder.dff, Lights.dff, Smoke.mts, sky.dff, H_Anim_1..4.anm, and stops after the `Searching piz for LAPDATA.LUA` line |
| CUT wall count (RACE-PIZ-DEVICE census method, same window) | **86** functions / 25,890 B (head d20fd3984 control: 108 / 32,236 B) |
| 0x00420420 execution count, knob-on arm (read only) | **0** (Frida entry counter; 0x0040d110 also 0, 0x0040d270 1). The race load still stops inside the course load, before 0x0040d270 calls 0x0040d110 |
| race frame body on a knob-on arm | **no** |
| knob boot (VS2022 exe, 600 frames) | `WinMain 0x00492370 returned 0 after 600 frames`, first run (no short exit, no re-run needed) |
| binder selftest | **PASS** (0 FAIL), after regenerating raceframe / gates outputs (section 4) |
| ddkobj check vs the head (IDENTICAL 2455/2562, DIFFERENT 79) | IDENTICAL 2467/2576, DIFFERENT 83: 0 gone, 4 newly DIFFERENT, all replay-C4 bodies (section 5) |
| rva-lint | 670 findings, allowlisted 670, NEW 0 (head 662); ORIGTOP-ONLY 655 (head 647, +8 this row); D4 burn-down 4 (unchanged) |
| hash moved | none run (row kind) |
| decision C | 0x0055f800 / 0x00487280 not touched (both still named stops in the raceload plan); the next stops after 0x005595D0 include 0x0055f800 (section 7) |

## 1. The 0x004E99B0 stop and what the stop chain needed

0x004e99b0's source (re/match/rwl02/004e99b0.c) existed at 45.5% but as raw g2s output it read the 0x40-byte struct chunk
through 16 separate locals and stored float products into dword slots, so as C it did not compute the original's values.
Rewritten from the disassembly 0x004e99b0..0x004e9e33 (Ghidra Mashed_pool0, read-only; frame and constant addresses in the
source header). The same defect class (struct chunks read into separate locals, float stored through a dword, register-arg
callers) recurred along the chain. Every stop and its fix, in arm order:

| arm | stop (caller) | fix | body class |
|---|---|---|---|
| sg3 (start) | 0x004E99B0 (0x004b3c60) | source rewritten; callees 0x004e9e40 / 0x004ea220 rewritten, 0x004e51e0 checked against the disassembly, 0x004e5280 bound (C4), 0x004f3e90 aliased to the lifted core body RwCoreLift_004f3e90 | provisional x4, match x1, lift |
| rl1 | 0x00558DF0 (0x0042a740, Lake.uva) | source rewritten (error record as two locals, not C++); C4 list helpers 0x005c4d20 / 0x005c4e10 bound | provisional, match x2 |
| rl2 | 0x005518F0, RW core path (RwCore_005515a0) | the RW core generator's EXE_BODY table now names its C4 raceload C unit PortTop_Bound_005518f0 (1 entry in verify/rwccorerun_20261111/gen_rwcore_tus.py, the 3 generated lines it changes: RwCore_005515a0.cpp, RwCore_Missing.cpp, RwCore_Bodies.tsv) | match (existing) |
| rl3 | 0x00558CB0 (0x005518f0) | the 7 roots-only stops of the raceload plan bound as roots (0x0041a5b0, 0x0041a5d0, 0x00558cb0, 0x004783f0, 0x004b53b0, 0x00559560) plus their matched callees 0x00409300, 0x00409330, 0x00563de0; 0x0041a5b0 / 0x0041a5d0 bound as C units (cpp-unclean, `unclean` step), extern guard applied to both sources (BYTE-IDENTICAL after) | match x7, replay x4 |
| rl4 | 0x004CF2B0 (0x00478110, after Smoke.mts) | source rewritten (13-dword header as separate locals) | provisional |
| rl5 | AV EIP 0 after 0x004cf2b0 -> 0x004c4670 -> 0x004c4680 | re/match/s19/004c4680.c rewritten (raw g2s used separate float locals as 3-vectors through pointers and converted the flags word float -> uint -> float; under VS2022 it wrote over its own return slot). It is the RW core body RwCore_004c4680; its PROVISIONAL line replaced | provisional (row replaced) |
| rl6 | 0x004790E0 (0x00479330), register-arg | new TU Match/PortTop_RL_004790e0.cpp compiles the C4 source and exposes a stack-arg entry; raceload `alias` replaces the old `stop_reason`; re/match/s12/00479330.c passes the world under MATCH_PORT only (DDK branch untouched, match.py 97.2% before and after) | match (ddkobj ___ddkreg_004790e0 IDENTICAL) |
| rl7 | 0x004B4550 (0x00478660 / 0x00479030 / 0x004b53b0) | `prov_bind` of the C3 hand source | provisional |
| rl8 | 0x004B4EB0 (callback of 0x004b5030) | source rewritten (raw g2s left out the count update and the early return at the limit, 0x004b4ffd..0x004b500a; its preamble redefined bool / wchar_t) | provisional |
| rl9 | 0x004785E0 (0x00478660), register-arg | new TU Match/PortTop_RL_004785e0.cpp over the replay-C4 source; `alias` replaces the `stop_reason` this row had added in rl4 | replay |
| rl10 | 0x004C3D90 (0x00478660 / 0x0047bf70 / 0x00485460) | its c_hold.txt line removed (per-RVA ledger, this row's RVA), `alias` to the lifted core body RwCoreLift_004c3d90 | lift |
| rl11 | 0x004A3384 CRT acos (0x004783f0) | new Match/PortTop_CrtMath.cpp: MashedCrt_acos_004a3384 runs the original's x87 sequence (control word 0x027f, fpatan(fsqrt((1+x)(1-x)), x), 0x004a3394..0x004a33b6) for abs(x) < 1; abs(x) >= 1 and NaN use the exe CRT acos [UNCERTAIN, register line] | hand CRT |
| rl12 | 0x0057C4B0 (0x0047f4c0) | new source re/match/rwl06/0057c4b0.c, BYTE-IDENTICAL (rw9044 and ddk), relocheck OK; re/promotions/RACE-LOAD-004E99B0.tsv (route match; `c4_reclass.py batch --tsv` dry run: 1 pass) | match (pending promotion) |
| rl13, rlF | **0x005595D0** (0x00559b50) | not worked (section 7) | - |

Rule kinds used, all in re/tools/bind/state/raceload/ (rules.tsv, roots.txt, c_hold.txt): `prov_bind` x9, `alias` x5
(0x004f3e90, 0x004790e0, 0x004785e0, 0x004c3d90 and the existing CRT route by name), `stop_reason` 0x004790e0 replaced,
roots +22. Generated outputs regenerated with `write --roots-only` / `externs` / `sync` / `unclean` / `prune`.

## 2. Bodies, with class (RULE 9: match, replay and provisional apart)

match.py --bytes and relocheck.py on every source this row binds or edits: `scope_match.tsv` (`scope_match.py`).

| class | rvas | match.py --bytes | relocheck |
|---|---|---|---|
| **match (byte-identical)**, bound or rebound here | 0x004e5280, 0x005c4d20, 0x005c4e10, 0x0041a5b0, 0x0041a5d0, 0x00409300, 0x00409330, 0x00563de0, 0x004790e0, 0x005518f0 (RW core callers), **0x0057c4b0 (new source, pending promotion)** = 11 | all BYTE-IDENTICAL | all OK |
| **replay-C4** (hooks.csv c4=frida-replay, not byte matches) | 0x00558cb0, 0x004783f0, 0x004b53b0, 0x00559560, 0x004785e0 = 5 | 78.4 / 90.8 / 45.4 / 68.3 / 97.8% | BAD x4, OK x1 (expected below 100%) |
| **provisional** (NOT-PROVEN, PROVISIONAL.tsv, class P3, never C4) | new rows 0x004e99b0 (69.5%), 0x004e9e40 (34.1%), 0x004ea220 (60.5%), 0x004e51e0 (90.7%), 0x00558df0 (49.8%), 0x004cf2b0 (47.8%), 0x004b4550 (63.3%), 0x004b4eb0 (22.0%); replaced row 0x004c4680 (37.3% rw9044 / 34.1% ddk) = 9 | as listed | BAD except 0x004e51e0 OK |
| lifted core bodies reused (existing PROVISIONAL rows) | 0x004f3e90, 0x004c3d90 | n/a | n/a |
| hand CRT | 0x004a3384 | n/a | n/a |

equiv.py (merged tool 1ed7ceb1a, unchanged on d20fd3984): every provisional source NOT-PROVEN, verdict lines in
`equiv_<rva>.txt` and in each PROVISIONAL.tsv row. None is an admission, none needs a decision row (all callers read the
result or are void, listed per row). PROVISIONAL.tsv: one row per RVA, `git show HEAD:re/match/PROVISIONAL.tsv | cut -f1 |
sort | uniq -d` prints nothing. `prov_drop.py` dry run: none of this row's provisional sources is byte-identical
(`prov_drop.txt`). Sources rewritten: 0x004e99b0, 0x004e9e40, 0x004ea220, 0x00558df0, 0x004cf2b0, 0x004b4eb0, 0x004c4680,
plus the new 0x0057c4b0; 0x00479330.c edited under MATCH_PORT only.

## 3. Binding numbers

- Units reachable from a bound caller on the raceload plan (top-down path): bind 299, reuse 162, stop 31 (head d20fd3984
  plan.json: bind 278, reuse 140, stop 46). Measured executing on the object-build arm (Frida count, section
  6): 0x0040d270 1, 0x004b3c60 3, 0x004e99b0 3; the arm log shows each listed asset load completing, so the bodies under them
  ran, but only those five symbols were counted.
- Pre-bound units (keep-alive only, Match/PortTop_BulkRefs.cpp): 126 (head 127: the PortTop_Keep_005518f0 anchor left, since 0x005518f0 now has a bound caller, RwCore_005515a0).
- Bindable rows left unbound, with reason: 0x005595D0 (the new stop: C3, raw g2s 57.5%, not C++-clean, stores the float
  param_4 into a dword slot, 1,381 B, needs a rewrite from the disassembly); 0x0055f800 and 0x00487280 (decision C); 0x00409680,
  0x00425bf0, 0x00451730 (c_hold, not reached); the other 25 raceload stops not reached on any arm (plan.json).
- Listed unit -> 100% neighbour that fell back to its VS2022 body: **none** (every provisional source of this row defines
  one MATCH function).
- Dual bodies (rva-lint ORIGTOP-ONLY lines this row added, 8): DUP-IN-TARGET 0x004e99b0 / 0x004e9e40 / 0x004ea220 (hand
  D3d9Render/RwWorldLoad.cpp, frozen, untouched), 0x00409300 / 0x00409330 (Gameplay/RangeTable_ah1.cpp), 0x004b4550
  (Util/Vec3Centroid.cpp); CROSS-TARGET 0x0041a5b0 (Util/PromoLoop_round62.cpp), 0x00559560 (Lane2/L2_00559560.cpp).
  ddkobj "dual body" count 33 (unchanged).

## 4. Binder selftest, other profiles, BINDINGS.tsv

The first selftest on the rebased tree gave FAIL 11: the raceframe and gates plans now reuse this row's new exe bodies
(raceframe: MashedCrt_acos_004a3384, PortTop_Bound_004b4550; gates: PortTop_Bound_00409330), and the audio / bulk
BINDINGS.tsv rows differed by one each. RULE 3: regenerated raceframe and gates with `write` + `sync` (7 generated files, 2
plan.json; no state file touched). Selftest then **PASS** (`binder_selftest.txt`).

`bindings --profile raceload` with the merged tool (L0's request) was run and **reverted** (53dd958a7, reverted by
5a8e6c268): it updated 15 rows and added 301, and the 15 included rows other profiles write (0x004a34b0, 0x0057c210 audio;
0x0041a8d0, 0x0055b940 bulk; 0x0055dff0 and 0x0057c210 are BIND-BINDINGS-BULK-DRIFT's rows; 0x00487280 is decision C), which
put the audio and bulk selftests at DIFFERENT. BINDINGS.tsv is therefore byte-identical to d20fd3984. The 5 SYNC rows came
out `near-match` in that run, as L0 expected. This is the cross-profile drift BIND-BINDINGS-BULK-DRIFT owns; the raceload
rows (316) are left for after it.

## 5. ddkobj check (object build, 46ad1a749 + the build of this tree)

`[ddkobj summary] IDENTICAL 2467/2576  DIFFERENT 83  not-in-map 26  units linked 2451/2736  refusals 17
(externalized-static-register-convention=12, static-global-unmapped=1, wrapper-redefines-globals=4)  provisional kept
VS2022 268 units + 7 fns  dual body 33  fn-addr refs ok 66 bad 0`. Head (L0, d20fd3984): IDENTICAL 2455/2562, DIFFERENT 79.
DIFFERENT list against the head's: 0 gone, 4 new, each classified **replay-C4 body** (hooks.csv c4=frida-replay; the
binder classifies each bind as `replay`): 0x004783f0 (209 B differing), 0x004785e0 (7 B, ___ddkreg_ unit), 0x004b53b0
(412 B), 0x00559560 (103 B). [UNCERTAIN] that each bound file is the replay-verified one: hooks.csv does not name the
source file (register line). New units IDENTICAL: 0x004e5280, 0x005c4d20, 0x005c4e10, 0x0041a5b0, 0x0041a5d0, 0x00409300,
0x00409330, 0x00563de0, 0x0057c4b0, 0x004790e0 (___ddkreg_). Provisional units (skip:provisional, VS2022 body): the 9 above
plus 0x00558cb0 (listed by RWC-PLUGIN-ATTACH). BYTECHECK.tsv committed here.

## 6. Checks run (rebased tree unless a row says otherwise)

| check | result |
|---|---|
| mashed_lock.py status before every build, check, selftest and arm | free each time (once HELD by BIND-0048AE00-CALLEES, once PRIORITY-LINGER: waited by the wrappers) |
| test-slot --status / C: free before builds | printed each time; C: 49 to 78 GB free, never under 25 |
| VS2022 build (`MASHED_NO_DEPLOY=1`) | Build OK (build_vs_F.log); 14 earlier rounds during the stop chain, 2 failed and fixed (C++ errors 0x0041a5b0 / 0x0041a5d0 -> `unclean` + C units; LNK2019 0x004785e0 static) |
| object build (`MASHED_DDKOBJ=1`) + ddkobj check | Build OK, section 5 |
| binder selftest | PASS (section 4) |
| knob boot kb1 (VS2022 exe, MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600) | rc 0, `WinMain 0x00492370 returned 0 after 600 frames`, first run |
| arm RO_ROLOADRWC-rlF (object build, exe 16dff3a9, staged image 81cc7775) | exit 5608912 = 0x005595D0 after 6 s; race.n 0, reached false; warp n 2402 tick 300; selftest 22196 checks 0 fails; OT_TIMER self-test 2990 checks 0 failed |
| Frida entry count (verify/bindgates_20261127/tools/count_body.py, same knob set, object build) | 0x0040d270 1, 0x0040d110 0, **0x00420420 0**, 0x004b3c60 3, 0x004e99b0 3 |
| CUT wall re-cut (verify/racepiz_20261127/cut_join.py on prov_anchor.json 4043240e..., this tree / head d20fd3984) | 86 / 108 (`cut_walls.tsv`, `cut_summary.txt`) |
| match.py --bytes + relocheck.py, 26 sources | section 2, `scope_match.tsv` |
| equiv.py (merged tool) on 10 sources | NOT-PROVEN x10, `equiv_*.txt` |
| c4_reclass.py batch --tsv re/promotions/RACE-LOAD-004E99B0.tsv (dry run) | 1 pass (0x0057c4b0 match), 0 refused |
| prov_drop.py (dry run) | none of this row's rows |
| PROVISIONAL.tsv duplicate keys on HEAD | none |
| rva-lint (scripts/lint_rva_bodies.py) | NEW 0 (`rvalint.txt`) |
| frida-<hash> folders in %LOCALAPPDATA%\Temp | 0 before the first harness run; 1 new after the count run, frida-e2a76a2111f28b6fac1f5354a92ce4b9 (created 18:37:10, inside the count run, which ended 18:37:18): deleted; 0 after |
| G-CTRL / B / 12x4 | not run (row kind) |

## 7. Unfinished scope, next

- **0x005595D0** (caller 0x00559b50, RenderWare-Physics-3.7, 1,381 B, C3): next stop. Its raw source needs a rewrite from the
  disassembly (separate locals, `puVar7[3] = param_4` float into a dword slot). The raceload plan shows 0x0055f800 (decision
  C held) under 0x0047f4c0's subtree next to it, so the chain is about to reach a held gate; the census lists 44 of the 86
  remaining walls in RenderWare-Physics-3.7.
- BINDINGS.tsv for raceload (section 4): left to after BIND-BINDINGS-BULK-DRIFT.
- gen_rwcore_tus.py is not in sync with the tree on the head (a full regenerate would rewrite PORT-TOP-RENDERFRAME's hand
  lines in RwCore_00492e90.cpp and 1,748 rsp lines): only the 3 lines this row's EXE_BODY entry changes were kept.
- [UNCERTAIN] lines added to re/orchestrator/UNCERTAIN_REGISTER.md: acos edge path; replay-C4 source identity.
- The raw g2s defect class seen 8 times here (struct chunks read through separate locals, floats stored through dwords):
  every raw g2s source on the race-load path deserves the same check before it is bound.

## 8. Raw files (`D:/MashedWT/RACE-LOAD-004E99B0_data`)

| file | sha256 |
|---|---|
| build_vs_F.log | d8acb73ea11681295a0b57b64f6c1c7cc50aeee6440fc34974813f2b1a81c356 |
| build_ddk_F.log | c8e2fe08cde6d3611d24348d91e019bbc320b133dc29d5f57c692b615d8d5fa2 |
| kb1.log / kb1.console.txt | bc839f22b47ed2799198d699a5bcb8a8e53c0221c91dc75e4ac91133eb8c81e8 / 826ca507beebb950548c43217e362690af7c7f98107ab9af67df3472011b8bbe |
| RO_ROLOADRWC-rlF.log | d79176aff2fa5721ee6f35ca20ad89d6fa4a9f3dfb1cb8b53da8af362e9e06a7 |
| RO_ROLOADRWC-rlF/meta.json | a3738e3e9c1f3508dcdbdf0c645ec0f9dacdb05f4eeeabc6b9083340758aad12 |
| RO_ROLOADRWC-rlF/ticks.bin | 1d90cce4fc6fc0f8b59f6b04b64f1f129bdc12210671200f40b6379d80eb36e5 |
| RO_ROLOADRWC-rl1.log (first stop move) | b6d7d4f9dd57bd841dcea1f4d74959583b7ea39f6b4d55a74ed266953b9866d1 |
| RO_ROLOADRWC-rl5.log (the 0x004c4680 AV) | c9d14ef704027f150256c6283094446e8f86ebc8206d26f497da0e84bbda73d5 |
| count_rlC.txt | 0f551accefd8507b598ce934c5a57a7afa9c296543966d69deca682c92837157 |
| frida_before.txt / frida_after.txt | e3b0c442... (empty) / 58094cdf... |
| worktree mashedmod/build/ddkobj/mashed_re_ddk.exe | 16dff3a9391ab9ff74c7b32c78849b415676ef61a8b82cbc31a99e84e6a23c7b |
| worktree mashedmod/build/mashed_re.exe | bdda787e2d0fa4953cf3d4ac1ab4a2f3a1de9da6b92b18a51d230247d0904b2c |
| BYTECHECK.tsv (committed here) | a2067b2082d03a6a238a4bf7ea2d6bc3ea7eb8ab7b1bb177d755a9f8b951cbd0 |
| census prov_anchor.json (RACE-PIZ-DEVICE, read) | 4043240e0ec2b0b1b848a1e4d41ec86cd95128796b601d1f1ff0c6fb2d8f8fbb |

Committed here: this RESULT, PREREG, scope_match.py / .tsv, equiv_*.txt (10), cut_walls.tsv, cut_summary.txt,
binder_selftest.txt, prov_drop.txt, rvalint.txt, BYTECHECK.tsv, scope_match.txt (first step).
