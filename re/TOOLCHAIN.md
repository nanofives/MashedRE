# Toolchain — Mashed RE

## Build target

`MASHED.exe` is a 32-bit native Windows PE built with **MSVC** (era ~2003–2004, almost certainly Visual C++ 6.0 or 7.1, given the MSVCRT linkage and the era of RenderWare 3.x). Our replacement DLL must be **ABI-compatible** with this binary.

## Decision: MSVC x86 (Visual Studio 2022 Build Tools)

We build `mashed_re.dll` using **`cl.exe` + `link.exe` from VS 2022 Build Tools, x86 target**. No IDE. No vcpkg. No CMake until we actually need it. A single `mashedmod\src\mashed_re\build.bat` that calls `vcvars32.bat` and runs `cl /c` + `link`.

## Why MSVC, not MinGW or Clang

The hooks we install replace `__thiscall` member functions and reach into RenderWare-era C++ classes. Three ABI details have to match the original byte-for-byte:

1. **`__thiscall` calling convention.** MSVC passes `this` in `ecx` for nonvariadic methods. MinGW/GCC supports `__attribute__((thiscall))`, but breaks down on some edge cases (varargs methods, calls through a pointer to member function, return slot for large struct returns).
2. **vtable layout.** MSVC orders virtual functions in declaration order, with multiple-inheritance vtables that include offset-to-top fields different from Itanium-ABI vtables (which is what GCC/Clang non-MSVC mode use). RenderWare classes are usually single-inheritance, but `RpClump` and a few MFC-touching paths use multiple inheritance.
3. **RTTI / name mangling.** MSVC uses its own mangling (`?Foo@CBar@@QAEXXZ`) and RTTI structures (`_RTTI_BaseClassDescriptor` etc.). gta-reversed assumes MSVC RTTI for runtime type checks.

MinGW gets you 90% there cheaply, but the failure mode of the remaining 10% is *silent corruption* — a method with the wrong calling convention crashes a frame later, in another subsystem. Not worth it.

Clang-cl works (it targets the MSVC ABI explicitly) but still requires the MSVC headers + linker, so the install footprint is the same as just using MSVC. No upside for solo work.

## Install (one-time)

1. Download **Visual Studio Build Tools 2022** from https://visualstudio.microsoft.com/downloads/.
2. In the installer, select workload **"Desktop development with C++"**, then under "Individual components" check:
   - MSVC v143 - VS 2022 C++ x64/x86 build tools (latest)
   - Windows 11 SDK (latest)
   - C++ ATL for v143 build tools (x86/x64) — only if we need ATL (probably not).
3. Verify install:
   ```cmd
   "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
   cl.exe /?
   link.exe /?
   ```
4. The eventual `build.bat` will lazily call `vcvars32.bat` if `%VSCMD_VER%` isn't set.

## Compiler flags (when first hook lands)

Baseline:
```
cl /c /nologo /MD /O2 /Zi /Zc:__cplusplus /std:c++17 /W4 /WX
   /DWIN32 /D_WIN32 /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS
   /I deps\rwsdk\include
   foo.cpp
```

Linker:
```
link /DLL /MACHINE:X86 /NOLOGO /SUBSYSTEM:WINDOWS
     /OUT:mashed_re.dll
     /DEBUG /PDB:mashed_re.pdb
     /DEF:mashed_re.def
     foo.obj kernel32.lib user32.lib
```

`/MD` matches the original binary's CRT linkage (dynamic). `/Zc:__cplusplus` makes the `__cplusplus` macro report the right value. `/std:c++17` is enough — no need to chase newer standards.

## Why no CMake (yet)

A single-DLL build with one source tree doesn't need CMake. Adopt CMake when:
- We split into multiple libraries.
- We add unit tests that need their own targets.
- We need to support a second platform (we won't).

## Tooling helpers (MinGW kept around for these)

A 32-bit MinGW is convenient for non-ABI-bound work:
- Quick `gcc -E` macro-expansion checks.
- `nm`, `objdump`, `strings` against the original binary.
- Building helper command-line tools that don't link against game classes.

If/when MinGW is needed, vendor a fresh copy under `mashedmod\deps\mingw\` (don't reach into `TD5RE\`). Until then, MSVC alone is sufficient.

## Versioning

- VS Build Tools 2022 + MSVC v143: pinned at install time, recorded in `re/analysis/GHIDRA_SETUP.md` (or a sibling `re/analysis/BUILD_SETUP.md` once tools land). Update the pin every time VS auto-updates and verify hooks still pass `diff-original`.
