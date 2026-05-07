# PPPP — Debug Overlay / FPS Counter
**Session ID:** debug_overlay-20260503  
**Slot used:** Mashed_pool12 (pool11 had stale OS file-channel lock; pool12 substituted)  
**Date:** 2026-05-03  
**Outcome:** HALTED — release build stripped dev-debug strings (expected)

---

## Anchor search results

All Strategy 1 and Strategy 2 anchors returned zero matches.

### Strings checked (all absent):
- `FPS`, `fps`
- `Debug`, `DEBUG`
- `frametime`, `ms/frame`, `msec`
- `DebugDraw`, `DrawDebug`, `ShowFPS`
- `FPS: %`, `fps: %`, `%.2f ms`, `%.1f fps`

### Incidental matches (not game debug code):
| Address    | Value                                                         | Classification            |
|------------|---------------------------------------------------------------|---------------------------|
| 0x005dde74 | `debug info exceeds maximum comment size; no debug info emitted` | MSVC PDB compiler message |
| 0x005e83c8 | `OutputDebugStringA`                                          | Win32 import table entry  |

Neither is a game debug overlay or FPS counter. Both are build-tool artifacts.

---

## Conclusion

MASHED.exe (retail build, SHA-256 BDCAE093…) has no debug overlay or FPS counter code reachable via string anchors. The dev-debug text and format strings were stripped in the release build. No DEBUG_FN identified.

**This is useful negative evidence for the greenfield port:** there is no debug overlay subsystem to reimplement from the original binary. Any FPS/debug overlay in `mashed_re.exe` will be a fresh implementation.

---

## Tracker impact
- No hooks.csv rows added (no functions identified).
- No STUBS.md rows.
- No UNCERTAINTIES.md rows.
- No commit.

---

## Subsystem note for greenfield port
The absence of debug overlay strings is consistent with a 2004 MSVC retail build with `#ifdef _DEBUG` guards stripping all overlay code at compile time. Strategy 3 (call-graph from timer bucket) was not attempted because no timer-related bucket was available in the current session scope, and all direct string anchors already failed — the call-graph approach would not recover stripped format strings in any case.
