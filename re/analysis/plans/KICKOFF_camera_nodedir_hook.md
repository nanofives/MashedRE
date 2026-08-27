# KICKOFF — the 0x00441820 camera hook (next session)

Paste the block below as the opening prompt. Everything it references is committed on
branch `d1-camera-pose`.

---

Mashed RE. Task: author the `RH_ScopedInstall` hook for **0x00441820** (per-node camera
direction + height) so the row can reach C3. It is the ONLY missing criterion.

READ FIRST, in this order:
  - `re/analysis/race_camera/FUN_00441820.asm` — already dumped, 117 instructions,
    0x00441820..0x00441984. This is the thing you are transcribing.
  - `mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp` — the pattern to copy,
    verbatim-transcription style, forwarders to original RVAs, `RH_ScopedInstall`
    at the bottom. It is 88 lines and it is the whole template.
  - `mashedmod/src/mashed_re/Race/RaceCamera.cpp` `NodeDir()` (~line 124) — the
    standalone port of the same function. Use it as a SEMANTIC reference only.
    See TRAP 1 before you lean on it.
  - the Traps section below. Two of them are defects that actually shipped in this
    exact cluster and were found only on 2026-08-26.

WHY ONLY THIS ONE ROW. The camera cluster's other C2 rows do NOT ride along:
  - `0x00446520` — 7411 bytes, and `re/analysis/WS_H2_C4_LANE_FINDINGS_2026-06-16.md`
    already deferred it as "its own work session". Do not start it here.
  - `0x00410d10` — the 2026-08-26 offline diff never called `EliminationCheck`, so
    there is no evidence touching it. Promoting it would be evidence laundering.
  - `0x0040e180` — already C4, correctly, via `CameraClusterHooks.cpp`. Leave it.

C3 GATE STATUS for 0x00441820 (`re/CONFIDENCE.md:32-36`), verified 2026-08-26:
    prose + citations ................ MET
    reimplementation exists .......... MET  (Race/RaceCamera.cpp NodeDir)
    build clean ...................... MET
    caller at C2+ .................... MET  (0x00446520, C2)
    callee at C2+ .................... MET  (Vec3Magnitude C4, RwMatrixRotate C4,
                                             RwV3dTransformPoints C4, __ftol C3, ...)
    hooked + runtime-toggleable ...... **MISSING — this is the task**

WHAT THE FUNCTION DOES (read off the ASM 2026-08-26; confirm, do not trust):
  signature `(int node, float* out_dir, float* out_h)` — param_3 is `out_h`
    (`0x00441824 mov ebx,[esp+0x74]`; `0x00441842 mov [ebx],0`), param_1 is the node
    index (`0x00441854 mov edi,[esp+0x74]` after two more pushes).
  locals seed the default direction (0, -1, 0) at `[esp+0x18/0x1c/0x20]`
    (`0x0044182a..0x0044183a`, note `0xbf800000` = -1.0f).
  override entry = `0x004098a0()` + node*0xc  (`0x0044185e lea ecx,[edi+edi*2]`,
    `0x00441861 lea esi,[eax+ecx*4]`), i.e. three floats {elev, azim, height}.
  height at `+8`: if != -1.0f then `*out_h = height` (`0x00441864..0x00441870`).
  elev at `+0`: if == -1.0f take the FALLBACK branch at `0x4418e9`.
  elev path: rotate by `90.0 - elev` (`0x005ccad0`) about `0x006146f0` (X axis),
    then by `azim + 180.0` (`0x005cd09c`) about `0x006146fc` (Y axis). Each rotation
    is `call 0x4c4d20` (build matrix) followed by `call 0x4c3df0` (transform, count 1)
    — NOT a single rotate helper.
  fallback path (`0x4418e9` onward): node dir via `0x00426cc0`, tilted -25 deg about
    (corner0 - corner3) from `0x00426d00(n,0)` / `(n,3)`. NOT yet read instruction by
    instruction — do that.

FORWARD, DO NOT REIMPLEMENT, these callees (bit-exactness comes from calling the
originals, exactly as CameraClusterHooks.cpp does):
  0x004c4d20 RwMatrixRotate     0x004c3df0 RwV3dTransformPoints
  0x004c3ac0 Vec3Magnitude      0x004c3b30 FastSqrt
  0x00426cc0 node getter        0x00426d00 node corner getter
  0x004098a0 override table     0x004a2c48 __ftol
  0x00441760 Camera::Apply      0x004c1c10 Camera::SetProjection

TRAPS — the first two are defects that SHIPPED in this cluster and were found only
by measuring on 2026-08-26:

  1. TWO IMPLEMENTATIONS OF ONE RVA WILL DRIFT, AND A COMMENT WILL NOT SAVE YOU.
     `0x0040e180` has a hooked copy (CameraClusterHooks.cpp, C4, correct) and a
     standalone copy (`RaceCamera::MostSeparatedPair`) whose out-params were
     BACKWARDS for months. The divergence was written down at
     `CameraClusterHooks.cpp:16` and shipped anyway. The same thing happened with
     `__ftol`: `Math/FPURound.cpp` listed `RaceCamera.cpp BankersRound` as a
     known-wrong approximation, and it shipped. **Authoring this hook creates a
     SECOND copy of NodeDir.** Decide up front how the two stay in sync, and do not
     rely on a comment to do it. `hooks.csv` has one `file` column per RVA and
     cannot see the second copy at all.

  2. THE HOOK AND THE STANDALONE ARE DIFFERENT CODE PATHS. `RH_ScopedInstall`
     patches an inline JMP inside `MASHED.exe` (the .asi, dev-only). The offline
     driver (`re/tools/camera/cam_driver.cpp`) exercises `mashed_re.exe`'s
     `RaceCamera`. **The hook cannot change the offline diff's numbers, and the
     offline diff cannot verify the hook.** They are separate evidence for separate
     things. Do not report one as if it validated the other.

  3. VERIFY CONSTANTS AGAINST THE PE, NOT AGAINST A NOTE. Use
     `py -3.12 re/tools/pedisasm.py 0x<rva>` (new 2026-08-26; reads
     `MASHED.exe.unpatched` and CHECKS THE SHA-256 ANCHOR before decoding, because
     the working binary's code-cave detours would show a JMP where the original has
     real code). For a data constant, read the dword out of `.rdata` directly. This
     is how `0x005ccd18 = 0.00015` was confirmed genuine rather than a gloss error.

  4. `[esp+N]` ARITHMETIC SHIFTS WITH EVERY PUSH. This function does `sub esp,0x64`
     then three pushes then more pushes inside the call sequences, so the same
     parameter appears at `+0x74`, then `+0x88`, etc. Recompute the offset at each
     site; do not carry one forward.

  5. A DEFECT FOUND UNDER A BROKEN REGIME MAY NOT EXIST, and the reverse: the
     2026-08-26 session made three premature attributions (a speed gap blamed on
     `RecoverOffMesh` without quantifying it; a "tighter radius" hypothesis refuted
     by measurement; a `velB` lead term added and then reverted after reading the
     ASM properly, where BOTH velocity fetches load `[ebp-0x28]`). Read the
     instructions before naming a mechanism.

GATES:
  a. `mashedmod\build.bat` clean, both targets.
  b. Re-run the offline camera diff and confirm it is UNCHANGED — it should be, per
     trap 2, and if it moves you have accidentally touched the standalone:
       `log\cam_driver\cam_driver.exe log\camera_trace_v2.csv log\cam_nodes.txt ^`
       `  original\TOASTART\Common\LED.piz > log\cam_driver\port_out.csv`
       `py -3.12 re\tools\camera\cam_diff.py`
     Current baseline: eye median 0.0001, look-at median 0.0000, aim 0.0007 deg,
     pair 92.2% exact.
  c. C3 promotion goes through the `re-classify` skill, not by hand.
  d. C4 needs a `diff-original` canonical-scenario run with the inline JMP LIVE.
     An offline or hook-bypassed A/B is C3 evidence at best — this rule has been
     violated before and the anti-overclaim clause in CLAUDE.md is explicit.

STILL OPEN in this lane, do not lose:
  - 7.8% of frames (60 of 766) pick a genuinely DIFFERENT most-separated pair.
    Leading suspect, untested: the offline driver derives `active` as
    `(alive != -1)` instead of replicating `IsCarSlotActive` (0x0040e370), whose
    table walk the original applies at `0x0040e1b5` via `PTR 0x005f2770`.
  - The verbatim camera pose is still DISCARDED at `TrackRenderer.cpp:4059-4135`;
    `race_cam_.pos()` / `.target()` have zero call sites. Wiring it is a separate,
    larger question, and `verify/d1_carproj/RESULT.md` argues these fields are not
    the world camera — though its Candidate A test was mis-specified (it treated the
    aim DIRECTION field as a look-at point), so that conclusion is not established.
