# RESULT PORT-TOP-AUDIOTICK-IMGTRAP-4568A0 (2026-11-14, run 46, section D4)

Verdict: GREEN. Top-down row (knob path only).

The slot 0x005f99b0 now points at an exe body for 0x004568a0. Before this row it pointed at a stop thunk. With
`MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1`, both AUDIOTICK control arms now pass 0x004568a0.
They stop one entry later in the same table, at **0x004540C0, slot 0x005F99F0**. That stop is filed as its own open
row, `PORT-TOP-TEARDOWN-TABLE-5F99B0`.

**Self-test results (both arms, both builds):**
- `[OT_TIMER]` 600 calls, `checks 8413 failed 0` at call 600. The arms reach AppDestroy (0x00402a40) after the 600
  frames, so the stop happens during teardown, not in the tick.
- `[OT_CODEPTR] ... self-test PASS (0 of 359 checks failed)`, 188 slots, now 11 bound to an exe body (10 before) and
  177 to a stop thunk (178 before). Image slots: 99 written, 0 refused.
- `[portstate-image] ... self-test PASS (0 of 8 checks failed)`.
- `[code-trap] self-test PASS (0 of 6 checks failed)`.

Nothing on the default path changed. The new TU is exe-only (`MASHED_STANDALONE`) and marked `// RVA-LINT: ORIGTOP-ONLY`.
The slot write happens only in `PortTopCodePtr_Apply`, which needs `MASHED_ORIG_TOP=1` and `MASHED_OT_CODEPTR=1`.

No C4 claim. 0x004568a0 was already C4 by route match (hooks.csv, verify/c4rmatchbatch_20261107), and there is no
promotions TSV.

## What 0x005f99b0 is (from the sources)
- `re/match/s09/0045b930.c` (C4 match) loops `for (i = 0; i < DAT_005f9bd8; i++) if (PTR_FUN_005f99b0[i].m_fn) m_fn();`.
  The entries are 0x40 bytes long, function pointer first.
- `DAT_005f9bd8` is 9 in `MASHED.exe.unpatched`, and the AppDestroy log line prints `DAT_005f9bd8=9`.
- Slot values read from the file, and how this branch binds each one:

| i | slot | file value | binding on this branch |
|---|---|---|---|
| 0 | 0x005f99b0 | 0x004568a0 | `Orig_004568a0` (this row) |
| 1 | 0x005f99f0 | 0x004540c0 | not bound (CODEPTR_IMAGE.tsv `bind=trap`) |
| 2 | 0x005f9a30 | 0x004555d0 | not bound (trap) |
| 3 | 0x005f9a70 | 0x00457b10 | not bound (trap) |
| 4 | 0x005f9ab0 | 0x0045a790 | not bound (trap) |
| 5 | 0x005f9af0 | 0x0045b2d0 | not bound (trap) |
| 6 | 0x005f9b30 | 0x00454af0 | not bound (trap) |
| 7 | 0x005f9b70 | 0x00456ca0 | not bound (trap) |
| 8 | 0x005f9bb0 | 0x004530b0 | not bound (trap) |

Slot 0 is in `Compat/PortTopCodePtr.inc` (generator `verify/portorigtop_20261109/codeptr_inventory.py`, 22-entry list,
entry 0x005f9998). Slots 1..8 are in `re/tools/image/CODEPTR_IMAGE.tsv` (IMG-4), and `codeptr_image.py` writes them
only when the target has an exe body.

## Change
- `re/match/s09/004568a0.c`: `FUN_004768c0(&DAT_006887d0)` -> `FUN_004768c0((int)&DAT_006887d0)`. This is the only
  edit, a cast so the file compiles as C++.
  - Re-checked: `match.py --bytes` gives 100.0%, 14/14 ins, 43 B, BYTE-IDENTICAL. `relocheck.py` gives OK checked=4.
  - The src sha256 recorded in the hooks.csv C4 note (`dd8fc8310dc7bf9d`) is now stale. The new value is
    `cdf01c9d1e54c052`. The byte-match verdict is unchanged. hooks.csv is not edited on a branch (HB-TRACKER-AT-MERGE).
- `mashedmod/src/mashed_re/Match/PortTop_004568a0.cpp` (new) includes the source as `Orig_004568a0`. Its callees bind
  by name:
  - 0x004e6e00 binds to `MashedRw_ClumpDestroy_004e6e00`. This is the existing binding, so no new librw binding was
    added (L-124).
  - 0x004768c0 binds to `Orig_004768c0` (matched).
- `verify/porttop_20261108/BINDINGS.tsv`: +1 row (0x004568a0 matched Orig_004568a0, level 3, caller 0x0045b930).
- `mashedmod/exe_sources.rsp`: +1 TU.
- `codeptr_inventory.py --write` regenerated two files:
  - `Compat/PortTopCodePtr.inc`: `CP_FIX(0x005f99b0, Orig_004568a0, 0x004568a0)`, the `CP_STOP` for 0x004568a0 removed,
    `CP_EXTERN(Orig_004568a0)` added.
  - `verify/portorigtop_20261109/CODEPTR_SLOTS.tsv`: the same slot. No other line moved.

## Builds and checks
- **VS2022** (`MASHED_NO_DEPLOY=1`, `MASHED_ROOT` = private root): Build OK.
  - rva-lint OK, 301 known, NEW 0.
  - `mashedmod/build/mashed_image.bin` sha256 `81CC7775...9AFCD8`, the same as IMG-1's.
  - Log `D:/MashedWT/PORT-TOP-AUDIOTICK-IMGTRAP-4568A0_build_vs.log` (sha256 4c903cf1d35ea8d6...).
- **Object build** (`MASHED_DDKOBJ=1`): Build OK. Log `D:/MashedWT/PORT-TOP-AUDIOTICK-IMGTRAP-4568A0_build_ddk.log`
  (261596bc5d1b8883...).
- **`ddkobj.py check`**: IDENTICAL 1413/1462, DIFFERENT 34, not-in-map 15, units linked 1406/1686, 4 refusals
  (`wrapper-redefines-globals`, none of them this row's).
  - **0x004568a0 `_Orig_004568a0`: IDENTICAL**, with 2 relocs masked and 0 diff bytes.
  - The DIFFERENT list is the base branch's admitted and provisional set. This row adds no DIFFERENT entry.
  - Log `D:/MashedWT/PORT-TOP-AUDIOTICK-IMGTRAP-4568A0_ddkcheck.log` (704b7979b0cacfa8...).
- No G-CTRL, no B and no captures: this is a top-down row. No menu walk: no menu code changed. No equiv.py run: the body
  is 100% byte-identical.

## Knob boots (control arms)
- Wrapping: `test-slot` outside (not dispatched to the hub; it ran locally after a 600 s memory wait), and
  `mashed_lock.py run --label PORT-TOP-AUDIOTICK-IMGTRAP-4568A0` inside.
- Script `verify/portorigtop_20261113/audiotick_stops_arms.py vs_audio ddk_audio`, with:
  - `ARMS_STEP=PORT-TOP-AUDIOTICK-IMGTRAP-4568A0`
  - `ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIOTICK-IMGTRAP-4568A0_root` (robocopy of `original\`, plus this build's
    `mashed_image.bin`)
  - `ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1"`
  - The arms add `MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 MASHED_DET_FRAMES=600 MASHED_OT_AUDIOTICK=1`.
- Console output: `verify/portorigtop_20261113/imgtrap_4568a0_arms1.out`.

| arm | build | exit | trap line | log sha256 |
|---|---|---|---|---|
| vs_audio | VS2022 | 0x004540C0 after 15.4 s | `execute original 0x004540C0 slot 0x005F99F0 via "mov r,[sib]; test/jcc; call eax" ret mashed_re.exe+0x1736CE` | a8eb005a823b3590... |
| ddk_audio | object | 0x004540C0 after 16.5 s | `execute original 0x004540C0 slot 0x005F99F0 via "mov r,[reg+disp]; test/jcc; call eax" ret mashed_re_ddk.exe+0x5648` | 5b59372d324a8dd0... |

Before this row, `RESULT_PORT-TOP-RWAUDIO-TABLE-632BA8.md` sections 3 and 5 recorded this stop for both arms:
- 0x004568A0 via `call eax` from `_Orig_0045b930` (trap only);
- the `[OT_CODEPTR] STOP` thunk for 0x004568A0 (with `MASHED_OT_CODEPTR=1`).

**The body ran.** In vs_audio's log the teardown sequence between AppDestroy enter and the trap is:
1. the 0x004593b0 / 0x00404820 stand-ins;
2. four `RpClumpDestroy` calls: 10C5AB98, 10C5A508, 10C5A738, 10C5A2A0;
3. `RpAtomicDestroy(PTank record 109B1050)` with its texture release, plus four RwGlobals +0x10c frees. These are
   0x004768c0 on record 0x006887d0.

This matches the source: 4 iterations from 0x006886b8 in steps of 0x48 below 0x006887d8, then
`FUN_004768c0(0x006887d0)`.

Those four clumps are the four `rw::Clump::clone` results of the init side, log lines 368-371: 10C5A2A0, 10C5A738,
10C5A508, 10C5AB98, all cloned from 10C5A1C0. They are destroyed in the reverse of the clone order. 109B1050 is the
PTank record created at line 365. The ddk_audio log shows the same shape: four destroys, then the PTank record.

[UNCERTAIN] This row does not show which 0x48-record each clone was stored in. That needs a read of 0x00456760's store
offsets against the 0x006886b8 base, which was not done here. The run does show that the four destroyed pointers are
the four clones.

## Next (filed, open)
`PORT-TOP-TEARDOWN-TABLE-5F99B0` (D4, wave 8):
- **0x004540c0** (`DepthChargePerFrameTeardown`) is C4 by match (`re/match/s09/004540c0.c`, BYTE-IDENTICAL 109 B,
  hooks.csv), with no exe body yet. To bind it:
  1. write `Match/PortTop_004540c0.cpp` and a BINDINGS.tsv row;
  2. run `re/tools/image/codeptr_image.py --write`. The slot is in CODEPTR_IMAGE.tsv, not in PortTopCodePtr.inc.
- **The 7 later targets have no hooks.csv row, no `re/match` source, and no entry in `re/console/pc_funcs.csv`:**
  0x004555d0, 0x00457b10, 0x0045a790, 0x0045b2d0, 0x00454af0, 0x00456ca0 and 0x004530b0. They need master-Ghidra
  function creation first, as the 0x00632ba8 targets did.

## Files
- `re/match/s09/004568a0.c` (one cast)
- `mashedmod/src/mashed_re/Match/PortTop_004568a0.cpp` (new)
- `mashedmod/exe_sources.rsp`
- `mashedmod/src/mashed_re/Compat/PortTopCodePtr.inc` (generated)
- `verify/portorigtop_20261109/CODEPTR_SLOTS.tsv` (generated)
- `verify/porttop_20261108/BINDINGS.tsv`
- `verify/portorigtop_20261113/imgtrap_4568a0_arms1.out`
- this file
- Raw logs (not committed): `D:/MashedWT/PORT-TOP-AUDIOTICK-IMGTRAP-4568A0_data/vs_audio.log` (sha256
  A8EB005A823B35909A48927C075C8BEECC3CBE19D46F4AD92476C5DD212DA6C6) and `.../ddk_audio.log` (sha256
  5B59372D324A8DD0D4EEED08150BFFD6200DBF4CEAEDBE0F7FF0048DCAA2E2D1).
