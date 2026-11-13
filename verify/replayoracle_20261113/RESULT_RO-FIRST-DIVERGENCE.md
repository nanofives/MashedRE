# RESULT RO-FIRST-DIVERGENCE (2026-11-13): first-divergence report, exe ORIG_TOP replay vs the original

Row: replay oracle 9. Kind: top-down diagnosis row (no oracle gate exists yet). Run 43: worker on claude2 (moved to
claude3), lead cmwezbsdu03y9lh1nk6vyjtuv, stopped by L0 STOP. Run 44 (resume): worker cmwfgse0r2n58lh1nhk0h3mx2 merged
`orch/integration` `92fa3f30c` (`e4c5322d2`), committed the PREREG alone (`26a104935`) and built both targets. That worker
stopped on a claude2 policy prompt, and worker cmwfi9nh82r0qlh1n1sn5oz9a (claude3, lead cmwf9zx6d1ljulh1nd88fk6k9) ran the
arms. **Status: done, row to review.** The run-44 section comes first. The run-43 text below it is unchanged history,
except where run 44 corrects it.

# Run 44 (2026-11-13): the PREREG arms

PREREG: `verify/replayoracle_20261113/PREREG_RO-FIRST-DIVERGENCE.md` (`26a104935`). Exe: object build
`mashedmod/build/ddkobj/mashed_re_ddk.exe` SHA-256 `ca1854fc35c282b2c621e4e7115ed09b7f10f3b28cead211b2a62dfedd5d20cd`,
built on the merged tree by the previous run-44 worker (log `D:/MashedWT/RO-FIRST-DIVERGENCE_build_ddk3.log`). Root
`D:\MashedWT\RO-FIRST-DIVERGENCE_root` with `original\mashed_image.bin` staged (SHA-256 in the raw list). test-slot was
outside with `PROYECTOS_TEST_HUB=off`, mashed_lock was inside, and the driver was `firstdiv_arms.sh`. Masks:
`re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv`.

## Verdict

**GREEN for the diagnosis: all three questions are answered with evidence. Nothing is fixed, and nothing is filed.**

| question | answer |
|---|---|
| (a) `rec` field | **PASS on both sides.** Exe: the planted write carries tick 400 and `rec` 399, and it is the last writer in frame `rec == 399`. Original: the first `0x409fffff` store to `0x00898c00` is pc `0x00432763` with `rec` 17, in frame `rec == 17` (record 17 = tick 18). |
| (b) image ON, record 0 | **Record 0 still diverges.** FILEINIT-MISSING `.data` goes from 47,577 dwords to 0. Counted blocks go from 536 to 260, and VALUE dwords from 80,081 to 32,942. The new first divergent dword is `0x00604604` (BOOT-WRITE): the original writes it at pc `0x0045d6a8` in FUN_0045d460 before tick 1, and the exe does not. The first NEW block is unchanged: `0x00898c00` at record 17, IDENTICAL-FOR-N 17. |
| (c) `0x0043dfd0` / `0x004325c0` in the exe | The stand-in for `0x0043dfd0` runs once per frame up to frame 300. **`0x004325c0` never runs in the exe's ORIG_TOP replay menu:** over 600 ticks there are 0 writes at `Anim_Tick` pcs on `0x007f1004` and on `0x0067eca4`, and 0 bridge writes on `0x00898c00`. The only exe writer of `0x00898c00` is `_MenuEntryArrayInit+0xf`, once, at tick 0, with value 0. |

## Self-tests

- **Exe trace and replay self-tests** (lines in `mashed_re.log`, kept in `firstdiv/arms_run44*.out`):

| arm | `[TRACE]` self-test | `[REPLAY]` self-test |
|---|---|---|
| A1 `RO_DEF-fdref2` | PASS, 0 of 44,392 | PASS, 0 of 666 |
| A2 `RO_RONEG-fdneg2` | PASS, 0 of 44,392 | PASS, 0 of 666 |
| A1b `RO_DEF-fdbr` | PASS, 0 of 33,294 | PASS, 0 of 666 |
| A4 `RO_ROIMG-fdimg` | no log line (AV before the finalizer, see below). `meta.json` `selftest` checks 27,745, fails 0 | no log line |

- **`ddkobj.py check`** on this exe, re-run by this worker: IDENTICAL 1196/1223, DIFFERENT 12 (`0x00401690 0x004332a0
  0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00`),
  not-in-map 15, units linked 1171/1445, refusals 4 (wrapper-redefines-globals). This is the same as the build log. Run 44
  changes no exe code.

## Arms run

| arm | side | config | watch | outcome |
|---|---|---|---|---|
| A1 `RO_DEF-fdref2` | exe ddk | default RO arm, ckpt 0,1,17,18,299,301,399,400,600 | `0x007f0ff0` | exit 0, 600 records, 603 hits |
| A2 `RO_RONEG-fdneg2` | exe ddk | stack `RONEG`, same ckpts | `0x007f0ff0` | exit 0, 600 records, 604 hits |
| A4 `RO_ROIMG-fdimg` | exe ddk | stack `ROIMG` (`MASHED_PORTSTATE_IMAGE=1`), ckpt 0,1,17,18,299,301,600 | `0x005ea004,0x00898c00,0x007f1004,0x0067eca4` | 600 frames, then exit `0xC0000005` in AppDestroy (log `Calling AppDestroy`, IMG-4's AV). See "A4 data" |
| A1b `RO_DEF-fdbr` (added) | exe ddk | default RO arm (image OFF), ckpt 0,1,17,18,299,301,600 | same 4 words as A4 | exit 0, 1,203 hits written. Gives (c) the watch that A4 lost |
| A3 `o4` | original | matched replay, `--ckpt-ticks 17,18` | `0x00898c00` from the install | rc 0, 600 records, 531 hits |
| `o5` (added) | original | matched replay, `--ckpt-ticks 0,1` | `0x0068f800,0x008b1400` from the install | rc 0, 600 records, 165 hits. Names the writers of the image-ON record-0 blocks |
| A5 `RO_ROIMGPSF` | | | | **not run**: the PREREG runs it only if A4 shows the bridge runs, and it does not |

**A4 data.** The trace finalizer (`ReplayTrace_Finish`, reached from `Replay_Summary`, `Match/PortTop_Replay.cpp:550`)
runs after AppDestroy. The AV there therefore lost `watch.jsonl` and the tail of `ticks.bin`. The file holds 12,582,912
bytes, which is 566 complete records of 22,204 bytes. `meta.json` is complete (ticks total 600, watch hits 1201, self-test
0 of 27,745), and so are all 8 checkpoint dumps (each is closed when written). The reports compare A4 on records 0..565
(`records compared: 566`). That covers record 0 and record 17. The bridge question moved to A1b. A1b uses the same exe
with the image OFF, and the image does not affect the bridge's call path (see (c)).

## (a) The `rec` field

- **Exe, A1 vs A2** (`firstdiv/fd_neg2.md`): the first divergent record is 399 (tick 400) in block `0x007f0c00`. Dword
  `0x007f0ff0` is `0x00001356` (ref) against `0x00001357` (run). The blocks that differ at every record (`0x00772c00`,
  `0x007e9400`) are EXPLAINED at tick 1. Writers in frame `rec == 399`:
  - ref: `_PortTop_Body_00493390+0xc9`, value `0x1356`.
  - run: the last writer is `_ReplayTrace_Tick+0x85`, hit tick 400, `rec` 399, value `0x1357`.
  - The next body write in the run (tick 400, `rec` 400, value `0x1389`) falls in the next frame.
  - The run-43 gap ("planted write not inside the frame") is closed. The exe's `tick` field runs one ahead of `rec` for
    writes after the tick counter moved. `rec` is the frame key.
- **Original, A3 `o4`** (`firstdiv/watch_tally_run44.txt`, `firstdiv/fd_o4_RO_DEF-fdbr_t18.md`): the first hit that sets
  `0x00898c00` to `0x409fffff` has pc `0x00432763`, `tick` 17 and `rec` 17. The report names it as the only writer in frame
  `rec == 17`, and record 17 (tick 18) holds `0x409fffff`. On the original side `tick == rec` for these hits. The
  prediction holds, with no off-by-one.
- `o4` hits on `0x00898c00`:
  - pc `0x0042d3ef`, 1 hit at tick 16, value 0 (a same-value store; run 43 did not list it because o2 armed its watch
    elsewhere).
  - pc `0x00432763`, 283 hits.
  - pc `0x00432777`, 247 hits.

## (b) Image ON: record 0

Reports `firstdiv/fd_o2_RO_DEF-fdref2.md` (image OFF) and `firstdiv/fd_o2_RO_ROIMG-fdimg.md` (image ON), both against o2
(original, ticks 0/1/17/18 dumped in run 43). Both runs re-hash OK against record 0.

| | image OFF (A1) | image ON (A4) |
|---|---|---|
| first divergent record | 0 (tick 1) | 0 (tick 1) |
| counted blocks at record 0 | 536 | 260 |
| first block / first VALUE dword | `0x005ea000` / `0x005ea004` | `0x00604400` / `0x00604604` |
| VALUE dwords at tick 1 | 80,081 | 32,942 |
| FILEINIT-MISSING `.data` | 47,577 (first `0x005ea004`) | **0** |
| FILEINIT-MISSING `.data` (bss) | 25 (first `0x007032ec`) | 25 |
| BOOT-WRITE `.data` | 224, all run 0 (first `0x00604604`) | 1,140, of which 224 are run 0 (first `0x00604604`) |
| BOOT-WRITE `.data` (bss) | 31,663 (first `0x0063bd98`) | 31,650 |
| BOOT-WRITE `_rwdseg` | 2 (`0x00915000`) | 2 |
| OTHER `.data` | 465 (first `0x006147dc`) | 0 |
| OTHER `.data` (bss) | 125 (first `0x0067d974`) | 125 |
| first NEW block (detorig_ticks verdict) | `0x00898c00` at record 17, IDENTICAL-FOR-N 17 of 600 | `0x00898c00` at record 17, IDENTICAL-FOR-N 17 of 566 |

- Four dwords read from the tick-0 / tick-1 dumps:

| dword | o2 t0 | o2 t1 | A1 t0 / t1 | A4 t0 / t1 |
|---|---|---|---|---|
| `0x005ea004` | `004a57e4` | `004a57e4` | `00000000` | `004a57e4` |
| `0x006147dc` | `00493b50` | `00493b40` | `00000000` | `00493b50` |
| `0x00604604` | `00000000` | `00000004` | `00000000` | `00000000` |
| `0x0068f800` | `00000000` | `0d779458` | `00000000` | `00000000` |

  The image supplies the file initializers. What is left is what the original writes between its PE entry (tick 0) and
  tick 1.
- The 916 BOOT-WRITE `.data` dwords with a non-zero run value are dwords where the exe now holds the file initializer and
  the original changed it before tick 1. Example: `0x006147dc`, `0x00493b50` → `0x00493b40`. With the image OFF these
  were counted as FILEINIT-MISSING / OTHER, because the run held 0 there.
- **Writers of the residue, in the original, all on the main thread (tid = `meta.boot.main`) at tick 0:**
  - `0x00604604`: pc `0x0045d6a8` in FUN_0045d460 [audio] (hooks.csv C2), backtrace `0x0045d6a8 | 0x004669fb |
    0x004028e0` (o2, run 43).
  - `0x0068f800`: one store at hit pc `0x0045d600`, value `0x0d8f9458` (o5). The hit pc lies between FUN_0045d460's entry
    and the `0x0045d6a8` site above. [UNCERTAIN] which instruction stores it: Frida reports the trap eip, and the
    function's extent was not read from Ghidra. The block holds stride-8 pairs: heap pointer (`0x0d77xxxx`, step `0x98`)
    and float `0x3f79999a`.
  - `0x008b1400`: 153 stores at pc `0x004b6b88` (FUN_004b6b20 [render] C2), 78 of them at tick 0 and 75 during ticks
    1..600. Backtrace `0x004b6b88 | 0x004b6e3e (FUN_004b6b90) | 0x00550970 (RwsStreamRead)`, or through `0x004cbd67`
    (RwStreamRead). Also 11 stores at pc `0x004b69b4` (FUN_004b6940). The word changes with each read through that
    layer. The exe reads piz through its own PIZ_FS (log: `0x004b6560 callback ... not called by the exe reader`), so it
    leaves the block at 0.
  - The exe has no write to any of the three words before tick 1. For `0x005ea004` the A1b watch has 0 hits over 600
    ticks.

## (c) `0x00898c00` and the `0x004325c0` bridge in the exe

- **Stand-in count** (A4 and A1b logs, same values): `frame 0 ... 0x0043dfd0 stand-in calls 1`, frame 120 → 121,
  frame 240 → 241, frame 360 → 300, frame 480 → 300. That is one call per menu frame through frame 299, then none after
  the warp (phase 3).
- **The stand-in does not reach `0x004325c0`.**
  - `PortTop_StandIn05_0043dfd0` (`Match/PortTop_StandIns05.cpp:24`) runs `PortTop_HandFrameMenu` and
    `PortTop_HandFrameInputGlobals`.
  - The exe's only caller of `MenuAnimTickTwin` (0x004325c0) is `Anim_Tick` (`Frontend/MenuNavSM.cpp:824`), through
    `Nav_AnimTick`, called from the hand `RenderFrame` at `exe_main.cpp:5600`.
  - That call sits inside `if (g_bridge_installed && (g_frontend_phase == 2 || g_frontend_phase == 3))` at
    `exe_main.cpp:5532`.
- **Watch evidence (A1b, `firstdiv/watch_tally_run44.txt`).** `Anim_Tick` writes `0x007f1004` and `0x0067eca4` twice per
  call. With `MASHED_PORTSTATE_FRONTEND` OFF it also memcpys records 0..29 over `0x00898ac0..0x008990d7`, which covers
  `0x00898c00`. Hits over ticks 1..600:
  - `0x007f1004`: only `_FpsDiscretise+0xb1` (600 hits) and `_PortTop_Body_00493390+0x98` (600 hits).
  - `0x0067eca4`: only `_RaceArmReset+0x13`, once, at tick 0.
  - `0x00898c00`: only `_MenuEntryArrayInit+0xf`, once, at tick 0.
  - So `Anim_Tick` did not run.
- **The hand `RenderFrame` does run.** Log: `frame 0 rendered (1)`, then frames 120, 240, 360 and 480. The bridge is
  installed (`Milestone B15: RW Im2D->D3D9 bridge install = OK`).
- [UNCERTAIN] which part of the `exe_main.cpp:5532` gate is false in the replay arm, or whether `RenderFrame` returns
  before that line. Missing evidence: a log or watch of `g_frontend_phase` (`exe_main.cpp:653`) during the replay menu.
- **The exe has no writer for the `0x00898c00` block during the menu.** The `--tick 18` reports with the image OFF and ON
  (`firstdiv/fd_o4_RO_DEF-fdbr_t18.md`, `firstdiv/fd_o4_RO_ROIMG-fdimg_t18.md`) give the same 20 dwords, stride `0x34`,
  `0x00898c00..0x00898fdc`: ref `0x409fffff`, run `0x00000000`.

## Where a fix belongs (named, nothing filed)

1. **Record-0 BOOT-WRITE class** (the first divergence once the image is ON): the ORIG_TOP boot rows. The original's boot
   runs these before tick 1, and the exe's ORIG_TOP boot does not:
   - FUN_0045d460 [audio] (`0x00604604`, `0x0068f800`).
   - The `0x004b6xxx` file layer under RwStreamRead / RwsStreamRead (`0x008b1400`).

   The other choice is RO-ORACLE-GATE masking this class until the boot is matched. That is the decision already listed
   as open item 3 of run 43. It is a decision row, not this row's call.
2. **`0x00898c00` at record 17**: the `0x0043dfd0` body (provisional `Match/ProvS08_0043dfd0.cpp`, PORT-TOP-08 lane). The
   original reaches `0x004325c0` only through it (Ghidra caller list, run 43), and the stand-in path never calls the body
   in the replay menu. `MASHED_PORTSTATE_FRONTEND` (PORT-STATE-01) then decides whether the records live at
   `0x00898ac0`. A5 measures that once the body runs.
3. **Harness:** the replay trace finalizer runs after AppDestroy, so every image-ON run loses `watch.jsonl` and the
   `ticks.bin` tail until IMG-4 fixes the teardown AV. Alternatively, a trace row could flush at the frame cap.

## Tool change in this run

`re/frida/detorig_firstdiv.py`: constant blocks are now EXPLAINED only at a tick ≥ 1 that both runs dumped. Before, the
earliest common dump was used, and with the tick-0 checkpoints added in run 43 that is tick 0, where no record exists. On
the original side tick 0 also comes before the boot writes. As a result, the first reports of this run marked 229
(image OFF) and 258 (image ON) record-0 blocks as explained, which hid VALUE dwords such as `0x00604604` (ref 4, run 0).
Every report committed in this run comes from the fixed tool. The self-test report still gives record 399 with the same
dwords and writers.

## Evidence (run 44)

- Committed:
  - `firstdiv/fd_neg2.{md,json}`, `firstdiv/fd_o2_RO_DEF-fdref2.{md,json}`, `firstdiv/fd_o2_RO_ROIMG-fdimg.{md,json}`,
    `firstdiv/fd_o4_RO_DEF-fdbr{,_t18}.{md,json}`, `firstdiv/fd_o4_RO_ROIMG-fdimg{,_t18}.{md,json}`.
  - `firstdiv/watch_tally_run44.txt`, `firstdiv/arms_run44{,_b,_o5}.out` (queue lines stripped).
  - The stack files `verify/savelink_20261023/stacks/ROIMG.txt` and `ROIMGPSF.txt`.
  - `raw_sha256_RO-FIRST-DIVERGENCE_run44.txt`.
- Raw, not committed: `D:/MashedWT/RO-FIRST-DIVERGENCE_data/{RO_DEF-fdref2,RO_RONEG-fdneg2,RO_ROIMG-fdimg,RO_DEF-fdbr,o4,o5}`,
  the full dword lists `fd_o2_RO_DEF-fdref2_full.json` / `fd_o2_RO_ROIMG-fdimg_full.json`, the arm logs, `RUNLOG.txt`, and
  the staged `mashed_image.bin`. Their SHA-256s are in `raw_sha256_RO-FIRST-DIVERGENCE_run44.txt`.
- Gates: not run (top-down diagnosis row). No 12 captures. No tracker, ROADMAP or `.asi` edit. Nothing pushed.

# Run 43 (history)

## Verdict so far

**The tool works and the first divergence is found. Two items are still open (end of this file).**

- **Self-test (tool):** the planted divergence (`RONEG`: +1 on `0x007f0ff0` at tick 400, exe object build) is reported
  at record 399 = tick 400, block `0x007f0c00`, dword `0x007f0ff0`, `0x00001356` (ref) vs `0x00001357` (run), equal at
  tick 399 (`0x00001324` both). Every dump re-hashes to its own tick record (OK on all 4 dumps). The two blocks that
  differ at every record between two exe runs (`0x00772c00`, `0x007e9400`, the RULE:HWND words) are classed EXPLAINED at
  tick 399 and not counted. **PASS.** One gap, fixed in source but not yet re-run: with tick-based frames the planted
  write itself (pc `0x1a2de5` = `_ReplayTrace_Tick+0x85`, value `0x1357`, tick 400) is not inside "the frame tick 399 ->
  400". The `rec` field (below) makes the frame exact.
- **Self-test (exe trace):** `[TRACE] self-test PASS` on all 5 exe arms (0 of 16647 checks on fd1 / fd2 / fdvs, 0 of 11098
  on fdref / fdneg), `[REPLAY] self-test PASS (0 of 666)` on all 5.
- **Watch self-test (exe):** `0x007f1030` watched on fd1: 604 hits, 600 of them at one pc (`0x18b0f` =
  `_PortTop_Body_00493390+0xcf`, the matched 0x00493390 body), one per tick 1..600, the rest at boot (`_Store7f1030_Qpc3Mhz`,
  tick 0) and at the warp (tick 300). With 4 slots: `dr7 0xDDDD0055`, armed rc 0 on the main thread.

## The first divergence (exe object build vs the original, matched replay configuration)

Reference: the original, boot-mode replay matched to `replay_trace.json` (q0 `0x40000000`, warp at tick 300 with clock
300, 600 records): runs o1, o2, o3b. Run: the exe object build with `MASHED_REPLAY` + `MASHED_REPLAY_TRACE`: RO_DEF-fd1,
RO_DEF-fd2. Masks `re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv` (21 blocks wholly masked). Report:
`firstdiv/fd_o1_fd1.md`, `firstdiv/fd_o2_fd2.md`, `firstdiv/fd_o3b_fd2.md`.

### 1. Record 0 (tick 1): the exe image pad

- **First divergent record: 0 (tick 1).** 537 counted blocks differ at every one of the 600 records. The differences are
  already there at tick 0 (original: the PE entry point, `--ckpt-ticks 0`; exe: the trace init in `Replay_Init`).
- **First block `0x005ea000`, first dword `0x005ea004`:** ref `0x004a57e4` at ticks 0 and 1 (= the file initializer in
  `.data` of `MASHED.exe.unpatched`), run `0x00000000` at ticks 0 and 1. No absolute-operand reference in `.text`.
  Writers: exe none (watch armed from the trace init, 0 hits in 600 ticks, fd2). Original: no write that changed the
  value before tick 1 (o3b). One same-value write was seen at pc `0x00492502` (in FUN_004924f0 [boot]), value
  `0x004a57e4`, at tick 0. [UNCERTAIN] which of the two watched dwords it stored to: o3b watched
  `0x005ea004` and `0x007f1030`, Frida's context has no DR6, and neither value changed.
- **Dword census at tick 1** (the 537 blocks, both dumps re-hash OK against record 0): 81,067 differing dwords. 80,116
  VALUE, 10 IMGPTR, 317 PTR, the rest MASK / RULE rows. The VALUE dwords split against the original's file image:

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| FILEINIT-MISSING (ref = file initializer, run not) | .data | 47,577 | 47,576 | `0x005ea004` |
| FILEINIT-MISSING | .data (bss) | 25 | 0 | `0x007032ec` |
| BOOT-WRITE (run = file initializer, ref not: the original wrote it before tick 1) | .data | 224 | 224 | `0x00604604` |
| BOOT-WRITE | .data (bss) | 31,698 | 31,698 | `0x0063bd98` |
| BOOT-WRITE | _rwdseg | 2 | 2 | `0x00915000` |
| OTHER | .data | 465 | 465 | `0x006147dc` |
| OTHER | .data (bss) | 125 | 0 | `0x0067d974` |

- The source states the cause of the FILEINIT-MISSING class: `mashedmod/src/mashed_re/Track/CourseTableState.cpp:4-6`:
  "file-backed .data in the original ... The exe's image pad (exe_main.cpp B17) holds them zero-filled". The run holds
  0 in 47,576 of the 47,577 `.data` dwords.
- **First BOOT-WRITE dword `0x00604604`:** ref `0x00000004`, run `0x00000000`. Original writer: pc `0x0045d6a8` in
  FUN_0045d460 [audio] (hooks.csv C2, PERMDICT.RWS loader), at tick 0, backtrace `0x0045d6a8 | 0x004669fb | 0x004028e0`
  (o2). No absolute-operand reference (`xref.py --string 0x00604604`: none). Exe: 0 hits.

### 2. First NEW block: `0x00898c00` at record 17 (tick 18) = IDENTICAL-FOR-N 17 of 600

- The detorig_ticks.py verdict on the same pair. The block is equal on both sides at records 0..16.
- 20 dwords, stride `0x34`, `0x00898c00 .. 0x00898fdc`. At tick 17: 0 on both sides. At tick 18: ref `0x409fffff`
  (float 4.9999995), run `0x00000000`. BSS (file initializer 0). No absolute-operand writer.
- **Original writer (o2, watch from the install):** pc `0x00432763` in FUN_004325c0 [frontend] (hooks.csv C4, `impl`,
  `Match/MatchS08_004325c0.cpp`, notes "menu slide anim tick"), the only hit in frame 17 -> 18. 530 hits on the word in
  all: `0x00432763` 283 (ticks 17..299) and `0x00432777` 247. Ghidra caller of FUN_004325c0: FUN_0043dfd0 only (its exe
  source is the provisional body `Match/ProvS08_0043dfd0.cpp`).
- **Exe writer (fd2):** one hit in 600 ticks, at tick 0, pc `0x1b43f` = `_MenuEntryArrayInit+0xf`, value 0. Nothing writes
  the word during the menu. [UNCERTAIN] whether the exe ORIG_TOP path calls 0x0043dfd0 / 0x004325c0 at all. Evidence
  needed: a watch or a call counter on the exe body `_MenuAnimTickTwin`.
- The original side is deterministic here: o1 vs o2 have no NEW block before record 301 outside `0x00614400`
  (`0x006147d8`, MASK:os). Raw timer words `0x007f1030` / `0x007f1010` / `0x007f102c` differ from record 0 (boot-call-count
  dependence before the warp pin). o1 vs fd1 and o2 vs fd2 give the same record-17 block and values.

### 3. VS2022 build

RO_DEF-fdvs (VS2022 exe, sha8 `c84c3a41`) vs o2 gives the same answers: record 0 with 537 blocks, the same census, NEW
block `0x00898c00` at record 17. fd2 vs fdvs: IDENTICAL-FOR-N 600 of 600, 5 constant blocks (exe-image addresses, as
RO-EXE-TICK-TRACE found). Writers resolve through `mashed_re.map` (`_MenuEntryArrayInit+0x28`, `_Store7f1030_Qpc3Mhz+0x24`).

## What was built

- `re/frida/detorig_firstdiv.py` (new). Inputs: two tick-trace directories in the detorig format (original or exe).
  - It finds the first record with a counted block or raw word different. Wholly masked blocks are left out, and so are
    constant blocks EXPLAINED at a tick both runs dumped.
  - It re-hashes the tick c - 1 / c dumps against their records.
  - It lists the differing dwords with both values at c - 1 and c, the class (MASK / RULE, PTR, IMGPTR, VALUE), the file
    initializer and section, and the static referrers. The referrers are absolute operands with read / write access,
    named from the Ghidra facts and hooks.csv.
  - It gives a census by FILEINIT-MISSING / BOOT-WRITE / OTHER, and the writers from `watch.jsonl` (exe pcs through
    `--exe-map`).
  - When the evidence is missing it prints a re-run plan. Options: `--tick`, `--new-only`, `--masks`, `--ignore`,
    `--json`, `--md`.
- `re/frida/detorig.py` / `js/detorig.js`:
  - `--ckpt-ticks` (boot mode, k = 200 + i, tick 0 = at the install). Checkpoint fields that cannot be read before the
    CRT / RW exist are null.
  - `tick` in checkpoints and watch hits.
  - `rec` (records written) in watch hits. **Not yet run.**
  - In `w` mode the agent's own pins at the warp / anchor run with this thread's watch suspended. o3 failed without this
    (`onDue Error: single-step triggered at pinTimer`). o3b passed with it.
- `mashedmod/src/mashed_re/Match/PortTop_ReplayTrace.cpp` (exe only, under the already default-OFF `MASHED_REPLAY_TRACE`):
  - `MASHED_REPLAY_TRACE_WATCH=<va,...>` (up to 4): DR0..DR3 write watch on the main thread, armed by a helper thread,
    with a vectored handler. Hits go to `watch.jsonl`: tick, n, eip, slot, value, ebp-chain, and per-pc counts.
  - `MASHED_REPLAY_TRACE_WATCH_FROM`.
  - `MASHED_REPLAY_TRACE_CKPT` accepts tick 0 (dump at the trace init).
  - `meta.json` `watch`.
  - `rec` in hits: **built (second build: Build OK, the same ddkobj summary, rva-lint NEW=0,
    log `D:/MashedWT/RO-FIRST-DIVERGENCE_build_ddk2.log`), not yet run in an arm.**
- `verify/savelink_20261023/run_arms.py`: `RO_` kind passes `A4_RO_WATCH` / `A4_RO_WATCH_FROM`.
- `verify/replayoracle_20261113/firstdiv_arms.sh`: the arm driver (test-slot outside, mashed_lock inside).

## Builds (first build, the one every arm used)

- `MASHED_DDKOBJ=1` build: Build OK. One build.bat run made both `mashed_re.exe` (VS2022) and
  `build/ddkobj/mashed_re_ddk.exe`.
- rva-lint NEW=0, asm-lint OK.
- `ddkobj.py check`: IDENTICAL 1188/1214, DIFFERENT 11 (`0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00
  0x004929d0 0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00`), not-in-map 15, units linked 1164/1440, refusals 4
  (wrapper-redefines-globals). This row adds no matched unit.
- Log: `D:/MashedWT/RO-FIRST-DIVERGENCE_build_ddk.log`.

## Deviations

- **No PREREG was committed before the arms.** The exe change is harness code under `MASHED_REPLAY_TRACE`, which is
  default OFF and needs `MASHED_REPLAY` + `MASHED_ORIG_TOP`, so the default path does not execute it. WORKER_RULES still
  asks for a PREREG on a new knob, and this row did not write one.
- Gates: not run (top-down row). No 12 captures. No tracker, ROADMAP or `.asi` edit. Nothing pushed.

## Open (next session)

1. Finish the rebuild with `rec`. Re-run RO_DEF-fdref / RO_RONEG-fdneg and confirm that the planted write is the last
   writer in frame `rec == 399`. Re-run one original watch arm to confirm `rec` there.
2. Exe side of `0x00898c00`: does the ORIG_TOP menu call 0x0043dfd0 -> 0x004325c0? Watch or count `_MenuAnimTickTwin`.
3. For RO-ORACLE-GATE: the record-0 classes need a decision (fill the pad from the file image, or mask
   FILEINIT-MISSING / BOOT-WRITE until the boot is matched). That is a decision row, not this row's call.
4. Commit a PREREG for the watch knob if the lead wants one.

## Evidence

- Committed: this RESULT, `firstdiv/` (every report `.md` / `.json`, `ticks_*.json`, `arms_*.out`), and
  `raw_sha256_RO-FIRST-DIVERGENCE.txt`. The latter holds the SHA-256 of each cited raw file: per-arm `meta.json`,
  `ticks.bin`, `watch.jsonl`, the tick dumps, the arm logs and the build log.
- Raw (not committed): `D:/MashedWT/RO-FIRST-DIVERGENCE_data/{o1,o2,o3,o3b,RO_DEF-fd1,RO_DEF-fd2,RO_DEF-fdvs,RO_DEF-fdref,RO_RONEG-fdneg}`.
  Root: `D:/MashedWT/RO-FIRST-DIVERGENCE_root`.
- Exes as used by the arms: object build `70845a28bbe60596eab48013ecb13afe78fd7b3819cbc0362a239c2ede4dc4b6`,
  VS2022 sha8 `c84c3a41` (run_arms.py line). The full VS2022 hash was not taken before the rebuild replaced the file.
