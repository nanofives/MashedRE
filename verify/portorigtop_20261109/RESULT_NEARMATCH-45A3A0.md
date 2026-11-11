# RESULT NEARMATCH-45A3A0 (2026-11-10)

Verdict: **GREEN**. On the MASHED_ORIG_TOP path, 0x0045a3a0 now runs its unedited near-match source
`re/match/s09/0045a3a0.c` (best 81.0%), in place of the POWERUPINIT-C logging stand-in. Admission rests on L-117
(Mariano), applying L-115 to this one function. The byte table is in `RESULT_L115-REGCHOICE-45A3A0.md`. The residual is
register allocation: the original keeps param_1 in EBP from entry, ours reloads it into EAX after the loop. Port-only:
no tracker C-level change.

## Change

- `mashedmod/src/mashed_re/Match/PortTop_0045a6b0.cpp` (exe-only, `// RVA-LINT: ORIGTOP-ONLY`):
  - It includes `re/match/s09/0045a3a0.c` unedited, under `#define FUN_0045a3a0 Orig_0045a3a0`.
  - It then includes `re/match/s29/0045a6b0.c` with its callee bound to `PortTop_PWI_Call0045a3a0`. That forwarder
    passes the record pointer to `Orig_0045a3a0((int *)r)`.
  - Why a forwarder: the s09 source takes `int *` and 0045a6b0.c declares `Rec_0045a6b0 *`. Two `extern "C"` prototypes
    of one name with different types do not compile in one C++ TU.
  - The forwarder logs its first 4 calls.
  - A default-off evidence knob, `MASHED_DBG_45A3A0_POISON=1`, fills the ten cleared fields with `0xCDCDCDCD` before the
    call. The table is zero-filled at boot, so without it the clear cannot be seen.
- `verify/porttop_20261108/BINDINGS.tsv`: 0x0045a3a0 is `stand-in` -> `near-match` (`Orig_0045a3a0 (via
  PortTop_PWI_Call0045a3a0)`), and 0x0045a6b0's note gets a suffix. It was written by
  `verify/portorigtop_20261109/update_bindings_nearmatch45a3a0.py` through `bindings_rows.apply`, the no-chain
  convention from HB-GENFILES-NOCONFLICT.
- `hooks.csv` port column:
  - `backfill_exe_file.py` gave 0x0045a3a0 `exe_file = mashedmod/src/mashed_re/Match/PortTop_0045a6b0.cpp`.
  - `backfill_port.py` moved 0x0045a3a0 from `none` to `called`, with totals none=5091 body=552 called=526. No other row
    changed.
  - C-level stays C2.

**Default path.** The body is reached only from the matched 0x0045a6b0, which runs only on MASHED_ORIG_TOP. The
default-path gates below are unchanged.

## Evidence: the table clear and record +0x0c = 1.0f now happen

The `[ORIG_TOP] rva 0x0045a3a0` lines, 4 calls per boot, records 0x0068BC98 / BC30 / BBC8 / BB60 (the 0x68 stride from
0x0068bb60):

| boot (`knob_boot.py`, `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | rc | cleared-field nonzero before -> after | +0x00 / +0x0c / +0x14 / +0x18 |
|---|---|---|---|
| RWMOD off (`knob_on.log`:383-392) | 0, `WinMain returned 0 after 3000 frames` | 0 -> 0 | 0 / `3F800000` / 0 / 0 |
| RWMOD on (`knob_on_rwmod.log`:444-453) | 0, 3000 frames | 0 -> 0 | 0 / `3F800000` / 0 / 0 |
| RWMOD off + `MASHED_DBG_45A3A0_POISON=1` (`knob_on_poison.log`:439-448) | 0, 3000 frames | **1000 -> 0** (100 entries x 10 fields) | 0 / `3F800000` / 0 / 0 |

Under the stand-in, +0x0c stayed 0. Now it is `0x3f800000` = 1.0f. Every field the s09 body writes is zero after the call:
fields 0..6, 8, 11 and 12 of all 4 x 5 x 5 0x34-byte entries at 0x0068bd00. Logs are in `D:/MashedWT/NEARMATCH-45A3A0_data/`.

## Gates

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `.asi` not deployed. mashed_re.exe sha256 `9a23857b3c1556101d9beb7f0e3189c4d9b2ff4cd72c5667c444c4a9511e2434` |
| 12 captures, RWMOD off (`capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12` |
| 12 captures, RWMOD on (arm `nav_rwmod`, `MASHED_ORIG_TOP_RWMOD=1`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12` |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new |

Launch setup:
- Every launch ran inside one `test-slot bash -c 'mashed_lock.py run --label NEARMATCH-45A3A0 -- ...'`.
- The build waited for the merge train's priority-linger lock.
- Roots: `D:/MashedWT/POWERUPINIT-C_root` for boots and captures, `_rootA` for gates.

Build incident:
- The first build attempt failed with `user.obj: Permission denied`. A TaskStop'd wait loop of mine survived and started
  a second build.bat at the same moment.
- The stray build ran without `MASHED_NO_DEPLOY`. It deployed nothing shared: this worktree's `original\` is a plain
  directory with no `.asi`, and the shared `original\mashed_re_dev.asi` is unchanged (2026-10-24).
- The single rebuild above is the measured one.
