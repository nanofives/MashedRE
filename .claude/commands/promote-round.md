# /promote-round — one self-contained C2→C3 promotion round

Execute ONE promotion round (≤5 candidates), driven entirely by
`re/analysis/PROMOTION_LOOP_LEDGER.md` (the ledger is the only state that
survives between rounds — trust it over conversation memory). Designed to be
run under `/loop /promote-round` until the promotable pool is dry.

## Hard rails (violating any of these ends the loop, not just the round)

- NEVER touch `original/` (no rm/clean/reset on it or its parents).
- Trackers (`hooks.csv`, `UNCERTAINTIES.md`, `STUBS.md`, `DEFERRED.md`) are
  mutated only via the `re-classify` skill / an equivalent scripted
  transaction, once per round, path-scoped atomic commit.
- Commit after every working state (`git commit -- <paths>` with explicit
  paths, NEVER `git add -A` — another session may share this checkout).
- NO-GUESSING: every constant cited; no invented semantics; refuse C3 without
  the full gate (analysis note, reimpl + RH_ScopedInstall, GREEN diff,
  caller AND callee at C2+, no blocking U-rows).
- Verdicts: run_diff exit 0=GREEN, 1=RED, 5=INCONCLUSIVE-DEGENERATE (NOT
  evidence), 6=race-unreachable (retry once). Only GREEN promotes.

## Round procedure

### 0. Gate the round
- If `frida-sweep.WIP-*` or `master.WIP-*` exists, or `cl.exe`/`link.exe` is
  running → another session owns shared state: **skip this round** (log a
  ledger line, reschedule ~20 min).
- `MASHED.exe` running is OK to wait out (~5 min), then proceed (game windows
  are sanctioned at any time — user decision 2026-06-12).
- Anchor: SHA-256 of `original/MASHED.exe.unpatched` must be
  BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E. Mismatch → STOP THE LOOP.
- Baseline `cmd /c mashedmod\build.bat` must pass before any edit. Broken
  baseline → STOP THE LOOP and report (do not retry forever).
- If run_diff fails with device-creation errors (D3DERR_INVALIDCALL +
  ChangeDisplaySettings=-1): no active display output — skip round, tell the
  user to turn a monitor on.

### 1. Read the ledger
`re/analysis/PROMOTION_LOOP_LEDGER.md`: lane queues, done/deferred lists,
dry-round counter, harness-extension wishlist. If the previous round left an
unfinished candidate, finish or defer it first.

### 2. Select ≤5 candidates (first non-empty lane wins; top up from the next)
- **L0 — race1 leftovers**: the 10 RVAs of `c3_batch_race1.txt` are claimable
  if `re/PROMOTION_QUEUE.md` has no Queued/Merged row containing them (the
  human batch was never run). Their arg_types are already confirmed in that
  file — cheapest possible round.
- **L1 — race-lane viable**: `re/analysis/plans/c3_batch_race1_passed.tsv` ∩
  unlocked RVAs (`re/analysis/scenario_attach_probe_2026-06-12.json`), minus
  ledger done/deferred. Read each note; ship only candidates whose recovered
  signature maps to an arg_type that EXISTS in `re/frida/diff_template.js`
  today. Live-state candidates get `'scenario': 'race'`.
- **L2 — cheap re-earns**: regenerate `py -3.12 scripts/c2_gate_audit.py`;
  the `demoted-needs-reimpl` RVA list. Analysis exists; author reimpl +
  standard (or race) diff.
- **L3 — broad confirmed-shape pool**: run
  `py -3.12 re/analysis/plans/c3_filter_v4.py --subsystems <all first-party> --out-prefix loop_round_<n>`
  and pick small (≤100B) candidates whose notes confirm an existing arg_type.
  State-independent shapes (pointer-return getters, pure leaves) may run at
  menu-attach; anything reading `DAT_*` race state gets `scenario: 'race'`.
- **L4 — degenerate-GREEN triage** (evidence repair, not new C3s — only when
  L0–L3 yield nothing): take rows from
  `re/analysis/DEGENERATE_GREEN_AUDIT_2026-06-12.md` residuals, set
  `scenario:'race'` + in-bounds vectors on their existing registry entries,
  re-run, annotate hooks.csv notes (no status change on GREEN; demote per
  CONFIDENCE.md on RED).
- **L5 — harness extension** (at most ONE per round, only when L0–L4 give <3
  candidates AND the ledger wishlist shows one extension unlocking ≥10 rows):
  implement the handler in `diff_template.js`, validate it on ONE hook before
  using it for more. New arg_types are SWEEP-CRITICAL — say so in the commit.

### 3. Author + verify (per candidate)
- Cluster file `mashedmod/src/mashed_re/<Subsystem>/PromoLoop_<roundN>.cpp`;
  RVA-cited headers; `RH_ScopedInstall`. Pointer-returning getters return the
  ORIGINAL-image VA as a constant, never an .asi-local address.
- Registry entry with confirmed arg_type (+ `scenario:'race'` where needed).
  Vector rules: in-bounds indices for dereferencing getters; discretely-
  changing state preferred (A/B time-skew: a single-vector float mismatch →
  re-run once before debugging).
- Build, then `py -3.12 re/frida/run_diff.py <hook>` — SEQUENTIALLY (scenario
  runs drive a race each, ~60–90 s; never run two concurrently).

### 4. Classify + commit
- One re-classify transaction for the round's GREEN set (single-session work
  classifies directly on main — no worktree, no sweep).
- Path-scoped commit: cpp + build.bat + hooks_registry.py in one commit;
  trackers + CHANGELOG in the re-classify commit.

### 5. Update the ledger (always, even on a skipped/dry round)
Append a round row: date, lane(s), attempted, GREEN, deferred(+reasons),
exit-5/6 incidents, dry-counter. Move candidates between queues. Add any
"needs arg_type X (N rows)" finding to the harness-extension wishlist.

### 6. Loop control
- GREEN ≥1 this round → reset dry-counter, continue the loop (next round
  immediately or short delay).
- 0 GREEN → dry-counter +1. **Two consecutive dry rounds → END THE LOOP**:
  write the final report into the ledger (what remains, per gate: COM count,
  ABI-limited, oversize, harness-ext wishlist with unlock counts) and tell
  the user the promotable pool is drained.
- STOP THE LOOP immediately on: anchor mismatch, broken baseline build,
  >2 consecutive exit-6 (drive recipe broken), or any tracker conflict that
  needs human arbitration.
