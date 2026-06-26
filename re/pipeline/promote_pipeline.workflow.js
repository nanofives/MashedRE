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

// ---- config -----------------------------------------------------------------
// NOTE: the Workflow `args` global did NOT propagate to the script in this harness
// (run wf_980e3d3a ignored {rounds:8,model:sonnet} -> ran 1 Opus round). So the
// DEFAULTS below are the real control surface — edit them and re-invoke with
// {scriptPath}. args still override IF present.
const A = args || {}
const ROUNDS = A.rounds || 8             // plan->execute->collect rounds back-to-back
const CLUSTER_MAX = A.clusterMax || 6    // max functions per cluster/worker
const SESSIONS = A.sessions || 6         // (legacy) candidate-count target = SESSIONS*PER_SESSION
const PER_SESSION = A.perSession || 5
const AUTO_MERGE = A.autoMerge !== false // fully-autonomous: default ON
const DRY_EXECUTE = A.dryExecute === true // author+diff+queue but never merge (validation mode)
const MODEL = A.model || 'claude-sonnet-4-6'  // worker (Execute) model; orchestration inherits Opus

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
    clusters: {
      type: 'array',
      items: {
        type: 'object', additionalProperties: false,
        properties: {
          cluster_id: { type: 'number' },
          kind: { type: 'string' },
          subsystem: { type: 'string' },
          members: {
            type: 'array',
            items: {
              type: 'object', additionalProperties: false,
              properties: {
                rva: { type: 'string' }, name: { type: 'string' }, sub: { type: 'string' },
                arg_type: { type: 'string' }, size: { type: 'string' },
              },
              required: ['rva', 'name', 'sub', 'arg_type'],
            },
          },
        },
        required: ['cluster_id', 'members'],
      },
    },
  },
  required: ['round', 'plan_path', 'clusters'],
}

const CLUSTER_VERDICT_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    cluster_id: { type: 'number' },
    branch: { type: 'string' },
    verdicts: { type: 'array', items: VERDICT_SCHEMA_INNER() },
  },
  required: ['verdicts'],
}
function VERDICT_SCHEMA_INNER() {
  return {
    type: 'object', additionalProperties: false,
    properties: {
      rva: { type: 'string' },
      status: { type: 'string', enum: ['promoted', 'queued', 'failed'] },
      diff: { type: 'string', enum: ['GREEN', 'RED', 'NONE'] },
      arg_type: { type: 'string' },
      evidence: { type: 'string' },
      reason: { type: 'string' },
    },
    required: ['rva', 'status', 'diff', 'reason'],
  }
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

// ---- cluster worker prompt --------------------------------------------------
// COST REDESIGN (2026-06-26): one worker per CLUSTER of related functions, not
// one per function. Amortizes context-boot + ONE build + shared struct/global
// understanding across the whole family. Workers get ground-truth asm from the
// cheap capstone helper (scripts/dump_asm.py) — NO Ghidra session (that was ~25k
// tokens/worker of the old cost). author+verify-only; the Collect tier classifies
// centrally. Old per-function cost ~80k; target ~90k for a whole cluster of ~5.
function clusterWorkerPrompt(cl) {
  const rows = cl.members.map(m => `${m.rva}:${m.size || ''}  ${m.name}  arg_type=${m.arg_type}`)
  const asmToks = cl.members.map(m => `${m.rva}${m.size ? ':' + m.size : ''}`).join(' ')
  return [
    `You are a C2->C3 promotion CLUSTER worker in an isolated git worktree. Promote a FAMILY of related`,
    `functions (kind=${cl.kind || '?'}, subsystem=${cl.subsystem || '?'}). Author them as ONE coherent unit.`,
    `MEMBERS (${cl.members.length}):`,
    ...rows.map(r => `  - ${r}`),
    ``,
    `Anchor: original/MASHED.exe.unpatched SHA-256 = BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`,
    `(live MASHED.exe is patched — expected).`,
    ``,
    `STEP 0 — get ground-truth asm for the WHOLE cluster in one cheap call (NO Ghidra, NO MCP):`,
    `    py -3.12 scripts/dump_asm.py ${asmToks}`,
    `  Author every reimpl FROM THIS ASM. Do NOT open Ghidra/xtwin unless the asm is genuinely ambiguous.`,
    `  Because these are address-adjacent, they likely share a struct layout / globals — figure it out ONCE`,
    `  and reuse across the cluster (that shared understanding is why we cluster).`,
    ``,
    `For EACH member, REJECT FAST (status=queued, don't author) if: it's a thunk/5-byte JMP; a destructive`,
    `teardown/free/kill; 0-arg void; or touches COM/D3D/DSound/DInput live state. If its true signature`,
    `doesn't match an EXISTING arg_type in re/frida/diff_template.js, status=queued reason="needs arg_type <shape>"`,
    `(NEVER invent an arg_type — integration RED's on invented ones).`,
    ``,
    `PROTOCOL (gta-reversed, NO-GUESSING, cite the RVA + asm offset for every field):`,
    `1. Author each member's hook: mashedmod/src/mashed_re/<Subsystem>/<Name>.cpp (inline // 0xRVA),`,
    `   RH_ScopedInstall in InjectHooks, register in re/frida/hooks_registry.py, add to asi_sources.rsp/build.bat.`,
    `   Prefer ONE .cpp for the whole family when they share a struct (fewer files, shared header).`,
    `2. Build ONCE for the whole cluster: cmd /c mashedmod\\build.bat (must compile clean). If one member`,
    `   breaks the build, comment out just that member's install + registry entry (queue it) and rebuild.`,
    `3. Diff each member: py -3.12 re/frida/run_diff.py <hook_name>. promoted ONLY on a NON-DEGENERATE GREEN`,
    `   CSV (real A/B rows, bit-identical) — a static "obviously identical" argument is NOT acceptance.`,
    `   rc=3 timeout / rc=4 "terminated during injection" are TRANSIENT Frida flakes — retry up to 3x before`,
    `   giving up; if still failing, status=queued reason="diff flake/timeout".`,
    `4. UNWIRE every non-promoted member before committing: comment out its RH_ScopedInstall + its`,
    `   hooks_registry.py entry + its asi_sources.rsp/build.bat line, so the branch INSTALLS ONLY`,
    `   diff-verified hooks (a queued/RED hook that rides onto main would change behavior unverified).`,
    `   Rebuild once more to confirm the verified-only set compiles. Then path-scoped git commit -- your files.`,
    `   For each PROMOTED member append a row to re/PROMOTION_QUEUE.md "## Queued": branch, rva, hook_name,`,
    `   arg_type, CSV path. DO NOT re-classify or touch hooks.csv — Collect does that. Kill ONLY MASHED PIDs you spawn.`,
    ``,
    `Return {cluster_id:${cl.cluster_id}, branch:<your worktree branch>, verdicts:[one per member]}.`,
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
      `  py -3.12 scripts/pipeline_plan.py --cluster-max ${CLUSTER_MAX} --model "${MODEL}"`,
      `Then read the emitted re/pipeline/rounds/round_<N>_plan.json (highest N). Its "batches" array IS the`,
      `cluster list (each has cluster_id=session, kind, subsystem, candidates=members). Return:`,
      `round, plan_path, supply, the #1 argtype_gaps entry as argtype_top_gap/argtype_top_unblock, and`,
      `clusters[] (map each batch -> {cluster_id, kind, subsystem, members:[{rva,name,sub,arg_type,size}]}).`,
      `If 0 candidates, return an empty clusters array.`,
    ].join('\n'),
    { label: `plan:r${roundIdx}`, phase: 'Plan', schema: PLAN_SCHEMA }
  )
  if (!plan || !plan.clusters || plan.clusters.length === 0) {
    log(`round ${roundIdx}: PLAN produced 0 clusters (supply drained or all argtype-blocked). Stopping.`)
    return { round: roundIdx, drained: true, promoted: 0,
             argtype_gap: plan && plan.argtype_top_gap }
  }
  const nCand = plan.clusters.reduce((a, c) => a + (c.members ? c.members.length : 0), 0)
  log(`round ${roundIdx}: planned ${plan.clusters.length} clusters / ${nCand} candidates. ` +
      `Top arg_type gap "${plan.argtype_top_gap}" would unblock ${plan.argtype_top_unblock}.`)

  // PHASE 2 — Execute: one worktree-isolated worker per CLUSTER. Each returns an
  // array of per-member verdicts; flatten them. Concurrency capped by runtime +
  // frida_pool. Far fewer, larger workers => boot/build/understanding amortized.
  phase('Execute')
  const clusterResults = (await parallel(
    plan.clusters.map((cl) => () =>
      agent(clusterWorkerPrompt(cl), {
        label: `cluster:${cl.cluster_id}:${cl.subsystem || cl.kind || ''}`,
        phase: 'Execute',
        schema: CLUSTER_VERDICT_SCHEMA,
        isolation: 'worktree',
        model: MODEL,
      })
    )
  )).filter(Boolean)
  // attach the cluster's branch to each verdict (collect needs branch per hook)
  const verdicts = []
  for (const cr of clusterResults)
    for (const v of (cr.verdicts || []))
      verdicts.push({ ...v, branch: cr.branch })

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
  const branches = [...new Set(promoted.map((v) => v.branch).filter(Boolean))]
  const collect = await agent(
    [
      `Run the Frida-sweep MERGE for promotion round ${roundIdx}. This is the ONLY tier that writes main.`,
      `GREEN branches to land (verdict=promoted, diff=GREEN): ${branches.join(', ') || '(see PROMOTION_QUEUE Queued rows)'}`,
      ``,
      `CRITICAL — the integration gate must be PER-HOOK and FLAKE-TOLERANT, NOT abort-the-whole-round.`,
      `A round-2 post-mortem (wf_a3b31069) halted the entire 8-round campaign because ONE hook hit a`,
      `transient Frida attach flake ("process terminated during injection", rc=4). A single bad/flaky hook`,
      `must be DROPPED (left Queued), not allowed to discard the other good promotions or stop the loop.`,
      ``,
      `1. git pull --rebase; re-confirm anchor; cmd /c mashedmod\\build.bat baseline must be clean.`,
      `   If the baseline build is already broken => aborted=true (don't merge onto a broken tree).`,
      `2. scripts/frida_pool.sh cleanup. Record the pre-round main commit (git rev-parse HEAD) for reverts.`,
      `3. Merge each GREEN branch with git merge --no-ff. Resolve ONLY the 3 known conflicts (build.bat,`,
      `   hooks_registry.py, CHANGELOG.md) as a union of additions. Any OTHER conflict => skip that branch`,
      `   (leave Queued), continue. Rebuild canonical .asi (cmd /c mashedmod\\build.bat).`,
      `   If the BUILD breaks and you cannot attribute it to one branch => HARD ABORT (git reset --hard to`,
      `   the pre-round commit), aborted=true. If you CAN attribute it, drop that branch and rebuild.`,
      `4. PER-HOOK INTEGRATION DIFF: py -3.12 re/frida/run_diff_parallel.py over all newly-merged hooks.`,
      `   For EACH hook that is not GREEN:`,
      `     a. RETRY it up to 3x with py -3.12 re/frida/run_diff.py <hook>. Frida spawn/attach is flaky`,
      `        (rc=4 "terminated during injection" / rc=3 timeout are TRANSIENT) — a retry usually passes.`,
      `     b. If GREEN on retry: keep it.`,
      `     c. If still not GREEN after 3 tries: DROP it — revert that hook's merge commit (git revert --no-edit`,
      `        or reset the single merge) so its install is gone from main, leave its PROMOTION_QUEUE row`,
      `        Queued with reason "integration RED/flake (dropped, retry next round)". This is NOT an abort.`,
      `5. After drops, rebuild .asi and run ONE final run_diff_parallel.py over the KEPT hooks.`,
      `   - Final GREEN: re-classify each KEPT hook C2->C3 centrally (hooks.csv, STUBS/UNCERTAINTIES,`,
      `     CHANGELOG), move its queue row Queued->Merged, commit. aborted=false, landed=#kept (may be 0).`,
      `   - HARD ABORT (git reset --hard pre-round, aborted=true) ONLY if: the final diff shows a`,
      `     PREVIOUSLY-ON-MAIN (baseline) hook regressed, or the build is broken and unattributable.`,
      `     Dropping all of THIS round's new hooks with a clean baseline is landed=0, aborted=FALSE.`,
      `6. Append ONE ledger row to re/pipeline/LEDGER.tsv (tab-separated):`,
      `   round\\t<N>\\tlanded\\t<#kept>\\tqueued\\t<#dropped>\\tfailed\\t0\\tintegration\\t<GREEN|RED>\\tbranches\\t<kept csv>`,
      ``,
      `Return the collect summary (promoted_landed=#kept; aborted per the rule above). Kill ONLY MASHED PIDs you spawned.`,
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
