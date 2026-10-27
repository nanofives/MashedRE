# RESULT D4-FU-24EFC-WRITER (2026-10-27): the first-run writer of gamesave `0x24efc` is `0x00409e30`

Base `orch/integration` @ `1158f5c1c` (contains D4-FU-SETTRACKPU `8aa896b31`, checked with `git merge-base --is-ancestor`).
Static only, on `original/MASHED.exe.unpatched` with `re/tools/disasm_fn.py` and a byte scan of `.text`. No game run,
no build: the one source change is a comment. Dynamic witness reused from D4-FU-SETTRACKPU's W arm.

## Verdict: GREEN

The `[UNCERTAIN]` in `verify/wsf24efc_20261026/RESULT_D4-WSF-SAVE-24EFC.md` (Residual: "`0x00409e30` or `0x0040ad34`,
no --watch-write run on the first-run boot") is closed. The writer is **`0x00409e30` `MOV dword ptr [0x008a95ac],1`**,
case 16 of the save state machine. `0x0040ad34` (`FUN_0040ad30`) is excluded. There is no UNCERTAINTIES.md row for this
item (grep for `24efc`, `8a95ac`, `409e30`, `40ad34` in UNCERTAINTIES / STUBS / DEFERRED / CHANGELOG: only U-1551,
resolved, unrelated), so no tracker change is owed and re-classify was not run. No hooks.csv status moves.

## Evidence 1, dynamic (D4-FU-SETTRACKPU W arm, reused)

`verify/settrackpu_20261026/watch_hits_summary.txt`: hardware write watch on `0x008a95ac` over the original's first-run
boot (no gamesave.bin) to mode select, 85 threads covered. Two hits on that dword, in this order:

| seq | pc (after store) | store | value | return frames |
|---|---|---|---|---|
| 751 | `0x004098cb` | `0x004098c6` in `FUN_004098b0` | 0 | `0x0043f9f6`, `0x004030f9` |
| 1511 | `0x00409e3a` | **`0x00409e30`** | **1** | `0x0043e06e` |

No other writer fired, `0x0040ad34` included. `[0x008a95ac]` = 1 before the probe's save, and that save (`O_W` == `O_0`)
holds 1 at `0x24efc`. Raw: `D:/MashedWT/D4-FU-SETTRACKPU_data/W_probe.log`
sha256 `1bc00d746b56e05842a5adccdb56bc6f79ddeb5bca58689888cc1f1e3c7770b7`.

## Evidence 2, static: how `0x00409e30` is reached

1. `FUN_0043dfd0` `0x0043e062..0x0043e069`: `push [0x007f1004]`, `call 0x0040acd0`. Return address `0x0043e06e` = the
   watch frame.
2. `FUN_0040acd0` `0x0040acd0..0x0040acdb`: `eax = [0x008a9584] - 1`, `cmp eax,5`, `ja 0x0040ad00` (ret), else
   `jmp [eax*4 + 0x0040ad04]`. Entry 0 (`[0x008a9584]` == 1) = `0x0040ace2` `jmp 0x00409b00`.
3. `0x00409b00..0x00409b0e`: `eax = [0x008a9588]`, `cmp eax,0x10`, `ja 0x00409ba1` (ret), else `jmp [eax*4 + 0x00409e40]`.
   Ghidra has no function covering `0x00409b00`. The only reference to the table `0x00409e40` is at `0x00409b11`.
   Table (17 entries, byte read): 0 `0x409e3b`, 1 `0x409b15`, 2 `0x409b5f`, 3 `0x409bc3`, 4 `0x409c26`, 5 `0x409c3a`,
   6 `0x409e3b`, 7 `0x409c69`, 8 `0x409e3b`, 9 `0x409c8e`, 10 `0x409d3f`, 11 `0x409e3b`, 12 `0x409d6a`, 13 `0x409ccc`,
   14 `0x409db8`, 15 `0x409d15`, **16 `0x409e07`**.
4. Case 16 `0x00409e07..0x00409e3a`: `FUN_0042bfb0(0x1b7, 0xff460000, 0xb, 0x2d, 0, 0)`, `call 0x0045b350`,
   `MOV [0x008a9588],0x11` `0x00409e26`, **`MOV [0x008a95ac],1` `0x00409e30`**, `ret`.

So the store runs when `[0x008a9584]` == 1 and `[0x008a9588]` == 0x10 at the `0x0043e069` call.

## Which states lead there (byte scan of every `.text` store to `0x008a9588`, 25 refs = 19 `c7 05` + 3 `a3` + 3 reads)

- State 0x10 is stored at **one** site only: `0x00409ddd`, in case 14.
- Case 14 `0x00409db8`: `call 0x00404f50`. `FUN_00404f50` = `FUN_004b3bb0(0x005cc8e0 "gamesave.bin", 0x00803358, 0x24fa0)`,
  `FUN_004099e0(0)`, returns 0 (`xor eax,eax` `0x00404f6e`). Then `fld [0x008a95a8]`, `fcomp [0x005cc31c]`,
  `test ah,0x41`: nonzero goes to `0x00409cfc` ("Waiting for time to elapse" `0x005ccb98`), zero calls `0x0042c1a0` and
  stores state 0x10 (`0x00409ddd`). Both fall to `[0x008a95a8] += [esp+4]` (`0x00409df6..0x00409e00`).
- State 14 (0xe) is stored at one site only: `0x00409d92`, in case 12 (`0x00409d6a`): if `FUN_004d8560(0)` != 0,
  `FUN_0042bfb0(0x233, ...)`, state 0xe, `[0x008a95a8] = 0`, then `jmp 0x00404ee0` (Serialize, which copies
  `[0x008a95ac]` to `0x00828254` at `0x00404f03` / `0x00404f23`). If 0, `[esp+4] = 3`, `jmp 0x004099f0`.
- State 12 (0xc) is stored at `0x004098dc` (`FUN_004098d0`, with `[0x008a9584] = 1` `0x004098d2` and
  `[0x008a95ac] = 0` `0x004098eb`) and at `0x0040aa93` (dispatcher `0x0040aa60`, byte table `0x0040aab4`, called from
  `0x0040ab52`).
- The first-run entry: `FUN_004098b0` (called `0x0043f9f1` / `0x0043fa40`) stores `[0x008a9584] = [0x008a9588] = 1`,
  `[0x008a95b0] = [0x008a95ac] = 0`. That is watch hit 751.

Order on this path, from the code: case 12 Serialize (`0x00409da6`), case 14 write (`0x00409db8`), state 0x10
(`0x00409ddd`), case 16 store 1 (`0x00409e30`). The write at `0x00409db8` comes **before** the store.

Consequence (watch + code, not separately measured): between hit 751 (0) and hit 1511 (1) no store to `0x008a95ac`
fired, so the gamesave.bin written by case 14 on the first-run boot carries `0x24efc` = 0. The 1 reaches the file only
on a later save (here the probe's save at mode select, `O_0`). The boot-written file itself was overwritten by that save
and was not captured.

## Excluded writers (from the 9-reference enumeration in RESULT_D4-WSF-SAVE-24EFC)

| site | why not the first-run 1 |
|---|---|
| `0x00404ec1` (Deserialize, `= [0x00828254]`) | no file to load on a first run, and it did not fire |
| `0x004098c6` / `0x004098eb` / `0x00409c5e` | store 0 |
| `0x0040ad34` (`FUN_0040ad30(param)`, callers `0x0043fda6` / `0x0043ffce` / `0x0044050d`) | did not fire in the W arm |
| **`0x00409e30`** | **the writer**, value 1 |

## Still `[UNCERTAIN]` (not needed to cite the writer)

- Which state sequence the W boot took from state 1 to state 12. Case 2 (`0x00409b5f`) branches on `FUN_004d8560(0)`
  (twice) and `FUN_00404f80(0)` (= `FUN_00550b00(0x005cc8e0 "gamesave.bin") != 0`, `0x00404f80..0x00404f93`) to
  states 4, 9, 10 or 5. State 10 (`0x00409d3f`) shows message `0x2a3` with `0x285` / `0x286` and goes to 11, whose
  table entry `0x00409e3b` jumps to `0x00409a80`. The 11 to 12 step through `0x0040aa60` was not decoded, and
  `FUN_004d8560` / `FUN_00550b00` were not read. Missing evidence: a decode of `0x0040aa60` + its byte table, or a
  watch on `0x008a9588` over the same first-run boot.
- The gather-slot `[UNCERTAIN]` (what writes `0x7e96fc + i*0x200`) from D4-FU-SETTRACKPU is a separate item, not touched.

## What this means for the port (not done here)

`GameFlow.cpp` stores the live `g_saveCounter` (0 on a first run). To match `0x24efc` on the original's first-run save
sequence the port needs both halves: the first-run creation write keeps 0, and `g_saveCounter = 1` after it (case 16),
so the next save writes 1. That is D4-FU-SETTRACKPU's Next item 1, as a default-OFF knob with its own PREREG.

## What changed

- `mashedmod/src/mashed_re/Race/GameFlow.cpp`: comment at the `BuildImage` call naming `0x00409e30` as the first-run
  writer (comment only, no code). No build or gate run was needed for a comment change.
- `verify/fu24efc_20261027/RESULT_D4-FU-24EFC-WRITER.md`: this file.

`original/` untouched, `original/mashed_re_dev.asi` not redeployed, no game launched, no trackers edited.
