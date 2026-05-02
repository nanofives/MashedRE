---
function: entry
rva: 0x004a4bb7
subsystem: boot
confidence: C1
---

# `entry` — `MASHED.exe` entry point

## Evidence (from `mcp__ghidra__symbol_by_name "entry"`, session opened 2026-05-02)

- Address: `0x004a4bb7`
- Source type: `IMPORTED` (Ghidra recognized this as the PE entry point from the binary's optional header)
- Symbol type: `Function`
- Image base: `0x00400000` → entry RVA from base = `0xa4bb7`

## Anchor

`MASHED.exe` SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` (size 2,846,720). Pinned in `re/analysis/GHIDRA_SETUP.md`.

## What we know mechanically

Nothing yet. C1 = located only. The function has not been read end-to-end. Before promotion to C2:
- Decompile the function and document its shape (args, return type, what it reads/writes).
- Identify the MSVC CRT init pattern (this is almost certainly the standard `__scrt_common_main_seh` or pre-`mainCRTStartup` shape from MSVC 6/7).
- Find `WinMain` proper from the path through the CRT init.

## Surrounding evidence (signal that the binary IS MSVC)

The same Ghidra symbol query surfaced mangled MSVC names:

- `?_GetRangeOfTrysToCheck@@YAPBU_s_TryBlockMapEntry@@PBU_s_FuncInfo@@HHPAI1@Z` @ `0x004a3d7e`
- `?CatchIt@@YAXPAUEHExceptionRecord@@...@Z` @ `0x004a8f33`

These are MSVC C++ exception-handling runtime helpers (`_s_TryBlockMapEntry`, `_s_FuncInfo`, `EHExceptionRecord`). Confirms the binary is MSVC-compiled C++ — validates the toolchain choice in `re/TOOLCHAIN.md`.

## Next steps (do NOT speculate further until decomp is read)

1. Open via `ghidra-pool` skill, decompile `0x004a4bb7`.
2. Trace forward through the CRT to find `WinMain`.
3. File any `[UNCERTAIN]` markers in `UNCERTAINTIES.md`.
4. Re-classify when shape is documented (target: C2).
