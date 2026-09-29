# D3 powerups criterion (c) — the contact chain, ported and measured (2026-09-28)

Branch `race/first-frame-parity`. Continues `D3_CONTACT_2026-09-27.md` (commits
`f39747af`, `61b06778`), whose §8 left three open items and whose §7 recorded
criterion (c) as **1 clean / 1 diverges / 7 blocked**.

Anchor: every byte-level claim was disassembled from `original/MASHED.exe.unpatched`
(SHA-256 `BDCAE093…3C0E`, the pinned anchor) with `re/tools/disasm_va.py`, or
decompiled from a read-only Ghidra pool clone with `re/tools/decomp_pc.py`. The
Ghidra MCP did not load; no Ghidra project was opened for writing.

## 0. Headline

1. **Criterion (c) went from 1 clean / 1 diverges / 7 blocked to 8 clean +
   the sweep + the shared acquisition, 1 near-clean.** OIL, P_MINE, SHOTGUN,
   DRUM, FLASH, GUN, MORTAR and MISSILE all measure CLEAN call site by call site
   against fresh original-side captures (§4); **R_FLAME** is the only residue,
   exact on two of three captures with its 2-of-546 diagnosed to a named unported
   routine (§4.2d). P_MINE moved from
   **78 decision mismatches → 0** on the same `c2` capture the prior note measured
   it on (§4.3).
2. **The dispatcher's armed sweep is ported** (§3.3), including the deactivation
   branch at `0x0045bcf7`. That was the last residue in `c2`: the four remaining
   P_MINE mismatches were all at `t+82`, the single frame the sweep fired.
3. **`D3_CONTACT_2026-09-27.md` §8 item 1 is RESOLVED** (§2): the gate that refused
   6 of P_MINE's 7 press edges in `c2` is the **first** one, `FUN_004b4cd0 == 0`
   (`JE 0x00457e08` at `0x00457caa`). `FUN_0045c110` is now instrumented and
   returned 0 on every power-up call in two fresh captures.
4. **Two corrections to the prior note's §5 table** (§5), plus a decoded map of
   the five types still blocked (§5.3): DRUM's contact chain is
   gated on its own `0x004b4cd0` call at `0x0045444f` (the prior table listed only
   the two placement leaves), and the chain sites it attributed to MORTAR/GUN by
   activation window include other slots' and other systems' calls — the OIL and
   P_MINE rows are the only ones the window attribution gets right, because those
   two sites are reachable from one type only.
5. **No regression.** The 9-type decision replay is CLEAN on the archived
   `o3`/`o4`, on the new `m1`, and on all five current captures (§6).
6. **All four projectile types are closed** (§4.2d–§4.2h). Their contact sites
   sat inside per-type projectile updates, and each is now ported with its own
   non-degeneracy control. What is NOT closed is a separate, older thing: a
   **decision-half** defect on a MORTAR-then-MISSILE sequence, found by a new
   capture and shown by a control build to predate this session (§4.4b).
7. **`FUN_00459620` is SHARED — and it is a TARGET-ACQUISITION routine, not a
   projectile one** (§4.2e, §5.2). A call-site scan of the whole `.text` finds
   three callers: `0x00453bd9` (MORTAR tick), `0x00455c29` (MISSILE), `0x004569c5`
   (GUN tick). **It is now PORTED and measures clean**, including both
   non-degeneracy controls. Doing so **closed GUN outright** and unblocked MORTAR
   and MISSILE, whose only remaining debt is their own projectile integration.
   It also required adding the capture channel the replay was missing — see (8).

## 1. What the chain actually is

| RVA | what it does | evidence |
|---|---|---|
| `0x004b4cd0` | copies 6 dwords (a 2-point segment) to a local, writes tag `1`, tails `FUN_004b4c80` | decomp `FUN_004b4cd0` |
| `0x004b4c80` | `local_14 = 0`; `FUN_00538c80(world, seg, FUN_004b4bb0, &local_10)`; `return local_14` | decomp |
| `0x004b4bb0` | the collector. First hit stores `t`; later hits are written only when `t < bestT` (`LAB_004b4c68`); the count `*piVar1` increments on **every** candidate. Writes `puVar2[0..0xf]` | decomp `0x004b4bd4..0x004b4c5f` |
| `0x004b4650` | `out[i] = a[i] + t*(b[i]-a[i])`, a pure vec3 lerp (already C3 as `Lerp4b4650`) | decomp; hooks.csv `004b4650` |
| `0x004b5080` | orthonormal RwMatrix from the hit triangle: `right = norm(v0-v1)`, `at = norm(cross(A,B))`, `up = cross(at,right)`, `pos = 0`, flags `&= 0xfffdfffc`, then `RwMatrixTranslate(m, pos, 2)` | decomp |
| `0x0045c110` | `uVar2 = *(uint*)(param_1+4)` — the hit triangle's RpMaterial RwRGBA — `return 1` for `0xff010101` and `0xffff0080` (`0x0045c116..0x0045c129`), else a compare tree over `FUN_00472550` opcodes, else 0 | decomp |
| `0x004b4b60` | 4-dword copy, tag `3` (a sphere), tails `FUN_004b4a80` → the same walk with collector `FUN_004b49b0` | decomp |

So the return of `0x004b4cd0` is an **intersection count**, and the `0x40`-byte
result buffer holds the nearest hit. Offsets, confirmed twice over (by the
collector's writes and by both readers):

```
+0x00  normal[3]      +0x0c  triangle index      +0x10  3 vertices (9 floats)
+0x34  walked object  +0x38  t                   +0x3c  walker user data
+0x40  a 16-float RwMatrix scratch the caller hands to 0x004b5080
```

### 1.1 OIL, `FUN_00457800`, fully decoded

`0x00457800..0x00457a20`. Three exits, and they differ in whether the supply is
charged:

| exit | branch | supply `DAT_0068a250[owner*16]` |
|---|---|---|
| A distance gate | `FCOMP [0x5cc56c]` `0x0045785a`, `TEST AH,5` `0x00457866`, `JNP 0x00457a18` `0x0045786b` | **unchanged** |
| B query miss | `CALL 0x004b4cd0` `0x004578cc`, `TEST EAX,EAX` `0x004578d4`, `JE 0x00457a0e` `0x004578d7` | **charged** (the jump lands ON the decrement) |
| C surface refuse | `CALL 0x0045c110` `0x00457909`, `TEST EAX,EAX` `0x00457911`, `JNE 0x00457a18` `0x00457913` | **unchanged** |

Body: trail store `lastDrop[owner] = pos` at `0x00457874..0x00457881`
(`&DAT_0068a290[owner*3]`, `LEA EDX,[EAX+0x68a290]` `0x0045782c`); segment
`pos -> pos - (0, _DAT_005cc320, 0)` (`FLD [esp+0x2c]` `0x004578a3`,
`FSUB [0x5cc320]` `0x004578a7`); material lookup
`mat = (*(res+0x3c))->[0x10][ tri[res+0x0c].u16@+6 + (*(res+0x34))->u16@+0x80 ]`
(`0x004578dd..0x00457905`); lerp `0x0045792d`; impact `+= normal * _DAT_005cd18c`
(`FMUL` at `0x00457936` / `0x0045795a` / `0x00457972`); basis `0x0045797a`; then a
random yaw about `{0,0,1}` (`FUN_00472650(0,360.0,1)` + `FUN_004c4d20`), a ±0.2 xz
jitter (`FUN_004c51a0` combine 2), the slick spawn `FUN_004577f0 -> FUN_00456eb0`,
and FX `FUN_00465e80(0x17)`. Decrement `FLD [ESI]; FSUB [0x5cc56c]; FSTP [ESI]` at
`0x00457a0e`.

Constants read from the anchor: `_DAT_005cc56c = 0.1f` (it is BOTH the squared
distance threshold and the decrement), `_DAT_005cc320 = 1.0f`,
`_DAT_005cd18c = 0.04f`, `0x00614708 = {0,0,1}`.

### 1.2 P_MINE, `FUN_00457ef0` → `FUN_00457c10`

`FUN_00457ef0` has **no ammo test**: `MOV ESI,[EAX+0xac]` `0x00457ef5`,
`CMP [ESP+0xc],2` `0x00457efb`, `JNE 0x00457f21` `0x00457f00` (bare `POP ESI; RET`),
`CALL 0x00457c10` `0x00457f04`, then `MOV [ESP+4],0x1b` / `JMP 0x00465ca0`
`0x00457f1c` — the FX runs **whether or not the drop happened**.

`FUN_00457c10`: segment `A = car+0x30..0x38`,
`B = A + up * _DAT_005cc33c(-1.0) * _DAT_005cc32c(0.5)` where `up` is the car world
matrix's second row (`+0x10/+0x14/+0x18`); gate 1 `JE 0x00457e08` at `0x00457caa`;
gate 2 `JNE 0x00457e08` at `0x00457cfe`; `0x00457e08` is
`POP ESI; ADD ESP,0xb4; RET`, so **a refused gate leaves no state change**. The
only state change is `MOV EDI,[ESI+8]` `0x00457d29` / `DEC EDI` `0x00457d2c` /
`MOV [ESI+8],EDI` `0x00457d2d`, and it sits after both gates and after the lerp.
Then `impact += normal * _DAT_005cd0ec(0.005)`, `FUN_004b5080`, a `-90°` rotation
about `{1,0,0}` (`0xc2b40000`), `FUN_004c1480`, `RwFrameAddChild`.

## 2. RESOLVED — which gate refused P_MINE's 6 drops

`re/frida/scenario_launch.py`'s `PU_CONTACT` gained `0x0045c110` → `surface_gate`
(additive; a per-RVA cap `PU_CX_CAP` was added at the same time, 4000 for this one,
because it has callers outside the power-up path). Two fresh captures, same recipes
as the prior note's `c2`/`c3`:

```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
   --poke-ctrl-slots --statediff-out verify/d3_contact_20260928/g2.msd \
   --statediff-car 0 --statediff-drive --statediff-puhook --puhook-contacts \
   --pu-plan 11,7,10,12 --pu-warm 60 --hold 110
# g3: same with --pu-plan 19,16,18,9,17
```

Both muted, both launched and killed by the launcher, no crash: each 6650 frames /
6653 dispatcher calls (the recipe is reproducible at this level). `surface_gate`
fired 350 times in `g2` and 81 in `g3`, and was not hot in either.

MEASURED (`verify/d3_contact_20260928/g2.contact.txt`):

```
query_4b4cd0  0x4b4cd0 from 0x457ca5  calls=3  ret!=0=3
surface_gate  0x45c110 from 0x457cf9  calls=3  ret!=0=0
query_4b4650  0x4b4650 from 0x457d1f  calls=3  ret!=0=3
query_4b5080  0x4b5080 from 0x457db2  calls=3  ret!=0=3
```

Three press edges, three hits, three allows, three drops. The gate-2 call count
equals the gate-1 nonzero count exactly, which is the control-flow model. In `c2`
the same site recorded 7 calls / 1 nonzero and exactly 1 decrement, so the 6
refusals were all `JE 0x00457caa` — **gate 1, the world query**. `FUN_0045c110` has
not been observed refusing a power-up drop in any capture.

## 3. The port

New TU `mashedmod/src/mashed_re/Powerup/PowerupContact.{h,cpp}` (in
`exe_sources.rsp`, and in `re/tools/pu_replay/build.bat`).

### 3.1 What is ported and what is a stand-in

**Ported verbatim**: the decision structure — which leaf is called, in what order,
which branch each return takes, and what state each outcome changes. `0x004b4650`
and `0x004b5080` are ported as expressions, instruction-cited.

**Stand-in, stated plainly**:
- the BSP walk `FUN_00538c80` over `COLLI*.BSP`. `SegmentQuery` reproduces the
  collector's **result rule** (count every intersection, keep the smallest `t`)
  over the standalone's flat collision soup — the same triangles
  `TrackRenderer::GroundProbe` and the wheel solver already use, fed through a
  `TriSource` hook installed in `TrackRenderer::EnsurePowerupBackend`.
- the RpMaterial colour channel. `col_mat_` carries a material **index**;
  `FUN_0045c110` keys on the material's RwRGBA at `+4`. `TriSource` reports
  `matKey = 0`, so `SurfaceGate` allows. That is the **measured** behaviour on both
  power-up sites (§2), not an assumption.
- P_MINE's segment direction. `HostCar` has no matrix `up` row, so the port probes
  world `-Y` rather than the car's `-up`. **[UNCERTAIN]** — it moves the impact
  point on a banked surface, not the gate counts. To resolve: carry the up row in
  `TrackRenderer::SyncHostCar`, then re-run
  `py -3.12 re/tools/pu_contact_report.py <capture>.msd --slot 0`.
- `FUN_004c39b0` (`RwV3dNormalize`) is the CPU sqrt, preserving the original's
  `len² == 0 ⇒ zero vector` arm (`0x004c39d5`).

This is **C2-grade**. No tracker promotion is claimed.

### 3.2 OIL, P_MINE, SHOTGUN and DRUM

`Oil_Fire`, `PMine_Fire`, `Shotgun_Fire` and the DRUM pair `Drum_Fire`/`Drum_Tick`
in `Powerup/PowerupEffects.cpp` now run the chain with each branch RVA-cited
inline. The P_MINE `s.ammo == 0` guard is kept and marked as
port-added (the original has none; its CANFIRE is what stops the pool index going
negative), and `SfxByName` moved outside the gates to match `0x00457f1c`.

### 3.3 The dispatcher's armed sweep

Disassembled `0x0045bca0..0x0045bd1b`. `EDI = slot + 0x90`, `EBP = EDI - 0x10 =
slot + 0x80`; the dispatcher refreshes three floats at `slot+0x80` from `[EBX]`
every pass (`0x0045bcb2..0x0045bcca`), then:

```
0x0045bcd3  CALL 0x004b4b60   (world, slot+0x80, &result)   -> sweep_query
0x0045bcdd  TEST EAX,EAX / JE 0x45bd14
0x0045bce5  CALL 0x0045c350   (&result, slot+0x80)          -> sweep_confirm
0x0045bcef  TEST EAX,EAX / JNE 0x45bd14
0x0045bcf7  CALL 0x0045bac0   deactivate       <- the capture's deact_ra 0x45bcfc
0x0045bd0c  CALL 0x00476880   (slot+0x80, 0x6146fc, 360.0f, 1.5f)   FX
0x0045bd14  MOV EAX,[EDI+0x1c] / JNE 0x45bd4e  <- armed RE-TESTED before CANFIRE
```

Ported into `PowerupSystem::Tick` in that order — **before** CANFIRE, with the
armed re-test. `slot+0x8c` (the sphere radius) has no writer in the ported
lifecycle, so with no injector `SweepQuery` returns 0 and the branch is inert in
the shipping build: identical to the standalone's behaviour before it existed.
MEASURED original rate: 1 hit in 3795 slot passes (`c2` 1/1016, `c3` 0/1348,
`g2` 0/1431, `g3` 0/1448).

### 3.4 Measuring it: `MASHED_PU_CONTACTDUMP` + injected verdicts

`PowerupContact.cpp` writes `MASHED_PU_CONTACTDUMP` in the **exact** column shape
of `scenario_launch.py`'s `<out>.pucontact.csv`, with the original's own
`ret_addr` per call site and, for the sweep, `a2 = 0x0088fbe0 + slot*0xb4 + 0x80`
— so `re/tools/pu_contact_report.py` reads an original capture and a port run the
same way, including its slot rule.

`re/tools/pu_replay` gained a second injected input (it already injected OIL's
distance gate). With `<base>.pucontact.csv` present it feeds the original's
MEASURED `FUN_004b4cd0` / `FUN_0045c110` / sweep verdicts back into the port,
keyed on **(dispatcher call, call-site return address)** — and on slot, via the
`a2` rule for the sweep and via `code_pre` for the per-type sites. What that
measures is the **call structure and the state effect of each outcome**, not the
query itself. Two attribution traps were hit and fixed while building it:

1. rows outside the replayed call range are not divergences (the capture spans the
   whole race);
2. a `.pucontact.csv` row carries **no slot**, and these sites are per-type code
   every slot runs. `g2` has three `0x457ca5` rows, only two of which are slot 0's
   — the third lands inside slot 0's MORTAR window and is another slot's P_MINE.
   Without the `code_pre` filter that reads as a port under-count.

Captures with no `.pucontact.csv` (the archived `verify/d3_pu_20260926/*`) run
under the permissive verdict and print a banner saying criterion (c) is not tested
there — otherwise a missing input would read as a regression.

## 4. MEASURED — criterion (c), per call site

`verify/d3_contact_20260928/{c2,c3,g2,g3}.replay.txt`.

### 4.1 OIL — CLEAN (`g3`)

| site | ret_addr | orig | port | port-only | orig-only |
|---|---|---|---|---|---|
| OIL query `0x004b4cd0` | `0x4578d1` | 10 | 10 | 0 | 0 |
| OIL gate `0x0045c110` | `0x45790e` | 10 | 10 | 0 | 0 |
| OIL lerp `0x004b4650` | `0x457932` | 10 | 10 | 0 | 0 |
| OIL basis `0x004b5080` | `0x45797f` | 10 | 10 | 0 | 0 |
| SWEEP query / confirm | `0x45bcd8` / `0x45bcea` | 309 / 0 | 309 / 0 | 0 | 0 |

`CONTACT VERDICT: CLEAN`. This is the exact acceptance the prior note's §8 item 3
set for OIL ("one triple per drop, 10 drops, all 10 succeeding").

### 4.2 P_MINE — CLEAN (`g2`)

| site | ret_addr | orig | port |
|---|---|---|---|
| P_MINE query `0x004b4cd0` | `0x457ca5` | 2 | 2 |
| P_MINE gate `0x0045c110` | `0x457cf9` | 2 | 2 |
| P_MINE lerp `0x004b4650` | `0x457d1f` | 2 | 2 |
| P_MINE basis `0x004b5080` | `0x457db2` | 2 | 2 |
| SWEEP query / confirm | `0x45bcd8` / `0x45bcea` | 207 / 0 | 207 / 0 |

`CONTACT VERDICT: CLEAN`.

### 4.2b SHOTGUN — CLEAN (`g4`)

SHOTGUN's contact site is the one of the six that is **not** in a projectile
update: `FUN_0045b390`, the pellet detonation, is reached straight from FIRE
`0x0045b6e0`, which the port already runs. `0x004b4b20` — its query — was not in
`PU_CONTACT`, so the prior note could count the `0x45b582` basis calls but not the
gate that produced them. Added, and `g4` captured with the `c3` recipe:

| site | ret_addr | orig | port |
|---|---|---|---|
| SHOTGUN query `0x004b4b20` | `0x45b4c2` | 8 | 8 |
| SHOTGUN basis `0x004b5080` | `0x45b582` | 7 | 7 |

4 pellet fires × the 2-iteration loop (`local_c0 = 2` at `0x0045b3c2`) = 8 queries;
7 hit, so 7 basis calls — the one miss is reproduced too. `g4` also carries OIL
10/10/10/10 and the sweep 309/309: `CONTACT VERDICT: CLEAN`.

`0x004b4b20` is `FUN_004b4a80`'s wrapper, not `FUN_004b4c80`'s: same 6-float
segment and tag 1 (`MOV [ESP+0x24],1` at `0x004b4b33`), but collector
`FUN_004b49b0`, which fills an array up to a capacity (SHOTGUN passes 1,
`PUSH 1` at `0x004b4b31`) and therefore keeps the **first** hit, not the nearest.
Noted in `PowerupContact.h`, not modelled.

### 4.2c DRUM — CLEAN (`g2` and `c2`)

DRUM is the first PROJECTILE type measured clean, and it needed a new attribution
rule to be measurable at all. MEASURED on `g2`: slot 0 held DRUM for dispatcher
calls **1181..1190**, but its `0x45444f` queries run **1182..1242** — 95 of them,
two drums in flight from 1189, landing at 1222 and 1242. **The dropped drum
outlives the slot by 52 calls**, so the `code_pre` filter §3.4 uses for OIL/P_MINE/
SHOTGUN cannot see it.

`pu_replay` gained site `mode = 1`: attribute by call range only, valid **only when
no other slot held that type in the capture**, which the loader now CHECKS from the
puhook rows (status `contested`, excluded, otherwise). In `g2` the codes are
slot 0 `{7,10,11,12}`, slot 1 `{19}`, slot 2 `{12}` — DRUM is slot 0's alone.

| capture | query `0x45444f` | lerp `0x45448a` | basis `0x4544e0` |
|---|---|---|---|
| `g2` | 83 / 83 | 1 / 1 | 1 / 1 |
| `c2` | 83 / 83 | 2 / 2 | 2 / 2 |

(83, not 95, because the replay's call range ends before 1242; both sides stop at
the same call.) Two independent captures, both `CONTACT VERDICT: CLEAN`.

The port adds an 8-record pool to `Powerup/PowerupEffects.cpp` — the size the
original's own TICK loop declares, `CMP puVar3,0x688240` at `0x0045488a` over base
`0x00688020` stride `0x44` — and the verbatim state machine `0 free / 1 on the car /
2 flying / 3,4,5 one post-landing frame each`. Flight: `pos += vel*dt`,
`vel.y += _DAT_005ce430(-5.0)*dt`, `spin += _DAT_005ce438(720)*dt`,
`life += dt`, gated on `life <= _DAT_005cc358(5.0)`.

Note why the count is reproducible without faithful flight: the query count is
(frames in state 2 until the hit or the life gate), the drop frame comes from the
replayed press edge, the hit frame comes from the injected verdict, and `life`
integrates the dt the capture supplies exactly. The trajectory decides *where* it
lands, not *how many* queries it makes. That will not hold for a type whose flight
can end some other way.

### 4.2d R_FLAME — exact on two captures, 2 queries over on the third

R_FLAME is a 100-record spark pool (4 owners × 25), decoded in §5.3, and its
emission is driven by the two counters `RFlame_Tick` already tracked.

| capture | query `0x45afcc` | lerp `0x45aff3` | basis `0x45b04c` |
|---|---|---|---|
| `c3` | **510 / 510** | 24 / 24 | 24 / 24 |
| `g4` | **547 / 547** | 23 / 23 | 23 / 23 |
| `g3` | 546 / **548** | 23 / 23 | 23 / 23 |

The `g3` residue is **2 queries out of 546**, on the query site only, and it is
diagnosed rather than assumed:

- Reconstructing the emission calls from the per-call query counts and the landing
  calls gives **25 emissions at identical calls on both sides** (986, 988, 990,
  992, 994, 1001, … 1041).
- The **landing calls are identical**, all 23, last at 1059.
- The per-call query counts match **exactly from 986 through 1087**; the port alone
  queries once at 1088 and once at 1089.
- Ruled out by measurement, not by argument: the `_DAT_005cd114` ULP (simulating
  the float32 accumulation at the captured `dt = 0x3c888888` gives a 55-frame
  cutoff for **both** 0x3f8e38e3 and 0x3f8e38e4 — the port carries the correct bits
  anyway), and a frame-vs-dispatcher-call drift (the `frame` and `call` columns
  advance 1:1 across the whole window, 101 and 101).
- **Mechanism.** `FUN_0045ac40` (`0x0045ac40`), which the original's tick calls
  once per group of 5 immediately before the inner spark loop, **distance-sorts the
  group** — it swaps whole `0xd`-dword records around a reference point that comes
  from `FUN_004671d0(0)`. The port scans in fixed index order. When two sparks of
  one group query on the same dispatcher call and only one hit is recorded, the
  sort decides which one consumes it; the other survives and keeps probing. That
  produces exactly this signature: same emissions, same landings, same counts until
  the tail, then a surviving spark living a few frames longer.

Not ported, because the sort's reference point (`FUN_004671d0`) is a viewport
query the replay has no equivalent for. Recorded as the cause and as the residue's
bound: 2 of 546 on one of three captures, on a count only.

### 4.2e The ACQUISITION routine — ported, and the blocker it removed

`FUN_00459620` is **not** "the shared projectile routine" this note's own §5.2
called it. It acquires a **target**: it scores every other car in a cone, picks
the smallest angle, probes line of sight to it, and writes an aim record at
`0x0068b9f8 + slot*0x58`. No projectile position is integrated anywhere in it.
The record base is read off `IMUL EBP,EBP,0x58` (`0x00459632`) + `ADD
EBP,0x68b9f8` (`0x00459638`); the decompiler's `&DAT_0068b9fc` names the +4 field.

Its two query sites sit on opposite sides of one branch, `candidate count == 0`:

| site | when |
|---|---|
| `0x459c19` | the **vertical fallback** probe at `origin + at*range`, `== 0` side only |
| `0x459d54` | the **line-of-sight** probe, origin → endpoint, every call |

**Two corrections to §5.3's GUN row**, both from `pu_contact_report.py` on the
captures this note already cites. `0x459c19` on `g3` is **163 calls / 84 hits**,
not "96 calls / 0 hits", and `0x459c3c` (84) is inside `0x459c19`'s own hit arm
(`CALL 0x4b4cd0` `0x00459c14`, `TEST EAX,EAX` `0x00459c1c`, `JE 0x459c9a`
`0x00459c1e`, `CALL 0x4b4650` `0x00459c37`) — so a 0-hit query paired with an
84-call lerp was impossible on its face.

**The branch is exercised both ways, so no stub could be right:**

| capture / type | `0x459c19` calls | of hold |
|---|---|---|
| `g3` GUN | 163 | 163 — never a candidate |
| `g4` GUN | 163 | 163 — never a candidate |
| `g2` MISSILE | 0 | 32 — a candidate on every call |
| `g2` MORTAR | 96 | 153 — split |

**THE BLOCKER, named and then removed.** Reproducing that branch needs the four
car positions, their active flags and the firing car's aim matrix. The capture
recorded none of them, and `pu_replay`'s `Backend::car[4]` is **never populated**
— default-constructed all-zero, with nothing in `main()` writing to it. That, not
the contact chain, is what had blocked all three types.

So the input was added rather than guessed at. `scenario_launch.py --puhook-aim`
writes `<out>.puaim.csv`, one row per acquisition call: the four arguments, the
`at` row, the second list's count, and all four cars' flag + position. Sources:
car position `0x0063dc38 + car*0x2ac` (`FUN_0041f030`), active flag
`[[0x005f2770] + car*4 + 0x34]` (`FUN_0040e370`), list count `[0x0063a5d0]`
(`FUN_004075a0`). The `at` row is read on `FUN_0041f220`'s **leave**, filtered to
RA `0x004596b5`, because `FUN_00459620`'s epilogue overwrites the same buffer.
Both attachments are at function entries, never mid-function.

**Falsified before it was ported.** `re/tools/aim_model.py` predicts the branch
from those inputs and diffs it against the same run's contact rows:

```
aim calls 348   list_n {0: 348}   0x459c19 fired on 187   0x459d54 on 348
BRANCH PREDICTION   match=348  mismatch=0     VERDICT: CLEAN
```

**Ported and measured** (`Powerup/PowerupAim.{h,cpp}`), on
`verify/d3_contact_20260928b/m1.msd` — 6657 frames, MORTAR + GUN + MISSILE:

| site | orig | port | |
|---|---|---|---|
| `0x459c19` fallback query | 187 | **187** | clean |
| `0x459c3c` fallback lerp  | 33  | **33**  | clean |
| `0x459d54` LOS query      | 348 | 348 | clean *(schedule-derived, see below)* |
| `0x459db5` LOS lerp       | 152 | **152** | clean |

**Non-degeneracy, run and reported** (`MASHED_AIM_FORCE`, in the TU):

| control | `0x459c19` port | |
|---|---|---|
| `none` — loop always finds nothing | 348 (161 port-only) | DIVERGES |
| `lock` — loop always finds one | 0 (187 orig-only) | DIVERGES |
| *(unset, the real run)* | **187** | CLEAN |

**What this does NOT test, plainly.** The **schedule** comes from the capture:
`Acquire` runs on exactly the calls the original ran it on, because the ported
MORTAR/GUN/MISSILE ticks do not own their projectile pools yet. So the LOS site's
348 is true by construction and is **not** evidence. The fallback site's 187 is
the measurement — it is decided entirely by the ported candidate loop, which the
two controls confirm.

Three things inside the routine are **not** ported, each for a stated reason:
the **second candidate list** (`FUN_004075a0`/`FUN_004075b0`) — a *measured*
no-op, `list_n == 0` on all 348 rows, carried as an input so a non-zero capture
fails loudly; the **third loop** over RW atomics (`FUN_0047ce70`/`FUN_0047d130`)
— makes no contact call, so it cannot move a criterion-(c) count; and the locked
branch's **intercept prediction** (`FUN_0041f2c0`, with `FUN_00558b40` on its
refusal) — `[UNCERTAIN]`, the capture records only 3 of the 4 position dwords
(`tgt.w` is missing) and nothing about `FUN_0041f1c0`'s gate. Next command:
`py -3.12 re/tools/decomp_pc.py 0x0041f2c0 0x0041f1c0 0x00558b40 --slot 0`.

One correction the same read forced: `FUN_004726f0` is a **clamped** dot product,
bounded to `[_DAT_005cc33c, _DAT_005cc320] = [-1, +1]`, which is what keeps the
`FUN_004a3384` acos in domain. `_DAT_005cc98c = 0x42652ee1 = 57.29578` (180/π),
so the cone limit is **20.0 degrees** and the range **15.0**.

### 4.2f MORTAR's own chain — ported, and measured on its INTEGRATION

MORTAR's own contact site `0x453789` fires once per airborne projectile per
frame, so its count comes out of the projectile's per-frame integration. Both
halves are now ported (`Powerup/PowerupMortar.{h,cpp}`):

| RVA | what |
|---|---|
| `0x00453730` | the detonation test. Probes `pos → pos + delta`; on a hit the gate allows, blows up and returns 1. One caller, `0x004538fe` |
| `0x004538b0` | the per-frame update. Its **entire body** sits inside `if (FUN_00453730() == 0)`, so a detonation leaves the record untouched — age included |

**Pool**, disassembled from the tick's own second loop: `MOV ESI,0x684ea8`
(`0x00453c28`), `ADD ESI,0x110` (`0x00453c45`), `CMP ESI,0x6870a8`
(`0x00453c4b`) → base `0x00684ea8`, stride `0x110`, **32 records**.

**The arc.** Y is *not* integrated: `pos.y = h(age) * 1.5 + baseY`, with `h` a
half-sine in normalised age for the first 1.625 s and a falling line after. The
velocity's Y component is computed in the homing branch and then never used.
That is what makes a mortar lob rather than fly straight.

**Falsified offline first** (`re/tools/mortar_model.py`, on
`verify/d3_contact_20260928b/m2.msd`, 327 updates, 88 of them homing):

```
INTEGRATION  match=326  mismatch=0  (tol 1e-05)     VERDICT: CLEAN
  past expiry 1   detonated 1 (PRE must equal POST)
```

The one apparent mismatch at the first attempt was **not** a decode error: the
projectile had detonated that frame, so the record was frozen and the model had
integrated anyway. `FUN_00453730`'s verdict is an input — the world query behind
it is — and the model now consumes it, exactly as the replay does. Tightening to
`1e-6` leaves 2 fields at `1.91e-6`, i.e. float32 ULPs on a value near 16; the
residual is `math.sin`/`math.sqrt` in doubles against x87 `FSIN` and the RW sqrt
LUT, not a disagreement about the rule.

**Ported and measured**, `m2.msd`:

| site | orig | port | |
|---|---|---|---|
| `0x453789` query | 280 | 280 | clean |
| `0x4537bb` gate  | 2 | 2 | clean |
| `0x4537df` lerp  | 1 | 1 | clean |
| `0x45382c` basis | 1 | 1 | clean |

and, the number that actually carries the weight, the port **seeded once per
projectile life and then carrying its own state**:

```
updates replayed 280 of 327   projectile lives seeded 3
max position drift 4.77e-07   max age drift 0   (limit 1e-4)   clean
```

**Why the drift is in the verdict and not a footnote.** MORTAR's contact COUNTS
are schedule-derived — the port steps once per captured row and the first thing a
step does is the query — so they pass even with the integrator broken. That is
measured, not argued: all three `MASHED_MORTAR_FORCE` controls leave **every**
MORTAR row `clean`.

| control | drift | count rows | verdict |
|---|---|---|---|
| `noarc` — Y integrates by velocity | **6.03** | all clean | DIVERGES |
| `nohome` — homing branch never runs | **6.29** | all clean | DIVERGES |
| `allow` — gate polarity inverted | **0.226**, age 0.0167 | all clean | DIVERGES |
| *(unset, the real run)* | **4.77e-07** | all clean | CLEAN |

So `pu_replay` now folds the drift into `CONTACT VERDICT`. Without that the table
would have reported a broken integrator as clean — and note in particular that
the gate's **polarity** (non-zero REFUSES, from `CALL 0x45c350` `0x004537b6`
falling through to `return 0`) is pinned by the drift alone: inverting it leaves
the lerp at 1/1, because the two recorded gate returns are one 0 and one 1.

One correction it forced in `PowerupContact`: MORTAR's gate is `0x0045c350`, the
sweep's *confirm* leaf, **not** the `0x0045c110` surface gate the OIL/P_MINE
drops use — and it has to be keyed by **call site**, not by slot as the
dispatcher's is, because a mortar in flight outlives the slot that fired it.
Hence `Contact::ConfirmGateAt`.

Not ported, and none of it makes a contact call: the explosion effects
(`FUN_00477760`, `FUN_00486610`, `FUN_00453210`) and the trail ribbon
(`FUN_004532f0` / `FUN_00453100`, 15 segments).

### 4.2g MISSILE — mapped, not ported, and the map corrects the site list

MISSILE is the one type still owing its own chain. It was NOT started, so what
follows is a map, measured and disassembled, not a claim about a port.

**Its acquisition is already covered** by §4.2e: `FUN_00455b50` (`0x00455b50`)
calls `FUN_00459620(car, matrix+0x30, 8.0, 30.0, ptr)` at `0x00455c29` —
`PUSH 0x41000000` = **8.0 range**, `PUSH 0x41f00000` = **30.0 cone**, different
from MORTAR's 15.0/20.0 and read straight off the call site. The `--puhook-aim`
channel records whatever the call site passes, so both are covered by one port.
Measured on `m1`'s MISSILE window: `0x459c19` 32/32 calls, `0x459d54` 32/32.

**Two sites the earlier notes never listed**, both from `m1` and `m2`:

| RA | leaf | `m1` | `m2` |
|---|---|---|---|
| `0x455cd9` | `0x00455100` impact | 31 / 30 hits | 12 / 12 |
| `0x455e59` | `0x004b4cd0` | 31 / 31 | 12 / 12 |
| `0x455de0` | `0x004b4d10` | 15 / 2 | 6 / 0 |
| **`0x455df9`** | **`0x0045c350` gate** | **2 / 2** | — |
| `0x455e07` | `0x00455910` terminal | 1 / 1 (in `g2`) | — |

**The chain's shape, disassembled** (`0x00455de3`..`0x00455e07`):

```
0x00455de3  TEST EAX,EAX          ; the 0x4b4d10 query's result, RA 0x455de0
0x00455de5  JE   0x455e07         ; miss -> skip the rest
0x00455df4  CALL 0x45c350         ; RA 0x455df9
0x00455dfc  TEST EAX,EAX
0x00455dfe  JNE  0x455e07         ; NON-ZERO REFUSES
0x00455e02  CALL 0x455910         ; RA 0x455e07, the terminal
```

That is the **same gate polarity** as MORTAR's (§4.2f) — non-zero refuses — which
independently corroborates the polarity the mortar drift control pinned.

**What porting it needs, and the one real blocker.** `0x00455c90` is the MISSILE
tick and it walks **two interleaved pools** in one loop: the aim records
(`0x006885d0`, stride `0x2c`, 4 entries — `MOV EDI,0x6886ac` `0x00455c9a`,
`SUB EDI,0x2c` `0x00455ca9`) and the projectiles (stride `0x6c` —
`MOV EBP,0x688620` `0x00455c9f`, `SUB EBP,0x6c` `0x00455caf`). It is **not a
defined Ghidra function**, so there is no decompilation for it, only disassembly.
It also needs one leaf `PowerupContact` does not have: `0x004b4d10`.

Next commands, in order:
```
py -3.12 re/tools/disasm_va.py 0x455c90 0x400      # find the EBP loop's bound
py -3.12 re/tools/decomp_pc.py 0x004b4d10 0x00455910 0x00455100 --slot 0
```
Do **not** reach for a Ghidra master write to define `0x00455c90`: MORTAR's pool
was found off its tick's own loop bounds without one (§4.2f), and the same works
here.

### 4.2h MISSILE — ported, and the last type

The tick is `FUN_00455c90`. **Ghidra's auto-analysis never defined it**, which is
why every earlier note stopped at a call-site count. It was read with a `--create`
mode added to `re/tools/decomp_pc.py` this session: a **transient** function
definition against a `-readOnly` pool clone, discarded on exit. That is not a
master write, and none was needed.

**Two pools, walked in lockstep in one loop**, both bounds disassembled:

| pool | base | stride | count | from |
|---|---|---|---|---|
| aim records | `0x006885d0` | `0x2c` | **5** | `MOV EDI,0x6886ac` `0x00455c9a`, `SUB EDI,0x2c` `0x00455ca9` |
| projectiles | `0x006883b0` | `0x6c` | **5** | `MOV EBP,0x688620` `0x00455c9f`, `SUB EBP,0x6c` `0x00455caf`, loop exit `0x00688404` |

Two corrections this forces. §4.2g said the aim pool had **4** entries — it has
**5** (4 intervals, 5 records). And the older `DAT_006883bc stride 0x6c` named the
projectile record's **position field** (base + `0x0c`), not its base — the same
off-by-a-field as the acquisition record's `&DAT_0068b9fc` gloss. **The loop walks
DOWNWARD**, so record index 4 is stepped first, which is load-bearing for any
per-frame zip against contact rows.

**What the tick does per live record**, and the one thing that explains every
earlier count: the sphere query is gated on the **parity of `DAT_007f101c`**
(`AND ECX,0x80000001` + the signed fixup, `0x00455d83`..`0x00455d99`), while the
ground probe runs every frame. That is why `0x455de0` sits at about half of
`0x455e59` in every capture — `m1` 15 vs 31, `m2` 6 vs 12, `s1` 78 vs 155.

**Falsified offline first** (`re/tools/missile_model.py`, on `s1`, 195 live
projectile-frames over 195 frames):

```
even frames (parity gate) 98 of 195   0x455de0 rows 98   0x455e59 rows 195
A sphere-query count  frames match=195 mismatch=0
B ground-query count  frames match=195 mismatch=0
C bias sentinel       records match=195 mismatch=0
VERDICT: CLEAN
```

**Ported and measured** (`Powerup/PowerupMissile.{h,cpp}`):

| site | `s1` | `s2` | |
|---|---|---|---|
| `0x455de0` sphere `0x004b4d10` | 78/78 | 44/44 | clean |
| `0x455df9` gate `0x0045c350` | 1/1 | 1/1 | clean |
| `0x455e59` ground `0x004b4cd0` | 155/155 | 87/87 | clean |
| ground-bias value | *(no `hit_t`)* | **0 mismatches** | clean |

**One defect the measurement caught**, worth recording because reasoning would
not have: after the detonation call `CALL 0x455910` `0x00455e02`, execution
**falls through** to `0x00455e07`, the ground probe. Only the AGE path skips it,
via `JMP 0x455f2a` `0x00455d37`. The port returned early instead, and that cost
exactly **one** ground query on `s1` (155 vs 154) — which is how it was found.

**A missing replay input, closed.** `pu_replay`'s `QueryInject` hardcoded
`out->t = 0.5f` with a comment saying nothing downstream of it was measured. That
stopped being true: MISSILE's `+0x28` ground bias is a pure function of `t`
(`(t*3 - 0.4)*2.5*-0.5`, floored at `-0.05`). The contact capture now records the
hit's segment parameter as a `hit_t` column (the result buffer is arg3, `t` at
`+0x38`), the replay feeds it back, and the bias goes from 107 mismatches to
**0**. A capture without the column leaves the bias explicitly untested rather
than failing it.

**Non-degeneracy, run and reported:**

| control | effect |
|---|---|
| `noparity` — sphere query every frame | `0x455de0` 155 vs 78 (`s1`), 87 vs 44 (`s2`) — **DIVERGES** |
| `flatbias` — bias always the miss sentinel | 87 bias mismatches on `s2` — **DIVERGES** |
| `nolife` — age gate never expires | **INERT on both captures** |
| *(unset)* | every row clean |

The `nolife` result is reported as what it is: **no projectile aged out in either
capture**, so the 3.0 s lifetime gate has **no control coverage here**. Its two
terminals both came from the sphere path. A capture with a missile that times out
is what would close it.

**Port boundary, wider than MORTAR's.** The FLIGHT INTEGRATION is *not* ported:
`FUN_00455610` (unguided) and `FUN_004556f0` (homing) both read **and write** the
projectile's RenderWare frame matrix through `FUN_004c1520` / `FUN_004c1340` /
`FUN_004c15c0`, a closed loop the replay cannot reproduce without those RwMatrix
ops. Position and delta are INPUTS. So the port decides what a missile *does*, not
where it *is*. `FUN_00455100` (RA `0x455cd9`) belongs to the aim half and is not
ported either.

**`FUN_004b4d10` decoded**: it copies **four** dwords (centre + radius), writes
tag **3**, and tails the **same** `FUN_004b4c80` as `0x004b4cd0` — so it is that
query's sphere sibling and returns the same intersection count. Its decompilation
is typed `void`; the tick tests its `EAX`, and the tail call is what carries the
count.

### 4.3 The prior note's own captures

`c2` and `c3` predate the `surface_gate` instrument, and `c2`/`c3`/`g2`/`g3`
predate `query_4b4b20`, so those rows have zero original data. `pu_replay` marks
them **`not-armed`** (the site's RVA appears nowhere in the capture, so no Frida
listener was attached) and excludes them from the verdict, and it propagates the
mark to any site the not-armed one **gates** — with no injected verdict the port
takes the refuse arm and never reaches the gated site, which would otherwise read
as a divergence. With that rule all five captures report
`CONTACT VERDICT: CLEAN`. Every armed row is exact:

- `c2`: P_MINE query **7 / 7**, lerp **1 / 1**, basis **1 / 1**, sweep **280 / 280**,
  sweep-confirm **1 / 1**. Seven press edges, one drop — the port now reproduces
  the 6-in-7 refusal the prior note measured.
- `c3`: OIL query **10 / 10**, lerp **10 / 10**, basis **10 / 10**, sweep
  **309 / 309**.
- `g3`: OIL **10 / 10 / 10 / 10** including its gate, P_MINE all zero (no P_MINE
  in that recipe), sweep **309 / 309**.

### 4.4 Decision half, same runs

`re/tools/pu_diff.py`, slot 0:

| capture | types | verdict |
|---|---|---|
| `c2` | MISSILE, MORTAR, DRUM, **P_MINE** | **CLEAN** (was 78 mismatches, then 4, now 0) |
| `c3` | OIL, R_FLAME, FLASH, GUN, SHOTGUN | CLEAN |
| `g2` | MISSILE, MORTAR, DRUM, P_MINE | CLEAN |
| `g3` | OIL, R_FLAME, FLASH, GUN, SHOTGUN | CLEAN |
| `g4` | OIL, R_FLAME, FLASH, GUN, SHOTGUN | CLEAN |

The four mismatches that survived the gate port were all at `c2` `t+82`
(`code_post`, `fire_modes`, `canfire_rets`, `deact_ra orig='0x45bcfc'`) — the one
frame the armed sweep fired. §3.3 closed them.

### 4.4b A NEW decision-half defect, found by a new capture and NOT caused here

> **RESOLVED 2026-09-28d — `re/analysis/D3_BOX_STATE_2026-09-28.md`.** The cause is
> NOT below. The "next command" at the end of this section named the aim record
> `0x006885d0 + slot*0x2c`, and that hypothesis was **REFUTED**: `FUN_00455150` does
> consult it at `0x00455163` and the port already matched it verbatim — the original
> never reached the FIRE call. The real gate is the dispatcher's per-slot BOX STATE
> `DAT_0068d1f0[slot]`, read at `0x0045bc6b` BEFORE the armed test at `0x0045bcab`,
> whose values 4 / 2 / 3 short-circuit the whole per-slot pass. `s2` now replays
> **CLEAN** on both halves (sweep `0x45bcd8` 152 / 152), so this section's stated
> consequence for the contact table is also gone. Commits `2e7a2b92` + `be06d381`.

`s2` (plan `11,7,11,11` — MISSILE, MORTAR, MISSILE, MISSILE) is the first capture
to put a MISSILE pickup *after* a MORTAR one. Its **9-type decision** replay
reports **181 mismatches**, all on the THIRD activation; the first MISSILE and the
MORTAR before it are both CLEAN.

The shape: from `t+1` on, the port FIRES and the original does not —
`fire_modes orig='' port='2'`, `ammo orig=1 port=0`, `jet orig=0 port=1`,
`life orig=0.0 port=0.483`.

**It is not a regression from this session's work, and that is measured, not
argued.** `re/tools/pu_replay/build_control.bat 5bb0d5e3` links the *pre-R_FLAME*
`PowerupEffects.cpp` against everything else current, and it reports the **same
181 mismatches**. The defect is older than every commit in this note. (The control
build was extended this session to link the four new TUs, which is why it can run
at all.)

**Consequence for the contact table, stated so it is not mistaken for a
criterion-(c) failure.** The port holds the third MISSILE armed longer than the
original, so the dispatcher's armed sweep fires **33 extra times** —
`SWEEP query 0x45bcd8` 152 vs 185. That single row is the *whole* of `s2`'s
`CONTACT VERDICT: DIVERGES`. Every MISSILE, MORTAR and AIM row on `s2` is clean,
and `s1` (plan `11,11,11,11`, no MORTAR) is CLEAN throughout including its sweep.

Next command: replay `s2` with `--slot 0` and dump the third activation's
`FUN_00455150` (MISSILE FIRE) gate inputs; the likely coupling is the aim record
at `0x006885d0 + slot*0x2c`, whose `+0x1c` the tick tests before
`FUN_00455100` — the port's FIRE consults no aim record at all.

## 5. Corrections to `D3_CONTACT_2026-09-27.md` §5

1. **DRUM is gated too.** `g2` records `query_4b4cd0 0x4b4cd0 from 0x45444f
   calls=9 ret!=0=0` inside slot 0's DRUM window and **no** placement leaf, while
   the P_MINE window (another slot holding DRUM) records `0x45444f 11/1` with one
   `0x45448a` lerp and one `0x4544e0` basis. So DRUM's drop runs the same
   query-then-place shape; the prior table listed only the two leaves. It is **not**
   in `FUN_004541e0` (decompiled: no contact call at all), so the site lives in the
   drum's per-frame update past `0x00454311` — `FUN_00454350`, decoded in §5.3.
2. **Window attribution over-collects.** `pu_contact_report.py` slot-attributes the
   dispatcher sweep but not the per-type sites, so a type's window also shows other
   slots' and other systems' calls. `g2`'s MORTAR window lists GUN sites
   (`0x459c19`, `0x459d54`, `0x459db5`), a P_MINE triple and a `0x479124` row that
   is a per-frame ground probe outside the power-up system entirely (6668 calls,
   ~1 per frame). OIL's and P_MINE's rows are trustworthy only because those two
   sites sit inside functions reachable from one type.

### 5.2 `FUN_00459620` is SHARED — the prior note's GUN row is a window artifact

> **SUPERSEDED IN PART by §4.2e.** This section's call-site scan stands. Its
> characterisation of `FUN_00459620` as a *projectile* routine does not: it
> acquires a **target**, and integrates no projectile position anywhere. The
> routine is now ported (`Powerup/PowerupAim.{h,cpp}`). Its GUN call counts below
> are also wrong — see §4.2e for the measured ones.

A scan of the whole `.text` for `E8` rel32 calls (every decoded operand confirmed,
not a byte-pattern match) gives the exact caller sets:

| routine | callers |
|---|---|
| `FUN_00459620` | `0x00453bd9` (MORTAR tick `FUN_00453b80`), `0x00455c29` (MISSILE), `0x004569c5` (GUN tick) |
| `FUN_00453730` | `0x004538fe` only — inside `FUN_004538b0`, which the MORTAR tick calls at `0x00453c39` |
| `FUN_00454350` | `0x00454881` only — the DRUM tick `FUN_00454820` |
| `FUN_0045b390` | `0x0045b6f9` only — SHOTGUN FIRE `FUN_0045b6e0` |

So the four sites the prior note listed under GUN (`0x459c19`, `0x459d54`,
`0x459db5`, plus `0x459c3c` which it did not list) are inside a routine **three**
types share; which type they belong to depends on what is in flight, and an
activation window cannot tell. Consequence for the next slice: porting
`FUN_00459620` once is what unlocks MORTAR, MISSILE and GUN — and until it is
ported, none of those three can be attributed at all.

It also explains why DRUM and SHOTGUN were the two that could be closed today:
they are the only two of the six whose contact routine has a single caller.

### 5.3 The remaining four, decoded — each is a PROJECTILE slice

Decompiled this session so the next one starts from a map, not a call-site count.
All five put their contact call inside a **per-frame projectile/particle update**
that integrates a position and a velocity the ported effect module does not own.

| type | containing fn | shape |
|---|---|---|
| MORTAR | `FUN_00453730` (0x00453730..0x004538a7, 377 B) | `A = rec+0x14..0x1c`, `B = A + rec+0x38..0x40`; `0x004b4cd0` @`0x00453784`→RA `0x453789`; on a hit `FUN_0045c350(&res, rec+0x14)` RA `0x4537bb`; on `== 0` lerp RA `0x4537df`, `p += normal*_DAT_005cc9a0(0.05)`, basis RA `0x45382c`, explosion, `FUN_00453210`, `return 1` (detonated) |
| DRUM | **PORTED** — `FUN_00454350` (994 B) | state machine on `param_1[0xb]`: 1 = stuck to the car, **2 = flying**. Pos `param_1[4..6]`, vel `param_1[7..9]`, life `param_1[0xf]`, gated on `life <= _DAT_005cc358`. Each frame `B = pos + vel*dt`, `0x004b4cd0` RA `0x45444f`; **miss** → integrate and apply gravity `_DAT_005ce42c/430/434`; **hit** → the landing branch with lerp RA `0x45448a` and basis RA `0x4544e0` |
| R_FLAME | `FUN_0045ae80` — the ported TICK's original | 5 owners × 5 groups × 5 sparks. A spark with `pfVar8[6] != 0`, `*pfVar8 < 1.0` and `pfVar8[5] == 0` probes `A = pfVar8[-6..-4]` → `B = A + pfVar8[-3..-1]`: `0x004b4cd0` RA `0x45afcc`; **miss** → `vel.y -= _DAT_005ce018(0.002)`; **hit** → lerp RA `0x45aff3`, `p += normal*_DAT_005ce18c(0.02)`, zero the velocity, set `pfVar8[5] = 1` (landed), basis RA `0x45b04c`. Age `pfVar8[2] += dt`, `*pfVar8 = age*_DAT_005cd114(1.1111)` clamped to 1.0 |
| GUN | `FUN_00459620` (2727 B) | **PORTED, §4.2e.** Corrected counts: `0x459c19` is **163 calls / 84 hits** on `g3` (not "96 / 0"), `0x459d54` 163/98, lerps `0x459c3c` 84/84 and `0x459db5` 98/98. Not a projectile update at all — target acquisition |
| MISSILE | not a Ghidra function | `0x00455cd9`, `0x00455de0`, `0x00455e07`, `0x00455e59` are inside the MISSILE TICK region past `0x00455c90`, which Ghidra has not defined. Create the function first, then decode |

#### R_FLAME, decoded and PORTED (§4.2d) — kept here as the record

Worth writing down because it is most of the slice:

- **The emitter emits exactly ONE spark per stepper tick**, at a flat index the
  decompilation gives in closed form. `FUN_0045a950` `0x0045a9fa`:
  `iVar2 = sub + (owner + major + owner*4) * 5` = **`owner*25 + major*5 + sub`**,
  with `sub` read BEFORE its increment (`iVar4 = sub + 1` follows). `owner` is
  `car+0xb0`, `major` is `rec+0x14`, `sub` is `rec+0x18` — the two counters
  `RFlame_Tick` already tracks as `ammo` and `subState`, and whose transitions
  `pu_diff` already reports CLEAN.
- **What the emitter writes**: `alive = 1` (`&DAT_0068bd30[idx*0xd]`), a random
  angle in [-π, π] (`&DAT_0068bd28`), pos = the car frame's `+0x30..0x38` plus
  `carVel * DAT_005d757c` — and `DAT_005d757c` reads **0.0**, so the term drops —
  and vel = the car matrix `at` row `+0x20..0x28` × `_DAT_005ce4f0 (-0.02)` with a
  `FUN_00472650(-0.002, 0.002) * _DAT_005cc9f4 (8.0)` jitter added to x and z.
  It also re-arms `rec+0x8 = 0x3ca3d70a (0.02)` and, on `sub > 4`, does
  `major++ / jet = 0 / sub = 0` — all three already in the port.
- **Spark record**: stride `0xd` dwords (0x34 B). `[0..2]` pos, `[3..5]` vel,
  `[6]` normalised age, `[8]` age seconds, `[11]` landed latch, `[12]` alive.
  Both readers agree (`FUN_0045ae80`'s `pfVar8` is base+6; `FUN_0045ac40` tests
  `pfVar10[0xc]` and `pfVar10[6]`).
- **Per frame, while `alive && ageN < 1.0`**: if `landed == 0`, probe
  `A = pos → B = pos + vel` (no `dt` — the velocity is per-frame);
  miss → `vel.y -= _DAT_005ce018 (0.002)`; hit → lerp, `p += normal *
  _DAT_005ce18c (0.02)`, `vel = 0`, `landed = 1`, basis. Then unconditionally
  `age += dt`, `ageN = age * _DAT_005cd114 (1.1111)` clamped to 1.0, and
  `pos += vel`. So a spark stops probing after **0.9 s** even without a hit.
- **Pool geometry, RESOLVED** (the earlier ambiguity between the reset's range and
  the emitter's base was a decompiler-pointer-arithmetic artifact). Read off
  `FUN_0045a3a0`'s own instructions: outer `SUB EDI,0x514` from `0x0068d76c` down
  to `CMP EDI,0x68c31c` = 4 owners (`0x514 = 25*0x34`); mid `SUB ESI,0x104` ×5 = 5
  groups; inner `SUB EAX,0x34` ×5, clearing `[eax-4 .. eax+0x2c]`, so the record
  base is `EAX-4`. Lowest base reached is `0x0068bd00`, highest `0x0068d11c` —
  exactly **100 records** on that lattice, and the ten cleared dwords are indices
  0-6, 8, 11, 12, i.e. pos, vel, ageN, age, landed, alive. ARM clears the WHOLE
  pool, every owner's.
- **What is NOT ported, and it is the `g3` residue's cause**: `FUN_0045ac40`
  (`0x0045ac40`), the per-group distance sort the tick runs immediately before the
  inner 5-spark loop. See §4.2d.

So what each still needs is its **pool record + per-frame integration** ported into
the Powerup TUs (the `DAT_006883xx` pools the `PowerupSystem.h` ledger already
records as unmapped). Note the consequence for measurement: unlike OIL/P_MINE/
SHOTGUN, whose query count is fixed by the decision logic, these types' query
counts are a function of flight time. Injecting the original's verdicts is not
enough — the projectile has to be born and die on the same dispatcher calls, so
the flight integration has to be faithful before the count can be compared.

## 6. No regression

- Archived 9-type guard, re-run after every change in this note:
  `verify/d3_contact_20260928/o3.diff.txt` (OIL, FLASH, GUN, SHOTGUN, MISSILE)
  **CLEAN**, `o4.diff.txt` (MORTAR, DRUM, P_MINE, R_FLAME) **CLEAN**.
- The new `verify/d3_contact_20260928b/m1.diff.txt` and `m2.diff.txt` (MORTAR,
  GUN, MISSILE) **CLEAN** — a decision-side guard those three did not previously
  have.
- Contact replay, re-run after every change, on all NINE captures:
  `c2`, `c3`, `g2`, `g4`, `m1`, `m2`, `s1` CLEAN; `g3` DIVERGES on exactly the
  2-query R_FLAME residue of §4.2d; `s2` DIVERGES on exactly the dispatcher SWEEP
  row, downstream of the older decision defect of §4.4b, with every MISSILE,
  MORTAR and AIM row on it clean.
- 9-type decision replay: `o3`, `o4`, `m1`, `m2`, `s1` CLEAN; `s2` 181 mismatches
  which the **control build** (`build_control.bat 5bb0d5e3`, the pre-R_FLAME
  effects) reproduces exactly — so it is not this session's.
- All four current captures CLEAN (§4.4).
- `mashedmod\build.bat` built both targets clean.

**How strong this guard is, stated plainly.** `pu_replay` links
`Powerup/PowerupSystem.cpp`, `PowerupEffects.cpp` and now `PowerupContact.cpp` —
the same TUs the exe links — so unlike the prior note's run it *does* cover the new
code. What it does **not** cover is `TrackRenderer`'s `TriSource` wiring and the
live `SegmentQuery` walk, which no capture on either side exercises comparably.
Those are covered only by a clean build and by the fact that with `MASHED_PU_CONTACTDUMP`
unset and no tri source the new code logs nothing and returns 0/allow.

`o1`/`o2` were also replayed and show ~300 and ~68 float mismatches on accumulating
timers (`cooldown`, `life`, drift ~3e-6). That is a **capture-format** artifact, not
a regression: `o1`/`o2` record `dt` as the 6-digit decimal `0.016667`, while `o3`,
`o4` and every 2026-09-27+ capture record the exact float bits (`0x3c888888`). They
are not part of the guard and were not in the prior note's either.

## 7. Verdict against ROADMAP §D3 powerups criterion (c)

| type | (c) verdict | counts (orig / port, per call site) |
|---|---|---|
| OIL | **clean** | query/gate/lerp/basis **10/10/10/10** on `g3` and `g4`; `c3` 10/–/10/10 |
| P_MINE | **clean** | **2/2/2/2** on `g2`; `c2` 7/–/1/1 (7 press edges, 1 drop) |
| SHOTGUN | **clean** | query **8/8**, basis **7/7** on `g4` |
| FLASH | **clean** | no contact call exists (unchanged from the prior note) |
| — armed sweep | **clean** | 280 / 309 / 207 / 309 queries across `c2`/`c3`/`g2`/`g3`+`g4`; the single `c2` deactivation reproduced |
| DRUM | **clean** | query **83/83** on both `g2` and `c2`; lerp/basis 1/1 and 2/2 |
| R_FLAME | **clean on `c3` and `g4`, 2-query residue on `g3`** | query 510/510 (`c3`), 547/547 (`g4`), 546 vs 548 (`g3`); lerp and basis exact on all three. Residue mechanism cited in §4.2d: the unported per-group distance sort `FUN_0045ac40` |
| — acquisition (shared) | **clean** | `FUN_00459620`'s four sites on `m1`: fallback query **187/187**, fallback lerp 33/33, LOS lerp 152/152, LOS query 348/348 (schedule-derived). Both non-degeneracy controls DIVERGE. §4.2e |
| GUN | **clean** | no site beyond the acquisition four ever appears in a GUN window, and window attribution can only OVER-collect, so that negative is sound. The three others that do appear are not power-up sites: `0x479124` fires 6666 times in `g3` (~1/frame over the whole 6650-frame race, held or not), and `0x475229`/`0x4752b2` fire 1× in `g3` but 4× in `g4` and 4× in `m1`, i.e. outside GUN windows too |
| MORTAR | **clean** | the acquisition four plus its OWN chain, all ported (§4.2f): `0x453789` 280/280, gate `0x4537bb` 2/2, lerp `0x4537df` 1/1, basis `0x45382c` 1/1, and the carried integration within **4.77e-07** over 280 steps and 3 projectile lives. All three non-degeneracy controls DIVERGE |
| MISSILE | **clean** | acquisition 32/32 on `m1` (its range/cone are 8.0/30.0, not MORTAR's 15/20) plus its OWN chain, ported (§4.2h): sphere `0x455de0` 78/78 and 44/44, gate `0x455df9` 1/1 and 1/1, ground `0x455e59` 155/155 and 87/87, and the ground-bias VALUE exact on `s2`. The `noparity` and `flatbias` controls DIVERGE; `nolife` is inert (no projectile aged out) |

**8 clean + the sweep + the shared acquisition, 1 near-clean (R_FLAME)** —
against 1 clean / 1 diverges / 7 blocked at the start of the day. Every type's
contact outcomes are now ported and measured; R_FLAME is the only residue, and it
is bounded at 2 queries of 546 on one of three captures.

What is NOT closed is a separate thing, and it is worth not confusing with this:
the **decision** half diverges on one new capture (`s2`, §4.4b) in a
MORTAR-then-MISSILE sequence. That is criterion (b) territory, it predates this
session (measured against a control build), and it is what makes `s2`'s contact
verdict read DIVERGES — through the dispatcher SWEEP row only, with every
MISSILE, MORTAR and AIM row on that capture clean.

## 8. OPEN

1. ~~**`FUN_00459620`**~~ — **DONE** (§4.2e), and it was an ACQUISITION routine,
   not a projectile one. What remains of it is three named pieces, each with a
   stated reason: the second candidate list (measured empty, `list_n == 0` on all
   348 rows), the third loop over RW atomics (makes no contact call), and the
   locked branch's intercept prediction `FUN_0041f2c0` ([UNCERTAIN] — the capture
   records only 3 of the 4 position dwords and nothing about `FUN_0041f1c0`'s
   gate). Next command:
   `py -3.12 re/tools/decomp_pc.py 0x0041f2c0 0x0041f1c0 0x00558b40 --slot 0`.
2. ~~**R_FLAME**~~ — **DONE** (§4.2d, §5.3). What remains of it is one named
   routine: **`FUN_0045ac40`** (`0x0045ac40`), the per-group distance sort the
   original's tick runs immediately before the inner 5-spark loop. Not ported
   because its reference point comes from `FUN_004671d0(0)`, a viewport query the
   replay has no equivalent for. Bound on what that costs: **2 queries of 546 on
   one of three captures, on a count only**; lerp and basis are exact on all three.
3. ~~**MORTAR's own detonation test**~~ and ~~**MISSILE's own chain**~~ — both
   **DONE** (§4.2f, §4.2h). What is left of either is effects and trail ribbons,
   none of which makes a contact call, plus MISSILE's flight integration
   (`FUN_00455610` / `FUN_004556f0`), which is a closed loop through the
   projectile's RW frame matrix and would need `FUN_004c1520` / `FUN_004c1340`
   ported first.
3b. ~~**THE NEXT SLICE IS NOT criterion (c).**~~ — **DONE 2026-09-28d**, and it WAS
   criterion (c) after all: (c)'s own text gates on ammo / cooldown / fire-mode, which
   is exactly what the 181 mismatches were. Cause and fix in
   `re/analysis/D3_BOX_STATE_2026-09-28.md` — the dispatcher's box-state gate, not the
   MISSILE chain. Original text follows:
   It is the decision-half defect of
   §4.4b: on a MORTAR-then-MISSILE sequence the port fires a MISSILE the original
   refuses, 181 mismatches on `s2`, reproduced by a control build against the
   pre-R_FLAME effects so it is older than this note. Next command in §4.4b.
3c. **MISSILE's 3.0 s lifetime gate has NO control coverage.** The `nolife`
   control is inert on both `s1` and `s2` because no projectile aged out in
   either — both terminals came from the sphere path. A capture with a missile
   that times out would close it.
4. **P_MINE's and DRUM's segment direction** — [UNCERTAIN], §3.1. Both probe world
   `-Y` because `HostCar` has no matrix up row. Next command in §3.1.
5. **The sweep's sphere** `slot+0x80..0x8c`: the radius at `+0x8c` has no writer in
   the ported lifecycle, so the branch cannot fire in the shipping build. `EBX` at
   `0x0045bcb2` is the source of the centre; start there.
6. **`FUN_0045c110`'s material channel.** `col_mat_` is an index, the gate keys on
   the RwRGBA at `RpMaterial+4`. Carrying the colour through the collision soup
   would make the gate real rather than a measured-allow stand-in.
7. **SHOTGUN's per-pellet frames.** Both of the port's two passes probe from the
   owner car; the original probes from `param_1[1]` and `param_1[2]`. It did not
   change the counts on `g4`, but it moves both impact points.
8. **The new capture channels are thin, and all on track 0.** `--puhook-aim` has
   run four times, `--puhook-mortar` twice, `--puhook-missile` twice, and the
   `hit_t` column exists on ONE capture (`s2`). Every earlier capture lacks them,
   and `pu_replay` prints those rows as `not-armed` rather than `clean` — a 0-vs-0
   row is a missing input, not a match. A capture on a different track would
   harden §4.2e–§4.2h.
9. **MORTAR's 3 projectile lives are few.** §4.2f's drift is measured over 280
   updates but only 3 launches, all on one track. The `noarc`/`nohome`/`allow`
   controls make it a real measurement rather than a lucky one, but more lives
   would bound the homing branch better (88 of the 327 updates were homing).
10. Carried from the prior note: the Vehicle points/vectors defect (U-9138, latent,
    untouched here), `Rw_VtableDispatch`, `Rw_SetRotation` / `Math_Acos`.
