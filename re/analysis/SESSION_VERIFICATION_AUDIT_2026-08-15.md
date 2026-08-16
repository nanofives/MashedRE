# Verification audit — refreshed snapshot (2026-08-15, D0.1)

Supersedes `SESSION_VERIFICATION_AUDIT_2026-06-16.md` as the current answer. That file is
**kept, not deleted** — it is the historical record of the 06-16..06-19 physics C4 lane and
its per-session appendices are still the evidence for those verdicts. What it is not is a
snapshot: it grew into an append-log, and its front-matter classification is ~2 months
stale, predating B5, librw, and every 2026-07 promotion.

**This document is a snapshot and must be dated and replaced, never appended to.**

## The question changed

The old audit asked *"what did we port, and what still needs a C4?"* Its framing ends at
"port it, then C4 it" — **there is no step in it where the port becomes what the exe runs.**

ROADMAP v3 (2026-08-15) added that step. So this audit asks instead:

> For each subsystem, what does the **default build** — `mashed_re.exe` with no `MASHED_*`
> set — actually execute: an invented scaffold, a verbatim port, or nothing?

---

## 1. The finding that reframes D0 — CORRECTED 2026-08-15 (D0.7)

**The first version of this section was wrong about the cause, and the correction matters
more than the original claim.** It reported "193 of 433 `.cpp` linked; `Save/` 0 of 17;
no env var can reach them" and implied forgotten linkage. That reading does not survive
the diff against `asi_sources.rsp`.

Measured (`build.bat` exe block + the 5 isolated `.obj` compiles it links, vs `asi_sources.rsp`):

```
total .cpp under mashed_re/   433
in mashed_re.exe              198     (193 in the source list + 5 isolated .obj:
                                       QhullBridge, RwBridge, RwRasterBridge,
                                       RwSceneBuild, RwRaceSubmit)
in mashed_re_dev.asi          365
in BOTH                       147
asi-only                      218
in NEITHER                     17     (10 are tests/selftests; 7 unexplained)
```

**215 of those 218 asi-only files install `RH_ScopedInstall` hooks.** They are harness code
by construction — they patch the *original* `MASHED.exe`. They cannot be "linked into the
exe", because they are not standalone implementations. There is no drift to fix, and my
earlier framing would have sent someone to add build.bat lines that could not work.

**The real finding is different, and worse.** Take `Save/`: all 16 asi-only files are hook
installers (`GameSave.cpp` 4 hooks, `SettingsDialog.cpp` 6, `SettingsAndIO.cpp` 5, …). Zero
Save files are in the exe. So:

> **The standalone has no save subsystem at all.** Not "unlinked" — absent.

Same shape for audio: 21 of 25 `Audio/` files are asi-only hooks; only 4 reach the exe.

This reframes the coverage table rather than the build. `save` reads **93.8% C3+** — the
strongest subsystem in the project — and that number means *"we understand the original's
save code deeply and have verified it against the original"*. It does **not** mean the
standalone saves anything. Those are different claims, and `hooks.csv` cannot distinguish
them because it tracks understanding-of-the-original, not standalone capability.

**So the default-build rule needs its second clause, but a different one than I first
wrote.** Not "the TU must be linked" — that is a build detail. Rather:

> A subsystem counts toward P-DoD only when the **standalone implements it**. A C4-verified
> hook proves we understand the original; it contributes nothing to `mashed_re.exe` unless
> a standalone implementation exists alongside it.

Whether every subsystem *needs* a standalone implementation is a scope question (audio
might reasonably be re-implemented rather than ported; D-11062 already defers video on
exactly this reasoning). What is not defensible is reading a hook-derived percentage as
progress toward a working standalone.

**The 7 genuinely unexplained files** (dead to both targets, excluding the 10
tests/selftests): `Boot/CrtEnvArgv.cpp`, `Compat/PizOpenBypass.cpp`,
`Frontend/HudFrontendDispatchers_t4.cpp`, and four `MixedC3Sweep.cpp` under
`Frontend/`, `Input/`, `Render/`, `Util/` — batch-era artifacts. These are the only real
dead code found; triage separately, they are small.

## 2. Default-execution table

| Subsystem | What the DEFAULT build runs | Class | Port exists but gated? |
|---|---|---|---|
| Vehicle handling | kinematic scaffold in `TrackRenderer::UpdateCar` | SCAFFOLD | **YES** — `MASHED_REAL_PHYSICS` (`Vehicle/VehiclePhysicsRun.cpp:155`) |
| Collision | `GroundHeight` probe (`TrackRenderer.cpp:1894`) | SCAFFOLD | **YES, doubly** — 31 `Collision/*.cpp` link in, but the only caller sits inside the `MASHED_REAL_PHYSICS` branch (`VehiclePhysicsRun.cpp:354`) |
| Renderer (world) | hand-written D3D9 path | SCAFFOLD | **YES** — `MASHED_RENDER_LIBRW` (`LibRw/RwRaceSubmit.cpp:135`) |
| Camera | invented chase rig | **HYBRID — see §3** | no flag exists |
| AI driving | `faithful_nav` ON by default (`TrackRenderer.cpp:2569`), driving ported `0x00443dc0` lookahead | HYBRID | verbatim ControlStep bands deliberately not adopted (`:2573`); `Ai/AiController*.cpp` not linked |
| Race rules / scoring | rule engine, **already default-on** (`TrackRenderer.cpp:3171`) | VERBATIM | — |
| Powerup effects | scaffold | SCAFFOLD | no |
| Renderer (2D/HUD/menu) | D3D9 bridge; `QuadRenderer` no longer draws | SCAFFOLD | librw I6 planned, not built |
| Audio | — | **ABSENT (not linked)** | n/a — see §1 |
| Save / progress | — | **ABSENT (not linked)** | n/a — see §1 |
| Video / intro | static splash | SCAFFOLD | D-11062, deliberate |

**Only three flags select a port over a scaffold:** `MASHED_RENDER_LIBRW`,
`MASHED_REAL_PHYSICS`, and `MASHED_RW_RENDER` (`RwWorldRender.cpp:230`, inert even when
set). That is a smaller migration surface than feared — the flag half of the v3 gap is
tractable.

**Three flags are already correctly inverted** and are the model to copy:
`MASHED_RULE_ENGINE` (`TrackRenderer.cpp:3171`), `MASHED_LIBRW_INST` (`:222`),
`MASHED_GATE_RIBBON_AI` (`:1634`). Each defaults to the port and the flag only turns it
*off* for A/B — exactly corollary 2 of the default-build rule.

---

## 3. Camera: the case a flag census cannot see

`Race/RaceCamera` — verbatim ports of `0x00446520`, `0x00410d10`, `0x00441820` — **runs
every frame** at `TrackRenderer.cpp:3380`. Its computed pose is then **discarded
unconditionally** at `:3968-3993` in favour of the invented chase rig.

No flag guards this. Grepping the flag inventory would report camera as "no migration
pending" and be wrong. The port is present, executing, and its output thrown away.

Camera C4 = **0 of 14 rows**, and the old audit's Section-B "C4 QUEUE" item for
`RaceCamera` was never closed. It now has a deeper problem than missing evidence: even
with a green diff, the default build would not render from it.

This is also what `MASHED_CAM_POSE` (ported 2026-08-15) interacts with — that variable
overrides the *chase rig's* eye/at, which is why it works at all. Same-view parity
currently routes around the verbatim camera rather than through it.

---

## 4. The cleanest instance of the v3 debt

`re/analysis/CHANGELOG.md:448` (2026-06-18), verbatim:

> "WS-A8 `MASHED_REAL_PHYSICS` stays gated OFF until A6a/A6b reach C4."

A written, dated exit condition. Checked today against `hooks.csv`:

```
0046b540 VehicleSpawnInit             C4     (A3)
00470670 VehicleControlUpdate         C4     (A4)
0046ddb0 VehicleWheelForceIntegrator  C4     (A5)
00467650 VehicleWheelDrivetrainUpdate C4     (A6a)
00468980 VehicleAeroStabilizer        C4     (A6b)
```

**Physics is 5 of 5 at C4**, not the old audit's 3/5. A6a and A6b both reached C4 on
2026-07-01 (A6a 72/72 grounded + 20/20 airborne; A6b 12 natural airborne frames, ndiff=0).

**The exit condition was met on 2026-07-01 and the flag was never inverted.** Six weeks.
Nobody was wrong at any single step — the condition was recorded properly, the work was
done properly, and the C4s are sound. There was simply no step in the process that says
"a met exit condition triggers an action". v3's D2 exists to close exactly this.

Caveat kept honest: the statediff wedge caps *full-set* evidence at 5/6 boots
(`CHANGELOG:11`), second mechanism unbisected. The five A-stage C4s do **not** rest on
that lane, so they are not retroactively threatened — but new full-set claims are bounded
until it is fixed.

---

## 5. C4 evidence integrity — a real problem

- **`/log/` is gitignored** (`.gitignore:16`). 27 files tracked, 2,217 on disk. The C4
  evidence base is essentially not in version control.
- **15 cited C4 artifacts do not exist even locally.** Most seriously,
  `log/c4_racediff_result.json` is the sole evidence for **14 C4 rows** (mostly `vehicle`)
  and is absent. `log/mass_canonical_wave0.txt` covers 5 more.

Verified directly: the file is absent, and `grep -c "c4_racediff_result" hooks.csv`
returns **14**.

This does not mean those 14 promotions were wrong. It means **they are currently
unfalsifiable**, which under `re/CONFIDENCE.md` and the no-overclaim rule is not a state a
C4 row may sit in. Two honest options, and this needs a decision rather than a default:

1. **Re-run** the canonical scenario and regenerate the artifact — restores the evidence.
2. **Demote** the 14 rows pending re-verification — costs the headline C4 number.

Doing neither, and leaving them at C4 citing a file nobody can open, is the option the
project's own rules forbid. Same class of problem as `verify/s3bank_iso/` (found and fixed
2026-08-14), but with the artifact actually gone rather than merely uncommitted.

---

## 5b. The harness overwrites its own committed evidence

Found the hard way while running the D0 verification captures: **`MASHED_RACE_DEMO=1`
writes into `verify/race1/`, which is a committed evidence directory.**

`exe_main.cpp:1100`:

```c
std::snprintf(path, sizeof(path), "verify/race1/%s.bmp", tag);
```

Running the race demo silently overwrote 16 tracked BMPs across `verify/race1/` (9
tracked), `verify/r6/` (28 tracked) and `verify/r5/` (10 tracked) — including the R6 exit
demo stills that `DEFERRED.md` D-11061 cites as the evidence R6 was accepted on.

Recovered in full with `git checkout --`, because they were committed. Had they been in
the same state as `verify/s3bank_iso/` was yesterday — cited but untracked — they would
simply have been destroyed, with the overwrite invisible in `git status`.

Two things follow:

1. **The default output path of a verification harness must not be a committed evidence
   directory.** Captures belong in a dated scratch dir; promotion into a cited folder
   should be a deliberate, separate act.
2. This is the third instance this week of the same underlying pattern: evidence that a
   tracker cites being weaker than the citation implies (`verify/s3bank_iso/` uncommitted,
   `log/c4_racediff_result.json` absent, and now cited stills silently rewritable by a
   routine run). The trackers are load-bearing; **their evidence base currently is not.**

Filed as D0 item 8.

---

## 6. Flag inventory — corrected count

A raw prefix grep finds **149 distinct `MASHED_*` tokens**; only **128 have an actual
`getenv`/`GetEnvironmentVariableA` site**. The remainder are include guards, three
absolute-address macros (`MASHED_DAT_005…`), one compile-time `#define`
(`MASHED_PHYS_DIAG`), a filename (`MASHED_PATCHES`) and a prose prefix (`MASHED_QOL`).

ROADMAP v3's original "146" counted raw tokens and was wrong; corrected in that document
today. Reproduce:

```bash
grep -rhoE 'MASHED_[A-Z0-9_]+' mashedmod/src/ | sort -u | wc -l        # 149 tokens
# then filter to those with a literal getenv/GetEnvironmentVariableA site  -> 128
```

**Dead flags — referenced only in comments, no `getenv` anywhere:** `MASHED_REAL_AI`
(already known; `TrackRenderer.cpp:22,44`), plus three newly found — `MASHED_CHASE_FOG`,
`MASHED_AI_LEADER_SELFTEST`, `MASHED_INTERP_REGISTRY`. These are worse than clutter: a
comment naming a flag that does not exist is a false map. Delete the comments or implement
the flags.

Also corrected: `CHANGELOG:4` cites `TrackRenderer.cpp:2413` for
`MASHED_AI_DRIVES_PLAYER`; the `getenv` is at **`:2442`** (`:2413` is the countdown-freeze
block).

---

## 7. Reconciliation with the 2026-06-16 audit (its lines 12–99)

**Now WRONG — do not cite:**
- "Vehicle handling … **C4 blocked on the port**" — physics is 5/5 C4 (§4).
- "game-mode → rule mapping is scaffold" — rule engine is default-on and verbatim.
- "progression store is our own sidecar" — superseded.

**SUPERSEDED (true then, overtaken since):**
- AI gate-ribbon default — `faithful_nav` now default-on.
- Powerup dispatch, nav state machine, scoring trio, RW math leaves — all advanced.

**STILL TRUE:**
- Collision is scaffold on the default path (§2).
- Particles / HUD / results remain scaffold.
- All four Section-C data verifications hold.

The old audit has **no concept of** librw, the RWP-3.7 solver island, the statediff
harness, or the default-build rule. Read it for the physics-lane evidence, not for status.

---

## 8. Other corrections to the trackers

- **STUBS census stale by 4:** prose says 1,111 open / 145 struck; measured **1,107 open /
  149 struck** (both sum to 1,256). ROADMAP v3's "1,072 vs 1,109" was also off — replace
  both with the measured pair.
- UNCERTAINTIES: 2,999 rows. DEFERRED: 672 rows.
- These numbers move constantly. **Every count in this file states its command so it can
  be recomputed rather than trusted.**

---

## 9. What D0 still owes

1. ~~Refresh the audit~~ — this document.
2. Publish the full 128-flag classification (23 migration / ~102 verification / 4 dead was
   the working split; needs the per-flag table).
3. Reconcile UNCERTAINTIES `Type` vs `Blocks` (2,530 typed blocking vs 512 with a
   non-empty `Blocks` cell).
4. Fix the STUBS census header.
5. State CHANGELOG's scope in its header (decided 2026-08-15: it is the **tracker audit
   trail**, not a commit log).
6. **NEW — decide the 14 unfalsifiable C4 rows** (§5): re-run or demote.
7. **NEW — resolve the linkage gap** (§1): diff `asi_sources.rsp` against the exe list,
   then decide whether `Save/` and `Audio/` are unlinked by intent or by drift.

8. **NEW — stop the harness overwriting committed evidence** (§5b): change
   `exe_main.cpp:1100`'s default output away from `verify/race1/`.

Items 6, 7 and 8 did not exist when D0 was written. All three came out of this refresh.
Item 7 (linkage) is the largest by impact; item 8 is the cheapest to fix and the most
likely to cause silent damage before it is.
