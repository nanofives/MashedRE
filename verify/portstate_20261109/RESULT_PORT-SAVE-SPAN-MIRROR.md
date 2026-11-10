# RESULT PORT-SAVE-SPAN-MIRROR (2026-11-09)

PREREG: `PREREG_SAVE_SPAN_MIRROR.md` (committed alone, c6b6a556c). Branch `orch/PORT-SAVE-SPAN-MIRROR` from `orch/integration`
fa6fe7612.

## 1. Answer

**RED for the walk_04 goal, GREEN on the gates.** Results:

1. **Writer found.** On the original, 0x007f0a40 is written by 0x004924f0 at boot and then only by **0x00404e80
   Save::DeserializeFromBuffer** (`rep movsd` at 0x00404e91, 0x148 dwords from save buffer + 0x24a40), during the
   press-button load.
2. **The exe already mirrors that span.** `CopySpanAndRederive` (`Frontend/MenuNavSM.cpp:1535`) writes 0x007f0a40 at boot,
   and the shipped save passes its magic gate. The comment at `exe_main.cpp:189-190` saying otherwise is stale. That
   corrects PORT-TOP-04D's mechanism text and the PORT-TOP-04D-WALK04 RESULT, section 2 ("the exe does neither"). The
   knob `MASHED_SAVE_SPAN_MIRROR` adds the two steps of 0x00404e80 that the exe was missing (the 0x007f105c loop and the
   0x008a95ac / 0x008a94a8 stores). It writes what the original writes (`0 1 1 1 1 1 1 1 0 0 0 0`, 0x008a95ac = 0, block
   skipped because 0x008a94a8 = 0).
3. **The knob does not change walk_04**, on the hand path or with 0x004924f0 matched (`MASHED_ORIG_TOP=1
   MASHED_TOP04D_DATAZERO=1`). The prediction H2 (matched + knob ON = the original's red column 1) is **refuted**.
4. **The cause of the walk_04 difference is the exe's colour-select draw, not the save.** Memory at walk_04: matched
   minus hand differs only in 0x0067ea74..0x0067eaa8, the block `FUN_00431d00` writes (0x004924f0's last call). The
   matched values equal the original's. The exe draws player 1's keyboard icon from 0x0067ea98 (`exe_main.cpp:6213-6222`:
   x = 170 + (sel-1)*70, tint `kSwatch[sel-1]`, sel 0 = grey at x 85). Matched gives sel = 4 (column 4, yellow) and
   hand gives sel = 0 (grey, far left). The original draws red column 1 with the same 0x0067ea98 = 4, so its draw
   reads another field.

Gates: knob OFF G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3, unchanged. Knob ON: the same hashes.

## 2. Arm 1: watch-write (original, `savespan/watch_0a40.py`)

Hardware write watchpoints on 0x007f0a40 / 0x007f0a44 / 0x007f0a7c, armed before resume on every thread and re-armed every
50 ms. One boot, then the exe walk's three confirms. Run 1 stopped at the title + 4 s (3 hits). Run 2 (`--walk`) went
to the colour select (6 hits):

| hit | pc (after the store) | step | 0a40 / 0a44 / 0a7c after |
|---|---|---|---|
| 1-3 | 0x004925be / 0x004925d5 / 0x004925f6 (0x004924f0) | boot, before the title | 0 / 1 / 2 |
| 4-6 | 0x00404e91 (0x00404e80) | title -> main menu | 0 / 1 / 2 |

Main thread (tid 16344) for all 6. No write between the main menu and the colour select. 0x00404e80 in full
(MASHED.exe.unpatched): step 1 is `rep movsd` 0x148 dwords 0x00827d98 -> 0x007f0a40. Step 2 (0x00404e93..0x00404eb2)
is `*(0x007f105c + i*0x4c) = (u8)*(0x007f0f54 + i)` for i = 0..11. Step 3 (0x00404eb4..0x00404ed2) is
`0x008a95ac = *(0x00828254)`, then the 0x928f-dword copy from 0x0080335c to `*(0x008a94a8)` when that pointer is not zero.

## 3. The knob (commit fe5bd8ab3)

`Frontend/MenuNavSM.cpp` `SaveDeserializeTail`, called from `Nav_GameStateLoadSave` after step 1, exe only (the TU is in
`exe_sources.rsp` only). `MASHED_SAVE_SPAN_MIRROR=1` runs steps 2 and 3 at the real addresses from the loaded save
bytes and logs one line:

```
[save-span-mirror] 0x00404e80 steps 2-3: 0x007f105c+i*0x4c = 0 1 1 1 1 1 1 1 0 0 0 0; 0x008a95ac = 0x00000000; 0x008a94a8 = 0x00000000 (null, no block copy)
```

Build `563e6077` (`MASHED_NO_DEPLOY=1`): rva-lint `NEW=0`, Build OK. run_arms.py has stack `SSM` = the knob.

## 4. Gates (one run, test-slot + mashed_lock, private root `D:/MashedWT/PORT-SAVE-SPAN-MIRROR_root`)

| arm | hashes |
|---|---|
| H_DEF | `8284be46 / 67931834 / 31fe043e` |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 |
| H_SSM (knob ON) | `8284be46 / 67931834 / 31fe043e` |
| B_SSM_K0 / K1 / K26 (knob ON) | `1d5bf4f9` x3 |

## 5. walk_04 (nav demo, 04D settings baseline restored before each, `savespan/run_ssm.py`)

| run | env | walk_04 sha256/16 | P1 icon |
|---|---|---|---|
| hand, knob OFF | (none) | `2871487f1fcef51c` (= 04D hand) | grey, far left |
| hand, knob ON | `MASHED_SAVE_SPAN_MIRROR=1` | `2871487f1fcef51c` | grey, far left |
| matched, knob OFF | `MASHED_ORIG_TOP=1 MASHED_TOP04D_DATAZERO=1` | `da29bdef4b0bc8f1` (= 04D matched) | yellow, column 4 |
| matched, knob ON | the same + the knob | `da29bdef4b0bc8f1` | yellow, column 4 |
| original (WALK04) | | `d40b38fdd6a30c50` | red, column 1 |

All four exe runs logged `R2-2 gamesave load: LOADED (DEADBEEF save)`. In the matched runs, `0x004924f0 ... (matched)`
is logged before the load, the same order as the original.

## 6. Diagnostic: memory at walk_04 (`savespan/exe_snap.py`, `savespan/run_snaps.sh`)

0x0067ea70..0x0067eaff and 0x00803344, read when walk_04 is written (exe, Frida attach), and at the colour select on the
original (`verify/porttop_20261108/walk04/orig_walk04.py --snap`, the 2528ad5d4 blob). Non-zero or differing dwords at
the colour select:

| addr | original | exe hand | exe matched | writer (static) |
|---|---|---|---|---|
| 0x0067ea70 | 1 | 0 | 0 | not traced (set at the main menu on the original) |
| 0x0067ea74 | 1 | 0 | 1 | FUN_00431d00 |
| 0x0067ea84 | 1 | 1 | 1 | |
| 0x0067ea90 | 1 | 0 | 1 | FUN_00431d00 |
| 0x0067ea98 / 9c / a0 | 4 / 2 / 3 | 0 / 0 / 0 | 4 / 2 / 3 | FUN_00431d00 (1, 2, 3) then FUN_00431b80 x3 |
| 0x0067eaa8 | 0x3e99999a (0.3f) | 0 | 0x3e99999a | FUN_00431b80 |
| 0x0067eab4 / c0 / c4 / c8 | 0x1bc / 0xb / 1 / 0x2d | 0 | 0 | not traced (set at the main menu on the original) |
| 0x0067eaf0..0x0067eafc | 0 | 0 | 0 | |
| 0x00803344 | 0x3f400000 (0.75f) | 0 | 0 | 0x004924f0 (`DAT_00803344 = 0.75f`) |

Read-outs:

- Matched equals the original on 0x00431d00's whole block. Hand-versus-matched differs only there, so that block is
  what moves walk_04 in the exe.
- 0x00803344 is 0 in both exe paths. The exe's hand boot chain zero fill (`DataZeroFill`, 0x007f0f60..+0x373a4 =
  ..0x00828304) runs after the matched body and clears 0x00803344, along with 0x004924f0's other writes in that range.
  The original has no second zero fill.
- The exe's icon x and tint come from 0x0067ea98 (`exe_main.cpp:6213-6222`). Its comment (`exe_main.cpp:2250-2255`) says
  the original's tint comes from FUN_004335f0 reading the choice field 0x0067eaf0 + 12*profile. On the original that
  field reads 0 here while the icon is red column 1, and the port's table maps choice 0 to grey. [UNCERTAIN] Which
  field and profile index FUN_004335f0 reads on the original at this screen. Missing evidence: an onEnter trace of
  0x004335f0 / 0x0042bcb0 at the colour select (args and the address of the choice read). That trace decides the
  exe-side fix.

## 7. Decision for the lead, and next

- **Merge the knob?** It is the original's 0x00404e80 steps 2-3, default OFF, value-neutral on H, B and walk_04.
  Options: (a) merge and flip it ON as a faithful no-op on these recipes, (b) merge OFF, (c) drop it. Recommendation
  **(a)**. It removes one known difference from the original at no measured cost.
- **walk_04 next step** (not done here): the FUN_004335f0 / FUN_0042bcb0 trace at the colour select on the original. Then
  make the exe's icon read what the original reads, instead of the 0x0067ea98 cursor. Only after that does running
  0x004924f0 matched by default stop moving walk_04. The 55 px bar offset (PORT-COLOURSEL-BAR-Y) is not touched.
- Stale comment `exe_main.cpp:189-190` ("does not mirror the span back to the image") should be corrected in a step that
  changes exe_main.cpp anyway.

## 8. Commits

| commit | content |
|---|---|
| c6b6a556c | PREREG (alone) |
| fe5bd8ab3 | the knob (MenuNavSM.cpp), run_arms `SSM`, `savespan/watch_0a40.py` |
| this commit | `savespan/run_ssm.py`, `savespan/exe_snap.py`, `savespan/run_snaps.sh`, `verify/porttop_20261108/walk04/orig_walk04.py` (the 2528ad5d4 blob), this RESULT |

## 9. Raw files (`D:/MashedWT/PORT-SAVE-SPAN-MIRROR_data`, not committed)

| file | sha256 |
|---|---|
| watch1.json / watch1.log | 7ccdbf87896f5dfc1cc56bcf03971ccd8a5b45f85ece2f5dc05f3bdc2ee57292 / d7962aee35ad1130e5213839d6211765a56bc2b1fdf9772a6dde8a86ce26031e |
| watch2.json / watch2.log | 63d28f5a408e1b0498f0bedb1600c300952c74ee0b0d35f8c80680a59455a00f / fdaa96fa5d408ab578a4972e4f6f5a6b0aac41938d73968781a6aad14f96f59a |
| exe/mashed_re.exe | 563e6077b2e13b639c88836d76737c787bf6af8dea834035d60a17f08b7d2c3c |
| build.log | 85fe0b13050cfa9a51ed196fbbdb4e00b4910b622acf3379bf3ffcb7fb4f6fb1 |
| arms.log | 777bc7464e1f309a41bc74b2c05e09b14b514e0e284be9393b2796c90d2c390d |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 8284be46b3c6... / 6793183492e0... / 31fe043e261d... (full: 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198, 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c, 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3) |
| arms/B_DEF_K0.step.csv, arms/B_SSM_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/H_SSM.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| walk_hand_off.log / walk_hand_on.log | 8001dc16119c1b45d60bacba1713ca6fce9b3879c718300d375daeff22f6274c / 73f01fd98224578a27c51d1d184e437db0cd1eb62b0e780c757c265d7e9aec15 |
| walk_matched_off.log / walk_matched_on.log | 10f737c8a6354f5cc6f3c523cc32f8b43532174fa6c179c12a475f6b01b426d0 / e5eb8ef0490c0a9578a994816296687590caf45b7f25f2b509e277df10d0fd5a |
| walk_hand_{off,on}/.../walk_04_colourselect.bmp | 2871487f1fcef51cfd6787187ecb3d418a8a8d78859113e37383a3b19b495726 |
| walk_matched_{off,on}/.../walk_04_colourselect.bmp | da29bdef4b0bc8f12ef75ae9396ccba6a7fc0a41d182c751f46c0c582779b5ec |
| snap_hand.json / snap_matched.json | 4277a44c12d76e500e22d4031d66e5e45d49f58a606d21e1199fe107198f1f83 / 503e7babd852956210de9c139e9bca937114c3c41ce58515b8d0934a09b9f470 |
| orig_snap/walk.json | cf5c193dd2f9b73e58b97a6256ca0b0eb2758b9417bf03b851f3acd61f521dcb |
| orig_snap/walk_04_colourselect.bmp | 0b671018eefee0312d03faee30106ca8d8fb82d2a4fb1324a4309f392f6d6a49 (a second original capture: same layout as d40b38fd, the background video frame differs) |
| snaps_orig.log | a1cfc50a899c5978417d69a6c779c021220fe5e95a1d87fea1a942e16a082dd4 |
