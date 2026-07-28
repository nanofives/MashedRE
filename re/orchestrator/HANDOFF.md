# Mashed RE orchestrator — resume point (updated 2026-07-28, U-9025 ROOT-CAUSED AND FIXED)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions. Maximize account2.

Status brief — **C1 797 / C2 4034 / C3 849 / C4 184** (unchanged; this session changed no C-levels).
Branch `fix/u9025-recharacterise-and-regabi-defects`, everything committed through **219e5698**.

**A SECOND CLAUDE SESSION WAS ACTIVE IN THIS REPO** on 2026-07-28 afternoon — it wrote
`re/analysis/plans/promote_classified*.tsv` and `c3_batch_render{3,4}_*.tsv` at 17:00–18:37 and
had not touched the trackers since 12:05. Those files are still uncommitted and are NOT mine.
Coordinate before any bulk tracker write. See the multi-session skill.

---

## HEADLINE: U-9025 is resolved. It was our own port dropping an implicit return value.

`FUN_005aef00` (`AudioThreadDescInit`) loads `MOV EAX,[ESP+4]` as its first instruction and never
clobbers EAX, so it **returns `param_1`**. Ghidra types it `void`. All three callers branch on it:

```
005a8315  CALL FUN_005aef00
005a831a  ADD  ESP,0x10
005a831d  TEST EAX,EAX
005a831f  JZ   0x005a8334        <- skips the spawn
005a8328  CALL FUN_005aef30      <- __beginthread: the audio/stream WORKER THREAD
```

(same shape at `0x005be3db`). Our port was declared `void` and returned leftover EAX, so the
**worker thread was never created**. The semaphore at `[0x007dcae0]` is created *before* that
branch, so it still exists but is never serviced, and a later `WaitForSingleObject(..., INFINITE)`
hangs the GUI thread — no crash, no dump.

Fixed in **f1855ad9**; trackers in **219e5698**.

### Why it hid for so long

Eight prior instances of `feedback_installed_hook_abi_mismatch` were all **crashers**, so the hunt
was biased toward crash dumps. A dropped return that only *gates a branch* produces a missing
subsystem and a hang instead. And the row's C3 evidence — "Frida GREEN 10/10" — could never have
caught it: `arg_type thread_desc_init` fingerprints the five written fields and
`hooks_registry.py` declares `'ret': 'void'`.

**Rule worth keeping: Ghidra typing a function `void` is not evidence. The callers'
`TEST EAX,EAX` is.**

---

## Method that worked (reusable; it beat three rounds of bisecting)

1. **Catch the failure live** — `scripts/catch_wedge.py` inspects the wedged pid *before* anything
   kills it. Wedges caught on attempts 3, 2, 3.
2. **Instrument the object, not the code** — `MASHED_SEMTRACE=1` on `re/frida/statenav.py` hooks
   the *imported* wait/release/create APIs rather than 30 in-cluster call sites, filters on the
   live handle, and reports every site as `module+0xoffset`. Bare return addresses cost a whole
   round of wrong attribution (`DSOUND.dll` turned out to be the tell).
3. **Run a STOCK CONTROL for every claim.** This is what broke the case open: stock does ~5700
   balanced acquire/release pairs per run on a worker thread; the hooked build did almost none, on
   the GUI thread.
4. **Prove an absence with an UNFILTERED census.** The `CreateThread` census logged 96–101 events
   per run in both configs, so "zero game threads under hooks" is an absence, not a dead probe.
5. **Then localise with entry counts, not a bisect** — `MASHED_COUNT_RVAS` with milestone dumps
   (`dump_counts()` at title / track-confirm / start-attempt, because the end-of-run dump never
   happens on a wedge). Three RVAs along the chain; the first with count 0 pins the break.

Each of steps 1–5 is one or two runs. The three bisect rounds it replaced were ~4 h each and only
ever return a name.

### Three hypotheses died on the way — do not re-open

- **audio-hook interaction** (prior session's attribution) — disproved.
- **gate-zero** — predicted `[0x007dcb68]==0` at wedge time; measured `1`.
- **unpaired acquire by `FUN_005a8390`** — mine. In two caught wedges the blocking wait was the
  *first* wait ever issued on the handle, so no release could have been skipped.

The gate asymmetry at `0x005a83f4` / `0x005a8423` (acquire and release gated on two independent
reads of `[0x007dcb68]`, with `FUN_005a8460` decrementing it unlocked at `0x005a846b`) is **real
original-game code**, but stock never reaches its window. It only fired because the worker was
missing. Worth a note if a future hang looks similar; not worth patching now.

---

## Verification on record

| | before | after | STOCK |
|---|---|---|---|
| counts `0x005aef00` / `0x005aef30` / `0x005c2e79` | 1 / 0 / 0 | **1 / 1 / 1** | 1 / 1 / 1 |
| game `CreateThread` | 0 | **1** | 1 |
| stream-lock acquire/release | net −7, −6 | **2018/2018, 4010/4010** | net 0 (3/3) |
| runs completed | 3 wedges in 8 | **10/10** | 3/3 |

Emitted body byte-checked from the deployed `.asi` (export RVA `0x0000df80`): `MOV EAX,[ESP+4]`
… `c3`, EAX alive at the RET. "It compiled" is not evidence — cf. the `ds:` defect in `9f2a61d6`.

**Do not quote the completion rate as the proof.** Fisher one-tailed 3/8 vs 0/10 is **p = 0.069**,
short of 0.05. The claim rests on the measured mechanism. 10/10 also does not exclude a rarer
second wedge source (residual bound ≈26%/run).

---

## Lane B — the three fresh diffs asked for at session start

| RVA | outcome |
|---|---|
| `0x00430760` `IsMultiplayerMode` | **GREEN 10/10** path1 (`log/diff_430760_fresh.log`). |
| `0x004c1a00` `IntroSplashVtableSlot6` | **INCONCLUSIVE-DEGENERATE** — all 10 vectors are fake pointers, both sides all-zero. `--allow-degenerate` NOT used. Needs a scenario-attach seed with a real vtable object. |
| `0x004148b0` `AiLeader_Entry` | **not diffable as specified** — no `hooks_registry` entry, and the installed hook is a pure passthrough trampoline. The ported `LeaderTimer` is not installed anywhere and its snapshot/restore harness was deleted in `5811fd0c`. A `[0x0089a368]==2` seed would test the original, not our code. Decide what evidence this row should carry. |

---

## NEXT — recommended order

1. **Sweep the class.** Ports declared `void` (or with a return type narrower than the original's)
   whose callers do `TEST EAX,EAX` / `JZ` immediately after the `CALL`. Eight crashers plus this
   hang say the class is not mined out, and the sweep is static + cheap.
2. **Make `arg_type thread_desc_init` fingerprint the return value** (`diff_template.js`), then
   re-diff `0x005aef00` to re-earn its cleared evidence. Regenerate `re/frida/ARG_TYPES.md` after.
3. `0x004c1a00` scenario-attach seed; settle `0x004148b0`'s evidence story (item above).
4. `0x00442cbd` / `LoadingState2Enter` still not behaviourally confirmed.
5. `LobbySlotListRender` (`0x00439210`) remains a fabricated scaffold / NO-GUESSING violation.

## HYGIENE

- **Ghidra pool: slot 5 is the good analyzed clone** (120 MB). **Slot 4 is a stale 43 MB clone with
  no functions defined** — `function_at` and `search_bytes` return nothing and it looks like a
  broken project rather than an empty one. Slots 0/1/2 have JVM-leaked `.lock~` (slot 2's is held
  by a live handle: `rm` gives "Device or resource busy"). Clean shutdown is
  `program_close` → `ghidra_pool.sh release <N>`.
- **SCREENSAVER** blanking the display still makes MASHED exit `0xFFFFFFFF` ~4 s into boot with no
  dump, stock and hooked alike.
- **Never run two MASHED-spawning drivers concurrently.** All pids this session were killed by pid;
  `original/` intact; no worktrees.
- `verify/` has accumulated a lot of scratch pool dirs from this and prior sessions. Cited evidence
  for this session is in **`verify/u9025_semtrace/`** (committed; the multi-MB `.log` traces are
  gitignored and live in `log/`).

TO RESUME: read this file; start at NEXT item 1.
