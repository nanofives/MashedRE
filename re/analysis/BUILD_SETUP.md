# Build Setup

Pinned record of the local build toolchain. Re-verify when VS auto-updates.

## Installed (verified 2026-05-02)

| Item | Pinned value |
|---|---|
| Visual Studio | **2022 Build Tools** (no IDE) |
| Install path | `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools` |
| `vcvars32.bat` | `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat` |
| MSVC linker | v14.44.35223.0 (VS 17.14) |
| Target architecture | x86 (32-bit, matches `MASHED.exe`) |
| C runtime | MSVCRT (`/MD`, dynamic) — matches the original binary linkage |

## Quick-start for a build session

From any shell:

```cmd
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
cl.exe /?
link.exe /?
```

In a `build.bat` (when first authored under `mashedmod\src\mashed_re\`):

```bat
@echo off
if not defined VSCMD_VER (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
)
cl /c /nologo /MD /O2 /Zi /std:c++17 /W4 ...
link /DLL /MACHINE:X86 /NOLOGO ...
```

## Re-verify after VS updates

```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" && link.exe
```

If linker version changes, re-run any active C4-tier hook through `diff-original` to confirm no codegen drift broke verified behavior.

## Open questions (defer to Phase 4)

- Exact `/std:c++` level (17 vs 20). Default planned: C++17.
- Whether to enable `/Zc:dllexportInlines-` for hook-mode build. Default planned: yes (matches MSVC 6/7-era inline export semantics for the original binary's exports).
- Whether `mashed_re_dev.asi` and `mashed_re.exe` share a single .def file or split.
