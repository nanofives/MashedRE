<!-- AREA LOOP -- the recurring per-system-area RE + parity loop. -->
<!-- Built 2026-08-31. Constraints baked in: single account (account2), single machine, -->
<!-- Ghidra via analyzeHeadless CLI (NEVER MCP -- blocked on account2), star-topology fleet. -->

# Area loop

Goal: pick a system area (a canonical `hooks.csv` subsystem), and **keep coming back**
round after round until every function it executes is covered (S-DoD) and it looks
visually faithful (parity not regressed), then move to the next area.

Two roles. The **parent orchestrator** (one session) holds the ledger and the bus and
schedules areas. Each **area child** (one session per subsystem) runs `/area-round` on
its own area, in its own worktree, on its own Ghidra pool slot.

```
parent orchestrator ── owns AREA_LEDGER.md + CROSS_AREA_BUS.md, spawns fleet, runs sweeps
   ├── render child   ── worktree .worktrees/area-render,   pool slot, loops /area-round
   ├── hud child      ── worktree .worktrees/area-hud,      pool slot
   ├── ai child       ── ...
   └── ...            (one child per ACTIVE area; QUEUED areas have no child yet)
```

## Substrate (built, verified 2026-08-31)

| file | role |
|---|---|
| `re/tools/area_residue.py` | area -> ordered candidate queue (`--subsystem`, `--name-match`, `--out`) |
| `re/AREA_LEDGER.md` | per-area state: rounds, 3 coverage columns, dry counter, parity ptr |
| `re/CROSS_AREA_BUS.md` | star-topology cross-area finding bus, parent-owned, B-#### rows |
| `re/parity/recipes.toml` | area -> capture recipe + diff command (documentation, not executed) |
| `re/tools/parity_scoreboard.py` | parse a diff stdout -> committed `scoreboard.tsv` row + delta |

## Coverage math (why three columns, not one)

A `hooks.csv` row can be C3 and be **pure documentation** (its `file` points at
`re/analysis/*.md`). S-DoD needs the function LINKED into `mashed_re.exe` and reached on
the default path. So the ledger tracks `documented >= implemented >= linked`, and the
loop's exit gate reads **linked**, not documented. Baseline measured 2026-08-31: render
is 749 residue but only **2 implemented .cpp** -- render rounds are doc->cpp->link work.

---

## PARENT: orchestrator protocol

**Spawn the fleet.** For each area to run concurrently (start with the pilot alone):
1. Create its worktree + pool slot with the `worktree` skill (binds `.pool_slot`, log dir).
   NEVER `git worktree remove --force` (WORKTREE-SYMLINK-WIPE); teardown via `scripts/diag.py wt-remove`.
2. `mcp__happy__spawn_child` on **this account** (account2), model opus, effort high,
   directory = the worktree, prompt = "You are the `<area>` child. Loop `/area-round <area>`.
   Cross-area findings go to me (parent session id `<id>`), never to the bus directly."
3. Record the child session id in the ledger's area section.

**Own the bus.** When a child reports a cross-area finding (via `send_to_session` to you):
1. Arbitrate (is it real, per NO-GUESSING? cite the RVA/offset).
2. Append a `B-####` row to `re/CROSS_AREA_BUS.md` under `<!-- ENTRIES -->`, status OPEN.
3. `send_to_session` the affected area's child: "bus B-#### affects you: <claim>". Set status PINGED.
4. When that child acks/lands it, update status ACK/LANDED with the resolution + where it landed.
   A struct/global finding LANDED once is read by both areas -- do not carry it as OPEN.

**Schedule areas.** Read the ledger table each cycle. When an area is MINED-OUT or COVERED,
tear down its child + worktree and spawn the next QUEUED area (order: render, hud, ai, track, frontend).

**Run the sweeps** (serialized -- single machine). Between rounds, drain `re/PROMOTION_QUEUE.md`
with `frida-sweep` (merge child branches, rebuild the `.asi`, integration-diff every promoted
hook -- the GREEN gate). `ghidra-sweep` is **dormant** (no MCP master writeback this config);
symbol/comment writeback is deferred, the trackers + `.cpp` are the truth.

**Own the verification lane** (policy fixed 2026-09-01). On account2 the children can characterize,
author, and run *synthetic* path1 diffs, but *booted-race* verification stalls on them (prompts).
So most `ai` / `track` / render-parity C3s can only be AUTHORED by the child, not landed. Children
append those to `re/PROMOTION_QUEUE.md` tagged **`NEEDS-BOOTED-RACE`**; the PARENT drives the race
(`run_diff.py <hook>` with scenario:'race', or the scenario-attach lane) here on account2, one at a
time, approving its own prompts, and promotes on GREEN. Children NEVER boot a race. This keeps the
whole loop on one account and serializes the machine-bound half (which must be serial anyway).

**Base consistency** (GAP-6, learned 2026-09-01). Every child worktree MUST branch from the commit
that carries the area-loop infra (currently on `race/first-frame-parity`, not `main`). Two children
defaulted to `main`, lacked `area_residue.py`/recipes/ledger, and had to `git merge
race/first-frame-parity` to converge. When spawning, pin the base ref explicitly, or land the infra
on `main` first.

**Stop rule.** All areas COVERED or MINED-OUT => loop done. Emit a paste-ready resume kickoff.

---

## CHILD: one `/area-round <area>` iteration

Preflight (the gate) -- abort the round if any fails:
- `git status` clean-ish in your worktree; you hold your own `.pool_slot`.
- exe anchor SHA matches (`original/MASHED.exe` per CLAUDE.md version anchor).
- baseline `mashedmod\build.bat` succeeds before you touch anything.
- multi-session preflight (`scripts/ghidra_assert.sh preflight`) -- your slot only.

1. **Scope.** `py -3.12 re/tools/area_residue.py --subsystem <area> --out queue.tsv`.
   Take the top **K** (K=5 Sonnet-scale, 10-15 Opus). Cheapest-win-first: implemented+linked
   rows are one Frida diff from C3; doc-only C2 need a `.cpp` authored first. Also read the
   ledger's `<area>` section -- its **open visual defects must be closed or re-filed**, not dropped.

2. **Work each candidate** (serial within the round):
   - Decode: headless decomp via `analyzeHeadless` + `DecompPC.java` against YOUR pool slot
     ([[ghidra-mcp-down-use-analyzeheadless]]). **Never Ghidra MCP** (blocked on account2).
   - Author the hook gta-reversed-style (`hook-author` skill): one file per class, RVA comment
     per function, `RH_ScopedInstall`. Add the `.cpp` to `build.bat` (exe) or `asi_sources.rsp`
     -- unlinked code cannot satisfy S-DoD or be seen on screen.
   - Verify: `build.bat`, then `run_diff.py <hook>` (path1 bit-identity) + `run_verify_hook.py`
     (path2 install). GREEN is the only acceptance -- compiles-and-runs is not.
   - On GREEN: `re-classify` to C3, append a row to `re/PROMOTION_QUEUE.md` for the parent's sweep.

3. **Cross-area finding?** If a candidate exposes a shared struct offset, global, dispatcher,
   or a shared RVA another area reads -> **report to the parent** (`send_to_session` your parent
   id, kind + anchor + one-line claim). Do NOT edit `CROSS_AREA_BUS.md` yourself. Keep working;
   the parent pings the affected child.

4. **Parity** (visual areas only; skip for ai/util). Run the area's `recipes.toml` capture pair
   + diff BY HAND, then `py -3.12 re/tools/parity_scoreboard.py record --recipe <id> --round <N> --from out.txt`.
   A recipe with a `blocker` (e.g. render.race_first_frame_arctic / T-ARCTIC) -> `--blocked`.
   Same-frame basis caveats live in the recipe `reference` and [[race-camera-rolls-30deg-sine]].

5. **Ledger.** Append one round row to the `<area>` section:
   `round | date | candidates | landed C3+ | parity delta | dry? | note`.
   A round is **dry** if it landed 0 new C3+ AND did not improve any parity metric.
   Update the top-table `dry streak`. **2 consecutive dry rounds => state MINED-OUT**, tell the
   parent. Area is **COVERED** only when: residue<C3 for the area is 0 on linked terms, its STUBS
   section is empty, and every parity recipe is GREEN or explicitly BLOCKED (T-ARCTIC).

6. **Loop.** Under `/loop /area-round <area>` this repeats. The parent's scheduler, not the
   child, decides when to stop the area.

## Known blockers (do not paper over)

- **T-ARCTIC** -- no pose-matched original Arctic frame; render can't close Arctic parity until
  `race_draw_burst.py` reaches the mode-3 pose. Recipe records BLOCKED, area can still be COVERED
  on non-Arctic terms with T-ARCTIC cited.
- **Parallel master-Ghidra writes unsafe** -- the `master.WIP-*` flag is branch-local
  (`multi-session` TODO). Irrelevant while `ghidra-sweep` is dormant; if MCP writeback is ever
  re-enabled, the scribe must stay strictly serial.
- **Single machine** -- game lock + build + headless slot serialize the merge half. Fleet
  parallelism is across areas (children), not within a round.
