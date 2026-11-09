# RESULT PORT-X87-PC24-AUDIT: which float mode reproduces each matched x87 wrapper

Verdict: **DECISION.** The audit is complete. No `mashedmod/` code changed and no default flipped. PREREG `16ab9667b`
(`PREREG_X87AUDIT.md`, committed alone before any arm). Lead: cmw7xu1960f37p41557tefpnd. Follow-up of
PORT-STATE-03-FPU and L-102.

## 1. Answer

1. **The original runs every matched wrapper at x87 PC_24 once its D3D device exists.** 74 of the 82 reached matched rows
   run only at 0x007f, across 2 boots and about 4.6 M calls. The 6 RVAs at 0x027f all run before device creation (WinMain
   0x00492370, 0x00493900, 0x00498910 / 0x00498950 config load, 0x00499890, 0x00499ba0). The 2 mixed ones straddle it
   (0x00499820, 0x004963e0). No other CW value occurs.
2. **The exe runs the same code at PC_53, except under local PC_24 scopes.** Root cause, from the code:
   `CreateDeviceWatched` (`mashedmod/src/mashed_re/exe_main.cpp:3191`) creates the D3D9 device on a watchdog
   `std::thread`. D3D9's FPU setup lands on that worker, and the game thread keeps the CRT's 0x027f. Measured on the
   worktree build `37ea0a5e`: 15 called RVAs at 0x027f (camera entries, FastSqrt, the 0x0046c5f0 tri-normal,
   config, boot). 4 run at 0x007f because a local PC_24 scope encloses their call site (0x0040e470, 0x00442cc0,
   0x00458920, 0x004282a0).
3. **Which mode matches.** The rules (PREREG section 3) are measured by arm 3:
   - **No PC-sensitive event** (94 of 130 matched rows: 47 `NOFP`, 47 `PC-INSENSITIVE`): x87 @ PC_53, x87 @ PC_24 and
     SSE2 store the same values. This is a proof, so no discriminating input exists. **Keep.**
   - **Float chain held in ST** (36 rows `PC-SENSITIVE`): x87 @ PC_24 matches, and x87 @ PC_53 does not wherever a
     natural delta occurs. SSE2 matches when the data flow is float-only. Measured examples: 0x00428320 (X53 RED 80/80,
     X24 and S GREEN 80), 0x00420d80, 0x00422470, and the control 0x004325c0.
   - **Two findings that are not about PC:**
     **(a)** 0x00443300 `AiSplineEval_x87` (called): the port body differs from the original in **every** mode,
     44/80 calls at PC_24 and in SSE2. The DDK compiler (MSVC 7 `/O2`, no `/Op`) reassociated the Catmull-Rom sum.
     The listing at 0x00443383..0x004433c9 adds `(p1+p1)` last, while the C writes it first. VS2022 `/fp:precise`
     keeps the source order.
     **(b)** 0x00431b20 / 0x00431b50 / 0x00431b60: ST0 is returned unrounded. The raw 80-bit value differs in every
     port mode (80/80, 65/65, 67/67), and the f32 the caller stores is GREEN in every mode.

## 2. MATCH-S08 (first, as asked)

orig CW = arm 1, 2 boots. exe CW = arm 1 exe, Training race + frontend walk. "scan" = static class of the original
body / the exe body. X53 / X24 / S = arm 3, against the original on live-seeded calls.

| RVA | TU | port | orig CW | build | scan | natural deltas | X53 | X24 | S | modes that match | recommendation |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 0x004331a0 | MatchS08_004331a0 | called | not reached | x87 | NOFP / insens. | - | - | - | - | all three (proof) | keep |
| 0x0043c000 | MatchS08_0043c000 | none | 0x007f x1616 | x87 | SENS. / - | 0 / 51 | GREEN 51 | GREEN 51 | GREEN 51 | all three on 51 calls; static: an input exists | keep |
| 0x00442420 | MatchS08_00442420 | none | 0x007f x16842 | x87 | insens. | - | - | - | - | all three (proof) | keep |
| 0x00442c80 | MatchS08_00442c80 | body | 0x007f x44792 | x87 | insens. | - | - | - | - | all three (proof) | keep |
| 0x00442cc0 | MatchS08_00442cc0 | called | 0x007f x6844 | x87 (exe 0x007f) | insens. | - | - | - | - | all three (proof) | keep |
| 0x00442ce0 | MatchS08_00442ce0 | called | 0x007f x22336 | x87 | SENS. | 0 / 69 | GREEN 51 | GREEN 51 | GREEN 51 | all three on 51 calls | keep |
| 0x00442df0 | MatchS08_00442df0 | none | 0x007f x350 | x87 | insens. | - | - | - | - | all three (proof) | keep |
| 0x00443300 | MatchS08_00443300 | called | 0x007f x971318 | x87 | SENS. / SENS. | 56 / 80 | RED 52/80 | RED 44/80 | RED 44/80 | **none** | **fix the body first** (finding a) |
| 0x00449880 | MatchS08_00449880 | none | not reached | x87 | insens. | - | - | - | - | all three (proof) | keep |
| 0x0044dff0 | MatchS08_0044dff0 | none | not reached | x87 | NOFP | - | - | - | - | all three (proof) | keep |
| 0x0044e020 | MatchS08_0044e020 | none | not reached | x87 | insens. | - | - | - | - | all three (proof) | keep |
| 0x0044e050 | MatchS08_0044e050 | none | not reached | x87 | insens. | - | - | - | - | all three (proof) | keep |
| 0x0044e070 | MatchS08_0044e070 | none | not reached | x87 | SENS. (retst0 only) | - | - | - | - | caller-dependent: returns `fld; fmul` unrounded | none needed today (port none) |
| 0x004325c0 | MatchS08_004325c0 (control) | called | 0x007f x1546 | **SSE2** | SENS. | 1 / 50 | RED 1/50 | GREEN 50 | GREEN 50 | x87@PC_24, SSE2 | control holds (L-102 SSE2 confirmed) |

## 3. Every called matched row that is not a plain keep

| RVA | TU | orig CW | exe CW | modes that match | recommendation |
|---|---|---|---|---|---|
| 0x00443300 | MatchS08_00443300 `AiSplineEval_x87` | 0x007f | not reached in the 2 exe runs | none | **PROPOSED step:** port-only expression in the DDK order (finding a). Gate-relevant: AI spline, about 7000 calls/s in the original race |
| 0x0046c5f0 | MatchPilot_0046c5f0 `TriangleFaceNormal` | 0x007f x112204 | **0x027f x874** | x87@PC_24 (original bytes as proxy). x87@PC_53: no, **50/69 natural deltas** | **PROPOSED:** PC_24 (the global knob, section 5). SSE2 not proven. The port legs could not run: the original takes register args, the port is cdecl `(apex, p2, p1, out)` |
| 0x004282a0 | MatchS06_004282a0 `MenuMenusBA` | 0x007f | 0x007f x62 | x87@PC_24 (original bytes as proxy). O53: 33/33 natural deltas | keep: the exe already runs it at PC_24. Port legs VOID (emulator returns NaN-indefinite, artefact not separated) |
| 0x00451ce0 | MatchS09_00451ce0 | not reached | not reached | undetermined (exe scan: PC-sensitive, a `fmul` chain at the port's 0x0018fd56 site) | measure under the Warzone scenario |

Every other called matched row is a keep (35 of the 38). 32 have no PC-sensitive event (proof). 0x00442ce0 and 0x00414060 are
GREEN in all modes on 69 and 2 live calls, with no natural delta. 0x004282a0 already runs at 0x007f in the exe. Full
table, all 169 rows: `X87AUDIT_TABLE.tsv` (columns: rva, tu, port, orig_cw, exe_cw, static_orig, sse2_static,
static_exe, emu_n, natural_deltas, X53, X24, S, S_f32ret, match_modes, recommendation, scope, note).

Rows with no runtime effect today (port=body/none) where x87 @ PC_53 would not match once called: 0x00427680,
0x00427e00, 0x00427f00, 0x00428140, 0x00428320, 0x00428610, 0x004288a0, 0x00428a30, 0x00428d30, 0x00495080, and
ViewportScaledRectDraw 0x00428610. The global knob covers all of them. 0x00428610 is SSE2 `no` (a `dblop`). 6 of these
are `PC-INSENSITIVE` in the original and PC-sensitive only in the exe's codegen (an unrounded ST0 return or `_ftol`
argument), so the static leg rests on the exe scan there.

Not reached in either scenario, so no claim: 18 PC-sensitive matched rows, mostly MATCH-S06 text/draw helpers and S07
`0x0042bde0` / `0x0042f8d0` / `0x0042fb70` / `0x0042fe90`, listed `undetermined (not reached)` in the TSV.

Transcription TUs (CW only, no recommendation, PREREG section 1). The original runs all of them at 0x007f. In the exe,
CameraEntries 0x00441b30 / 0x00441c80 / 0x00442600 / 0x00445aa0 and FastSqrt 0x004c3b30 run at **0x027f**. They are
x87 TUs whose bit-identity was measured in the `.asi`, which runs at MASHED.exe's PC_24 (memory
asm-bit-identity-in-asi-not-standalone-pc24). The global knob covers them too.

## 4. Arms

**Arm 1 (CW).** Native FNSTCW onEnter stubs on all 162 distinct RVAs. 161 attached. 0x00496400 cannot be intercepted:
the `disable_log` patch region. Original: `D:/MashedWT/PORT-X87-PC24-AUDIT_root` (every `*.asi` left out), hooks
OFF, title -> menu -> Quick-Battle race, 2 boots (40 s and 60 s of race), the same 104 RVAs reached in both. Exe: the
G-CTRL Training env for 1200 frames (`MASHED_DETERMINISTIC=1`, `MASHED_GAME_MODE=10`, ...) and the PORT-STATE-03
frontend walk. Both self-exited. Stubs on the 39 `port=called` symbols only: with all 120 linked symbols stubbed, the
exe died at D3D init (both tries). With 1 stub or with the 39, it runs to its own exit, so this is a probe limit and
not a finding. Tables: `X87AUDIT_CW_ORIG.tsv`, `X87AUDIT_CW_EXE.tsv`.

**Arm 2 (static).** `x87_pcscan.py` over `MASHED.exe.unpatched` and over `mashed_re.exe` `37ea0a5e` (map symbols):
`x87_pcscan.tsv`, `x87_pcscan_exe.tsv`. Checked by hand on 0x0044e070 (`fld; fmul; ret` = retst0 R1), 0x00442c80
(`fld; fcomp` of a loaded float = insensitive) and 0x004325c0 (the chains PORT-STATE-03-FPU found).

**Arm 3 (live-seeded emulation).** `x87_live_emu.py`, 2 boots, 40 samples per RVA spread over the run. The two hot
RVAs (0x00443300, 0x00431b20) were attached late in the race for 40 consecutive calls, then detached. Port images:
`x87_port_images.py`, the 114 matched TUs with the exe's flags. The X image is `/arch:IA32` and the S image is
`/arch:SSE2`, `/FIXED` at 0x30000000 and 0x34000000. Their callees come from 252 of the exe's own objects (the exe map),
and the C++ dynamic initializers (`__xc_a..__xc_z`) were run first, because the MGLOBAL references live there. The
original's read-only pages come from the booted MASHED.exe on disk, since the live code pages carry Frida's inline hooks.
x87 transcendentals execute on the host CPU (64-bit thunk), because QEMU's soft x87 is not bit-exact for them.

Controls (PREREG section 4), all hold:
- **Emulator = hardware.** Every scored call also read the original's real write set and ST0 at onLeave. Emulated O =
  live on **564/564** (boot 1) and **483/483** (boot 2) calls.
- **0x004325c0.** O53 and X53 RED 1/50, S and X24 GREEN 50/50. This is PORT-STATE-03-FPU's answer, on the same call
  shape (the f8 default step).
- **O24 = O** on every scored call: the live CW was 0x007f on all of them.

Refused: draw helpers that call into d3d9 (fetch outside the images) and port legs whose entry ABI differs from the
original's (register args). Each is listed per RVA in `X87AUDIT_EMU.tsv`, column `refusals`.

## 5. Decision question for the lead

**Run the exe's game thread at x87 PC_24 from device creation on, as the original does?**

- **Evidence.** Sections 1.1 and 1.2. The original is at 0x007f for every post-device call measured. The exe is at
  0x027f because its device is created on a worker thread. Where it matters today: 0x0046c5f0 (50/69 natural deltas),
  the four CameraEntries RVAs and FastSqrt (x87 TUs at 0x027f), plus 10 matched rows once they are called.
- **Options.**
  - **A.** A knob `MASHED_X87_PC24` (default OFF): `_controlfp_s(&t, _PC_24, _MCW_PC)` on the game thread right after
    `CreateDeviceWatched` returns. This reproduces what D3D9 does to the original's thread. SSE2 code ignores the x87
    CW. Expected: race hashes move, because the x87 physics/camera TUs change. A flip needs its own PREREG, arms and
    new gate references.
  - **B.** Per-TU moves (SSE2 for float-only rows, local `PcScope` for the rest). More edits, and each one needs its own
    measurement.
  - **C.** Leave it as is.
- **Recommendation: A**, as a separate step with a PREREG. It is one line at the cause, the original's own state,
  and it makes the existing local PC_24 scopes (AiTargetClosest, AiLeafTargets, CollisionWorld) redundant without
  breaking them.

**Separate step for finding (a):** a port-only body of 0x00443300 in the DDK's evaluation order. The match source
stays as it is, because it byte-matches under the DDK. Verify with this arm-3 harness (X24 and S must go GREEN 80/80).

## 6. Commits (`orch/PORT-X87-PC24-AUDIT`, from `orch/integration` 4fdfb7556)

| commit | content |
|---|---|
| `16ab9667b` | PREREG_X87AUDIT.md (alone) |
| this commit | the tools (`x87_inventory.py`, `x87_pcscan.py`, `x87_listing.py`, `x87_cw_probe.py`, `x87_cw_score.py`, `x87_port_images.py`, `x87_live_emu.py`, `x87_emu_score.py`, `x87_table.py`), the inventory and names, the scored tables (`X87AUDIT_*.tsv`, `x87_pcscan*.tsv`), this RESULT |

Gates: not run (no `mashedmod/` change). The worktree build was used only as the probe target.

## 7. Raw files (`D:/MashedWT/PORT-X87-PC24-AUDIT_data`, not committed)

| file | sha256 |
|---|---|
| mashed_re.exe (worktree build, `MASHED_NO_DEPLOY=1`) | 37ea0a5ed79e796ddd30add48cddb8ba28076702023828d04128336a2dbe044a |
| build.log | bb1d48fd764fee2db222c234e9eeca03ec5aa6b8d13b46b261e30cf399a3950f |
| cw_orig_1.json | 38c6ce8ef985a350674ce20723cc111af6446662e110b64ae858b09a9b44084d |
| cw_orig_2.json | 18e6ca6c6a41b0e74e189944c0d4e6e91b5c22ab5e25f8ae902196ce69843ca7 |
| cw_exe_race.json | efc751575a046fbf15b6c80ee1bcbf2c01e8b2bfaea3585dd185e6163881c4a2 |
| cw_exe_walk.json | f42d9bdb0d47a33ba7a9276048e27780649604f9587f181d9ac1eb18cd985024 |
| emu_full_1.json | 0b2e312d1ef747df96916d5f9601202fde5df485056dbb626a0321b9f0930152 |
| emu_full_2.json | 7ef736fa8148a3ee2d519a68527b7c48f4f276734dfeaab27bd0a5f5a59d2cf0 |
| emu_full_1.log | 7e8dc6838ea596d543f7919d57870b1540eac406797389b48b17762973b05988 |
| emu_full_2.log | 355fa32086aa8f94d43bf6f6e71841522db0efcfdb65eb8f75482ed2f9fafbf6 |
| emu_smoke.json (control-only smoke) | c7d69a93106061156f29a2fb5be4610064b19233a86c62c90b6a2c53062ae882 |
| img/X/port_X.exe | 6312a13b98ec67137119d7b27ccaa9fb5674ad42341255835f81a31ddbf06d20 |
| img/S/port_S.exe | 645c46d7b2e249cd9af83ea54df4e15837455300aede3f3c160877a442379c86 |

Earlier smoke runs (`emu_1.json`, `emu_smoke2/3.json`) predate the engine fixes (read-only pages from disk, initializers,
callee objects, native transcendentals) and are not scored.
