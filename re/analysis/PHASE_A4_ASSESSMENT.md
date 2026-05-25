# Phase A4 — 6 boot-crashers reimpl assessment

Authored 2026-05-25. Tracks the 6 RVAs called out in `FRONTEND_ROADMAP.md`
§ Phase A.4: "Re-instate the 6 boot-crashers carefully".

Current status per RVA:

## Re-enabled this session (2 of 6)

### 0x00493550 — thunk_EngineStopDispatch
- **File:** `mashedmod/src/mashed_re/Boot/LaunchHandshake.cpp:103`
- **Body:** 5-byte JMP rel32 → `0x004938c0` (FUN_004938c0 / EngineStopHelper, C2).
- **Reimpl shape:** 1:1 passthrough — `extern "C" void thunk_EngineStopDispatch() { s_FUN_004938c0(); }`.
- **C3 evidence (already in hooks.csv):** `log/diff_engine_stop_dispatch.csv` Frida A/B GREEN 10/10 teardown_call_pair.
- **Action:** re-enabled 2026-05-25 a4-thunk-passthrough.
- **Verification deferred:** `.asi` rebuild pending — MASHED.exe was running at re-enable time (locked the link target). Next clean MASHED-stopped build will produce the rebuilt .asi with this hook live.

### 0x00493560 — thunk_HwExitDispatch
- **File:** `mashedmod/src/mashed_re/Boot/LaunchHandshake.cpp:128`
- **Body:** 5-byte JMP rel32 → `0x004954f0` (returns 0 unconditionally; ShowCursor(1) gated on FUN_00498bf0; 5 minor cleanup calls).
- **Reimpl shape:** 1:1 passthrough — `extern "C" std::uint32_t thunk_HwExitDispatch() { return s_FUN_004954f0(); }`.
- **C3 evidence (in hooks.csv):** "C2->C3 via implicit-survival sweep 2026-05-23" — installed via RH_ScopedInstall + MASHED reached menu + 30 s idle survived. Note the implicit-survival sweep memory `[[project-implicit-survival-sweep]]` was INVALIDATED 2026-05-24 because the loader was broken in that window. Re-verification needed.
- **Action:** re-enabled 2026-05-25 a4-thunk-passthrough.
- **Verification deferred:** same as above.

## Re-enable candidate (1 of 6) — needs canonical-observation re-run

### 0x004924f0 — DataZeroFill
- **File:** `mashedmod/src/mashed_re/Boot/SubsystemInit.cpp:451`
- **Body:** zero-fills `0xDCE9` DWORDs at `DAT_007f0f60` (~230 KB of live game state). Single side-effect; no branch.
- **Reimpl shape:** 1:1 passthrough — `extern "C" void DataZeroFill() { s_orig_DataZeroFill(); }`.
- **C3 evidence (in hooks.csv):** `boot-to-menu-canonical / log/mass_canonical_observe.txt` — canonical-observation 2026-05-23 count=1 GREEN 30 s menu. INSIDE the broken-loader window — needs re-validation post-dinput8-fix.
- **MASS-DISABLE reason:** `c3-boot-hangs` (per file comment). Suggests the hook was observed to hang MASHED during boot at some point. Conflict with C3 canonical-observation evidence.
- **Recommended next step:** sandbox re-enable + canonical-observation run with current dinput8 loader. If GREEN → keep enabled. If hang reproduces → demote hooks.csv row C3→C2 and document the hang condition.
- **Risk:** zeroes 230 KB of live state on every invocation. If our reimpl is byte-equivalent the runtime impact is zero, but bookkeeping is brittle. Do NOT re-enable without test.

## Blocked — needs harness work (1 of 6)

### 0x00431d00 — CarSelectReset
- **File:** `mashedmod/src/mashed_re/Frontend/GameModeCarSelect.cpp:103`
- **Status (hooks.csv):** C2 mapped; "C3-attempt blocked: FUN_00431b80 hangs at quiescent state (ESI=0 → infinite loop); see U-1655".
- **Body:** 7-global init + 3× FUN_00431b80; car-select state reset.
- **Blocker:** U-1655 — FUN_00431b80 enters an ESI-driven loop that doesn't terminate when called from a non-car-select menu state. The hook itself doesn't crash; the synthetic A/B caller in `run_diff.py` hangs.
- **Recommended path:** add an arg_type `seed_carselect_state` in `re/frida/diff_template.js` that pre-seeds the ESI source global (likely the car-select dispatcher state) before invoking; teardown restores. Once GREEN, promote C2→C3, re-enable.
- **Estimated effort:** 1 session (read FUN_00431b80, identify ESI source, write arg_type, run diff).

## Blocked — `a4-boot-crasher-canonical-only` (3 of 6)

These were tagged specifically for Phase A4 because their reimpls can't be
verified via the standard synthetic Frida A/B path — they have to run from
the actual game-boot entry, not from a sampled snapshot.

### 0x004a31f3 — CrtPreInit
- **File:** `mashedmod/src/mashed_re/Boot/CrtStartup.cpp:256`
- **Body summary (per existing reimpl comments):** Iterates 7 fn-ptr slots
  at `0x005ea03c..0x005ea058`; conditional atexit registration via
  `_atexit` (0x004a415e); reentrancy guarded via `0x00616044`.
  Calls library helpers `__lock` (0x004a787f), `___crtExitProcess`
  (0x004a31b1), `FUN_004a77eb`. Modifies the CRT init-state machine.
- **Why it can't use synthetic A/B:** the function is called exactly
  once per process boot — by the time MASHED has reached steady-state,
  the init table has already been consumed (each slot fires once and
  the entry is overwritten to a no-op pointer). Synthetic re-invocation
  would either no-op or double-register an atexit handler.
- **Recommended path:** canonical-only observation. Add a one-shot
  counting Interceptor on `0x004a31f3`; install our reimpl; spawn
  MASHED with `frida.spawn` (suspended); resume; observe that the count
  is exactly 1 and MASHED reaches the menu.
- **Risk:** if our reimpl mis-orders the init table walk, the entire CRT
  initialization corrupts subsequent process state.
- **Estimated effort:** 1 session.

### 0x004a3258 — CrtExitCore
- **File:** `mashedmod/src/mashed_re/Boot/CrtStartup.cpp:338`
- **Body summary:** Mirror of CrtPreInit — walks the exit-callback
  tables `0x005ea000..0x005ea038`, runs `_atexit` chain, sets reentry
  guard at `0x007739dc`.
- **Why it can't use synthetic A/B:** same reason — single-shot at
  process exit; synthetic calls would corrupt the exit chain.
- **Recommended path:** same as CrtPreInit, but observed at process
  shutdown (graceful exit via menu Quit). Trickier to observe
  cleanly because MASHED exit-paths are not always graceful.
- **Estimated effort:** 1 session.

### 0x004a4bb7 — WinMainEntry
- **File:** `mashedmod/src/mashed_re/Boot/CrtStartup.cpp:535`
- **Body summary:** mainCRTStartup-equivalent. SEH frame setup,
  command-line parsing, locale init, then calls user-level `WinMain`.
  Returns `int` from the inner `WinMain` then chains to ExitProcess.
- **Why it can't use synthetic A/B:** single-shot per process; SEH
  frame magic that doesn't survive snapshot/restore.
- **Recommended path:** observation-only, same as CrtPreInit. The
  reimpl needs SEH-equivalent prologue/epilogue handling that we
  haven't yet validated cross-process.
- **Risk:** SEH chain corruption would crash MASHED before menu init.
  Critical to verify.
- **Estimated effort:** 1-2 sessions (SEH research + observation).

## Summary

| RVA          | Status this session | Next step |
|--------------|---------------------|-----------|
| 0x00493550 (thunk_EngineStopDispatch) | RE-ENABLED (build pending) | Rebuild .asi when MASHED stopped; canonical-observe |
| 0x00493560 (thunk_HwExitDispatch)     | RE-ENABLED (build pending) | Same |
| 0x004924f0 (DataZeroFill)             | Stays disabled              | Sandbox re-enable + canonical-observe (re-validate post-dinput8) |
| 0x00431d00 (CarSelectReset)           | Stays disabled              | Resolve U-1655 + write seed_carselect_state arg_type |
| 0x004a31f3 (CrtPreInit)               | Stays disabled              | Canonical-observation-only re-validation |
| 0x004a3258 (CrtExitCore)              | Stays disabled              | Canonical-observation-only (at process exit — non-trivial) |
| 0x004a4bb7 (WinMainEntry)             | Stays disabled              | SEH research + canonical observation |

Net: 2 of 6 re-enabled this session pending build verification; 4 remain
with clearly-documented next steps. Phase A4 is not closed but has been
de-mystified — each remaining RVA has a specific blocker and a written
unblock path.

## Cross-references

- `FRONTEND_ROADMAP.md` § Phase A.4
- `re/CONFIDENCE.md` (C2 vs C3 evidence requirements)
- Memory: `[[project-loader-broken-9d-audit]]` (why 2026-05-23 C3 evidence is suspect)
- Memory: `[[project-loader-split-dinput8]]` (loader fix that unblocks re-test)
