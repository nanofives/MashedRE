# RESULT D4-FU-MENUANIM: exe menu animation 6x (`kMenuAnimSpeedup`), evidence for a keep/drop decision

Verdict: **DECISION**. No default changed, no exe or `.asi` rebuilt, no gate run (nothing it could move).
Original-side wall-clock measurement is **BLOCKED** in this session (section 3). The numbers below are a
model that runs the verbatim clock and tick math of both bodies, not an original capture.

## 1. The knob

`mashedmod/src/mashed_re/Frontend/MenuNavSM.cpp:819` `constexpr float kMenuAnimSpeedup = 6.0f;`, used at `:822`
(`base = (g_anim_phase >= 4 ? 2.0f : 1.0f) * kMenuAnimSpeedup`). Origin: user feedback 2026-06-13 "animations are too
slow" (comment `:812-818`). It multiplies the per-tick step of the K-based slide classes only. The default-tag float
path (`f8 += dt * 300.0f` to 180.0f, `:807-809`) has no base multiplier in the original (`0x00432751`) and none in the
exe, so it is **not** sped up. Only the exe has the knob. The `.asi` twin (`MenuAnimTickTwin.cpp`) runs base 1, which is why
`0x004325c0` stays two bodies (D4-WSH-EXEFILE RESULT section 11.5, `verify/wshexe_20261026/`).

## 2. Settle time, wall clock (`anim_settle_model.py`, output `settle_model.json`, SHA-256 `2eda2fa9e9c70dec...`)

Clock: `FrameClockUpdate` (`0x00493480`, ticks of 50 ms, `dt = ticks * 50 / 3000`, so game dt runs at 1/3 of wall
time at 60 fps). Step: `trunc(dt * base * K)` (`0x004a2c48` truncation). Start slide `0x1ff` (511), settle at `<= 0`.

| Class (K) | base 1.0 (original, phase < 4) | x6 exe | ratio | base 2.0 (phase >= 4) | x6, phase >= 4 |
|---|---|---|---|---|---|
| item `0xff040000` (-2000), 60 fps | 800 ms (48 frames) | 150 ms (9) | 5.3 | 400 ms | 100 ms |
| slide class (-800), 60 fps | 2000 ms (120) | 350 ms (21) | 5.7 | 1000 ms | 200 ms |
| float path (+300 to 180), 60 fps | 1850 ms | 1850 ms (no base) | 1.0 | n/a | n/a |

Frame-rate spread: item rows 800 / 800 / 778 ms at 30 / 60 / 144 fps, x6 exe 167 / 150 / 146 ms. The ~0.77 s in the
source comment matches the item-row figure. The slide class takes 2.0 s in the original at 60 fps, which is the
slowest visible class. After x6 the exe finishes item rows in 9 frames and the slide class in 21 frames, while the
float-path elements (prompt class, `0xff080000` per the comment) still take 1.85 s, so a screen change now shows
fast rows next to slow prompt/default-tag elements.

Link to measured evidence: D4-WSH-EXEFILE ran the original tick against the `.asi` twin (base 1, truncating after
the L-32 flip) GREEN 20/20 on game dt, 1 tick and 2 ticks (`verify/wshexe_20261026/RESULT_D4-WSH-EXEFILE.md` section 11,
run 12). The exe body differs from the twin only by the x6 base (and its own globals), so the base-1 column is
the original's timing and the x6 column is the exe's.

## 3. What was not measured, and why

Goal was original vs standalone wall-clock menu timing. Not obtained:
- `re/frida/menu_anim_diff.py` (the existing control) and my observe-only attach `orig_anim_timing.py` both die with
  `script has been destroyed` about 3.5 s after `frida spawn` of `MASHED.exe`, in the private root and in the
  D4-WSH-EXEFILE root, under `mashed_lock.py` and test-slot. A Frida-free `Start-Process` of the same exe stays
  alive 8 s. So spawn-under-Frida is broken in this session on this PC, not the script.
- Passive fallback `orig_anim_poll.py` (ReadProcessMemory on the record table `0x00898ac0` and phase `0x0067eca4`,
  no injection) saw 1 table state in 25 s and in 60 s, phase stayed 0: the original did not reach the menu there.
  Both runs are listed below.
- Standalone side: no existing dump of the animation table, and adding one is a code change this step does not allow.

Re-pickup: a session where `menu_anim_diff.py` runs (A0 GREEN there on 2026-10-26), then `orig_anim_timing.py`
(committed here, untested past boot) gives per-tick wall ms and slide values for the original. The model's 800 ms / 2000 ms
figures are the prediction it must hit.

## 4. Raw data (not committed), `D:/MashedWT/D4-FU-MENUANIM_data/`

| File | SHA-256 |
|---|---|
| `poll_run1.json` | `8671d922761974f20f466fff03a6d8a1b9eef86ff4e995c92fb57fdf14ec08bd` |
| `poll_run2.json` | `59c7a54c9eb01620459ee1e73f145c5493ff8743083ce2845c13969b31353a6b` |

## 5. Decision filed

Keep the x6 or drop it to 1.0? Options:
1. Keep 6.0 (status quo). Fast menus, but the exe does not match the original timing, `0x004325c0` stays two bodies
   (blocks consolidation, allowlist burn-down) and float-path elements stay slow beside fast rows.
2. Drop to 1.0. Exe timing equals the original (item 800 ms, slide class 2000 ms), one body becomes possible, but this
   reverts the 2026-06-13 user request.
3. Keep a speedup as an opt-in `MASHED_MENU_ANIM_SPEED` knob, default 1.0 (faithful), 6.0 selectable. Consolidation
   unblocked, the feel stays available. Nearly free, gates unaffected (menu-only, no race path).
Recommendation: 3 with a shorter default if Mariano wants it faster (about 2x gives item rows 400 ms), because the
original's slide class at 2.0 s is the reason for the 2026-06-13 complaint and a flat 6x also overshoots the float
path inconsistency.
