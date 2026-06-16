# Power-up effect cluster — function split + verbatim decompilation (WS-D / D2 prerequisite)

**Session:** WS-D powerup-effects D2 (2026-06-16), Ghidra slot `Mashed_pool11`
(pool5 was locked by another session; `acquire` handed pool11).
**Mode:** read-only-clone split — `function_create` at each table-target RVA was done
in the **clone, in-memory only**; `program_save` was **never** called, so neither the
master nor the on-disk clone was mutated (per the session ruling "do the split in the
clone purely to drive decompilation, no master write").
**Anchor:** MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
(verified on `original/MASHED.exe.unpatched`; live `MASHED.exe` carries the 5 boot
patches, expected).

Companion map: `powerup_system.md` (D1 — architecture, type table @0x005f9998, slot
struct @0x0088fbe0). This file adds the **per-function boundaries** (the "split") and
the **verbatim decompilation** of every dispatch-surface function, plus the per-type
dependency profile that gates the verbatim C++ port.

---

## 1. The split — function boundaries carved out of the merged blob

Ghidra had merged `0x00453f60..0x0045be81` into one function (`FUN_0045bba0`, entry
`0x0045bba0` near the END of that body). `function_create` at each table-target RVA
carved clean per-function bodies. Boundaries (body_start..body_end):

| RVA | role | body | RVA | role | body |
|---|---|---|---|---|---|
| 0045bba0 | **dispatcher** | 00453f60..0045be81* | 0045baa0 | lookup | 0045baa0..0045babf |
| 0045bfa0 | activate | 0045bfa0..0045bfc2 | 0045bac0 | deactivate | 0045bac0..0045badb |
| 0045c010 | pickup-wrapper | 0045c010..0045c028 | 0045b350 | shared no-op (RET) | 0045b350 |
| **GUN(9)** | | | **DRUM(10)** | | |
| 00456040 | arm | ..00456136 | 00453fe0 | arm | ..004540b1 |
| 004561c0 | fire | ..004566c7 | 00454740 | fire | ..00454788 |
| 004566d0 | canfire | ..004566eb | 00457ab0 | canfire | ..00457ac4 |
| 004566f0 | deact | ..00456739 | 00457ad0 | deact | ..00457ae8 |
| 004568d0 | tick | ..00456c27 | 00454820 | tick | ..00454895 |
| **MISSILE(11)** | | | **P_MINE(12)** | | |
| 00455060 | arm | ..004550fc | 00457a30 | arm | ..00457aa9 |
| 00455150 | fire | ..00455358 | 00457ef0 | fire | ..00457f22 |
| 00455360 | canfire | ..00455388 | (457ab0) | canfire | shared w/ DRUM |
| 00455390 | deact | ..004553df | (457ad0) | deact | shared w/ DRUM |
| 00455c90 | tick | ..00455f40 | 004582f0 | tick | ..00458357 |
| **R_FLAME(16)** | | | **SHOTGUN(17)** | | |
| 0045a7c0 | arm | ..0045a84b | 0045b200 | arm | ..0045b257 |
| 0045a850 | fire | ..0045a88f | 0045b6e0 | fire | ..0045b6ff |
| 0045a890 | canfire | ..0045a8aa | 0045b260 | canfire | ..0045b280 |
| 0045a8b0 | deact | ..0045a906 | 0045b290 | deact | ..0045b2a8 |
| 0045ae80 | tick | ..0045b1f5 | 0045b700 | tick | ..0045b80f |
| **FLASH(18)** | | | **OIL(19)** | | |
| 00454a40 | arm | ..00454a89 | 00456d80 | arm | ..00456dc4 |
| 00454db0 | fire | ..00454dfc | 00457800 | fire | ..00457a20 |
| 00454a90 | canfire | ..00454aa5 | 00456dd0 | canfire | ..00456df8 |
| 00454ab0 | deact | ..00454ac8 | 00456e00 | deact | ..00456e4a |
| 00454e00 | tick | ..00454e3e | 004577b0 | tick | ..004577e1 |
| **MORTAR(7)** | | | | | |
| 00453350 | arm | ..004533a9 | 004533b0 | fire | ..00453604 |
| 00453610 | canfire | ..0045362a | 00453630 | deact | ..0045366c |
| 00453b80 | tick | ..00453c57 | | | |

\* dispatcher body min/max spans the whole region because the entry sits at the tail;
its reachable code (from 0x0045bba0) is the per-frame logic in §3 — the indirect
table-called effect fns are NOT inlined into it.

---

## 2. Per-type dependency profile (what gates the verbatim port)

Every effect references a **per-type instance/projectile pool** + the **RW scene
graph** + **tuning constants** + **FX/particle** + (in TICK) **contacts**. None of
these backing subsystems are landed standalone (WS-B contacts, WS-E RW scene-graph are
still scaffold per SESSION_VERIFICATION_AUDIT_2026-06-16; the pools `0x006883xx..` and
the `DAT_005c*/005d*` tuning band are unmapped/unharvested). Pool bases + strides:

| type | instance pool base | stride | projectile sub-pool | stride |
|---|---|---|---|---|
| GUN | DAT_006886b0 | 0x48 | (shares 006886xx) | |
| DRUM | DAT_00688240 | 0x2c | DAT_00688020 | 0x44 |
| MISSILE | DAT_006885d0 | 0x2c | DAT_006883bc | 0x6c |
| P_MINE | DAT_0068b0d0 | 0x18 | DAT_0068a310 | 0x6c |
| R_FLAME | DAT_0068bb60 | 0x68 | (spark sub-array @+0x18) | 0xd*4 |
| SHOTGUN | DAT_0068d158 | 0x24 | | |
| FLASH | DAT_00688358 | 0x14 | DAT_006882f8 | 0x18 |
| OIL | DAT_0068a250 | 0x10 | | |
| MORTAR | DAT_00684e38 | 0x1c | DAT_00684ea8 | 0x110 |

Common leaf calls (the stub boundary for the C++ port):
- **RW scene-graph (WS-E):** `RwFrameAddChild` / `RwFrameRemoveChild`,
  `thunk_FUN_00426060` (model→frame), `FUN_004e4800` (get parent frame),
  `FUN_00474d60` / `FUN_00474d80` (atomic show/hide), `FUN_004c0ed0` (world matrix
  pos read), `FUN_004c1340/004c1480/004c1520/004c39b0/004c3d90` (RW frame
  transform/rotate/normalize).
- **Contacts (WS-B):** `FUN_0045bfe0`→`FUN_004b4b60/004b4cd0/004b4d10`→`FUN_0045c350`
  (sweep + resolve), `FUN_004b4650` (reflect), `FUN_004b40f0` (line query).
- **FX / particles:** `FUN_00465ca0(kind,pos)`, `FUN_00465e80(kind,pos,owner)`,
  `FUN_004661a0` (stop fx), `FUN_0048aa20`, `FUN_0048f780`, `FUN_004893d0..004894a0`.
- **Tuning constants:** `_DAT_005cc***`, `_DAT_005cd***`, `_DAT_005ce***`,
  `DAT_005d757c` (== 0.0 float sentinel), `_DAT_007f100c` (per-frame dt),
  `_DAT_007f101c` (a per-frame flag).
- **Owner vehicle (WS-A1, landed):** `param+0xb0` = owner controller/player index;
  `param+0x20..0x28` = forward/orientation vec; `param+0x90..0x98` = velocity vec;
  `param+0x30..0x38` = world position (read via FUN_004c0ed0 of the frame at +4).

**Conclusion (carried into the port):** the dispatch skeleton (dispatcher + lookup +
activate + deactivate) and each effect's *decision logic* (ammo/cooldown/fire-mode
gating, state transitions, target-acquisition branch) port verbatim; the *leaf*
operations above are routed through `IPowerupBackend` stubs, each citing the original
RVA it stands in for. This is C2-grade (faithful structure, un-diffable leaves), NOT
C4 — no overclaim.

---

## 3. Dispatcher `FUN_0045bba0` (verbatim) — per-frame, 3 passes

Gated `FUN_0040e350()==6`. (a) Per-slot loop over 4 slots (working ptr from
`&DAT_0088fc70`=slot0+0x90, stride 0x2d floats=0xb4, while `<0x88ff40`): refresh slot
transform (`FUN_0041f060` ×2 + delta write), run per-slot state machine
`DAT_0068d1f0[slot]` (states 2/3/4), and when armed (`slot+0xac != 0`): sweep/contact
(`FUN_0045bfe0`→`FUN_004b4b60`→`FUN_0045c350`; on fail deactivate + `FUN_00476880`
@consts 1.5f/360.0f); **CANFIRE** `(*(entry+0x0c))()` nonzero⇒deactivate; read fire
bytes `DAT_007f103c[c*0x4c+3]` + `DAT_007f14ff[c*0x4c]` → `fire_mode ∈{1,2,3}`;
**FIRE** `(*(entry+0x08))(slot, fire_mode)`; else pickup branch
`DAT_007f1055[c*0x4c]` set & `DAT_007f1515==0` → `FUN_0045c010`. (b) Per-type **TICK**
`(*(entry+0x1c))()` for all 9. (c) Per-type mode: `FUN_00426c00()==2`→`(*(entry+0x3c))()`,
`==0x21`→`(*(entry+0x38))()`. Tail: `FUN_0045a190`, `FUN_00459000`, deferred queue
`DAT_006870b8` (stride 9 ints, until 0x687ec8) → `FUN_00484cf0(args,0x19)`.

## 4. Lifecycle (verbatim)

```c
int* FUN_0045baa0(void){                     // lookup; type id in ESI
  for(i=0;i<DAT_005f9bd8;i++) if(ESI==tbl[i].code) return &tbl[i]; return 0; }
void FUN_0045bfa0(int slot){                  // activate; type id in ESI
  e=FUN_0045baa0(); *(slot+0xa8)=e; *(slot+0xac)=(*(e+4))(slot); }   // (e+4)=ARM
void FUN_0045bac0(void){                       // deactivate; slot in ESI
  (*( *(ESI+0xa8) +0x10))(); *(ESI+0xa8)=0; *(ESI+0xac)=0; }          // (e+0x10)=DEACT
void FUN_0045c010(void){ FUN_0045bfa0(); }     // pickup pass-through
```

## 5. Per-type effect bodies — key operations (verbatim logic, leaves cited)

Decompilations captured this session (small fns verbatim, big fns by operation). The
recurring shape: ARM allocates a free pool slot + attaches an RW model; CANFIRE reads
a counter/state; DEACT detaches the model + clears the slot; FIRE acts only on a
specific `fire_mode`; TICK iterates the pool advancing each instance.

- **GUN(9)** arm `00456040`: pool DAT_006886b0[owner]·0x48, RwFrameAddChild, init
  ammo `+0x10=0x32(50)`. fire `004561c0`: charge `+0x18 += _DAT_005cc35c` to
  `_DAT_005cd274`; if at-rate & ammo>0 decrement, fire ray via `FUN_004c0ed0` target,
  spawn tracer `FUN_0048aa20`, FX `FUN_00465ca0(0x13,pos)`; auto-target branch via
  `FUN_0045a0d0/004058e0/0047ce20/00481d90`. canfire `004566d0`: `ammo<1`.
  deact `004566f0`: RwFrameRemoveChild + clear. tick `004568d0`: 4 slots, spin a
  3-axis lock indicator via `FUN_004c1520`.
- **MISSILE(11)** arm `00455060`: pool DAT_006885d0·0x2c, ammo=1, attach. fire
  `00455150` (only `fire_mode==2`): ammo-- ; spawn into projectile pool
  DAT_006883bc·0x6c; pos from `FUN_004c0ed0`+0x30/34/38; aim from owner angles
  `FUN_004726f0`/`FUN_004a3384` × `_DAT_005ce480`,`_DAT_005ccad0`,`_DAT_005cd988`,
  `_DAT_005cc33c`; attach model; FX `FUN_00465e80(0x12/0x14)`. canfire `00455360`:
  `inflight==0 && ammo<1`. deact `00455390`. tick `00455c90`: iterate pool; homing
  (`FUN_004556f0`) vs straight (`FUN_00455610`); contact `FUN_0045bfe0`→`FUN_004b4d10`
  →`FUN_0045c350`; impact `FUN_00455910/00455100`; trail `FUN_0048f780`; FX
  `FUN_0045dce0(0x14)`.
- **DRUM(10)** arm `00453fe0`: pool DAT_00688240·0x2c + 2 child models. fire
  `00454740` (mode==2): drop `FUN_004541e0` + FX `FUN_00465ca0(0x12,pos)`. canfire
  `00457ab0`: `*(armed+8)==0`. deact `00457ad0`: `*(armed+0x10)=2; *(armed+0xc)=0`.
  tick `00454820`: pool state machine (1=track via `FUN_004c1480`, 2=`FUN_004547c0`),
  decrement timers; sub-pool DAT_00688020·0x44 → `FUN_00454350`.
- **P_MINE(12)** arm `00457a30`: pool DAT_0068b0d0·0x18 + sub. fire `00457ef0`
  (mode==2): `FUN_00457c10` + FX. canfire/deact shared w/ DRUM. tick `004582f0`:
  pool sm (1 track / 2 `FUN_00458020`); sub-pool DAT_0068a310·0x6c → `FUN_00458080`;
  `FUN_00476df0`.
- **R_FLAME(16)** arm `0045a7c0`: pool DAT_0068bb60·0x68, `+0x08=0x3ca3d70a`, FX
  `FUN_00465e80(0x16)`. fire `0045a850`: if `+0x14 > 4` reset off (`+0x1c=0,+0x00=0,
  +0x14=5`); else if mode!=1 set `+0x1c=1` (jet on). canfire `0045a890`: `pool==0`.
  deact `0045a8b0`: detach + `FUN_004661a0(0x16)`. tick `0045ae80`: per-owner spark
  array (5×5×0xd floats) — contact `FUN_0045bfe0`→`FUN_004b4cd0`, reflect
  `FUN_004b4650`, emit `FUN_004893d0..0048f780`, fade.
- **SHOTGUN(17)** arm `0045b200`: pool DAT_0068d158·0x24, `+0x10=4`. fire `0045b6e0`
  (mode==2 & `*(armed+0x10)!=0`): `FUN_0045b390(armed)`. canfire `0045b260`:
  `*(armed+0x10)==0 && *(armed+0xc)==0`. deact `0045b290`: `+0x18=2;+0x14=0`. tick
  `0045b700`: pool sm (1 track+decr, 2 `FUN_0045b360`); twin-pellet `FUN_004c1520`
  spread + line query `FUN_004b40f0`.
- **FLASH(18)** arm `00454a40`: pool DAT_00688358·0x14. fire `00454db0` (mode==2 &
  `*(armed+0xc)==1`): `FUN_00454c10` (trigger blind) + FX `FUN_00465ca0(0x18,pos)`.
  canfire `00454a90`: `*(armed+0xc)==4`. deact `00454ab0`: `+0xc=5;+8=0`. tick
  `00454e00`: pool DAT_00688358·0x14 → `FUN_00454c60`; sub DAT_006882f8·0x18 →
  `FUN_00454cd0`; `FUN_00476df0(&DAT_0088fec0)`.
- **OIL(19)** arm `00456d80`: pool DAT_0068a250·0x10, `+0x00=1.0` (supply). fire
  `00457800` (continuous, ignores mode): if owner moved ≥`_DAT_005cc56c` since last
  drop (trail DAT_0068a290[owner]·0xc), ground-contact `FUN_0045bfe0`→`FUN_004b4cd0`,
  place slick `FUN_004b4650`/`FUN_004b5080`/`FUN_004c51a0`, FX `FUN_00465e80(0x17)`,
  supply `DAT_0068a250 -= _DAT_005cc56c`. canfire `00456dd0`: `supply < 0.0`. deact
  `00456e00`: detach. tick `004577b0`: pool DAT_0068a258·0x10 track via `FUN_004c1480`;
  `FUN_00457100`.
- **MORTAR(7)** arm `00453350`: pool DAT_00684e38·0x1c, `+0x0c=3,+0x10=-1.0`,
  `FUN_004b5240`. fire `004533b0` (mode==1 & ammo≥0 & cooldown≤0): ammo--, cooldown
  `+0x10=0.4`; spawn into DAT_00684ea8·0x110 pool; vel from owner +0x20/+0x90 ×
  `_DAT_005cd02c`/`_DAT_005cc9c8` (straight) or random arc; FX `FUN_00465ca0(0x12,0x1e)`.
  canfire `00453610`: `ammo<1`. deact `00453630`. tick `00453b80`: pool track +
  shadow `FUN_00459620`; sub-pool DAT_00684ea8·0x110 → `FUN_004538b0`+`FUN_004532f0`.

(The full verbatim C of every function above was read this session; small fns are
reproduced 1:1 in `Powerup/PowerupEffects.cpp` headers, big fns are ported as their
decision logic with the leaf calls routed through `IPowerupBackend`.)

---

## 6. Harvested tuning constants (read 2026-06-16 from .rdata, slot Mashed_pool12)

Float values read directly from MASHED.exe (`memory.getInt` → `<f`) at each address
the effects reference. These are the real values; use them verbatim when the
projectile pools + RW scene-graph land and the effect bodies are de-stubbed.

| addr | value | seen in / meaning |
|---|---|---|
| DAT_005d757c | **0.0** | the float-zero sentinel (all effects compare against it) |
| DAT_005cc320 | 1.0 | GUN/MISSILE/R_FLAME unit/clamp |
| DAT_005cc31c | 3.0 | spin/clamp (GUN tick, MISSILE tick) |
| DAT_005ce4d4 | -3.0 | paired with 005cc31c |
| DAT_005ccac4 | 360.0 | angle wrap (GUN tick yaw) |
| DAT_005ccad0 | 90.0 | MISSILE aim base angle (FUN_00455150) |
| DAT_005cd988 | 0.075 | MISSILE aim pitch offset (FUN_00455150) |
| DAT_005ce480 | 0.0 | MISSILE aim horizontal factor (FUN_00455150) |
| DAT_005cc33c | -1.0 | MISSILE aim sign (FUN_00455150) |
| DAT_005cd274 | 40.0 | **GUN charge cap** (+0x18 ramps to this, FUN_004561c0) |
| DAT_005cc35c | 4.0 | **GUN charge per-frame increment** (FUN_004561c0) |
| DAT_005cc55c | 10.0 | GUN spread/recoil (FUN_004561c0) |
| DAT_005cc568 | 100.0 | GUN long-range branch (FUN_004561c0) |
| DAT_005cc9c8 | 0.9 | **MORTAR owner-velocity factor** (FUN_004533b0) + GUN |
| DAT_005cd02c | 0.13 | **MORTAR forward-velocity factor** (FUN_004533b0) |
| DAT_005cc99c | 0.3 | MORTAR/GUN/R_FLAME random spread |
| DAT_005cc998 | -0.3 | paired with 005cc99c |
| DAT_005cc56c | **0.1** | **OIL drop-distance² threshold AND supply decrement** (FUN_00457800); supply starts 1.0 → 10 drops |
| DAT_005cd18c | 0.04 | OIL reflect / MISSILE blend factor |
| DAT_005cc32c | 0.5 | GUN aim normalise |
| DAT_005cc9a8 | 0.023 | GUN tracer scale |
| DAT_005cd0fc | -0.1 | GUN tick clamp lo |
| DAT_005cd088 | 2.5 | GUN/MISSILE tick gain |
| DAT_005ce1b0 | -2.5 | paired with 005cd088 |
| DAT_005cc348 | 1.5 | MISSILE/R_FLAME radius |
| DAT_005ccac0 | 0.4 | MISSILE tick threshold |
| DAT_005cd50c | -0.5 | MISSILE tick gain |
| DAT_005cd054 | -0.05 | MISSILE tick clamp |
| DAT_005ce018 | 0.002 | R_FLAME spark drift (FUN_0045ae80) |
| DAT_005ce18c | 0.02 | R_FLAME spark reflect (FUN_0045ae80) |
| DAT_005cd114 | 1.111111 | R_FLAME spark life scale (FUN_0045ae80) |
| DAT_005cc564 | 0.25 | R_FLAME spark clamp (FUN_0045ae80) |

(`_DAT_007f100c` = the per-frame dt and `_DAT_007f101c` = a per-frame flags dword are
runtime RAM, not .rdata, so they are 0 statically — not harvestable here.)

**Promoted into the port (PowerupEffects.cpp `tune::`):** the scale-independent
counts/rates/seconds are now bit-faithful — GUN ammo 50 / rate 0.06s, MISSILE ammo 1,
MORTAR ammo 3 / rate 0.4s, SHOTGUN 4 pellets, **OIL 0.1 supply-decrement (10 drops)**.
The projectile launch *velocities* (MISSILE/MORTAR) and the R_FLAME duration stay
host-scaled stand-ins (world-coordinate / frame-count domain — not portable to the
radius-relative backend); the real model is cited in code for the verbatim de-stub.
