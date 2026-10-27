# PREREG D4-WSB-C4-BROAD-A, addendum 1: 0x0055e050 control and field-source tokens

Committed before the arms it names (X1-X3). Written after the Quick Battle racediff of `0x0055e050` returned HOLD
`BEHAVIOR-DIFF` (OFF PID 2368, ON PID 10536, 6 calls each side, 4 signatures each side). That HOLD stands as recorded.

## What differed

Every signature on both sides has the same shape, `[c57c590] [c55ab30,A0+<d>,A0+0,Sloc] eax=A0+0`. Only `<d>` differs:
OFF `ba8 e28 e68 ea8`, ON `1370 1470 16b0 16f0`. That argument is `param_1[9]`, loaded at `0x0055e063`
(`mov eax,[esi+0x24]`) and pushed at `0x0055e06c` for `call 0x0055ab30` at `0x0055e06d`. The port reads the same field
(`RwpSolverGlue5.cpp:119`, `param_1[9]`). v7/v8 name a pointer that lies within `0x4000` above an entry arg by its
distance from that arg, so `<d>` is the heap distance between two separate objects (`param_1` and the object at
`param_1[9]`).

[UNCERTAIN] whether that distance is the same on two boots of the unhooked original. Missing: an OFF-vs-OFF run.

## Arms (Quick Battle, window 300-1200, `C4_TOOL=c4_racediff_v9.py`, `make_tool4.py`)

- **X1** `C4_CONTROL=1`: two boots of the original, v8 tokens.
- **X2** `C4_CONTROL=1 C4_DEREF=1`: two boots of the original, field-source tokens (`A0@24` for `param_1[9]`).
- **X3** `C4_DEREF=1`: hook installed alone (ON) vs original (OFF), field-source tokens.

## Rules

- X1 `CONTROL-SAME` (distance stable across original boots): the v8 BEHAVIOR-DIFF is real. `0x0055e050` stays C3
  whatever X3 shows.
- X1 `CONTROL-DIFF` on `<d>` only: the v8 token does not compare across boots. Then `0x0055e050` is promoted only if
  X2 is `CONTROL-SAME` **and** X3 is `C4-CLEAN`. Any other X2 / X3 outcome keeps it C3.
- One re-run only for both-sides-empty or a U-9677 crash, as in the PREREG.
