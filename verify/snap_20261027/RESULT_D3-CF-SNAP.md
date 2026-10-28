# RESULT D3-CF-SNAP: the port-only restore fires only on stuck relocation (G-CTRL 0, B K1 2). K-POST removes it and moves B K1 only (2026-10-27)

Branch `orch/D3-CF-SNAP` from `orch/integration` `96c032d0b`. PREREG `PREREG_D3-CF-SNAP.md` (`0fceee57f`), committed alone
before the code (`f805d071c`) and the runner (`8ec8b8c01`). Exe `b95b67fe` (`MASHED_NO_DEPLOY=1`, copy
`D:/MashedWT/D3-CF-SNAP_data/exe_b95b67fe.exe`). Private root `D:/MashedWT/D3-CF-SNAP_root`. All 12 arms ran in one boot sequence under
`mashed_lock.py run --label D3-CF-SNAP` (queued behind merge-train13 `20261027_214012`, then held). No default changed.

## 1. What section 5.1 requires

Stated in the PREREG section 0. `verify/substep_20261021/RESULT_substep.md:88-93` records the gap and its `[UNCERTAIN]` (the
restore count). `:110-111` names the one alternative, "move the skip / relocation decisions ahead of the physics step". This lane
measured the count (R1, `MASHED_SNAP_LOG`) and built the alternative (R2, `MASHED_SNAP_POST`).

## 2. Code (`D3d9Render/TrackRenderer.cpp`, both default OFF, not seeded in `Core/FaithDefaults.cpp`)

- `MASHED_SNAP_LOG=1`: `[snap] <site> frame <n> slot <v>` in `mashed_re.log` for every `OuterRestore` (sites `dead`, `spin`,
  `reloc`, `mismatch`) and for every stuck relocation (`reloc-any`, knob off, or `reloc-post`, knob on). Cap 2000 lines.
- `MASHED_SNAP_POST=1`: a car the outer driver stepped skips the loop-head dead / spin restore for this frame, and the stuck
  relocation (now a lambda, `stuckReloc`) runs after the commit, with the loop head's skips re-applied. Knob off, the code
  path is the previous one (`stuckReloc()` called at its old site).

## 3. Gate table

| arm | step | gates | ptrace | `[snap]` lines |
|---|---|---|---|---|
| ref (`merge_train.json`) G-CTRL | `5943c5f1` | `364350ae` | `52052890` | |
| A0 `H_DEF` | `5943c5f1` | `364350ae` | `52052890` | (no log) |
| A1 `H_SL` | `5943c5f1` | `364350ae` | `52052890` | 0 |
| A2 `H_SPSL` | `5943c5f1` | `364350ae` | `52052890` | 0 |

| arm | K0 | K1 | K26 | `[snap]` lines K0 / K1 / K26 |
|---|---|---|---|---|
| ref B | `3a03670c` | `e85ea6fb` | `53778664` | |
| A0 `B_DEF` | `3a03670c` | `e85ea6fb` | `53778664` | (no log) |
| A1 `B_SL` | `3a03670c` | `e85ea6fb` | `53778664` | 0 / `reloc` 2 + `reloc-any` 2 / 0 |
| A2 `B_SPSL` | `3a03670c` | **`9fcdbd3a`** | `53778664` | 0 / `reloc-post` 2 / 0 |

B_SL_K1: `reloc` at frame 331 slot 1 and frame 395 slot 2. B_SPSL_K1: `reloc-post` at the same two (frame, slot) pairs, so the
trigger frames are unchanged, as the PREREG predicted. Step dumps first differ at frame 332 (827 differing lines of 1825),
car 1's row: the car stepping from the relocated position one frame later.

(b) per-regime envelopes (`verify/savelink_20261023/vsum.py`, `vsum_b.txt`): A0 K0 A 3/3, K1 B 3/3, K26 C 3/3, A2 the same,
**9/9 both**. `[substep] outer` mismatch lines: 0 across all 12 arms.

D2: not run. Structurally unreachable: D2 arms set `MASHED_MEASURE_SOLO=1` (`TrackRenderer.cpp:4784`), `ai_cars_` is empty, the
opponent loop has no iteration and the outer driver needs `outerN > 1`. 13 tracks: not run (knob default OFF, the default
gates above are unchanged, and `D3d9Render/` is outside `full_set_paths`).

## 4. Predictions

- P0 holds: A0 = refs on all six hashes.
- P1 holds: A1 = A0 on all six.
- P2 measured: the restore fires only at the `reloc` site. G-CTRL 0, B K0 0, K1 2, K26 0. 0 `dead`, 0 `spin`, 0 `mismatch`. This
  settles section 5.1's `[UNCERTAIN]` for these arms and agrees with its "only the stuck relocation reaches the restore".
- P3 holds: A2 = A0 wherever A1 logged no relocation (G-CTRL, K0, K26). K1 differs (`9fcdbd3a`).
- P4 holds: A2 logs 0 `dead` / `spin` / `reloc` restore lines.

Verdict per PREREG section 4: **DECISION** (P0, P1, P4 hold, A2 differs on B K1).

## 5. Decision for Mariano

Flip `MASHED_SNAP_POST` to default ON? Effect: the stuck relocation stops discarding a frame of physics. The car keeps the frame's
step (with any car-car impulse it received) and starts from the relocated position on the next frame. The original has no stuck
relocation at all (FUN_00470c70's respawn calls `0x004713b2` / `0x004713d1` are not ported), so neither timing is the
original's. What the flip removes is the restore, the one port-only mechanism section 5.1 names.

Expected refs after the flip: G-CTRL `5943c5f1 / 364350ae / 52052890` (unchanged), B `3a03670c / 9fcdbd3a / 53778664` (K1 moves),
(b) 9/9. Recommendation: flip. It removes a port-only undo for no gate cost (G-CTRL unchanged, (b) 9/9). After the flip, PREREG step 3
(retire `OuterSnapshot` / `OuterRestore` once the mismatch net has stayed at 0) becomes possible.

## 6. Raw data (`D:/MashedWT/D3-CF-SNAP_data`, not committed)

| file | SHA-256 |
|---|---|
| `exe_b95b67fe.exe` | `b95b67fecfecdc1f536eeea6f1a4c1f7812281fd09b7bf14627971f9e179dc21` |
| `RUNLOG_arms.txt` | `bc3d143aebd44d0ba44773a46dd95768e21fdd07432d4ab093f9afe1cbe7e250` |
| `vsum_b.txt` | `419575f22bf83308c8af918c274c18d2295ee454940c04ce27259d16110a491a` |
| `arms/RUNLOG.txt` | `c39a964ecb72be488bf10ce8d1aea7d0186972f5cf47035956d6aebf9187b17f` |
| `arms/H_SL.log` | `a179c8e4ad84f64fd7befa131fbc0ce86a716569393a6d3b20b740ca2c084e84` |
| `arms/H_SPSL.log` | `a179c8e4ad84f64fd7befa131fbc0ce86a716569393a6d3b20b740ca2c084e84` |
| `arms/B_SL_K1.log` | `2644dc4fd8823e4963360f2ccf561bb1a96d33e471a9df26df71d378b336804c` |
| `arms/B_SPSL_K1.log` | `a70b2b7c4f10a684407e5b5de6fdce3c1696e0304d1c6efa47e2c3b149ea9c04` |
| `arms/B_SL_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `arms/B_SPSL_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `arms/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |

Re-run: `A4_EXE=D:/MashedWT/D3-CF-SNAP_data/exe_b95b67fe.exe py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D3-CF-SNAP --
py -3.12 verify/snap_20261027/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_SL B_SL_K0 B_SL_K1 B_SL_K26 H_SPSL B_SPSL_K0 B_SPSL_K1 B_SPSL_K26`.
