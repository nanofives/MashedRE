# RESULT POWERUPINIT-C (2026-11-10): GREEN, 7 of the 9 powerup type-table inits on matched bodies under MASHED_ORIG_TOP

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/POWERUPINIT-C`. It was cut from `orch/POWERUPINIT-B` `386cd1e6b`, then rebased onto `orch/integration`
`3f1efb22b` once POWERUPINIT-B, PORT-OT-TRIPOOL and REFILE-4B3FC0 had merged. Every measurement below is on the rebased
tree. This tree also carries the lead-review fixes (0x004b3f20 matched, 0x0045a3a0 a stand-in). Two earlier passes gave the
same verdicts, and their files are kept:
- the pre-rebase tree (exe `737cf5a6…`), in `D:/MashedWT/POWERUPINIT-C_data/pre_rebase/`
- the rebased tree before the fixes (exe `f44a08df…`), in `rebase1/` The BINDINGS updater chains after `update_bindings_tripool.py`.

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
| 0x004b3f20 (its callback) | `Orig_004b3f20` | matched | new Match/PortTop_004b3f20.cpp = re/match/s29/004b3f20.c (this row: BYTE-IDENTICAL 34 B, relocheck OK, C4 by match) |
| 0x004b5240 | `Orig_004b5240` | matched | new Match/PortTop_004b5240.cpp = re/match/s18/004b5240.c |
| 0x0045a3a0 | `PortTop_PWI_StandIn_0045a3a0` | stand-in | Match/PortTop_0045a6b0.cpp (section "0x0045a3a0 and L-115" below) |
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

### 0x004b3f20 matched (lead review, L0 correction)

The first commit ran an exe-only copy of the callback written from the disassembly. L0 does not allow a hand body on
the knob path, so the callback was byte-matched instead:
- **Source:** `re/match/s29/004b3f20.c`. Both context words are loaded into locals before the branch.
- **match.py --bytes:** 100.0%, 15/15 instructions, 34 B BYTE-IDENTICAL.
- **relocheck:** OK 0/0.
- **pc_funcs.csv:** already held the 34 B extent (0x004b3f20..0x004b3f41).
- **hooks.csv:** row added at C1 through re-classify (`render`, as its caller 0x004b3fc0 after REFILE-4B3FC0), then
  `c4_reclass.py batch --apply --strict --result` this file: 1 pass, C1 -> C4.
- **Exe:** Match/PortTop_004b3f20.cpp compiles it as `Orig_004b3f20`, and 0x004b3fc0 binds its callback to it.

### 0x0045a3a0 and L-115

0x0045a3a0 runs on this path: the matched 0x0045a6b0 calls it once per flame record. Both boots log 4 calls, the first
on record 0x0068BC98 (knob_on_1.log:423).

1. **Byte match:** not reached. Best 81.0% (re/match/s09/0045a3a0.c, regs 90.5%). POWERUPINIT-B tried 9 forms this
   week, listed in its RESULT, and the s09 header lists the earlier ones.
2. **L-115 proof of the residual (a new class, register choice; for L0 / Mariano, NOT applied).** The whole diff of
   match.py --show, ours on the left, the original with addresses on the right:
   ```
      !                                       | 0x0045a3a1 push ebp
      !                                       | 0x0045a3a2 mov ebp, dword ptr [esp + 0xc]
      ! lea ebx, [ebx]                        | 0x0045a3ad nop        (loop-head padding, both no-ops)
      ! mov eax, dword ptr [esp + 0x10]       |
      ! mov dword ptr [eax], ecx              | 0x0045a400 mov dword ptr [ebp], ecx
      ! mov dword ptr [eax + 0x14], ecx       | 0x0045a403 mov dword ptr [ebp + 0x14], ecx
      ! mov dword ptr [eax + 0x18], ecx       | 0x0045a406 mov dword ptr [ebp + 0x18], ecx
      ! mov dword ptr [eax + 0xc], 0x3f800000 | 0x0045a409 mov dword ptr [ebp + 0xc], 0x3f800000
      !                                       | 0x0045a410 pop ebp
   ```
   - **Same pointer.** Both read the same stack word. The original reads param_1 at entry: [esp+0xc] after push ebx and
     push ebp is entry+4. Ours reads it after the loop: [esp+0x10] after push ebx, esi, edi is entry+4. In
     0x0045a3a6..0x0045a3fe the original writes EBP nowhere, and no instruction in the body writes the stack: the loop's
     only stores go through EAX, derived from the constant 0x0068d76c (the global table). So the slot still holds
     param_1 when ours reads it.
   - **Same stores.** Same four, in the same order, with the same values (ECX = 0 from `xor ecx, ecx`, and the
     immediate 0x3f800000).
   - **Registers on return.** EBP is saved and restored by the original and untouched by ours, so it is equal on return.
     EAX is not a return value (void) and is caller-saved scratch in cdecl.
   - **Not observable.** The only other difference is the 4-byte saved-EBP push below ESP, which no caller can see.

   This class (register choice for a reloaded argument) is not one L-115 admits today (only SIB base/index order is).
   So the exe runs the stand-in, and the proof goes to L0.
3. **Stand-in:** `PortTop_PWI_StandIn_0045a3a0` in Match/PortTop_0045a6b0.cpp logs and returns. What it leaves undone:
   - Zeroing fields 0..6, 8, 11 and 12 of the 4 x 5 x 5 0x34-byte table at 0x0068bd00..0x0068d150. The pad starts
     zero-filled, and no exe code writes that range.
   - Record +0x00 / +0x14 / +0x18 = 0, which is already the pad's 0.
   - Record +0x0c = 1.0f, which stays 0. The only reader on this path would be the per-type update 0x0045bba0, which is
     uncalled.
   Match/PortTop_0045a3a0.cpp (the 81% copy) is deleted and out of exe_sources.rsp.

## rva-lint

- One new ORIGTOP-ONLY pair: CROSS-TARGET 0x004b5240 (exe-only Match/PortTop_004b5240.cpp vs asi-only
  Util/PromoLoop_sessionB.cpp), allowlisted with the tag. All 13 new TUs (Match/PortTop_004b3f20.cpp in, the 0x0045a3a0 copy out) carry `// RVA-LINT: ORIGTOP-ONLY`.
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
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed`. mashed_re.exe sha256 `70c97e79be3132760c5ad80fd1c53afd1ee15419482fd6e7bfd5c976ec0c2056` |
| knob-ON boot, RWMOD off (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `exited rc=0 (0x00000000) after 51.9 s`, :1210 `WinMain 0x00492370 returned 0 after 3000 frames`. Seven `matched body returned` lines, :375 entry 0 through :555 entry 8. :376-377 entries 1, 2 stand-ins. Four 0x0040cda0 and four 0x0045a3a0 stand-in calls (first at :423), 0 trap lines, 0 part-code -1 lines. A's lines unchanged: :789 0x00412010 matched (6, 6), :797 0x00458880 matched (0, 0), :832 0x004593b0 stand-in (RWMOD off) |
| knob-ON boot, RWMOD on (+ `MASHED_ORIG_TOP_RWMOD=1`) | `exited rc=0 (0x00000000) after 51.7 s`, :1224 `returned 0 after 3000 frames`. :380-560 the same seven matched entries, :837 `0x004593b0: matched body (RWMOD on) (1, 0)`, the same counts as POWERUPINIT-A's run |
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
- Matched: 0x00454e40, 0x0045b810, 0x00458360, 0x004b3fc0, 0x004b3f20, 0x004b5240.
- Stand-in: 0x0040cda0, 0x0045a3a0.
- Bound: 0x004e6ab0, 0x004e4800, 0x004b5190, 0x004b52f0, 0x004c13e0, 0x004c0ed0.

Result: 481 -> 494 rows; matched 224 -> 237, stand-in 70 -> 65, port 45 -> 45, bound 43 -> 49, rw 30 -> 29. This row adds no port (hand-body) row.

`backfill_exe_file.py`: 16 rows given an exe_file, none with more than one TU. Then `backfill_port.py` on this build:
`none=5100 body=553 called=515`. 18 rows differ from orch/integration.
- none / empty -> called: 0x00452f70, 0x00454e40, 0x00454e70, 0x00456760, 0x004576f0, 0x00457ba0, 0x00458020, 0x00458360,
  0x004583a0, 0x0045a6b0, 0x0045b810, 0x0045b830, 0x004b3fc0, 0x004b3f20 (new row), 0x004b5240.
- 0x0045a3a0 stays none (stand-in).
- none -> body: 0x00458080 (compiled, not called).
- empty -> none: 0x00455400, 0x004b5560 (rows added by POWERUPINIT-B).

The statics 0x00454bd0 / 0x00454c00 / 0x0045b360 keep their port value. Their copies in the s29 sources carry no
`// MATCH:` line, because their MATCH lines live in s09.

## Raw files (D:/MashedWT/POWERUPINIT-C_data, final tree, not committed)

| file | sha256 |
|---|---|
| build.log | 2dae2c521918129d… |
| mashed_re.exe (copy of the measured build) | 70c97e79be3132760c5ad80fd1c53afd1ee15419482fd6e7bfd5c976ec0c2056 |
| knob_on_1.log / .stdout | 3bd6213ec34dbf1a… / 5b760406e667df9f… |
| knob_on_rwmod.log / .stdout | 2f3db6a69a242e3a… / e3386eb0c8916556… |
| runs.tsv | f560646e9825f27b38517ec0c4d851cd17931fc8944763b7fbaa1d1204796438 |
| capflake_nav1.stdout / capflake_rwmod.stdout | 7b8505af404f4661… / 3b599ee0dc17cb96… |
| nav1/r01..r04 logs | 106cde62… / 37f226a0… / 47c6ad87… / d90076ee… |
| nav_rwmod/r01..r04 logs | 0cc13b95… / b0980c19… / cac44f3e… / a1f3e70d… |
| arms.stdout | c963046039b688ef… |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 8284be46… / 67931834… / 31fe043e… |
| arms/B_DEF_K0, K1, K26 .step.csv | 1d5bf4f9… (all three) |

## Next

- **POWERUPINIT-D1:** entry 1 (0x004548e0), librw bindings for RpAtomicClone / RpAtomicSetFrame / RwFrameAddChild on
  the 0x004b3f90 view's atom, plus the 0x004548a0 tree.
- **POWERUPINIT-B2:** entry 2 (0x00455400, 94.0%) and 0x0045a3a0 (81.0%). 0x0045a3a0 moves from stand-in to matched when it
  byte-matches, or earlier if L0 admits the register-choice class (proof above).
- **0x0040bb10:** match the 0x0040cda0 callback, which retires the entry 7 stand-in (it also needs the 0x004672d0 camera).
