# RESULT D4-WSB-NARROW-FIX2

Verdict: **GREEN, 8 of 8 rows**. With `MASHED_NARROW_DOT_ORDER=1 MASHED_NARROW_PAIR_INT=1 MASHED_LEAVES1_X87=1`
all 8 NARROW rows write the same bytes as the original on every call (diff 0, noo 0, 0 `DIFF` lines). The two rows
left RED by run 13, `00575c60` and `00575fe0`, are now GREEN. Both OFF controls still show the defect, and the
default gates are unchanged on this exe.

Branch `orch/D4-WSB-NARROW-FIX2`, cut from `orch/D4-WSB-NARROW-FIX` @ `e005a6283`:
- `82b782287` merge of `orch/integration` @ `b4aab7780` (the base was 18 commits behind, merge-base `1158f5c1c`)
- `356130184` PREREG amendment 1, committed alone before any arm
  (`verify/wsbnarrow_20261027/PREREG_D4-WSB-NARROW-FIX.md`, section "Amendment 1")
- `4a7f24589` the Core17 edit and `run_c4n_fix2.py` (a copy of `run_c4n_fix.py` with its own root, data folder and lock label)
- this RESULT

## The change (default OFF, same knob, not seeded in `Core/FaithDefaults.cpp`)

`mashedmod/src/mashed_re/Collision/RwpSolverCore17.cpp`, `0x00575fe0` face append, under `NarrowDotOrder()`.
Core17 is an x87 TU, so the change is association only. Source of the order: `re/tools/disasm_fn.py 0x00576320 0x00576400`
on `MASHED.exe.unpatched`:

| field | original (FADD order) | FSTP |
|---|---|---|
| flag-2 cross `f[0xc]` | `((v6-v10)*v1 + (v2-v6)*v9) + (v10-v2)*v5` | `0x00576391` |
| flag-2 cross `f[0xd]` | `((v4-v8)*v2 + (v8-v0)*v6) + (v0-v4)*v10` | `0x005763b0` |
| flag-2 cross `f[0xe]` | `((v1-v5)*v8 + (v5-v9)*v0) + (v9-v1)*v4` | `0x005763d1` |
| face-plane dot `f[7]` (`+0x1c`) | `(f[0xd]*v1 + f[0xc]*v0) + f[0xe]*v2` | `0x005763f7` |

The port had all four in the reverse grouping (Ghidra print order). OFF = the previous code, unchanged.

## Build and root

`MASHED_NO_DEPLOY=1 mashedmod\build.bat`, Build OK. `original\mashed_re_dev.asi` was not redeployed.
Private root `D:\MashedWT\D4-WSB-NARROW-FIX2_root` (robocopy of `original\`, plus this build's `.asi`).

| file | SHA-256 |
|---|---|
| root `.asi` (= `mashedmod\build\mashed_re_dev.asi`) | `fd58c9850699f036a35f9af4a4f34b210f09b1e14930fca95fb183e288113dac` |
| exe `mashedmod\build\mashed_re.exe` | `0e08cf4b653c7004c303043d024c7fa27b38bdc867e8d46e0562dcbbfac268b4` |
| root `MASHED.exe.unpatched` | `bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e` (anchor) |

## Probe arms (2026-10-27)

All boots ran inside the machine test slot and `mashed_lock.py run --label D4-WSB-NARROW-FIX2`, one boot per row.
Every boot reached the race on attempt 1 (`RACE_OK`), with no crash and no piz collision. Every row:
`A/B-IS-REAL`, `installed=1` (its own `L2_C4N_<rva>` only), `ret_diff 0`, `skip 0`.

```
proyectos_test_slot.py bash -c "py -3.12 verify/wsbnarrow_20261027/run_c4n_fix2.py --arm OFF --legs P --rvas 00575c60,00575fe0 --run _f2 && py -3.12 verify/wsbnarrow_20261027/run_c4n_fix2.py --arm FIX --legs P --run _f2 --env MASHED_NARROW_DOT_ORDER=1 --env MASHED_NARROW_PAIR_INT=1 --env MASHED_LEAVES1_X87=1 && ..."
```

| arm | row | calls = samples | ok | diff | noo | DIFF lines | verdict |
|---|---|---|---|---|---|---|---|
| OFF | `00575c60` | 9089 | 6458 | **2631** | 0 | 20, first `region=1+c o=00020147 n=00000000` | control holds |
| OFF | `00575fe0` | 1473 | 486 | **987** | 0 | 20, first `region=1+c o=0000002f n=00000000` | control holds |
| FIX | `005757d0` | 2241 | 2241 | 0 | 0 | 0 | **GREEN** |
| FIX | `00575880` | 8705 | 8705 | 0 | 0 | 0 | **GREEN** |
| FIX | `00575c60` | 9025 | 9025 | 0 | 0 | 0 | **GREEN** (run 13: 82/6273) |
| FIX | `00575fe0` | 4289 | 4289 | 0 | 0 | 0 | **GREEN** (run 13: 81/1985) |
| FIX | `00577ec0` | 36737 | 36737 | 0 | 0 | 0 | **GREEN** |
| FIX | `005784a0` | 6913 | 6913 | 0 | 0 | 0 | **GREEN** |
| FIX | `00578610` | 9665 | 9665 | 0 | 0 | 0 | **GREEN** |
| FIX | `00578e50` | 7745 | 7745 | 0 | 0 | 0 | **GREEN** |

The OFF controls first differ at the pair word `+0x0c` (the `MASHED_NARROW_PAIR_INT` defect), so the probe still sees
the OFF body's defect on this build.

`[UNCERTAIN]` whether the flag-2 cross block (`*(byte*)(W+0x50) & 2`) ran in this scenario. The probe reports
equality of the full write set, so a GREEN row means no wrong byte with or without that branch. It does not show
that the branch executed. No branch counter was added.

## Gates (default exe, no knob): GREEN

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe `0e08cf4b`, `A4_ROOT` = the private root,
`A4_OUT=D:/MashedWT/D4-WSB-NARROW-FIX2_data/arms`, inside the test slot and `mashed_lock.py run --label D4-WSB-NARROW-FIX2`.
The first gate attempt (end of `arms_f2.txt`) stopped with a FileNotFoundError because the `arms` folder did not exist.
I created the folder and re-ran the gates (`gates_f2.txt`).

| arm | file | sha8 | ref (`re/orchestrator/merge_train.json`, read 2026-10-27) |
|---|---|---|---|
| H_DEF | step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | step | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## Raw files (not committed), `D:\MashedWT\D4-WSB-NARROW-FIX2_data\`

| file | SHA-256 |
|---|---|
| `build1.log` | `23fe9a80718bf69b70c3b4191822cbbf9dea58b64ce599c52524ea9c5f6dd628` |
| `arms_f2.txt` (probe driver output + the failed first gate attempt) | `74835d95cb926673aee16a6fcd7dd58d60cddd9f78599c38a16dba39e3ec1f59` |
| `gates_f2.txt` | `75a99c625f9aa599eda31eaae3bfab8db2b6fd69ca7cb3237158e81f169df6dd` |
| `c4n_OFF_P_00575c60_a1_f2.log` | `2cb963931ad8f757395586e367599696b2a7b7843cb1c7db02154777aa657101` |
| `c4n_OFF_P_00575fe0_a1_f2.log` | `82a65fb44bb43177846bee3a1fc686c11724db403f52de38af2a4744735a0a84` |
| `c4n_FIX_P_005757d0_a1_f2.log` | `a0295fb86bb16357c23998cc8760f847db1d4e565ad3ccf0f4f1c8236a9f1664` |
| `c4n_FIX_P_00575880_a1_f2.log` | `5a94c161cac448519524b43ffa76c178c302bed207008b7955eb08f5eaa3ad9e` |
| `c4n_FIX_P_00575c60_a1_f2.log` | `293ef8c418450181da067cbbae947ed01c85cf69840e1f72094438c263ecc49a` |
| `c4n_FIX_P_00575fe0_a1_f2.log` | `72b8c1177da2173dc1319d02ef17fd74909c8002156a114965c795ea4608d1e6` |
| `c4n_FIX_P_00577ec0_a1_f2.log` | `f0a219b502985b889a78f130eb6e55a6b7266f4be4014df4cd26cbe112e48a52` |
| `c4n_FIX_P_005784a0_a1_f2.log` | `29790c68b2392d2588b26f4a472f8a15e6c3299c86ba8b785caa2edb953ab053` |
| `c4n_FIX_P_00578610_a1_f2.log` | `a99464c41ac873f5d24bcb30a615e2c61f2bae67539882236e8e51e3586c5803` |
| `c4n_FIX_P_00578e50_a1_f2.log` | `8e7a370143fccab1413913d8fdf43df56bc9ed3fcf4b45509deaee2f5cff2ef7` |
| `launch_OFF_P_00575c60_a1_f2.txt` | `e2ca3ffe85b19584803b22dead2ab3d04184952ea1df0ae1988a9a95466a0979` |
| `launch_OFF_P_00575fe0_a1_f2.txt` | `af684c0f41797f574a8a1d3e9b4ec725715d096f5a4b5181369149fcd794f2c1` |
| `launch_FIX_P_005757d0_a1_f2.txt` | `5fea4f9b0e846f37e865e1a6c2b34c171a59b05b06550bf9e8e86d67c9fc253d` |
| `launch_FIX_P_00575880_a1_f2.txt` | `55fd764fc1fd13e0b8f73372492ab08a853f3d7410aea53e4b762240ce4b6f02` |
| `launch_FIX_P_00575c60_a1_f2.txt` | `3c320c3e5940bc40a7304692be95fe8965b2cd8c49130fdb3dd062f3091fb997` |
| `launch_FIX_P_00575fe0_a1_f2.txt` | `29913d49ea46ccfa0933b829466a330cc851c59e5255fdc03904e8861e226706` |
| `launch_FIX_P_00577ec0_a1_f2.txt` | `937ef48212850db0138d62be8cb6846b85c4203debd07611c30710ad1e9bb9db` |
| `launch_FIX_P_005784a0_a1_f2.txt` | `e2cfe149bcc14ad163d0be45d2420e704c659ee01a2bda12bb4a5dacafbe9e8b` |
| `launch_FIX_P_00578610_a1_f2.txt` | `0be46bb1c4f7cd8229f3a41dc24fe1cc0e856db37ba3cd0123baa118be14d278` |
| `launch_FIX_P_00578e50_a1_f2.txt` | `c6028ed8ab605dffae1219c9433695005d1f3ae350d058d0cc9f19f8049b1463` |

Gate arm outputs: `D:\MashedWT\D4-WSB-NARROW-FIX2_data\arms\` (sha8 values in the gate table).

## What this RESULT does not do

No tracker change and no default flip. The 8 GREEN rows cover the knob-ON body only. The default build runs the OFF
body, so no row moves to C4 on this evidence. Next is a decision row: flip `MASHED_NARROW_DOT_ORDER`,
`MASHED_NARROW_PAIR_INT` and `MASHED_LEAVES1_X87` to default ON together (all 8 rows at once), then re-score the gates.
The flip changes what the default build executes, so the gates must be re-run on it. A moved hash then needs a re-baseline decision.
