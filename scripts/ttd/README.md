# TTD (Time Travel Debugging) verification lane — feasibility spike

Alternative to the Frida synthetic-A/B `run_diff` promotion lane, targeting the
function classes that defeat it (documented in
`re/analysis/WS_H2_C4_LANE_FINDINGS_2026-06-16.md`):

- **hot paths** — Frida `Interceptor` on >1000 calls/s destabilizes MASHED in ~6 s.
- **nondeterminism** — a live race gives a different elim order/winner each run.
- **destructive callees** — eliminator/result-setup mutate state, so a clean A/B
  needs a call-through trampoline.
- **deep feedback state** — camera/physics chains.

**Idea:** record a deterministic TTD trace of a canonical run *once*, then replay
it offline and extract, for **every** invocation of a target function, the entry
state and exit state. Diff that against the reimpl offline. TTD replays the **real
CPU execution**, so there is no x87/float-fidelity gap (unlike an emulator) — which
matters here given the LUT-vs-`std::sqrt` / `sinf`≠`FSIN` bit-exactness history.

## Tooling status (verified 2026-06-17)

| Component | Status |
|---|---|
| `TTD.exe` recorder (v1.11.592.0, x86) | ✅ present in `Microsoft.WinDbg` AppX; copied to `tools\ttd_x86\` |
| TTD replay engine (`x86/ttd/TTDReplay*.dll`, `amd64/ttd/wow64/`) | ✅ present — 32-bit/WOW64 traces are replayable |
| `Microsoft.WinDbg` (replay UI + data model) | ✅ installed (v1.2603.x) |
| Recording under the MASHED compat shim | ✅ **confirmed 2026-06-17** — 9.9 s attach-at-menu, no crash (`log\ttd\MASHED_*.out`) |
| Headless replay via WinDbgX `-z`/`-c` | ❌ **blocked** — `DbgX.Shell.exe` re-activates as a packaged app and CLI args don't survive AppX activation (engine idle, `-c` never fires). |
| Headless replay via `dbgeng.dll` (Python ctypes) | ✅ **works** — `ttd_query.py` opens the trace, runs `dx`, captures output. No GUI, no MCP. |
| `-module MASHED.exe` selective recording | ❌ **empty trace** — produced "no recorded threads" on attach. **Record full-process.** |

### Gotchas (learned the hard way 2026-06-17)

- **Don't `-module`-scope on attach.** `TTD.exe -module MASHED.exe -attach` yielded a
  trace with *no recorded threads* (`TTD.Calls`/`TTD.Memory` all 0, "selective recording …
  empty trace"). `record_menu.ps1` now records full-process (`-ring -maxFile 2048`).
- **The WinDbgX GUI can't be scripted** (AppX activation eats CLI args). Use `ttd_query.py`.
- **`JSProvider.dll` is absent** from the copied engine, so the TTD *Analyzer* (`!tt`-style
  JS) won't load — but the `dx @$cursession.TTD.*` **data model works without it**, which is
  all the extractor needs. Copy the package's `amd64\winext\` too if JS analysis is ever wanted.

### Headless extraction (the durable lane)

```
py -3.12 scripts\ttd\ttd_query.py log\ttd\MASHED_<ts>.run -f scripts\ttd\diag_cmds.txt
py -3.12 scripts\ttd\ttd_query.py log\ttd\MASHED_<ts>.run -c "dx @$cursession.TTD.Calls(0x4c3b30).Count()"
```

`ttd_query.py` loads the **amd64** `dbgeng.dll` from `tools\ttd_amd64\` (64-bit engine
replaying the x86 trace via `amd64\ttd\wow64\`), opens the `.run`, runs commands, and
captures all engine output via `OpenLogFile` (no COM output-callback object needed).

Recording requires **admin**; the WindowsApps binaries are ACL-blocked in place,
hence the copy to `tools\ttd_x86\` (run `setup_recorder.ps1` to (re)create it).

## How to run the spike

1. **One-time:** `pwsh scripts\ttd\setup_recorder.ps1` (copies recorder out of the AppX).
2. Boot `MASHED.exe` the normal way; leave it **at the main menu, visible**
   (don't minimize — `project_intro_minimize_freeze`).
3. **Capture (elevated — UAC prompt):** `pwsh scripts\ttd\record_menu.ps1`
   → writes `log\ttd\MASHED_<timestamp>.run` (module-scoped to MASHED.exe,
   512 MB ring, ~10 s). Expect MASHED to stutter while recording — that's TTD
   instrumentation, not a hang.
4. **Extract:** open the `.run` in WinDbg and run
   `$$><scripts\ttd\extract_fastsqrt.txt`.

## Go / no-go criterion

The decisive question — *can TTD capture the Frida-hostile hot path?* — is answered
by step [1] of the extraction: `TTD.Calls(0x4c3b30).Count()`. FastSqrt fires
~2,700×/s at the menu, so a ~10 s trace should report **thousands** of calls, each
with a replayable entry/exit position. If yes, the lane is viable and the next
build is `extract_calls.js` (per-call (in→out) CSV dump → offline bit-diff vs the
reimpl), generalized from FastSqrt to the hard targets (camera director
`0x00446520`, physics chain).

## Files

- `setup_recorder.ps1` — copy recorder/replay engine out of the WinDbg AppX.
- `record_menu.ps1` — self-elevating attach-at-menu capture.
- `extract_fastsqrt.txt` — WinDbg command script: census + in/out spot-check.
