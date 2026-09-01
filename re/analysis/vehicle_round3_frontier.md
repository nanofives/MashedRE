# Vehicle round-3 frontier map (characterization, no authoring)

Area-vehicle child, 2026-09-01. Parent-directed characterization round: for each
queued live-state row, name the **exact observable** a diff must read to tell
"ran and correct" from "never ran", group rows by the **harness** they'd need, and
flag anything **ai** also touches. No hooks authored this round.

All RVAs decoded via `analyzeHeadless` + `DecompPC.java` against `Mashed_pool4`
(read-only), evidence `log/area-vehicle/frontier_dec.json`. NO-GUESSING: every
address below is cited from the decomp; where a field's meaning is unproven it is
reported as a raw offset.

Note on stride: the decomp prints `idx * 0x341` on **dword** arrays, i.e. a byte
stride of `0x341 * 4 = 0xd04`. All "+0xNNN" offsets below are byte offsets from
the record base `0x00881630`.

---

## The shared per-vehicle record (CROSS-AREA — reported to parent for the bus)

Base `0x00881630`, stride **0xd04**, 16 slots (all these fns gate `idx > 0xf -> return 0`).
It is **one struct**, written by vehicle and read by ai:

| field addr | = base + | written/read by | role (cited, not guessed) |
|---|---|---|---|
| `0x00881630` | +0x000 | **vehicle** `0x0046b1c0` writes +0x00..+0x9c | AABB corners/midpoints block (~40 floats) |
| `0x00881ec8` | +0x898 | **ai** `0x0046d510` reads (arg4 to FUN_004c3df0); `0x0046d4a0` returns ptr into it | source/position block |
| `0x00881f48` | +0x918 | **ai** `0x0046d4a0` reads as a `*0x10` multiplier index | an index field |
| `0x00881f74` | +0x944 | **ai** `0x0046d510` writes out[0..2] from here (post FUN_004c3df0 transform w/ matrix `0x00614708`) | velocity block |

**=> CROSS_AREA_BUS candidate:** vehicle's `0x0046b1c0` AABB writer and ai's
`0x0046d510`/`0x0046d4a0` velocity getters address the SAME 0xd04-stride per-vehicle
record. A harness that seeds/reads any of these must agree on the layout. Reported
to the parent via send_to_session (children never edit the bus).

---

## Per-row observable + witness

### Group S — seed a record field/param, call, fingerprint the out, restore
Shared harness shape: seed N bytes of the per-vehicle record at `0x00881630+idx*0xd04`
(and/or matrix `0x00614708`), call `fn(out, idx)`, fingerprint the out region + fold
the return, **save/restore** the seeded record. One new arg_type covers all three.

- **`0x0046b1c0`** `int fn(uint slot, float* in6)` — **PURE-PARAM**, the cleanest of the
  three. Writes ~40 floats to `0x00881630+slot*0xd04` (+0x00..+0x9c) derived ENTIRELY
  from `in6[0..5]` + const `_DAT_005cc32c` (=0.5f); the apparent reads of the record are
  reads of its OWN same-call writes. **Observable:** the ~40 output floats at
  `0x00881630+slot*0xd04`; distinct `in6` per test -> distinct fingerprints. Needs only
  a save/restore of the slot region (no live state read). *This is the row to land first.*
- **`0x0046d4a0`** `int fn(void** out, uint idx)` — address computation:
  `*out = 0x00881ec8 + [0x00881f48+idx*0xd04]*0x10 + idx*0xd04`. **Observable:** the
  pointer written to `*out`; witness-correct needs the index field at `0x00881f48+idx*0xd04`
  seeded (one dword). Reads one live field only -> synthetically diffable with a 1-field seed.
- **`0x0046d510`** `int fn(float* out3, uint idx)` — reads the velocity block
  `0x00881f74+idx*0xd04` (after `FUN_004c3df0` transforms it with matrix `0x00614708`),
  writes `out[0..2]`. **Observable:** `out[0..2]` + return. This is the row the out3_idx
  audit DEMOTED for a degenerate all-zero green on unseeded state; ai already built a
  contrived-state lane for it (`cache_setter_observe`, registry `0x0046d510`). Witness-correct
  requires seeding the velocity block + matrix; **must** run with a control hook known to fire
  (absent-out proves nothing).

### Group R — RW render-state dispatch recorder
- **`0x00411ce0` Ghost::SetupRender** `void(void)` — gate `FUN_0042f6a0()==2`, then 5 calls
  through the RW device vtable `(**(DAT_007d3ff8 + 0x20))(state,value)`:
  `(10,5)(0xb,6)(8,1)(6,1)(0x14,2)`. **Observable:** patch the device fn-ptr slot
  `DAT_007d3ff8+0x20` with a recorder and confirm the 5 `(state,value)` pairs in order
  (the `vtable_table_dispatch`/`stub_dispatch_observe` shape). `DAT_007d3ff8` is live at
  menu-attach (same global RwMatrixInvertEntry uses), so the *dispatch* is observable without
  a race **once** `FUN_0042f6a0` is seeded to return 2 (a mode global). Cleanest of the
  replay/ghost four.

### Group C — live replay/ghost recording state; NO clean synthetic observable yet
These need a booted race with replay/ghost recording ACTIVE (valid record pointers
`DAT_0063bb10/14/1c/20`). State-in, state-out; no isolable out-param.

- **`0x00411870` Replay::LapFinish** `void(int)` — gates `FUN_0042f6a0()==2` and
  `DAT_0063bb20==0`; fills a local vec via `FUN_0046d4a0`, then **appends a frame** via
  `FUN_004829d0(DAT_0063bb14, vec, DAT_007f0ff4-DAT_007f1008, 0xffffffff)`, walks table
  `DAT_005f29d0`, reads `*(DAT_0063bb10+0x174)`. **Observable:** the bytes appended to the
  replay buffer pointed to by `DAT_0063bb14`, but that pointer must be a valid live record.
  **No clean synthetic observable** — needs live replay recording.
- **`0x00411ae0` Ghost::PlaybackTick** `void(p1, uint frame)` — writes `DAT_0063bb1c = frame`
  (a frame-index global), then interpolates from the ghost record at `DAT_0063bb10` via
  `FUN_00482c10` (Replay::ReadFrame, outputs a 16-float matrix). **Observable(did-run):**
  `DAT_0063bb1c == frame` — cheap witness it executed. **Observable(correct):** the 16-float
  matrix from Replay::ReadFrame, which needs `DAT_0063bb10` -> a valid ghost buffer.
  Did-run witness is synthetic-cheap; correctness needs a live ghost.
- **`0x00411d90` Replay::CreateOrLoad** `void(int)` — heavyweight: `_wprintf` logging,
  security cookie, ~10 callees, builds a 64-byte replay struct (`local_44[64]`), loads/allocates
  a replay. **No clean synthetic observable** — file/asset + allocation side effects; booted
  state only.

---

## Recommendation to the parent (harness economics)

- **One new arg_type** (`per_vehicle_record_seed`: seed M bytes at `base+idx*stride`,
  optionally seed a side matrix, call `fn(out, idx)`, fingerprint out region + fold return,
  save/restore) covers **all of Group S** (`0x0046b1c0`, `0x0046d4a0`, `0x0046d510`) and
  subsumes ai's existing `cache_setter_observe`. This is the same generalization frontend's
  `seed_globals+fold_ret` is reaching for, extended to a strided record. Cheapest single
  approval; `0x0046b1c0` (pure-param) is the zero-risk first landing.
- **Group R** (`0x00411ce0`) reuses an existing dispatch-recorder shape + one mode-global seed;
  no new handler, but it is a booted-adjacent observe.
- **Group C** (`0x00411870`, `0x00411ae0` correctness, `0x00411d90`) is genuinely the parent's
  booted-race lane — a race with replay/ghost active, and a control hook known to fire in the
  same boot (per the parent's absent-log lesson). `0x00411ae0`'s `DAT_0063bb1c==frame` did-run
  witness is the one cheap synthetic toe-hold in this group.

## Not in the cluster (child-synthetic lane, confirmed exhausted this round)
`0x00474d60` (named callee RpClumpForAllAtomics but param=0 no-op degenerate, live-clump
otherwise), `0x004039c0` (loads Bomb.dff, anonymous loader callee), `0x0041a960` (2-iter
init, anonymous callee), `0x004927c0` (C1 void input-state reader needing a big seed_globals).
None self-landable synthetically.
