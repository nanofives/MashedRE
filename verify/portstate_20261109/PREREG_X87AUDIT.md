# PREREG PORT-X87-PC24-AUDIT: which float mode reproduces each matched x87 wrapper

Committed alone, before any arm. Follow-up of PORT-STATE-03-FPU (`verify/portstate_20261108/RESULT_PORT-STATE-03-FPU.md`,
L-102: whichever of SSE2 and x87 matches the original wins). Lead: cmw7xu1960f37p41557tefpnd.

## 1. Question and scope

For every **matched** wrapper in `mashedmod/x87_tus.txt` (a TU that `#include`s a `re/match/**.c` byte-matched body,
MATCH-S08 first): the original's x87 control word at entry, and which of **x87 @ PC_53** (the exe today),
**x87 @ PC_24** and **SSE2** reproduces the original's values. Inventory: `x87_inventory.tsv` (`x87_inventory.py`),
168 (TU, RVA) rows. Rows with `how = match:*` are in scope (the deliverable table). The transcription TUs
(`RwpSolver*`, `CameraEntries`, `PathNodes`, `ProgressBarSetA`, `RwpSolverLeaves1X87`, the `install`/`comment` rows)
get the CW measurement only and are listed separately, with no recommendation.

Control row (in the images, not in `x87_tus.txt`): `0x004325c0` (`MatchS08_004325c0`), the PORT-STATE-03-FPU
function. Known answer: original CW 0x007f, SSE2 = original, x87 @ PC_53 differs.

Known before the arms (code reading, not an arm): the exe creates its D3D9 device on a watchdog `std::thread`
(`mashedmod/src/mashed_re/exe_main.cpp` `CreateDeviceWatched`, `std::thread(...)` at line 3191, no
`D3DCREATE_FPU_PRESERVE` anywhere in `mashedmod/src`). D3D9's FPU setup therefore runs on that worker thread, and the
game thread keeps the CRT default 0x027f. The original creates its device on the game thread.

## 2. Arm 1, CW at entry (`x87_cw_probe.py`)

The PORT-STATE-03-FPU arm-1 recipe on every inventory RVA at once: a native onEnter stub per function (FNSTCW, no JS on
the call path), counting 0x007f / 0x027f / other per function.

- **orig:** `D:/MashedWT/PORT-X87-PC24-AUDIT_root` (robocopy of `original\`, every `*.asi` left out), hooks OFF.
  Stubs placed before resume. Windows: boot to title, title to the race confirm (record_race.py Quick-Battle recipe),
  40 s of race. 2 boots.
- **exe:** the worktree's `mashed_re.exe` (built `MASHED_NO_DEPLOY=1`), stubs on the map symbols of `x87_names.tsv`.
  Runs: the D2 race env (`MASHED_RACE_DEMO`, track 12) and the PORT-STATE-03 frontend walk.

Score: per RVA, calls per CW value per window. An RVA never reached is `NOT REACHED` (no claim).

## 3. Arm 2, static precision scan (`x87_pcscan.py`)

Recursive-descent walk of each body's x87 stream with a tag per stack value (E float / I int / D double leaf, R1 one
rounding, NF float chain, NW wide chain). Events: chain, cmp, intconv, widestore, retst0, dblop (PC-sensitive),
intop, trans (SSE2 not proven equal). Run on `MASHED.exe.unpatched` (the original bodies) and on the exe image.

Rules used for "no discriminating input exists" (a proof, no arm needed):
- PC affects only FADD/FSUB/FMUL/FDIV/FSQRT results. One such op on two float operands, stored to m32, gives the same
  float at PC_24 and PC_53 (53 >= 2*24+2: the double rounding is innocuous). A body with **no PC-sensitive event**
  (class `NOFP` or `PC-INSENSITIVE`) therefore stores identical values under x87 @ PC_24 and x87 @ PC_53.
- SSE2 rounds each float op to float, which equals x87 @ PC_24 for float-only data flow. Not claimed when `dblop`,
  `intop` or `trans` is present.

## 4. Arm 3, live-seeded emulation (`x87_live_emu.py`, images from `x87_port_images.py`)

For each in-scope RVA the scan marks `PC-SENSITIVE` and arm 1 reaches: sampled real calls in the original (orig root,
hooks OFF, same Quick-Battle recipe). At a sampled entry the game thread waits while Unicorn replays the call from the
live memory (pages fetched on demand from the paused process, nothing written back). Variants on the same state:

| id | code | CW |
|---|---|---|
| O | original bytes | the live CW (the reference) |
| O53 | original bytes | live CW with PC = 53 |
| O24 | original bytes | live CW with PC = 24 |
| X53 | port body, `/arch:IA32` image | PC = 53 (the exe today) |
| X24 | port body, `/arch:IA32` image | PC = 24 |
| S | port body, `/arch:SSE2` image | live CW, MXCSR 0x1fa0 (the exe's) |

Compared per call against O: every byte written outside the callee stack frames and the port images' own data, and the
return value by the C signature (eax for int, ST0 for float: raw 80-bit and rounded to f32). Calls that reach an x87
transcendental, an import, or code outside the original and port images are `REFUSED` (counted, not scored).

Per RVA and variant: `GREEN` = equal on every scored call, `RED n/N` otherwise. A **natural delta** is a call where
O24 != O53. RVAs with none get `no natural delta in N calls` (and the static class stands).

Controls (must hold, or the arm is void):
- `0x004325c0`: O53 differs from O on at least one call, X53 differs, S is GREEN (PORT-STATE-03-FPU).
- O24 = O on every call when the live CW is 0x007f (the emulator honours PC).

## 5. Deliverable and what changes

Per-wrapper table: RVA, TU, original CW, current build mode, modes that match, recommendation. Recommendations, no
flip of any default:
- keep x87: no PC-sensitive event, or x87 @ PC_53 GREEN;
- `SSE2` candidate: x87 @ PC_53 RED and S GREEN, float-only;
- `PC_24` candidate: X53 RED and X24 GREEN, S RED or not proven.

A proposed build-mode or precision change is listed, not applied. If one is written, it ships as a default-OFF knob
with its own PREREG. The precision question (game-thread PC_24 after device creation, as the original has it) goes to
the RESULT as a decision question for the lead.

Gates: if no `mashedmod/` code changes, none are run. Any code commit keeps the default gates (`merge_train.json`
`gctrl.ref` / `b.ref`) unchanged.
