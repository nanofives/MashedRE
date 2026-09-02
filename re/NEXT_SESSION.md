# Next session — kickoff prompt

Written at the end of the 2026-09-02 parent booted-race session (tip `c6505667` on
`race/first-frame-parity`, tree clean, `re/PROMOTION_QUEUE.md` empty). Paste the block below.

---

Resume the Mashed parent booted-race lane. Branch `race/first-frame-parity` @ `c6505667`, tree
clean, `re/PROMOTION_QUEUE.md` empty, no children running, no worktrees or pool slots held.

Read `re/analysis/CHANGELOG.md` (head only — newest first, the 2026-09-02 entries) and the memory
index before acting. Do NOT re-read the whole session history; the CHANGELOG entries are written to
be self-contained.

**State.** 8 C3s landed 2026-09-02: `0x0045dbe0` FloatSlider2Adjust (util), `0x00411ae0`
Ghost::PlaybackTick (vehicle), `0x004161e0` AiSplineTargetInit (ai), and five render raster rows
`0x004c7600` / `0x004c76f0` / `0x004c7860` / `0x004d5310` / `0x004d5340`. All verified by booted
in-race A/B self-tests with a control hook in the same boot.

**Two harnesses were built and are the main leverage available to you:**
- `scenario_launch.py --observe-texture-cluster` with `MASHED_OBSERVE_SPEC=<spec.json>` and
  `MASHED_OBSERVE_OUT=<name>` — records args, return value and per-row observables for ANY set of
  RVAs during a real booted run, and prints a per-row degenerate/non-degenerate verdict. `obs`
  entries can dereference an argument (post-call) or read an **absolute block pre AND post**, so a
  within-call delta is available. Shared agent code lives in `re/frida/observe_block.js`; specs in
  `re/frida/specs/`.
- `replay_session.py --observe` — same capture, but driving a **recorded human input trace**
  instead of the auto-driver. This reaches game states the auto-driver cannot: it is what made
  `Ghost::PlaybackTick` witnessable after the auto-driver made it look inert.

**Pick up one of these, in descending value:**

1. **The 5 allocator/stream-reader rows of the texture/raster cluster** — `0x004c77c0` RasterCreate,
   `0x004cc5e0`, `0x004cee90`, `0x004cefd0`, `0x004db2e0`. All C2, no reimpl. They were deliberately
   NOT dispatched to the round-3 child: their only measured observable is a freshly allocated
   pointer, so an A/B cannot compare returns (the modded pass allocates a second object) and
   stubbing the allocator is unsafe because the result is dereferenced downstream. **This is a
   design task first, not an authoring task.** The candidate design is the two-boot structural
   comparison written up in
   `re/analysis/promote_c2_vehicle_lowrva/replay_ghost_family_witness_20260902.md` — and note it is
   deliberately weaker than bit-identity, because heap pointers differ per boot, so it compares
   structure (which fields are null, invariants between fields, branch flags). Decide whether that
   clears the C3 bar BEFORE authoring anything.

2. **`FUN_0043d2a0`** — the revised unblock for `0x0047b9e0`. That row needs TWO independent
   preconditions, not one: an effect ID of `0x1f`/`0x21` in the table at `0x0067ed3c + idx*0x40`,
   **and** `DAT_007f0f50 != 0`. The second was measured 0 in every run including with powerups
   enabled, and `FUN_0043d2a0` (write site `0x0043d3c6`) is its only semantic writer. Characterise
   when it runs and what makes it write non-zero. Do this before any powerup-scenario work for this
   row — an earlier claim that one powerup scenario would unblock both `0x0047b9e0` and
   `0x00415200` was measured and partly disproved; it holds only for `0x00415200`.

3. **A powerup-forcing scenario** for `0x00415200` AiVehicle0ZeroProgressGuard, whose gate IS the
   powerup case directly (7/9/0xb/0x10/0x11). Its port is already authored and installed and its
   self-test already exists; the row fires ~1 run in 8, and the one green run predates its
   `ret1/ret0` coverage counters, so it needs a reproducible trigger and then a re-run requiring
   `ret1>0 AND ret0>0`. Useful lever already proven: `--powerups <n>` does take effect
   (`DAT_0067e9f8` moves 0 -> 1, and the indexed record then holds `0x16`).

4. **A completed Time Trial lap.** This unblocks `0x00411870` Replay::LapFinish (gate:
   `DAT_008991bc == 0xb && FUN_0040e350() == 6`, and the sector counter reaches only 1 in 150 s
   because the auto-driver never steers) AND the three unverified arms of the freshly-promoted
   `0x00411ae0`. Needs a human lap recorded via `record_session.py --name tt-lap --cov
   0x00411870,0x00411ae0,0x00411d90,0x00429310 --seconds 300`, then replayed. **This one requires
   the user at the keyboard once** — ask, do not assume.

5. **Two infra defects, both filed with proposed fixes**: `GHIDRA-POOL-DOUBLE-ISSUE` in
   `re/diag/KNOWN_ISSUES.md` (`ghidra_pool.ps1 acquire` can hand the same slot to two sessions; fix
   is a root-level per-slot lock stamped with the owning session id), and the pre-existing
   full-hook-set `--asi` replay exit (a full-hook-set replay of `003-race-drive` exits the game
   partway through; verified pre-existing by rebuilding with the new TU removed).

**Standing rules that earned their place this session — do not relearn them:**
- Build via PowerShell `& "mashedmod\build.bat"` and epoch-check the `.asi` is < 120 s old.
- Every booted verification gets a **control hook known to fire in the same boot**. An absent log
  with a live control is a real negative; an absent control means a broken run, not a negative.
- **Arm coverage counters BEFORE the first run**, not after it turns green. A bare
  `calls=N mism=0 ALL-GREEN` cannot separate "every compared field was exercised" from "the branch
  never fired". Count each branch, each observable that actually CHANGED, and an XOR fold so a
  repeated constant is visible.
- **A write of an unchanged value is invisible to a delta test.** "Did not move" is not "did not
  write". This produced a wrong "inert" verdict on `0x00411ae0` and a false MISMATCH on
  `0x004d5310` in the same day.
- **Count it before designing a witness for it.** `0x0047b9e0` had an "add an observable" task that
  was premature: it executes 0 times in a race. One `MASHED_COUNT_RVAS` run beats a harness design.
- **Check the callee is side-effect free before any run-both-and-compare A/B**, and where you
  snapshot/restore, **validate the restore** by running the original twice and counting VOID on
  disagreement.
- **A dispatch declared `void` perturbs its callers.** Garbage EAX reaching a caller that gates on
  the return value collapsed a whole child dataset. Dispatches must return the original's value.
- Grep for the **sole `RH_ScopedInstall`** before promoting any row, and put the macro at file scope.
- Shell state does NOT persist between tool calls — set env vars in the SAME command as the run.
- Never `git worktree remove --force`; use `py -3.12 scripts/diag.py wt-remove <path>`.

Ask before spawning a fleet (it re-acquires worktrees and slots), and before anything needing a
human at the keyboard.
