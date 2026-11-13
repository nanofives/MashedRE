# RESULT RWC-FRAME-492E90: the frame render 0x00492e90 on the RW core, menu-executed slice first

Run 42, 2026-11-13. Branch `orch/RWC-FRAME-492E90`, based on `orch/integration` 145dec4d0. Top-down row (WORKER_RULES):
both builds, `ddkobj.py check`, knob boots, self-test. G-CTRL / B / 12 captures not run (they run with the knob off).
PREREG `verify/rwcframe_20261113/PREREG_RWC-FRAME-492E90.md`, committed alone first (bf72f99a6).

**Verdict: GREEN for the slice and the binding. The frame does not run on the core yet.** The boot reaches
AppInitialiseOnBootup's texture loads on the core and stops at the cup.dff clump read, before the main loop (section 5).

- **Slices.** The game-side closure of 0x00492e90 over re/match sources is 352 functions (266 at 100%, 57 below, 29
  with no source, 23 of those being switch labels or CRT). The **menu-executed slice**, reachable from 0x00492e90 through
  functions the INT3 census (`re/coverage/reach.tsv`) marks `reached_menu = 1`, is **44 game functions (34 at 100%, 10
  below) plus 15 RW-band callees** (`SLICE_EXEC_reach_menu.tsv`). Everything else is a later slice.
- **Binding.** Under `MASHED_RWC_FRAME`, `PortTop_StandIn_00492e90` calls `RwCore_00492e90`, the 100% source compiled as
  an RW core closure body. Its slice is generated from re/match sources, and each of the 37 game callees outside the slice
  is a logged stop (kind `stop`, never lifted, L-137 / L-141).
- **N = 1 unchanged.** vs_frame and ddk_frame end at 0x5257F001. The Im2D compare against the original's first disc draw
  is **IDENTICAL, 18 of 18 fields, on both builds**.
- **Self-test: PASS** in every core arm (RwTexDictionaryCreate binding 0 of 11 checks failed; RWC-DEVICE selftest PASS,
  0 fail).
- **Object build: `ddkobj.py check` IDENTICAL 1182 of 1208.** Of the 43 slice units it substitutes, 40 are IDENTICAL.
  The other 3 are 2 not-in-map (section 4) and 0x0043c5b0 (40.5% source, DIFFERENT, expected).

## 1. Slices of the 0x00492e90 closure

Tools: `size_closure.py` walks the re/match sources, picking each source the way `gen_rwcore_tus.py` does, and writes
`CLOSURE_492E90.tsv` (one row per member: depth, best_pct, source, census, the exe TUs that already compile the source,
callers). `slice_exec.py` writes `SLICE_EXEC_reach_menu.tsv`.

| set | game functions | 100% | below 100% | no source |
|---|---|---|---|---|
| whole closure | 352 | 266 | 57 | 29 (switch labels in 0x004368e0 / 0x0043af10 and CRT among them) |
| census reached_menu = 1 (any caller) | 77 | 59 | 13 | 5 |
| **slice 1: reachable through executed members only** | **44** | **34** | **10** | **0** |

RW-band callees of slice 1: 15 (13 at 100%; 0x00556ca0 60.0 and 0x00556e90 97.5). Slice 1 by role: the frame shell
(0x00492e90, timers 0x004950b0, camera 0x004671a0, clear / begin / end / show 0x004c1bb0 / 0x004c1a00 / 0x004c19f0 /
0x004c1be0, 0x00492440 x96), the case-1 menu branch (0x0042b930, 0x0042b8d0, 0x00492e60, 0x004c1c80), MenuDrawLoop
0x0043c5b0 (40.5) with the chrome 0x0042e3a0 / 0x0042e5b0 and the Im2D draw leaves 0x00472c60 / 0x00472f40 /
0x004730b0 / 0x00473c20 / 0x00473ee0 / 0x00474890 / 0x00473220 / 0x004733b0, the text path 0x00427e00 / 0x00427780 /
0x004277a0 / 0x00427680 / 0x00428140 / 0x004282a0 with RtCharset 0x00552d10 / 0x00552d70 / 0x005554d0 / 0x00556ca0 /
0x00556e90, the modal 0x00433f40 (99.3), and the loading screen 0x00403050 / 0x00402fb0 / 0x00428760 / 0x00428610 /
0x00450b10.

The 10 slice members below 100% (`equiv.py`, `EQUIV_SLICE.txt`), plus 0x00556ca0 / 0x00556e90 / 0x00551a50 from the
RW band:

| rva | pct | equiv.py |
|---|---|---|
| 0x0042f530 | 96.9 | NOT-PROVEN branch-condition |
| 0x0043c5b0 | 40.5 | NOT-PROVEN unsupported (conditional jump out of the function at 0x0043c616) |
| 0x00433f40 | 99.3 | NOT-PROVEN switch-table |
| 0x0042e5b0 | 54.3 | NOT-PROVEN call-regargs (call 0x473c20, edi 2 / 4) |
| 0x00473c20 | 99.0 | NOT-PROVEN x87-region |
| 0x00473ee0 | 66.3 | NOT-PROVEN loop-without-branch |
| 0x00474890 | 45.2 | NOT-PROVEN x87-region |
| 0x004733b0 | 97.3 | **EQUIV [VALUE-FLOW]** |
| 0x00473220 | 97.3 | **EQUIV [VALUE-FLOW]** |
| 0x00450b10 | 86.6 | NOT-PROVEN (already in PROVISIONAL.tsv, RWC-FIRST-FRAME-IM2D) |
| 0x00556ca0 | 60.0 | NOT-PROVEN indirect-jump |
| 0x00556e90 | 97.5 | NOT-PROVEN x87-region |
| 0x00551a50 | 96.4 | **EQUIV [BYTE-IDENTICAL]** |

All 12 new ones are in `re/match/PROVISIONAL.tsv` (status provisional, knob path only, never C4 by this status). The three
EQUIV rows are admission candidates. ADMITTED.tsv needs the EQUIV-ADMIT cross-check tally, which was not run here.

## 2. What changed

| file | change |
|---|---|
| `verify/rwccorerun_20261111/gen_rwcore_tus.py` | root 0x00492e90; `FRAME_SLICE` from `SLICE_EXEC_reach_menu.tsv` followed like RW code; game callees of a slice source outside the slice become `FRAME_STOP` (kind `stop`, logged stop, no lift); 0x005c4d30 followed; `EXE_BODY` += 0x00552750 / 0x005c4d30 (their 100% exe-only top-down bodies `Orig_00552750` / `Orig_005c4d30`); **RVAs the previous generation lifted stay lifted** (`pin_lift`: the match lanes added sub-100% sources for 156 of them since, and taking those would swap verbatim transcriptions for NOT-PROVEN sources outside this row); `PIN_MISSING` 0x004d1630 / 0x004deb20 / 0x005518f0 stay stops (their new g2sh / rwtb sources fail VS2022 C++: C2632, C2664, C2197) |
| `mashedmod/src/mashed_re/RwCore/*` | regenerated: closure **1651** = match100 524, provisional 277, hand 16, lift 680, exe 14, missing 103, stop 37 (RWC-FIRST-FRAME-IM2D: 1571, match100 492, provisional 268, lift 680, missing 103). 55 new TUs. 8 TUs left the closure (0x0052da20, 0x00551ad0, 0x00552230, 0x005522e0, 0x00552720, 0x00558cb0, 0x0055dec0, 0x005c4e10): no current source or lifted body names them. [UNCERTAIN] which reference dropped (the inputs moved between 145dec4d0's generated files and its sources). 0x004cba80 is now listed provisional: MATCH-EXACT-DATA (6ed67d96d) re-scored its source to 90.3 / relocheck BAD, same source text |
| `RwCore/RwCore_Knob.cpp` | `MASHED_RWC_FRAME_N` (default 1): the boot ends after RwRasterShowRaster call N on the core. `RwCore_GlobalsAtInit()`: [0x007d3ff8] after RwEngineStart under the frame knob. The stop log names kind missing or stop |
| `Match/PortTop_00492370.cpp` | `PortTop_StandIn_00492e90`: `RwCore_00492e90()` when `RwCore_FrameRoute()`, else the hand RenderFrame as before |
| `LibRw/RwApiBind.cpp` | `MashedRw_InstallRwGlobals` under the frame knob: [0x007d3ff8] is set back to the core's engine instance and the librw record is not installed (section 5, AV 1) |
| `D3d9Render/RwIm2DBridge.cpp` | `RwIm2DBridge_Install` under the frame knob: does not write the fake device to [0x007d3ff8] |
| `re/match/s01/00403050.c`, `00402fb0.c` | bare `extern` globals -> `MGLOBAL` (expands to the same `extern` under C). Still **100.0%, BYTE-IDENTICAL, relocheck OK** (7/2 and 7/1): the port links them at their original addresses (build 3 failed LNK2019 on `_DAT_00771964`, `_DAT_007d3ff8`, `_DAT_00636afc`) |
| `re/tools/ddkobj/ddkobj.py` | `__ddkreg_<rva>` owner is the unit (source + wrapper): a second wrapper of the same source (MatchS06 and RwCore copies of 0x00427680 / 0x004277a0) takes `_<wrapper stem>` (object build 1: LNK2005) |
| `re/tools/dual_copy_allowlist.txt` | + 39 `ORIGTOP-ONLY` pairs (RwCore copies of slice RVAs that also have exe bodies), cited to this RESULT |
| `re/match/PROVISIONAL.tsv` | + 12 rows (section 1) |

No librw binding added (L-124). No lifted body added (L-137). The 2 hand-install guards remove librw use on the knob path
and add none. Default and knob-off paths: unchanged (section 3).

## 3. Builds and arms

| step | result |
|---|---|
| default VS2022 build (`build_vs8.log`, the arms used the build made inside the object build, same tree) | Build OK, rva-lint OK: 231 findings, all allowlisted, NEW 0 |
| object build `MASHED_DDKOBJ=1` (`build_ddk2.log`) | Build OK, link rc 0, `mashed_re_ddk.exe` |

Arms (`arms_final.out`), all inside `test-slot` -> `mashed_lock.py run`, private root `D:/MashedWT/RWC-FRAME-492E90_root`:

| arm | exit | result |
|---|---|---|
| vs_off | 0 | "WinMain 0x00492370 returned 0 after 600 frames" |
| ddk_off | 0 | vs_off vs ddk_off **1144 / 1144 lines, 0 differ** (`verify/rwcdevice_20261111/logcmp.py`) |
| vs_core / ddk_core | 0xFFFFFFFF (by design) | engine bring-up, probe, failure path, as before |
| vs_frame | **0x5257F001** | Im2D compare **IDENTICAL, 18 fields** (`IM2D_COMPARE_vs_frame.tsv`) |
| ddk_frame | **0x5257F001** | Im2D compare **IDENTICAL, 18 fields** (`IM2D_COMPARE_ddk_frame.tsv`) |
| vs_frameN (`MASHED_RWC_FRAME_N=120`) | AV (section 5) | passes the first present, AppInitialiseOnBootup to the cup.dff clump read |

Original side: `D:/MashedWT/RWC-FIRST-FRAME-IM2D_data/orig_im2d_1.log` (the RWC-FIRST-FRAME-IM2D capture, same anchored
binary). No original launch in this row.

## 4. ddkobj.py check (`BYTECHECK.tsv`, 1208 rows)

IDENTICAL 1182, DIFFERENT 11, not-in-map 15. Refusals 4, all wrapper-redefines-globals. The VS2022 body is kept for those.

- **Slice 1 RwCore units:** 40 match100 IDENTICAL. 0x0043c5b0 DIFFERENT (40.5% source, provisional).
  `___ddkreg_00427680_RwCore_00427680` and `___ddkreg_004277a0_RwCore_004277a0` are **not-in-map**. The owner copies
  `___ddkreg_00427680` / `___ddkreg_004277a0` (MatchS06 TUs) are IDENTICAL. The RwCore copies are absent from
  `mashed_re_ddk.map`. [UNCERTAIN] identical-COMDAT folding or reference removal by the linker: not checked.
- **DIFFERENT rows with a 100% ledger score:** 0x004929d0 (`_GameModeMachine_004929d0`), 0x00493710 and 0x00493900.
  All three were DIFFERENT in RWC-FIRST-FRAME-IM2D's BYTECHECK. For 0x00493710 and 0x00493900 the compiled source (s15)
  is not the ledger's 100% source (pilot/a2, topm01). None of the three is in this row's change.
- Other DIFFERENT (sub-100% sources, from the integration tree): 0x00401690 60.2, 0x004332a0 97.3, 0x0045a3a0 81.0,
  0x00475a00 93.1, 0x00498810 86.7, 0x004c0c20 98.9, 0x004c0f00 98.8.

The row count moved from 1321 to 1208 against RWC-FIRST-FRAME-IM2D: 128 RVAs new, 269 gone. Most of that comes from the
integration merges since (DDKOBJ-CELLS and others), not from this row.

## 5. The N > 1 frontier: from the first present toward the main loop (the next slice)

With `MASHED_RWC_FRAME_N=120` the boot continues past ViewportInit's present into WinMain's next calls on the core.

| # | where it stopped | cause | status |
|---|---|---|---|
| 1 | AV EIP 0 in `RwCore_004ce2d0` (RwImageSetPath) via binding `MashedRw_ImageSetPath_004ce2d0`, from 0x004283a0 <- AppInitialiseOnBootup 0x00402750 | `MashedRw_InstallRwGlobals` (AppInitialiseOnBootup piece hooks) replaced [0x007d3ff8] with the librw record, so the core bodies read its slots | **resolved**: under the frame knob the install keeps the core's engine instance |
| 2 | AV EIP 0 in `RwCore_004c2c90` from `RwCore_004c2f00` (RwEngineGetCurrentMode), from 0x00402750 +0xC6 | my first fix restored the value [0x007d3ff8] had after RwEngineInit (0x007d3ec8). That is the static template. RwEngineOpen allocates the live instance (0x0164D958 in that run), whose +0x10 is the device fpSystem | **resolved**: the value is taken after RwEngineStart |
| 3 | **open**: AV EIP 0x00000001 (write, eax 0, ecx = instance + 0x12C, edx 1) while reading `cup.dff` (24,976 bytes from the piz) | stack (exe map): `Orig_0040bbb0+0x7E` -> `FUN_0042a6b0+0x72` -> `Orig_004b3d80+0x47` -> `Orig_004b3bf0+0x3A` -> `MashedRw_ClumpStreamRead_004e7420+0x39` (routed to `RwCore_004e7420`, **provisional 37.9%**, re/match/rwl02/004e7420.c). The words `DffModel::Parse` / `MashedRw_TextureRead_004c5cb0` on the stack are older frames. [UNCERTAIN] which callee of 0x004e7420 jumps to 1: the RpClumpStreamRead core closure (frame list / geometry list / atomics / extension readers) | next slice |

Before #3 the core ran, through the routed bindings, the TXD reads of fx.txd / badges.txd / TrackImages.txd /
Interface.txd (0x004cc230 memory streams, `MashedRw_TextureRead_004c5cb0`), HardwareShowIntroVideo and the font36.piz
mount. Not yet reached: the rest of AppInitialiseOnBootup, the run loop 0x00492290, and so `RwCore_00492e90` itself.
**No frame of 0x00492e90 has run on the core**, so the draw-for-draw compare (`menu_draw_burst.py`) is not run.

Next slice proposal (`RWC-FRAME-CLUMP`): the RpClumpStreamRead 0x004e7420 closure on the core, executed members first
(census `reached_menu`): replace the 37.9% source with a matched or EQUIV body (L-144 strict for executed RW). Then run
`MASHED_RWC_FRAME_N=120` again until the main loop reaches `RwCore_00492e90`, and compare its draws with
`menu_draw_burst.py`. Two more items: the hand frontend stand-in 0x0043dfd0 updates the menu state that 0x0043c5b0
reads, and the 37 `stop` callees name the next game slice once the frame runs.

## 6. Raw data

`D:/MashedWT/RWC-FRAME-492E90_data` (build logs vs1..vs8, ddk1..ddk2, the arm logs, `vs_frameN.log`, `vs_watch.log`, the
equiv output, `arms_final.out`, `ddkcheck.out`), sha256 in `RAW_SHA256_RWC-FRAME-492E90.txt`. `vs_watch.log` is from a
diagnostic hardware-watch build. The watch did not arm (the process ended after its RaiseException) and was removed
before build 8. Every game launch went through `test-slot` -> `mashed_lock.py run`. No merge-train hold was active at
any build.
