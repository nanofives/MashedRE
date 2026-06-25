export const meta = {
  name: 'promote-pipeline',
  description: 'Autonomous C2->C3 promotion pipeline: plan -> execute -> collect/merge, one round per loop iteration',
  whenToUse: 'Drain the C2->C3 promotion frontier unattended. Each round plans a batch from the runnable-today supply, fans out worktree-isolated worker agents to author+diff each candidate, then (if autoMerge) merges the GREEN branches to main behind an integration-diff gate.',
  phases: [
    { title: 'Preflight', detail: 'anchor + shim + clean baseline build' },
    { title: 'Plan', detail: 'recon scripts + pipeline_plan.py -> round plan' },
    { title: 'Execute', detail: 'one worktree worker per candidate: author + diff' },
    { title: 'Collect', detail: 'merge GREEN branches behind integration-diff gate; update ledger' },
  ],
}

// ---- args (all optional) ----------------------------------------------------
const A = args || {}
const ROUNDS = A.rounds || 1            // how many plan->execute->collect rounds to run back-to-back
const SESSIONS = A.sessions || 6        // worker sessions per round
const PER_SESSION = A.perSession || 5   // candidates per session
const AUTO_MERGE = A.autoMerge !== false // user chose fully-autonomous: default ON
const DRY_EXECUTE = A.dryExecute === true // workers author+diff+queue but never merge (validation mode)
const MODEL = A.model || 'claude-opus-4-8[1m]'

// ---- schemas ----------------------------------------------------------------
const PREFLIGHT_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    ok: { type: 'boolean' },
    anchor_ok: { type: 'boolean' },
    shim_present: { type: 'boolean' },
    build_clean: { type: 'boolean' },
    reasons: { type: 'array', items: { type: 'string' } },
  },
  required: ['ok', 'anchor_ok', 'shim_present', 'build_clean', 'reasons'],
}

const PLAN_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    round: { type: 'number' },
    plan_path: { type: 'string' },
    supply: { type: 'object', additionalProperties: true },
    argtype_top_gap: { type: 'string' },
    argtype_top_unblock: { type: 'number' },
    candidates: {
      type: 'array',
      items: {
        type: 'object', additionalProperties: false,
        properties: {
          rva: { type: 'string' }, name: { type: 'string' }, sub: { type: 'string' },
          arg_type: { type: 'string' }, score: { type: 'number' }, session: { type: 'number' },
        },
        required: ['rva', 'name', 'sub', 'arg_type', 'session'],
      },
    },
  },
  required: ['round', 'plan_path', 'candidates'],
}

const VERDICT_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    rva: { type: 'string' },
    status: { type: 'string', enum: ['promoted', 'queued', 'failed'] },
    diff: { type: 'string', enum: ['GREEN', 'RED', 'NONE'] },
    branch: { type: 'string' },
    arg_type: { type: 'string' },
    evidence: { type: 'string' },
    reason: { type: 'string' },
  },
  required: ['rva', 'status', 'diff', 'reason'],
}

const COLLECT_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    merged_branches: { type: 'array', items: { type: 'string' } },
    integration_diff: { type: 'string', enum: ['GREEN', 'RED', 'SKIPPED'] },
    promoted_landed: { type: 'number' },
    aborted: { type: 'boolean' },
    ledger_appended: { type: 'boolean' },
    notes: { type: 'string' },
  },
  required: ['integration_diff', 'promoted_landed', 'aborted', 'notes'],
}

// ---- worker prompt builder --------------------------------------------------
// Encodes the existing promote-c3-batch worker protocol: worktree-isolated,
// author+verify-only (NO per-worker re-classify — the Collect tier classifies
// centrally on main), confirmed-arg_type-ONLY (never invent an arg_type), and
// append the result row to re/PROMOTION_QUEUE.md.
function workerPrompt(c) {
  return [
    `You are a C2->C3 promotion worker in an isolated git worktree. Promote ONE function to C3.`,
    `TARGET: ${c.rva}  ${c.name}  (subsystem: ${c.sub})  candidate arg_type: ${c.arg_type}`,
    ``,
    `Binary anchor: original/MASHED.exe SHA-256 must be BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`,
    `(the .unpatched backup holds the anchor; the live exe is patched — that is expected).`,
    ``,
    `REJECT FAST (return status=queued, do NOT author) if any holds — these never diff clean:`,
    `  * the target is a thunk / 5-byte JMP rel32 trampoline (esp. into an already-C3 target): no new behavior.`,
    `  * the function is a destructive teardown/free/kill/release/destroy/close (frees live heap => timeout).`,
    `  * it takes no args and returns void (nothing to feed or observe in the A/B harness).`,
    `  * it touches COM/D3D/DSound/DInput live state.`,
    `PROTOCOL (gta-reversed style, NO-GUESSING, cite every RVA):`,
    `1. Read the function's decompilation (Ghidra MCP or re/console/xtwin.py) + any analysis note. Confirm the`,
    `   signature (conv/nargs/ret). If the real signature does NOT match an arg_type that already exists in`,
    `   re/frida/diff_template.js, DO NOT invent one — return status=queued, reason="needs arg_type <shape>".`,
    `2. Author the hook: one file mashedmod/src/mashed_re/<Subsystem>/<Name>.cpp, inline // 0xRVA comment,`,
    `   RH_ScopedInstall in InjectHooks, register in re/frida/hooks_registry.py, add to build.bat. Runtime-toggleable.`,
    `3. Build: cmd /c mashedmod\\build.bat  (must compile clean).`,
    `4. Diff: py -3.12 re/frida/run_diff.py <hook_name>  (acquires its own frida_pool slot; bit-identity A/B).`,
    `   Status=promoted ONLY if run_diff.py actually produced a NON-DEGENERATE GREEN CSV (real A and B rows,`,
    `   bit-identical). A static "it's obviously identical" argument is NOT acceptance — you must have a CSV.`,
    `   If run_diff TIMES OUT or crashes (common for state-touching fns), status=queued, reason="diff timeout/crash".`,
    `   RED or non-identical => status=queued (cite the divergence).`,
    `5. Commit to THIS worktree branch (path-scoped git commit -- of only your files). Append ONE row to`,
    `   re/PROMOTION_QUEUE.md under "## Queued": branch, rva, hook_name, arg_type, evidence (diff CSV path).`,
    `   DO NOT run re-classify and DO NOT touch hooks.csv — the Collect tier does that centrally.`,
    `6. program_close any Ghidra project you opened (lock hygiene). Kill ONLY MASHED PIDs you spawned.`,
    ``,
    `Return the verdict object. evidence = the diff CSV path or the decomp citation that blocked it.`,
  ].join('\n')
}

// ---- one round --------------------------------------------------------------
async function runRound(roundIdx) {
  // PHASE 0 — Preflight (cheap guards before anything spawns MASHED)
  phase('Preflight')
  const pf = await agent(
    [
      `Preflight the Mashed promotion pipeline. Run and report (NO promotions):`,
      `1. sha256sum original/MASHED.exe.unpatched and confirm == BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (anchor_ok).`,
      `2. Confirm original/d3d9.dll exists (the windowed-mode shim; shim_present). If missing, that's a blocker.`,
      `3. cmd /c mashedmod\\build.bat must compile clean on main (build_clean). Capture tail on failure.`,
      `ok = anchor_ok && shim_present && build_clean. List every failing reason.`,
    ].join('\n'),
    { label: `preflight:r${roundIdx}`, phase: 'Preflight', schema: PREFLIGHT_SCHEMA }
  )
  if (!pf || !pf.ok) {
    log(`round ${roundIdx}: PREFLIGHT FAILED — ${pf ? pf.reasons.join('; ') : 'agent died'}. Skipping round.`)
    return { round: roundIdx, skipped: true, reason: 'preflight', promoted: 0 }
  }

  // PHASE 1 — Plan (deterministic, no MASHED)
  phase('Plan')
  const plan = await agent(
    [
      `Generate the next promotion-round plan. Run, in order:`,
      `  py -3.12 scripts/promote_frontier.py`,
      `  py -3.12 scripts/promote_enrich.py`,
      `  py -3.12 scripts/pipeline_plan.py --sessions ${SESSIONS} --per-session ${PER_SESSION} --model "${MODEL}"`,
      `Then read the emitted re/pipeline/rounds/round_<N>_plan.json (highest N). Return:`,
      `round, plan_path, supply, the #1 argtype_gaps entry as argtype_top_gap/argtype_top_unblock, and the`,
      `flattened candidates (each with its session number). If 0 candidates, return an empty candidates array.`,
    ].join('\n'),
    { label: `plan:r${roundIdx}`, phase: 'Plan', schema: PLAN_SCHEMA }
  )
  if (!plan || !plan.candidates || plan.candidates.length === 0) {
    log(`round ${roundIdx}: PLAN produced 0 candidates (supply drained or all argtype-blocked). Stopping.`)
    return { round: roundIdx, drained: true, promoted: 0,
             argtype_gap: plan && plan.argtype_top_gap }
  }
  log(`round ${roundIdx}: planned ${plan.candidates.length} candidates. ` +
      `Top arg_type gap "${plan.argtype_top_gap}" would unblock ${plan.argtype_top_unblock}.`)

  // PHASE 2 — Execute (worktree-isolated workers; concurrency capped by the
  // runtime + frida_pool). pipeline() so each worker's diff runs as soon as its
  // authoring is done — no barrier.
  phase('Execute')
  const verdicts = (await parallel(
    plan.candidates.map((c) => () =>
      agent(workerPrompt(c), {
        label: `promote:${c.rva}`,
        phase: 'Execute',
        schema: VERDICT_SCHEMA,
        isolation: 'worktree',
        model: MODEL,
      })
    )
  )).filter(Boolean)

  const promoted = verdicts.filter((v) => v.status === 'promoted' && v.diff === 'GREEN')
  const queued = verdicts.filter((v) => v.status === 'queued')
  const failed = verdicts.filter((v) => v.status === 'failed')
  log(`round ${roundIdx}: ${promoted.length} GREEN, ${queued.length} queued, ${failed.length} failed ` +
      `of ${verdicts.length} attempted.`)

  // PHASE 3 — Collect (serial; the only step that touches main)
  phase('Collect')
  if (DRY_EXECUTE || !AUTO_MERGE) {
    log(`round ${roundIdx}: dry/no-merge mode — left ${promoted.length} GREEN branches in PROMOTION_QUEUE for manual frida-sweep.`)
    return { round: roundIdx, promoted: promoted.length, queued: queued.length,
             failed: failed.length, merged: 0, argtype_gap: plan.argtype_top_gap,
             argtype_unblock: plan.argtype_top_unblock }
  }
  if (promoted.length === 0) {
    log(`round ${roundIdx}: nothing GREEN to merge.`)
    return { round: roundIdx, promoted: 0, queued: queued.length, failed: failed.length, merged: 0 }
  }
  const branches = promoted.map((v) => v.branch).filter(Boolean)
  const collect = await agent(
    [
      `Run the Frida-sweep MERGE for promotion round ${roundIdx}. This is the ONLY tier that writes main.`,
      `GREEN branches to land (verdict=promoted, diff=GREEN): ${branches.join(', ') || '(see PROMOTION_QUEUE Queued rows)'}`,
      ``,
      `Follow the frida-sweep skill protocol EXACTLY:`,
      `1. git pull --rebase; re-confirm anchor; cmd /c mashedmod\\build.bat baseline must be clean.`,
      `2. scripts/frida_pool.sh cleanup.`,
      `3. For each GREEN branch: git merge --no-ff. Resolve ONLY the 3 known conflicts (build.bat,`,
      `   hooks_registry.py, CHANGELOG.md) as a union of additions. ANY other conflict => abort that branch,`,
      `   leave its row Queued, continue.`,
      `4. Rebuild canonical mashed_re_dev.asi (cmd /c mashedmod\\build.bat). If build breaks => HARD ABORT:`,
      `   git reset --hard to the pre-round main, return aborted=true. NEVER leave a broken build on main.`,
      `5. INTEGRATION-DIFF GATE: py -3.12 re/frida/run_diff_parallel.py against EVERY landed hook.`,
      `   If the integration diff is not GREEN for all => HARD ABORT (git reset --hard to pre-round main),`,
      `   return integration_diff=RED, aborted=true. A red integration diff must NOT survive on main.`,
      `6. Only if integration GREEN: re-classify each landed hook C2->C3 centrally (updates hooks.csv,`,
      `   STUBS/UNCERTAINTIES, CHANGELOG), move its PROMOTION_QUEUE row Queued->Merged, commit.`,
      `7. Append ONE ledger row to re/pipeline/LEDGER.tsv (tab-separated):`,
      `   round\\t<N>\\tlanded\\t<count>\\tqueued\\t<count>\\tfailed\\t<count>\\tintegration\\t<GREEN|RED>\\tbranches\\t<csv>`,
      ``,
      `Return the collect summary. Kill ONLY MASHED PIDs you spawned.`,
    ].join('\n'),
    { label: `collect:r${roundIdx}`, phase: 'Collect', schema: COLLECT_SCHEMA }
  )
  const landed = collect && !collect.aborted ? collect.promoted_landed : 0
  if (collect && collect.aborted) {
    log(`round ${roundIdx}: COLLECT ABORTED (${collect.integration_diff}). main left at pre-round state. ${collect.notes}`)
  }
  return {
    round: roundIdx, promoted: promoted.length, queued: queued.length, failed: failed.length,
    merged: landed, integration: collect && collect.integration_diff,
    aborted: collect && collect.aborted, argtype_gap: plan.argtype_top_gap,
    argtype_unblock: plan.argtype_top_unblock,
  }
}

// ---- driver -----------------------------------------------------------------
const summaries = []
for (let r = 1; r <= ROUNDS; r++) {
  const s = await runRound(r)
  summaries.push(s)
  if (s.skipped || s.drained || s.aborted) {
    log(`stopping the loop after round ${r}: ${s.skipped ? 'preflight' : s.drained ? 'supply drained' : 'merge aborted'}.`)
    break
  }
  // Budget backstop: stop if the token target is nearly spent.
  if (budget.total && budget.remaining() < 80_000) {
    log(`stopping: token budget remaining ${Math.round(budget.remaining() / 1000)}k < 80k.`)
    break
  }
}

const totalMerged = summaries.reduce((a, s) => a + (s.merged || 0), 0)
const totalQueued = summaries.reduce((a, s) => a + (s.queued || 0), 0)
log(`PIPELINE DONE: ${summaries.length} round(s), ${totalMerged} promoted to C3 on main, ${totalQueued} queued.`)
return { rounds: summaries.length, total_merged: totalMerged, total_queued: totalQueued, summaries }
