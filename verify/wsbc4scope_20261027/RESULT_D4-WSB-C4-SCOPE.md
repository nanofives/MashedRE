# RESULT D4-WSB-C4-SCOPE: out-memory scoping for the BROAD and SOLV bands (C3 -> C4)

Verdict: **GREEN** (scoping leg only). Static work only: no game run, no build, no source / tracker / default
change. No row changes level here. Every C4 below still needs its runtime legs, and any status change goes
through re-classify.

Ledger rows `D4-WSB-C4-BROAD` and `D4-WSB-C4-SOLV`. Branch `orch/D4-WSB-C4-SCOPE` off `orch/integration` @
`ebc7b0c36`. Inputs: `verify/wsbbvh_20261026/RESULT_D4-WSB-C4-BVH.md` (the C4 method: racediff + out-memory leg A
or B) and the in-progress `D4-WSB-BVH-WRITESET` branch (read only: its `scan_blindspot.py` / `blindspot.tsv` and
the write-set method).

## Scope

The two steps cover the RW-Physics rows of `hooks.csv` (`third-party-library[RenderWare-Physics-3.7]`):

- BROAD `0x00558000-0x0055efff`: 10 C3/impl + 1 C3/ported = **11 rows**.
- SOLV `0x00566000-0x0056ffff`: **30 C3/impl rows**.

The address band also holds 5 C3 rows of other subsystems and 4 RW-Physics rows already at C4. They are not in
these two steps and are listed in the appendix (the "7 BROAD C3 rows with no shadow wrapper" count of the
BVH-WRITESET note is over all subsystems: 5 of those 7 are outside RW-Physics).

## Method (static, original binary only)

Binary: `original/MASHED.exe.unpatched` of the main checkout, SHA-256 checked against the anchor
`BDCAE093...3C0E` by the script before any read. `original/` not touched otherwise.

1. **Body extent**: Ghidra headless via `re/tools/decomp_pc.py` (pool slot `Mashed_pool10`, read-only, 50
   functions, `decomp.json` raw). Every row's Ghidra function size equals the capstone recursive-descent extent
   of item 2 (last instruction end = entry + size, all 50 rows).
2. **Stores**: `scan_scope.py`. Recursive descent from the entry, ESP delta per path (push/pop, add/sub esp,
   stack probe `0x004a3440`, each direct callee's own `ret N`), register provenance per path. Every write-access
   memory operand (x87 `fst*`/`fist*`/`fnst*` counted, capstone does not flag them), and every string store, is
   classed `local` (entry offset < 0, the own frame), `arg` (an entry stack slot, offset >= 4 = the caller's
   frame), `global` (absolute address) or `ptr` (any other base). "Outside the frame" = `arg + global + ptr +
   str`. Output `scope_stores.tsv` (one row per RVA), `scope_detail.txt` (every non-local store with its
   address and operand, every call, every tail jump).
3. **Wrapper**: the ShadowAB template the port's wrapper calls (`ShadowAB::Run` = return only, `RunTracked` = page
   write tracking + caller 4 KB stack window, `RunRegion` = one hand-declared region), read from
   `mashedmod/src/mashed_re/**/*.cpp`, plus the C3 evidence kind in `hooks.csv` (`shadow_ab` / `shadow_tracked`
   / `shadow_region`). The two agree on every row.
4. **Arg-slot read-back** (rows whose only non-region stores are entry-slot writes): `scan_argslot_callers.py`
   walks every direct caller forward from the return and reports a read of the overwritten slot before it is
   freed or rewritten. A slot that falls in a later callee's parameter window is checked against that callee's
   arity (`scan_arity.py`). Output `argslot_callers.txt`, `arity.txt`.
5. **Census**: `verify/wsbcensus_20261026/wsb_census.tsv` joined per row (EXEC / NOT-EXEC, phases).

Known limits of the scan: an indirect `call [reg+k]` has an unknown `ret N` (6 such calls, all in BROAD,
listed in `scope_detail.txt`); every row's `ret` still reaches d = 0 with no ESP conflict, so the ESP-relative
classing after those calls is consistent. A store reached only through a computed jump would be missed (no
indirect `jmp` occurs in any of the 41 bodies). Register provenance is printed for reading only; the class of a
store (local / arg / ptr) does not depend on it, except the RunRegion check below, re-read by hand.

## Correction to `blindspot.tsv` (BVH-WRITESET)

Its linear sweep scans `[rva, next hooks.csv rva)`, which runs past the function's last instruction where the
next `hooks.csv` row is far away:

- `0x0055e050`: listed blind with 45 stores. Its body is `0x0055e050..0x0055e07b` (Ghidra size 44), the sweep
  ran to `0x0055e190`. The body has **no store outside its frame**: one indirect call `[eax+0x18]` at `0x0055e060`
  and `call 0x0055ab30` at `0x0055e06d`. Not blind.
- `0x00568fd0`: listed 30 stores. Body `0x00568fd0..0x00569131` (Ghidra size 354), sweep to `0x00569690`. The body
  has 2 pointer stores (`mov [edx+0xd0], ebx` at `0x00569026`, `mov [edx+0xd0], eax` at `0x005690dd`) and 2
  param_1-slot stores. Still blind, smaller set.
- ESP-based stores were excluded there, so entry-slot writes were not counted: `0x0055ae50` (Run, 2 slot writes
  then a tail jump) is out-of-frame by this scan and was "no" there.

Net: the 9 blind rows of that note become **8 Run rows with pointer/global stores** plus `0x0055ae50` (slot
writes only, see BROAD-TAIL), and `0x0055e050` leaves the list.

## BROAD per-row table (11 RW-Physics rows)

`out` = stores outside the own frame (arg / global / ptr / str). `group` = C4 work step (plan below).

| rva | lvl | wrapper (C3 evidence) | census | out | outside-frame stores (address: operand) | group |
|---|---|---|---|---|---|---|
| `0x0055a1f0` | C3/impl | Run (shadow_ab) | EXEC race_run | 46 | 29 ptr, 2 global `[0x007dc8c0]` (`0x0055a783` =0, `0x0055a79b` =1), `rep movsd` into param_5 ecx=5 (`0x0055a8bc`), 14 entry-slot writes (param_1/5/6/9) | BROAD-WS |
| `0x0055ab30` | C3/impl | Run (shadow_ab) | EXEC | 1 | `or [eax], edx` at `0x0055ab9d` | BROAD-WS |
| `0x0055abb0` | C3/impl | Run (shadow_ab) | EXEC | 0 | none (1 call `0x00565200` at `0x0055abea`) | BROAD-A |
| `0x0055ae50` | C3/impl | Run (shadow_ab) | EXEC | 2 | param_2 slot `0x0055ae5e`, param_1 slot `0x0055ae62`, then `jmp 0x00565550` at `0x0055ae66` (tail call, no `ret`) | BROAD-TAIL |
| `0x0055bae0` | C3/impl | Run (shadow_ab) | **NOT-EXEC** | 3 | `fstp [eax]` / `[eax+4]` / `[eax+8]` at `0x0055bb49`/`0x0055bb50`/`0x0055bb5e`, eax = return of `call [ecx+8]` at `0x0055baf0` | BROAD-WS |
| `0x0055bb70` | C3/impl | Run (shadow_ab) | **NOT-EXEC** | 0 | none (calls `0x004c4600`, `0x0055bae0` x3) | BROAD-A |
| `0x0055c000` | C3/ported | **none** (batch-w-s5) | EXEC | 3 | `fstp [eax]` / `[eax+4]` / `[eax+8]` at `0x0055c0c9`/`0x0055c0d0`/`0x0055c0de`, eax = return of `call [edx+0x14]` at `0x0055c073` | BROAD-WS |
| `0x0055c0f0` | C3/impl | RunTracked (shadow_tracked) | EXEC | 8 | 8 ptr, `0x0055c157` .. `0x0055c225` (5 of them param_1+0/4/8/0xc) | BROAD-WS |
| `0x0055c230` | C3/impl | Run (shadow_ab) | EXEC | 0 | none (indirect `[edx+0x18]` at `0x0055c2a8`, `call 0x0055c0f0` at `0x0055c2ba`) | BROAD-A |
| `0x0055deb0` | C3/impl | bespoke b5c leaf compare (return) | EXEC menu+race | 0 | none (3 instructions) | BROAD-A |
| `0x0055e050` | C3/impl | Run (shadow_ab) | EXEC | 0 | none (see correction above) | BROAD-A |

`0x0055deb0`'s wrapper (`RwpWorldSolverHandle`, `RwpIntegrator.cpp:140-158`) is a bespoke b5c self-test that
compares the return only. The regex scan reads it as `none`. With no store, return-only is complete for it.

## SOLV per-row table (30 rows)

| rva | wrapper (C3 evidence) | census | out | outside-frame stores | group |
|---|---|---|---|---|---|
| `0x005675d0` | RunTracked | EXEC | 16 | 16 ptr | SOLV-TRK1 |
| `0x00567c00` | RunTracked | EXEC | 7 | 7 ptr | SOLV-TRK1 |
| `0x00567c60` | RunTracked | EXEC | 41 | 25 ptr, 16 slot | SOLV-TRK1 |
| `0x00567f00` | RunTracked | EXEC | 20 | 16 ptr, 4 slot | SOLV-TRK1 |
| `0x005684c0` | RunTracked | EXEC | 26 | 26 ptr | SOLV-TRK1 |
| `0x00568560` | RunTracked | EXEC | 21 | 21 ptr | SOLV-TRK1 |
| `0x005685f0` | RunTracked | **NOT-EXEC** | 49 | 48 ptr, 1 str | SOLV-TRK1 |
| `0x00568990` | **Run** | EXEC | 9 | 9 ptr, base `esi` = param_1+0x37c: +0x70/+0x74 (`fstp`, `0x005689d0`/`0x005689d6`), +0x30/+0x3c/+0x78 (`0x00568a03`..`0x00568a09`), +0x4/+0x1c/+0x10/+0x5c (`0x00568aa6`..`0x00568aaf`) | SOLV-WS |
| `0x00568dd0` | **Run** | EXEC | 12 | 12 ptr, `0x00568deb` .. `0x00568f8b` (`[esi+0..0x24]`, `[eax+0..8]`, `[ebp]`, `[*(param_1)+eax*4-4]`) | SOLV-WS |
| `0x00568fd0` | **Run** | EXEC | 4 | 2 ptr `[edx+0xd0]` (`0x00569026`, `0x005690dd`), 2 param_1-slot (`0x00569019`, `0x005690d9`) | SOLV-WS |
| `0x0056b7a0` | **Run** | EXEC | 7 | 6 ptr (`[esi+0x84]` `0x0056b945`, `[eax+0/4/8/0xc/0x10]` `0x0056b956`..`0x0056b967`), 1 param_4-slot (`0x0056b7f3`) | SOLV-WS |
| `0x0056ba30` | **Run** | EXEC | 4 | `[esi+0x84]`=0 `0x0056ba46`, `[ebx+ecx+0xdc]` `0x0056bae6`, `[edi+4]` `0x0056bb0c`, `add [esi+0x90], eax` `0x0056bb12` | SOLV-WS |
| `0x0056bb30` | Run | EXEC | 0 | none (1 call `0x0056ba30`) | SOLV-RGN |
| `0x0056bce0` | RunRegion param_1, 0x10 | EXEC | 7 | 4 ptr all in region (param_1+0xc/+0/+4/+8, `0x0056bdc3`..`0x0056bddd`), 3 slot spills param_1/2/3 (`0x0056bd67`/`0x0056bd87`/`0x0056bdb7`) | SOLV-RGN |
| `0x0056bdf0` | RunTracked | EXEC | 6 | 6 ptr | SOLV-TRK1 |
| `0x0056c0a0` | RunTracked | EXEC | 19 | 19 ptr | SOLV-TRK2 |
| `0x0056c310` | RunTracked | EXEC | 20 | 19 ptr, 1 slot | SOLV-TRK2 |
| `0x0056c580` | RunTracked | EXEC | 34 | 12 ptr, 21 slot, 1 str | SOLV-TRK2 |
| `0x0056c8e0` | RunTracked | EXEC | 6 | 6 ptr | SOLV-TRK2 |
| `0x0056caa0` | RunTracked | EXEC | 71 | 67 ptr, 4 str | SOLV-TRK2 |
| `0x0056cf90` | RunRegion param_1, 0x60 | EXEC | 24 | 24 ptr, param_1+0x0..+0x5c step 4 (`0x0056cfa9`..`0x0056d064`), all in region | SOLV-RGN |
| `0x0056d070` | RunTracked | EXEC | 44 | 38 ptr, 6 slot | SOLV-TRK2 |
| `0x0056dd40` | RunTracked | EXEC | 16 | 16 ptr | SOLV-TRK2 |
| `0x0056e680` | RunTracked | EXEC | 44 | 32 ptr, 12 slot | SOLV-TRK2 |
| `0x0056ed60` | RunRegion param_1, 0x2c | EXEC | 9 | 9 `fstp` param_1+0/4/8/0x10/0x14/0x18/0x20/0x24/0x28 (`0x0056ee59`..`0x0056ef28`), all in region | SOLV-RGN |
| `0x0056ef30` | RunTracked | EXEC | 26 | 26 ptr | SOLV-TRK2 |
| `0x0056efc0` | RunTracked | EXEC | 4 | 4 ptr | SOLV-TRK2 |
| `0x0056f020` | RunTracked | EXEC | 7 | 7 ptr | SOLV-TRK2 |
| `0x0056f1f0` | RunTracked | EXEC | 28 | 27 ptr, 1 slot | SOLV-TRK2 |
| `0x0056fad0` | RunRegion param_1, 0x3c | EXEC | 12 | param_1+0/4/8 (`0x0056fae8`..`0x0056faf4`), +0x10..+0x38 `fstp` (`0x0056fb0a`..`0x0056fb86`), all in region | SOLV-RGN |

RunTracked rows' individual store addresses are in `scope_detail.txt`; their out-memory is compared whole by the
write-tracked shadow, so they need no per-store list.

### RunRegion coverage (checked by hand)

Every pointer store of the 4 RunRegion bodies lands inside `[param_1, param_1 + size)`, with the base register
loaded from the param_1 slot before any write to that slot (`0x0056bce3` for `0x0056bce0`; the other three
never write an entry slot). Highest byte written: `0x0056bce0` 0x10 of 0x10, `0x0056cf90` 0x60 of 0x60,
`0x0056ed60` 0x2c of 0x2c, `0x0056fad0` 0x3c of 0x3c. So each declared region covers every pointer store.

### Entry-slot writes: no caller reads them back (`argslot_callers.txt`)

| function | slots written | direct callers | result |
|---|---|---|---|
| `0x0055ae50` | param_1, param_2 | 5 (`0x0047dfaa`, `0x0047e0a1`, `0x0047e18c`, `0x0047e2ea`, `0x0056b833`) | every site frees both slots (`add esp`) before any read; no absolute reference to `0x0055ae50` in the image |
| `0x0056bce0` | param_1..3 | 3 (`0x0056bf3c`, `0x0056c17b`, `0x0056c3e2`) | no read; at `0x0056c086`/`0x0056c2e9`/`0x0056c552` the slots sit in the param_2..4 window of `call 0x004c45f0`, which reads only its param_1 (`arity.txt`); then freed |
| `0x00568fd0` | param_1 | 1 (`0x00561e73`) | freed at `0x00561e78`, no read |
| `0x0056b7a0` | param_4 | 2 (`0x0056b9f8`, `0x0056ba7e`) | no read; the slot is param_11 of `0x00573670` (reads up to param_7) and param_9 of `0x00568990` (reads up to param_5); then freed |
| `0x0055a1f0` | param_1/5/6/9 | 1 (`0x0055fe91`) | freed at `0x0055fe96`, no read |

Two `EBPFRAME` notes in that file (`0x0056b838`, `0x00421b72`) are pointer accesses: in both callers `ebp` is loaded
from memory (`mov ebp, [esp+0x74]` at `0x0056b7e3`) or computed (`lea ebp, [ebx+eax*8]` at `0x00421b15`), not set
from `esp`. So for `0x0055ae50` and `0x0056bce0` the slot writes reach no reader, and their observable
out-memory is: none (`0x0055ae50`) / the declared region (`0x0056bce0`). For the three Run rows that also have
pointer stores, the slot result is informational: their write-set leg is needed anyway.

## Rows that need a write-set re-check (D4-WSB-BVH-WRITESET method)

A C3 resting on `ShadowAB::Run` (return only) or on no shadow, with a store outside the frame:

- **BROAD**: `0x0055a1f0`, `0x0055ab30`, `0x0055bae0` (Run), `0x0055c000` (no wrapper).
- **SOLV**: `0x00568990`, `0x00568dd0`, `0x00568fd0`, `0x0056b7a0`, `0x0056ba30` (Run).

9 rows. Harness step first: swap the 8 `Run` wrappers to `RunTracked` (the change BVH-WRITESET made in
`9577eac4d`), and give `0x0055c000` a `RunTracked` shadow wrapper (it has none). The wrappers link into the exe,
so the default gates are re-run on that build and must match `merge_train.json`.

Not in that set: rows with no store outside the frame (return-only is complete), `0x0055ae50` (slot writes with no
reader), the RunRegion rows (region covers every pointer store; `0x0056bce0`'s slot spills have no reader), and
the 21 RunTracked rows (their C3 already compared writes).

## C4 plan

Per row, C4 (re/CONFIDENCE.md, WORKER_RULES) = a **canonical-scenario run with the hook installed and its body
executing**, both legs on that run kind:

1. **Racediff** with the hook installed alone (`MASHED_HOOK_ONLY=0x<rva>` ON, `__none__` OFF), tool v6
   (`verify/wsbbvh_20261027/c4_racediff_v6.py`, the BVH v5 tool + declared-width masking + census driver),
   Quick Battle window first, census scenario (`C4_SCENARIO=census`, track 0 / mode 10 / 4 cars) when the Quick
   Battle window does not observe the row. Verdict `C4-CLEAN` with the original RVA starting with `0xE9`.
2. **Out-memory leg**, one of:
   - A. no store outside the frame (this note's scan), or slot writes with no reader (BROAD-TAIL), or
   - B. write-tracked shadow (`RunTracked`) with the hook installed in the census scenario, `MASHED_SHADOW_AB=1`,
     0 divergent samples, patch-byte proof `A/B-IS-REAL installed=e9`, re-run on the step's own build (the C3 shadow
     logs are from 2026-09-10 and the BVH C4 step re-ran its shadows the same way), or
   - R. region shadow (`RunRegion`) with the hook installed, plus this note's region-coverage check.

Promotion only through re-classify, per row, when both legs are clean.

| step | rows | out-memory leg | harness change | notes |
|---|---|---|---|---|
| `D4-WSB-C4-BROAD-A` | `0x0055abb0`, `0x0055bb70`, `0x0055c230`, `0x0055deb0`, `0x0055e050` | A | none | `0x0055bb70` NOT-EXEC in the census: find a scenario that runs it first. `0x0055deb0` is the register-ABI row (`RwpWorldSolverHandle_RegAbi`): check the racediff reads its register args |
| `D4-WSB-C4-BROAD-TAIL` | `0x0055ae50` | A (slot writes, no reader) | racediff must record the tail `jmp 0x00565550` at `0x0055ae66` as the call-out (OFF side has no `call`, the port calls `FUN_00565550`, `RwpSolverBroadphase3.cpp:538`) | without that change ON / OFF call sets differ by construction |
| `D4-WSB-C4-BROAD-WS` | `0x0055a1f0`, `0x0055ab30`, `0x0055bae0`, `0x0055c000`, plus `0x0055c0f0` (RunTracked already) | B | Run -> RunTracked x3, new RunTracked wrapper for `0x0055c000`, gates re-run | `0x0055bae0` NOT-EXEC in the census (same scenario search as BROAD-A, its only static callers are 3 sites in `0x0055bb70`) |
| `D4-WSB-C4-SOLV-WS` | `0x00568990`, `0x00568dd0`, `0x00568fd0`, `0x0056b7a0`, `0x0056ba30` | B | Run -> RunTracked x5, gates re-run | |
| `D4-WSB-C4-SOLV-RGN` | `0x0056bce0`, `0x0056cf90`, `0x0056ed60`, `0x0056fad0`, plus `0x0056bb30` (no store) | R (A for `0x0056bb30`) | none | |
| `D4-WSB-C4-SOLV-TRK1` | `0x005675d0`, `0x00567c00`, `0x00567c60`, `0x00567f00`, `0x005684c0`, `0x00568560`, `0x005685f0`, `0x0056bdf0` | B | none | `0x005685f0` NOT-EXEC in the census |
| `D4-WSB-C4-SOLV-TRK2` | `0x0056c0a0`, `0x0056c310`, `0x0056c580`, `0x0056c8e0`, `0x0056caa0`, `0x0056d070`, `0x0056dd40`, `0x0056e680`, `0x0056ef30`, `0x0056efc0`, `0x0056f020`, `0x0056f1f0` | B | none | 12 rows, the largest group; split again if a session cannot fit 24 boots |

Order and dependencies:

- **Every runtime leg is blocked while the original does not boot on LIVING-PC** (BVH-WRITESET: AV at
  `0x005baf48` before the menu since 2026-10-27 02:15, NULL audio object, `[UNCERTAIN]` cause: no active audio
  render endpoint). Unblock = an active audio render device. The static parts of BROAD-TAIL (tool change) and
  the wrapper swaps of BROAD-WS / SOLV-WS (build + standalone gates) can run before that.
- BROAD-WS, SOLV-WS, SOLV-TRK1/2 and SOLV-RGN reuse `verify/wsbbvh_20261027/run_shadow.py` and the v6 tool: start
  them after `D4-WSB-BVH-WRITESET` merges into `orch/integration`, or copy the tools in.
- Call chains inside the bands (port calls port callees in the ON boot): `0x0055e050` -> `0x0055ab30`,
  `0x0055c230` -> `0x0055c0f0`, `0x0055abb0` -> `0x00565200`, `0x0056bb30` -> `0x0056ba30` -> `0x0056b7a0` /
  `0x00568990`, `0x0055bb70` -> `0x0055bae0`. A racediff compares the call-out and its args, not the callee's
  effect, so a caller's C4 does not depend on its callee's level. Running a callee's group before its caller's
  keeps a caller's ON boot on C4 callees.

## Appendix: band rows outside these two steps

| rva | subsystem / lvl | wrapper | out | note |
|---|---|---|---|---|
| `0x00558100` | gameplay C3/impl | none (earlywindow r151) | 4 | `[ecx+4]` `0x0055811b`, `[eax+0x48]` = `0x557b70` / `0x4e5f90` (`0x00558125`/`0x0055812e`), `[ecx]`=1 `0x00558135` |
| `0x00558140` | gameplay C3/impl | none (earlywindow r144) | 3 | `[eax+0x48]` `0x0055815f`, `[ecx+4]`=0 `0x00558162`, `[ecx]`=0 `0x00558169` |
| `0x0055c4f0` | vehicle C3/impl | none (race diff, negctl reearn) | 1 | `[param_1+0x50]` `0x0055c536` |
| `0x0055c810` | vehicle C3/impl | none (race diff, negctl reearn) | 3 | param_1+0x40/+0x44/+0x48 (`0x0055c829`..`0x0055c835`) |
| `0x0055dec0` | util C3/new | none | 0 | 3 instructions |
| `0x0055ac00` | RW-Physics **C4** | bespoke b5c compare | 2 | both stores `mov [edx], ecx` (`0x0055ac31` set path, `0x0055ac4b` clear path) to one word; the b5c compare snapshots and compares that word (`RwpIntegrator.cpp:229-243`), so its C4 has an out-memory leg |
| `0x0055e200` | RW-Physics **C4** | bespoke b5c compare | 1 | `fstp [param_1]` `0x0055e208`; the compare snapshots and compares `*ctx` (`RwpIntegrator.cpp:435-448`) |
| `0x0055dff0` | RW-Physics **C4** | none | 2 | param_2 / param_1 slot writes (`0x0055e035`/`0x0055e039`) then `jmp 0x0055b800`; 11 direct callers, none reads the slots (`argslot_callers.txt`) |
| `0x0055b800` | RW-Physics **C4** | none | 0 | |

The 4 non-RW-Physics C3 rows with stores (`0x00558100`, `0x00558140`, `0x0055c4f0`, `0x0055c810`) have no
shadow wrapper, so whichever step takes them to C4 needs out-memory leg B (a new wrapper) or a store tracer.

## Files

Committed (`verify/wsbc4scope_20261027/`): `scan_scope.py`, `scan_argslot_callers.py`, `scan_arity.py`,
`scope_stores.tsv` (`c78bf0a1...e5f`), `scope_detail.txt` (`5c233fb6...9bb`), `argslot_callers.txt`, `arity.txt`,
this file. Regenerate: `py -3.12 verify/wsbc4scope_20261027/scan_scope.py [--detail]`.

Raw (not committed), `D:/MashedWT/D4-WSB-C4-SCOPE_data/`:

| file | SHA-256 |
|---|---|
| `decomp.json` (decomp_pc.py, 50 functions, pool slot 10) | `3a1324dc1e8475f64c6c5f736783d804034b168e1418bb4c3df6c8f50843cf89` |

No game, no build, no Frida, no watcher was started by this step.
