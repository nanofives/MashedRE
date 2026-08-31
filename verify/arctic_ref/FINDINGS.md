# ORIGINAL Arctic in-race reference — captured (2026-08-31, branch race/arctic-cap)

**Verdict: DONE.** A pose-matched ORIGINAL Arctic in-race frame is captured, with the
same 4-file set `race_draw_burst.py` produces for TRAINING. Closes T-ARCTIC / U-9059.
The reference exe was never modified; `original/gamesave.bin` is pristine at the end.

## Deliverables (this directory)

| file | content |
|---|---|
| `orig_arctic.bmp` | 640×480 backbuffer dump, ARCTIC.PIZ in-race (visually confirmed) |
| `orig_cambasis.txt` | 12-float same-frame camera basis (pos, right, up, at) — **use this** |
| `orig_lens.json` | RwCamera lens: viewWindow 0.60/0.45, fovy 48.46°, near 0.10, far 70 |
| `orig_frame.json` | full RwCamera frame (modelling + LTM) + euler + ctrl fields |
| `orig_track.txt` | `ARCTIC`, single piz open `TOASTART\TRACKS\ARCTIC.PIZ` |
| `orig_arctic.bmp.draw3d.json` | draw_calls 115, prims 43613, verts 28406 |
| `orig_campose*.txt`, `orig_carproj.txt` | controller-struct eye/at (DISCREDITED for transplant, kept for the record) + car-projection cross-check |

12-float basis: `-25.52725,4.55718,39.87078, -0.87753,0.47719,-0.04718, 0.47952,0.87327,-0.08634, -0.00000,-0.09839,-0.99515`

The frame carries roll (euler elev 32.43, azim 360.46, roll 0 at this instant; the
`right`/`up` vectors are tilted). Use the 12-float basis, NOT the 6-float
`orig_campose.txt` (mis-sourced from the controller struct and roll-lossy —
`campose_ctrl_DISCREDITED`).

## The blocker and how it was cleared

Arctic is unlock-gated in `original/gamesave.bin`. Cleared without touching the
reference: unlock a cup row on a SAVE COPY, drive the mode-3 Challenge-Cup flow.

- Unlock (copy only): `re/tools/gamesave_edit.py <copy> -o <edited> --rows 0,1,2,3 --set c1=1,c11=1`.
  Championship span at file `0x24A40`, 13 rows × 0x30, col1 = mode-3 launch gate,
  col11 = mode-10 gate. The editor refuses to write `original/gamesave.bin`.
- Every ORIGINAL launch wrapped in `re/tools/run_with_unlocked_save.py <edited> -- <cmd>`:
  one-command swap window, restores + sha-verifies the reference in a `finally`.
  Final `original/gamesave.bin` sha = `bd18788182b2343e5203eb983fddd8bd947a5f3385e9fcaa46a152a9ec002a70` (pristine).

## Challenge-select index global (the open problem from race/nav-champ)

The 2026-08-30 probe could not isolate the challenge index because only row 0 was
unlocked (nothing to move to). With rows 0-3 unlocked:

- The real index is **`DAT_0067f17c`**. Down (control code 12) steps it 0→1→2→3, up
  steps it back, it caps at 3 (four entries), and the on-screen highlight + track
  preview follow it (`../nav_shots/chall_step{0..3}.bmp`).
- The per-depth cursor `0x0067ee80` (what `nav_agent.js` `setsel()` writes) does **NOT**
  drive it — confirming the earlier negative. Launching with `setsel(1)` still loaded
  TRAINING (row 0's track); only navigating `DAT_0067f17c` changes the launched track.
- Launch reads this index as `track` in
  `(&DAT_007f0a40)[FrontendModeIndex(mode)+track*0xc]` (mode 3 → col 1).

## Bronze Cup 1 row/entry → area(.piz) map (behaviourally confirmed)

Each entry launched under the unlocked save; loaded `TRACKS\*.piz` read from `CreateFileA/W`.

| challenge index `DAT_0067f17c` | preview | loaded .piz |
|---|---|---|
| 0 | dusty canyon (Battle, 1 opp) | TRAINING.PIZ |
| 1 | desert arena (Battle, 2 opp) | EGYPT.PIZ |
| 2 | snow (Battle, 3 opp) | NEUSTEIN.PIZ |
| 3 | night storm / harbour (Race, 3 lap) | **ARCTIC.PIZ** |

So Arctic needs only a `col1=1` edit on row 3 + challenge index → 3. No cup
progression, no reference-exe mod. The snow preview (index 2) is Neustein, not Arctic;
Arctic is the stormy harbour (index 3).

## Repro

```
cp original/gamesave.bin <scratch>/save_work.bin
py -3.12 re/tools/gamesave_edit.py <scratch>/save_work.bin -o <scratch>/save_unlock_0123.bin --rows 0,1,2,3 --set c1=1,c11=1
py -3.12 re/tools/run_with_unlocked_save.py <scratch>/save_unlock_0123.bin -- \
  py -3.12 re/frida/race_draw_burst.py --challenge 3 --settle 4.0 \
    --out <worktree>/verify/arctic_ref/orig_arctic.bmp
```

`race_draw_burst.py --challenge N` (added this branch) forces the mode-3 flow and
reaches challenge entry N by N down-presses. `nav_champ_probe.py --plan challlaunch
--track-sel N` (added this branch) is the discovery harness that built the map above.

## [UNCERTAIN]

Sub-frame roll drift: the basis is read one frame before the shim dumps the BMP, and
the race camera rolls on a 1024-tick sine (memory `race-camera-rolls-30deg-sine`), so
basis and BMP can differ by ~1 frame of roll. Same same-frame method as the TRAINING
capture; drift is small but not zero.
