# RESULT D4-WSB-PORT-NARROW-TAIL: GREEN, 9 rows C2->C3, 4 rows stay C2 (2 allocator, 1 caller gate, 1 held), gates equal refs

PREREG `PREREG_D4-WSB-PORT-NARROW-TAIL.md` committed alone at `17c6b91b9`, before arm S. Amendment 1 (reboot recovery and
launch-failure re-run rule) committed at `7573210e5` and `aa674cb6d`, each before the re-run it governs. Branch
`orch/D4-WSB-PORT-NARROW-TAIL` off `orch/integration` @ `0e3d71a75`. Code commit `997947a7b`.

## Recovery note (run 28 reboot)

LIVING-PC rebooted at 14:23 local (17:23Z) while arm S row 1 (`0x00574920`) was in its first window. That attempt took
no sample, and its partial transcript is kept as `00574920/shadow_transcript_reboot_cut.txt` (not evidence). Arm S was
re-run from row 1 for all 12 armed rows on the same build. The default gates had finished at 14:12-14:14 local, before
the reboot, every arm self-exited, on this step's exe, so they are complete runs and were not re-run.

## Per-row outcome

| row | before | after | shadow A/B (census scenario, hook alone) | heap pages o/n per sample |
|---|---|---|---|---|
| `0x00574920` | C2 | **C3** | CLEAN n=1, `uninstalled=55` (re-run after a launch failure, amendment 1) | 0/0 |
| `0x005749b0` | C2 | **C3** | CLEAN n=1, `uninstalled=55` | 0/0 |
| `0x00574a20` | C2 | **C3** | CLEAN n=1, `uninstalled=83` | 0/0 |
| `0x00574ac0` | C2 | **C3** | CLEAN n=1, `uninstalled=a1` | 0/0 |
| `0x0057c1b0` | C2 | **C3** | CLEAN n=24, `uninstalled=56` | 1/1 x24 |
| `0x0057c220` | C2 | **C3** | CLEAN n=24, `uninstalled=a1` | 3/3 x24 |
| `0x0057c270` | C2 | **C3** | CLEAN n=1, `uninstalled=68` (KV2 body, `.asi` wrapper) | 1/1 |
| `0x0057c2b0` | C2 | C2 (caller gate) | CLEAN n=24, `uninstalled=56` (KV2 body, `.asi` wrapper) | 1/1 x18, 2/2 x6 |
| `0x0057c300` | C2 | C2 (allocator) | DIVERGENT 24/24, `uninstalled=a1` | 6/6 x16, 5/6 x4, 6/5 x4 |
| `0x0057c420` | C2 | C2 (held, no arm) | not armed (PREREG "Held row") | n/a |
| `0x0057c440` | C2 | **C3** | CLEAN n=24, `uninstalled=83` (KV2 body, `.asi` wrapper) | 1/1 x24 |
| `0x0057c500` | C2 | C2 (allocator) | DIVERGENT 22/24 (CLEAN 2/24), `uninstalled=a1` (re-run after a launch failure, amendment 1) | 5/5 x10, 7/7 x4, others 3-13 |
| `0x0057c550` | C2 | **C3** | CLEAN n=24, `uninstalled=56` | 1/1 x24 |

Every CLEAN row: ndiff 0 on every sample, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL` for the row's own hook. Verdicts
from `re/tools/shadow_ab_report.py` (`shadow_summary.tsv`). Heap pages counted from the row's own sample lines. The
`0x0057c2b0` and `0x0057c440` logs also carry a `FUN_004c51a0 PATCHBYTE installed=8b ... SUSPECT-no-restore` line: that
is the nested `RwMatrixTranslate` port called from the port body's B run, not installed on that boot (MASHED_HOOK_ONLY),
and is not the row's own line. The race_load / menu rows sample 1 call per boot because the census scenario loads once.

**Caller / callee gate (C3).** Caller at C2+ per row: `0x00574a20` C2 for `00574920` / `005749b0`; `0x00567c40` C3 for
`00574a20` / `00574ac0`; `0x0057c500` C2 for `0057c1b0`; `0x004218b0` C3 for `0057c220`; `0x00493640` C2 for `0057c270`;
`0x0057c2b0` C2 for `0057c440`; `0x004218d0` C3 for `0057c550`. Callee half: `00574920`, `005749b0`, `00574ac0` are
leaves (cpuid / xorps probe / one global read), leaf exemption with the bit-identical shadow A/B; `00574a20` ->
`00574920` / `005749b0` / `0x004a3a90` C2; `0057c1b0` -> `0x0055c380` C2; `0057c220` -> `0x0055dff0` C4; `0057c270` ->
`0x004e7d40` `RpAtomicRegisterPlugin` C2; `0057c440` -> `0x004c51a0` C3, `0x004c45f0` C3; `0057c550` -> `0x0055b930` C2.

**No C4.** No row has a race diff with the hook installed. Rows with heap pages compared > 0 are listed above for a
later C4 leg.

## The 4 rows that stay C2

- `0x0057c2b0`, caller gate. Ghidra's caller set is empty (`decomp_pc.py --callers`). The one reference is the DATA
  store `mov dword ptr [0x624058], 0x57c2b0` at `0x0057c293` inside `FUN_0057c270`. The PREREG pre-check counted it on
  the strength of `re/CONFIDENCE.md` installer clause item 3 ("Ghidra's caller set already counts these"). The caller set
  read for this step does not hold it, and item 3 excludes `store_mem` rows from the installer route, so neither route
  is met. The row stays C2 with its CLEAN result recorded. `0x00624058` = `0x0062403c + 0x1c`, and `0x0062403c` is the
  table address `0x0057c300` stores into each body (`0x0057c31a`). Whether that store counts as an installer is a rubric
  question for the lead (see Next), not something this step decides.
- `0x0057c300` and `0x0057c500`, allocator class. Both call the RwGlobals slot `+0x108` allocator (`0x0057c30d`,
  `0x0057c50d`; `0x0057c500` also calls the `0x00564130` pool allocator at `0x0057c52c`). The A and B runs start from the
  same restored pre-state and the allocator hands back a different block on the second run, as `Core/ShadowTrack.h`
  LIMITS documents for allocation-heavy functions.
  - `0x0057c300`: 22 of 24 divergent samples have a return mismatch, and the one differing allocator dword (heap page
    offset `+0x150`) takes a different next index in A and B (sample 0: pre `0009001f`, A `0009001e`, B `0019001e`). The
    other 2 samples return the same block and differ in one dword of it, a heap address (sample 16: `rv+0x10`, A
    `0f4ebcf0`, B `0f4ebb70`).
  - `0x0057c500`: 18 of 22 divergent samples have a return mismatch (allocator dword at heap page offset `+0xd48`, same
    pattern). The other 4 differ in two dwords of the returned block, `rv+0x0` and `rv+0x4c`, both heap addresses
    (sample 12: A `0f50f540`, B `0f50f270`). 2 samples are CLEAN.
  - Same class as `0x00564130` / `0x00562520` in D4-WSB-PORT-BVH. I read both ports against the disassembly: same slot
    arguments (`0x34` / `0x54`, `0x30900`), same stores in the same order. A C3 needs an arm that pins the slot result
    for both runs (the D4-WSB-PORT-BVH proposal), not a code change.
- `0x0057c420`, held (PREREG): the existing void ports `FUN_0057c420` (`Collision/RwpVtableKV2.cpp`) and `FUN_0055bd80`
  (`Collision/RwpSolverGlue5.cpp`) drop the EAX that three callers read (`0x0047d213`, `0x0047d30e`, `0x00480263`, each
  `push eax` into `0x0055ab30`, tested at `0x0055ab69..0x0055ab6f`). Both install by default in the `.asi`. The fix
  (return the slot call's EAX through both) changes default `.asi` behaviour, so it is reported here, not made.

## Launch failures (amendment 1)

Two rows of the recovery pass took no sample: the transcript shows `scenario_launch pid=N` then `error: could not attach`
(`0x00574920` pid 17208, `0x0057c500` pid 1772). Each was re-run once, same build and env, as amendment 1 states: both
launched, `0x00574920` CLEAN, `0x0057c500` DIVERGENT (allocator class, above). The pass-1 transcripts are kept as
`*/shadow_transcript_pass1_launchfail.txt`.

## What was built

- 9 Lane-2 TUs `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp` and 3 `.asi`-only `ShadowAB::RunTracked` wrappers in
  `Collision/RwpVtableKV2.cpp`, commit `997947a7b` (details and every hand fix with its address in the PREREG).
  Registered `L2_FUN_<rva>` (opt-in); the 3 KV2 wrappers keep the `FUN_` names and call the same body with
  `MASHED_SHADOW_AB` unset. No exe source changed. rva-lint 0 NEW.
- Build `MASHED_NO_DEPLOY=1` at `997947a7b`: `mashed_re.exe`
  `26f6e7d6f06cc9bddacb34096f00ae948edc7c682de7748de029c2b375561a1a`, `mashed_re_dev.asi`
  `343d85914ede226a931a074875f63ab945a47b468a6f53432f665607a901a390` (copied to the private root only,
  `original/mashed_re_dev.asi` not touched).

## Default gates on this build

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + machine lock, private root, exe
`26f6e7d6`. All six equal `re/orchestrator/merge_train.json`:

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `8284be46` | `8284be46` |
| H_DEF | gates.csv | `ba407a16` | `ba407a16` |
| H_DEF | ptrace.log | `31fe043e` | `31fe043e` |
| B_DEF_K0 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K1 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K26 | step.csv | `1d5bf4f9` | `1d5bf4f9` |

## Run log

- Audio check around every window (`drive_nt.py`): a VB-Audio endpoint present before and after each of the 14 recovery
  windows (12 rows + 2 re-runs). 0 MASHED Event-1000 events in any window, no fault at `0x005baf48`, no U-9677 signature.
  The pre-reboot row-1 window logged an empty endpoint list after the run: that was the machine shutting down at 14:23,
  not an endpoint fault.
- The machine lock was shared with D4-WSB-PORT-BROAD-2, D4-DC-RFX-CORE, MATCH-S05+NAKED-gates, D5-SHIM-DINPUT8-SPAWNERS
  and the merge train. Rows queued for their solo windows. No run was stopped.

## Tracker transaction (re-classify)

- `hooks.csv`: 9 rows C2 -> **C3 impl** (`file` = the port, `scenario` `shadow_tracked:track0-mode10-cars4`,
  `frida_diff` `shadow_ab:verify/d4wsb_portnarrowtail_20261101/shadow_summary.tsv`, dated note naming the caller and
  callee). `0x0057c2b0`, `0x0057c300`, `0x0057c500` stay C2 (`file` = the port, status `impl`) with a dated note.
  `0x0057c420` stays C2 `mapped` with the held-row note.
- `UNCERTAINTIES.md`: U-9341 (`0x0057c500`), U-9342 (`0x0057c300`), U-9485 (`0x0057c440`), U-9486 (`0x0057c550`)
  resolved through `re/tools/uncertainty_resolve.py`. Hand-read from the disassembly:
  - U-9341 / U-9342: `call [eax+0x108]` at `0x0057c50d` / `0x0057c30d`, `eax` = `[0x007d3ff8]` (RwGlobals), a slot read
    at run time (indirect-dispatch clause). Caller-cleans: 2 args pushed, popped by `add esp, 8` at `0x0057c515` right
    after the call in `0x0057c500`, and by `add esp, 0x14` at `0x0057c328` together with the 3 pushes for `0x0055ded0` in
    `0x0057c300`, so the esp depth at `0x0057c338` is entry - 4 (`push esi`) and `[esp+8]` is arg 1.
  - U-9485: straight-line code from entry to `0x0057c44d`: `sub esp, 0xc` + 5 pushes, depth entry - 0x20. The
    `push 1` at `0x0057c44b` is the third arg of `0x004c51a0`, popped by `add esp, 0x10` at `0x0057c49a`.
  - U-9486: `call [ecx+0x10c]` at `0x0057c57e`, `ecx` = `[0x007d3ff8]`, 1 arg, `add esp, 4` at `0x0057c584`. The join at
    `0x0057c570` is reached at the same depth from both paths (the `jne` at `0x0057c55c`, and the fall-through whose 2
    pushes are popped by `add esp, 8` at `0x0057c56d`).
  The CLEAN shadow results confirm U-9485 / U-9486. U-9341 / U-9342 rows stay C2 for the allocator reason above.
- No new U or S rows. `re/analysis/CHANGELOG.md`: one line per row below `<!-- ENTRIES -->`.

## Next

- Allocator rows `0x0057c300` / `0x0057c500` (with `0x00564130` / `0x00562520`): a shadow arm that pins the slot
  `+0x108` / `+0x114` result for both runs, then re-classify.
- `0x0057c2b0`: lead decision on whether the `0x0057c293` store into the `0x0062403c` table counts as an installer, or a
  caller row elsewhere.
- `0x0057c420` / `0x0055bd80`: return the slot EAX through both void ports, as a default-OFF knob under its own PREREG
  (default `.asi` behaviour change).

## Files

Committed: `PREREG_D4-WSB-PORT-NARROW-TAIL.md`, `RESULT_D4-WSB-PORT-NARROW-TAIL.md`, `shadow_summary.tsv`, `disasm_lin.py`,
`scan_eax_use.py`, `run_shadow10.py` (copy of `verify/d4wsb_portbroad2_20261101/run_shadow9.py`, data path and label
changed), `drive_nt.py` (copy of `verify/d4wsb_portbroad2_20261101/drive_pb2.py`, runner name, data path and label changed).

Raw (not committed), `D:/MashedWT/D4-WSB-PORT-NARROW-TAIL_data/`:

| file | SHA-256 |
|---|---|
| `decomp.json` | `38cbaa5a23f06de59fb34035acc19459ad9fbe8c5706f1734db74766111bbb5c` |
| `drive_nt.log` | `c6a56f46fd8c900bf970eb03fc07adfa85f23c1ee1f462eb6e402cecc8867506` |
| `arm_s_recovery.out` | `93d058b059fa85de6362873c9b12061fa2ec9c18f95c59f7d1f1bb99798879f2` |
| `arm_s_rerun.out` | `21e539e99662efd5663794bb50ea4c61d4528b2da551ee72af9e155aa60c7a94` |
| `00574920/shadow_ab.log` (re-run) | `77509403ce70c9d668b8ec213a80491bc8b12039a768bfef334381e633d59b6a` |
| `005749b0/shadow_ab.log` | `e9f933c153417a04afeb3128b515e69eed31b08230e05978386ac419637c594e` |
| `00574a20/shadow_ab.log` | `ed7495d3497da609a0ea8a9afc9f99c90d040de130ec013abf26031f6c1c4cff` |
| `00574ac0/shadow_ab.log` | `0afa4da21412062082c4c3389aefbde10a9d126f21e0efa07b43ca35e3a74c4b` |
| `0057c1b0/shadow_ab.log` | `3981b56f10d0df3bb091fd86936d4b6a52fce60b53c038bb369df67db164bcaf` |
| `0057c220/shadow_ab.log` | `7d4cb27d56ecfb821de9f05bf2cc6d060fd50a79bc83d748640859d2f2a336ca` |
| `0057c270/shadow_ab.log` | `efa50010689f152acc61eb5a3810ed9cfa5a7468b4038a9c35117a498b2ce064` |
| `0057c2b0/shadow_ab.log` | `4c42c54b0a9cda4ed8ee83e4d0f2fbc6f1f8a64017b25a7675121eb5b219c307` |
| `0057c300/shadow_ab.log` | `afe4c62905a3c4ac1dba4622653e23660a3ea805f575e143a5d90f29c02bf4c3` |
| `0057c440/shadow_ab.log` | `2572761b03481227ec14ee91cfb33c35dbc7b0c5e136bb6fef7e420ece3aaaea` |
| `0057c500/shadow_ab.log` (re-run) | `67214026a4cc6443ae40d5d2032c6aa76b29822c32d1221787b0161af3bc1765` |
| `0057c550/shadow_ab.log` | `5dc5263659beb6b8e6284b5c9176a15b1dfcf526adbf70023a728a11a31d6ba1` |
| `00574920/shadow_transcript_reboot_cut.txt` (not evidence) | `d6d4fe7f5668f412195fb4ad7a8b4c22c270d8987efb050098fd1d86d5da217d` |
| `00574920/shadow_transcript_pass1_launchfail.txt` | `579b413acfcf6e97f53f2866c83745b55571adbe4153be68ad6572656a255fc1` |
| `0057c500/shadow_transcript_pass1_launchfail.txt` | `3403a6771f1aa8ee71e7ff0bf3ddbdc9eca63064a9152f56862ae21d7827cbc4` |
| `gates/H_DEF.step.csv` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| `gates/H_DEF.gates.csv` | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| `gates/H_DEF.ptrace.log` | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `gates/B_DEF_K0.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K1.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K26.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/RUNLOG.txt` | `e2b3b7c07cd2d7a4f064eabe27b8c76fdfa6badb50fca140f85f6c076aebb8dc` |

Game PIDs spawned this session (only these killed, by PID, by the runner): shadow 17208 (launch failure), 4268, 20964,
17828, 24784, 1028, 372, 8648, 23048, 25216, 1772 (launch failure), 16992, re-runs 13268, 4032. Gates (before the
reboot) 14288, 23152, 22720, 23500, self-exited. None alive at the end. No watcher left running.
