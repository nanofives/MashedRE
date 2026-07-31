# Mashed RE orchestrator - resume point (updated 2026-07-31, end of iter27)

> ## iter27: the synthetic getter lane is MINED OUT. This is a mission-level fork.
>
> Directive was "find a bucket with SAFE read-only getter leaves." I briefed **32
> fresh C2 rows across 6 buckets** (audio x2, hud x2, vehicle, frontend),
> off-quota $2.86, 6/6 OK (`runs/iter27_getters/`). **Zero authorable getters.**
> The one GETTER_SAFE (`0x005aeed0`) is the ledger's known-degenerate row
> (`WaitForSingleObject((HANDLE)*p,0)` - every seed returns 1). Everything else
> dies on **harness safety**: teardown (FontSys shutdown, RW frees), WRITES_GLOBAL
> (allocator stores, frame-counter incs, video-handle clears), DESTROYS_DEVICE
> (vtable flush), or CALLS_UNKNOWN (uncharacterized callees, `RpClumpForAllAtomics`
> over live objects). The worker independently concluded the same.
>
> **This is structural, not a bad bucket.** Combined with iters 22/24 (util,
> gameplay, particle, render all 0 READY), **every subsystem is now sampled** and
> the getter lane is empty. The clean field/global getters were the EARLY
> promotions - that is *why* they are C3 now. What remains at C2 is dominated by
> mutators / teardown / device / dispatch, which is exactly why harness safety has
> been the bottleneck for three straight sessions. Briefing more getter buckets is
> spinning: **0 is the expected value.**
>
> ### THE FORK (mission-level - yours)
> Batch C2->C3 promotion via the synthetic getter lane is exhausted. To continue
> producing promotions, ONE of:
> 1. **Build harness capability** for the mutator/setter shapes that dominate the
>    remaining C2 pool - each unlocks a class, not one row:
>    - `stub_ret_buf` + `observe_buf` on `stub_dispatch_observe` (callee-stubbed
>      setters like the `0x004b8080`/`0x004b7fd0` Lua pair) - iter26 spec.
>    - per-side sentinel reset on `entity_field_set` (strided-global setters like
>      `0x0047cde0`) - iter26 spec; fixes a latent false-GREEN in a shipped handler.
>    - a real snapshot/restore mutator A/B driver (the `mutator_ab_pilot` lane) -
>      needs the deterministic-write-surface screen first (iter14: only 2 of 44
>      rows survive, and both need a call-frequency boot).
>    Each must be proven non-degenerate on a KNOWN-WRONG port before any promotion.
> 2. **Pivot to lane A** - fix the game per `RE_MASTER_PLAN_2026-07.md` / the
>    `re/analysis/plans/` frontier. The dual mission (A: fix the game, B: promote
>    functions) has leaned entirely on B for ~6 sessions; B is now capital-gated.
> 3. **Accept lower confidence** - stop chasing C2->C3 and mine C0->C1 / C1->C2
>    discovery (the `discover-c1-batch` lane), which does not need the runtime
>    harness at all.
>
> My read: option 1's `entity_field_set` sentinel-reset is the cheapest capital
> (small, reusable, fixes a real bug, unlocks a whole setter class), but whether
> B is even the right mission now is a call above the orchestrator.
>
> **Tracker conflict (route to re-classify, not resolved here):** `0x0041c090`
> plate says C1, hooks.csv says C2.
>
> **Hygiene:** slot 9 acquired + released cleanly (fifth clean cycle). Slots 5,
> 10, 11 hold leaked `.lock~`. `acquire` stakes a `.lock` Ghidra trips over.
> Never pool-wide `cleanup` while another session is live.

---

# (iter26 resume point - superseded above)
# Mashed RE orchestrator - resume point (updated 2026-07-31, end of iter26)

> ## iter26: BOTH "author next" rows need harness code, not a .cpp. STOP-AND-ASK.
>
> The directive was "author 0x004b8080". It is **not authorable with any existing
> handler** - verified against both candidates' source, not guessed. And the
> fallback 0x0047cde0's handler is **degenerate** for its shape. No boot was spent;
> neither row can produce honest evidence without new harness JS first. Ledger:
> `author_004b8080` (blocked), `entity_field_set_degeneracy` (candidate).
>
> ### 0x004b8080 - real handler gap (first genuine one after 7 false alarms)
> Raw listing (Mashed_pool9): `__cdecl void f(L, value, idx): slot =
> FUN_004b7ff0(L, idx); *slot = value; *(u16)(slot+0xc) = 0`. The tag write
> (`0` here vs `1` in sibling 0x004b7fd0) is the ONLY thing distinguishing the
> row. To observe it, callee `FUN_004b7ff0` (a Lua stack-realloc that derefs `*L`
> and AVs on a fake state) must be stubbed **to return a scratch-buffer pointer**,
> then the harness must **read that buffer back** at +0 (u32) and +0xc (u16).
> - `ptr_seed_observe`: reads buffers, **no `stub_at`** (iter25 grep).
> - `stub_dispatch_observe`: **has `stub_at`** but the stub returns a **static
>   int** (`stub_ret`, default 0) and observes only `observe_ret`/`observe_calls`
>   - never a post-call buffer. So `*slot=value` writes to `*0` -> AV, and the
>   tag write is invisible.
> - **Additive spec:** add to `stub_dispatch_observe` two knobs - `stub_ret_buf:k`
>   (recorder returns `&bufs[k]`) and `observe_buf:[{buf,off,type}]` (post-call
>   readback folded into the fingerprint). Then author 0x004b8080 (tag=0) AND
>   0x004b7fd0 (tag=1) as a pair; distinct VALUE per vector + the tag contrast
>   give non-degeneracy.
>
> ### entity_field_set is DEGENERATE for shared-live-global setters (reusable)
> 0x0047cde0 = `if (0<=i<200) (&DAT_006c9438)[i]=v` - `entity_field_set`
> (`diff_template.js:780`) shape-fits exactly, BUT `callFn` (line 5648) runs Orig
> then Reimpl **in one process on the same real global with no reset**, and the
> handler does no save/restore. Orig writes `v` first, so Reimpl's readback
> returns `v` **even if Reimpl writes nothing** -> every in-bounds vector is a
> false GREEN. Only an OOB vector tests anything (the bounds check), never the
> write's base/stride/value. **Fix (additive, reusable):** per-side sentinel
> pre-fill of `base+i*stride` before each side, so a non-writing port REDs.
> Plus, for THIS row: `DAT_006c9438` is live per-frame vehicle water-zone state
> (read by `FUN_00481a30`, the row's own caller-gate witness, as a byte bitfield),
> so the no-restore write is safe **only at the menu early-window**
> (`DAT_006c6eb0==0`).
>
> ### THE DECISION (yours)
> Both rows are gated on the same kind of work: a small, reusable **additive
> harness capability**, then self-verify it isn't itself false-GREEN (needs a
> boot), then author. Options:
> 1. Build `stub_ret_buf`/`observe_buf` on `stub_dispatch_observe` -> author the
>    0x004b8080 / 0x004b7fd0 Lua-setter PAIR.
> 2. Build the `entity_field_set` per-side sentinel reset -> author 0x0047cde0 at
>    the menu window (and it generalises to other strided-global setters).
> 3. Defer both; mine a different bucket where the getter lane still has SAFE
>    read-only leaves (thin, per iter24 - harness safety is the bottleneck).
>
> Whichever handler is built must be proven non-degenerate on a KNOWN-WRONG port
> before any promotion (a body that writes nothing must RED).
>
> **Hygiene:** slot 9 acquired + released cleanly (fourth clean cycle). Slots 5,
> 10, 11 hold leaked `.lock~`. `acquire` stakes a `.lock` Ghidra trips over -
> clear it before opening. Never pool-wide `cleanup` while another session is live.

---

# (iter25 resume point - superseded above)
# Mashed RE orchestrator — resume point (updated 2026-07-31, end of iter25)

> ## iter25: U-4976 resolved — it DISQUALIFIES 0x004c7600. Author 0x004b8080 next.
>
> The iter24 handoff called 0x004c7600 "one fact from READY". That fact — the
> identity of vtable slot `DAT_007d3ff8 + 0x88` — is now resolved, and it kills
> the row rather than promoting it. Evidence:
> `re/analysis/render_4_c1_to_c2_s4/U4976_resolution.md`, commit `e202fd26`.
>
> **Slot +0x88 = RwStandard index 16 = `FUN_004d0290` = the raster-unlock
> standard.** Proven from the binary: RwEngineOpen (`FUN_004c30b0`) registers
> 0x1d standards at device+0x48 via device-system request 0xb; the D3D9 device
> system fn `FUN_004c7a70` case 0xb calls installer `FUN_004c8e50`, whose inline
> {index,funcptr} table sets index 0x10 → `FUN_004d0290`; device+0x48+16*4 =
> device+0x88. `FUN_004d0290` writes raster fields, walks the raster parent chain
> (unbounded pointer chase), drives the **live D3D9 device** `DAT_007d4110`
> vtable +0x78, and releases surfaces. So a synthetic call of 0x004c7600 either
> takes the null path (both sides return 0 — degenerate false GREEN), AVs, or
> corrupts the renderer. `int_scalar` was the right shape; harness_safety is
> DESTROYS_DEVICE/AV. **0x004c7600 stays C2.**
>
> **The U-4976 row in `UNCERTAINTIES.md` is NOT closed** — that file is owned by
> the other live session. Close it via `re-classify` **after** multi-session
> coordination; the evidence note + commit are the substantiation.
>
> ### AUTHOR NEXT: 0x004b8080 (from the iter24 gate brief, still the best row)
>
> - Caller-gate satisfied: `0x004b87e0` (C2), resolved in iter24.
> - The iter24 brief said ADDITIVE_FIELD "ptr_seed_observe + stub_at" and claimed
>   ptr_seed_observe supports stub_at. **It does NOT** — I grepped its dispatch
>   block: `null_args`/`ptr_to` yes, `stub_at`/`stub_ret`/`stub_nargs` no.
>   **Use `stub_dispatch_observe` (`ARG_TYPES.md:57`)** — it already has `stub_at[]`,
>   scratch buffers, and `arg_layout`. Likely NO new field needed.
> - Shape: 3 params; param_1 is a Lua state pointer (typed undefined4 — the
>   recurring deref-defect). Body: `puVar1 = (undefined4*)FUN_004b7ff0(param_1,
>   param_3); *puVar1 = param_2` — stub `FUN_004b7ff0` to return a scratch-buffer
>   pointer, then observe the write. Plate layout: value at puVar1+0, tag uint16
>   at byte +0xc.
> - **AUTHORING NEEDS THE RAW LISTING FIRST** (rule #4). Open a slot, read
>   `FUN_004b8080` and `FUN_004b7ff0` via `listing_code_units_list` (NOT
>   `listing_disassemble_*` — write-gated on a slot), author the .cpp + registry,
>   `orch_preflight`, then verify in a `state_batch` boot. This is a full lift —
>   give it its own session; iter25 stopped here (context heavy after the U-4976
>   chase) rather than half-author an unverifiable hook.
> - If it clears, batch it with `0x0047cde0` (MUTATOR_LANE,
>   `(&DAT_006c9438)[param_1]=param_2`; `entity_field_set` (`ARG_TYPES.md:46`)
>   models the exact strided-global-array shape).
>
> **Handler discipline reminder (now 7 instances):** grep the dispatch block in
> `diff_template.js` before believing ANY handler-gap claim — including an
> ADDITIVE_FIELD one. iter25 refuted one more (the ptr_seed_observe/stub_at claim).
>
> **Do not bother:** 0x00421980 (TEARDOWN), 0x004cbb20 (DESTROYS_DEVICE —
> SetTextureStageState on live device), 0x004c0ed0 (returns param_1+0x50 →
> per-side false-RED), 0x004c7600 (this row), 0x0052df40/0x0052ddc0 (library).
>
> **Hygiene:** slot 9 acquired + released cleanly (third clean cycle). Slots
> 5, 10, 11 hold leaked `.lock~`. `acquire` stakes a `.lock` Ghidra trips over —
> clear the stake before opening. Never pool-wide `cleanup` while another session
> is live. On a slot, `memory_read`/`reference_to`/`decomp_function`/
> `listing_code_units_list` are read-safe; `listing_disassemble_*` is write-gated.

---

# (iter24 resume point — superseded above)
# Mashed RE orchestrator — resume point (updated 2026-07-31, end of iter24)

> ## iter24: the caller-gate fix is APPLIED AND PROVEN. Harness safety is the real bottleneck.
>
> **Zero rows returned `CALLERS_NEEDS_GHIDRA`** (versus 22 of 24 the run before).
> The caller column is now resolved orchestrator-side and marked authoritative in
> the prompt. `brief_caller_gate_fix` is closed. Brief: `runs/iter24_gate/`,
> 2/2 OK, 7 RVAs, $1.51 off-quota.
>
> **Resolution method — `function_callers` alone is NOT enough.** 9 of 17
> candidates returned zero callers because their call sites live in un-wrapped
> blocks. Use `function_callers`, else `reference_to` + `orphan_owners.tsv`.
> Two false positives to reject when reading `reference_to`:
> a `UNCONDITIONAL_JUMP` from a `thunk_` is an alias, not a caller
> (`0x004b7fd0`, `0x00496970`); a `DATA` reference is a function-pointer table
> slot, not a call (`0x0047b230`, `0x00474f30`, `0x004d8530`).
> Only 7 of 17 had a genuine caller — **and all 7 were C2+**, so the gate was
> never the real filter.
>
> **Still 0 READY, but now honestly so:** 4 MUTATOR_LANE, 2 NEEDS_GHIDRA,
> 1 ADDITIVE_FIELD. With callers settled, READY turns solely on arg_type and
> harness safety — and safety is where rows now die. Expect the synthetic getter
> lane to stay thin; **the yield is in the mutator and ADDITIVE_FIELD routes.**
>
> **Best next candidates, in order:**
> 1. **`0x004c7600`** — `int_scalar` is the right shape and the row is one fact
>    from READY: its whole effect is a vtable dispatch at `DAT_007d3ff8+0x88`
>    whose identity is open as **U-4976**. Resolve that slot in Ghidra; if it is
>    a read-only getter this flips to READY.
> 2. **`0x004b8080`** — the brief called it ADDITIVE_FIELD `ptr_seed_observe +
>    stub_at` and claimed ptr_seed_observe "supports stub_at". **I checked the
>    dispatch block in `diff_template.js`: it does NOT** (`null_args` and
>    `ptr_to` are there; `stub_at`/`stub_ret`/`stub_nargs` are not). But
>    **`stub_dispatch_observe` (`ARG_TYPES.md:57`) already has `stub_at[]`** plus
>    scratch buffers and `arg_layout`. Try that handler before writing any field.
>    Seventh instance of "the handler you want already exists under another
>    name" — grep the dispatch block before believing any handler-gap claim,
>    **including an ADDITIVE_FIELD one**.
> 3. **`0x0047cde0`** (MUTATOR_LANE, `(&DAT_006c9438)[param_1] = param_2`) —
>    `entity_field_set` already models exactly this strided-global-array shape.
>
> **Do not bother with:** `0x00421980` (TEARDOWN, detaches an atomic from the
> world), `0x004cbb20` (**DESTROYS_DEVICE** — vtable `+0x1a8` is
> `SetTextureStageState` on the live D3D device `DAT_007d4110`; a synthetic call
> reconfigures the renderer), `0x004c0ed0` (returns `param_1+0x50`, so per-side
> buffers differ and a raw-retval compare false-REDs; needs address
> normalisation).
>
> **Data defect:** `0x004d8530` is in `candidate_buckets.json` but is **not a
> function start** in the program — reachable only as a DATA reference from
> `0x004c34ab`. The buckets file needs a validity pass against the live program.
>
> **Hygiene:** slot 9 acquired and released cleanly twice this session. Slots
> **5, 10, 11 hold leaked `.lock~`**. The pool script's `acquire` stakes a
> `.lock` that Ghidra then trips over — clear the stake before opening, and
> never run pool-wide `cleanup` while another session is live.

---

# (iter23 resume point)
# (iter23 resume point — superseded above)

> ## iter23 SUPERSEDES the "START HERE" below — do NOT author `0x0052df40`
>
> **`0x0052df40` and `0x0052ddc0` are library residue and are not port targets.**
> `re/analysis/bucket_00516bb0/_BUCKET_HALT.md` declares bucket
> `0x00516bb0..0x0052df40` statically-linked third-party (libpng + zlib + a
> DevIL-shaped image loader); `0x0052df40` is literally the stated upper bound of
> its image-library region, and `0x0052ddc0` is inside it. That halt was taken
> under the stop-and-ask library-residue clause and says *"do NOT re-issue this
> bucket."* `hooks.csv` agrees: the row is tagged `library_residue` /
> `static-linked third-party` and cites that very file.
>
> **Why iter22's handoff got it wrong:** `0x0052df40` has TWO plates. The brief
> read the one with no library tag (`plate_rank` scores frontmatter confidence
> then size; a HALT report has neither), while `hooks.csv` cites the one that
> declares the halt. Fixed in `73273edd`.
>
> **Three gates were closed (commit `73273edd`), all verified:**
> 1. `orch_rank_gate.LIBRARY_BANDS` — `libpng-zlib` widened to
>    `0x00516000..0x0052df6d`, renamed `libpng-zlib-image-loader`. The old
>    `hi=0x0052a000` stopped ~0x4000 bytes short, so `library_band()` returned
>    `None` and **`orch_preflight` would have PASSED both rows and spent the
>    boot** — its cheapest disqualifier failing silently in the safe-looking
>    direction. The correct range already existed in
>    `bulk_add_library_residue.py:52`; the table had drifted out of sync.
>    Boundary-verified (`0x0052df6c`→library, `0x0052df70`→None) and
>    regression-verified (25 rows newly covered: 24 C2 + 1 C1, **zero C3/C4**).
> 2. `cited_plate()` — the `hooks.csv` `file` column now outranks `plate_rank`.
> 3. `screen_bucket()` — drops library-band rows, `_BUCKET_HALT.md`-cited rows,
>    and **already-C3/C4 rows** at queue-build time. The C3/C4 case was found by
>    smoke-testing the screen: `state_render_b1_s6` was still queuing
>    `0x004b6b00` and `0x004cbb50` at C3 (third instance of that class, after
>    iter22's `0x00407550`).
>
> **Measured: 27 of 144 candidate RVAs (19%) now never reach a worker** — 18
> already promoted, 9 library (7 `msvc-crt-main`, 2 `libpng-zlib-image-loader`).
> The 7 CRT rows include `0x004af32d` / `0x004af31a`, which iter22 *did* pay to
> screen.
>
> **Ground truth banked for `0x0052df40`** (so it is not re-derived if the scope
> call is ever revisited): full 18-instruction listing read from `Mashed_pool9`;
> both dimensions come from **param_1**, the destination (`MOV ECX,[EAX+0x2c]` at
> `0x0052df4a`, `IMUL ECX,[EAX+0x24]` at `0x0052df50`); `REP MOVSD` then
> `AND ECX,3` + `REP MOVSB` tail; `MOV EAX,1` at `0x0052df60` so the return is a
> constant and not a discriminator; `RET` with no operand → `__cdecl`.
> Caller-gate: `function_callers` returns **0** because the call sites
> (`0x005517be` for `0x0052df40`; `0x005517a5` / `0x005519c1` for `0x0052ddc0`)
> live in un-wrapped blocks; `reference_to` + `orphan_owners.tsv` put all three
> under owner `0x005515a0`, **C2**.
>
> **NEXT:** the ladder has no authorable row. Re-brief a fresh bucket — the
> queue builder is now trustworthy, so use
> `py -3.12 scripts/orch_make_brief_queue.py <out.json> <bucket_id>...` rather
> than an ad-hoc plate resolver (iter22's ad-hoc script picked plates by size,
> which is exactly what caused this). Still apply `brief_caller_gate_fix`:
> resolve callers + confidence into the prompt table, or the screen still cannot
> emit READY.
>
> **Hygiene:** pool slot **11 now also holds a leaked `.lock~`** (a failed MCP
> open leaks an in-JVM lock; `rm` says "Device or resource busy"). **5, 10, 11
> are all leaked; slot 9 works** and was released cleanly. Note the pool script's
> `acquire` stakes a `.lock` that Ghidra then trips over — clear that stake
> before opening, and never run pool-wide `cleanup` while another session is
> live.

---

# (iter22 resume point — superseded above)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions.

**C1 795 / C2 4005 / C3 881 / C4 185.** Branch `fix/u9025-recharacterise-and-regabi-defects`,
clean, pushed, still 153 ahead of `origin/main` (unmerged).

Resume with `/orchestrate` — it reads `re/orchestrator/state.json`, which is current.
iter22 ran its full 4-cycle budget. No hard stop; the budget simply ran out.

---

## START HERE: author `0x0052df40`. It is authorable NOW.

iter21 handed over "nothing is authorable". **That was wrong**, and iter22's main job was
finding out why (below). Concretely:

- **`0x0052df40`** — SAFE, **leaf** (`callees_depth1: []`, inline copy loops, no calls), so
  the callee-half of the gate is exempt. Two params, both correctly typed as pointers and
  both literally dereferenced (param_1 at +0x24/+0x2c/+0x4c, param_2 at +0x4c). Returns
  `int` 1. **`ptr_seed_observe` matches with no new handler and no additive field**
  (`ARG_TYPES.md:1597`). Caller-gate satisfied by owner **`0x005515a0` C2** (iter21).
  **NON-DEGENERACY — read before seeding:** it returns the constant 1 on every path, so the
  return value is *not* a discriminator. Evidence must come from the destination buffer, and
  the seed must differ per vector at +0x24/+0x2c/+0x4c, or a port that copies nothing still
  scores GREEN.

Batch it with one or two of these (all need one additive field first, none need a new handler):

- **`0x0052ddc0`** — SAFE leaf, same owner `0x005515a0` C2. Needs `count_header_list_ring`
  (`ARG_TYPES.md:55`) **+ a `count` list_op**. Three alternatives were named and refuted by
  MECHANISM: `audio_list_count` (next@+4, wrong layout), `ptr_arg_int_get` (fills a dword
  pattern — a random buffer is not a valid ring list, so it AVs or spins), `thunk_list_count`
  (list at p+0xc, next at node+4, no stride descriptor).
- **`0x0049ff30`** — SAFE (every write relative to param_1: `*(undefined1*)(param_1+0x95)=1`;
  the Enter/LeaveCriticalSection pair uses the CS *inside the passed struct* at +0x14, not
  live engine state). Needs `struct_call_observe` (`ARG_TYPES.md:1490`) **+ a `cs_init_at`
  additive field** (pre-call `InitializeCriticalSection`, exactly analogous to `stub_at`).
  **This supersedes the mutator lane for this row**: the synthetic lane doesn't care how
  often it is called naturally, so `cs_init_at` sidesteps `mutator_ab_pilot`'s open
  call-frequency question entirely. Prefer it.

Pre-flight (`py -3.12 scripts/orch_preflight.py <hook>...`), then verify all of them in
**ONE** `state_batch` boot.

---

## The finding: the s6 brief could never say READY

**24 fresh RVAs, 4 buckets, 4/4 units OK, $2.78 off-quota — and 24/24 came back
`NEEDS_GHIDRA`. In 22 of 24 the deciding column was `CALLERS_NEEDS_GHIDRA`.**

The READY criterion requires "≥1 caller is C2+". The prompt hands the worker **only** plate
paths plus `ARG_TYPES.md`, and explicitly forbids globbing, grepping `hooks.csv`, or
searching `re/analysis`. **Plates do not name their callers.** The one fact READY depends on
is the one fact the worker is structurally denied — the verdict is decided before the screen
runs.

This retro-explains **all 8 prior s6 briefs** tallying 0 READY, which iters 9–11 recorded as
if the candidate pool were dry. It is a prompt defect, not a fact about the candidates.

**Fix (ledger item `brief_caller_gate_fix`, one prompt change, not a new lane):** resolve
callers + caller confidence orchestrator-side — the same way plate paths already are — and
put them **in the table**. That is one `hooks.csv`/Ghidra lookup by the orchestrator, not 24
read-fleet globs, so it does not re-open the read-only boundary the prompt exists to protect.

**Second, smaller fix:** filter the batch against `hooks.csv` **before** building the queue.
`0x00407550` was screened at C2 while already **C3** (`SubsystemARecordFind`, promoted
iter21), wasting a row. (It did independently re-derive the shipped config —
`esi_global_search`, tgt `0x639d80`, stride `0xec`, `key_off 0x44` — a clean incidental
corroboration of that port.)

**Keep verbatim: the mandatory-refutation clause.** iter22 added "before writing
NEEDS_NEW_HANDLER, state which MECHANISM lines you considered and why each fails; if the gap
is a small knob, call it ADDITIVE_FIELD instead." Result: **0 NEEDS_NEW_HANDLER in 23 of 24
rows**, versus six consecutive prior runs that each invented a handler that already existed.
The sole exception (`0x0049d240`) named and refuted four handlers by mechanism —
`fastcall_reg`, `thiscall_nested_field_get`, `reg_this_call_observe`, `vtable_table_dispatch`
— before claiming a gap (ECX-as-this with a 2-level deref *and* a dynamic vtable stub).
Still confirm against the implementation before writing it.

## Two stale ledger notes reconciled (iter22 cycles 2–3)

1. **`handler_specs_22` was stale → closed as `promoted`.** Its two "AUTHORABLE NOW" rows
   were in fact authored *and* promoted in iter21 and are C3 today: `0x004b6b00`
   `StoreEaxAtEcx` (GREEN 5/5) and `0x00407550` `SubsystemARecordFind` (GREEN 4/4). The
   iter21 handoff named only `0x00407580` + `0x005b1160` and so **undercounted its own
   output** — which is exactly why this run opened believing the ladder was dry.
   **Lesson: the ledger, not the handoff prose, is the source of truth. Reconcile briefed
   notes against `hooks.csv` before declaring anything dry.**
2. **`mutator_ab_pilot`'s "NEXT" was superseded.** It said "screen the remaining 12
   WRITES_GLOBAL rows"; iter14 already screened **all 44** and the answer is **2**, not 12
   (AB_READY 2 / NONDET 7 / UNENUMERABLE 11 / ONESHOT 4 / IRREVERSIBLE 20). Survivors:
   `0x0049ff30` and `0x0045c820`. The only remaining gate is a **runtime** measurement — one
   `MASHED_COUNT_RVAS` boot — not more screening. There is no existing evidence for it:
   `re/analysis/plans/ab_reachable.tsv` is a *static* write-surface survey (columns
   rva / resolved-store-count / call-tree-size, per `scripts/survey_ab_reach.py:79-81`), not
   a call counter. Prefer the `cs_init_at` synthetic route for `0x0049ff30` instead.

## Recurring defect classes confirmed again

- **Pointer param declared `int`**: 6 of 24 this run (`0x00451cc0`, `0x00451730`,
  `0x004722e0`, `0x0049f2e0`, `0x0048fef0`, `0x0049ff30`). Reliable recurrence, not anecdote.
- **Plate-vs-`hooks.csv` confidence conflicts**: 4 genuine ones in `state_util_b1_s6`
  (`0x00495110`, `0x004af32d`, `0x004292d0`, `0x0045d430` — plate C1, table C2). No winner
  picked; route via `re-classify` before any promotion.

## Do NOT re-pick

- `0x00495110` — QPC timer via `FUN_004950b0`; rejected in iter12, re-confirmed twice since.
- The 6 heap-address writers (iter14) — allocation addresses differ per run.
- The 14 NO_OWNER ORPHAN rows via reference-chain BFS — it ran out of edges on them.
- `0x0048fce0` / `0x0048fd10` — both DESTROYS_DEVICE via the same
  `(**(code**)(DAT_007d3ff8+0x20))(8,0)` disabling RW renderstate 8.

## Standing rules (unchanged, all measured)

1. Pre-flight before every boot: `py -3.12 scripts/orch_preflight.py <hook>...`.
2. Author 3–4 rows, verify in ONE `state_batch` boot.
3. `NEEDS_NEW_HANDLER` is a hypothesis, not a fact — check `ARG_TYPES.md` MECHANISM lines,
   prefer an additive defaulted field (`stub_at`, `null_args`, `this_reg:'stack'`, `key_off`,
   `eax_from_test`, `reseed_per_side`).
4. A decompiler summary is not evidence about x87. Read the raw listing.
5. A "absent" harness reading has two causes — absent, or a broken probe — and they look
   identical. Make probes report *why* they failed.
6. `out3_idx` is retired; the harness throws if anything references it.

## Hygiene

- Pool slots **5 and 10** hold leaked `.lock~` (clears only when the MCP JVM restarts).
  **Slots 9 and 11 work.** Slot 4 is a stale broken clone.
- Open the root-level `mashed_pool/Mashed_poolN.gpr` only; the `Mashed_poolN/` subdirectory
  projects are stale 0-byte duplicates.
- iter22 spawned **no** MASHED process and created no worktrees; `original/` untouched.
- Another session is active in this checkout — leave `scenario_launch.py`,
  `PromoLoop_sessionB.cpp`, `UNCERTAINTIES.md`, and
  `re/analysis/bucket_audio_005ab710_005af040/0x005ab980.md` alone.
- **Account2 policy v29 (2026-07-31 15:59)**: adds `allow_routines=False`. No impact on the
  read-fleet — it only disables scheduled/cloud routines, alongside the already-disabled
  Remote Control and Workflows. Read/Grep/Glob via `delegate.ps1` is unaffected.
