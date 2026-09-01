# FUN_00415220 jump-table reachability map (why 0x00415200 never fires in a plain race)

Round 7 (area-ai, 2026-09-01). Answers the parent's directive: map the jump table at
`0x0041582c`, place each `0x00415200`/`0x00415190` call site in its switch arm, and state
what `[ESI+0x89a520]`/`[ESI+0x89a524]` must hold. **NO semantics assigned to mode values —
raw indices with cited addresses.** All addresses read from `MASHED.exe.unpatched`
(SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`) via
analyzeHeadless/DisasmPC/DecompPC on Mashed_pool2 + a pefile byte scan.

## Dispatch (FUN_00415220)

- `0x004152be  ADD EAX,-0x7`   — index = (dispatched value) - 7
- `0x004152c1  CMP EAX,0xc`
- `0x004152c4  JA  0x00415822` — out-of-range (index > 0xc) -> default/exit
- `0x004152ca  JMP dword ptr [EAX*0x4 + 0x41582c]`

Per-tick prologue writes to the two gate fields (`ESI = EBP*0x74 + base`, `EBP =` arg
`[ESP+0x38]`; accumulator `[ESI+0x89a51c]`):

- `0x00415254..0x00415264`  `[ESI+0x89a51c] += [0x007f1008]` (capped/compared to `0xea60`)
- `0x0041526f  JLE 0x00415277` / `0x00415271  MOV [ESI+0x89a524],1` — sets `0x89a524=1` iff accumulator `> 0xea60` (60000)
- `0x0041527c  JGE 0x00415288` / `0x0041527e  MOV [ESI+0x89a520],0xbb8` — sets `0x89a520=0xbb8` iff accumulator `< 0xbb8` (3000)

## Jump table @ 0x0041582c (13 dwords, pefile read)

| index | value (=idx+7) | arm target | contains a 0x00415200 call? | a 0x00415190 call? |
|------:|---------------:|------------|---|---|
| 0  | 0x07 | `0x004152d1` | **yes** (0x004152ed) | — |
| 1  | 0x08 | `0x00415822` (exit) | — | — |
| 2  | 0x09 | `0x00415383` | **yes** (0x004153d1) | — |
| 3  | 0x0a | `0x004155aa` | — | yes (0x004155e6) |
| 4  | 0x0b | `0x00415421` | **yes** (0x0041543d) | — |
| 5  | 0x0c | `0x0041563a` | — | yes (0x00415672) |
| 6  | 0x0d | `0x00415822` (exit) | — | — |
| 7  | 0x0e | `0x00415822` (exit) | — | — |
| 8  | 0x0f | `0x00415822` (exit) | — | — |
| 9  | 0x10 | `0x004156b9` | — | yes (0x00415703) |
| 10 | 0x11 | `0x00415499` | — | yes (0x004154c0) |
| 11 | 0x12 | `0x00415746` | — | — |
| 12 | 0x13 | `0x004157a0` | — | — |

So **`0x00415200` (r5) is called only from arms {value 7, 9, 0xb}**, and **`0x00415190`
(r6, which logged 129+ calls) only from arms {0xa, 0xc, 0x10, 0x11}**. The r6 arms are the
ones selected in a plain race; the r5 arms are not — matching the empirical logs and
refuting the earlier "FUN_00415220 does not run" reasoning (it runs; the r6 arms prove it).

## Local gate at each 0x00415200 call site (cited)

- **`0x004152ed`** (arm value 7):
  - `0x004152d3 MOV EAX,[ESI+0x89a524]; 0x004152d7 TEST; 0x004152d9 JNZ 0x0041536c` — bypass if `0x89a524 != 0`
  - `0x004152e1 MOV EAX,[ESI+0x89a520]; 0x004152e5 TEST; 0x004152e7 JNZ 0x00415822` — bypass if `0x89a520 != 0`
  - reached ⟺ value==7 ∧ `0x89a524==0` ∧ `0x89a520==0`
- **`0x004153d1`** (arm value 9, call block `0x004153b5`):
  - `0x004153b7 [ESI+0x89a524]; JNZ 0x004157b4` — bypass if `0x89a524 != 0`
  - `0x004153c5 [ESI+0x89a520]; JNZ 0x00415822` — bypass if `0x89a520 != 0`
  - reached ⟺ value==9 ∧ `0x89a524==0` ∧ `0x89a520==0`
- **`0x0041543d`** (arm value 0xb):
  - `0x00415423 [ESI+0x89a524]; 0x00415429 JZ 0x0041542f; 0x0041542b MOV byte [EDI+7],1` — `0x89a524` gates only a byte write, **not** the call
  - `0x00415431 [ESI+0x89a520]; 0x00415437 JNZ 0x00415822` — bypass if `0x89a520 != 0`
  - reached ⟺ value==0xb ∧ `0x89a520==0`

## Who writes 0x89a520 / 0x89a524 (all accessors enumerated by pefile byte scan)

- `[ESI+0x89a520]` (22 `.text` refs): the **only writes** are `←0xbb8` by the FUN_00415220
  prologue (`0x0041527e`, when accumulator `< 0xbb8`) and `←0` by the three reset functions
  below. Every other ref is a read.
- `[ESI+0x89a524]` (13 refs): `←1` by the prologue (`0x00415271`, when accumulator `> 0xea60`)
  and `←0` by the three resets.
- `[ESI+0x89a51c]` (5 refs): accumulated in the prologue, `←0` by the three resets.

The three reset functions clear all three together — `(&DAT_0089a51c)[p*0x1d]=0`,
`(&DAT_0089a520)[p*0x1d]=0`, `(&DAT_0089a524)[p*0x1d]=0` (`p*0x1d` dwords = `p*0x74` bytes,
matching `ESI=EBP*0x74`):
- `FUN_00416250` (writes at `0x0041656c/0x00416572/0x00416578`)
- `FUN_00416a30` (`0x00416cbc/0x00416cc2/0x00416cc8`)
- `FUN_00417da0` (`0x004180bc/0x004180c2/0x004180c8`)
All three are called from `FUN_00418560` (CallersPC).

`[0x007f1008]` (the per-tick accumulator increment) = the **per-frame race delta**: it is
written only at `0x0040fc65 MOV [0x7f1008],ESI` inside `FUN_0040fc00` ("Per-frame race
tick", C1), where `DAT_007f1008 = unaff_ESI` (the frame delta the caller leaves in ESI). It
is read as an integer (FILD at `0x00441d48/0x00445aa5`).

## Reachability conclusion (the real finding)

`0x00415200` is reached ⟺ value∈{7,9,0xb} ∧ `[0x89a520]==0` (∧ `[0x89a524]==0` for values
7,9). But:

1. `[0x89a520]` is set to `0xbb8` by this prologue on **every** tick where the accumulator
   `[0x89a51c] < 0xbb8`.
2. `[0x89a520]` is cleared to 0 **only together with** `[0x89a51c]=0` (the three resets).
3. `[0x89a51c]` accumulates the per-frame delta `[0x007f1008]` **one frame at a time** from 0.

So the accumulator can only reach `>= 0xbb8` by passing through frames where it is `< 0xbb8`,
each of which re-arms `[0x89a520]=0xbb8`. Therefore `[0x89a520]==0` (needed for the call) and
`[0x89a51c] >= 0xbb8` (needed for the prologue to skip re-arming) are **mutually exclusive in
steady per-frame stepping**. The only way `[0x89a520]` stays 0 at a switch is the first
FUN_00415220 tick after a reset with a single-frame delta `[0x007f1008] >= 0xbb8` (3000) —
i.e., a load/hitch stall, not steady race.

**Hence the three `0x00415200` call sites are effectively dead in a steady-state race** — the
reason r5's `MASHED_AI_V0GUARD_SELFTEST` log was never created. This is a reachability
property, not a claim the reimpl is wrong (r5 is a faithful transcription; it is simply not
exercised).

Options for r5 (parent decides classification; child does not edit hooks.csv):
- (a) **Contrived-state verification**: force `[0x89a51c] >= 0xbb8` and `[0x89a520]=0`, drive
  a vehicle in mode 7/9/0xb, then the A/B self-test fires — a `run_diff_scenario`
  cache-setter lane (as used for `VehicleVelocityWorldGet`), parent/harness work.
- (b) Accept the reachability finding: keep `0x00415200` at C2, documented as steady-state
  unreachable; it is not promotable to a race-witnessed C3.
