# Injection mechanism — dev-time verification harness only

## Project intent: greenfield, not hook-and-replace

The end state is a **standalone `mashed_re.exe`** that loads Mashed's data files and runs without `MASHED.exe`. This is the same pattern as **re3** (GTA III port) and the user's own **TD5RE**. We are *rewriting the game from scratch*, not augmenting it.

Therefore: injection is a **development-time verification harness**, not the shipping architecture. By v1, no part of `MASHED.exe` is in the loop.

## Why we still need injection during dev

A reversed function is only "verified" when its behavior matches the original under realistic game state. That requires running both versions over the same data and diffing observable state.

Two ways to do that:

1. **Frida-only.** Attach Frida to a clean run of `MASHED.exe`, dump args/return/memory at the target RVA. Then write a unit test in `mashed_re.exe` that calls our reimplementation with the same inputs, dump the same fields, diff CSVs.
   - Pro: zero new code in the original process.
   - Con: requires us to *recreate* the input game state in the test — which for anything past leaf utilities is impossibly tedious. Game state is hundreds of pointers, RW handles, RNG, frame counters.

2. **Hook + Frida.** Inject a dev-only DLL into the live `MASHED.exe`, hook the target function so our reimplementation runs in place, Frida-trace both modes and diff.
   - Pro: the *real* game state drives the test — no recreation needed.
   - Con: requires the injection mechanism (this document).

We use **both**. Leaf utilities (math, string compare) — Frida-only. Anything that touches game state — hook + Frida.

## The dev harness: `mashed_re_dev.asi` + Ultimate-ASI-Loader

```
original\
├── MASHED.exe                    (untouched)
├── d3d9.dll                      ← Ultimate-ASI-Loader (dev only; in .gitignore)
├── mashed_re_dev.asi             ← our reversed code in HOOK MODE (dev only; .gitignore)
└── ...
```

- **Ultimate-ASI-Loader** sits in front of `d3d9.dll`. Standard, BSD-licensed, single-DLL.
- **`mashed_re_dev.asi`** is a separate build configuration of `mashed_re` that compiles the reversed functions as a DLL and installs them as JMP-hooks over `MASHED.exe`'s originals. Same source files as the standalone exe; different entry point and a `#define MASHED_RE_HOOK_MODE`.
- Each hook is **runtime-toggleable**, so `diff-original` can A/B test inside the same process: run scenario with hook off, dump trace; turn hook on, run again, diff.
- Neither file is in the repo or in any release artifact. They live only in dev environments.

## The shipping product: `mashed_re.exe`

```
release\
├── mashed_re.exe                 ← standalone, no original needed
├── *.dll                         (CRT, RenderWare or replacement renderer)
└── data\                         (extracted Mashed assets, repackaged or referenced)
```

The shipping exe has its own `WinMain`, its own asset loaders (built from the Phase 2 tools — `piz_extract.py` etc.), its own renderer (RenderWare3 if licensable, or a D3D11 replacement à la TD5RE). The reversed functions land here as plain C++ — no hooks, no JMP patches.

## Build configurations

We have one source tree, two build targets:

| Target | Output | Mode | Purpose |
|---|---|---|---|
| `mashed_re.exe` | EXE | greenfield | shipping; standalone game |
| `mashed_re_dev.asi` | DLL (renamed) | hook | dev-time verification only |

Both share the same `mashed_re/` source. The hook target adds a thin `hooks/` directory (the inline-JMP installer + per-function `RH_ScopedInstall(Foo, 0xRVA)` registrations). The exe target ignores `hooks/`.

## Lifecycle of a function

1. **Reverse it** in Ghidra → C2.
2. **Reimplement** in `mashed_re/<Subsystem>/Foo.cpp`. Compiles into both targets.
3. **Verify under dev harness:** build `mashed_re_dev.asi`, drop into `original\`, run game with hook on, Frida-diff vs hook off → C3 → C4.
4. **Promoted** in `hooks.csv`. The function now appears in both targets, but only ever *runs from a hook* in the dev target.
5. When the standalone target boots far enough that it can run the same scenario without `MASHED.exe`, the dev harness is no longer needed for that subsystem.

## What gets dropped at v1

- Ultimate-ASI-Loader: not shipped.
- `mashed_re_dev.asi`: not shipped.
- The inline-JMP installer: builds only into the dev target; absent from the shipping exe.
- `MASHED.exe`: not redistributed (it's the user's own game install).

The shipping exe links the same reversed code, statically, with no patching. Verification methodology is dev infrastructure, not product.

## Hook installer mechanism (only matters during dev)

For the dev harness only. Following gta-reversed: a custom **inline-JMP installer**, ~200 lines of C++. For each registered hook:

1. Compute `target = module_base(MASHED.exe) + RVA`.
2. `VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &old)`.
3. Write `0xE9` + `(replacement - target - 5)`.
4. `VirtualProtect(target, 5, old, &old)`.
5. Track in a registry so the toggle UI can flip individual hooks on/off.

Saved-prologue bytes let `diff-original` enable/disable a single hook without restarting the game.

## Anti-patterns

- Designing the standalone exe around hook-mode requirements. The hook is throwaway.
- Putting hook-only code (`#ifdef MASHED_RE_HOOK_MODE`) in subsystem source files outside obvious bridge points. Keep the harness in `mashedmod/src/mashed_re/hooks/`; subsystem code stays target-agnostic.
- Patching `MASHED.exe` on disk. Forbidden — even for tests.
- Distributing `d3d9.dll` (Ultimate-ASI-Loader) or `mashed_re_dev.asi` outside dev. They are tools, not artifacts.
