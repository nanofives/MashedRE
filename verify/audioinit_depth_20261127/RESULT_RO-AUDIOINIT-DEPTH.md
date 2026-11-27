# RESULT RO-AUDIOINIT-DEPTH (run 56, G2, decision A): 52 distinct functions between 0x005B8910 and the 0x00604604 write, 13 not bound (3,239 B)

Branch `orch/RO-AUDIOINIT-DEPTH`, worktree `D:\MashedWT\RO-AUDIOINIT-DEPTH_wt`. It was created from orch/integration
`1379467a6` on C:, then moved to D: during the run's disk hold (L0 order, removed with `diag.py wt-remove`). Row kind:
evidence from the ORIGINAL only. One census arm on ref1, no port work, no build, no gates. The worker ran Opus 5.5 medium
on claude2.

## Verdict: GREEN (G2 met with a measured number)

The original executes **52 distinct functions** from the first enter of 0x005b8910 (the current stop) up to the store
that writes 0x00604604 at tick 0 (record 0). Each one is classed below.

| class | functions | bytes (pc_funcs size) |
|---|---|---|
| bound (bind or reuse line in a `re/tools/bind/state/*/plan.json`) | 39 | 4,769 |
| C4 unbound | 6 | 370 |
| source but not C4 | 2 | 1,788 |
| no source (hooks.csv row below C4, no re/match file) | 2 | 73 |
| no row (no hooks.csv row) | 3 | 1,008 |
| **rest of the lane (not bound)** | **13** | **3,239** |

There is one possible addition, outside the count: 0x004a2d30 (`_pow`, 72 B, C4 libid, unbound). It has no counter, so
it may be a 14th unbound function (section 4). The full table is `window_functions.tsv`: one row per function, with
class, calls in the window, calls before it, size, confidence, c4 route, binder lines, stop lines and source path.

### The 13 not bound

| rva | class | calls in window | size | status | note |
|---|---|---|---|---|---|
| 0x005b8910 | no row | 27 (+1 before = the snap call) | 715 | none; audio plan `stop` | the stop. Draft 71.0% in `D:/MashedWT/RO-AUDIOINIT-45D460-REACH-4_data/draft_005b8910.c` |
| 0x005b8c40 | source not C4 | 28 | 1,676 | C2, `re/match/s27/005b8c40.c`; audio `stop` | 14 indirect call sites (section 4) |
| 0x005c6f50 | no row | 28 | 278 | audio `stop` | 2 indirect call sites |
| 0x005a65d0 | source not C4 | 200 | 112 | C2, `re/match/s25/005a65d0.c`; audio `stop` | |
| 0x005a66c0 | no row | 200 | 15 | audio `stop` | |
| 0x005a7420 | C4 unbound | 200 | 64 | C4 match, `re/match/s25/005a7420.c` | not in any plan |
| 0x005aeb90 | C4 unbound | 28 | 98 | C4 frida-replay, `re/match/s26/005aeb90.c` | not in any plan |
| 0x005c6bb0 | C4 unbound | 28 | 36 | C4 match, `re/match/s28/005c6bb0.c` | not in any plan |
| 0x004a4554 | C4 unbound | 34 | 111 | C4 libid `__heap_alloc`; bulk `stop` | CRT, under the reused `MashedCrt_malloc_004a45fb` |
| 0x004a45cf | C4 unbound | 34 | 44 | C4 libid `__nh_malloc`; bulk `stop` | same |
| 0x004a59bf | C4 unbound | 34 | 17 | C4 libid `CrtSehEpilog`; bulk `stop` | CRT |
| 0x004a5984 | no source | 34 | 59 | C2 `CrtSehProlog` | CRT |
| 0x004ccef0 | no source | 34 | 14 | C1, RenderWare; audio `stop` | |

Size of the rest of the lane:
- 13 functions, 3,239 B.
- Of these, 6 (370 B) are C4 and need only a binding: 3 match or replay audio bodies and 3 CRT libid.
- 7 (2,869 B) need RE work: 2 sources below C4 (1,788 B), 2 rows with no source (73 B, both CRT/RW), and 3 with no
  row (1,008 B).
- 0x005b8c40 and 0x005b8910 are 2,391 B of the 3,239.

## 1. Method

- **Arm** `d1`, run by `depth_arms.py`. It is the T2-RACEPATH-CENSUS recipe (`verify/census_20261116/census_arms.py`,
  the RO-REF-NAME ref1 argv, unchanged), with this row's root, data folder and lock label added. Changes:
  - `--prov-count count_list8.txt`: the c8 list, 6,445 counters, sha256 `c35a34f6...`.
  - `--prov-snap 0x004669b0:enter,0x004669b0:leave,0x0045d460:enter,0x005b8910:enter,0x0045d678:enter,0x0045d748:enter`.
- **Root** `D:\MashedWT\RO-AUDIOINIT-DEPTH_root`: a robocopy of `T2-RACEPATH-CENSUS_root`, then `original\` copied over
  it with `/E`. `original\MASHED.exe` sha256 is `b9977dab70607720...`, the ref1 exe.
- **Launch:** under test-slot (slot 2/8, after a 600 s CPU wait), with `mashed_lock.py run --label RO-AUDIOINIT-DEPTH`
  inside it. `mashed_lock.py status` was free before the launch.
- **Window:** a function is in the window when (counter at the first enter of 0x0045d748) minus (counter at the first
  enter of 0x005b8910) is above 0. 0x005b8910 itself is included as the window's first call.
- **Join** `depth_join.py`. Classes in precedence order: bound, C4 unbound, no row, source not C4, no source. Inputs:
  hooks.csv (confidence and the `c4=` note), every `re/tools/bind/state/*/plan.json` (bind and reuse lines = bound), and
  `re/match/**/<rva>.c`, all at `1379467a6`.
- The `d1/prov_warp.json` and `d1/prov_anchor.json` reports give byte-identical tables (cmp SAME).
- The join was smoke-tested on c8's `prov_warp.json` with stand-in snap keys before d1 ran. That was a mechanics test
  only, not cited as a count.

**Why the end snap is 0x0045d748 and not the store** (the L0 check). From the disassembly of 0x0045d460:
1. 0x0045d68c sets edi = 0x00604578.
2. 0x0045d6a5 `mov [edi-8], esi` stores 0x00604570 (bank 0).
3. The bank-0 inner loop (0x0045d6b0..0x0045d742: call 0x005a6710 at 0x0045d6bc, call 0x005a6dc0 at 0x0045d707 and
   0x0045d733) exits to 0x0045d748, through `jbe` at 0x0045d6a8 or by falling through at 0x0045d742.
4. 0x0045d756 `add edi, 0x94` makes edi = 0x0060460c.
5. 0x0045d762 `jb 0x0045d698` is the back-edge, then 0x0045d6a0.
6. 0x0045d6a5 stores to 0x00604604.

So the first entry of 0x0045d748 comes after the bank-0 store and before the 0x00604604 store, with no call between them.
A snap on 0x0045d6a5 itself would fire first on the bank-0 store, at the wrong address. The writer agrees with
`verify/replayoracle_20261113/firstdiv/fd_o2_fd2.md` line 38 (ref writer pc 0x45d6a8 = after the store at 0x0045d6a5,
tick 0, value 4).

**Snaps in d1** (all thread 24192, tick 0, in this order): 0x004669b0 enter n=12, 0x0045d460 enter n=18, 0x005b8910
enter n=18, 0x0045d678 enter n=46, 0x0045d748 enter n=46, 0x004669b0 leave n=46. They are the same as c8's split:
0x004669b0 enter n=12, 0x0045d460 n=18, leave n=46.

## 2. The window, measured
- 0x005b8910: 1 call at its snap, 28 at the end snap (27 in the window). 0x005b8c40: 0 before, 28 in the window.
- Before the bank loop (0x005b8910 enter to 0x0045d678 enter), 51 functions move. In the bank-0 sub-window
  (0x0045d678 to 0x0045d748) only 0x005a6710 moves, with 4 calls (the 4 bank-0 entries). 0x005a6dc0 is not called there:
  both its call sites are skipped by the `je` at 0x0045d6e6 / 0x0045d6fe and 0x0045d728.
- The hottest bound function is 0x005aac00, with 33,632 calls.

## 3. Checks run, with results
- `mashed_lock.py status` before the arm: free.
- test-slot: acquired slot 2/8 after 600 s ("cpu still short after 10 min, starting anyway under the memory cap").
- Arm d1: exit 6. The game exited rc 0xC0000005 **after the race anchor** (`prov_anchor.json` was written). That is the
  known census race-tick crash (RESULT_T2-RACEPATH-CENSUS section 6, c8 to c10). The window is at tick 0, so it is not
  affected. The warp line: `prov n 6445, count 6445, fail 1`. The one failure is 0x00497310, which detorig already hooks
  (as in c8).
- Join on the warp report and on the anchor report: identical tables.
- `blind_callees.py`: capstone direct-call / jmp scan of the 52 window functions for pc_funcs entries with no counter.
  Result: 1 target, section 4.
- `scan_604604.py`: capstone scan of every pc_funcs extent for a memory operand within 0x40 of 0x00604604 as an absolute
  displacement. Result: none. The store is register-relative (`[edi-8]`), as shown above.
- Not run (row kind): builds, ddkobj, binder selftest, G-CTRL, B, 12 captures, the oracle. RULES 1, 3, 7 and 9 are not
  applicable: no binder state was touched.

## 4. Blind spots ([UNCERTAIN], two register lines added)
1. **All-thread counters.** The census counters count every thread. The audio worker (entry 0x005aef70, also 0x005a84d0
   and 0x005c2dd9, count 1 already at 0x0045d460 enter) was running during the window. The bank-0 sub-window is shown
   free of worker counts (only 0x005a6710 x4 moves), but the part before 0x0045d678 is not proven main-thread only. The
   count of 52 is an upper bound for the main-thread set. Test: per-thread counters, or a Stalker follow of the main
   thread over the same window. Register line `RO-AUDIOINIT-DEPTH (all-thread counters)`.
2. **118 pc_funcs entries carry no counter.** 96 come from the c8 exclusions and 22 from pc_funcs.csv growing after c8's
   list was cut, e.g. 0x005c92c0 / 0x005c92f0 / 0x005c9310 from REACH-4. The direct-call scan of the 52 finds one: **0x004a2d30
   `_pow`** (C4 libid, unbound, 72 B), called by 0x005b8c40 at 0x005b91c2. If it runs, it is one more C4-unbound function
   (7 C4 unbound, 14 not bound, 3,311 B). Register line `RO-AUDIOINIT-DEPTH (_pow)`.
3. **Indirect calls** whose target has no counter would be missed: 26 indirect call sites in 9 of the 52 functions,
   14 of them in 0x005b8c40. Targets that do have a counter are already in the count.
4. Imports (dsound, kernel32, ...) are outside MASHED.exe and are not counted.

## 5. Frida temp folders
`%LOCALAPPDATA%\Temp\frida-*`: 698 before the arm (all sessions) and 1 after. Something else cleaned the folder in
between. The one left (`frida-316dbdfaf0005d08d77d0eb3454c4657`) is newer than the arm's start, so it is taken as this
run's. It was left in place, not deleted.

## 6. Files
Committed: this RESULT, `window_functions.tsv`, `blind_callees.txt`, `depth_arms.py`, `depth_join.py`,
`blind_callees.py`, and two lines in `re/orchestrator/UNCERTAIN_REGISTER.md`.

Raw (`D:/MashedWT/RO-AUDIOINIT-DEPTH_data/`, sha256 also in `sha256.txt` there):

| file | sha256 |
|---|---|
| d1/prov_warp.json | f4d3a6ced0f773d2d4154e6f9a95e208a5434716b8fadc95f4bb12e4aa110bf8 |
| d1/prov_anchor.json | e799fbe832df7c701c8485654fcbbab905418ed890edf9b404a9731d088607e6 |
| d1/prov_install.json | 7e1b6c53d5ec3e9885dacbf667a93ae72daf41734936ab108771891e8101db3f |
| d1/meta.json | 4580b9031f0126ac415627b894ec3f9eed1bb10aaafad7ac2f7a38bb2cbda09b |
| d1.log | c4bb12b6ea2248417ac51d70c4e09180c033094278a86fa3a0fff8926f40f44b |
| d1_driver.out | add6e2b935eea93b260d5c62e001e8e1845edac5b1b67b6ab10fb95a7fbf91dd |
| count_list8.txt | c35a34f606f3eb278b348fb7212c404b24347be45127bb816576cb9bbe92226a |
| window_anchor.tsv (anchor-report join) | 3144022a9d53d03e04a3398dd5518de25e53a8d8013a908119588eec1c4ef5c1 |
| scan_604604.py | 4a3575cc114f238741796ee2425c0d3751cfadaf8ec7705ea38c876385b6c47e |
| ghidra_funcfacts.jsonl (rebuilt by xref.py, not used for counts) | b3cca676656a1c689c52c7535b0e93c66c9007550c0b0537bb1d4654d002058b |

## Next
REACH-5 staffing is the parent's call (decision A). The measured size: 13 unbound functions (3,239 B), 6 of them
bind-only C4. The RE work is 0x005b8910 (no row, 715 B), 0x005b8c40 (C2 source, 1,676 B), 0x005c6f50 (no row, 278 B),
0x005a65d0 / 0x005a66c0, plus two small CRT/RW rows. A per-thread re-count (blind spot 1) would tighten the 52.
