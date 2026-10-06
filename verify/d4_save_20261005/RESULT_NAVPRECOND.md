# RESULT — the save load is NOT screen-driven. It is a state machine, and that ends the nav search.

**RAN 2026-10-05.** Static read via `re/tools/decomp_pc.py` (read-only pool slot, Ghidra MCP
unreachable). **No C-level moved, no band moved, no code changed, `original/` untouched.**

---

## 1. The precondition, found

`FUN_00404e50` (`SaveLoad`) is called from **`switchD_00409b0e::caseD_d`** — a case of a **switch
dispatched at `0x00409b0e`**:

```c
void switchD_00409b0e::caseD_d(float param_1) {
  iVar1 = FUN_00404e50();                       // SaveLoad
  if (iVar1 == 0) {
    if (_DAT_005cc31c < _DAT_008a95a8) {
      FUN_0042c1a0();
      DAT_008a9588 = 0xf;                       // advance to the next state
      goto LAB_00409df6;
    }
    FUN_00409a80();
    _Format = "Waiting for time to elapse\n";
  } else {
    _Format = "Waiting for load to complete\n";
  }
  FID_conflict__wprintf(_Format);
LAB_00409df6:
  _DAT_008a95a8 = _DAT_008a95a8 + param_1;      // param_1 is a time delta
  return;
}
```

`SaveWrite` (`0x00404f50`) is called from the sibling case `caseD_e` at `0x00409db8`, plus two more
sites at `0x0040a082` and `0x0040a8c2`.

**So the save read is driven by a STATE VARIABLE, not by screen depth.** The strings
*"Waiting for load to complete"* and *"Waiting for time to elapse"* are the classic
memory-card/save-progress flow, ticked with a time delta.

**That ends the nav search.** No amount of pushing frontend screens will trigger the load, because
the trigger is not a screen — it is entering this state machine. Four legs of address-chasing and
two of nav-scanning were looking in the wrong dimension.

## 2. A Ghidra artefact I nearly reported as a finding

`--callers` on all three save functions returned **`(none)`** — which read as "the original never
reads its own save file", and I came close to writing that down.

**It is an artefact.** `--datarefs` on the same addresses shows the calls plainly:

| target | call sites |
|---|---|
| `0x00404e50` `SaveLoad` | `0x00409ccc`, `0x0040a30c` |
| `0x00404f50` `SaveWrite` | `0x00409db8`, `0x0040a082`, `0x0040a8c2` |

Ghidra reports them as *"in (data)"* — the call sites sit in regions it never defined as functions,
so the caller list is empty while the references exist. This is the known gap recorded in memory
`ghidra-missed-functions-repaired` and `transient-create-reads-ghidra-missed-functions`;
`decomp_pc.py --create` reads them fine.

**The lesson, which is the same one three times today: the second instrument corrected the first.**
`--callers` alone would have produced a confident, wrong, high-impact claim.

## 3. What this implies for the standalone's four ported functions

The four `GameSave` functions wired into `mashed_re.exe` earlier today
(`RESULT_GAMESAVE_EXE.md`, `RESULT_WIRE.md`) are faithful transcriptions of this cluster. **What was
NOT ported is their driver** — the `0x00409b0e` state machine. In the standalone they are called
from `Race/GameFlow.cpp` instead, which is a **port-side substitute for that state machine, not a
port of it**. That is a deviation worth recording explicitly, and it is new information: when those
functions were wired, the original's caller was unknown.

**No C-level changes.** The four rows' C4 was earned on the `.asi` against the original's callees,
and nothing here touches that. But a future C4 claim for the exe copy would have to reckon with the
fact that the original reaches these functions through a timed state machine and the port reaches
them from a progression hook.

## 4. The save-acceptance question, and the honest state of it

**Still unanswered** — but the search is now correctly aimed for the first time today. The blocker
sequence, each step measured:

1. `0x00803358` — wrong address class (transient buffer). VOID.
2. `*0x008a94a8` — superseded, never run.
3. `0x007F0A40`, race-warped — right address, wrong harness. VOID on a degenerate control.
4. `0x007F0A40`, menu hold — right harness, file still not read.
5. nav scan — **a phantom**; push works, my reading lacked a baseline.
6. **Now: the trigger is `DAT_008a9588` / the `0x00409b0e` state machine, not a screen at all.**

**The next step is a state poke, not navigation:** drive `DAT_008a9588` into the load case with the
arm P save in place and `0x00803358` as the witness. That is contrived state (C3-grade, the same
class as `scenario_launch.py`'s existing `pokeLap` / `pokeCtrlSlots`) and must be declared as such,
because it proves the file *can* be read and parsed — not that the game reaches that state on its
own.

## 5. Not established

- Whether the original accepts the standalone's save. Unchanged.
- What enters the `0x00409b0e` state machine, and from where. Its callers were not traced.
- Whether `DAT_008a9588` is the only state input. It is the variable this case writes; no claim is
  made that it is the sole gate.
- No name is given to `_DAT_005cc31c`, `_DAT_008a95a8`, `FUN_0042c1a0` or `FUN_00409a80` beyond what
  the decompilation shows.
