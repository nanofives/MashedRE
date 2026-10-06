# The car<->car call site, decoded — and the ONE thing the port cannot supply

**MEASURED / READ 2026-10-05.** No C-level moved, no band moved, no tracker mutated, no
game code edited, no build run, `original/` untouched. Anchor verified before any tool
ran (see §0). Everything below is either a quoted instruction from the anchored binary or
a number produced by a committed tool over committed data.

Companion pre-registration: [`verify/d3_carcar_20261005/PREREG_CARCAR.md`](../../verify/d3_carcar_20261005/PREREG_CARCAR.md).
New tool: `re/tools/hull_invariants.py`.

---

## 0. Anchor

```
original/MASHED.exe.unpatched   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
```
Every disassembly quote below is `py -3.12 re/tools/disasm_fn.py` over that file;
every decompilation is `py -3.12 re/tools/decomp_pc.py` against a read-only pool slot
(`Mashed_pool0`). The master Ghidra project was not opened and not written.

---

## 1. There is exactly ONE call site, and two instruments agree

```
py -3.12 re/tools/decomp_pc.py 0x00469df0 --callers --no-decomp
  // callers: FUN_004709a0@0x004709a0

py -3.12 re/tools/decomp_pc.py 0x00469df0 --datarefs --no-decomp
  //   1 reference(s) total
  //   other (1):  0x00470bcd in FUN_004709a0  [UNCONDITIONAL_CALL]  CALL 0x00469df0
```

Both instruments were run because `--callers` returns `(none)` when the caller sits in a
Ghidra-missed region, and that artefact nearly produced a false "the original never calls
this" claim in the previous session. Here they agree: **one caller, `FUN_004709a0`, one
call instruction, `0x00470bcd`.**

`FUN_004709a0` is `VehicleCollisionBroadPhase` in `hooks.csv` (C2, `mapped`, **no port
body** — its `file` column is an analysis plate, not a `.cpp`). Its body is
`0x004709a0..0x00470c5f`.

## 2. The ABI, from the four instructions that set it up

```
0x00470bc3: 55                       push ebp                      ; stack arg 2
0x00470bc4: 53                       push ebx                      ; stack arg 1
0x00470bc5: 8d87a0158800             lea eax, [edi + 0x8815a0]     ; EAX = &records[j]
0x00470bcb: 8bce                     mov ecx, esi                  ; ECX = &records[i]  (this)
0x00470bcd: e81e92ffff               call 0x469df0
0x00470bd2: 83c408                   add esp, 8
```

`EDI` is `j * 0xd04` (`0x00470b1d: imul edi, edi, 0xd04`); `ESI` is the self record
(`0x00470aef: mov eax,[esi+0x9ec]`, `0x00470b50: fadd dword ptr [esi+0x4a4]`). `EBX` is
`j` (`0x00470b11: inc ebx` off `[esp+0x14]` = `i`); `EBP` is the substep/pass counter
(`0x00470ab0: cmp ebp,2`).

Ghidra declares the callee `bool __thiscall FUN_00469df0(int *param_1, undefined4
param_2, int param_3)` with an undeclared register input `in_EAX`. Mapping the two
together:

| original | register / slot | value |
|---|---|---|
| `param_1` (the `__thiscall` `this`) | `ECX` | `&records[i]` — **the self car** |
| `in_EAX` | `EAX` | `&records[j]` — **the other car** |
| `param_2` | `[esp+4]` | `j`. **UNUSED** — `param_2` occurs exactly **once** in the whole decompilation, in the declaration line. |
| `param_3` | `[esp+8]` | the pass counter. `return param_3 == 0`. |

**The port's parameter names are the other way round from the registers.** The body
`mashedmod/src/mashed_re/Collision/CarCarContacts.cpp:34-35` is

```cpp
float* slotA = vFP(vehA, vehA[0x26a] * 0x10 + 0x256);   // local_a8 (vehA active slot)
float* pfVar2 = vFP(vehB, vehB[0x26a] * 0x10 + 0x256);  // vehB active slot
```

and the decompilation's matching lines are `local_a8 = (float *)(in_EAX + in_EAX[0x26a] *
0x10 + 0x256)` and `pfVar2 = (float *)(param_1 + param_1[0x26a] * 0x10 + 0x256)`. So the
port's `vehA` is `in_EAX` and its `vehB` is `param_1`, i.e.

> **the call is `VehicleCarCarContact(rec(j), rec(i), pass)` — the OTHER car first, the
> self car second.** Calling it `(self, other)` would swap the two bodies.

The TU header's claim that `in_EAX` is *"vehicle A (\"this\", register)"* is wrong about
which register is `this` (`ECX` is), and both plates
(`re/analysis/vehicle_dynamics_d2/00469df0.md`,
`re/analysis/vehicle_promote_c2_b/00469df0.md`) repeat it. The parameter *mapping* in all
three is right; only the word "this" is misplaced. That is a comment-level correction and
no C-level depends on it, but it is exactly the kind of label that would make a wiring
attempt pass its argument pair backwards.

Also recorded: both plates gloss `[699]` as byte `0xAF0`. 699 decimal is `0x2BB`, so the
byte offset is **`0xAEC`**, and the port already has it right
(`CarCarContacts.cpp:188-189`). Comment-level, two plates, no C-level effect.

## 3. The five gates, each with the instruction that implements it

Per self car `i` (outer loop `0x004709f0..0x00470c53`), **inside** its substep structure:

```
for (j = i + 1; j < 0x10; ++j)                          0x00470b11 / 0x00470b12
    if (j >= participantCount) continue;                0x00470b23  cmp ebx,[esp+0x24]
                                                        (count from FUN_0040e340 @0x004709a7)
    if (!(I(recJ,0x004) != 0 || I(recJ,0x010) == 1))    0x00470b2d / 0x00470b37
        continue;
    radSum = (F(recJ,0x4a4) + F(recI,0x4a4)) * 0.75f;   0x00470b44 / 0x00470b50 / 0x00470b70
                                                        (0.75f = _DAT_005cc950)
    cI = recI + 0x958 + I(recI,0x9a8)*0x40;             0x00470b4a / 0x00470b5c / 0x00470b5f
    cJ = recJ + 0x958 + I(recJ,0x9a8)*0x40;             0x00470b56 / 0x00470b66 / 0x00470b69
    if (!(|cI - cJ|^2 < radSum*radSum)) continue;       0x00470b76..0x00470ba9
    if (mode != 6 && mode != 7 && mode != 10 && mode != 0xb) continue;
                                                        0x00470baf..0x00470bc1
                                                        (mode from FUN_0040e350 @0x004709a0)
    if (VehicleCarCarContact(recJ, recI, pass)) {       0x00470bcd / 0x00470bd5
        ++pass;                                         0x00470bde  inc ebp
        anyContact = true;                              0x00470be2  mov [esp+0x1c],0
        ... optional FUN_00467300 ...                   0x00470bd9..0x00470c0f
    }
```

Note `0x00470b5f` is `lea eax,[ecx+esi+0x958]` with `ECX = [ESI+0x9a8] << 6`, so the
centroid comes from the ring slot selected by **`+0x9a8`**, not `+0x9ac`. The matrix the
contact scan transforms by comes from `+0x9ac`. Both selectors are live in the original.

### The retry, and therefore the exact insertion point in the port

`anyContact` is seeded `1` at the top of each substep (`0x00470acb: mov [esp+0x24],1`,
which is `[esp+0x1c]` at loop-head ESP) and cleared on any car<->car contact. After the
`j` loop:

```
0x00470c25: mov eax,[esp+0x1c] / test eax,eax
0x00470c2b: je 0x470ab0        ; a contact happened -> re-run the whole substep chain
0x00470c31:                    ; otherwise: flip the ring selectors and finish this car
```

and the `j` loop is reached from **two** places in the car<->world half, not one:

- `[esi+0x9ec] == 0` (`0x00470af7: je 0x470b0d`), and
- the fixup ran but the scan returned 0 (`0x00470b06: test ebx,ebx` / `0x00470b08: je 0x470b0d`).

In the port both of those fall through to the **same** statement:
`mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1093` — the `break;` that ends the
`for (int pass = 0; pass < 2; ++pass)` retry at `:959`. The third original path
(`[esi+0x9ec] != 0` **and** the scan returned nonzero → `inc ebp; jmp 0x470ab0`) skips the
`j` loop entirely, and the port already matches it with the `continue;` at `:1088`.

> **So the pair loop goes in exactly one place: immediately before
> `VehiclePhysicsRun.cpp:1093`'s `break;`**, and it either `continue`s the retry (contact)
> or falls into that `break` (no contact).

## 4. The hull the SAT reads: the chain, resolved

`0x00469df0` runs a 4-halfplane SAT over four world points at record **`+0xa28`,
`+0xa34`, `+0xa40`, `+0xa4c`** and an axis at `+0x9c8` (measured `(0, 1, 0)` on every
capture). Static search says nothing writes `+0xa28`:

- `py -3.12 re/tools/findconst.py 0xa28` → **2** raw matches in `.text`, both **reads**
  inside `0x00469df0` (`0x00469e58`, and the `0x0046a0ab` second pass).
- `py -3.12 re/tools/findoffset.py --writes 0xa28 0xa2c 0xa54` → **0** accesses.
- Ghidra `--datarefs` on car 0's absolute `0x00881fc8` → **0 references total.**

That is the blind spot memory `findoffset-blind-to-computed-bases` names, and the
producer reaches it through a computed base. Found by reading the one function that owns
that region:

```
FUN_00469aa0 (the contact scan, 0x00469aa0):
  FUN_004c3d90(unaff_ESI + 0x27e, unaff_ESI + 0x18, 0x12, unaff_ESI + iVar4 * 0x10 + 0x24a);
```

i.e. transform **18** points from `+0x60` (stride `0xc`) into **`+0x9f8`** (stride `0xc`)
by the `+0x928` ring matrix. `0x9f8 + 4*0xc == 0xa28`, so **the SAT hull is world points
4..7 of that array.** (Two further consumers of the same array confirm the stride and
base: `FUN_004694e0` at `0x00469591` `lea ebp,[edi+0xa00]` then `add ebp,0xc`, and
`FUN_0046bfc0` at `0x0046c062` `lea ebx,[esi+0xa58]` then `add ebx,0xc` four times.)

Their body-space source is `FUN_0046b1c0`, whose 24 stores at `0x0046b1e3..0x0046b2df`
put, for points 4..7,

```
p4 = (b[0], b[4], b[2])    p5 = (b[3], b[4], b[2])
p6 = (b[0], b[4], b[5])    p7 = (b[3], b[4], b[5])
```

from its 6-float box — **the TOP FACE of the AABB**, which is why all four share a `y`.

### This is checkable without instrumenting anything, and it checks out

The four world points are a rigid transform of that face, so their three edge lengths are
invariants of the box alone. `re/tools/hull_invariants.py` scores them straight out of the
committed MSD1 captures (which carry the whole `0xd04` record). Reference edges computed
from `kContactHullBox` (`mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp:148-155`, the six
values measured on the running original at `0x0063dc10..0x0063dc24`):
**`e01 = 0.437600`, `e02 = 0.977100`, `e03 = 1.070616`.**

| capture | car | non-degenerate frames | `e01` median | `e02` median | `e03` median | within `1e-3` |
|---|---|---:|---:|---:|---:|---|
| `verify/d2_b0c_20261002/orig_sl1.msd` | 0 | 2332 of 2334 | 0.437600 | 0.977100 | 1.070616 | **2332 of 2332** |
| `verify/d2_wheelstate_20261002/orig_ws1.msd` | 0 | 2330 of 2332 | 0.437600 | 0.977100 | 1.070616 | **2330 of 2330** |
| `verify/d2_writer_20261001/orig_bp1.msd` | 0 | 2332 of 2334 | 0.437600 | 0.977100 | 1.070616 | **2332 of 2332** |
| `verify/d3_spect_20261005/s1.msd` | 1 | 3284 of 3286 | 0.437644 | 0.977100 | 1.070616 | **3284 of 3284** |

Largest median deviation anywhere: **4.43e-05** (`s1`, `e01`); the other eleven are
`<= 8.94e-08`. **10,278 of 10,278 non-degenerate frames within `1e-3`, on four captures
and two cars.** The denominator is printed because an all-zero hull is a pre-race frame,
not a failure — two per capture.

This gate could have failed: if the hull were any other four of the 18 points, or the box
were wrong, the edge lengths would not land on 0.437600 / 0.977100 to eight decimals.

### Consequence: one STALE CODE COMMENT — and U-9155's own row is NOT the stale thing

**Read this distinction before quoting the next paragraph.** U-9155 names **two**
producers and only one of them is open:

- the producer of the record's 14 body points at `+0x90..+0x137` — **`FUN_0046b1c0`,
  identified, C3, and called by the exe build.** The paragraph below is about this one.
- the producer of the **6-float box** at `DAT_0063d9e0 + slot*0x2ac + 0x230` that
  `FUN_0046b1c0` consumes — **still open**, and `UNCERTAINTIES.md`'s U-9155 row is
  correct and current on it (structure corrected 2026-09-30, search bounded to
  `0x0041ec0f..0x0042089b`). **Nothing here closes it.**

The §4 measurement is a **third** independent witness for the box seed, after the
`--peek` samples that measured it and `re/analysis/D2_REOPEN_2026-09-29.md` §16.6's read
of `rec+0x90..+0x137` out of `orig_solo3.msd`. It adds a different kind of evidence — a
geometric invariant over 10,278 world-space frames rather than a byte comparison of the
body-space points — and it still does not name the box's writer.

`mashedmod/src/mashed_re/Collision/ContactStubs.cpp:100-110` still says *"the port's A3
… writes only the FIRST FOUR (the wheel points) and leaves points 4..17 zero … Their
producer is NOT yet identified."* It **was** identified, in the same session that wrote
that paragraph: `VehicleInit.cpp:98-200` carries `VehicleBuildContactHull`, a
record-base-relative exe-side body for `0x0046b1c0`, and `VehicleInit.cpp:196` calls it.
`0x0046b1c0` is **C3 `impl`** in `hooks.csv` (`Vehicle/VehicleSlotAabbExpand.cpp`,
`frida_diff log/diff_vehicle_slot_aabb_expand.csv`) — a deliberate dual copy, registered
in `re/tools/dual_copy_allowlist.txt` as CROSS-TARGET.

**So the port DOES populate body points 4..17 and DOES run the transform**
(`CarWorldContacts.cpp:448`, bound to `Math::RwV3dTransformPointsCPU` at
`ContactStubs.cpp:126-131`). The ContactStubs paragraph is a **stale code comment**, not a
gap, and not a stale tracker row. Correcting it goes through `re-classify` with the
U-9155 row left OPEN on the box-producer question, not closed.

## 5. The ONE thing the port cannot supply: ring slot `[+0x9a8]`

Both `+0x9a8`-selected and `+0x9ac`-selected ring slots are live on the original.
Measured over the same four captures, `+0x9a8 == 0` and `+0x9ac == 1` on **every** frame
(2334 / 2332 / 2334 / 3286 — the selector flip at `0x00470c31` returns to the same pair
by the sample phase), and at `orig_sl1` frame 1167 slot 0's translation row is
`(-0.9237, 0.4913, -0.2017)` while slot 1's is `(-0.9611, 0.4910, -0.2139)`. Slot 0 is
all-zero on **1 of 2334** frames, slot 1 on **1 of 2334**.

In the port, slot 0 is **never written**:

- `mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp:252` — `WI(rec, 0x9a8, 0); WI(rec, 0x9ac, 1);`
  and nothing else in `mashedmod/src/` writes either selector (`grep 0x9a8|0x9ac`).
- `SyncContactRingMatrix` (`VehiclePhysicsRun.cpp:378-387`) and the broadphase-centre
  write (`:443-447`) both take `sel = I(r, 0x9ac)`, i.e. **1**, so they publish at
  `+0x968` / `+0x998`. Nothing publishes `+0x928` / `+0x958`.

Independent empirical witness, already committed and already noticed: `rec_958` and
`rec_960` are **0.0 on 7705 of 7705 rows** of `verify/d3_arm_20261005/A1.csv`, while
`own_x` / `own_z` on the same rows are nonzero on 7705 of 7705. `re/tools/ai_yawrate.py:71-73`
states the same thing in writing and substitutes `own_x`/`own_z`, so U-9191 leg 3 is not
affected and **this is not a new defect to file.** Only
`D3d9Render/TrackRenderer.cpp:4000`'s comment — *"the record's own world X / Z … so it
cannot be contaminated"* — reads as if those columns carried a position. They carry zero.

### Why that blocks the wiring, with the two lines it blocks

`CarCarContacts.cpp:93` and `:97` use the two centroids as the **lever arms** for the
angular impulse:

```cpp
float vAB[3]  = { local_124 - pfVar2[0], local_120 - pfVar2[1], local_11c - pfVar2[2] };
float vB[3]   = { local_124 - slotA[0],  local_120 - slotA[1],  local_11c - slotA[2] };
```

With both centroids at `(0,0,0)` those arms become the contact point's absolute world
position — on the measured captures that is tens of units where the real arm is ~0.5 —
and the proximity gate in §3 degenerates to `0 < radSum²`, i.e. always true for every
pair on the track. So the car<->car call cannot be wired faithfully until slot 0 carries
the pose.

### And publishing slot 0 is NOT inert — this is the part to measure, not assume

Three sites already read the `+0x9a8`-selected slot and currently get a **zero matrix**:

- `mashedmod/src/mashed_re/Vehicle/VehicleControl.cpp:103` — A4's `wheelBlock`
  (`reinterpret_cast<char*>(v) + Ib(v, 0x9a8) * 0x40 + 0x928`, citing `0x0047068e`)
- `mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp:536` — same expression, same RVA
- `mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp:2749` — same, for A6b

`BodyOrientationIntegrate.cpp:171-179` already flags the reconciliation as owed:
*"we keep it in caller-owned storage instead … moving it into +0x928 is a follow-up that
requires reconciling the contact-ring"*. So **publishing slot 0 changes what A4 and A6b
are handed**, which is squarely inside the D2-certified player solver. It can move (e) and
(b) in either direction and must be gated on its own, before any car<->car call is added.

`+0x4a4`, the radius the proximity gate sums, is **not** a gap: it is
`0.6780367493629456` and constant on all four captures and both cars, and the port writes
it at `VehicleInit.cpp:317` as `maxr * 1.05` from the same hull.

## 6. What this establishes, and what it does not

**Establishes.** One call site with its ABI, its five gates and its retry semantics, read
from instructions. The port's single insertion point. That the argument pair is
`(other, self)` and not `(self, other)`. That the SAT hull's whole production chain is
already in `mashed_re.exe` and is geometrically exact to `<= 4.4e-05` on 10,278 frames.
That the blocker is one unpublished ring slot, and that publishing it is a physics change
with three existing readers rather than an inert addition.

**Does not establish.** Anything about whether the car<->car contact closes criterion (b)
— the 2026-10-02 counterfactual matrix had no arm passing (b) on any car, and nothing here
changes that. Nor that `VehicleCarCarContact`'s body is correct: it is **C2** (faithful
transcription, no behavioural diff), and giving it a call site does not promote it.
Nor anything about `U-D2-OPPONENT-COUPLING` — the opponents already move the player's
median speed 2538 → 691 with `0x00469df0` never running (`ROADMAP.md:1929-1932`), so that
coupling is shared mutable state and is a different question from this one.
