# re-classify transaction D5-RFX-RECLASS (gated 2026-10-28, no C-level change on existing rows)

Source: `verify/rfx_emitters_20261028/RESULT_D5-RFX-EMITTERS.md` section 5. Worktree branch `orch/D5-RFX-RECLASS`
off `696021549`. Files: `hooks.csv` (cp1252, keep encoding and line endings), `re/analysis/CHANGELOG.md`.
STUBS / UNCERTAINTIES / DEFERRED: no change (no new stub or uncertainty marker is introduced).

## Gate checks done before the edit

- `Render\RfxEmitter.cpp` is in `mashedmod/exe_sources.rsp:78`. Each RVA in part A except `004770a0` has a body
  citation there (`// 0x<rva>` at RfxEmitter.cpp:64..260, the six callbacks inside `Instance` at :268..:332).
- `004770a0` has NO citation in RfxEmitter.cpp. Its exe body is `RaceFx_DrawEmitter` in
  `LibRw/RwRaceFx.cpp:2248`, an exe-only TU compiled from `mashedmod/build.bat`. Its exe_file is that file
  (deviation from RESULT section 5, which listed RfxEmitter.cpp).
- `00534b60` already has a row (C2 mapped). It gets exe_file only, not a new row (deviation from RESULT section 5).
- New rows at C1, which needs xref evidence plus a subsystem. There are no analysis notes, so C2 is not met. The xrefs
  come from a byte scan of `original/MASHED.exe.unpatched` (E8 rel32 and absolute dwords). `decomp_pc.py` reports
  `0x00536780` as "no function, never disassembled" (Ghidra-missed, as are the six callbacks).

## Part A: set `exe_file` (column 10) and append to `notes` (column 9). Nothing else changes.

Append to notes (with a leading ` | `), where `<X>` is per row below:
`[D5-RFX-RECLASS 2026-10-28] exe body <X> (D5-RFX-EMITTERS); no C-level change`

| rva | exe_file | `<X>` |
|---|---|---|
| 004769a0 004769d0 004769f0 00476a10 00476a30 00476a40 00476a80 00476ae0 00476b30 00476b90 00476be0 00476c10 00476c60 00476cb0 00476d00 | `mashedmod/src/mashed_re/Render/RfxEmitter.cpp` | `Render/RfxEmitter.cpp, arm A1 0 mismatches vs the original's bytes in Unicorn; second body next to the asi hook, CROSS-TARGET allowlisted (re/tools/dual_copy_allowlist.txt)` |
| 004770c0 00476df0 00534b60 | `mashedmod/src/mashed_re/Render/RfxEmitter.cpp` | `Render/RfxEmitter.cpp, arm A1 0 mismatches vs the original's bytes in Unicorn` |
| 004770a0 | `mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp` | `LibRw/RwRaceFx.cpp:2248 RaceFx_DrawEmitter, not executed by arm A1` |

## Part B: append 7 new rows at the end of hooks.csv

Common cells: subsystem `third-party-library[renderware]`, confidence `C1`, status `mapped`,
file `verify/rfx_emitters_20261028/RESULT_D5-RFX-EMITTERS.md`, scenario empty, frida_diff empty,
exe_file `mashedmod/src/mashed_re/Render/RfxEmitter.cpp`. Name `FUN_<rva>`. Notes (quote the cell):

| rva | notes |
|---|---|
| 00536780 | `[D5-RFX-RECLASS 2026-10-28] Ghidra-missed (decomp_pc: never disassembled). Called at 0x00536337 (E8). Flag word -> position / colour / uv callback addresses (immediates 0x00536804, 0x00536843, 0x0053687b, 0x005368ad, 0x005368e2, 0x00536913). Exe body RfxEmitter.cpp:260 SelectCallbacks; arm A1 set C3 146 cases 0 mismatches (RESULT_D5-RFX-EMITTERS.md)` |
| 00536e90 | `[D5-RFX-RECLASS 2026-10-28] Ghidra-missed. Address as immediate at 0x00536366 and 0x00536843 (selector). Position instancer, unrotated quad: X = (sx 0.5) r, Y = (sy 0.5) u, 0.5 = [0x005cc32c] (0x00536ed0..0x00536fc2). Exe body RfxEmitter.cpp Instance; arm A1 set C4 0 mismatches` |
| 00536fe0 | `[D5-RFX-RECLASS 2026-10-28] Ghidra-missed. Address as immediate at 0x00536804 (selector). Position instancer, rotated quad via fsincos (0x00537042..0x005371d4); args from call site 0x005366e2..0x005366f3. Exe body RfxEmitter.cpp Instance (inline x87, PC_24); arm A1 set C4 0 mismatches` |
| 00537b20 | `[D5-RFX-RECLASS 2026-10-28] Ghidra-missed. Address as immediate at 0x005363c2 and 0x005368e2. Colour instancer: bytes b0..b3 -> b3<<24 OR b0<<16 OR b1<<8 OR b2 on four vertices (0x00537b43..0x00537b68). Exe body RfxEmitter.cpp Instance; arm A1 set C4 0 mismatches` |
| 00537d20 | `[D5-RFX-RECLASS 2026-10-28] Ghidra-missed. Address as immediate at 0x00536913 (selector). Constant-colour instancer; the port needs FUN_00476b30 to have run (plugin +0xc4 initial value not transcribed). Exe body RfxEmitter.cpp Instance; arm A1 set C4 0 mismatches` |
| 00537d70 | `[D5-RFX-RECLASS 2026-10-28] Ghidra-missed. Address as immediate at 0x0053687b (selector). Per-particle uv instancer: (u0,v0,u1,v1) -> (u0,v1)(u1,v1)(u1,v0)(u0,v0). Exe body RfxEmitter.cpp Instance; arm A1 set C4 0 mismatches` |
| 00537e60 | `[D5-RFX-RECLASS 2026-10-28] Ghidra-missed. Address as immediate at 0x005368ad (selector). Constant uv from plugin +0xd8, default (0,0,1,1) [0x00613294] (0x00537e77..0x00537ebe). Exe body RfxEmitter.cpp Instance; arm A1 set C4 0 mismatches` |

## Part C: `re/analysis/CHANGELOG.md`

Insert 26 lines immediately below the line that is exactly `<!-- ENTRIES -->` (exact full-line match, never a
substring search). Never rewrite or truncate. Format, two spaces between fields:

- Part A rows: `2026-10-28  <rva>  <name as in hooks.csv>  <C>-><C>  exe_file=<exe_file> (D5-RFX-RECLASS, verify/rfx_emitters_20261028/RESULT_D5-RFX-EMITTERS.md)`
- Part B rows: `2026-10-28  <rva>  FUN_<rva>  new->C1  new row, byte xrefs (D5-RFX-RECLASS, verify/rfx_emitters_20261028/RECLASS_TXN_D5-RFX-RECLASS.md)`
