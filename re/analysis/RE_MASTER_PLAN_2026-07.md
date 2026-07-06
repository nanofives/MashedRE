# Mashed RE — Master Execution Plan (2026-07-03)

Companion to `ROADMAP.md` (v2, phases R0–R8). ROADMAP defines the *gates*; this plan defines the
*route*: current state, recommended sequencing, per-workstream detail, the open decision gates, and a
token-cheap operating model for a solo dev. Numbers are parsed from the trackers 2026-07-03, not
estimated. Re-open this each session (or have the account2 worker summarize it) instead of re-deriving.

> **Status update 2026-07-06 (foundation reset session):**
> - **R6 CLOSED** (exit demo 2026-07-02; literal-criterion residue filed **D-11060/D-11061**).
>   Active phase: **R7 scaffold→verbatim conversion** on this plan's route. ROADMAP + CLAUDE.md updated.
> - §7 queue: item 3 (D-11056) and item 4 (D-11057) **DONE 2026-07-04**; item 5's `__ftol` head
>   **DONE 2026-07-03** (C3, byte-identical). Next up: **T1** (delegation-reach test), the **D1/D5
>   gates**, and WS-PHYS-DRIVE-STABILIZE.
> - Repo foundation: `ws-r6-ai-control-chain` merged to main (3b6b6bd7, build green both targets);
>   branch triage — 80 merged + 39 stale unmerged deleted, all tips preserved in
>   `log/branches_backup_2026-07-06.bundle`, salvage candidates kept (`promote-c4`,
>   `ws-visual-polish`, `harness/ext-ag-arg-types`, `b6/transform`); tracker strike-pass (36
>   stale-RESOLVED rows struck); STUBS.md got a per-subsystem open-stub census (NOTE: the
>   "~980 open stubs are audio" reading from the 2026-07-06 survey was WRONG — audio is only
>   ~90 open rows; render/boot/util/particle dominate, see the census block); stray root pool
>   marker removed.
>   Standing between-slices work now lives in **`re/HARNESS_BACKLOG.md`**.

---

## 1. Where we actually are

**Breadth is done. Depth is the mountain.** Every subsystem *exists* and the full game loop runs
end-to-end on `mashed_re.exe` (R6 exit demo passed 2026-07-02: boot → menu → race vs AI → 7
elimination rounds → match won → results → progression → exit). But most in-race logic is still
**SCAFFOLD** — placeholder behavior, not the 1:1 verbatim port that F/S/P-DoD require.

Confidence snapshot (refreshed 2026-07-06) — **5,864 hook rows**: C1 927 · C2 3,925 · C3 851 · C4 161 (C3+ = 1,012, 17.3%).
Of these, **~2,116 rows are third-party library** (RenderWare, MSVC CRT, Lua, RW-Physics, qhull, D3DX9)
and are *out of scope for porting* by policy — so first-party is ~3,748 rows, and the real "how much
verbatim work remains" number is the first-party sub-C3 pool, not the raw 83%.

### Scaffold-vs-verbatim ledger (the true remaining-work map)
Authoritative source: `re/analysis/SESSION_VERIFICATION_AUDIT_2026-06-16.md`.

| Subsystem | Status | What that means for v1.0 |
|---|---|---|
| Menu / frontend nav | **VERBATIM C3/C4** | Done (R2). 148 C3 / 32 C4; parity-swept. |
| Save / gamesave / unlock | **VERBATIM C4** | Done. 28 C4; real writer wired. |
| Race rule engine (modes/cup/win-cond) | **VERBATIM C3, oracle-verified** | Done 2026-07-02. ~28k oracle comparisons, 0 mismatch. |
| Race camera / scoring / elimination | **VERBATIM C3** (C4 lane open) | Nearly done; C4 diff pass remaining. |
| AI control chain (as `.asi` reference) | **VERBATIM C3** | 8 fns C3'd; ~35k paired A/B calls, 0 mismatch. |
| AI *driving the standalone* | **SCAFFOLD** | Runs faithful-lookahead `AiStandalone.cpp`, not the verbatim chain. Gated on WS-A8. |
| Vehicle physics / handling | **SCAFFOLD → PARTIAL** | Kinematic in standalone; real RW-Physics ported to `.asi` (A5/A6 C4 grounded) but drive is unstable/uncalibrated. |
| Collision | **SCAFFOLD** | Ground raycast only. Real = RW-Physics contact system (WS-B), architecture gate still open. |
| Renderer | **SCAFFOLD (D3D9 spike) + PARTIAL** | Dev viewer renders textured tracks+cars; shipping RW-subset verbatim (WS-E) barely started. **770/965 render rows < C3, 217 open stubs.** |
| Power-ups | **PARTIAL (C2 scaffold-structure)** | Dispatcher + 9 types ported structurally; leaves stubbed; numeric rates are stand-ins. |
| Particles | **SCAFFOLD** | Invented visuals. |
| Audio | **DATA-VERIFIED / PARTIAL** | Both RWS formats cracked; plays. Char-bank map + impact FX + music-state transitions remain. |
| Video playback | **NONE** | 4 C2 rows, 0 ported. |
| Multiplayer / split-screen | **NONE** | R7 tail; online out of scope for v1.0. |

**The two long poles:** (1) **Renderer WS-E** — largest, least-mature subsystem; (2) **Physics+Collision
WS-A/B** — the standalone doesn't yet drive on real physics. Everything else is the proven
parse → port → `diff-original` verify loop at manageable size.

### Two structural facts that shape the whole plan
- **The C2→C3 batch-fanout lane is mined out** (flat-lane yield fell below the ~30% do-not-run
  threshold). *All further progress is demand-driven slice work* — no more mass batches. The throttle
  was always harness `arg_type` coverage.
- **Discovery gap:** the demand map finds **1,788 RVAs inside the race slice's static call-closure
  that have no `hooks.csv` row at all.** Undiscovered functions the standalone reaches will keep
  surfacing as new demand — budget for discovery, not just promotion.

---

## 2. The finish line (DoD recap)

- **F-DoD** (function): RVA pinned, ≥C3, no `[UNCERTAIN]`, no stubs, `diff-original` clean, hook
  registered + toggleable, inline RVA comments.
- **S-DoD** (subsystem): standalone runs its canonical scenario *natively*; every function it executes
  is F-DONE; unexecuted functions ported-or-`deferred-not-needed`; structs documented; asset formats
  have round-trip tools; `STUBS.md` section empty.
- **P-DoD** (v1.0 ship): every subsystem S-DONE · clean playthrough of **every track + vehicle + mode**
  on `mashed_re.exe` alone · trackers zero-or-`wontfix` · dev `.asi` dropped from the ship build.

Horizon is 12–24 months solo. **Order phases, not dates.** The discipline that makes it finite: never
advance a phase until its DoD holds; every verbatim port lands a `diff-original` C4.

---

## 3. Recommended route — four milestones

The two long poles (WS-E, WS-A/B) are independent and each enormous; a solo dev cannot push both hard
at once. The cheapest-value-first route makes the *whole game playable on the current scaffold first*,
then climbs the poles against a complete, testable game. Depth work is easier to verify when the game
around it already works.

### M1 — "Playable whole game" (scaffold-faithful) — CHEAPEST, DO FIRST
Goal: every mode, menu path, power-up, save/unlock, audio cue, and screen works end-to-end on the
*current* scaffold physics + D3D9-spike renderer. Deliberately **not** bit-identical yet.
Mostly WS-G / WS-D / WS-J / WS-F tails + the known gaps below. Delivers a testable, motivating,
fully-playable build and de-risks "does the whole loop hold together" before the expensive depth work.
Contents:
- ~~Close **D-11056 / U-8997** — rule-5 collectible feed~~ **DONE 2026-07-04** (KTC_NewCopter chain
  traced with definitive xref closure; TrackRenderer feed landed; suites GREEN).
- ~~Close **D-11057** — cup-progression tier advance + continue-cup + game-length config screens
  (WS-G2)~~ **DONE 2026-07-04** (Nav_ConfigEditWrap + config-edit demo landed; residue narrowed to
  **D-11059**: GameFlow reachability, overlay rewire U-9013, threshold table).
- **WS-G4** — remaining menu screens/options paths.
- **WS-D** power-ups: replace stand-in numeric rates with the real values; finish per-type wiring that
  isn't blocked on WS-B/E.
- **WS-J** audio: char→engine-bank map + music-state transitions (skip impact FX — those need WS-B).
- **Video playback**: scope and port the 4-function video subsystem (or `deferred-not-needed` with
  rationale if the menus never require it).
- **WS-I** split-screen local MP: decide in/out for v1.0 (gate below), wire if in.
- **WS-F** tail: **D-11058** LapData `Lap_Line_End` (find the consumer or `deferred-not-needed`).

### M2 — "Faithful race" (physics/collision depth) — BIGGEST INVESTMENT
The critical path **WS-B1 gate → WS-B → WS-A core → WS-A8 wire → WS-C verbatim-AI drive.** Race *feel*
becomes bit-faithful and the standalone drives on real RW-Physics. This is where most tokens go.
Prerequisite decision (WS-B1) is cheap and must be made first (gate below).

### M3 — "Faithful render" (WS-E) — SECOND-BIGGEST, PARALLELIZABLE
Climb the RW-subset verbatim renderer to replace the D3D9 spike (E1 world → E2 material/TXD → E3
RpWorld lighting + vehicle-lighting consumer → E4 immediate-mode/2D → E5 wire+parity). Independent of
M2, so it can run as the parallel track / change-of-pace whenever the physics lane is blocked.

### M4 — "Ship" (P-DoD)
Per-subsystem stub/uncertainty burn-down to empty, drop the `.asi` from the ship build matrix,
clean full playthrough of every track/vehicle/mode, packaging + README.

> **Reorder fork (your call — see gate D5):** if *faithful feel* matters more to you than *breadth
> soonest*, swap M1 and M2 — resolve WS-B1 and drive the physics spine first. My recommendation is
> M1-first (cheaper, delivers a playable game sooner, gives M2/M3 a complete test harness), but this
> is a genuine priorities decision and it's yours.

---

## 4. Per-workstream detail (the route through each)

Each workstream = one focused session (worktree + Ghidra pool slot). Acceptance = `diff-original` C4
on a canonical scenario (or the parity harness for frontend/visual). Status from the ROADMAP completion
plan + the 2026-07-03 tracker parse.

- **WS-A Vehicle physics** — biggest, mostly sequential. A1 struct ✅, A2 RW-math ✅(C4), A3 spawn
  chain, A4 control-input (0x00470670), **A5 core (0x0046ddb0)** C4-grounded / airborne 1-ULP residual
  (U-8991, accepted), A6 (0x00467650/0x00468980) C4, A7 tuning-const harvest, **A8 wire+diff** (the
  payoff: real physics driving the standalone). *Blocked on A2 (done) + WS-B.* The immediate blocker is
  the **WAVE16 drive bring-up** (below) — real-physics drive is currently unstable (speed 0/260/0) and
  the steer sign/scale is uncalibrated.
- **WS-B Collision / RW-Physics** — **B1 architecture gate (STOP-AND-ASK, still open):** vendor real
  qhull-2002.1 vs port the RW-Physics contact subset. B2 car↔world, B3 car↔car, B4 wire+diff.
  *Prereq for WS-A handling, WS-D projectiles, WS-J impact FX.* **This gate is the single highest-leverage
  decision in the project — make it first in M2.**
- **WS-C AI drivers** — C1 blueprint ✅, C2 verbatim port ✅ (now C3), **C3 wire+diff** the verbatim
  chain onto the canonical race (replaces the scaffold `AiStandalone.cpp`). *Gated on WS-A8* (needs real
  physics under the AI to diff position).
- **WS-D Power-up effects** — D1 ✅, **D2 per-type verbatim** gated on Ghidra fn-split (0x453f60–0x45be81)
  + WS-A1 + WS-B + WS-E, D3 wire+diff. The M1 slice is only the un-blocked parts (rates, structure).
- **WS-E Renderer (RW-subset verbatim; librw = fallback)** — E1 world path, E2 material/multi-TXD, E3
  RpWorld lighting + vehicle-lighting consumer (ledger #9), E4 immediate-mode/2D, E5 wire+parity.
  770/965 rows < C3, 217 stubs — expect this to be the longest single lane. Runs independently.
- **WS-F Data formats** — SPL/ANM/UVA/LAPDATA/MTS cracked and mostly DATA-VERIFIED. Loose end: **D-11058**
  `Lap_Line_End`. Otherwise parallel/complete.
- **WS-G Modes & frontend** — G1 rules ✅, G2 wire modes (mostly done; residual **D-11057**), G3 cup
  place-names (needs Frida), **G4 remaining screens/options**, G5 gamesave ✅.
- **WS-H Verification / C4 lane** — ongoing; every landed port gets a `diff-original`. The C4-campaign
  tooling + parity pipeline already exist (`re/parity/`, the `canonical_c4_*` harnesses — just
  hardened in the 2026-07-03 review).
- **WS-I Multiplayer** — R7 tail; **online out of scope for v1.0**; split-screen = gate D3.
- **WS-J Audio remainder** — char→engine-bank map, music-state transitions (M1); impact/skid FX need
  WS-B collision events (M2-adjacent).

### Discovery & queues
- **Demand map §3** (`re/analysis/r6_demand_map_2026-07.md`) is the port queue: 730 ranked C2 gaps the
  race slice statically needs. Top pulls: `0x004a2c48` (`__ftol`, in-deg 31 — pervasive leaf, do early),
  the AI control/targeting cluster, render RW primitives (`RwMatrixTranslate`, `RpClumpRender`,
  `RpWorld*`), and the audio RWS chain (`0x005a*`). **Demand map §4** = the 1,788 undiscovered RVAs
  (discovery backlog).
- **`re/SCRIBE_QUEUE.md`**: 11 queued (drain via `ghidra-sweep`). **`re/PROMOTION_QUEUE.md`**: empty.
- **`re/SESSION_PROMPTS_WAVE16.md`** is the last generated wave (physics-drive bring-up; predates the
  2026-07-02 rules work) — its WS-PHYS-DRIVE-STABILIZE / STEER-CALIB items are the live M2 opener.

---

## 5. Open decision gates (STOP-AND-ASK — these need *you*)

Resolve these before the dependent work, not mid-stream. Each is a genuine fork with material cost.

- **D1 — WS-B1 collision architecture:** vendor real qhull-2002.1 (faster to working, larger binary,
  less "verbatim") vs port the RW-Physics contact subset (true to F-DoD, much larger effort). *Blocks
  WS-A handling, WS-D, WS-J.* **Highest leverage. Decide first in M2.**
- **D2 — Renderer commitment:** RW-subset verbatim (ratified Option B, but 770 rows + 217 stubs of
  work) vs accept `librw` as the shipping renderer to save months (deviates from pure verbatim, but the
  D3D9 spike proves a non-RW renderer is viable). Confirm before sinking M3 tokens.
- **D3 — Multiplayer scope for v1.0:** split-screen local MP in or out? (Online is already out.) In =
  a real WS-I lane; out = `deferred-not-needed` and one fewer mode in the P-DoD playthrough.
- **D4 — Airborne bit-identity:** accept the A5 airborne 1-ULP float10 residual (U-8991) as
  C4-grounded (recommended — it's faithful within FP tolerance) vs invest in the naked-asm float10 shim
  for strict bit-identity.
- **D5 — Milestone order:** M1 breadth-first (recommended) vs M2 depth-first (if "faithful feel" is
  the priority).

---

## 6. Operating model — delegate the maximum to claude2 (Accenture), spend Fable only on the irreducible

**Governing principle:** the Fable/personal account (account3) does *only what is physically impossible
on claude2* — live **Ghidra MCP**, live **Frida MCP**, builds, running the game, git mutations, and the
final RE *judgment*. **Everything text-shaped goes to claude2** (Accenture worker): all reading,
surveying, cross-referencing, mechanical decode plating, **first-draft verbatim ports**, doc/plate
writing, and log/CSV/diff analysis. claude2 is read-only but has generous limits and **does not spend
Fable tokens** (`.claude/skills/repo-fleet/scripts/delegate.ps1 -Repo Mashed`, or just spawn a
non-`[local]` subagent — the agent-gate auto-routes it there).

### Why this split (per the Accenture policy on claude2)
claude2's MCP allowlist **excludes `ghidra-headless-mcp` and `frida-game-hacking`**, and
`enableAllProjectMcpServers:false` disables the project `.mcp.json` — so interactive decomp and
behavioral diffs *cannot* run there. But **none of the policy blocks plain project reads or text
generation**, and the allowed model list includes Sonnet/Haiku/Opus. So push all text-in/text-out work
to claude2; keep only MCP + judgment + execution on Fable.

### Who runs what
| Task class | Runs on | Note |
|---|---|---|
| Source surveys, call-site maps, subsystem inventories | **claude2** | pure read |
| Tracker reads/filters (hooks.csv, STUBS, UNCERTAINTIES, DEFERRED, CHANGELOG) | **claude2** | pure read |
| Demand-map slicing (§3 queue, §4 discovery backlog) | **claude2** | pure read |
| Prior-art / cross-build / analysis-note cross-reference | **claude2** | pure read |
| **Mechanical decode plate** from decomp *text* | **claude2** | scribe-transcriber pattern |
| **First-draft verbatim C++ port of a leaf**, from decomp *text* | **claude2** | account3 reviews before it lands |
| Doc/report/kickoff-prompt drafting from existing sources | **claude2** | returns text; account3 writes |
| Frida trace / diff-CSV / limiter-log analysis (*after* the run) | **claude2** | pure read |
| Port-vs-decomp transcription-error review | **claude2** | two texts, no MCP |
| — | — | — |
| Pull raw decompilation text (target + depth-1 callees) | **account3** | Ghidra MCP — blocked on claude2 |
| Judgment ports (dispatchers, calling-convention, x87/float10 fn-ptrs) | **account3** | RE judgment |
| Frida `diff-original` / oracle / behavioral runs | **account3** | Frida MCP — blocked on claude2 |
| Builds, running MASHED, patch scripts | **account3** | execution |
| Apply writes, `re-classify`, git commit/merge | **account3** | writes/mutations |

### Per-slice loop (delegation-maximized "port one function")
1. **claude2** — survey: locate the target, its callers/callees, existing notes, demand rank → scoped brief.
2. **account3** — one Ghidra MCP pull of the decomp (target + depth-1 callees). Paste the text into the next claude2 task.
3. **claude2** — mechanical decode + **first-draft verbatim C++ port** (RVA citations, constants, `[UNCERTAIN]` list) from that decomp text → draft package.
4. **account3** — review the draft (judgment), correct, write the file, register the hook (`RH_ScopedInstall`), build.
5. **account3** — Frida `diff-original`. If RED, hand the trace to **claude2** to analyze; account3 applies the fix.
6. **account3** — `re-classify` + commit.

Fable spend collapses to steps 2/4/5/6-judgment; **claude2 absorbs 1, 3, and RED-trace analysis** — the
token-heavy reading and drafting. Keep *dispatchers and judgment-heavy* ports (calling-convention,
x87/float10, live-state) on account3; delegate *leaf/mechanical* drafts to claude2.

### Experimental — push even verification to claude2 via the allowlisted `shell` MCP
The Frida Python harnesses (`re/frida/*.py`) and Ghidra headless CLI scripts run via **shell**, not the
blocked MCPs — and `shell` *is* on claude2's allowlist. If claude2 has shell access to the game machine,
it may run `diff-original`/oracle/behavioral checks and headless decode itself, moving even verification
off Fable. Gated by claude2's "Ask" install rules + disabled bypass-mode (fragile headless). **Run test
T1 (§7) before relying on it**; if it works, promote Frida-CLI verification and Ghidra-headless decode
to claude2 and shrink account3 to interactive-MCP + judgment only.

### Standing rules (unchanged)
- **One workstream = one focused session** (worktree + Ghidra pool slot); end each with a ready-to-paste
  kickoff prompt for the next; split at measure → port → verify → merge boundaries so context doesn't re-bill.
- On **account3**, route the mechanical residue (scribe, tracker edits, log parse, leaf decode) to
  **Sonnet/Haiku**; reserve **Fable** for the hard decomp/diff judgment only.
- **Demand-driven, not batches** (the fanout lane is mined out): pull from demand-map §3 / the milestone slice.
- **Acceptance = `diff-original` C4** (or the parity harness for visual work) — never "compiles and doesn't crash."
- **Trackers only via `re-classify`**; discovery via `ghidra-sweep` (SCRIBE_QUEUE) + demand-map §4.
- **State lookups via one-liners** (hooks.csv/queues via PowerShell filters; CHANGELOG head only) — and
  delegate even those to claude2 when they're more than a one-liner.

---

## 7. Concrete next-sessions queue (start here regardless of the M-fork)

Every item below leads with a **claude2** delegation (survey/decode/draft) and reserves account3/Fable
for the MCP + judgment tail, per §6.

- **T1 — Delegation-reach test (do first; ~2 min).** From account3, `delegate.ps1 -Repo Mashed` a
  trivial *shell* task to claude2 (e.g. `py -3.12 --version`, then a dry `import frida` check, then
  `py -3.12 re/frida/run_diff.py --help`). Confirms whether claude2 can execute shell at all under the
  Accenture "Ask"/no-bypass rules. **If it runs:** escalate to a real `re/frida/run_diff.py <leaf>` on
  claude2 and move Frida-CLI verification off Fable (§6 experimental lane). **If it prompts/hangs:**
  claude2 stays read-only-and-draft; keep verification on account3.
1. **Decision session — WS-B1 collision architecture (D1).** *claude2:* summarize the qhull-island +
   RW-Physics cluster notes and lay out both options (effort/fidelity/binary-size). *account3/you:*
   decide. Cheap; unblocks the entire physics pole.
2. **WS-PHYS-DRIVE-STABILIZE (WAVE16).** *claude2:* survey the standalone drive path + WAVE16 notes and
   draft the sustained-drive harness. *account3:* run it, Frida-diff, root-cause the erratic speed
   (0/260/0) + steer sign/scale. The live blocker to WS-A8.
3. ~~**D-11056 / U-8997 — rule-5 collectible feed.**~~ **DONE 2026-07-04** — chain traced
   (KTC_NewCopter → FUN_00405780 → DAT_0063a5d0/d4, definitive xref closure), TrackRenderer feed
   landed, suites GREEN.
4. ~~**D-11057 — cup-progression tier advance + game-length screens (WS-G2).**~~ **DONE 2026-07-04**
   — full dec/inc decode of both config screens, Nav_ConfigEditWrap landed + behavioral demo GREEN;
   follow-on gaps narrowed to **D-11059**.
5. **Top demand-map §3 leaves.** (The `__ftol` head item 0x004a2c48 went **C3 2026-07-03**,
   byte-identical `FPURound_4a2c48`.) *account3:* one Ghidra pull → *claude2:* first-draft ports of
   the leaf cluster → *account3:* review + `diff-original`. Pairs with draining SCRIBE_QUEUE (11)
   via `ghidra-sweep`.

Then reassess against the chosen milestone order.

---

## 8. Risks & how this plan handles them

- **Renderer scope (WS-E)** — the #1 long pole. *Handled:* the D3D9 spike keeps the game playable, so
  WS-E is not on the M1 critical path; gate D2 lets you cap the effort with librw if needed.
- **Physics not driving cleanly** — *Handled:* WAVE16 stabilize is session #2; WS-B1 (D1) is resolved
  before the dependent WS-A core work.
- **Discovery gap (1,788 unmapped RVAs)** — *Handled:* budget discovery sessions (demand-map §4 +
  `ghidra-sweep`) alongside promotion; expect new demand, don't treat the map as fixed.
- **Batch lane mined out** — *Handled:* the plan is demand-driven throughout; no reliance on fanout
  yield.
- **Token exhaustion (Fable)** — *Handled:* §6 pushes the maximum to claude2 (Accenture) — all reads,
  surveys, decode plates, *first-draft verbatim ports*, and diff/log analysis — leaving Fable only the
  MCP + judgment + execution tail; mechanical account3 residue routes to Sonnet/Haiku. *Residual risk:*
  Ghidra/Frida MCP are policy-blocked on claude2, so the interactive decomp+diff core is irreducibly on
  Fable — the shell-loophole test (T1) is the one lever that could move even that off Fable.
- **Rule-5 unwinnable / soft-locks** — *Handled short-term:* the 2026-07-03 review fix falls rule-5
  back to the legacy terminating flow; D-11056 is the real fix in M1.

---

*Maintenance: refresh the §1 counts and the scaffold/verbatim ledger after each milestone; update the
next-5 queue as sessions land. Keep this doc as the single strategic index; per-slice detail lives in
`re/analysis/` notes and the demand map.*
