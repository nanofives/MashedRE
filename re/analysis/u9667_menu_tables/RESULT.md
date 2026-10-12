# U-9667: live menu tables vs the standalone's ItemActionCode

**Verdict: difference (1) fires on real tables. Differences (2) and (3) never fire.**

- On cursor >= 0, `ItemActionCode` (`MenuNavSM.cpp:432`) and the bytes of 0x0042ac90 agree on all 80 items of all
  33 live tables (83/83 cursor values, including the 3 tables where cursor -1 also agrees).
- On cursor -1 they disagree on 30 of the 33 tables. This is cause (1): `ItemActionCode` returns `0xffffffff`, but the
  original's final scan runs from word 0 and returns item 0's action code.
- The original does reach cursor -1 at select. On **screen 6** (table `0x005f6da0`, depth 5) a DOWN press set the slot
  cursor to -1. The confirm that followed called 0x0042ac90 with cursor -1, and it returned **`0xff260000`**. This
  was seen in 3 of 3 runs. `ItemActionCode` returns `0xffffffff` for the same input.

Worktree branch `work/u9667-tables`. No build was made. `MenuNavSM.cpp`, the trackers and `original/` are unchanged.

## Capture

Tool: `re/frida/u9667_menu_table_dump.py`. It runs `original/MASHED.exe` with `MASHED_RE_NO_AUTO_HOOK=1`, kills only
the PID it spawned, and reads memory with plain reads. On every dump it reads:

- the slot index `[0x0067e9f8]`;
- for slots 0..max(3, depth): the whole 0x40-byte record at `0x0067ed38 + slot*0x40`. Word 0 is the table pointer
  read at `0x0042ac99` and word 2 is the cursor read at `0x0042ac9f`;
- each table's words up to and including `0xff070000` (cap 512), plus 32 trailing words;
- the `PTR_005f7638` screen-to-table array (34 dwords).

The dump agent has no Interceptor. The input lane adds one entry/leave probe on `0x0042ac90`. Its only caller is
`0x0043e4ca`, so the probe fires once per select, not once per frame.

| Lane | File | How screens were reached |
|---|---|---|
| push | `dump_push.json` | From the root menu (depth 1): `FUN_0043d2a0(id,0)` for every id 0..33, dump, then `FUN_0043d2a0(0,1)` pop. This is the mechanism `orig_nav_hold.py --scan` already uses. Every id went to depth 1 -> 2 and popped back to 1, all in one boot. |
| input | `dump_input.json` (route 0,0,0, first version, no probe), `dump_input_r000.json`, `dump_input_r00m1.json`, `dump_input_r10000.json` | `statenav.py` drives the in-process `FUN_00497310` override (4 = confirm, 11 = up, 12 = down). Title -> depth 2 -> past the load modal -> depth 3. At each level it steps the cursor down until it repeats and up once from item 0, dumping after every press, then selects the route's cursor. |

**Screens reached:** all 33 non-null ids through push (0..26 and 28..33). Id 27 was skipped because
`PTR_005f7638[27] = 0`, both in the file and live. Through the real input path the run reached screens 22 (root,
depth 1), 1 (d2), 2 (d3), 4 (d4), 6 (d5) and 24 (d6, route `1,0,...`). No other screen was attempted by input,
because the brief's question is answered by the push lane: every table was identical across lanes (below).

**Facts the dump establishes:**
- In every dump, each pushed slot's table pointer equals `PTR_005f7638[id]`. The live `PTR_005f7638` equals the file's.
- All 33 live tables are word-for-word equal to the file's `.data` (`MASHED.exe.unpatched`) and to the standalone's
  `kT0..kT33` arrays in `MenuNavSM.cpp`. There are no runtime-built tables on these screens.
- The 512-word cap was never hit. Tables run from 14 to 57 words.
- In all 80 items, the word at T4+2 is the action key: `0xff140000` in 76 items and `0xff050000` in 4. No string-id
  word (T4+1) is a sentinel.
- Cursor values seen live at the top slot: 0 on every screen right after a push. By input: screen 2 {0,1}, screen 4
  {0}, screen 6 {0,-1}, screen 24 {0,1,2,4,5,6,7}. Screen 24 skips 3.
- On screen 6, once the cursor is -1, neither UP nor DOWN moves it off -1 (3/3 runs).
- What the confirm at cursor -1 on screen 6 did next differed between runs. Route `1,0,…`: depth 5 -> 6 (screen 24).
  Routes `0,0,m1` and `0,0,0`: depth stayed 5 and phase `[0x0067eca4]` went 3 -> 0. This is recorded raw. The dump
  does not explain the split.

## Scoring

Tool: `re/tools/u9667_score.py`. Output: `score.json`.

- (a) `orig_meg` transcribes the bytes of 0x0042ac90 (the disasm is in the tool header). Cross-check: its returns over
  the 16 `z_fix_u9660.py` vectors reproduce the run_diff fingerprints (FNV-1a over the 4 LE bytes) in
  `re/analysis/negctl/reearn_20261011/evidence/fix_diff_menu_entry_get.csv` **16/16**.
- (b) `item_action_code` transcribes `MenuNavSM.cpp:432-457`.
- Each table is tested at cursor -1, at every item 0..n-1, and at every live cursor seen. Causes:
  - (1) means cursor == -1.
  - (3) means ItemActionCode returned through its `0xff070000` test.
  - (2) means a word-by-word variant of ItemActionCode would agree with the original.
  - "other" is anything else.

| screen | table | words | items | live cursors | tested | agree | (1) | (2) | (3) | other |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 0x005f6860 | 40 | 5 | 0 | 6 | 5 | 1 | 0 | 0 | 0 |
| 1 | 0x005f6980 | 39 | 5 | 0 | 6 | 5 | 1 | 0 | 0 | 0 |
| 2 | 0x005f6a20 | 30 | 3 | 0,1 | 4 | 3 | 1 | 0 | 0 | 0 |
| 3 | 0x005f6a98 | 25 | 2 | 0 | 3 | 2 | 1 | 0 | 0 | 0 |
| 4 | 0x005f6c68 | 20 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 5 | 0x005f6d58 | 18 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| **6** | **0x005f6da0** | 22 | 1 | **-1**,0 | 2 | 1 | **1 (live)** | 0 | 0 | 0 |
| 7 | 0x005f6df8 | 22 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 8 | 0x005f6e50 | 42 | 5 | 0 | 6 | 5 | 1 | 0 | 0 | 0 |
| 9 | 0x005f6c00 | 25 | 2 | 0 | 3 | 2 | 1 | 0 | 0 | 0 |
| 10 | 0x005f7058 | 31 | 3 | 0 | 4 | 4 | 0 | 0 | 0 | 0 |
| 11 | 0x005f70d8 | 25 | 2 | 0 | 3 | 2 | 1 | 0 | 0 | 0 |
| 12 | 0x005f7190 | 18 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 13 | 0x005f7140 | 20 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 14 | 0x005f71d8 | 17 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 15 | 0x005f6cb8 | 20 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 16 | 0x005f6d08 | 20 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 17 | 0x005f7220 | 31 | 3 | 0 | 4 | 4 | 0 | 0 | 0 | 0 |
| 18 | 0x005f72a0 | 52 | 7 | 0 | 8 | 7 | 1 | 0 | 0 | 0 |
| 19 | 0x005f7458 | 37 | 4 | 0 | 5 | 4 | 1 | 0 | 0 | 0 |
| 20 | 0x005f7598 | 20 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 21 | 0x005f67e8 | 29 | 3 | 0 | 4 | 3 | 1 | 0 | 0 | 0 |
| 22 | 0x005f6900 | 17 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 23 | 0x005f75e8 | 19 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 24 | 0x005f7370 | 57 | 8 | 0,1,2,4,5,6,7 | 9 | 8 | 1 | 0 | 0 | 0 |
| 25 | 0x005f6944 | 14 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 26 | 0x005f7000 | 22 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 28 | 0x005f6b68 | 37 | 4 | 0 | 5 | 4 | 1 | 0 | 0 | 0 |
| 29 | 0x005f6b00 | 25 | 2 | 0 | 3 | 2 | 1 | 0 | 0 | 0 |
| 30 | 0x005f7540 | 21 | 1 | 0 | 2 | 2 | 0 | 0 | 0 | 0 |
| 31 | 0x005f6ef8 | 37 | 4 | 0 | 5 | 4 | 1 | 0 | 0 | 0 |
| 32 | 0x005f74f0 | 20 | 1 | 0 | 2 | 1 | 1 | 0 | 0 | 0 |
| 33 | 0x005f6f90 | 27 | 2 | 0 | 3 | 2 | 1 | 0 | 0 | 0 |
| **total** | | | **80** | | **113** | **83** | **30** | **0** | **0** | **0** |

Screens 10, 17 and 30 agree at -1 because item 0's action code there is `0xffffffff`, so both bodies return
`0xffffffff`. The other 30 tables at -1 differ, with the original returning item 0's code. Per-table values are in
`score.json` under `fires`.

**Why (2) and (3) cannot fire on these tables.** `ItemActionCode` steps T4, id, then key. Every item has its key at
T4+2, so the pair step lands on the key on the first try. It never reaches `0xff070000` while it is inside an item.

**orig(-1) == orig(0) on 33/33 tables.** At cursor -1 the original's final scan starts at word 0 (`0x0042acd2`).
None of the header words before the first T4 is T5, T14 or T6. The scan does not stop on T4 (`0x0042acf5` tests T5
only, `0x0042ace0`/`0x0042ace8` test T14/T6), so it reaches item 0's key.

## Live 0x0042ac90 calls (16, three input runs)

The transcription matches the live return on **16/16** calls. ItemActionCode matches on 13/16. The 3 misses are the
screen-6 cursor -1 calls.

| screen (depth) | table | cursor | live return | orig_meg | ItemActionCode |
|---|---|---|---|---|---|
| 22 (1) | 0x005f6900 | 0 | 0xff4b0000 | = | = |
| 1 (2) | 0x005f6980 | 0 | 0x00000002 | = | = |
| 2 (3) | 0x005f6a20 | 0 | 0xff3d0000 | = | = |
| 2 (3) | 0x005f6a20 | 1 | 0xff4d0000 | = | = |
| 4 (4) | 0x005f6c68 | 0 | 0xff1d0000 | = | = |
| **6 (5)** | **0x005f6da0** | **-1** | **0xff260000** | = | **0xffffffff** |
| 24 (6) | 0x005f7370 | 0 | 0xff420000 | = | = |

Routes `0,0,0` and `0,0,m1` each produced 5 calls: depths 1, 2, 3 (cursor 0), 4 and 5. Route `1,0,0,0,0` produced 6:
depths 1, 2, 3 (cursor 1), 4, 5 and 6. That is 16 calls in total.

## The original's select path has no cursor gate before the call

`0x0043e3a0` to `0x0043e4ca` (disasm of `MASHED.exe.unpatched`) does four things. It finds `0xff090000` in the
active table (`0x0043e3b4..0x0043e3ec`). It scans the keys up to `0xff0a0000`, setting EDX for `0xff100000` /
`0xff230000` (`0x0043e420..0x0043e499`). It calls `0x0045b350` and requires EDX != 0 (`0x0043e4a3`). It calls
`FUN_0042ae10` (`0x0043e4b0`), which reads `[0x67eab0]`, `[0x67e7c8]`, `[0x898ab0]`, `[0x7f1a0c]` and the
0x4c-stride bytes at `0x7f1502`, but no cursor. Then it calls 0x0042ac90 at `0x0043e4ca`. The return goes into EDI
and is compared against -1 (EBP) at `0x0043e4fb`.

[UNCERTAIN] Whether some check before `0x0043e3a0` gates on the cursor was not read. The live probe shows that the
call does happen with cursor -1 on screen 6.

## Two standalone details the fix has to account for

- `Nav_Select` returns at `MenuNavSM.cpp:1290` (`s.cursor < 0 || s.cursor >= s.item_count`) before it reaches
  `ItemActionCode`. Correcting `ItemActionCode` alone does not change what the standalone does at cursor -1.
- On screens 6 and 7 the standalone's cursor is a cup-row index that wraps over `trackCount` rows
  (`MenuNavSM.cpp:1073-1080`). In the original, DOWN on screen 6 sets the slot cursor `[0x0067ed40 + 5*0x40]` to -1
  and leaves it there. So the standalone's screen-6 cursor model differs from the original's slot cursor before
  select is ever reached.

## Proposed fix (parent decides)

**Route the standalone through the MenuEntryGet body (one body per RVA).**

1. Split the walk in `MenuNav.cpp` `MenuEntryGet` into a pure function `MenuEntryWalk(const uint32_t* table, int32_t
   cursor)`, using the bytes' semantics: cursor -1 skips the item walk, the loop runs while `ecx != 0`, and the final
   scan stops on T5/T14/T6 only. The .asi export reads the three globals and calls it. `MenuNavSM.cpp:1298` calls it
   with `(s.desc_table, s.cursor)`.
2. Delete `ItemActionCode`. On real tables, the only value this changes is cursor -1, which then returns item 0's
   action code (orig(-1) == orig(0) on 33/33).
3. That change only shows up at runtime if the select gate at `MenuNavSM.cpp:1290` lets cursor -1 through, and if the
   screen-6/7 cursor model can produce -1 the way the original does. Both are outside 0x0042ac90.
   [UNCERTAIN] What `FUN_0043dfd0` does with the action at cursor -1 beyond the call, for example any avail/disabled
   test, was not read. Making the standalone's select reproduce the original at -1 needs that section of
   `FUN_0043dfd0` read first.

The live-ret table above gives a ready-made vector set for path1 on the shared body: 7 distinct (table, cursor) pairs
taken from real game state.

## Files

- `re/frida/u9667_menu_table_dump.py`: capture (push and input lanes, plus the 0x0042ac90 probe).
- `re/tools/u9667_score.py`: offline scorer and the 16-vector self-test.
- `re/analysis/u9667_menu_tables/dump_push.json`, `dump_input*.json`: raw dumps.
- `re/analysis/u9667_menu_tables/score.json`: per-table, per-cursor results and the live-call check.

Reproduce: `py -3.12 re/frida/u9667_menu_table_dump.py --lane push --out <json>`, `... --lane input --route 0,0,m1
--out <json>`, then `py -3.12 re/tools/u9667_score.py`. The scorer reads every `dump_*.json` in this folder.
