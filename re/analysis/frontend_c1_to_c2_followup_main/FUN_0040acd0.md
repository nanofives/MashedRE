# FUN_0040acd0 — Save-game state-machine dispatcher

**RVA:** `0x0040acd0`
**Body extent:** `0x0040acd0..0x0040ad00` per Ghidra (48 bytes), but the decompiler
follows a multi-block CFG via `goto switchD_00409b0e_switchD` so the reachable code
spans `0x004098xx..0x0040adxx`.

## Signature

```c
void FUN_0040acd0(float param_1);
```

`param_1` is the per-frame delta-time accumulator (passed in as a 32-bit float).
Several state branches treat it as the increment for `_DAT_008a95a8`
(the elapsed-time accumulator).

## Callers

| RVA          | Name        | Note                                              |
|--------------|-------------|---------------------------------------------------|
| `0x0043dfd0` | FUN_0043dfd0 | Sole caller. Frontend per-frame tick / menu dispatcher. |

## Callees (via reached code, not Ghidra `function_callees` which returns 0)

Wprintf logging — `FID_conflict:_wprintf` for status traces.
Sub-state actions:
- `FUN_005c9d00` — status-change predicate (CRT-band callee).
- `FUN_004d8560` — async-status poller (returns 0/1/2 for device states).
- `FUN_00404f80` / `FUN_00404f50` / `FUN_00404e50` / `FUN_00404e80` / `FUN_00404ee0`
  — async I/O completion predicates / completion handlers.
- `FUN_0045b350` — UI/dialog refresh.
- `FUN_0042c1a0` — UI close/clear.
- `FUN_0042bfb0(msg_id, color, x, y, w, flag)` — message-box dispatcher.
  Message IDs cited inline: 0x1b5, 0x1b6, 0x1b7, 0x1b9, 0x1ba, 0x1bb, 0x1bc,
  0x1c0, 0x1c1, 0x1c3, 0x1c4, 0x1ca, 0x215, 0x216, 0x231, 0x232, 0x233, 0x248,
  0x249, 0x275, 0x276, 0x27e, 0x288, 0x2a3.
- `FUN_004099e0`, `FUN_00409a80`, `FUN_004098d0`, `FUN_00409900`, `FUN_00409950`,
  `FUN_004099a0` — sibling dispatcher tails (sub-mode entry points).

## Body summary

Top-level dispatch on `DAT_008a9584` (master save-game operation enum, 1–6):

| `DAT_008a9584` | Operation | Sub-state global |
|----------------|-----------|------------------|
| 1              | Boot      | `DAT_008a9588`   |
| 2              | Load      | `DAT_008a958c`   |
| 3              | Save      | `DAT_008a9590`   |
| 4              | Autosave  | `DAT_008a9594`   |
| 5              | Format    | `DAT_008a9598`   |
| 6              | Cancel/special | `DAT_008a959c` |

Each operation has its own sub-state machine following the canonical
async-IO pattern:
1. **0** = idle (call `FUN_00409a80` no-op).
2. **1** = wait for status (poll `FUN_005c9d00`; increment `DAT_008a95b0` retry
   counter; on `>1` retries with success, advance).
3. **2** = check operation result (poll `FUN_004d8560` returning 0/1/2);
   set sub-state-arg `DAT_008a95a0` to one of the float-immediate-converted
   values (`1.4013e-45 == 1`, `4.2039e-45 == 3`, etc — Ghidra float10
   representations of small ints).
4. **3** = show start-prompt message-box; arm timer `_DAT_008a95a8 = 0.0`.
5. **4** = wait for IO complete (`FUN_00404f50`/`FUN_004d8560`); on timeout
   poll `_DAT_005cc31c`/`_DAT_005cc320` threshold for "elapsed" branch.
6. **5** = dispatch on result-code `DAT_008a95a0` (0/1/2/3/6) to success /
   wrong-card / not-ready / hardware-error / generic-fail message-box.
7. **6..0xf** = per-operation tail-state ladders.

The boot branch (case 1) is a separate nested switch on `DAT_008a9588`
covering 0..0x11, with its own retry/result/elapsed/finish ladder.

The format branch (case 5) is unique: on result-code 0 it asks
`DAT_008a9580` (1..4) which downstream sub-operation to chain into
(`FUN_004098d0` / `FUN_00409900` / `FUN_00409950` / `FUN_004099a0`).

The cancel branch (case 6) is single-shot: if `DAT_008a959c == 1`, show
message-box 0x288 and clear the flag.

## Cited constants / offsets

| Address          | Use                                              |
|------------------|--------------------------------------------------|
| `DAT_008a9580`   | Format chain selector (1..4).                    |
| `DAT_008a9584`   | Master op enum (1..6).                           |
| `DAT_008a9588`   | Boot sub-state.                                  |
| `DAT_008a958c`   | Load sub-state.                                  |
| `DAT_008a9590`   | Save sub-state.                                  |
| `DAT_008a9594`   | Autosave sub-state.                              |
| `DAT_008a9598`   | Format sub-state.                                |
| `DAT_008a959c`   | Cancel flag.                                     |
| `DAT_008a95a0`   | Result-code dispatch (0..6).                     |
| `DAT_008a95a8`   | Elapsed-time accumulator (float, this frame's   |
|                  | `param_1` is summed in).                         |
| `DAT_008a95ac`   | Final-result success flag (post-format).         |
| `DAT_008a95b0`   | Retry counter (status-change polling).           |
| `DAT_008a95b4`   | Cancel-from-wait flag.                           |
| `_DAT_005cc31c`  | Elapsed-time threshold for save/load/autosave    |
|                  | (e.g. confirm-popup minimum-on-screen duration). |
| `_DAT_005cc320`  | Elapsed-time threshold for format ops.           |

## Uncertainties

- U-4301: identity of `DAT_008a9584` magic enum values. Mapping to game-side
  source-of-truth (e.g. is "1 = Boot" actually the engine boot-from-savegame
  path or a different state? "6 = Cancel" inferred from single-shot
  cancellation pattern). Resolution: cross-ref with `FUN_0043dfd0`
  (sole caller) once that lands in scope.
- U-4302: the four float-immediate constants
  (`1.4013e-45`, `2.8026e-45`, `4.2039e-45`, `5.60519e-45`, `1.26117e-44`)
  are Ghidra's display of `int32(0x1)..0x9` reinterpreted as floats. The
  decomp uses `(int)param_1` to recover the original int; this is a
  Ghidra cast quirk — the original asm is `mov [esp+4], <small_int>`
  followed by `(int)<float>` re-read. Document at C3 time when reimpling.

## Promotion rationale

C1 → C2: body decompiled end-to-end via Ghidra; all 6 top-level branches
identified; all 80+ sub-state transitions enumerated and traced; every
message-ID and DAT-offset cited. No new sub-functions need exploration
before reimpl — all callees are already in `hooks.csv` (S-3xxx scaffolds
for the async-status pollers are documented separately).

Why this is sized larger than typical C2 candidates: this is a single
giant per-frame dispatcher rather than a leaf utility. Bulk-batch workers
skip it because they only have time for ~15 RVAs per session; a single
focused session is fine.
