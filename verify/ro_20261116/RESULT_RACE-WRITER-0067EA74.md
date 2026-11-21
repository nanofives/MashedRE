# RESULT RACE-WRITER-0067EA74 (run 53, 2026-11-21)

Branch `orch/RACE-WRITER-0067EA74` from `orch/integration` `a9e5b4c8a` (unchanged at the RESULT, no rebase). PREREG
`verify/ro_20261116/PREREG_RACE-WRITER-0067EA74.md`, committed alone at `430789878` before any arm. Row kind: top-down /
race-path. Not run: G-CTRL, B, 12x4 captures (top-down row). No default flipped, no tracker edit, no promotion.

## 0. Verdict: BLOCKED at the RW core (0x004b5190 -> 0x00543d40 on a librw atomic). Writer found and bound. Load length not measured

| item | result |
|---|---|
| writer of `0x0067ea74` on the original (W1, measured) | **one write** from install to tick 600: pc `0x00431d10` (the instruction after the store, inside 0x00431d00 `CarSelectReset`), value 1, tick 0. Return chain `0x00492684` (inside 0x004924f0 `DataZeroFill`) -> `0x004923a4` (inside WinMain 0x00492370) |
| bound on ORIG_TOP | `MASHED_OT_RACELOAD=1` (default OFF) makes the 0x004924f0 stand-in run the matched `DataZeroFill` (`re/match/topm01/004924f0.c`, already compiled, PORT-TOP-04D), which calls the matched 0x00431d00 by name |
| effect on the exe | the matched 0x004264d0 enters with `DAT_0067ea74 = 1` (was 0), mounts `training.piz` and loads `powerups_gold.dff` (its piz / clump branch runs for the first time) |
| first stop | `0xC0000005` at exe+0x2E96D = `RwCore_00543d40+0xd`, read of address 0, inside 0x004264d0's call to 0x00426460 (section 4). Both knob-on arms |
| load length under `MASHED_ORIG_TOP` | **not measured**: no knob-on arm reaches the race anchor (`race.reached` false). ref1: 108. Knob-off control: 1 |
| first divergence against ref1 | record 0 (tick 1) on all three arms, as before. Knob-on IDENTICAL-FOR-N 16 of 283 (PROV4: 17 of 300). First NEW block `0x0067e800` at record 16 (section 3) |
| self-test | knob-off arm `[REPLAY]` PASS 0/666, `[TRACE]` PASS 0/33294, `[OT_TIMER]` PASS 0/5990. Every arm and the knob boot: `[portstate-image]` PASS 0/8, `[OT_CODEPTR]` PASS 0/367. Knob-on arms fault before `[OT_TIMER]` prints |
| object build | `ddkobj.py check` IDENTICAL 1803/1855, DIFFERENT 28, not-in-map 24. The same 28 DIFFERENT RVAs as PROV4 on the same integration head: **no newly DIFFERENT unit** |

Binding numbers: units executing or reachable from a bound caller on the top-down path: +1 caller site switched (0x004924f0
runs its matched body under the knob; 0x004924f0, 0x00431d00, 0x00431b80 and the 0x00431ae0 / 0x00431af0 / 0x00431b00 /
0x00431b10 / 0x0045b350 callees were already compiled, now executing on the knob path). Pre-bound units added: 0
(`PortTop_BulkRefs.cpp` not touched). Bindable rows left unbound: none for this row. Ownership: 0x004924f0 and 0x00431d00 are
in no list of `log/orch/run53_bindlists/`. 0x00431b80 is in `bulk_cp2.tsv` (dropped this run). It gets no binding of its own: it
is the `static` callee in the same source as 0x00431d00 (`re/match/s07/00431b80.c`), already compiled into `CarSelectReset`.
0x0045b350 is in `race_owned.txt` (this group).

## 1. Change (knob `MASHED_OT_RACELOAD=1`, default OFF, exe only, ORIG_TOP only)

`mashedmod/src/mashed_re/Match/PortTop_00492370.cpp`, `PortTop_StandIn_004924f0`: runs `DataZeroFill()` when
`MASHED_TOP04D_DATAZERO=1` (as before) **or** `MASHED_OT_RACELOAD=1`, and logs which. Knob off: unchanged (stand-in, returns 1).
Commit `5ad74958d`. Driver `verify/ro_20261116/writer_arms.py` (= `refname_arms.py` with this row's root, data and lock label).

PORT-TOP-04D measured that the matched `DataZeroFill` changes nav-demo walk_04 (0x007f0a40 defaults survive the hand init). That
stays off the default path: this row turns it on only under `MASHED_OT_RACELOAD=1`. The menu walk was not run (no menu code changed).

## 2. Arms and builds

Machine state: the test slot waited 600 s for memory before W1 (23.1 to 24.1 GB committed of a 24.0 GB budget, then started
under the cap). Before each build and arm, `mashed_lock.py status` showed `HELD 'mashed_machine' label=PROV-INSITU-REF1`
(no PRIORITY-HOLD, no SOLO). No other Mashed build.bat / ddkobj.py was running (Win32_Process). Commit 23.5 / 23.5 / 23.9 / 24.0 GB
at the VS build, the object build, the arms and the ddkobj check. The hub was not used (harness-lock runs stay local).

| run | exe / root | result |
|---|---|---|
| W1 (original) | MASHED.exe `b9977dab...fe87`, root `D:\MashedWT\RACE-WRITER-0067EA74_root` (robocopy of `RO-REF-FIXED-LOAD_root`) | exit 0, `race.dn_warp` 108, `bootlen` qpc 222, 1 watch hit. ckpt tick 299: `0x0067ea74` 1, `0x0067ea90` 1, `0x0067ea98` 4, the same as ref1 |
| VS2022 build | `MASHED_NO_DEPLOY=1` | `=== Build OK ===`, `mashed_re.exe` `24d2a2d2...4994376` |
| object build | `MASHED_DDKOBJ=1` | `=== Build OK ===`, `mashed_re_ddk.exe` `0a5dbaa1...f72c549f` |
| `ddkobj.py check` | | IDENTICAL 1803/1855, DIFFERENT 28 (same set as PROV4), not-in-map 24 |
| knob boot `knob_rl_on` | VS exe, root `D:\MashedWT\RACE-WRITER-0067EA74_exeroot` (robocopy of `PORT-OT-RACE-BIND-PROV4_root`) | 3000 frames, WinMain returned 0. Log line 78: `0x004924f0 DataZeroFill (matched, ...) returned 1` |
| `RO_ROIMG-pr1` (knob off) | object build | exit 0, 14 s, 600 ticks, load length 1, IDENTICAL-FOR-N 17 of 600 (unchanged) |
| `RO_ROLOAD-pr1` (stops exit) | object build | `0xC0000005`, 5 s, 283 records, IDENTICAL-FOR-N 16 of 283. PROV4: exit `0x0047B860` stop |
| `RO_ROLOADC-pr1` (stops count) | object build | `0xC0000005`, 5 s, the same fault and the same compare |

Prediction 1 (a hit before tick 299 with value 1) holds. Prediction 2 (pc inside 0x00431d00..0x00431d61) holds: 0x00431d10.
Prediction 3 (W1 ckpt 299 = 1) holds.

## 3. First divergence and the block 0x0067e800

All arms still diverge first at record 0 (tick 1, 120 counted blocks, 76 raw words), so nothing moved there. In block
`0x0067e800..0x0067ebff` the knob-on exe now matches ref1 at tick 1 (0 differing words). PROV4's exe at tick 17 had 6 differing
words there (`0x0067e9f8`, `0x0067ea74`, `0x0067ea90`, `0x0067ea98`, `0x0067ea9c`, `0x0067eaa0`). This row's exe has one:
**`0x0067e9f8` ref 1, exe 0, from record 16 (tick 17)**. At tick 299 it also has `0x0067e848` ref 2, exe 0. Since the block now
matches before record 16, the tick-hash compare counts it as a NEW block at record 16. That is the whole drop from 17 to 16.
The writers of `0x0067e9f8` and `0x0067e848` are not measured.

## 4. The first stop (both knob-on arms, `RO_ROLOAD-pr1.log` lines 827..837, `racewriter_log_extract.txt`)

1. `PortTop_Bound_004264d0(0000001E) entered, DAT_0067ea74 = 1, DAT_007d3ff8 = 0110F468`.
2. The 0x00458c70 stand-in (owned by RACE-RWGLOBALS-FRAMELIST, untouched).
3. 0x00495280 mounts `c:\toastart\tracks\training.piz` (43 entries).
4. 0x0042a5d0("powerups_gold.dff") loads 11902 bytes from the piz. `RpClumpStreamRead` goes to the librw binding
   (`rw::Clump::streamRead`, 17 atomics) and returns a non-zero clump, so 0x004264d0 calls 0x00426460(clump)
   (`re/match/s06/004264d0.c`, `if (clump != 0) { FUN_00426460(clump); FUN_004e6e00(clump); }`).
5. Fault `C0000005` at exe+0x2E96D. With `mashed_re_ddk.map` (preferred load `0x00010000`) that is VA `0x0003E96D` =
   `_RwCore_00543d40` (`ddk_rwl04_00543d40_RwCore_00543d40.obj`) + 0xd. Fault address 0, eax 0, ecx 0.

[UNCERTAIN] The call path from 0x00426460 to the fault comes from the sources and the log order, not from a backtrace.
0x00426460 calls `FUN_004b3fc0(clump, list)` (plan: `reuse Orig_004b3fc0`), then `FUN_004b5190(list[i], 0, 0)` (plan:
`reuse RwCore_004b5190`, a matched RW core body). `RwCore_004b5190` calls `RwCore_00543d40`, which reads
`*(int *)([0x007dc634] + p + 4)` (`re/match/rwl04/00543d40.c`), the RW core plugin offset, inside an atomic that librw built.
Register line: `re/orchestrator/UNCERTAIN_REGISTER.md` "RACE-WRITER-0067EA74 (RW core on a librw atomic)".

This is the RW layer gap of RESULT_PORT-OT-RACE.md 7.5 item 1 (L-124): matched RW core bodies on objects from the librw
bindings. It is outside this row: 0x004b5190 and 0x00543d40 are existing RW core reuses, and the RW core frame list and
RwGlobals belong to RACE-RWGLOBALS-FRAMELIST. Work-arounds such as skipping 0x00426460, stubbing 0x004b5190 or adding a
librw shim (forbidden by L-124) were not done. The next callee on this path, 0x004e6e00 (`reuse RwCore_004e6e00`, RpClumpDestroy),
is an RW core body too and gets the same librw clump.

## 5. Next

1. RW layer for the race load (RACE-RWGLOBALS-FRAMELIST, or a new row): `RpClumpStreamRead` on ORIG_TOP must produce an RW
   core clump, or 0x00426460 / 0x004b5190 / 0x00543d40 / 0x004e6e00 stay unreachable. With that fixed, the exit arm should get
   back to the 0x0047B860 stop through the Lua-less branch (`clump != 0`), and the stops of the else branch are skipped.
2. The writers of `0x0067e9f8` (ref 1 from tick 17) and `0x0067e848` (ref 2 at tick 299) on the original. Same W1 recipe:
   `writer_arms.py w2 -- --watch 0x0067e9f8,0x0067e848 --watch-from install --watch-stack 16`.
3. The load length stays unmeasured until a knob-on arm reaches the race anchor.

## Evidence

Committed (`verify/ro_20261116/`): this RESULT, `PREREG_RACE-WRITER-0067EA74.md`, `writer_arms.py`,
`racewriter_log_extract.txt` (the fault lines and W1's `watch.jsonl`), `raw_sha256_RACE-WRITER-0067EA74.txt` (33 raw files),
the register edit (old line settled, one new line).

Raw (not committed): `D:/MashedWT/RACE-WRITER-0067EA74_data` (`w1/`, `w1.log`, `knob_rl_on.log`, three arm dirs + logs,
`fd_ref1_<arm>.out`), `D:/MashedWT/RACE-WRITER-0067EA74_build_vs.log`, `_build_ddk.log`, `_ddkcheck.log`. Key SHA-256:
`w1/watch.jsonl` `29f419ea147435d0f592af29a417271f82c2c7114af082c8e4f79d6da96f028a`, `w1/meta.json`
`1ef92722ac67c20f5ecde2b3d7e30923bcca43617884f01d5e77c95488f372b1`, `w1/ckpt204.data.bin`
`45829a85390bae203b5352b60f6d3efc5b04f1ec7f5f04511088796e7fe57948`. All the others are in `raw_sha256_RACE-WRITER-0067EA74.txt`.

Commands:
- W1: `test-slot bash -c 'py -3.12 verify/ro_20261116/writer_arms.py w1 -- --watch 0x0067ea74 --watch-from install --watch-stack 16'`
- Builds: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, the same with `set MASHED_DDKOBJ=1&&`; `py -3.12 re/tools/ddkobj/ddkobj.py check`.
- Knob boot: `PORTTOP_OUT=D:/MashedWT/RACE-WRITER-0067EA74_data test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label RACE-WRITER-0067EA74 -- py -3.12 verify/porttop_20261108/knob_boot.py knob_rl_on D:/MashedWT/RACE-WRITER-0067EA74_exeroot 300 MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000'`
- Arms: `A4_OUT=D:/MashedWT/RACE-WRITER-0067EA74_data A4_ROOT=D:/MashedWT/RACE-WRITER-0067EA74_exeroot A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label RACE-WRITER-0067EA74 -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROIMG-pr1 RO_ROLOAD-pr1 RO_ROLOADC-pr1'`
- First divergence: `py -3.12 re/frida/detorig_firstdiv.py D:/MashedWT/RO-REF-NAME_data/ref1 <arm dir> --masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv --ignore 0x00614400-0x00614800,0x00771800-0x00771c00`
- Words: `py -3.12 verify/ro_20261116/portotrace_words.py <ref ckpt> <exe ckpt> <every dword of 0x0067e800..0x0067ebff>` at ticks 1 / 17 / 18 / 299.
