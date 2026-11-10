# RESULT PORT-OT-IMAGE-FMT: the bmp / png formats of 0x00493600 on MASHED_ORIG_TOP_RWMOD, 2 more rows released

Branch `orch/PORT-OT-IMAGE-FMT`, stacked on orch/PORT-OT-RWG-DEVSLOTS `fd0f11771`. orch/integration was at `086427678`
when the branch was made, so no merge. PREREG `08466f064` (committed alone), code `dd2cb7e8c` + `e9605e3fc` (the node-list
log line). Worker cmwal53s12879le1ce8kqrv3z.

## Verdict: GREEN

- With `MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1`, RwGlobals rows: **OK 69, needs binding 3**. PORT-OT-RWG-DEVSLOTS had
  67 / 5. This matches the PREREG prediction.
- Released (`RWG_IMAGEFMT_RELEASED.tsv`): 0x004ce790, 0x004ce8e0. Image module +0x21c is `0x10ea6450` (the original holds
  a heap list there, `0x0365ce4c`).
- No row went from OK to needs binding.
- Seed columns: OK 112 / needs seed rule 13 (DEVSLOTS: 111 / 13). This is **one more row than the PREREG said** ("seed
  unchanged"). The new row is 0x00493600 itself. Its callees now have BINDINGS.tsv rows, so `ot_measure.py lists` counts
  it in the route (192 rows, was 191). It reads the two strings at 0x005cf748 / 0x005cf74c, which this step seeds, so it
  is OK. The 191 old rows have the same seed verdicts as before.
- Default path: unchanged. G-CTRL `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9` x3.

## The change

**0x00493600 registers two formats.** `re/match/s15/00493600.c`: `0x004ce790(&PTR_DAT_005cf74c, 0x004935a0, 0x0052d530)`,
then `0x004ce790(&PTR_DAT_005cf748, 0x004935d0, 0x00513f90)`. The two "pointers" are the strings themselves. The bytes in
`original/MASHED.exe.unpatched` are `62 6d 70 00` = "bmp" at 0x005cf74c and `70 6e 67 00` = "png" at 0x005cf748.

| rva | what it is (disassembly, `re/tools/disasm_va.py`) | exe |
|---|---|---|
| 0x00493600 | the two calls above (C4 match) | matched body, `Match/PortTop_00493600.cpp` (new, exe-only, `// RVA-LINT: ORIGTOP-ONLY`) |
| 0x004ce790 | RwImageRegisterImageFormat on the image module (C4 match, `re/match/s20/004ce790.c`): ".ext" lower / upper case through +0xd0 / +0xfc / +0xf8, lookup through +0xe8, 0x34-byte node from +0x118 on the freelist at +0x218, head insert at +0x21c, read fn at node +0x28, write fn at +0x2c | matched body, `Match/PortTop_004ce790.cpp` (new, exe-only, marker) |
| 0x004935a0 | at 0x004935a5..0x004935b8: `0x00496400("Reading texture %s\n", name)`, `0x004c5c80(0)`, `0x0052cb00(name)`. Then `add esp,0x10 / pop esi / ret`, so eax = 0x0052cb00's result | hand port `PortTop_ImgRead_004935a0`. The matched C declares it void and calls 0x00496400 with two arguments through a one-parameter prototype, which does not compile as C++ |
| 0x004935d0 | the same, `call 0x514330` at 0x004935e8 | hand port `PortTop_ImgRead_004935d0` |
| 0x0052cb00 | BMP reader: RwStream, 'BM' magic, RwImageCreate (`re/match/s23/0052cb00.c` header) | **RW toolkit, librw provides it:** bound to `rw::readBMP` |
| 0x0052d530 | BMP writer: `0x4cc230(2, 2, name)` at 0x0052d53e, bytes 0x42 / 0x4d at 0x0052d56d / 0x0052d57f, returns the image at 0x0052d7c5, 0 when the open fails at 0x0052d7d0 | bound to `rw::writeBMP`, returns the image |
| 0x00514330 | PNG reader: `0x4cc230(2, 1, name)` at 0x0051434b, then 0x5173f0 with "1.0.12" (0x00620e1c) | bound to `rw::readPNG` |
| 0x00513f90 | PNG writer: `0x4cc230(2, 2, name)` at 0x00513fb6, then 0x514d00 with "1.0.12". A null name or image skips the write (0x00513fa1 / 0x00513fa9). Returns the image at 0x005142bd | bound to `rw::writePNG`, returns the image |

The four bindings are in `LibRw/RwApiBind.cpp`. The two readers check that the file opens first, because `rw::readPNG`
asserts it (`deps/librw/src/png.cpp`).

**Not a duplicate of librw's own registry.** librw keeps its own format list. It registers tga / bmp / png with these
same four functions at `rw::Engine::init` (`deps/librw/src/engine.cpp:320-322`). The list at image module +0x21c is the
RW 3.x-layout list that the matched 0x004ce8e0 (`re/match/s20/004ce8e0.c`) walks, so 0x004ce790 runs as its matched body
on the RwGlobals record. Neither 0x00493600 nor 0x004ce790 nor the two wrappers had an exe body before, so rva-lint
counts no new pair. Both new TUs are exe-only and carry the ORIGTOP-ONLY marker, so a later hand body of any of these RVAs
would land in the class.

**When it runs.** 0x00493710 calls 0x00493600 right after RwEngineStart (`probe_mashed_re.log` line 180). The RwGlobals
record and its modules come later, after piece 4 (line 319). In the original the RW engine has opened its modules by
then. So with the knob ON, the call site `PortTop_Bound_00493600` records the call and returns 1. The original
0x00493710 discards the value too (`re/match/s15/00493710.c:128`). Then `RegisterRwModules` runs `Orig_00493600` once,
after it has set up the image module, the freelist headers and `*(0x007d4628) = 0x170`. With the knob OFF the call site
returns 1 without registering, as the old missing stub did.

**Other edits.**
- `Match/PortTop_RdataSeed.inc` is regenerated (`gen_rdata_seed.py` over the same sources plus 00493600.c). It gains the
  two 4-byte rows 0x005cf748 "png" and 0x005cf74c "bmp".
- `verify/porttop_20261108/BINDINGS.tsv`: 0x00493600 goes from missing to matched, plus 7 new rows (0x004ce790 matched,
  0x004935a0 / 0x004935d0 port, 0x0052d530 / 0x00513f90 / 0x0052cb00 / 0x00514330 bound).
- `Match/PortTop_00493710.cpp`: `FUN_00493600` -> `PortTop_Bound_00493600`, and the missing stub is gone.
- `mashedmod/exe_sources.rsp`: the two new TUs.

**The node list as the exe holds it** (probe log lines 319-321):
```
image formats via 0x00493600 ran (1), image +0x21c = 0x10EA6450
  image format node 10EA6450: ".png" / ".PNG" read 00167B00 write 001BA730 next 10EA6390
  image format node 10EA6390: ".bmp" / ".BMP" read 00167AE0 write 001BA660 next 00000000
```
`mashed_re.map`: 0x00167b00 `PortTop_ImgRead_004935d0`, 0x001ba730 `MashedRw_RtPNGImageWrite_00513f90`, 0x00167ae0
`PortTop_ImgRead_004935a0`, 0x001ba660 `MashedRw_RtBMPImageWrite_0052d530`. The nodes are in head-insert order, png
before bmp, as 0x004ce790 links them.

**[UNCERTAIN] items.**
- No run here calls a node function. 0x004ce8e0's only caller, 0x00498810 (screenshot, `re/match/s16/00498810.c`), sits
  under a stand-in (BINDINGS.tsv 0x00498860 under-stand-in). Its image comes from 0x004cdca0 (RwImageCreate, unbound), and
  its pixels from 0x004d52d0 (+0x60, unbound). Evidence missing: a knob-path run that reaches 0x00498810. The verdict is
  static plus slot values, as for MODULES / STRINGS / DEVSLOTS.
- The bindings take the image as `rw::Image*`. This holds once 0x004cdca0 binds to librw's image create, as the rest of
  the RW API binding layer does. Evidence missing: that binding.
- The original opens the file through RwStreamOpen, which goes through the RtFS piz file system. librw's readers and
  writers use `rw::engine->filefuncs`, which is fopen on the disk. Evidence missing: a knob-path caller of the read
  functions. The texture reader the knob path uses, `MashedRw_TextureRead_004c5cb0`, has its own piz path and does not
  walk +0x21c.

## Arms (PREREG)

| arm | result |
|---|---|
| A1 knob boot | PASS on the first try, no retry needed. rc 0, 7 samples, RwGlobals record at `0x00ffb368` from 1.5 s. Log lines 180 / 318 / 319 above. An earlier probe of the first code commit (`dd2cb7e8c`, before the node-list log line) also passed: rc 0, OK 69 / 3, the same 4 changed rows. Both runs are kept. |
| A2 measure | PASS. OK 69 / needs binding 3. Released 0x004ce790, 0x004ce8e0. Seed 112 / 13, the deviation above. |
| A0 gates | PASS, exe sha8 `df80fa32`. H_DEF step `8284be46`, gates `67931834`, ptrace `31fe043e`. B_DEF_K0 / K1 / K26 `1d5bf4f9`. |
| A3 captures | PASS. OFF@0 ON@0 OFF@0 ON@0 with RWMOD=1: rc 0 x4, freeze `pinned pos=55 hash=28538c74`. All 12 hashes are equal across the four runs and equal to the PORT-OT-RWG-DEVSLOTS runs (walk_04 `4d53d93f87f0332d`). |
| A4 lint | PASS. `[rva-lint] OK -- no NEW duplicate bodies (2 known)`, `ORIGTOP-ONLY=0 ... 2 marked TU(s)` (the two new TUs), in build.bat and standalone. |

**Process note: one build ran during a merge-train PRIORITY-HOLD.** At 08:36 `mashed_lock.py status` showed
`PRIORITY-HOLD ... merge-train:20261110_083627`, and my command ran `build.bat` right after it in the same call without
gating on the output. That was the node-list rebuild. WORKER_RULES says no `build.bat` while a PRIORITY-HOLD is shown.
All game runs went through `mashed_lock.py run`, and they queued behind the train's solo window.

**orch/integration moved during the step** to `5e1069705`. `git merge-tree` against this branch reports no conflict.
Note for the train: PORT-COLOURSEL-ICON-FLIP (in integration) moves the walk_04 reference to `ba34030d1b7e2fe4`. The
captures here are from this branch's base, before that flip.

## Remaining 3 needs-binding rows

| row | blocker | original |
|---|---|---|
| 0x004d52d0 | +0x60 | `0x004de1b0`. Needs RW-layout rasters on the knob path, or a 0x004d52d0 binding on `rw::Raster` |
| 0x0047ade0 | +0x0 | the original holds 0 here too |
| 0x004c1680 | data +0xbc | heap `0x03659e7c` in the original |

## Evidence (raw data in `D:/MashedWT/PORT-OT-IMAGE-FMT_data`, not committed)

| file | SHA-256 |
|---|---|
| probe.json (exe df80fa32, ORIG_TOP + RWMOD, rc 0) | 19f1ccaab1eea7e3deede6bed3db461939e56325a0bbe548db6320bcbd8cfd6c |
| probe_mashed_re.log (lines 180, 318-321) | 7a1997a94543c0e034055ee905ea691b01bc6c5fcd2720dc2ef0daf97254be93 |
| probe_run.log | 2b0dbc3fef764eb7c2bf87b97dd7ce732934907739bae37828d52b72b54a2bed |
| probe_v1.json (exe e8d61360, first code commit, rc 0) | 111c4fab3a2872bdf49ba42784cf6e3ec91f0fd84837ecdee4567e65e1929803 |
| probe_v1_mashed_re.log | a3d3d8670b8aede64e09e1a3c539f63f8d8b06e19b1b60117c2a8a8118a9610b |
| OT_MEASURE_before.tsv (the DEVSLOTS table) | 4642f9c09650ab615a1ab3c2485fb463f4ec5b9bab997b7e466c7c20d86032c9 |
| arms_run.log (A0, A3) | e2655a27b312d55b9ae386e1ed2af67d45de83cb0734e1b37f1cdfbf6ed48fdc |
| cap/runs.tsv (A3) | 751071ea0ab6a42ac6aaa3193f39d7308f5a0663c0fa41137bdb74040af76244 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| mashed_re.exe (built from e9605e3fc) | df80fa32c7fc188c6af63e9c75823c736e88094b1a5689f0018b419693ce3d2f |
| mashed_re_v1.exe (built from dd2cb7e8c) | e8d613601c00698c953be2fcab17805359143600543f525d594f226a7085442f |
| build.log | 0b675202fb04d6bdfd074296a41d276b05078242c3b8ec1582bd5ecaa0319060 |
| ../PORT-OT-RWG-MODULES_data/orig/orig_probe.json (original values, --orig) | ae6d8a62f6ec97dc8f530f15796a8d4649864ab07cf7fa3ad6275a968b1cf2c2 |

## REPORT

```
REPORT PORT-OT-IMAGE-FMT
verdict: GREEN
branch: orch/PORT-OT-IMAGE-FMT @ <RESULT commit>
evidence: verify/portorigtop_20261109/RESULT_PORT-OT-IMAGE-FMT.md
gates: G-CTRL 8284be46/67931834/31fe043e  B 1d5bf4f9/1d5bf4f9/1d5bf4f9
decision: none
next: 0x004d52d0 (+0x60) needs a raster-layout decision; 0x004c1680 data +0xbc; screenshot path 0x00498860 needs 0x004cdca0 / 0x004cdd00 / 0x004cdd60 bound to librw images
```
