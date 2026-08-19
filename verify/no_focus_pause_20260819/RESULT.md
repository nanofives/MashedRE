# `no_focus_pause` — measured proof, 2026-08-19

**Question:** does `original/MASHED.exe` keep rendering while its window is not focused?

**Metric:** the d3d9 shim appends one `present_fps=...` line per second to
`log/fps_limiter.txt` when `MASHED_FPS_LOG=1` (`d3d9_shim.cpp:386-391`). Lines
accumulating means `Present()` is being called, which means frames are being produced. If
the main loop is parked in `WaitMessage()` the count freezes.

**Why minimize rather than "click elsewhere":** minimizing the window is deterministic and
leaves no ambiguity about who owns the foreground. The harness asserts it — it prints
whether `GetForegroundWindow()` is the game, and in both arms it was **not**.

## Result

| arm | `present_fps` lines gained while minimized (15 s) | reading |
|---|---:|---|
| **patched** (`0x996d3` = `0xEB`) | **15** | 1/s — full rate, uninterrupted |
| **unpatched** (`0x996d3` = `0x75`) | **1** | frozen |

**15× difference.** Both arms booted normally, both were still alive at the end, and both
were confirmed non-foreground. The single line the unpatched arm gained is the partial
second before the window was minimized, plus whatever stray window message briefly woke
`WaitMessage()`.

This is the behaviour the patch claims, measured rather than argued.

## Reproduce

```
py -3.12 re/tools/focus_pause_check.py PATCHED
py -3.12 scripts/patch_mashed_no_focus_pause.py --restore
py -3.12 re/tools/focus_pause_check.py UNPATCHED
py -3.12 scripts/patch_mashed_no_focus_pause.py          # re-apply
```

## State after the run

Restored to the adopted (patched) state and verified:

- `original/MASHED.exe` sha256 `b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87`
- 112 bytes differ from `.unpatched`, byte at `0x996d3` is `0xeb`
- `original/MASHED.exe.unpatched` untouched, still `BDCAE093…`

## Process hygiene

Both arms tracked their own PID and terminated only that one. No kill-by-name — a blanket
kill destroyed another session's capture on 2026-06-17. `WaitForExit` precedes every exit-code
read, because the boot AV and a force-kill share the `0xFFFFFFFF` signature.
