---
name: hook-author
description: Author a new gta-reversed-style hook for MASHED.exe — generate the C++ skeleton, RVA-cite the target function, register through InjectHooks, and pin the version anchor. Use when the user says "hook X", "reimplement function Y", "replace the original sub_…", "scaffold a hook for 0x….".
---

# Mashed hook-author skill

Mashed RE follows the **gta-reversed methodology**: a single 32-bit DLL is loaded into `MASHED.exe`, and individual functions are replaced one at a time with reimplementations that match the original's behavior byte-for-byte (or at least observably). Hooks use a custom inline-JMP installer; each is **runtime-toggleable** so we can A/B reversed-vs-original behavior.

## Version anchor — VERIFY EVERY TIME

Before authoring any hook, confirm the target binary matches:

```
MASHED.exe size  : 2,846,720 bytes
MASHED.exe SHA256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
```

If sizes/hashes diverge, the user has a different build (Steam vs disc vs patched) and ALL RVAs are wrong. **Stop and report — do not write code against an unknown binary.**

## Standard hook scaffold

For a target at `0x00xxxxxx` named `CFoo::Bar` in `mashedmod/src/mashed_re/`:

```cpp
// CFoo.h
#pragma once
#include "rwsdk/rwcore.h"

class CFoo {
public:
    // 0x00xxxxxx
    void Bar(int arg);

    static void InjectHooks();
};
```

```cpp
// CFoo.cpp
#include "CFoo.h"
#include "../hooks/RH_Hook.h"

void CFoo::Bar(int arg) {
    // Reimplementation. Decompiled at 0x00xxxxxx.
    // Field offsets from Ghidra master at 0x00xxxxxx + 0xNN
    // ...
}

void CFoo::InjectHooks() {
    RH_ScopedClass(CFoo);
    RH_ScopedCategory("Foo");
    RH_ScopedInstall(Bar, 0x00xxxxxx);
}
```

## Workflow when invoked

1. **Confirm binary anchor** — run `Get-FileHash` on `original\MASHED.exe`. If it doesn't match, abort.
2. **Acquire a Ghidra slot** via the `ghidra-pool` skill.
3. **Pull the decomp** with `mcp__ghidra__decomp_function` for the RVA the user gave.
4. **Read original behavior literally** — apply NO-GUESSING. Field offsets cited via `+0xNN` comments. Never paraphrase semantics.
5. **Generate the scaffold** above; place it under `mashedmod/src/mashed_re/<Subsystem>/<Class>.cpp`.
6. **Update `hooks.csv`** — append a row `RVA, ClassName::Method, status=todo|wip|done, file`.
7. **Register in InjectHooks chain** — add `CFoo::InjectHooks()` call from the parent subsystem registrar.
8. **Release the Ghidra slot.**

## Naming & style (lifted from gta-reversed)

- `m_` instance field, `ms_` static member, `g_` global, `s_` file-static
- `PascalCase` for types, `CamelCase` for methods, ALL_CAPS for macros
- Inline-comment **every** RVA: `// 0x00xxxxxx` directly above the function
- Use RW types verbatim (`RpClump*`, `RwTexture*`, `RwFrame*`) — copy headers from `gta-reversed-modern/source/RenderWare/`

## Diff verification (handoff to `diff-original` skill)

Once a hook is written, the next step is the `diff-original` skill: enable the hook, disable it, capture both runs with Frida, compare. A passing diff is the only acceptance criterion. Code that compiles but diverges from the original is a bug.

## Anti-patterns

- Adding a hook without an RVA citation → reject.
- Speculating on a field's meaning from name alone → reject; cite the address that reads/writes it.
- Implementing more than the original function does → reject; minimal byte-for-byte port first, refactor later.
- Bumping C++ standard or pulling a new dependency for one hook → reject.
