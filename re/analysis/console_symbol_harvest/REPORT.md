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

## Match results (2026-06-12)

Ghidra auto-analysis found **6,810 Xbox** and **5,800 PC** functions.

- Pass 1 (`xbuild_match.py`, byte-identity masked hash, unique-unique only):
  **235** pairs (185 full-body, 50 prefix). Low rate as expected — PC MSVC vs
  XDK codegen differs.
- Pass 2 (`xbuild_match2.py`, feature tiers): **1,334** pairs total —
  `byte` 235 → `+mnem` 257 (mnemonic-sequence hash unique-unique) →
  `+string` 298 (unique-string anchors) → `+prop` 544 (call-graph gap
  propagation, fixpoint).

| tier | pairs |
|---|---|
| prop-weak | 459 |
| string | 298 |
| mnem | 257 |
| byte-full | 185 |
| prop-mnem | 85 |
| byte-prefix | 50 |

Coverage: **654 / 3,638 first-party hooks.csv rows (18.0%)**; subsystem
leaders: render 150, util 124, audio 110, boot 83, frontend 58. Output:
`re/console/match/xbuild_match_v2.csv` (pc_va, xbox_va, tier, name,
subsystem, confidence).

Quality corroboration:
- 580/1,334 pairs (43%) have *identical* full mnemonic sequences.
- Known named functions land plausibly and with per-TU locality, e.g.
  `TrackNodeRecordScan`/`TrackNodeRecordFind` (PC 0x0041e870/0x0041e980 →
  Xbox 0x001c32a0/0x001c31a0, adjacent in both builds);
  `LoadingState1Enter`/`LoadingState3Enter` byte-identical.
- `prop-weak` (459) is the only tier without an exact-feature witness —
  treat as candidate-grade; verify before relying on a specific pair.

Regeneration:
```
py -3.12 re/tools/console/xbe_flatten.py re/console/xbox/default.xbe re/console/xbox/toast_flat
<analyzeHeadless import runs — see git log of this report for exact commands>
py -3.12 re/tools/console/xbuild_match.py && py -3.12 re/tools/console/xbuild_match2.py
```

## Lane tooling (2026-06-12, post-match)

- **Names applied**: `ApplyTwinNames.java` renamed all 1,334 matched Xbox
  functions in `Mashed_Console` (semantic PC name where hooks.csv has one,
  else `pc_<va>`; tier + PC VA in the plate comment). 0 missing.
- **Twin decompilation on demand**:
  `py -3.12 re\tools\console\xtwin.py 0x<pc_va>` — looks up the match table,
  decompiles the Xbox twin headlessly (~30-60 s), prints C. Warns on
  `prop-weak` pairs. Single-writer project: don't run two at once.
  Verified on 0x0041e870 `TrackNodeRecordScan` → Xbox 0x001c32a0 (0x48-stride
  record scan, key at +0x10 — structure agrees with the PC side).

## Round 2 (2026-06-12, backlog levers): 1,334 → 1,813 pairs (26.4% first-party)

Three new tiers in `xbuild_match2.py` over an extended feature dump
(`DumpFuncFeatures.java` now also emits unusual scalar immediates ≥0x10000
and 8-byte data fingerprints of referenced non-string initialized data):

- **const** (234) — unique-token anchoring over immediates + data
  fingerprints. Notable: `FrontendDirInput` 0x00423040 → Xbox 0x00204283.
- **caller / caller-mnem** (12) — reciprocal-best votes from matched callers'
  unmatched callee sets.
- **interval / interval-mnem** (87) — positional pairing between consecutive
  strong anchors. Key discovery: **Xbox link order is globally REVERSED vs
  PC** (PC-ascending → Xbox-descending, detected at runtime); a thunk train
  at PC 0x004b5350..90 maps in exact lockstep to Xbox 0x0008eb60..a0. Also
  surfaced that the Xbox build links libpng (`png_zalloc`, `png_get_uint_32`
  matched by interval position).

Verification: an **ordinal check** flags pairs whose Xbox VA contradicts both
address-order neighbors; 89 contradicted weak-tier pairs were dropped,
1,623/1,813 surviving pairs are ordinal-consistent (flagged pairs keep
`ordinal_ok=0` in the CSV and an `ORDINAL-FLAGGED` plate in Ghidra — many sit
at library-band boundaries where band link order legitimately differs).

| tier | pairs | | tier | pairs |
|---|---|---|---|---|
| prop-weak | 587 | | prop-mnem | 101 |
| string | 298 | | interval | 59 |
| mnem | 259 | | byte-prefix | 50 |
| const | 234 | | interval-mnem | 28 |
| byte-full | 185 | | caller(+mnem) | 12 |

Coverage: **959 / 3,638 first-party rows (26.4%)**; audio 224, render 218,
util 169, boot 94, frontend 62, gameplay 47, vehicle 41, hud 34.
`ApplyTwinNames` re-run: 1,813 named, 30 stale labels cleaned.

## Round 3 (2026-06-12): scaffold verification + platform attribution

**Layout, characterized empirically** (`strong` pairs, NO-GUESSING): sorted by
PC address, consecutive Xbox deltas are inc=755 / dec=400 → the binary is
**ascending runs (translation units) laid out in DESCENDING xbox-base order**;
functions within a TU ascend together (top TU run = 53 functions, 401 runs).
The round-2 "globally reversed" model was the coarse view of this.

**Scaffold verification** (replaces the global-neighbor ordinal check): every
pair is classified against the *strong-pair* scaffold. A strong bracket [L,R]
with `xb_L < xb_R` and width ≤ 0x8000 is a same-TU span, so any pair whose PC
lies inside must have xbox in [xb_L, xb_R]:
- `ok-asc` (corroborated) — fits a tight ascending same-TU bracket: **859**,
  of which **352 are weak-tier pairs promoted out of candidate-grade**.
- `flag` — violates a tight ascending bracket: weak-tier flags are **dropped**
  (111 removed); the **161 remaining flags are all strong-tier** (byte-full 22,
  string 50, const 47, mnem 30, …) where independent feature evidence outranks
  the layout heuristic — retained, annotated `SCAFFOLD-FLAGGED`, treated as
  layout-model noise not match error.
- `ambiguous` — only a descending (cross-TU) or wide/edge bracket available,
  no constraint: 771.

Final table: **1,791 pairs**, 26.5% first-party. CSV column `scaffold`
(`ok-asc`/`flag`/`ambiguous`) lets consumers trust corroborated weak pairs and
discount the rest. `ApplyTwinNames` re-run: 1,791 named, 95 stale cleaned.

**Interval-tier correction** (found while characterizing the layout): round 2's
`tier_interval` used the *global* reversal flag and only processed *descending*
brackets (which straddle TU boundaries → unreliable cross-TU positional
pairing), reversing each gap. Round 3 rewrites it to use *per-bracket*
direction and only trust *tight ascending same-TU* brackets, pairing forward.
Same count (87) but a disjoint, correct set — **all 87 interval pairs are now
`scaffold=ok-asc`** (same-TU corroborated), vs cross-TU before. Recall did not
rise (the within-TU gaps are count-gated 1:1), so the small-getter recall gap
stays lever 1.

**Platform attribution** (`xbuild_platform_report.py` →
`re/console/match/platform_candidates.csv`), two signals, reported honestly:
- *Clean (platform-api):* 6 unmatched first-party functions reference a
  platform-API token. Three are **verified PC-only platform glue** —
  `FUN_004944c0` DirectShow video graph, `FUN_00494820` D3DX texture create,
  `sub_00499ba0` D3D9 device/window (`"MASHED [D3D9] [Release]"`); the Xbox
  build has these in separate XBE sections (XMV/D3D), so their absence as
  twins is correct. The greenfield port already reimplements this boundary
  (PngLoader, custom video path). 3 others are CRT (user32 MessageBox/exit).
- *Diagnostic (recall-gap):* 324 unmatched first-party functions with matched
  callers. **NOT platform attribution** — dominated by small getters that hash
  ambiguously (render 78, util 62, frontend 56). Shipped as a matcher-coverage
  map: it shows exactly which small-function class the matcher misses, which
  motivates lever 1 below.

## Remaining levers

1. **Small-function matching** — the recall-gap diagnostic shows the matcher
   misses tiny getters (no strings, generic mnemonics). A within-TU-run
   positional tier (now that TU runs are characterized) could place them:
   inside a corroborated ascending run, unmatched gaps pair positionally.
2. **PS2 `.rodata` data-mining** — shared float/string tables as a second
   witness for PC global identification (demand-driven).
3. **Use of matches**: (a) hairy PC function → read the Xbox twin
   (`xtwin.py`); (b) the 3 platform-api functions confirm render/IO platform
   boundaries for the port; (c) Xbox `.data` refs corroborate PC globals.
4. The Xbox disc asset tree (`ToastArt/`) and PS2 `TOASTART/` are format
   variants of the PC assets — useful when a PC format field is ambiguous.

## PS2 ELF — retained value despite stripping

- MIPS `.text` 3,259,936 bytes + `.vutext` (VU microcode) + DVP overlays.
- Cross-ISA code matching is not practical, but `.rodata`/`.data` tables
  (float constants, handling tables, string tables) are shared with PC and can
  anchor data-label identification where PC `.rdata` context is thin.
- Parked at `re/console/ps2/SLES_531.52.elf` for future data-side mining.
