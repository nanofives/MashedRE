# RESULT D4-WSB-PORT-BROAD-2: GREEN, 15/18 shadow CLEAN, 14 rows C2->C3, 4 rows held at C2 (1 caller gate, 3 allocating DIVERGENT)

PREREG `PREREG_D4-WSB-PORT-BROAD-2.md` committed alone at `039b29bf4`, amendment 1 at `4e1812a6e` (with its one-file code
fix), amendment 2 at `a4aed0408` (reboot recovery, harness order only), each before the arms it governs. Branch
`orch/D4-WSB-PORT-BROAD-2` off `orch/integration` @ `0e3d71a75`. Default gates equal `merge_train.json` on the final exe.

## Per-row outcome

| row | before | after | shadow A/B (census scenario, hook alone) | heap pages o/n per sample |
|---|---|---|---|---|
| `0x0055b060` | C2 | **C3** | CLEAN n=24, `uninstalled=8b` (pass 2, `.asi a7275505`; pass 1 NO_LOG launch failure, superseded) | 2/2 x24 |
| `0x0055b080` | C2 | **C3** | CLEAN n=24, `uninstalled=83` (amendment 1 build `.asi a7275505`; pass 1 DIVERGENT 4/24, superseded) | 7/7 x24 |
| `0x0055b4a0` | C2 | **C3** | CLEAN n=24, `uninstalled=8b` | 2/2 x24 |
| `0x0055b650` | C2 | **C3** | CLEAN n=24, `uninstalled=83` | 1/1 x24 |
| `0x0055b860` | C2 | **C3** | CLEAN n=24, `uninstalled=83` | 1/1 x24 |
| `0x0055b940` | C2 | **C3** | CLEAN n=24, `uninstalled=83` | 1/1 x24 |
| `0x0055bab0` | C2 | **C3** | CLEAN n=14, `uninstalled=d9` | 1/1 x14 |
| `0x0055bac0` | C2 | **C3** | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x0055bbf0` | C2 | **C3** | CLEAN n=24, `uninstalled=83` | 0/0 x24 |
| `0x0055c380` | C2 | **C3** | CLEAN n=24, `uninstalled=8b` | 1/1 x24 |
| `0x0055c490` | C2 | **C3** | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x0055c4a0` | C2 | **C3** | CLEAN n=24, `uninstalled=53` | 1/1 x24 |
| `0x0055c540` | C2 | **C3** | CLEAN n=24, `uninstalled=56` | 1/1 x24 |
| `0x0055cd40` | C2 | C2 (caller gate) | CLEAN n=24, `uninstalled=56` | 0/0 x24 |
| `0x0055dc70` | C2 | C2 (DIVERGENT, allocates) | DIVERGENT 1/1, `uninstalled=a1`, return differs | 30/30 |
| `0x0055dca0` | C2 | C2 (DIVERGENT, allocates) | DIVERGENT 1/1, `uninstalled=8b` | 31/31 |
| `0x0055ddd0` | C2 | **C3** | CLEAN n=1, `uninstalled=8b` (pass 2, `.asi a7275505`, amendment 2) | 2/2 |
| `0x0055ded0` | C2 | C2 (DIVERGENT, allocates) | DIVERGENT 23/24, `uninstalled=53` (pass 2, `.asi a7275505`, amendment 2) | 4/4 x23, 5/5 x1 |

Every CLEAN row: ndiff 0 on every sample, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`. Verdicts from
`re/tools/shadow_ab_report.py` (`shadow_summary.tsv`), table from `readout.py`. Heap pages counted from the row's own
sample lines only.

**Caller gate (pre-registered).** `0x0055cd40`: its only reference is `DATA 0x005e4ff4` (a table slot), listed
`data_orphan` in `re/analysis/pointer_callers_2026-10-11.tsv`. It stays C2 with a dated note. Its port and CLEAN shadow
result stand. Next step for it: an identified caller of the `0x005e4ff4` slot.

**DIVERGENT rows: the differing dwords are allocator results.** All three bodies call the `RwGlobals` (`DAT_007d3ff8`)
slot `+0x108` with `(size, 0x30900)` (`0x0055dca0` also slot `+0x114`), so the original run and the port run each take a
fresh heap block. `Core/ShadowTrack.h:37` records this limit ("Allocation-heavy functions produce different heap pages on
the two runs by construction"). Read-out of the offsets (`D:/MashedWT/D4-WSB-PORT-BROAD-2_data/<rva>/shadow_ab.log`):

- `0x0055ded0`: on each of the 3 printed samples the one differing dword is `param_1+0x10` (return `0x0f363128`, diff at
  `0x0f363138`; `0x0f362d28` / `0x0f362d38`; `0x0f3630a8` / `0x0f3630b8`), the field the body fills with the slot `+0x108`
  result (`L2_0055ded0.cpp`, `*(param_1+0x10) = slot(param_2*0xc, 0x30900)`). Values are heap addresses
  (`0x0f35d6d8` original vs `0x0f35d648` port on sample 0).
- `0x0055dc70`: the return differs (`ret=1`). The return is the block from slot `+0x108` (`0x20` bytes) passed through
  `0x0055dca0`, which returns its `param_1`. One more dword differs at heap page `0x0ffcb000+0x7b8`
  (pre `0x000d0016`, original `0x00160014`, port `0x001c0014`), outside the returned block.
- `0x0055dca0`: 19 dwords differ, among them `param_1+0x8` (`0x0e6c52b0` vs `0x0e6c4fe0`) and `param_1+0x14`
  (`0x0e6c5100` vs `0x0e6c5250`), heap addresses written from the allocation calls.

These rows stay C2 (PREREG read-out: DIVERGENT stays, no re-fix in this step). The shadow lane cannot compare an
allocating body bit for bit. Next step: an allocator-aware compare (slot `+0x108` / `+0x114` results paired between the two
runs and masked), which is a harness item, not a port fix. U-9337 / U-9531 / U-9534 stay open with them.

**No C4.** No row has a race diff with the hook installed (`re/CONFIDENCE.md` write-set leg, L-38/L-40/L-54).

## Amendment 1: `0x0055b080`

Pass 1 (`.asi 23d17c78`): DIVERGENT 4/24 (samples 16-19), return equal, 1 page differs, 1-2 dwords at `+0x770/+0x774` and
`+0x780/+0x784` of one heap page, each a 1-ulp difference (e.g. `bbfc4cb0` vs `bbfc4caf`), at a 16-byte stride. Fix
`4e1812a6e`: the `0x00546b10` call in `L2_0055b080.cpp` goes through a raw-RVA thunk to the original instead of the port in
`Collision/RwpSolverMath2.cpp`. Nothing else changed. Re-run on `.asi a7275505`: CLEAN 24/24.

**Finding, not fixed here:** with the `0x00546b10` port bound, 4/24 calls of `0x0055b080` in the census scenario produce
1-ulp differences in the four floats that `0x00546b10` writes per table entry. `0x00546b10` is C3 in `hooks.csv`. The
pass-1 row is kept in `shadow_summary_pass1.tsv` as the measurement (raw folder
`D:/MashedWT/D4-WSB-PORT-BROAD-2_data/0055b080_pass1_superseded/`).

## Amendment 2: reboot recovery

Row 17 `0x0055ddd0` of pass 1 never launched (audio endpoints dropped while it queued, `post endpoints: <none>`
17:14:42Z, BLOCKED, no shadow log). The first pass-2 launch was BLOCKED (no endpoint, 17:23:20Z), then LIVING-PC
rebooted (14:23 local). No arm was cut mid-capture, and no partial output is used. After the reboot, rows 17-18 ran on
`.asi a7275505` with the two re-runs (their TUs are unchanged between the two builds). In the recovery run the machine
lock's helper raised `PermissionError` in `solo_owner` (`mashed_lock.py:232`, lock-file race) on the `0x0055b080` window,
so the driver moved on without launching. `0x0055b080` was then run alone (`run_pass4.sh`). That is a launch failure, not
an arm outcome.

## What was built

- 18 Lane-2 TUs `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp` (`e498a8356`: 16 from `decomp2port.py` with hand fixes, 2
  hand-written), amendment 1 `4e1812a6e`. Registered `L2_FUN_<rva>`, `.asi` only, opt-in (`MASHED_HOOK_ONLY` /
  `MASHED_HOOK_LANE2=1`). A default `.asi` run and `mashed_re.exe` do not install them. No exe source changed. Hand fixes
  are listed in the PREREG and cited in each TU.
- Final build (`MASHED_NO_DEPLOY=1`, at `4e1812a6e`): `mashed_re.exe`
  `285749aa80bbc25f32a6c06d5056cf6e4c0a34806aa25eaa6cb181bd32bb4d61`, `mashed_re_dev.asi`
  `a7275505f8bb01de2a9543f02f8c04ff6476ddb2dc8bd472efd1dd51b89f76d5` (private root only, `original/mashed_re_dev.asi`
  not touched). rva-lint 0 NEW.

## Default gates on this build

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + solo machine lock,
`A4_ROOT=D:/MashedWT/D4-WSB-PORT-BROAD-2_root`, exe `285749aa`. All six equal `re/orchestrator/merge_train.json`:

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `8284be46` | `8284be46` |
| H_DEF | gates.csv | `ba407a16` | `ba407a16` |
| H_DEF | ptrace.log | `31fe043e` | `31fe043e` |
| B_DEF_K0 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K1 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K26 | step.csv | `1d5bf4f9` | `1d5bf4f9` |

The first build's exe (`b53c0ed2`, same exe sources) gave the same six values (`D:/MashedWT/D4-WSB-PORT-BROAD-2_data/gates/`).

## Run log

- Audio endpoint checked before (inside the solo window) and after every shadow launch. Pass 1 rows 1-16: VB-CABLE present,
  0 crash events. After the reboot: VB-CABLE endpoints plus `Philips FTV` present before and after all 4 launches, 0
  MASHED Event-1000 events, no fault at `0x005baf48`, no U-9677 signature, no re-run for a crash.
- The machine lock was shared with D4-WSB-PORT-NARROW-TAIL and the merge train. Rows queued for their windows.

## Tracker transaction (re-classify)

- `hooks.csv`: 14 rows C2 -> **C3 impl**, `file` = the port, `scenario` `shadow_tracked:track0-mode10-cars4`, `frida_diff`
  `shadow_ab:verify/d4wsb_portbroad2_20261101/shadow_summary.tsv`, dated note. 4 rows stay C2 (`mapped` -> `impl`, `file` =
  the port) with a dated note naming the failed gate.
- C3 gate per promoted row: port under `mashedmod/src/mashed_re/`, clean build, `RH_ScopedInstall`, runtime-toggleable,
  a caller at C2+, callees at C2+ or none (PREREG "C3 gate pre-check"; the `0x0055bbf0` slot call is runtime-data
  dispatch, CONFIDENCE.md indirect-dispatch clause).
- `UNCERTAINTIES.md`: U-9528 (`0x0055bbf0`, generator items around `call dword ptr [eax+0xc]` at `0x0055bc07`) resolved
  through `re/tools/uncertainty_resolve.py`: the target is read at run time from the object (`mov eax,[esi+0x5c]` at
  `0x0055bbfd`), the call is caller-cleans (`add esp,0xc` at `0x0055bc0d`, 3 pushes), which fixes the esp depth at every
  listed address, and the port built on this reading is shadow CLEAN 24/24. The other open U rows that name these RVAs
  (U-7402, U-7524, U-8084, U-8086, U-9710, U-7561) are filed on their callers or on `0x00546b10` and carry `Blocks: none`
  or are not on the promoted row. U-9337 / U-9531 / U-9534 stay open (their rows stay C2).
- `STUBS.md`: S-3704, S-3736, S-3737, S-3738, S-3740 name these RVAs as passthrough callees of other ports. Those callers
  still call the original RVA, so the rows stay open.
- `re/analysis/CHANGELOG.md`: one line per row below `<!-- ENTRIES -->`, plus the U-9528 line.

## Files

Committed: `PREREG_D4-WSB-PORT-BROAD-2.md`, `RESULT_D4-WSB-PORT-BROAD-2.md`, `shadow_summary.tsv`,
`shadow_summary_pass1.tsv`, `readout.py`, `disasm_lin.py` / `scan_eax_use.py` (copies from
`verify/d4wsbportbroad1_20261029`), `run_shadow9.py` (copy of `run_shadow8.py`, data path and label changed), `drive_pb2.py`
(copy of `drive_pb1.py`, runner name, data path and label changed).

Raw (not committed), `D:/MashedWT/D4-WSB-PORT-BROAD-2_data/`:

| file | SHA-256 |
|---|---|
| `decomp.json` | `5d1b21b667469958553f46e276ce4af1c1079844423ae8b99f390c92ba64b2ef` |
| `drive_pb2.log` | `dfb5b1a6b69c8f823b8328354ad575bb41dda01396a6e8ac86138e81aad211d1` |
| `build1.log` | `9150812a70a17eb7dfd980c584c456fb2ae8c67e4e875b2b25bfafeebc03b747` |
| `build2.log` | `a09ebe258a7735a850112a7fc61a45b355fbb47bcea45e327c37e68d39bda7c5` |
| `gates_final/RUNLOG.txt` | `d8e3678a9a5f39c2be044a79492ad2ef21db4d137b01802237e2503d450bea0a` |
| `gates_final/H_DEF.step.csv` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| `gates_final/H_DEF.gates.csv` | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| `gates_final/H_DEF.ptrace.log` | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `gates_final/B_DEF_K{0,1,26}.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `<rva>/shadow_ab.log` (18) | column `log_sha256` of `shadow_summary.tsv` |
