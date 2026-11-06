# RESULT MATCH-PORTSTATE-GAMEFLOW-FU (2026-11-02, gates 2026-11-06)

Verdict: **GREEN (2026-11-06). P1, P2 and P3 hold, and P4 measured `phase 3 -> 7`.** Default gates are unchanged:
G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3. The knob stays default OFF. See "Gates (run 33, 2026-11-06)"
below. The 2026-11-02 text that follows is kept as written.

## Gates (run 33, 2026-11-06)

Branch merged with `orch/integration` at `6f383c743`. The merge brought ledger files only (`re/orchestrator/DECISIONS.md`,
`ledger/events.jsonl`, `ledger/roadmap.json`), with no source change. Rebuilt with `MASHED_NO_DEPLOY=1`: `=== Build OK ===`,
`[rva-lint] ... allowlisted=2 NEW=0`, `[asm-lint] OK`. Exe SHA-256 `701bd75d...` (the 11-02 build was `bda9abcb...`;
the source is the same, so the hashes differ by build only). Runner `verify/matchgameflowfu_20261102/gates2.sh`
(gates1.sh with the new exe) under `proyectos_test_slot.py` (outside) and `mashed_lock.py run` (inside), private root
`D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_root`. All 8 arms exited 0 by themselves (H 131 s / 134 s, B 13-19 s).
The first queued attempt (2026-11-05 23:55) never got a slot before the session stopped. It ran no arm
(`gates2_queued_dead.log`).

| arm | step.csv | gates.csv | ptrace.log |
|---|---|---|---|
| H_DEF | 8284be46 | ba407a16 | 31fe043e |
| H_PSGF | 8284be46 | 67931834 | 31fe043e |
| B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | | |
| B_PSGF_K0 / K1 / K26 | 1d5bf4f9 x3 | | |

Full hashes: `gates2_hashes.txt`. Witness extract: `gates2_witness.txt`.

- **P1 GREEN.** The knob is OFF, and every DEF arm equals the `merge_train.json` refs. No DEF log has a
  `[portstate-gameflow-fu]` line.
- **P2 GREEN.** H_PSGF step 8284be46 and ptrace 31fe043e match the refs. Its gates.csv is 67931834, the H_PSG value of
  MATCH-PORTSTATE-GAMEFLOW (COURSE's abs columns), as predicted. B_PSGF = 1d5bf4f9 x3.
- **P3 GREEN.** All 4 PSGF logs print `knob ON`, and `machine` equals `ticks` (H_PSGF through 13800). The logs have no
  `0x0040d440 reached` line, no `step 0x00771980` line, and show `e450=0`, `c080=0`, `step=0`.
- **P4 measured.** `0x0042c220 returned 1 (first), 0x0067eab0=0`, then `phase 3 -> 7 (tick 1)`. After that, `phase=7`
  and `eab0=1` hold for the rest of each PSGF arm (`c220=1`). The gates do not move, which matches the PREREG's static
  read: no race-path exe reader consumes `0x0067ea10` / `0x0067eab0` / `0x00771968`.

Raw data (D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_data, not committed):

```
701bd75d85bfc196fc082cd11ed2e3a8d583bdb2313521354df530457e018f16  mashed_re_6f383c743.exe
dce21091043cdd1a6fe18748228ed01959277378d58d2b0931d4094f64025ab8  build2.log
888274b9a6ba7f23fd56266739d92b6ea91a753c9756c7ec5b6e0f35457a5d82  gates2.log
```
Arm outputs: `arms_g2/` (step / gates / ptrace hashes in `gates2_hashes.txt`).

The handoff list below is unchanged. The flip needs an original-side run off Training, and no decision is filed.

## 2026-11-02 text

Verdict (2026-11-02): **BLOCKED. Built @ `e86684a29`, gates not run: display D3DERR_NOTAVAILABLE.** The static leg is done. 11 of
the 14 callees of the game-mode machine 0x004929d0 that had no exe body now have one (12 of 14 counting 0x0040e350,
which `orch/integration` already had). The machine itself has a matched source (95.7%) and runs in the exe behind
`MASHED_PORTSTATE_GAMEFLOW_FU` (default OFF), phases 3 and 7 only. The two left are 0x0040d440 and its callee
0x0040d270, the original course loader. No gate arm ran, so no prediction of the PREREG is measured.

- PREREG: `verify/matchgameflowfu_20261102/PREREG_MATCH-PORTSTATE-GAMEFLOW-FU.md`, committed alone at `f7d2da325`,
  after the source commit `e86684a29` and before any arm (none ran).
- Branch `orch/MATCH-PORTSTATE-GAMEFLOW-FU` from `orch/integration` @ 78be531e1. MATCH-PORTSTATE-GAMEFLOW was already
  merged (`b16e76e0b`), so nothing was stacked.

## Display

`d3dprobe.py` (committed here) creates a windowed 640x480 HAL device with `IDirect3D9::CreateDevice`. It ran every
5 minutes from 10:55 to 11:50, 20 runs. Every run returned `hr=0x8876086a` (D3DERR_NOTAVAILABLE), see
`display_poll.txt`. Two overlapping poll loops ran after the session moved licences at 11:07, so the numbering
repeats. The 60-minute bound was reached, so I stopped polling. `mashed_lock.py status` was `free` at 10:55. No
game was started and no MASHED PID was spawned.

## What the next session runs

`verify/matchgameflowfu_20261102/gates1.sh`. It copies the built exe
(`D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_data/mashed_re_e86684a29.exe`, SHA-256 `bda9abcb...`) and runs
`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSGF B_PSGF_K0 B_PSGF_K1 B_PSGF_K26` under `mashed_lock.py run`, on the
private root `D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_root` (robocopy of `original/` plus the GAMEFLOW root's
`extracted/` and `videocfg.bin`, made 10:55). Wrap it in `test-slot`. Score it against the PREREG's P1-P4. P1 (knob OFF
= the `merge_train.json` refs 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3) is the gate for `review`.

## Change (`e86684a29`)

Base `orch/integration` @ 78be531e1 (MATCH-PORTSTATE-GAMEFLOW is merged there, `b16e76e0b`). Built with
`MASHED_NO_DEPLOY=1`: `=== Build OK ===` for `mashed_re.exe` and `mashed_re_dev.asi`, `[rva-lint] 2 finding(s):
CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`, `[asm-lint] OK -- no bare absolute asm operand in 1143
source files`. Exe SHA-256 `bda9abcb99957da93e4bf1cdb718c12aeec2504f97def111f11bfacb013e35e2`.

### Matched sources (`re/match/gffu/`, new)

| rva | score | note |
|---|---|---|
| 0x004927c0 | 100% (156/156) | raw Ghidra form 26.3%. Case 3 written before case 2 (jump table 0x004929b4) moved the last 7% |
| 0x00417740 | 100% (3/3) | Ghidra lost the float load. Listing: `mov eax,[esp+4]; fld dword [eax*4+0x0089a870]; jmp 0x004a2c48` (__ftol) |
| 0x00497450 | 100% (12/12) | dword array, stride 0x200 (`shl eax,9` in the listing). Ghidra's decomp showed a byte array and `bool` |
| 0x005c9d00 | 100% (2/2) | `xor eax,eax; ret` |
| 0x004929d0 | 95.7% (235/235) | the machine. g2s 72.8%. Residual: the esi save sits before the call of 0x004927c0 in this compile and after the jump-table range check in the original (0x004929e8), so 10 push/pop instructions differ in position. No other instruction differs |

The existing matched sources of the other callees re-scored 100% on this tree: 0x0042c2f0, 0x0040d440, 0x0042c220,
0x0042c280, 0x00432080, 0x0040e450, 0x0040b6e0, 0x0040b540, 0x0040b700, 0x0040e460.

### Exe bodies for the 14

| rva | before (callee_bodies.txt) | now | file |
|---|---|---|---|
| 0x004927c0 | none | exe+asi body, no install | `Match/MatchGFFU_004927c0.cpp` (`Step771980Advance`) |
| 0x0042c2f0 | asi | exe+asi | `Match/MatchS07_0042c2f0.cpp` added to `exe_sources.rsp` |
| 0x0040d440 | asi | **asi only (not ported)** | its callee 0x0040d270 is the original course loader |
| 0x0042c220 | none (TU without install, asi) | exe+asi, no install | `Match/MatchS07_0042c220.cpp` added to `exe_sources.rsp` |
| 0x0042c280 | asi | exe+asi | `Match/MatchS07_0042c280.cpp` added to `exe_sources.rsp` |
| 0x005c9d00 | none (install disabled, 2-byte function) | exe+asi body, no install | `Util/GameStateGetters.cpp` `GetRaceEndTrigger` (already in both targets) |
| 0x00432080 | asi | exe+asi | `Match/MatchS07_00432080.cpp` added to `exe_sources.rsp` |
| 0x0040e450 | asi | exe+asi, install kept | `Match/MatchS02_0040e450.cpp` (moved out of `Util/PromoLoop_round26.cpp`) |
| 0x0040b6e0 | asi | exe+asi, install kept | `Match/MatchS02_0040b6e0.cpp` (moved out of `Util/PromoLoop_sessionB.cpp`, was a naked call of 0x0042f6a0 by address) |
| 0x0040b540 | asi | exe+asi, install kept | `Match/MatchS02_0040b540.cpp` (moved out of `Gameplay/ScoreMasks_ah3.cpp`, called 0x00417740 by address) |
| 0x0040b700 | none | exe+asi body, no install | `Match/MatchS02_0040b700.cpp` (`Counters8a9530Inc`) |
| 0x0040e460 | asi | exe+asi, install kept | `Match/MatchS02_0040e460.cpp` (moved out of `Util/PromoLoop_round26.cpp`) |
| 0x0040d270 | none | **none (not ported)** | the original course loader, see below |
| 0x0040e350 | asi in the GAMEFLOW listing | exe+asi already on `orch/integration` | `Match/MatchS02_0040e350.cpp` (MATCH-TOPDOWN-T13) |

Callees of those bodies that needed one too: 0x00417740 (`Match/MatchGFFU_00417740.cpp`, no install), 0x00497450
(`Match/MatchGFFU_00497450.cpp`, install kept, moved out of `Util/PromoLoop_sessionB.cpp`) and its jmp thunk
0x00496900 (`SlotActiveThunk`, same TU, install kept, moved out of `Util/PromoLoop_round24.cpp`, now forwards to the
port body instead of calling 0x00497450 by address).

So 11 of the 14 got an exe body here, 0x0040e350 already had one on `orch/integration`, 12 of 14 in all, and 28 of the machine's 30 callees have one. The link
of `Race/GameModeMachine.cpp`, which binds every callee by symbol, is the check: it fails if any reached callee has
no exe body.

**0x0040d440 / 0x0040d270, not ported.** 0x0040d270 (441 B, decomp_pc.py, pool0) calls 0x00426ba0, 0x004950b0,
0x00462510, 0x00426c00, 0x0042f790, 0x0042fe30, 0x0040d040, 0x00462500, 0x0040cf80, 0x0040d110, 0x00458c70,
0x00426340, 0x00426020, 0x00407800, 0x004074a0, 0x004264d0, 0x004ccde0, 0x0040cfd0, 0x0040d020 (-> 0x00426e10,
opens the track piz and runs COURSE.LUA), 0x0042f7a0, 0x00426070, 0x00411d90 and 0x004987b0 ("course LoadTime is
%u (%.3f)\n"), and 0x0040d110 calls 0x0040cea0 (dirt.piz). That is the original's course load. The exe loads the
course with TrackRenderer port code, so an exe body for 0x0040d440 means replacing the exe's course load, which is
outside this row (handoff).

### asi-side effect

Six asi installs now run the matched source in place of an older hand port: 0x0040e450, 0x0040e460, 0x0040b6e0,
0x0040b540, 0x00497450, 0x00496900. Each pair computes the same value (each matched source is 100% against the
original's bytes). 0x0040b540 now calls the port body of 0x00417740 and 0x00496900 the port body of 0x00497450
instead of the original's code at those addresses. No asi install was added for 0x004927c0, 0x0040b700,
0x00417740 or 0x0042c220.

### Knob `MASHED_PORTSTATE_GAMEFLOW_FU` (default OFF, exe only)

`Race/GameModeMachine.{h,cpp}`, called once per main-loop frame after `GameFlow_Update` (`exe_main.cpp`). Phase
word `0x00771968` in {3, 7}: the matched machine (0x004927c0, then case 3 or case 7). Other phases: 0x004927c0 only,
the machine's entry call. Cases 1, 2, 4, 5, 6 stay with GameFlow / the dev-race route. Case 2's 0x0040d440 binds to a
logging stub, unreachable from the tick (case 3 writes 7 or 5, case 7 writes 3). Witness lines
`[portstate-gameflow-fu]` in `mashed_re.log`.

## Handoff (what remains for 0x004929d0 to replace GameFlow)

1. 0x0040d440 -> 0x0040d270: the original course loader in the exe in place of the TrackRenderer load. This is its own
   row, and it is the largest piece.
2. Exe writers for the inputs the case-3 path reads: `0x0063b908` (read by 0x0040e450; no exe writer outside this
   step's 0x0040e460, whose caller named in the old body's comment is 0x00410860), `0x007e96fc + id * 0x200` (0x00497450's slot-active word, no
   exe writer). The round state at `0x008a94d0` / `0x008a94e0..60` stays as MATCH-PORTSTATE-GAMEFLOW listed it.
3. Cases 1, 4, 5, 6 in the tick, once GameFlow no longer owns those transitions.
4. The default flip of COURSE + GAMEFLOW (+ FU) needs an original-side run on a track other than Training (Arctic
   handling type 18 vs 6). Not run, no decision filed: this step measured nothing yet.

## Re-classify

None. No row changes level. The moved bodies change `hooks.csv`'s `file` / `exe_file` columns for 0x0040e450,
0x0040e460, 0x0040b6e0, 0x0040b540, 0x00497450 and 0x00496900, and the new exe bodies (0x004927c0, 0x0040b700,
0x00417740, 0x0042c220, 0x0042c2f0, 0x0042c280, 0x00432080) need `exe_file` entries. Those column updates go through
re-classify after the gates.

## Raw data (D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_data, not committed)

```
bda9abcb99957da93e4bf1cdb718c12aeec2504f97def111f11bfacb013e35e2  mashed_re_e86684a29.exe
46922b30a3efbc6540e03c338f0c37b89fd82613d86e18654057366e4327ce11  build1.log
```

Committed here: `PREREG_*.md`, `display_poll.txt`, `d3dprobe.py`, `gates1.sh`.
