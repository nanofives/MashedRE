# Pipeline Overview — the whole process in plain language

*Written 2026-07-16 for the project owner. This is the orientation map: what the pipeline is,
who does what, where to look for status, and how visual feedback works. Technical sources:
`re/CONFIDENCE.md`, `ROADMAP.md`, `re/analysis/RE_MASTER_PLAN_2026-07.md`, `re/analysis/parity_tooling.md`.*

---

## 1. The pipeline — the life of one game function

The original game (`MASHED.exe`) is made of thousands of small functions. Rebuilding the game
means taking each one from "unknown machine code" to "faithfully rewritten in our own source."
Every function is one row in `hooks.csv` with a confidence level:

| Level | Plain meaning | Who/what does it |
|-------|---------------|------------------|
| **C0 — Unknown** | We know an address exists, nothing else. | (default state) |
| **C1 — Located** | We have a rough idea of its purpose and which subsystem it belongs to. | `discover-c1-batch` / `ghidra-sweep` skills (Ghidra) |
| **C2 — Transcribed** | The code has been read line-by-line and written down mechanically — but not yet turned into working C++. | Ghidra decompilation + scribe sessions |
| **C3 — Reimplemented** | We wrote a C++ version, it builds, and it's installed as a toggleable "hook" that can replace the original at runtime. | `hook-author` skill, recorded via `re-classify` |
| **C4 — Proven** | The C++ version was shown to behave **bit-for-bit identical** to the original during a real gameplay run, with our code actually live. | `diff-original` skill (Frida side-by-side trace) |

Two hard rules keep this honest:
- **Promotion is evidence-gated.** Only the `re-classify` skill may change a level, and it
  refuses promotions without proof. "It compiles and doesn't crash" earns nothing.
- **NO-GUESSING.** Every claim about the original cites the exact address it came from.

**"Promotion" = moving a function up this ladder.** When you hear "a promotion session,"
it means sessions doing C2→C3 (writing + verifying C++ ports) or C3→C4 (proving
bit-identity in a real race).

## 2. Where the project stands (2026-07-16)

**Breadth is done — depth is the mountain.** The standalone `mashed_re.exe` already runs the
whole game loop end-to-end (menu → pick track → race → results). But much of the in-race
logic is still *scaffold* — placeholder code that looks and mostly behaves right without
being the faithful 1:1 port yet.

Function ledger (5,895 tracked): **C1 920 · C2 3,922 · C3 861 · C4 161** (~17% at C3+).

- **Active: the B5 lane** — reconstructing the car physics/collision engine (RenderWare
  Physics 3.7 + the qhull geometry library). B5a–B5d are done; **B5e (the full solver
  island, ~185 functions)** is in progress in a parallel session — it is the **largest
  single job left in the project**, comparable in size to the renderer.
- **After B5e:** wiring real physics into the car (A8), then AI driving on real physics.
- **Deliberately deferred:** intro videos, split-screen/online multiplayer (out for v1.0),
  a microscopic airborne float rounding residual (accepted).
- **Retired:** the mass "batch promotion" pipelines — the easy functions are mined out;
  work is now demand-driven (port what the running game actually executes).

## 3. Why it takes so long

1. **The acceptance bar is bit-for-bit.** A function is only "done" when a Frida trace shows
   our code produced *the exact same bytes/floats* as the original during a real gameplay
   scenario. Floating-point code makes this brutal — one instruction ordered differently can
   shift the last decimal digit and fail the diff.
2. **The two long poles are huge.** The renderer (~770 functions below C3) and the physics
   engine (the current B5 lane) are engine-sized libraries, not game code.
3. **Verification is irreducibly serial.** Ghidra/Frida runs can't be offloaded to the cheap
   worker account; each verification needs a live game run on this machine.
4. **~1,788 functions the game calls aren't even catalogued yet** (discovery gap).

This slowness is a *choice*: the alternative (guess at behavior, ship what looks right) is
how ports drift subtly wrong forever. The scaffold-first approach means you get a playable
game early while the faithful parts replace the scaffold piece by piece.

## 4. Where to look — the tracking surfaces

| File | Question it answers |
|------|---------------------|
| `hooks.csv` | What's the status of every function? (the master ledger) |
| `STUBS.md` | What placeholder wiring still needs replacing? (~1,118 open) |
| `UNCERTAINTIES.md` | What don't we know yet, and how will we find out? |
| `DEFERRED.md` | What did we decide NOT to do, and what would make us revisit? |
| `re/PROMOTION_QUEUE.md` | What's waiting for its C3 verification pass? |
| `re/SCRIBE_QUEUE.md` | What's waiting for its Ghidra write-up? |
| `re/analysis/CHANGELOG.md` | What changed and when? (newest at top) |
| `re/analysis/RE_MASTER_PLAN_2026-07.md` | What's the strategic order of work? |

**To get oriented in any session, ask:** *"Give me the status brief"* — the session should
report: C-level counts from `hooks.csv`, active lane from the master plan, the head of
`CHANGELOG.md`, and any open stop-and-ask gates. This document is the map; those files are
the live state.

## 5. Visual feedback — how the owner reviews the game

**Why it's needed:** bit-identical functions can still *compose* wrong (right function,
wired to the wrong place, wrong draw order, wrong palette). Automated tooling catches a lot,
but human eyes on the running game are a distinct, necessary check.

**Existing tooling** (`re/analysis/parity_tooling.md`): a draw-command differ
(`drawlist_diff.py`, menu baseline GREEN 118/118), a menu-coverage checker, and a pixel
backstop (`imgdiff.py`). Limits: it covers **menu/frontend composition only** — in-race 3D
rendering is out of its scope, which is exactly where owner eyes matter most.

**The feedback-round protocol** (established, works well):
1. **Round 1 — autonomous:** Claude works a batch of items without interruption.
2. **Round 2 — batched questions:** Claude collects every question/ambiguity into one list
   for the owner, instead of interrupting per-item.
3. **Round 3 — guided visual:** the owner runs/watches the game and reports differences
   **one item at a time**, each compared side-by-side against the original.

**Rules for visual reports (owner side):**
- Report *what you see*, not what you think causes it: "the car shadow is missing on Arctic,"
  "the countdown font is thinner than the original," "smoke puffs disappear too fast."
- Side-by-side beats memory: when possible, look at the original (it boots windowed in ~5 s)
  and the standalone together.
- Every report gets triaged into exactly one of:
  - **FIX NOW** — a wiring/composition bug in already-ported code → fixed and re-verified.
  - **BLOCKED-ON-PORT** — the visual owner is a function still at scaffold/C2; logged with
    the blocking RVA and revisited when that function lands. *(This is the legitimate
    "pushback" case — expect it often while renderer/physics work is in flight.)*
  - **ORIGINAL-DOES-THIS-TOO** — verified against the original; not a bug.
- Triaged items live in a visual-feedback tracker note (per-round file under `re/analysis/`,
  like `frontend_feedback_20260612.md` was) so nothing reported is silently dropped.

## 6. Assessment — are we on the best path? (2026-07-16)

Short answer: **yes, with one gap.** The strategy choices are sound and were each decided
deliberately (recorded in the master plan and memory):
- Demand-driven porting (only what the game executes) beats percentage-chasing — the batch
  lanes were retired *because measurement showed they ran dry*, not by neglect.
- The B5 physics lane is the correct priority: driving feel is the game, and physics blocks
  AI, collisions, and race correctness downstream.
- The strict C4 bar is what makes "done" mean done.

The gap: **owner-visible progress reporting.** The trackers are excellent for sessions but
opaque to the owner — hence this document, the "status brief" convention (§4), and the
visual feedback rounds (§5) as the standing owner-facing loop.
