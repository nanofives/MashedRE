# RESULT GATERANK-ALLKNOBS (run 55, text-and-tool row: no build, no game)

Change: `verify/t2gaterank_20261121/t2_gate_rank.py` gains `--all-knobs` (and `--out <dir>`, default
`log/orch/run55_gaterank/all_knobs`). The default path is unchanged.

## Knobs in the all-knobs view (file:line of each getenv)
| knob | what it switches | source |
|---|---|---|
| MASHED_OT_RACELOAD=1 | 0x0040d270 -> Orig_0040d270 | Match/PortTop_StandInsRL.cpp:22 (and PortTop_00492370.cpp:137) |
| MASHED_OT_RACEFRAME=1 | 0x004111c0 -> PortTop_Bound_004111c0 (raceframe plan.json) | Match/PortTop_RaceFrameKnob.cpp:26 |
| MASHED_OT_RENDERFRAME=1 | direct callees of 0x00492e90 -> walked bodies (renderframe plan.json) | Match/PortTop_RenderFrameSel.cpp:14 |
| MASHED_OT_RL_D110=1 | 0x0040d110 -> PortTop_Bound_0040d110 (bulk plan.json) | Match/PortTop_StandInsRL.cpp:191 |
| MASHED_OT_TITLETICK=1 | 0x00429290 -> PortTop_Bound_00429290 (renderframe plan.json) | Match/PortTop_00402750_Pieces.cpp:239 |

Precondition of the 0x00492e90 root (PortTop_00492370.cpp:312-315): the root is the RW core body RwCore_00492e90 only when
`RwCore_FrameRoute()` (MASHED_RWC_FRAME) is also on; otherwise it is PortTop_HandRenderFrame. The view assumes it on. [UNCERTAIN]
whether the replay arm sets MASHED_RWC_FRAME; test: read the arm's env block.

Plan layers read in addition to audio and raceload: `re/tools/bind/state/{bulk,raceframe,renderframe}/plan.json`. A plan `run`
entry wins; a plan `stop` only sets an rva no earlier layer made `run`. The four knob roots are then forced to `run`.

## Checks
1. Default output unchanged: ran the default before the edit and after it. `cmp` of stdout (`/tmp/gr/before.out` vs `after.out`)
   and of the rewritten T2_GATE_RANK.tsv: IDENTICAL (empty diff). The committed TSV differs from a fresh run at this head
   (it was written at run 53), so the rewritten TSV was restored with `git checkout` and is not in the commit.
2. All-knobs run: exit 0, saved to `log/orch/run55_gaterank/all_knobs/` (`t2_gate_rank.out`, `T2_GATE_RANK.tsv`).
3. 0x004111c0 and 0x00492e90 are no longer ranked gate rows (route state run).
4. Not run: build, selftest, game, ddkobj (text-and-tool row).

## Effect (all-knobs vs default at this head)
route states: run 1240 -> 1353, stand 59 -> 37, stop 158 -> 367. Executed path R: 810 -> 1093 functions, T2 census in R 551 -> 817.
Gates on R: 165 -> 231 (stop 120, stand 33, unbound 78); ranked 137 -> 197.

## Top 10 (all-knobs)
| rank | rva | name | class | rel_tool fns | rel_tool calls | behind | state |
|---|---|---|---|---|---|---|---|
| 1 | 0x005a7b60 | FUN_005a7b60 | C4 frida-replay with source | 41 | 3290 | 42 | stop |
| 2 | 0x00448220 | Frontend::PostRaceResultCamera | C2 with a source (97.0%, in-situ DIVERGED@1116) | 34 | 2488993 | 17 | stop |
| 3 | 0x00440bc0 | FUN_00440bc0 | c4=match cpp-unclean | 30 | 241 | 32 | stop |
| 4 | 0x00420420 | FUN_00420420 | C2 with a source (79.8%) | 27 | 2379852 | 33 | stop |
| 5 | 0x004103a0 | SubStateHeldCountdown5 | c4=match bindable | 20 | 148932 | 41 | stop |
| 6 | 0x0055f800 | FUN_0055f800 | C3 with a source (65.3%) | 15 | 31504 | 26 | stop |
| 7 | 0x00487280 | FUN_00487280 | no source (C2) | 12 | 2150108 | 4 | stop |
| 8 | 0x0048ae00 | FUN_0048ae00 | hand stand-in | 11 | 1574016 | 1 | stand |
| 9 | 0x004e99b0 | FUN_004e99b0 | C2 with a source (45.5%) | 10 | 46124 | 6 | stop |
| 10 | 0x00409710 | FUN_00409710 | c4=match cpp-unclean | 10 | 10119 | 2 | stop |

## Unfinished
Static view only: whether a bound stop actually exits at runtime is not tested. MASHED_OT_AUDIOINIT / AUDIOTICK / TIMER /
AUDIOTHREAD_STANDIN sub-knobs were not in the requested set and are not forced (audio plan already read by default).
