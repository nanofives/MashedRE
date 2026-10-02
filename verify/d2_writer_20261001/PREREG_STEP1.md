# PRE-REGISTRATION — D2 attempt 15, STEP 1: confirm the `+0xbf8 = 2` writer on the RUNNING original

Written and committed **before** the first run of this step. Nothing below may be amended
after a run. If a registered gate fails, the step STOPS and the failure is reported as-is.

Branch `race/first-frame-parity`, HEAD at writing `043859e4`.
Binary anchor: `original/MASHED.exe.unpatched`, SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` (CLAUDE.md anchor).

---

## 1 The static finding this step is testing

`U-9174` records that a byte scan for the displacement `F8 0B 00 00` finds **7** sites in the
image and that **every literal-displacement writer of `+0xbf8` writes ZERO**. That is correct
and is **not** contradicted here. The writer uses a **folded absolute base**, which no
displacement scan and no decompiler grep on `+ 0xbf8` can see:

The 16-slot vehicle record array base is **`0x008815a0`**, stride **`0xd04`** (independently
recorded in `verify/d2_reopen_20260929/orig_solo3.msd.provenance.json` as
`base_va 0x8815a0`, `rec_size 0xd04`). MSVC folds `&veh[i].field` into
`i*0xd04 + (0x008815a0 + field)`, so

| field | folded absolute operand |
|---|---|
| `+0xbf4` | `0x00882194` |
| `+0xbf8` | `0x00882198` |

A capstone sweep of `.text` for memory operands whose displacement is `0x00882194` /
`0x00882198` returns **exactly 10 instructions**, all between `0x0046c742` and `0x0046d854`.
`imul eax,eax,0xd04` at `0x0046d78e` and `0x0046d848` is the stride witness.

### The candidate writer — `FUN_0046d780`, body `0x0046d780..0x0046d7ec`

Listing (`re/tools/disasm_va.py`, `MASHED.exe.unpatched`):

```
0x0046d780  8b 54 24 04        mov  edx, [esp+4]          ; param_1 = car index
0x0046d784  83 fa 10           cmp  edx, 0x10
0x0046d787  7c 03              jl   0x46d78c
0x0046d789  33 c0              xor  eax, eax
0x0046d78b  c3                 ret
0x0046d78c  8b c2              mov  eax, edx
0x0046d78e  69 c0 04 0d 00 00  imul eax, eax, 0xd04
0x0046d794  8b 88 94 21 88 00  mov  ecx, [eax+0x882194]   ; ecx = veh[i].+0xbf4
0x0046d79a  81 f9 e8 03 00 00  cmp  ecx, 0x3e8            ; 1000
0x0046d7a0  7e 21              jle  0x46d7c3
0x0046d7a2  c7 80 98 21 88 00 02 00 00 00
                               mov  dword [eax+0x882198], 2   ; <== veh[i].+0xbf8 = 2
0x0046d7ac  b8 e8 03 00 00     mov  eax, 0x3e8
0x0046d7b1  2b c1              sub  eax, ecx
0x0046d7b3  50                 push eax                   ; 1000 - charge  (negative)
0x0046d7b4  52                 push edx
0x0046d7b5  e8 96 53 fb ff     call 0x422b50
0x0046d7ba  83 c4 08           add  esp, 8
0x0046d7bd  b8 01 00 00 00     mov  eax, 1
0x0046d7c2  c3                 ret
0x0046d7c3  85 c9              test ecx, ecx
0x0046d7c5  7e 20              jle  0x46d7e7
0x0046d7c7  81 c1 e8 03 00 00  add  ecx, 0x3e8
0x0046d7cd  51                 push ecx
0x0046d7ce  52                 push edx
0x0046d7cf  c7 80 98 21 88 00 01 00 00 00
                               mov  dword [eax+0x882198], 1   ; veh[i].+0xbf8 = 1
0x0046d7d9  89 88 94 21 88 00  mov  [eax+0x882194], ecx       ; veh[i].+0xbf4 += 1000
0x0046d7df  e8 6c 53 fb ff     call 0x422b50
0x0046d7e4  83 c4 08           add  esp, 8
0x0046d7e7  b8 01 00 00 00     mov  eax, 1
0x0046d7ec  c3                 ret
```

### The charge accumulator — `FUN_0046d7f0`, body `0x0046d7f0..0x0046d87f`

`FUN_0046d7f0(int carIdx, int delta)`. Call site `0x00410441` pushes `ebx` then `esi`
(`0x0041043f push ebx` / `0x00410440 push esi`), so the arity is **2**; Ghidra's decompiler
prints one argument and is wrong there (memory `decomp-is-silent-about-register-args` family).

```
if (160.0f < (float)accel_byte_of(carIdx))   ;  _DAT_005cea3c = 160.0  (0x43200000)
    veh[i].+0xbf4 += delta*2;                ;  0x0046d845
veh[i].+0xbf4 -= delta;                      ;  0x0046d864
if (veh[i].+0xbf4 > 3000) veh[i].+0xbf4 = 3000;   ; 0x0046d869  (0xbb8)
if (veh[i].+0xbf4 < 0)    veh[i].+0xbf4 = 0;      ; 0x0046d874
```

The accel byte it reads is `(&DAT_007f103c)[ctrl*0x13]`; `0x007f103c` is
`0x007f1038 + 4`, and the capture provenance records `accel_byte: 4` off
`block_base 0x007f1038`. So "charge while accel is held above 160/255".

### The trigger — `FUN_004103a0`, the pre-race countdown tick

Sole caller of **both** `0x0046d780` and `0x0046d7f0`; itself called only by the race
state machine `FUN_004111c0` (dispatcher on `DAT_0063ba8c`).

```
per active car:  FUN_0046d7f0(i, delta)        ; 0x00410441      charge, every tick
FUN_0041da90(&t) ; t = DAT_0063d588
if (t >= 1.86f)  ;  _DAT_005ccdf4 = 1.86 (0x3fee147b), test at 0x00410460
    per active car:  FUN_0046d780(i)           ; 0x0041049b      RELEASE
                     DAT_007f0a04[i] = FUN_0046c750(i)   ; +0xbf4 readback
                     DAT_007f0a08[i] = FUN_0046c730(i)   ; +0xbf8 readback
    DAT_0063ba8c = 6                           ; 0x004104e3      -> racing
```

**Mechanism, stated mechanically:** a per-car integer charge at `+0xbf4` accumulates while
accel is held during the pre-race state and saturates at 3000; at the green light
`FUN_0046d780` converts it to a state at `+0xbf8` — `2` when the charge exceeds 1000,
`1` otherwise — which A6a (`0x00467d2e` / `0x00467def`) then consumes. The `== 2` arm
(`0x00467e06..0x00467e44`) zeroes `+0xb14/+0xb18/+0xb1c` and counts `+0xbf4` down, which is
the measured 15-frame drive-force hold. `3000 / 200 = 15`.

### The complete reference set for the two fields (both encodings, whole image)

| RVA | instruction | field | kind |
|---|---|---|---|
| `0x0046c742` | `mov eax,[eax+0x882198]` | `+0xbf8` | READ (getter `FUN_0046c730`) |
| `0x0046c762` | `mov eax,[eax+0x882194]` | `+0xbf4` | READ (getter `FUN_0046c750`) |
| `0x0046d794` | `mov ecx,[eax+0x882194]` | `+0xbf4` | READ |
| **`0x0046d7a2`** | **`mov [eax+0x882198], 2`** | **`+0xbf8`** | **WRITE = 2** |
| `0x0046d7cf` | `mov [eax+0x882198], 1` | `+0xbf8` | WRITE = 1 |
| `0x0046d7d9` | `mov [eax+0x882194], ecx` | `+0xbf4` | WRITE |
| `0x0046d834` / `0x0046d845` | `mov edi,[eax+0x882194]` / `mov [eax],edi` | `+0xbf4` | READ / WRITE (`+= 2*delta`) |
| `0x0046d84e` / `0x0046d864` / `0x0046d869` / `0x0046d874` | via `lea eax,[ecx+0x882194]` at `0x0046d854` | `+0xbf4` | READ / WRITE (`-= delta`, clamp `[0,3000]`) |
| `0x00467d2e` | `cmp [esi+0xbf8], 1` | `+0xbf8` | READ (A6a) |
| `0x00467def` | `cmp [esi+0xbf8], 2` | `+0xbf8` | READ (A6a) |
| `0x00467dd7` `0x00467de5` `0x00467e36` `0x00467e44` | | `+0xbf8` | WRITE = 0 (A6a) |
| `0x00467cf1` `0x00467d08` `0x00467d10` `0x00467d1a` `0x00467d24` `0x00467d3b` `0x00467db9` `0x00467dcd` `0x00467ddd` `0x00467df8` `0x00467e2c` `0x00467e3c` | | `+0xbf4` | READ/WRITE (A6a) |

Method note: the enumeration is a capstone sweep over `.text` for displacement/immediate
`0x00882194` / `0x00882198` / `0x008815a0`, **plus** a Ghidra decompile of **all 6239**
defined functions (`re/tools/ghidra_scripts/GrepDecompAll.java`, 0 decompile failures)
grepped for `0xbf[048c]` — which returns `FUN_00467650` as the only function referencing
`+0xbf4`/`+0xbf8` in folded-struct form. **Neither instrument alone would have found
`0x0046d7a2`**; the first is why it was found, the second is why no second writer is claimed.
Coverage caveat **[UNCERTAIN]**: both instruments see only `.text`; a writer inside a region
Ghidra left undefined AND that the capstone linear sweep mis-synchronised over would be
missed by both. No such region is known.

---

## 2 What STEP 1 runs

One ORIGINAL-side capture, the **same arm** as `verify/d2_reopen_20260929/orig_solo3.msd`
(so the result is directly comparable to the measurement U-9174 rests on), plus a new
**entry-hook-only** probe:

```
py -3.12 re/frida/scenario_launch.py \
    --statediff-out verify/d2_writer_20261001/orig_bp1.msd \
    --statediff-drive --statediff-drive-late --statediff-steer 1 \
    --hold 38 --poke-ctrl-slots --boost-probe
```

`--boost-probe` adds `Interceptor.attach` at **two entry addresses only** —
`0x0046d780` and `0x0046d7f0` — with `onEnter` + `onLeave` on each attach. **No
mid-function probe, no watchpoint, no page guard.** Each callback reads
`0x008815a0 + car*0xd04 + 0xbf4` and `+ 0xbf8`, plus `DAT_0063ba8c` (`0x0063ba8c`),
and appends a row. Rows are drained to `verify/d2_writer_20261001/orig_bp1.boostprobe.csv`.

**Rate safety.** Both functions are called only from `FUN_004103a0`, which runs in one
race state; `0x0046d7f0` fires once per active car per tick and `0x0046d780` once per car
per race. Expected total well under 100 calls/s, far below the ~1000 calls/s threshold
CLAUDE.md records. A **hard row limit of 20000 with auto-detach** bounds exposure, matching
`magProbeArm`'s discipline. If the drained row count hits the limit the run is declared
**VOID** and no verdict is read from it.

Launch rules: muted, `MASHED_TITLE` set, `MASHED_WIN_POS=primary-bl`, PID tracked and only
that PID killed.

---

## 3 Registered gates and the verdict rule

All addresses below are read for **car 0** unless stated.

| id | gate | PASS condition |
|---|---|---|
| **G0** | probe armed | the arm call returns success for both RVAs and the run is not VOID |
| **G1** | the writer executes | `0x0046d780` is entered **exactly once** for car 0 in the run |
| **G2** | pre-state | at that `onEnter`: `+0xbf8 == 0` **and** `+0xbf4 == 3000` |
| **G3** | post-state | at that `onLeave`: `+0xbf8 == 2` **and** `+0xbf4 == 3000` (unchanged) |
| **G4** | the charge path executes | `0x0046d7f0` is entered on **>= 100** ticks before G1's call, and `+0xbf4` observed at those entries is non-decreasing up to 3000 |
| **G5** | the state transition | `DAT_0063ba8c` read at G1's `onLeave` is **6** on the tick after, or the probe records `DAT_0063ba8c` changing to 6 within the same tick |
| **G6** | consistency with the record | in the same run's `.msd`, `+0xbf8` is `2` on exactly **14** frames and `+0xbf4` falls `3000 -> 0` at `-200`/frame, reproducing `U-9174`'s three-capture measurement |

### VERDICT

- **CONFIRMED** iff **G0 ∧ G1 ∧ G2 ∧ G3**. Those four alone establish "`0x0046d7a2` is the
  instruction that sets `+0xbf8 = 2`, it runs once, and it runs with the measured charge".
- **G4/G5/G6 are corroborating, not deciding.** A failure of any of them is reported and
  blocks nothing in STEP 1, but **G6 failing means this capture is not the capture U-9174
  describes**, and in that case no cross-reference to the attempt-14 numbers may be made.
- **REFUTED** if G1 passes but G2 or G3 fails — the write observed is not the one modelled.
  STEP 2 does not start; the finding is reported and U-9174 stays open.
- **NO-EXECUTION** if G1 fails (the function is never entered). STEP 2 does not start.

A second boot is required before believing any negative verdict
(memory `shadow-lane-failure-windows`): if the first run yields REFUTED or NO-EXECUTION,
run it once more and report both.

---

## 4 What STEP 1 does NOT claim

- It does **not** claim `0x0046d7a2` is the only possible writer in every scenario — only
  that it is the one that fires in this arm, and that the image contains no other instruction
  referencing `+0xbf8` (section 1's table is exhaustive for `.text` under the two instruments,
  with the coverage caveat stated).
- It does **not** promote any function. Promotion happens in STEP 2 against
  `re/CONFIDENCE.md`, with only the evidence actually earned.
- It does **not** change any scored number. The section-3 bounds and the §16.7 arm are
  untouched by this step.
