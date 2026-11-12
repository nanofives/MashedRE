# RESULT PORT-TOP-08 (2026-11-11): BLOCKED. 0x0042c960 is reached only through the unmatched menu top 0x0043dfd0, under the 0x00492d30 stand-in

L2 worker `cmwcuqlh44r32sc1bdprb16iy`, run 38, section D4. Branch `orch/PORT-TOP-08` from orch/integration
`bf44aae7d`. Row kind: top-down. No source change, so no build, no knob boot and no menu walk were run: there is no
knob-path change for them to measure.

## Verdict

BLOCKED. 0x0042c960 and the menu top cannot run on the MASHED_ORIG_TOP path from their real call chain until two
other rows land:

1. **PORT-TOP-05** (open): 0x00492d30 is still `PortTop_StandIn_00492d30 -> HandTop_FrameDetTick + HandTop_FrameUpdate`
   (verify/porttop_20261108/BINDINGS.tsv). Everything below it does not run on the knob path.
2. **A source for 0x0043dfd0** (the menu top, 10,969 B, C2): re/match/s08/0043dfd0.c is 17.7% (regs 46.9%).
   equiv.py: `NOT-PROVEN x87-region` (`fst m4[esp+0xc] | fst m4[esp+0x4]`, first x87 instruction,
   `equiv_0043dfd0.txt`). The dynamic cross-check in verify/equivcheck_20261111/dyn_notproven.tsv says `DIFFERS`.
   L-126 never admits a difference in x87 instruction order. The only knob-path route left is a provisional body, and
   `PROVISIONAL.tsv` does not exist yet (row PROVISIONAL-BODIES, open). The provisional body would also run a source
   the cross-check shows computing other values.

I did not call the matched 0x0043c000 / 0x0042c960 from inside the hand frame stand-in. That would run the original's
menu code on globals the hand menu (Frontend/MenuNavSM.cpp, frozen hand path) does not keep at their original
addresses. That works around the blocker instead of binding the real chain, and nav_coverage / drawlist could not
see it.

## The call chain (callsites.py on original/MASHED.exe.unpatched, every site disassembler-confirmed)

| callee | call sites | caller | state on the knob path |
|---|---|---|---|
| 0x00492d30 | 0x004922b3 | 0x00492290 (matched, bound) | stand-in (PORT-TOP-05) |
| 0x0043dfd0 | 0x00492d92, 0x00492dce | 0x00492d30 | not reached, 17.7%, NOT-PROVEN |
| 0x0043c000 | 0x0043e05d | 0x0043dfd0 | not reached, C4 match, body asi-only (Match/MatchS08_0043c000.cpp) |
| 0x0042c960 | 0x0043c2d9 | 0x0043c000 | not reached, C4 match 2340 B (re/match/s07/0042c960.c), no exe body |
| 0x004111c0 | 0x0042c982, 0x00492d85, 0x00492dc3 | 0x0042c960, 0x00492d30 | exe body Match/MatchT01G_004111c0.cpp, under-stand-in (PORT-TOP-06) |

This confirms the row title: 0x0042c960 is the other caller of 0x004111c0 (0x0042c982). The hooks.csv name of
0x0042c960 is CameraTransitionStateMachine (subsystem render), not a frontend state machine.

## What the chain needs once PORT-TOP-05 and 0x0043dfd0 are settled (closure_04c.py)

0x0042c960, depth 2 (`closure_0042c960.txt`): 37 exe, 17 asitu, 3 gen, 2 none.

- gen (matched source, no body): 0x0042c960 itself, 0x00424270 (99.6%, equiv.py `EQUIV [VALUE-FLOW]`,
  `equiv_00424270.txt`), 0x0042c3c0 (100%).
- none: 0x0042c510 (C2, no source, hooks.csv subsystem vehicle) and sprintf 0x004a2b60 (CRT, already bound on the knob
  path, BIND).
- asitu: 17 single-TU matched bodies (0x00429840, 0x0042c7c0, 0x0042c300, 0x004241c0, the 0x00423b40..0x00423c40
  score accumulators, 0x0040b460), to be added to the exe list.

0x0043c000 callees (`#define`s in Match/MatchS08_0043c000.cpp): 0x0042f7b0 and 0x0042fa00 have exe bodies.
0x004332a0 (C2, 97.3%) has no port body, and equiv.py gives `EQUIV [LAYOUT,VALUE-FLOW]` (`equiv_004332a0.txt`), so
EQUIV-ADMIT can admit it with no decision row.

## Proposed next rows

- PORT-TOP-05 first (0x00492d30 matched under the knob, with 0x0043dfd0 as a named stand-in at 0x00492d92 / 0x00492dce).
- A MATCH row for 0x0043dfd0, or a decision on running it as a provisional body once PROVISIONAL-BODIES exists.
- In the same pass as those: ADMITTED.tsv rows for 0x004332a0 and 0x00424270 (EQUIV above), and a MATCH row or a
  stand-in for 0x0042c510.

## Files

- `equiv_0043dfd0.txt`, `equiv_004332a0.txt`, `equiv_00424270.txt`: equiv.py output, tool at `bf44aae7d`.
- `closure_0042c960.txt`, `closure_0043c000.txt`: closure_04c.py output.

Gates: not run. Top-down row with no source change (WORKER_RULES: G-CTRL / B are never run for a top-down row).
