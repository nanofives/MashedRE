# RESULT RWC-FIRST-FRAME-IM2D: the loading disc on the first RW core frame, compared with the original

Run 41, 2026-11-12. Branch `orch/RWC-FIRST-FRAME-IM2D`, based on `orch/RWC-PLUGIN-ATTACH` cf266b029. Top-down row
(WORKER_RULES): both builds, `ddkobj.py check`, knob boots, self-test. G-CTRL / B / 12 captures not run (those hashes run
with the knob off). Same knobs as `verify/rwccorerun_20261111/PREREG_RWC-CORE-RUN.md` plus `MASHED_RWC_FRAME`: no new
knob, no default changed.

**Verdict: GREEN for the Im2D part. The 0x00492e90 part is not done** (sized in section 6, follow-up row proposed).

- The loading-disc draw 0x00428450 now runs on the RW core under `MASHED_RWC_FRAME`, through the matched Im2D quad
  0x00450b10 and the core's device block. **Its Im2D traffic is IDENTICAL to the original's first disc draw, 18 of 18
  fields**: call args, the spin angle [0x0067d974] before and after, the screen dims, all 7 RenderStateSet calls (state,
  value, result), the Im2DRenderPrimitive type / count / result, and the 4 vertices as raw dwords.
- The first comparison found one difference, `RenderStateSet(12, 1)` = 0x007D6B14 on the core against 1 in the original.
  Cause: re/match/s21/004d7100.c declared the state-0xc handler `void`. The original returns 1 on every path. With
  `int` + `return 1` the source is **100% byte-identical, relocheck OK (27/0)**, a match-route C4 candidate filed as
  `re/promotions/RWC-FIRST-FRAME-IM2D.tsv` (dry run PASS).
- Self-test: **PASS** (RwTexDictionaryCreate binding 0 of 11 checks failed, RWC-DEVICE selftest 0 fail). Section 5.
- Knob-OFF paths unchanged: section 4.

## 1. What changed

| file | change |
|---|---|
| `verify/rwccorerun_20261111/gen_rwcore_tus.py` | root `00428450`; `XFOLLOW` += 0x00428450, 0x00450b10, 0x0042b8b0, 0x0042b8c0; `EXE_BODY` binds 0x0042b8b0 / 0x0042b8c0 to the exe's matched `ScreenWidthGet` / `ScreenHeightGet` (100%, already compiled); `PCT` 0x004d7100 = 100.0 (the ledger row still says 21.4) |
| `mashedmod/src/mashed_re/RwCore/RwCore_00428450.cpp`, `RwCore_00450b10.cpp` | generated: re/match/s06/00428450.c (84.6%) and re/match/s08/00450b10.c (86.6%) compiled as the core bodies. Closure 1571 RVAs: match100 492, provisional 268, hand 16, lift 680, exe 12, missing 103 |
| `mashedmod/src/mashed_re/Match/PortTop_00493710.cpp` | `PortTop_Missing_00428450` (ViewportInit 0x00428590's call) calls `RwCore_00428450` when `RwCore_FrameRoute()`; for that one call it wraps RwGlobals +0x20 / +0x30 with logging thunks and restores them after. Otherwise unchanged (logged no-op) |
| `re/match/s21/004d7100.c` | `int`, `return 1` (section 3) |
| `re/match/PROVISIONAL.tsv` | + 0x00428450, + 0x00450b10 (status provisional, equiv NOT-PROVEN with the first difference); - 0x004d7100 (now 100%) |
| `re/tools/dual_copy_allowlist.txt` | + `ORIGTOP-ONLY CROSS-TARGET:00428450` (exe core body vs the .asi `HudSpinCoinAnim`, HUD/TextCluster.cpp) |
| `verify/rwccorerun_20261111/run_arms_attach.sh`, `frame_iter.sh` | `RWC_DATA` / `RWC_ROOT` / `RWC_LABEL` overrides (defaults unchanged) |
| `verify/rwcim2d_20261113/orig_im2d_boot.py`, `compare_im2d.py` | original-side capture (Frida, spawn, hooks 0x00428590 / 0x00428450 / 0x00450b10 and the two RwGlobals entries, each fires once at boot) and the field comparator |

No librw binding added (L-124). No lifted body added (L-137): 0x00428450 and 0x00450b10 are re/match sources.
`equiv.py` on both: 0x00428450 `NOT-PROVEN call-x87: region ours +0x0 / orig 0x00428450 call 0x42b8c0`, 0x00450b10
`NOT-PROVEN x87-region: ... x87 instruction 6 differs: fld st(1) | fld m4[esp+0x1c]`, so both are provisional (L-126),
knob path only, never C4 by this status.

## 2. Im2D comparison against the original

Original side: `orig_im2d_boot.py` on the patched MASHED.exe (private root, `MASHED_RE_NO_AUTO_HOOK=1`, so stock),
inside `mashed_lock.py run`. The first 0x00428450 call is ViewportInit's, args (32, -32). It killed only its own pid.
Exe side: `vs_frame` arm (`MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1`), the `[IM2D]` lines.

Original (`orig_im2d_1.log`):

```
[IM2D] 0x00428450(32, -32) original: [0x0067d974] 3FC90FDB (1.5707963705062866), [0x00771960] 0D662888 -> tex 0D65FE70, screen [0x0067ea54] 640 x [0x0067ea56] 480, RwGlobals 0413A328
[IM2D] 0x00450b10(tex 0D65FE70, x 43FC0000 y 42200000 w 42A00000 h 42A00000, col FFFFFFFF, uv 00000000 00000000 3F000000 3F800000)
[IM2D] RenderStateSet(1, 0x0D65FE70) = 1
[IM2D] RenderStateSet(9, 0x00000002) = 1
[IM2D] RenderStateSet(8, 0x00000000) = 1
[IM2D] RenderStateSet(6, 0x00000000) = 1
[IM2D] RenderStateSet(12, 0x00000001) = 1
[IM2D] Im2DRenderPrimitive(type 4, verts 00898A20, n 4)
[IM2D] v0 hex 43FC0000 42200000 00000000 3F800000 FFFFFFFF 00000000 00000000 = x 504 y 40 z 0 rhw 1 col FFFFFFFF u 0 v 0
[IM2D] v1 hex 44120000 42200000 00000000 3F800000 FFFFFFFF 3F000000 00000000 = x 584 y 40 z 0 rhw 1 col FFFFFFFF u 0.5 v 0
[IM2D] v2 hex 43FC0000 42F00000 00000000 3F800000 FFFFFFFF 00000000 3F800000 = x 504 y 120 z 0 rhw 1 col FFFFFFFF u 0 v 1
[IM2D] v3 hex 44120000 42F00000 00000000 3F800000 FFFFFFFF 3F000000 3F800000 = x 584 y 120 z 0 rhw 1 col FFFFFFFF u 0.5 v 1
[IM2D] Im2DRenderPrimitive = 1
[IM2D] RenderStateSet(8, 0x00000001) = 1
[IM2D] RenderStateSet(6, 0x00000001) = 1
[IM2D] 0x00428450 returned; [0x0067d974] 3FD5DCA8 (1.6707963943481445)
```

| exe build | file | verdict |
|---|---|---|
| build_vs2 (before the 0x004d7100 fix) | `vs_frame_2.log`, `IM2D_COMPARE_vs_frame_b2.tsv` | DIFFERENT, 1 of 18: field 8 `RenderStateSet(12, 1)` result 8219412 (0x007D6B14) against 1 |
| build_vs3 | `vs_frame_b3.log`, `IM2D_COMPARE_vs_frame.tsv` | **IDENTICAL, 18 of 18** |
| object build (`mashed_re_ddk.exe`) | `ddk_frame.log`, `IM2D_COMPARE_ddk_frame.tsv` | **IDENTICAL, 18 of 18** |

Masked as process addresses: RwGlobals, [0x00771960], the texture pointer, the verts pointer. The texture is compared as
"nonzero, and RenderStateSet(1)'s value is the texture read from [0x00771960] on the same side" (both true). The verts
pointer is 0x00898A20 on both sides (the exe maps the original's data range). The first frame is then presented on the
core as in RWC-PLUGIN-ATTACH (`RwRasterShowRaster 0x004c7730 ... first frame presented`, exit 0x5257F001), now with the
disc drawn into it.

## 3. 0x004d7100: the state-0xc handler returned garbage on the core

0x004d7480 (RenderStateSet, `re/match/s21/004d7480.c`) case 0xc returns `FUN_004d7100(value)`. The source of 0x004d7100
was `void` (21.4%), so on the core the result was whatever EAX held, the address 0x007d6b14 it had just loaded. The
original (`MASHED.exe.unpatched`, capstone):

```
004d710e  b801000000   mov eax, 1
...                    (no write to eax on any path)
004d71ea  c3           ret
```

The source's own note named the residual "the original holds the constant 1 in eax". With `int FUN_004d7100(int)` and
`return 1;` at the end: `match.py` **100.0% (55/55 ins, regs 100%)**, `relocheck.py` **OK checked=27 unchecked=0**.
`c4_reclass.py batch --tsv re/promotions/RWC-FIRST-FRAME-IM2D.tsv` (dry run): `[1/1] 0x004d7100 match PASS byte-identical
235 B, relocheck OK (27/0)`. Only core TUs compile this source (`RwCore_004d7100.cpp`; 0x004d07b0 and 0x005412d0 bind to
it), so the default path is not affected. The ledger row (`match_ledger.tsv`, 21.4) is left to the match lanes' batch
tools; the generator carries `PCT['004d7100'] = 100.0` meanwhile.

## 4. Builds, byte check, arms

| step | result |
|---|---|
| default VS2022 build (`build_vs3.log`) | Build OK, rva-lint OK (build_vs1 stopped on the new ORIGTOP-ONLY pair 0x00428450, allowlisted by `allow_origtop.py`) |
| object build `MASHED_DDKOBJ=1` (`build_ddk1.log`) | Build OK, link rc 0, `mashed_re_ddk.exe` |
| `ddkobj.py check` (`BYTECHECK.tsv`, 1321 rows) | IDENTICAL 1091, DIFFERENT 216, not-in-map 14; refusals 47 (static-register-convention 38, wrapper-redefines-globals 9), the same mechanism as before. Against RWC-PLUGIN-ATTACH's 1319 rows exactly 3 rows changed: 0x00428450 new DIFFERENT (84.6% source), 0x00450b10 new DIFFERENT (86.6% source), **0x004d7100 DIFFERENT -> IDENTICAL**. No other row changed |

Arms (`arms_final.out`, plus `vs_frame_b3.log` from `frame_iter_3.out`), all inside `test-slot` -> `mashed_lock.py run`:

| arm | exit | result |
|---|---|---|
| vs_off | 0 after 600 frames | knob off, librw path |
| ddk_off | 0 after 600 frames | knob off; vs_off vs ddk_off **1134 / 1134 lines, 0 differ** (`verify/rwcdevice_20261111/logcmp.py`), as in RWC-PLUGIN-ATTACH |
| vs_core | 0xFFFFFFFF (by design) | Init / Attach / Open / Start = 1 on the core, probe, Stop, failure path, Close = 1, Term = 1 |
| ddk_core | 0xFFFFFFFF (by design) | same as vs_core on the object build |
| vs_frame (build_vs3) | **0x5257F001** | disc drawn on the core (section 2, IDENTICAL 18 / 18), then the first Present on the core, Stop / Close / Term |
| ddk_frame | **0x5257F001** | same on the object build; Im2D compare **IDENTICAL, 18 of 18** (`IM2D_COMPARE_ddk_frame.tsv`) |

In the object build 0x00428450 and 0x00450b10 run as the VS2022-compiled provisional bodies' DDK objects, which ddkobj
reports DIFFERENT from the original's bytes (84.6% / 86.6% sources); their observable Im2D traffic is still identical.

## 5. Self-test

**PASS** in every core arm. `RwCoreBind_TexDictionaryCreate_004c5890` self-test "PASS (0 of 11 checks failed, 0
failing calls so far)" in vs_core, ddk_core, vs_frame (build_vs3) and ddk_frame. RWC-DEVICE "selftest PASS (0 fail)"
(backbuffer 640x480, windowed, getters 640x480, GetDirect3D and QueryInterface return the wrapper, client 640x480) in
vs_frame and ddk_frame. The knob-OFF arms do not run either self-test.

## 6. Not done: 0x00492e90 on the core (follow-up row)

The matched frame render 0x00492e90 (re/match/s15/00492e90.c, 100%) still runs as `PortTop_StandIn_00492e90` (the hand
RenderFrame), and the frame knob still ends the boot at ViewportInit's Present. Sizing from the re/match sources (scratch
script, game RVAs outside the RW bands, transitive): **378 game functions**: 300 at 100%, 69 below 100%, 9 with no
source (0x004128f0, 0x00419530, 0x0041dab0, 0x00484310, 0x00484360, 0x004b3f90, 0x004b4070, 0x004b49b0, 0x0055deb0),
plus 81 RW-band callees for the core closure and 5 CRT. Lowest: 0x0043af10 22.9, 0x004368e0 25.1, 0x0041ded0 27.3,
0x0041aac0 29.2, 0x0043a610 35.1, 0x00439210 40.5, 0x0043c5b0 (MenuDrawLoop) 40.5. Its 31 direct callees: 27 at 100%,
0x00404320 98.3, 0x0042f530 96.9, 0x00433f40 99.3, 0x0043c5b0 40.5. That is the T1 menu tree itself, not one session.
Proposed row: `RWC-FRAME-492E90`: under `MASHED_RWC_FRAME`, bind 0x00492e90 to its matched source with the 100% game
callees and the RW core, provisional bodies for the sub-100% ones (L-126), and move the frame-knob endpoint from
ViewportInit's Present to main-loop frame N, compared draw for draw with `menu_draw_burst.py`.

Other open items from RWC-PLUGIN-ATTACH section 7 are unchanged: 0x00493700 / 0x004936f0 (no body, logged), 17 bindings
not routed, 0x004c51a0 refused in PROVISIONAL.tsv but still routed on the core, lifted 0x0052e3d0 executing on
EngineStart (L-141).

## 7. Raw data

`D:/MashedWT/RWC-FIRST-FRAME-IM2D_data`, sha256 in `RAW_SHA256_RWC-FIRST-FRAME-IM2D.txt`. Private root
`D:/MashedWT/RWC-FIRST-FRAME-IM2D_root` (`original\` is a junction to `D:\MashedWT\PORT-TOP-04J_rootK\original`, as in
RWC-PLUGIN-ATTACH). Every game launch went through `mashed_lock.py run`; no merge-train hold was active at any build.
