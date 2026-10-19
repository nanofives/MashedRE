# RESULT u9769: the player's state at calls 22..39 is the first car<->car contact (2026-10-19)

Branch `work/u9769` from `race/first-frame-parity` `e9d9665cb`. PREREG `PREREG_u9769.md` (section 1 + amendments 1..3, each
committed before its arms). Exe `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\d3bc_root`. Model claude-opus-5-5, account2.

## 0. Verdict

1. **The player's record is exact through window call 21 and splits at 22, the first car<->car contact** (player = self /
   ECX, car 2 = other / EAX, pass 0, VehicleCarCarContact `0x00469df0` from `0x00470bcd`). New original capture o_c0
   (`--statediff-car 0`). This is the car-2 gap of U-9769; car 3's row-39 target is the player too, but car 3 has its own split.
2. **Three causes, all measured, all default OFF:**
   - **Stub: `Rw_MatrixDerive` (FUN_004c4dc0) was empty** (`Collision/ContactStubs.cpp:161`), so the spin tail transformed the
     contact arm by an uninitialised 64-byte stack local and took the spin sign from it. FUN_004c4dc0 is `RwMatrixInvertEntry`
     (C3, `Render/RwMatrixInvert.cpp`). `MASHED_CC_MATRIX_INVERT=1` calls it. Replay (`MASHED_CC_REPLAY`, the port body on the
     original's entry records): the contact row's car-2 spin `+0x148` goes from 0.176054 (port) / -0.023946 (original) to equal.
   - **Order: car 2 entered the contact before its own A4 / A6a of the frame.** FUN_00470c70 runs FUN_00470670 for all 16
     records first (call `0x00471076`), then FUN_004709a0 (substeps, pair loop). The port's car 2 arrived with velocity
     `-300.85376` = the original's previous copy `+0xae0`; the original's was `-316.573639`. The existing `MASHED_A4_ORDER`
     (U-9746) fixes it: car 2's entry record then equals the original's (0 differing dwords).
   - **Arithmetic: `MASHED_CC_PC24=1`** runs a noinline copy of the body with the RW LUT bodies for FUN_004c39b0 / FUN_004c3ac0
     (the port used `sqrtf`, `ContactDeps.h:35..47`), float impulse terms (PC_24) and RwMatrixInvertEntry. Replay **64/64 rows
     exact**; in-run on the full stack + `MASHED_A4_ORDER` the call-22 contact is **bit-exact at entry and exit on both cars**.
3. **Per car on `EXL7AFSM7PCA` (d3bc full stack + `MASHED_CC_PC24` + `MASHED_A4_ORDER`):** player record equal to o_c0 through
   call 61 apart from three one-call one-ulp blips (`+0x218` at 39 / 48, `+0x464` at 49). Car 2's commands equal the original's
   through **row 138** in C (was 22); its record has only one-call one-ulp blips in wheel fields (`+0x214` at 11, `+0x3a0` at 54
   / 100). Car 1 unchanged (row 149, FUN_00414c30). Car 3 splits at **record call 7**, `+0x9b8` `-1964.98584` vs `-1964.98596`,
   its own physics; commands through row 57.
4. **(b): no default candidate.** `MI` alone A 1/3 (car 2 `c0# 37 [13,35]`, car 1 `|steer|~ 6.5 [7,23]`), B 3/3, C 0/3. `PC`
   alone A 1/3, B 2/3 (car 3 `c0# 38 [13,37]`), C 0/3. `EXL7AFSM7PCA` A 0/3, B 1/3, C 0/3 (car 2 C misses `c1# 43 [41]`,
   `steer# 75 [73]`, `|steer|~ 10 [9]`; car 1 C `c0# 19 [20]`, `steer# 39 [40]`).
5. **The default build reads uninitialised stack in every car<->car contact.** A build that only wrapped VehicleCarCarContact in
   a new frame (knobs off) moved `B_DEF_K0` from `6efb49ed` to `ddddf961`. The default's (b) and D2 numbers are therefore a
   function of that function's stack layout as well as of the game state; any edit to it or its caller can move them. The
   trace was rebuilt in noinline helpers so this branch keeps `6efb49ed / 3e0724ac / 395fb8b7`. A flip of
   `MASHED_CC_MATRIX_INVERT` (or `MASHED_CC_PC24`) is Mariano's decision; the (b) gate as written refuses it (car 2 A).

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knobs off, exes `72230970`, `6a85044d`, `24e6d3f6`, `d1d8eaec`: `6efb49ed / 3e0724ac / 395fb8b7` | PASS |
| G-CC | `CR_PC` 64/64 rows exact; in-run contact entry + exit exact (`ccentry.py`) | PASS |
| G-B | section 0 item 4 | FAIL as default |
| G-DEF | not run: G-B regresses A on the default | - |

## 2. Tools

`re/frida/scenario_launch.py --carcar-probe N` (VehicleCarCarContact entry + leave, both records). `MASHED_CC_REPLAY` /
`MASHED_CC_REPLAY_OUT` (the port body on the original's entry records, diffed against its leave records, at track load, no
game state). `MASHED_CC_TRACE` (port in-run rows in the probe's format). Scripts: `recslot0.py`, `ccentry.py`.
Evidence in `D:/MashedWT/u9769_data` (`MOVED_OFF_GIT.txt`).

## 3. Next

U-9770: car 3's record call 7 (`+0x9b8`), and car 1's row 149 (FUN_00414c30 returns 0 in the original, 2 in the port).
