# RESULT POWERUPINIT-C (2026-11-10): GREEN, 7 of the 9 powerup type-table inits on matched bodies under MASHED_ORIG_TOP

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/POWERUPINIT-C`. It was cut from `orch/POWERUPINIT-B` `386cd1e6b`, then rebased onto `orch/integration`
`3f1efb22b` once POWERUPINIT-B, PORT-OT-TRIPOOL and REFILE-4B3FC0 had merged. Every measurement below is on the rebased
tree. The first pass, on the pre-rebase tree (exe `737cf5a6…`), gave the same verdicts. Its files are in
`D:/MashedWT/POWERUPINIT-C_data/pre_rebase/`. The BINDINGS updater chains after `update_bindings_tripool.py`.

## What changed

The forwarder `PortTop_PWI_Call0045bae0` (Match/PortTop_0045bae0.cpp) stores one pointer in each `DAT_005f9998[i]`
+0x14 slot that holds 0, then runs the matched 0x0045bae0. Seven of those pointers now run matched bodies, through
`PortTop_PWI_Init_<i>`, which logs the first clone the init wrote:

| entry | rva | before | now | TU |
|---|---|---|---|---|
| 0 | 0x00456760 GatlingGun | stand-in | `Orig_00456760` | new Match/PortTop_00456760.cpp = re/match/s09/00456760.c |
| 1 | 0x004548e0 DepthCharge | stand-in | stand-in (reason below) | Match/PortTop_0045bae0.cpp |
| 2 | 0x00455400 Missile | stand-in | stand-in: no byte match (94.0%), row POWERUPINIT-B2 | Match/PortTop_0045bae0.cpp |
| 3 | 0x004583a0 Mine | stand-in | `Orig_004583a0` | new Match/PortTop_004583a0.cpp = re/match/s29/004583a0.c |
| 4 | 0x0045a6b0 FlameThrower | stand-in | `Orig_0045a6b0` | new Match/PortTop_0045a6b0.cpp = re/match/s29/0045a6b0.c |
| 5 | 0x0045b830 shotgun | stand-in | `Orig_0045b830` | new Match/PortTop_0045b830.cpp = re/match/s29/0045b830.c |
| 6 | 0x00454e70 flashunit | stand-in | `Orig_00454e70` | new Match/PortTop_00454e70.cpp = re/match/s29/00454e70.c |
| 7 | 0x004576f0 oilslicker | stand-in | `Orig_004576f0`, its 0x0040cda0 a stand-in | new Match/PortTop_004576f0.cpp = re/match/s29/004576f0.c |
| 8 | 0x00452f70 mortar | stand-in | `Orig_00452f70` | new Match/PortTop_00452f70.cpp = re/match/s29/00452f70.c |

Callees, all bound by name, with no raw-address call:

| rva | exe symbol | kind | TU |
|---|---|---|---|
| 0x00454e40 (tail of 0x00454e70) | `Orig_00454e40`, statics 0x00454bd0 / 0x00454c00 in the source | matched | new Match/PortTop_00454e40.cpp = re/match/s29/00454e40.c |
| 0x0045b810 (tail of 0x0045b830) | `Orig_0045b810`, static 0x0045b360 | matched | new Match/PortTop_0045b810.cpp = re/match/s29/0045b810.c |
| 0x00458360 (from 0x004583a0) | `Orig_00458360`, statics 0x00458020 / 0x00457ba0 | matched | new Match/PortTop_00458360.cpp = re/match/s09/00458360.c |
| 0x004b3fc0 | `Orig_004b3fc0` | matched | new Match/PortTop_004b3fc0.cpp = re/match/s29/004b3fc0.c |
| 0x004b3f20 (its callback) | `PortTop_004b3fc0_Cb004b3f20` | port (exe-only copy) | same TU, written from the disassembly 0x004b3f20..0x004b3f41: no Ghidra function, no row, no source |
| 0x004b5240 | `Orig_004b5240` | matched | new Match/PortTop_004b5240.cpp = re/match/s18/004b5240.c |
| 0x0045a3a0 | `PortTop_0045a3a0_Copy` | port (exe-only copy) | new Match/PortTop_0045a3a0.cpp = re/match/s09/0045a3a0.c at 81.0% (register-allocation residual only) |
| 0x0040cda0 | `PortTop_PWI_StandIn_0040cda0` | stand-in | Match/PortTop_004576f0.cpp |
| 0x004e6ab0 RpClumpClone | `MashedRw_ClumpClone_004e6ab0` | bound | LibRw/RwApiBind.cpp: rw::Clump::clone |
| 0x004e4800 | `MashedRw_ClumpGetWorld_004e4800` | bound | rw::Clump::world |
| 0x004b5190 | `MashedRw_AtomicPartCode_004b5190` | bound | rw::UserDataArray on the geometry, -1 when absent (never logged in any run) |
| 0x004b52f0 | `MashedRw_AtomicGeometryFlags_004b52f0` | bound | the 0x004b5300 work on a->geometry->flags |
| 0x004c13e0 RwFrameScale | `MashedRw_FrameScale_004c13e0` | bound | rw::Frame::scale (RW and librw number the combine ops alike) |
| 0x004c0ed0 RwFrameGetLTM | `MashedRw_FrameGetLTM_004c0ed0` | bound | rw::Frame::getLTM (0x40-byte RwMatrix layout, static_assert) |

The rest are already exe symbols: 0x0040bb30 Search40bb30, 0x004770c0 Orig_004770c0, 0x00476cb0 Batch476cb0, 0x00476c10
Batch476c10 (Render/RfxEmitter.cpp, writes the PTank block of the 0x004770c0 record), 0x0042a5d0 FUN_0042a5d0, 0x004c5c80 /
0x004e6e00 / 0x004e45b0 / 0x004e66d0 bindings, 0x004b6520 / 0x00474d60 / 0x004892c0 / 0x004893a0 Orig_.

0x00458360's source also holds 0x00458080 (static) and 0x004582f0, which nothing in the exe calls. Their 12 callees bind
to logging traps `PortTop_PWC_NotRun_<rva>`, and no run reached one.

### Why every atomic, frame and clump is librw

Each init loads through the matched 0x0042a5d0 (a librw rw::Clump from the piz mount stack), clones it (rw::Clump::clone)
and collects the clone's atomics through 0x004b3fc0. The callback receives librw rw::Atomic pointers in RW order from
the ForAllAtomics binding. So the bindings above take librw objects only. The matched leaves that touch the atomic
directly read offsets that librw shares with RW:
- atomic +0x02 is rw::Object::flags, bit 0x04 = rw::Atomic::RENDER (0x004b5240).
- atomic +0x04 is rw::Object::parent, the frame (`atom->frame` in 0x0045a6b0 / 0x0045b830).

### The builders of what the inits read (the 0x004768c0 precedent)

- **Records written by the inits themselves:** clone slots, atom slots, the 0x004770c0 batch records (0x006887d0,
  0x0068a2c0, 0x0068a210, 0x0068b090, 0x0088fec0) and the 0x004892c0 pools. 0x00476cb0 / 0x00476c10 / 0x004893a0 run on
  them in the same init, right after.
- **0x00458360:** reads only the dropper / mine records 0x004583a0 fills just before it.
- **The two tail targets:** read the records their own init filled.
- **`[0x007dc57c]`** (the PTank slot Batch476c10 writes through) = 0x70, set by the 0x00493640 shim.
- **Original .data / .rdata, which reads 0 in the exe image pad:** three addresses, added to the port-state seed
  (verify/portstate_20261108/SEED_CANDIDATES.tsv, `gen_portstate_seed.py`: 262 entries, 3180 bytes, fnv 0x1b44a1c6,
  `--check` OK):
  - 0x005f9820, 16 B: {0, 0, 0.5, 1}, the colour 0x004583a0 passes to 0x00476c10.
  - 0x005ce4bc, 15 B: "GatlingGun.dff".
  - 0x005ce4cc, 6 B: "flash".
  The s09 source of 0x00456760 reads its two strings as MGLOBALA arrays. The first build ran without the seed and
  logged `0x004e6ab0(clump 00000000)` four times for entry 0: the empty name loaded nothing.
- **Not built, so stood in:** 0x0040cda0 runs only when DAT_00636ad0 == 0 (0 on this path). It then reads the camera
  at DAT_006905b0 +0x04 through 0x004672d0 and calls RpClumpForAllAtomics(clump, 0x0040bb10, args), and 0x0040bb10 has
  no matched source. It logs 4 calls per boot.

### Entry 1 (0x004548e0) stays a stand-in

It passes `0x004b3f90(DepthChargeRound clump)` to 0x004e69a0 (RpAtomicClone). In the exe that value is the RW-layout
view MashedRwAtomicView, not a librw atomic. The clones then go to 0x004e7e30 (whose exe body stores the frame in a PTank
record), 0x004c1040 (RW frame-hierarchy pointer arithmetic) and the 0x004548a0 tree (0x004547c0, 0x00454170 with
0x004c1210 / 0x004c15c0 / 0x004e43b0 / 0x004e4440). None of these has a librw binding for this object. Proposed row
POWERUPINIT-D1: a librw atomic clone / set-frame / add-child binding set that recognises the view.

### Who reads these records afterwards

Nothing new on this path. 0x0045b930 calls the +0x18 slots (`PTR_FUN_005f99b0`), which stay 0, and 0x0045bba0 (the
per-type update) is uncalled. The clones are never added to a world and never destroyed. That is the same lifetime as
the 0x00459290 lazer clones in POWERUPINIT-A.

## rva-lint

- One new ORIGTOP-ONLY pair: CROSS-TARGET 0x004b5240 (exe-only Match/PortTop_004b5240.cpp vs asi-only
  Util/PromoLoop_sessionB.cpp), allowlisted with the tag. All 13 new TUs carry `// RVA-LINT: ORIGTOP-ONLY`.
- The first lint run also flagged DUP-IN-TARGET 0x00457ba0. It was a false call-through hit from a comment line in
  RwApiBind.cpp that started `// 0x00457ba0 ...` above the 0x004e4800 binding; the comment was reworded.
- Final: `35 known, NEW=0`, `ORIGTOP-ONLY=33 (46 marked TU(s))`, OK.

## Acceptance

Machine rules:
- **Builds:** each build waited until no `%TEMP%\mashed_machine_priolinger_*.lock` was held:true with a live pid and no
  SOLO lock was held. The merge train held both for about 40 min.
- **Launches:** every launch was `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label
  POWERUPINIT-C[-gates] -- ...'`.
- **Roots:** private roots `D:/MashedWT/POWERUPINIT-C_root` (boots, captures) and `_rootA` (gates), `original` = a junction
  to `D:/MashedWT/PORT-TOP-04J_rootK/original`.

| check | result |
|---|---|
| syntax | `cl /Zs /EHa /W3 /O2 /std:c++17 /DMASHED_STANDALONE` on the 13 new and 2 edited TUs: no error |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed`. mashed_re.exe sha256 `f44a08dfb68d429d703c7a364e9e04781a76ed6ce981d1b967c5968c0297d400` |
| knob-ON boot, RWMOD off (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `exited rc=0 (0x00000000) after 47.6 s`, :1206 `WinMain 0x00492370 returned 0 after 3000 frames`. :375 entry 0 first clone `108D1658`, :419 entry 3 `108D12D8`, :431 entry 4 `108D1FF8`, :454 entry 5 `108D1C40`, :462 entry 6 `108D1930`, :476 entry 7 `108D19D8`, :551 entry 8 `108D22D0`. :376-377 entries 1, 2 stand-ins. Four 0x0040cda0 stand-in calls, 0 trap lines, 0 part-code -1 lines. A's lines unchanged: :785 0x00412010 matched (6, 6), :793 0x00458880 matched (0, 0), :828 0x004593b0 stand-in (RWMOD off) |
| knob-ON boot, RWMOD on (+ `MASHED_ORIG_TOP_RWMOD=1`) | `exited rc=0 (0x00000000) after 41.2 s`, :1220 `returned 0 after 3000 frames`. :380-556 the same seven matched entries, :833 `0x004593b0: matched body (RWMOD on)` |
| 12 captures, RWMOD off (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. Both ON logs: 7 `matched body returned`, `WinMain returned 0 after 578 frames` |
| 12 captures, RWMOD on (arm `nav_rwmod`, `MASHED_ORIG_TOP_RWMOD=1`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`, both ON logs 7 matched |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows. No capture moved (eqE=12 everywhere). runs.tsv is byte-identical to POWERUPINIT-A's (sha256 f560646e…) |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint | OK, 35 known, 0 new |

**Default path.** Every new body runs only on MASHED_ORIG_TOP: the new TUs are reached only from the 0x0045bae0 forwarder,
and the bindings only from them. The seed is default ON for every run (L-101), and it adds three addresses: 0x005f9820,
0x005ce4bc and 0x005ce4cc. The default path has no reader of them: the only code citing them is the new ORIGTOP-ONLY
TUs. The unchanged G-CTRL and B gates are consistent with that.

## BINDINGS and the port column

`verify/portorigtop_20261109/update_bindings_powerupinit_c.py` first runs `update_bindings_tripool.py` (POWERUPINIT-A, then
PORT-OT-TRIPOOL), which reproduces the integration file's 481 rows. Then it writes this row's 23 rows:
- The seven inits: stand-in -> matched.
- 0x004548e0 / 0x00455400: stand-in, with the new reasons.
- Matched: 0x00454e40, 0x0045b810, 0x00458360, 0x004b3fc0, 0x004b5240.
- Port: 0x004b3f20, 0x0045a3a0.
- Stand-in: 0x0040cda0.
- Bound: 0x004e6ab0, 0x004e4800, 0x004b5190, 0x004b52f0, 0x004c13e0, 0x004c0ed0.

Result: 481 -> 494 rows; matched 224 -> 236, stand-in 70 -> 64, port 45 -> 47, bound 43 -> 49, rw 30 -> 29.

`backfill_exe_file.py`: 16 rows given an exe_file, none with more than one TU. Then `backfill_port.py` on this build:
`none=5099 body=553 called=515`, 18 rows changed.
- none / empty -> called: 0x00452f70, 0x00454e40, 0x00454e70, 0x00456760, 0x004576f0, 0x00457ba0, 0x00458020, 0x00458360,
  0x004583a0, 0x0045a3a0, 0x0045a6b0, 0x0045b810, 0x0045b830, 0x004b3fc0, 0x004b5240.
- none -> body: 0x00458080 (compiled, not called).
- empty -> none: 0x00455400, 0x004b5560 (rows added by POWERUPINIT-B).

The statics 0x00454bd0 / 0x00454c00 / 0x0045b360 keep their port value. Their copies in the s29 sources carry no
`// MATCH:` line, because their MATCH lines live in s09.

## Raw files (D:/MashedWT/POWERUPINIT-C_data, rebased tree, not committed)

| file | sha256 |
|---|---|
| build.log | e21cec1f0a605b577757f1f618b2393ccc72b9f1072a0ff965bf18226f591198 |
| mashed_re.exe (copy of the measured build) | f44a08dfb68d429d703c7a364e9e04781a76ed6ce981d1b967c5968c0297d400 |
| knob_on_1.log / .stdout | be4a021f855e9a196e5d206a1875ecb48e8ce5697b976b8c44fa7922490cf861 / 847766db37c42f37d675edbc3784f5d48a0172e6315db4fd8277e451934248a7 |
| knob_on_rwmod.log / .stdout | bd3e0dbbed0eec3362381717471458e21e3ae86afccef0370f38aae7f420d1bd / f7ef3f6a14958fa7d6fc0cd307649ad8998c04bb6d1e25acb98370defdae81fb |
| runs.tsv | f560646e9825f27b38517ec0c4d851cd17931fc8944763b7fbaa1d1204796438 |
| capflake_nav1.stdout / capflake_rwmod.stdout | 939ebed395cd75fe44d28669a415491d557b2bc1df379fec29b60227e9da9ed2 / c4a88299db4e9353869d1c81a2bb0474bc16c4e4e3b541b1ff11409322ae2b7a |
| nav1/r01..r04 logs | 86528224… / c9e0fd28… / ff5e96d9… / 21cdd4b2… |
| nav_rwmod/r01..r04 logs | 479c2f2b… / a625a27d… / 1883683b… / 70bfdcbf… |
| arms.stdout | 4a7bdc573fb61217b9c65a5fc366ac723bd0bcc540b71b8e7f08f1e50687edf5 |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 8284be46… / 67931834… / 31fe043e… |
| arms/B_DEF_K0, K1, K26 .step.csv | 1d5bf4f9… (all three) |

## Next

- **POWERUPINIT-D1:** entry 1 (0x004548e0), librw bindings for RpAtomicClone / RpAtomicSetFrame / RwFrameAddChild on
  the 0x004b3f90 view's atom, plus the 0x004548a0 tree.
- **POWERUPINIT-B2:** entry 2 (0x00455400, 94.0%) and 0x0045a3a0 (81.0%). When 0x0045a3a0 matches, its BINDINGS row
  moves from port to matched.
- **0x0040bb10:** match the 0x0040cda0 callback, which retires the entry 7 stand-in (it also needs the 0x004672d0 camera).
