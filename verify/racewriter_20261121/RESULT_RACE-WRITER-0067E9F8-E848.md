# RESULT RACE-WRITER-0067E9F8-E848 (run 54, 2026-11-21)

Branch `orch/RACE-WRITER-0067E9F8-E848` from `orch/integration` `d0fadd43d` (the head when the worktree was made; the brief
named `77dedfece`, one commit earlier). PREREG `verify/racewriter_20261121/PREREG_RACE-WRITER-0067E9F8-E848.md` committed alone
at `ac91c848d` before the arm. Row kind: race-path. No exe change, no profile data, no tracker edit, no promotion.

## 0. Verdict: BLOCKED. Both writers measured, neither is bindable by this row

| word | ref1 / W2 | writer (measured, W2) | why not bound |
|---|---|---|---|
| `0x0067e9f8` | 1 from tick 17 (ckpt 17 = 1 on both) | tick 16, store `inc dword ptr [0x67e9f8]` at `0x0043d7a3` (watch pc `0x0043d7a9` = next instruction), inside `0x0043d2a0` (FUN_0043d2a0, C2) | (a) `0x0043d2a0` has no 100% source: `porttop_bind.py plan --profile raceload --roots 0043d2a0` answers `stop C2`; `equiv.py` NOT-PROVEN (call-regargs at orig `0x0043d578`, call `0x0042ac00`: ebx `0xff000000` against `0x0`, a value difference, not a register rename). (b) its caller on the original, `0x0043dfd0`, runs on ORIG_TOP as the hand-menu stand-in `PortTop_StandIn05_0043dfd0` (HandTop_FrameMenu, `Match/PortTop_00492d30.cpp:31`), so the original menu code that writes this word never runs on the exe. Wiring `0x0043dfd0`'s provisional body (17.7%, 63 counting stubs) is a code edit of `PortTop_00492d30.cpp` and the T1 "menus with no stand-in" job, not profile data (RULE 1) |
| `0x0067e848` | 2 at tick 299 (ckpt 299 = 2 on both) | tick 162, store `add dword ptr [0x67e848], esi` at `0x0042e6ad` (watch pc `0x0042e6b3`), inside `0x0042e5b0` (MenuChromeShellB, C3, hand body `Frontend/MenuSpriteDispatch.cpp`) | the caller chain on the original runs through `0x0043c5b0` (MenuDrawLoop), a `reuse` entry of the PORT-TOP-RENDERFRAME-492E90 closure (`re/tools/bind/state/renderframe/plan.json` on `orch/PORT-TOP-RENDERFRAME-492E90` `8b673edc4`, key `0043c5b0` = reuse, referenced by the bound root `00492e90`). That closure is PORT-TOP-LAND-06-RF's (RVA ownership table): **STOP, reported, not bound**. Also `equiv.py` NOT-PROVEN on `re/match/s07/0042e5b0.c` (call-regargs, call `0x00473c20`: edi `0x2` against `0x4`), already listed in PROVISIONAL.tsv (54.3%) |

Third hit (context only): tick 0, `mov dword ptr [0x67e9f8], 0` at `0x00433275` inside `0x00433240` (C4, exe body
`Match/MatchS08_00433240.cpp`, already called), values 0 / 0.

Neither writer is `0x00458dd0`, a library function, or an RVA of PORT-OT-RACE-BIND-HANDLER / RO-AUDIOINIT.

## 1. Arm W2 (original)

`test-slot bash -c 'py -3.12 verify/racewriter_20261121/writer_arms.py w2 -- --watch 0x0067e9f8,0x0067e848 --watch-from install --watch-stack 16'`
(the W1 recipe of RACE-WRITER-0067EA74; `detorig.py` has no `--watch-global` flag, `--watch` with `--watch-from install` is
the recipe). Root `D:\MashedWT\RACE-WRITER-0067E9F8-E848_root` (robocopy of `RACE-WRITER-0067EA74_root`), MASHED.exe
`b9977dab...fe87` as W1. Test slot waited 600 s for memory, then started under the cap. `mashed_lock.py status` before the arm:
`HELD label=PROV-INSITU-REF1-WS2` (no PRIORITY-HOLD, no SOLO); `mashed_lock run` queued on it.
Exit 0, `bootlen` qpc 222, `race.dn_warp` 108 (= ref1), 3 watch hits, 22 checkpoints.

| tick | pc (after store) | store | values (e9f8, e848) after | fuzzy return chain (watch-stack) |
|---|---|---|---|---|
| 0 | `0x0043327f` | `0x00433275` mov [0x67e9f8], 0 | 0, 0 | `0x004927a5` (in 0x00492770 MainLoopInit) `0x00492295` `0x004923de` |
| 16 | `0x0043d7a9` | `0x0043d7a3` inc [0x67e9f8] | 1, 0 | `0x0043e0ee` (in 0x0043dfd0) `0x004950f9` `0x004030f1` `0x00492dd3` (in 0x00492d30) `0x004922b8` (in 0x00492290) |
| 162 | `0x0042e6b3` | `0x0042e6ad` add [0x67e848], esi | 1, 2 | `0x0043c678` (in 0x0043c5b0 MenuDrawLoop) `0x004c2bde` `0x00492ffc` (in 0x00492e90) `0x00493008` `0x004922bd` (in 0x00492290) |

Store instructions: capstone on `MASHED.exe.unpatched` (anchor) ending exactly at each watch pc. Containing function: the
largest `hooks.csv` start at or below the pc.

Checkpoints against ref1 (`D:/MashedWT/RO-REF-NAME_data/ref1`, `portotrace_words.py`): ticks 1 / 17 / 18 / 299 equal on both
words (0,0 / 1,0 / 1,0 / 1,2). PREREG predictions 1 and 2 hold. Prediction 3 (game function, bindable and unowned) fails:
`0x0043d2a0` is unowned but has no bindable body and an unbound caller, `0x0042e5b0` sits under an owned closure.

[UNCERTAIN] The caller chains (`0x0043dfd0` -> `0x0043d2a0`, `0x0043c5b0` -> `0x0042e5b0`) come from the FUZZY stack scan and
the sources (`re/match/s08/0043dfd0.c`, `re/match/s08/0043c5b0.c`), not an exact backtrace (the exact `bt` holds only the pc).
Register lines added (`re/orchestrator/UNCERTAIN_REGISTER.md`, "RACE-WRITER-0067E9F8-E848").

## 2. Checks

No build, no `ddkobj.py check`, no knob boot, no self-test, no replay-oracle arm: nothing in the exe or the bind profiles
changed, so each would reproduce the integration head. Binding numbers: executing/reachable +0, pre-bound +0, unbound 2
(`0x0043d2a0`: binder `stop C2`, equiv NOT-PROVEN, caller is the hand-menu stand-in; `0x0042e5b0`: under the
PORT-TOP-LAND-06-RF closure, equiv NOT-PROVEN). ddkobj against `log\orch\run54_headcheck\`: not run, no newly DIFFERENT unit
possible. Listed unit -> 100% neighbour fallback pairs: none (nothing bound). G-CTRL / B: not run (race-path row, no change).

## 3. Next

1. `0x0067e9f8`: needs the original menu navigation on ORIG_TOP, i.e. `0x0043dfd0` run as its body instead of the hand-menu
   stand-in, plus a body for `0x0043d2a0` (54.3%; its call-regargs difference at `0x0043d578` has to be fixed in the source
   before it can be a provisional body). T1 menu work, a new row, not a binding.
2. `0x0067e848`: for PORT-TOP-LAND-06-RF (or a follow-up of it) once `0x0043c5b0` stops being a reuse in the renderframe
   closure; `0x0042e5b0` would need its edi `2`/`4` call-arg difference at the `0x00473c20` call fixed first.
3. Both words are menu state the menu-less race arm inherits. A replay arm that seeds them from ref1's tick-17 / tick-299
   image instead of running the menu is the cheaper route to clear the record-16 NEW block; that is a decision for the RACE
   lead, not done here.

## Evidence

Committed: this RESULT, the PREREG, `writer_arms.py`, `w2_watch.jsonl` (the 3 hits, 1.7 KB), the register lines.
Raw (not committed), `D:/MashedWT/RACE-WRITER-0067E9F8-E848_data`:

| file | SHA-256 |
|---|---|
| `w2.log` | `7dfca23d04177ae113438b7cbd8bcf610d76050b8a497dc2a841aa6be0a8138d` |
| `w2/watch.jsonl` | `6998d2b3eef26ed435c360c3efc5313ec52568b5251713053f38c5152326f395` |
| `w2/meta.json` | `99d80d113f9bca9b71ad5307bbf6060324af81de448af99c0b2c5923959fb635` |
| `w2/ckpt201.data.bin` (tick 1) | `c1c4882927d5a456fed2768443800bc71bf15671ad8853c6563285eb8a7d727a` |
| `w2/ckpt202.data.bin` (tick 17) | `4c6a814cf8383a85a61d71a6fa996dfdb6542f1881be7b6f2a1c5f13397a119f` |
| `w2/ckpt203.data.bin` (tick 18) | `b5c2592e3940cf913d9106053a430f43cee1744dbc8c4afe4f760547b9f5498d` |
| `w2/ckpt204.data.bin` (tick 299) | `f4ea23f10642b6f28715e46436f96af3151a855dcd39ea122299867c20e6ab9c` |
| `w2/calllog.bin` | `9243e9796f444d9364360f3bd3298d4bfb37b3dbdb2220932054338a263d386c` |
| `w2/calllog_boot.bin` | `8fdf7a75545523d554d9b7f5b21f7fbcc65c11ce66552478e17c74fcd10c7de1` |
| `w2/ticks.bin` | `87293c5fa6c5f416ebd2a7efb20f245ff7f5382595374447df262a23cbacffed` |

Commands: `py -3.12 re/tools/bind/porttop_bind.py plan --profile raceload --roots 0043d2a0` (and `--roots 0043dfd0`:
`reuse ProvBody_0043dfd0`); `py -3.12 re/tools/match/equiv.py re/match/s08/0043d2a0.c re/match/s07/0042e5b0.c`.
