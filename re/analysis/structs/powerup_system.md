# Power-up System — dispatch + per-type effect cluster (WS-D / D1)

**Session:** WS-D powerup-effects D1 (2026-06-16), Ghidra slot Mashed_pool5
**Status:** D1 map COMPLETE — dispatch architecture + 9-entry type table + per-type
effect-fn RVAs fully enumerated from the binary. D2 (port per-type) and D3 (wire +
diff) are GATED — see "Gating" at the bottom.
**Anchor:** MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
(read read-only from Mashed_pool5; every RVA below cited from that program).

> **ROADMAP correction.** ROADMAP.md WS-D and SESSION_VERIFICATION_AUDIT_2026-06-16
> name the real power-up family as `FUN_00430670`. That is **wrong**: `FUN_00430670`
> (`re/analysis/hud_frontend_d5/0x00430670.md`, `frontend_c1_to_c2_followup_s1`) is a
> **player-slot resolver** (maps an ordinal position to a player slot index via
> `DAT_0067e9fc`/`FUN_00413f90`), unrelated to power-ups. The real power-up cluster is
> the **`FUN_0045bba0` dispatcher + the 9-entry type table at `0x005f9998`**, documented
> here.

---

## 1. Architecture overview

The power-up subsystem is a vtable-dispatched effect system:

- A **type table** at `0x005f9998` (9 entries, stride 0x40). Each entry = a type code
  (+0x00) plus ~14 function pointers (the per-type "vtable").
- Per-frame the **central dispatcher `FUN_0045bba0`** (entry 0x0045bba0) walks the
  4 player power-up **slots** and the 9 type entries, calling the appropriate vtable
  slots (ARM / FIRE / CAN_FIRE / DEACTIVATE / per-frame ticks).
- A small set of **lifecycle helpers** (`FUN_0045baa0` lookup, `FUN_0045bfa0` activate,
  `FUN_0045bac0` deactivate, `FUN_0045c010` pickup-wrapper).
- Input arrives through the shared **controller-state arrays** at `0x007f103c` — AI and
  human players use the same fire-button bytes (no separate AI fire path; confirmed in
  `re/analysis/ai_powerup_decisions/SESSION_END.md`).

### Ghidra bounds caveat (D2 prerequisite)
The whole effect-code region **`0x00453f60`–`0x0045be81`** is currently ONE merged
"function" in the master project (Ghidra reports `FUN_0045bba0` with
body_start=0x00453f60, body_end=0x0045be81). The per-type effect functions the type
table points into are mostly **not individually defined** — e.g. `function_at(0x004561c0)`
returns null even though `0x004561c0` is a real prologue (`SUB ESP,0x34`, verified via
listing). A handful ARE defined (e.g. `FUN_00455150` Missile-FIRE). **Splitting this
region into its ~80 real functions (`function_create` at each table-pointer target) in
the master Ghidra project is a prerequisite for the D2 verbatim port** and is itself a
write-to-master session.

---

## 2. Power-up slot struct (per player)

Slot array base **`0x0088fbe0`**, stride **0xb4 (180 bytes)**, **4 slots**
(slot N base = 0x0088fbe0 + N*0xb4). Derivation: `FUN_0045bba0` walks a working pointer
`pfVar15` from `&DAT_0088fc70` (= slot0 + 0x90) in 0xb4 steps while `< 0x88ff40`
(→ 4 iterations); `FUN_0045bfa0` writes the active entry at `slot+0xa8`, and the
dispatcher reads that same entry at `pfVar15[6]` (= slot+0xa8). The two reconcile at
slot0 = 0x0088fc70 − 0x90 = 0x0088fbe0.

| Slot offset | Meaning | Evidence |
|---|---|---|
| +0x00 .. +0x8f | per-slot working/transform block (position, prev-position, deltas) | dispatcher reads pfVar15[-0x24..], FUN_0041f060 transforms |
| +0x30 (pfVar15[-0x18]) | current position vec3 (x,y,z) | dispatcher `fVar4..6 = *pfVar1..` |
| +0x70 (pfVar15[-8]) | second vec3 (prev pos / vel) | dispatcher `fVar7..9` |
| **+0xa8** (pfVar15[6]) | **active type-table entry ptr** (0 = none held) | `FUN_0045bfa0` writes it; deactivate clears it |
| **+0xac** (pfVar15[7]) | **ARM-result handle** (entry+0x04 return; the armed instance/state) | `FUN_0045bfa0`; dispatcher gates fire on `pfVar15[7] != 0` |
| +0xb0 | owner controller/player index (used by FIRE handlers, e.g. Missile reads owner+0xb0) | `FUN_00455150` `*(param_1+0xb0)` |

(`0x0088fc70` in the AI note = the dispatcher's working pointer slot0+0x90, NOT the
struct base — corrected here.)

---

## 3. Type-table entry vtable (stride 0x40)

Offsets cross-validated against the dispatch loops that call each slot:

| Entry offset | Role | Caller / evidence |
|---|---|---|
| +0x00 | **type code** (int) | `FUN_0045baa0` matches ESI against it |
| +0x04 | **ARM** — called on activate; returns state handle → slot+0xac | `FUN_0045bfa0`: `slot+0xac = (*(entry+4))(slot)` |
| +0x08 | **FIRE** — per-frame when fire pressed & can-fire; arg = fire_mode (1/2/3) | dispatcher `(*(entry+8))(slot, local_64)` |
| +0x0c | **CAN_FIRE / EXPIRE** — query; nonzero ⇒ auto-deactivate | dispatcher `(*(entry+0xc))()`, then `FUN_0045bac0` |
| +0x10 | **DEACTIVATE** — tears down the armed instance | `FUN_0045bac0`: `(*(entry+0x10))()` |
| +0x14 | INIT / startup (subsystem) | per-type init dispatcher |
| +0x18 | SHUTDOWN | `FUN_0045b930` (+0x18 dispatch) |
| +0x1c | **per-frame TICK** — called for ALL types every frame | `FUN_0045bba0` loop2 `&PTR_FUN_005f99b4`, stride 0x40 |
| +0x20 | (dispatched by `FUN_0045b990`) | `FUN_0045b990` (+0x20) |
| +0x24 | per-frame RENDER | `FUN_0045b9d0` (+0x24) |
| +0x28 | per-frame UPDATE | `FUN_0045be90` (+0x28) |
| +0x2c | ROUND CLEANUP | `FUN_0045bed0` (+0x2c) |
| +0x30 | TEARDOWN-ALL | `FUN_0045bf30` (+0x30) |
| +0x34 | [UNCERTAIN U-WSD-1] set for GUN/MORTAR/MISSILE/etc; calling dispatcher not yet found | raw table |
| +0x38 | network-mode (FUN_00426c00()==0x21) per-frame fn | `FUN_0045bba0` loop3 `puVar14[-1]` |
| +0x3c | single-player-mode (FUN_00426c00()==2) per-frame fn | `FUN_0045bba0` loop3 `*puVar14` (&DAT_005f99d4) |

`0x0045b350` = the shared **no-op handler** (a single `RET`, verified via listing). It
fills the lifecycle slots a given type doesn't use.

Table count = **`DAT_005f9bd8` = 9** (dword immediately after the 9th entry:
0x005f9998 + 9*0x40 = 0x005f9bd8). A 3rd structure (per-type config: index, a recurring
`10.0f`=0x41200000, a `-1` sentinel) begins at 0x005f9bdc — [UNCERTAIN U-WSD-2], not in
D1 scope.

---

## 4. The 9 type-table entries (complete)

Order is table order (NOT sorted by code). All RVAs read from `0x005f9998` raw bytes.
LUA names from `POWERUPS_GOLD.LUA` (`MINE=6 MORTAR=7 DETONATOR=8 GUN=9 DRUM=10
MISSILE=11 P_MINE=12 R_FLAME=16 SHOTGUN=17 FLASH=18 OIL=19 BLANK=21`).

| # | entry RVA | code | LUA name | +0x04 ARM | +0x08 FIRE | +0x0c CANFIRE | +0x10 DEACT | +0x14 INIT | +0x18 SHUT | +0x1c TICK | +0x24 REND | +0x28 UPD | +0x2c CLEAN | +0x30 TEAR | +0x34 ? | +0x38 net | +0x3c sp |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 0x005f9998 | 9  | GUN     | 0x456040 | 0x4561c0 | 0x4566d0 | 0x4566f0 | 0x456760 | 0x4568a0 | 0x4568d0 | 0x456c90 | 0x45b350 | 0x45b350 | 0x45b350 | 0x456740 | 0 | 0 |
| 1 | 0x005f99d8 | 10 | DRUM    | 0x453fe0 | 0x454740 | 0x457ab0 | 0x457ad0 | 0x4548e0 | 0x4040c0 | 0x454820 | 0x45b350 | 0x454a20 | 0x457490 | 0x454a20 | 0x457af0 | 0 | 0 |
| 2 | 0x005f9a18 | 11 | MISSILE | 0x455060 | 0x455150 | 0x455360 | 0x455390 | 0x455400 | 0x4555d0 | 0x455c90 | 0x455fd0 | 0x455fd0 | 0x455fd0 | 0x455fd0 | 0x4553e0 | 0x455c40 | 0 |
| 3 | 0x005f9a58 | 12 | P_MINE  | 0x457a30 | 0x457ef0 | 0x457ab0 | 0x457ad0 | 0x4583a0 | 0x457b10 | 0x4582f0 | 0x457f30 | 0x4584d0 | 0x457ff0 | 0x4584d0 | 0x457af0 | 0x457e40 | 0x457eb0 |
| 4 | 0x005f9a98 | 16 | R_FLAME | 0x45a7c0 | 0x45a850 | 0x45a890 | 0x45a8b0 | 0x45a6b0 | 0x45a790 | 0x45ae80 | 0x45ac20 | 0x45b350 | 0x45b350 | 0x45b350 | 0x45a910 | 0x45ad70 | 0 |
| 5 | 0x005f9ad8 | 17 | SHOTGUN | 0x45b200 | 0x45b6e0 | 0x45b260 | 0x45b290 | 0x45b830 | 0x45b2d0 | 0x45b700 | 0x45b350 | 0x45b920 | 0x45b350 | 0x45b920 | 0x45b2b0 | 0 | 0 |
| 6 | 0x005f9b18 | 18 | FLASH   | 0x454a40 | 0x454db0 | 0x454a90 | 0x454ab0 | 0x454e70 | 0x454af0 | 0x454e00 | 0x45b350 | 0x454f70 | 0x454f70 | 0x454f70 | 0x454ad0 | 0 | 0 |
| 7 | 0x005f9b58 | 19 | OIL     | 0x456d80 | 0x457800 | 0x456dd0 | 0x456e00 | 0x4576f0 | 0x456ca0 | 0x4577b0 | 0x456ea0 | 0x45b350 | 0x45b350 | 0x456ea0 | 0x456e50 | 0 | 0x457650 |
| 8 | 0x005f9b98 | 7  | MORTAR  | 0x453350 | 0x4533b0 | 0x453610 | 0x453630 | 0x452f70 | 0x4530b0 | 0x453b80 | 0x45b350 | 0x45b350 | 0x45b350 | 0x453720 | 0x453670 | 0 | 0 |

(+0x20 column omitted from the table for width; GUN +0x20=0x456c30, DRUM=0x454130,
MISSILE=0x455f50, P_MINE=0x457b70, R_FLAME=0x45abb0, SHOTGUN=0x45b300, FLASH=0x454b30,
OIL=0x45b350, MORTAR=0x453690.)

### Type codes present vs the 12 LUA codes
- **Present (9):** GUN(9), DRUM(10), MISSILE(11), P_MINE(12), R_FLAME(16), SHOTGUN(17),
  FLASH(18), OIL(19), MORTAR(7).
- **Absent from the table (3):** MINE(6), DETONATOR(8), BLANK(21).
  - [UNCERTAIN U-WSD-3] BLANK(21) is the LUA "random box"; resolution: trace the
    pickup→activate type source (the dispatcher pickup branch calls `FUN_0045c010` with
    a register-passed type that the decompiler can't surface — it pushes `0x10` on the
    stack but the ESI type is invisible). Likely BLANK rolls a random one of the 9, and
    MINE(6)/DETONATOR(8) alias onto P_MINE(12)/DRUM(10) — **NOT confirmed this session.**

---

## 5. Lifecycle helpers (all small, fully read)

- **`FUN_0045baa0` PowerupTypeLookup** — linear search of the 9 entries for ESI==code;
  returns entry ptr or NULL. (`for i in 0..DAT_005f9bd8: if ESI==*(0x005f9998+i*0x40)`.)
- **`FUN_0045bfa0` PowerupSlotActivate(slot)** — `slot+0xa8 = lookup(ESI)`;
  `slot+0xac = (*(entry+0x04))(slot)` (ARM). **No NULL-guard** ⇒ activating an absent
  code (6/8/21) would deref NULL+4, reinforcing U-WSD-3 (codes must be normalized before
  activate).
- **`FUN_0045bac0` PowerupSlotDeactivate** — `(*(slot+0xa8 → +0x10))()`; clears
  slot+0xa8/+0xac.
- **`FUN_0045c010` PowerupPickupWrapper** — thin pass-through to `FUN_0045bfa0`.

---

## 6. Central per-frame dispatcher `FUN_0045bba0` (entry 0x0045bba0)

Gated on `FUN_0040e350() == 6` (game-mode discriminator). Three passes:

1. **Per-slot pass** (4 slots, 0x0088fbe0 stride 0xb4): updates the slot transform
   (FUN_0041f060 ×2), runs a per-slot state machine `DAT_0068d1f0[slot]` (states 2/3/4),
   and when a power-up is armed (`slot+0xac != 0`):
   - sweep/contact query: `FUN_0045bfe0` → `FUN_004b4b60` → `FUN_0045c350`
     (on failure: deactivate + `FUN_00476880` effect at consts 1.5f/360.0f);
   - **CAN_FIRE**: `(*(entry+0x0c))()` — nonzero ⇒ deactivate;
   - read controller fire bytes (`DAT_007f103c[ctrl*0x4c+3]`, `DAT_007f14ff[ctrl*0x4c]`)
     → compute fire_mode `local_64` ∈ {1,2,3};
   - **FIRE**: `(*(entry+0x08))(slot, fire_mode)`;
   - pickup branch (no power-up held): if collected flag `DAT_007f1055[ctrl*0x4c]` set &
     not consumed → `FUN_0045c010` (arm). [type source = U-WSD-3]
2. **Per-type TICK pass** — `(*(entry+0x1c))()` for all 9 types.
3. **Per-type mode pass** — `FUN_00426c00()`: ==2 → `(*(entry+0x3c))()` (single-player);
   ==0x21 → `(*(entry+0x38))()` (network).

Tail: `FUN_0045a190`, `FUN_00459000`, then a deferred-effect queue at `DAT_006870b8`
(100 entries × 0x24 bytes, until 0x687ec8) driving `FUN_00484cf0(...,0x19)`.

---

## 7. Representative effect — Missile FIRE `FUN_00455150` (entry 2 +0x08)

`void FUN_00455150(owner, fire_mode)` — only acts on `fire_mode==2`:
- ammo from `DAT_006885d8[owner_idx*0x2c]` (decrement, bail if <1);
- spawns a projectile into the global **projectile pool** at `0x006883bc..` (stride
  **0x6c**, 0x1b ints/entry); sets pos from `FUN_004c0ed0()+0x30/+0x34/+0x38`, aim from
  owner angles (`FUN_004726f0`/`FUN_004a3384` trig × tuning consts `_DAT_005ce480`,
  `_DAT_005ccad0`, `_DAT_005cd988`, `_DAT_005cc33c`, `DAT_005d757c`);
- attaches the missile model to the RW scene graph (`RwFrameAddChild`/`RemoveChild`,
  `FUN_00474d60`) and registers FX (`FUN_00465e80(0x12/0x14, …)`).

This single effect already depends on: the **owner vehicle struct** (fields +0x20/+0x90/
+0xb0 → WS-A1), the **projectile pool struct** (new), the **RW scene-graph/renderer**
(WS-E), **tuning constants** (WS-A7-style harvest of DAT_005c/005d), and — for the
per-frame tick (entry+0x1c=`0x00455c90`, homing + impact) — **collision/contacts**
(WS-B). The hitscan types (GUN/SHOTGUN) and area types (OIL/FLASH/R_FLAME) have their own
per-type dependency profiles.

---

## 8. Input bridge (for D3 wiring)

Per controller `c` (stride 0x4c), the dispatcher reads:
- `(&DAT_007f103c)[c*0x4c + 3]` — primary fire button (char)
- `(&DAT_007f14ff)[c*0x4c]` — secondary fire flag
- `(&DAT_007f1040)[c*0x4c]`, `(&DAT_007f1500)[c*0x4c]` — second button + edge flag
- `(&DAT_007f1055)[c*0x4c]` / `(&DAT_007f1515)[c*0x4c]` — pickup-collected flag + edge
- `(&DAT_007f1a14)[slot*4]` — slot→controller index

AI drivers write the SAME bytes (no separate AI fire dispatch — the power-up subsystem is
policy-blind; the choose-to-fire logic is upstream in the AI driver update, WS-C).

---

## 9. Gating — why D2/D3 are NOT done in this session

D1 (this map) is **not** blocked on anything and is complete. The verbatim port is:

- **D2 (port per-type effects)** is gated on:
  1. **Ghidra function-split** of 0x00453f60–0x0045be81 in the master project (§1 caveat).
  2. **WS-A1** — the vehicle/owner struct map (effects read/write owner+0x20/+0x90/+0xb0…).
  3. A new **projectile-pool struct** map (0x006883xx, stride 0x6c) + the per-type
     ammo/config arrays (0x006885xx; 0x005f9bdc registry).
  4. **WS-E** — RW scene-graph (`RwFrameAddChild`) for projectile/effect models.
  5. Tuning-constant harvest (DAT_005c…/005d…), as WS-A7 does for handling.
- **D3 (wire + diff)** additionally needs **WS-B** (car↔car / car↔projectile contacts)
  for missile/mine impacts, and the boot-original `diff-original` lane (WS-H) for C4.

**Recommended D2 entry point:** start with the self-contained, low-dependency effects
(OIL `0x457800`, FLASH `0x454db0` — area/screen effects that touch fewer vehicle fields)
once WS-A1 lands; defer projectile types (MISSILE/MORTAR/DRUM) until WS-B contacts exist.

Until then the standalone keeps the PickupField economy scaffold; its type roster has been
made **data-faithful** to the real 9 codes (see CHANGELOG / PickupField).
