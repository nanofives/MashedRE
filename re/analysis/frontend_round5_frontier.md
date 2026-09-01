# Frontend area-loop round 5 — frontier characterization (2026-09-01)

Base: race/first-frame-parity tip. Pool slot Mashed_pool14. Decoded the next 10
undecoded doc-only C2 frontend rows (r2 already covered the top 8) via
`DecompBatch.java` headless against Mashed_pool14:

`0042f400, 00430670, 00431b80, 004322c0, 00432450, 004324a0, 00432800, 004332a0, 0043d2a0, 0043df00`

## Finding: synthetic cheap-win frontier is exhausted (confirms r2 at depth)

All 10 are `undefined FUN(void)` in Ghidra. By CALL count:
- 9 of 10 are multi-CALL void side-effecting controllers (0042f400=2, 00430670=1,
  004322c0=4, 00432450=1, 004324a0=9, 00432800=8, 004332a0=7, 0043d2a0=24,
  0043df00=7) — the r2 shape (void, GPU/state side effects, unverified callees).
- 1 (00431b80) is a pure integer LEAF (0 CALLs).

Every row reads/writes the **frontend-state global block** at roughly
`0x0067ea94..0x0067ecf4` (menu/player-slot state), and the interesting paths branch
into **live-state callees** (`FUN_00413f90` table getter, `FUN_0042c1a0`,
`FUN_0042bfb0`). So a hook-bypassed synthetic path1 on unpopulated globals is a
degenerate green ([[scratch-field-false-green]]) unless the exact global footprint
is seeded — and several paths cannot be seeded synthetically at all (they deref a
live table pointer returned by `FUN_00413f90`).

## The two least-blocked candidates (teed up, footprint enumerated)

### 0x00430670 — MenuOrdinalToPlayerSlot (scalar int return, cleanest)
- `int __cdecl FUN_00430670(int param_1)` (ordinal 1..3 -> player slot idx or -1).
  param_1 at `[ESP+0x1c]`->ESI (listing 0x430693). Body 0x430670..0x430754 (226 B).
- **Clean path** (`DAT_0067e9fc == 10`, listing 0x43067c/0x430691): pure global
  reads, NO callee. Loops ECX=0..3 counting non-zero `[ECX*4+0x0067ea98]`
  (0x4306a4); when running count EDX==param_1 returns `[ECX*4+0x0067ea94] - 1`
  (0x4306be/0x4306c6); else returns -1.
  - **Global footprint to seed:** `DAT_0067e9fc` (=10) + dword window
    `0x0067ea94..0x0067eaa8` (covers `0067ea94[0..4]` and `0067ea98[0..3]`).
  - **Non-degenerate vectors:** fix e.g. `0067ea98[0..3]={5,0,7,3}` (mix zero/nonzero),
    `DAT_0067e9fc=10`, vary param_1 in {1,2,3,4}: 1/2/3 return distinct
    `0067ea94[idx]-1`, 4 falls through to -1.
- **Else path** (`!=10`): CALLs `FUN_00413f90` (0x4306d5) then derefs
  `FUN_00413f90() + DAT_0067f17c*0x30 (+0x10 if ==4, +0x20 if ==5)` and a table
  loop against `DAT_007f1a1c`. **Needs a booted race** — live table pointer.

### 0x00431b80 — pure integer leaf (0 CALLs), but non-standard register conv
- Reads/mutates one element `[EAX*4+0x0067ea98]` (EAX=index), increment ESI
  (`unaff_ESI`), stack param `[ESP+8]`. Compares against `DAT_007f1a1c`,
  `DAT_0067ea9c`, `DAT_0067eaa0`. Body 0x431b80..0x431cf1.
- Diffable only with a **register-arg + global-seed + observe-one-global** arg_type
  (EAX/ESI register delivery like `eax_ptr_ebx_outbuf`'s trampoline). New handler.

## Harness gap that blocks BOTH (report to parent)

Neither existing arg_type fits a **scalar-return function gated on read-only
globals**:
- `cache_setter_observe` seeds per-test globals (`input.seed=[{addr,val}]`) but
  observes GLOBALS, not the return value.
- `scalars_to_scattered_globals` folds the return (`fold_ret`) but seeds only via
  `pre_fill_byte` / a prep call — it cannot set `DAT_0067e9fc=10` + specific array
  dwords.

**Missing:** a `seed_globals + fold_ret` arg_type = per-test `seed=[{addr,val}]`
(snapshot+restore) + pass `args` to fn + compare the RETURN value. This is a small
additive delta on `cache_setter_observe` (add `fold_ret`, drop the obligatory
global observe), and it would unblock 00430670's clean path plus the whole
scalar-return-gated-on-globals frontend cluster. SWEEP-CRITICAL when authored
(handler lives in diff_template.js + verify_hook_install_template.js).

No unverifiable reimpl authored into the tree (respects the acceptance bar; see the
track area's same discipline). Round 5 is a characterization round: 0 C3 landed,
next win (00430670 clean path) is one `seed_globals+fold_ret` arg_type away.

## Round 7 addendum (2026-09-01) — cluster hypothesis REFUTED

After landing 0x00430670 (r6) with the new `seed_globals_fold_ret` arg_type, the parent
asked whether the handler unlocks a CLUSTER of the ~59 remaining rows. Decoded 12 more
frontend-range doc-only rows (DecompBatch/pool14):
`00422470, 004224d0, 00424270, 00424b80, 00425b90, 00425bf0, 00425c00, 00425ca0, 00426d20, 00426d90, 004273e0, 004274e0`.

**Result: 0 of 12 read the frontend-state block (0x0067ea9x/0x0067ecx); all are void
side-effecting.** So `seed_globals_fold_ret` unlocked exactly ONE row — 0x00430670 was a
singleton scalar-return getter, not a cluster head. Combined with r2 (8) + r5 (10), that
is **30 of 76 doc-only rows decoded**, and the pattern is decisive: the frontend residue
is void setters / cleanup loops / RW wrappers / file-I/O one-liners, plus rare scalar
getters. The CHEAP synthetic scalar-return frontier is exhausted.

**Remaining shape is block-write setters (medium effort, per-row, NOT a cluster).**
Worked example, teed up: `0x004224d0` `void(int slot, u32* p2, u32 p3)` zero-fills a
0xf40 slot at `&DAT_006403e8 + slot*0xf40` (callee `FUN_004b6520` ZeroFillWrapper **C3**)
then writes ~15 fields (index, p2[0..3], constants 0x3f000000/0xbfe66666/0x3f800000/
0xbf59999a, DAT_005f6154, p3). Diffable via the EXISTING `cache_setter_observe` (args
[slot, null->buf, p3], observe the written slot offsets) — no new arg_type. But it is a
verbatim ~15-store reimpl + a buffer arg + a scattered-observe list per row, i.e. medium
effort each, not a cheap sweep. Verdict: frontend is WORKABLE but no longer CHEAP; the
per-row block-observe lane is the same acceptance shape ai/track reached.
