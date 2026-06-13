# Console-build symbol harvest — PS2 + Xbox (2026-06-12)

User supplied two retail console images of Mashed: Fully Loaded:

| image | format | size | extracted executable |
|---|---|---|---|
| `xbox/Mashed Fully Loaded.bin/.cue` | PS2 CD, MODE2/2352 | 526,923,264 | `SLES_531.52` ELF, 4,221,020 bytes, SHA-256 `F71CD5E6D9C6B43899419194AFF2A5C6E4C61EA93FD3F18AF45DAF8E10459F6A` |
| `xbox/Mashed_XBOX.iso` | XDVDFS (XISO) | 466,944,000 | `default.xbe` (`toast.exe`), 2,965,504 bytes |

Extraction tools (pure stdlib, no new deps): `re/tools/console/ps2_bincue_extract.py`,
`re/tools/console/xiso_extract.py`, `re/tools/console/xbe_flatten.py`.

## Finding 1 — both retail builds are symbol-stripped

- PS2 ELF has 47 section headers (`.text`, `.vutext`, VU overlay tables, `.data`,
  `.rodata`, …) but **no `SHT_SYMTAB`/`.strtab`**. Retail-stripped.
- Xbox XBE has no symbol sections (XBEs never carry them) and **zero RTTI
  strings** (`.?AV` count = 0) — the game code is C-style / compiled without RTTI.
- Full recursive listings of both discs show no debug leftovers (no `.map`,
  `.sym`, `.txt`): PS2 carries only `SLES_531.52` + `SYSTEM.CNF` + SDK
  `IOP/IOPRP*.IMG` + assets; Xbox only `default.xbe` + `data/dsstdfx.bin`
  (DirectSound DSP effects) + assets.

So there is no free symbol table. The harvest value is elsewhere (Findings 2–4).

## Finding 2 — library identifications string-proven

Both console executables embed Perforce `$Id:` tags that confirm, from shipping
binaries, identifications the project previously made from code shape:

- **RenderWare Physics 3.7**: dozens of `//Physics/Rwp37Active/src/...` tags
  (volume/context/solverutils/body/ragdoll/qhull). Matches the
  `third-party-library[RenderWare-Physics-3.7]` band and the qhull island
  (`RwpQHullWrapper.c` tag present; qhull `user.h` diagnostics strings identical
  to the PC band).
- **RW 3.7 toolchain**: Xbox XBE debug path is
  `c:\TOAST\Code\WORKSPACE\PROJECT\rw37 xbox cdrom\toast.exe` — the internal
  name of the game binary is **toast.exe**, and the workspace is `rw37 ...`.
- PS2 build references `c:/daily/rwsdk/plugin/pds/sky2/...` (RW SDK PS2
  pipeline sources).

## Finding 3 — game source-tree paths

Assert/diagnostic strings leak real source paths of the game itself:

- PS2: `C:/US_MASHED/TOAST/CODE/src/AppCode/Audio/soundengine.c` and
  `C:/US_MASHED/TOAST/CODE/src/PS2Code/ssuv/skyssuv.c`
- Xbox: `\TOAST\Code\src\AppCode\AUDIO\soundengine.c`

This confirms the codebase split **`AppCode` (shared game code) vs platform
code (`PS2Code`, and by symmetry a PC layer)** — useful language for plates and
subsystem attribution. (`TOASTART` on disc = "Toast art", the project codename
is TOAST.)

## Finding 4 — the Xbox build is an x86 second compile of the same source

`toast.exe` `.text` is 1,930,564 bytes of i386 code built from the same source
tree with an MSVC-family XDK toolchain, with all Microsoft middleware living in
*separate* XBE sections (D3D, D3DX, XGRPH, DSOUND, XMV, WMADEC, XPP, DOLBY) —
i.e. the game+RW code is isolated in `.text`. This enables **cross-build
function matching** against `MASHED.exe`:

- every matched pair transfers our PC-side knowledge (names, analysis,
  confidence) onto a second instance — and back: a function that decompiles
  badly on PC may decompile cleanly on Xbox (different optimization context);
- PC↔Xbox match presence localizes shared `AppCode` vs PC-only platform code;
- divergences in matched functions highlight platform `#ifdef`s.

Pipeline (this session): flatten XBE sections to a raw memory image
(`re/tools/console/xbe_flatten.py` → `re/console/xbox/toast_flat.bin`, base
0x10000, entry 0x000396F9, kernel thunk 0x00284760) → dedicated headless Ghidra
project `re/console/ghidra/Mashed_Console` (zero contention with master/pool) →
auto-analysis seeded at entry + MSVC `55 8B EC` prologue scan
(`re/tools/console/ghidra_scripts/SeedXbeFunctions.java`) → function-boundary
dumps (`DumpFunctions.java`) → masked-hash matcher
(`re/tools/console/xbuild_match.py`: E8/E9 rel32 + in-image abs32 dwords
masked, full-body and 64-byte-prefix SHA1 tiers, unique-unique matches only).

## Match results

(to be filled by `xbuild_match.py` run)

## PS2 ELF — retained value despite stripping

- MIPS `.text` 3,259,936 bytes + `.vutext` (VU microcode) + DVP overlays.
- Cross-ISA code matching is not practical, but `.rodata`/`.data` tables
  (float constants, handling tables, string tables) are shared with PC and can
  anchor data-label identification where PC `.rdata` context is thin.
- Parked at `re/console/ps2/SLES_531.52.elf` for future data-side mining.
