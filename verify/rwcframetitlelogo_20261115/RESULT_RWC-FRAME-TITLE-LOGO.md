# RESULT RWC-FRAME-TITLE-LOGO (run 50)

**Verdict: GREEN for this row's scope.** The title quad A[81] (x 80 y 80 w 480 h 240, `@0x450c7a,0x428750`) is now drawn on
the RW core in every dumped frame, in both builds. Its gate DAT_00771964 is non-zero (`08E63EC0` VS2022, `08FAA3A8` object
build) where run 49 logged `00000000`. The only row still missing in the draw compare is A[0], the movie backdrop
(RWC-FRAME-MOVIE-BACKDROP, another row, not touched). **Self-test: PASS on every arm of both builds** (section 4).
Both builds `=== Build OK ===`. `ddkobj.py check`: IDENTICAL 1553/1589, DIFFERENT 21, not-in-map 15, the same 21 units
as a fresh check of the base 9a44eee36 (BYTECHECK files byte-identical, section 5). G-CTRL / B / 12 captures not run
(top-down row: every change runs only under `MASHED_RWC_FRAME`).

Row kind: top-down RW core (L-124). No librw binding was added. Two bindings that pointed at librw (`rw::readBMP`,
`rw::readPNG`) or at a missing stop now point at exe code that builds RW core objects on the frame route.

## 1. Cause (logged)

Run 49's vs_dump (`D:/MashedWT/RWC-FRAME-CLUMP_data/vs_dump.log`) logs
`[ORIG_TOP] 0x00493600: missing (MASHED_ORIG_TOP_RWMOD off: no image module), returns 1`. 0x00493600
(re/match/s15/00493600.c, 100%) is the call that registers the "bmp" and "png" image formats with
RwImageRegisterImageFormat 0x004ce790. Its call site in 0x00493710 (`PortTop_Bound_00493600`,
Match/PortTop_00493600.cpp) returned 1 without registering when `MASHED_ORIG_TOP_RWMOD` is off, which the frame arms
leave off. So the RW core's image module had an empty format list, and the chain RwTextureRead 0x004c5cb0 -> read
callback 0x004c61b0 -> 0x004c6750 -> RwImageFindFileType 0x004ce6c0 found no extension.

Registering the formats alone does not finish the read. Two more steps on the same chain had no working body on the
core. This row fixed all three in one change and ran once with all three, so the two below rest on the run's log plus
the source and the routing table, not on a run with only the registration:

1. **File-exists.** The file-type search (0x004ce6c0 -> 0x004ce480 -> callback 0x004ce6f0, lifted) and the read callback
   0x004ce690 ask RtFS file-exists 0x00550b00 (re/match/s24/00550b00.c, 100%). Its body answers through the RtFS manager
   (DAT_007dc75c, file system list DAT_007dc754, default DAT_007dc76c) and returns 0 when DAT_007dc75c is 0 (source
   text, `GetFS`). The exe has no RtFSManager (PORT-TOP-04F), and this row's run logs `RtFS manager [0x007dc75c] = 0`
   on every one of the 24 logged calls (the log cap).
2. **Readers.** The registered read functions are the hand ports 0x004935a0 / 0x004935d0, which call RtBMPImageRead
   0x0052cb00 / RtPNGImageRead 0x00514330. At the base, ROUTED.tsv routes 0x00514330 on the frame route to
   `RwCoreMissing_00514330` (a logged stop: RwCore_Bodies.tsv kind missing, no source) and lists 0x0052cb00 as
   `not routed: not in the closure`, so its binding returns a librw `rw::Image*` (re/match/s23/0052cb00.c scores 22.9%).

## 2. Fix (frame route only)

| piece | file | what it does |
|---|---|---|
| A | Match/PortTop_00493600.cpp | `PortTop_Bound_00493600`: when `RwCore_FrameRoute()`, run the matched `Orig_00493600` at the original's place (0x00493710 calls it after RwEngineStart, which has run on the core) and log the core's format list (image module offset [0x007d4628], list head at +0x21c). Knob off: unchanged. |
| B | RwCore/RwCore_Bind_00550b00.cpp (new), gen_rwcore_tus.py `CALL_VIA` + new `LIFT_VIA` | `RwCoreBind_FsFileExists_00550b00`: the matched body when an RtFS manager is open (DAT_007dc75c != 0), else the 0x004b5350 shim's answer (`PortTop_FsFileExists_004b5350`: name in a mounted piz, or a file on disk under `original/`). Both core callers bind to it: the source TU 0x004ce690 (`#define FUN_00550b00 RwCoreBind_...`) and the lifted 0x004ce6f0 (`call RwCoreBind_...` at 0x004ce72f / 0x004ce74d). The body stays compiled (RwCore_00550b00) and stays a DDK object. Declared deviation: no call of the error callback DAT_007dc768(6) on a miss. |
| C | RwCore/RwCore_Bind_ImageRead.cpp (new), route_bindings.py `SPECIAL` + `FRAME_ONLY` | On the frame route the bindings `MashedRw_RtPNGImageRead_00514330` / `MashedRw_RtBMPImageRead_0052cb00` call `RwCore_FrameImageRead_<rva>`: read the named file from the piz mount stack or disk (`PortTop_Fs_ReadAll`, the same source as the RwStreamOpen binding 0x004cc230), decode with the exe's WIC decoder, build a core RwImage with RwImageCreate 0x004cdca0 (w, h, 32) + RwImageAllocatePixels 0x004cdd60 (core bodies), pixels R, G, B, A (the byte order of the BMP reader's palette build, re/match/s23/0052cb00.c `pal[0] = p[2]` .. `pal[3] = 0xff`). |
| log | RwCore/RwCore_Bind_ImageRead.cpp, route_bindings.py `SPECIAL` | `RwCore_FrameTextureRead_004c5cb0`: the frame-route binding of RwTextureRead calls the core body `RwCore_004c5cb0` and logs name, mask, result, texture raster / dict / refCount and the texture module words (current dict, read / find callbacks, +0x1c). The row's requested test. |

Generated outputs regenerated (`gen_rwcore_tus.py`, `route_bindings.py`): RwCore_004ce690.cpp (1 define),
RwCore_Lifted_00.cpp (2 calls + 1 declaration), RwApiBind.cpp (3 routed lines + the declaration block), ROUTED.tsv
(3 rows), PROVISIONAL_RWC.tsv (also gains 0x00473220 / 0x004733b0, 97.3% sources whose TUs were already generated at the
base: the base's PROVISIONAL_RWC.tsv was stale, no TU changed for them). `exe_sources.rsp`: the generator reorders it
(as in run 46); the committed order is kept and only the two new TUs are inserted (`RwCore_Bind_ImageRead.cpp` after
`RwCore_Missing.cpp`, `RwCore_Bind_00550b00.cpp` after `RwCore_Bind_004c5890.cpp`). The generator's other rewrites
were line-ending only and are not committed. BINDINGS.tsv: notes on 0x0052cb00 / 0x00514330, new row 0x00550b00.

**Declared deviation, [UNCERTAIN]:** the wrapper image is always 32-bit. The font36.piz files are MASHEDNEWLOGO.PNG
256x128 8-bit palette (colour type 3), BIGE.BMP 32x32 4 bpp, PROLOGICII.BMP 128x64 4 bpp, DOLBYDIG.BMP 128x64 8 bpp
(headers read from the extracted piz). The 22.9% BMP source text gives depth 4 for 1 / 4 bpp and 8 for 8 bpp
(re/match/s23/0052cb00.c, its `depth` assignment), which is not proven, and 0x00514330 has no source. What depth the
original returns decides the raster format 0x004d5340 picks, not the draw list. Test: an original-side read of the
RwImage that 0x00514330 / 0x0052cb00 return (+0x0c depth, +0x18 palette) for these four names. Ledger row (open)
RWC-IMGREAD-DEPTH.

## 3. Log of the read (vs_dump, `TITLELOGO_LOG_r50.txt`)

```
0x00493600 (matched) on the RW core (MASHED_RWC_FRAME): bmp / png registered, returned 1
after 0x00493600: RwGlobals 017491D0, image module offset [0x007d4628] 0x170, format list head [+0x21c] 08E5F844
  node 08E5F844: lcExt ".png" ucExt ".PNG" readFn 00180570 writeFn 0025F6F0 next 08E5F810
  node 08E5F810: lcExt ".bmp" ucExt ".BMP" readFn 00180550 writeFn 0025F5F0 next 00000000
0x00550b00("MashedNEWLogo.png") call 1: RtFS manager [0x007dc75c] = 0 -> piz/disk (0x004b5350 shim) = 1
0x00514330("MashedNEWLogo.png"): 15377 bytes from piz, 256x128 -> core RwImage 08F15018 (depth 32, stride 1024)
RwTextureRead 0x004c5cb0("MashedNEWLogo", 0) call 1 on the RW core = 08E63EC0 (raster 08E614A0 dict 00000000 refCount 1);
  texture globals: current dict 00000000 read 001C7ED0 find 001C8D90 [+0x1c] 00000000
... bigE.bmp 32x32, proLogicII.bmp 128x64, DolbyDig.bmp 128x64: each read, texture non-zero
phase ladder call 60: DAT_0067eca4 = 3, ..., DAT_00771964 = 08E63EC0 (0x00403050 gate)
drawdump core_f0..f7: 81 Im2D prim draws (run 49: 80)
```

The png node is first in the list because 0x004ce790 inserts at the head and 0x00493600 registers bmp then png. The
texture is in no dictionary (`dict 00000000`): the png read function 0x004935d0 calls RwTexDictionarySetCurrent(0)
before the read (disassembly cited in Match/PortTop_004ce790.cpp, 0x004935e2), so 0x004c5cb0 has no current dictionary
to add it to. Two later reads on the core, "test" and "OrangeDisplay", now search .png / .PNG / .bmp / .BMP and return
0 (no such file in a mounted piz or on disk); in run 49 they returned 0 with the empty format list. [UNCERTAIN] what the
original returns for those two names: not observed in this row.

## 4. Arms and self-tests (`ARMS_r50_summary.txt`)

Private root `D:/MashedWT/RWC-FRAME-TITLE-LOGO_root` (robocopy of `original\`), run through the run-46 arm script
`verify/rwcframeclump_20261113/run_arms.sh` with `RWC_DATA` / `RWC_ROOT` / `DUMPTAG=r50`.

| arm | exit | note |
|---|---|---|
| vs_off / ddk_off | 0 / 0 | `WinMain 0x00492370 returned 0 after 600 frames` |
| vs_frame / ddk_frame | 0x5257F001 | present 1, 0 live clumps at RwEngineStop |
| vs_frameN / ddk_frameN (N=120) | 0x5257F001 | present 120 |
| vs_frameT / ddk_frameT (N=100000, OT_TIMER) | 0 / 0 | WinMain returned 0 after 600 frames, 1 live clump (unchanged, RWC-FRAME-BOMB-DESTROY) |
| vs_dump / ddk_dump (N=590, OT_TIMER) | 0x5257F001 | 8 frames dumped at `DAT_0067eca4 3`, 81 draws each |

**Self-test on every arm, both builds: PASS.** `[OT_CODEPTR]` `self-test PASS (0 of 260 checks failed)` on all ten;
RwTexDictionaryCreate binding `self-test PASS (0 of 11 checks failed, 0 failing calls so far)` and `[RWC-DEVICE]
selftest PASS (0 fail)` on the eight frame-route arms; `[OT_TIMER] self-test PASS (0 of 6000 checks failed)` on
vs_frameT / ddk_frameT. 0 `AV at`, 0 `STOP`, 0 `FAIL` lines in all ten logs.

## 5. Draw compare and object build

`drawlist_diff.py --scale-b 1`, original `orig_burst_492e90.json` (run 45 capture, sha256 924d5547...) frames 0..3 vs
core frames 0..3 (`MASHED_RWC_DRAWDUMP_SKIP=200 MASHED_RWC_DRAWDUMP_FRAMES=8`):

| dump | strict (`--tol-anim 0`) | `--tol-anim 8` |
|---|---|---|
| VS2022 | RED match 156 / mismatch 168 / missing 4 / extra 0 | RED match 324 / missing 4 / extra 0 |
| object build | RED match 156 / mismatch 168 / missing 4 / extra 0 | RED match 324 / missing 4 / extra 0 |
| VS2022 vs object build (8 frames each) | | GREEN 648 / 648 |

Run 49: strict 152 / 168 / 8, tol-8 320 / 8. The 4 matched rows gained = A[81] in each of 4 frames. The 4 missing left
are A[0] (x 0 y 0 w 512 h 512, `@0x473ebd,0x42e65a`), the backdrop quad of row RWC-FRAME-MOVIE-BACKDROP. The 168 strict
mismatches are the 42 per frame `@0x474464,0x42e89c` jitter rows of RWC-FRAME-JITTER-STRICT, unchanged. So for this row
the compare is GREEN except rows owned by other open rows.

Builds: VS2022 (`MASHED_NO_DEPLOY=1`) `=== Build OK ===`, `[rva-lint] OK -- no NEW duplicate bodies (314 known)`.
Object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) `=== Build OK ===`, `[ddkobj] units 1751 ddk objects 1539`, `link rc=0`.
`ddkobj.py check` on the branch: **IDENTICAL 1553/1589, DIFFERENT 21, not-in-map 15**, refusals 11. A fresh object
build and check of the base 9a44eee36 (same worktree, detached, then back to the branch) gives the same counts and the
same 21 DIFFERENT units (0x004332a0 0x0045c480 0x00475a00 0x004929d0 0x00554200 0x005a6b30 0x005a6e50 0x005a7810
0x005a78b0 0x005a8890 0x005a9970 0x005aa480 0x005aab00 0x005ad030 0x005ad320 0x005adb70 0x005ae210 0x005ae5b0 0x005ae610
0x005ae920 0x005aea60); `BYTECHECK_base.tsv` and the branch `BYTECHECK.tsv` are byte-identical (sha256 2c950005...).
No newly DIFFERENT unit. No unit with a 100% source is DIFFERENT that is not DIFFERENT at the base. This row changes no
re/match source and makes no match or C4 claim; 0x004c5cb0 stays provisional (PROVISIONAL_RWC.tsv).

Exe sha256: mashed_re.exe 4f556c54a248a0d1..., mashed_re_ddk.exe 47d0f58bbf7983b4... (branch build, copies in the data
folder, not committed).

## 6. Process

Every launch went through `test-slot` (outside, no hub override) -> `mashed_lock.py run --label RWC-FRAME-TITLE-LOGO`
(inside); `knob_boot.py` kills only the PID it spawns (none timed out). `mashed_lock.py status` was checked before every
build (never PRIORITY-HOLD / SOLO). No `original\` write, no deploy of `original\mashed_re_dev.asi`. No tracker edit.

Committed: this RESULT, `TITLELOGO_LOG_r50.txt`, `ARMS_r50_summary.txt`, `DRAWDIFF_r50_{vs,ddk}_t{0,8}.txt`,
`DRAWDIFF_r50_vs_vs_ddk_t8.txt`, `BYTECHECK.tsv`, `RAW_SHA256_RWC-FRAME-TITLE-LOGO.txt`. Raw data:
`D:/MashedWT/RWC-FRAME-TITLE-LOGO_data` (arm logs, core dumps, build logs, base and branch BYTECHECK, exe copies), sha256
in `RAW_SHA256_RWC-FRAME-TITLE-LOGO.txt`.

## 7. Open rows

- RWC-IMGREAD-DEPTH (added to the ledger, open): the depth / palette of the RwImage the original's 0x00514330 and
  0x0052cb00 return for MashedNEWLogo.png, bigE.bmp, proLogicII.bmp, DolbyDig.bmp (original-side read), then a core
  reader that returns the same; a source for 0x00514330 (RtPNGImageRead, libpng 1.0.12 callees) and a better than 22.9%
  source for 0x0052cb00 are match work.
- Unchanged: RWC-FRAME-MOVIE-BACKDROP (A[0]), RWC-FRAME-JITTER-STRICT (the 42 strict rows), RWC-FRAME-BOMB-DESTROY (1 live
  clump at teardown), RWC-FRAME-LADDER-CHECK.
