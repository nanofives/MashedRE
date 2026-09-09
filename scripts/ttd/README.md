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

## `asi:<Export>` backend (added 2026-09-09) — the real reimpl diff

`ttd_reimpl_diff.py` used to carry only pure-Python stand-in backends, and its own
docstring flagged the gap: "For a TRUE reimpl diff, add an `asi:<export>` backend."
That backend now exists.

    py -3.12 scripts/ttd/ttd_reimpl_diff.py log/ttd/calls_004c3b30.csv --reimpl asi:FastSqrt

It spawns MASHED **muted**, lets the dinput8 proxy auto-load `mashed_re_dev.asi`, polls the
session-phase global until the RW engine is up, resolves the export, and calls it once per
captured input through Frida. One boot for the whole CSV, not one per call.

Three project constraints are honoured in code, each a scar from an earlier session:

* **Never `Module.load` the already-auto-loaded `.asi`** (memory `no-explicit-module-load-asi`
  — a double load corrupts state). `Process.findModuleByName` first; `Module.load` only as a
  last resort, and the status line says which happened (`ok-auto@` vs `ok-loaded@`).
* **Launch muted** (`MASHED_MUTE=1`, memory `always-launch-muted`).
* **Kill only the spawned PID**, never by image name (memory `multisession-mashed-kill-by-pid`).

The scratch buffer is held in a module-scope JS var so Frida cannot reclaim it
(memory `frida-keepalive-scratch-buffers`).

### First result (2026-09-09)

`asi:FastSqrt` vs the 2026-06-17 capture of `0x004c3b30`: **128/128 bit-identical**,
phase=1 reached in 7.0 s.

**Read that with its bound:** the capture holds 128 calls but only **8 distinct inputs**, so
it is 8 values matched 16 times each, not 128 independent samples. The tool now prints the
distinct-value domain on every run for exactly this reason, and calls out a domain with
fewer than 2 distinct values as DEGENERATE.

It is also **path1** — the export is called directly, which does not prove the inline-JMP is
installed (memory `path1-green-does-not-prove-install`). Not an automatic C4.

### Harness non-degeneracy, proven not assumed

* wrong export (`asi:NoSuchExport`) → `init failed: no-export` + a module dump; it exits
  before comparing, so it cannot report a false PASS.
* a real but *different* export (`asi:FastInvSqrt` against the FastSqrt capture) →
  **9/128** bit-identical, 119 divergent, max 108,865,074 ULP. The harness distinguishes a
  correct implementation from an incorrect one.

### Still deferred: TTD RECORDING

This finishes the **replay/diff** half only. Producing NEW captures still needs the
recording lane deferred on 2026-07-17, so today the backend can only consume
`log/ttd/calls_004c3b30.csv`. Widening that capture's 8-value domain, and capturing other
RVAs, is the next constraint on this lane — not the diff code.

## RECORDING IS BLOCKED ON THIS MACHINE (2026-09-09) — Defender ASR

The un-deferral was attempted and **failed**. Recording no longer works here, and the cause
is machine policy, not the tooling:

> Accion de riesgo bloqueada — El administrador bloqueo esta accion.
> Aplicacion o proceso bloqueado: **TTD.exe**
> Bloqueado por: **Reduccion de la superficie expuesta a ataques**
> Regla: **Bloquear el uso de herramientas del sistema copiadas o suplantadas**
> Elementos afectados: `...\Mashed\tools\ttd_x86\TTDInject.exe`

The machine became organization-managed (RoboticCrew / Project Foodbox, for Teams + company
files), so this ASR rule is now enforced by policy and cannot be turned off locally.

**Why this rule hits precisely this lane:** `setup_recorder.ps1` *copies* the recorder out of
the WinDbg AppX into `tools\ttd_x86\` because the WindowsApps ACLs block running it in place.
"Copied system tool" is exactly what the rule is written to stop. The lane's one workaround
is the thing that trips it.

Symptom in the transcript: TTD reports an empty trace name and writes no `.run`, while the
nav step succeeds and prints a valid `RACE_PID`.

### Ways forward, in order of cost

1. **Use TT-11 (`Core/ShadowAB.h`) instead.** It needs no external instrumentation tool at all
   — it is compiled C++ inside the `.asi`, so it presents no ASR surface. The ASR block makes
   the shadow-A/B lane *more* valuable, not less.
2. **Ask the org admin for an ASR exclusion** for `tools\ttd_x86\` (path exclusion) or for the
   `Block use of copied or impersonated system tools` rule. Needs whoever manages the device.
3. Re-verify whether the AppX copy can be avoided entirely (run the recorder in place / via its
   execution alias). The README's original note says WindowsApps ACLs prevent this; that was
   recorded 2026-06-17 and has not been re-tested under the new policy.

Until one of those lands, **`log/ttd/calls_004c3b30.csv` (8 distinct inputs) is the only
capture this project has**, and the `asi:<Export>` backend is bounded by it.

### capture_race.ps1 reported a FALSE SUCCESS — fixed

Worth recording because it is the more dangerous half. When TTD wrote nothing, the script
still printed `DONE -> MASHED_2026-06-17_221159.run  size: 2,048.0 MB` and a ready-to-run
`extract_calls.py` line: it selected "the newest `.run` in the output directory" without
checking that the file was **new**, so it handed back a trace from three months earlier.
Feeding that into the diff lane would have produced "fresh" evidence from a stale tape.

Fixed 2026-09-09: the script snapshots the pre-existing `.run` set before recording, accepts
only a file absent from that set, and otherwise exits 4 with the ASR diagnosis above.

