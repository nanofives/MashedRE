# RESULT OT-TIMER-VS2022-FORM (run 42, top-down row)

Branch `orch/OT-TIMER-VS2022-FORM` from `orch/integration` @ `145dec4d0`. Fix commit `5e02ea369`.

## Verdict

The VS2022 body of 0x00493390 now stores `DAT_007f1010` in the same form as the object build (the original's bytes)
on every call measured, in both halves of the clock range. One correction to the row's premise: the strict check from
RO-EXE-REPLAY-KNOB run 4 passes on **both** builds only while `now < 2^31`. When the 3 MHz clock value is `>= 2^31`,
the object build fails it as well (122 and 227 of 6000). The cause is the check's model, not either body (section 3).

## 1. Change (one file, `mashedmod/src/mashed_re/Match/PortTop_00493390.cpp`)

The matched text `re/match/s15/00493390.c` compiles as `PortTop_Body_00493390`. It is declared
`__declspec(noinline)` so the wrapper keeps a real call that ddkobj.py can repoint. `PortTop_Bound_00493390` is the
name the caller binds, and it is unchanged. It calls the body, then re-stores `DAT_007f1010` from
`now = DAT_007f1030` (the body stores it at 0x00493459) using the original's instruction sequence:

| original | wrapper (VS2022 `dumpbin /disasm`) |
|---|---|
| `0x00493414 test edx,edx` | `test edx,edx` |
| `0x0049341a fild dword [esp+4]` | `fild dword ptr [ebp-4]` |
| `0x00493428 jge 0x00493430` | `jge` |
| `0x0049342a fadd dword [0x005cc94c]` (float 2^32) | `fadd dword ptr [k2p32]` (4294967296.0f) |
| `0x00493436 fmul dword [0x005ccd68]` (3.3333333e-07f) | `fmul dword ptr [kScale]` |
| `0x0049344b fstp dword [0x007f1010]` | `fstp dword ptr [ebp-8]`, then `movss` copy to `0x7F1010` (bit copy) |

Nothing reads `DAT_007f1010` between the body's store and the re-store. On the object build the DDK object replaces
`PortTop_Body_00493390` (BYTECHECK row `0x00493390 _PortTop_Body_00493390 ... IDENTICAL`), and the re-store writes
the same bits again. The caller file, the OT_TIMER check and `PortTop_00492370.cpp` are untouched.

A first build without `noinline` inlined the body into the wrapper, which would have hidden it from ddkobj. The
committed version has the call (`call _PortTop_Body_00493390` at wrapper +0x06).

## 2. Runs

Builds: VS2022 and `MASHED_DDKOBJ=1`, both `Build OK`, rva-lint `NEW=0`.
`ddkobj.py check` (inside the DDKOBJ build): IDENTICAL 1145/1175, DIFFERENT 11 [0x00401690 0x0042f530 0x004332a0
0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00], not-in-map 19, units linked
1126/1393, refusals 4 (wrapper-redefines-globals=4). This matches the merge-train baseline for `orch/integration`
(`re/orchestrator/INTEGRATION_LOG.md:4`, 2026-11-13 00:04). Both 0x00493390 and 0x00493480 are IDENTICAL.

The strict check is on RO-EXE-REPLAY-KNOB, which is not merged yet. To run it here, the one check line in
`PortTop_00492370.cpp` was changed **in the working tree only, never committed** to the run-4 form
(`TmCheck(g1010 == x1010, ...)`. `X87MulExact` is the same function as that branch's `F1010SingleRounding`). The
line was reverted afterwards. Arms: `knob_boot.py <label> D:/MashedWT/OT-TIMER-VS2022-FORM_root 180
MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600 MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1`, each under test-slot ->
`mashed_lock.py run`, all rc 0, 600 frames.

| arm | exe | `now` range | strict OT_TIMER | x87-only hits |
|---|---|---|---|---|
| vs_timer_strict_nofix (control, integration wrapper) | VS2022 | 3.355e9 .. 3.37e9 | FAIL 98 of 6000 | 0 |
| vs_timer_strict_fix | VS2022 + fix | 9.79e8 .. 9.92e8 | **PASS 0 of 6000** | 112 |
| vs_timer_strict_fix_b | VS2022 + fix | 9.72e8 .. 9.87e8 | **PASS 0 of 6000** | 111 |
| ddk_timer_strict_fix | object build + fix | 3.259e9 .. | FAIL 122 of 6000 | 0 |
| ddk_timer_strict_lo | object build + fix | 2.945e9 .. 2.960e9 | FAIL 227 of 6000 | 0 |
| vs_timer_strict_fix_hi | VS2022 + fix | 2.514e9 .. 2.529e9 | FAIL 211 of 6000 | 0 |

"x87-only hits" counts calls where the stored value equals the single-rounding form and differs from the
cvtsi2ss + mulss form. Below 2^31 the fixed VS2022 body stores the single-rounding form on 111 to 112 calls where the
old body did not, and fails 0. Above 2^31 both builds fail in the same way: the stored value is the cvtsi2ss + mulss
form (call 1 of ddk_timer_strict_fix: got `0x4487C68D` = `float(now) * k` for now 3258614363, model `0x4487C68E`;
ddk_timer_strict_lo: got `0x4475660B` = `float(now) * k` for now 2944783368, model `0x4475660C`).

`now` is `(QPC * 3000000 / freq) mod 2^32` (`re/match/s16/004950b0.c`). It wraps every 1431.7 s, so a run lands in
either half depending on wall-clock time. Run 4's real-clock arms and its virtual clock (about 3.2e8) were all below
2^31.

## 3. The strict model is wrong for `now >= 2^31` (handoff to the check owner)

For `now >= 2^31` the original takes the unsigned fix-up: `fild` loads a negative int32, and
`0x0049342a fadd dword [0x005cc94c]` adds 2^32. The x87 rounds that sum to the precision-control setting. The data
fits PC_24: the sum is rounded to 24 bits (= `(float)now`), then `fmul` rounds once more, which is exactly the
cvtsi2ss + mulss value. Under PC_53 the fadd would be exact and the result would match the single-rounding model. The
object build, which runs the original's bytes, gave the `float(now) * k` value on every failing call. `F1010SingleRounding`
assumes an exact `now` on every call, so it fails the original's own bytes in that half.

[UNCERTAIN] The x87 control word in the exe was not read directly. The PC_24 reading comes from the object build's
values above, and it is consistent with D3D9 `CreateDevice` without `D3DCREATE_FPU_PRESERVE`. A log of `_controlfp`
inside the timer arm would settle it.

Suggested model fix (RO-EXE-REPLAY-KNOB owner, `PortTop_00492370.cpp`, not done here because this row may not touch
the check): expected = `F1010SingleRounding(now)` when `now < 2^31`, else `FBits((float)now * 3.3333333e-07f)`, or model
the PC_24 fadd rounding directly. Then re-run the strict arms with `now >= 2^31` on both builds.

## 4. Not run

- Final rebuild of the committed tree after reverting the scratch check line: not run because `mashed_lock.py status`
  showed the merge train's PRIORITY-HOLD (`merge-train:20261113_043823`), and WORKER_RULES forbids build.bat then. The
  committed tree differs from the measured exes only in the reverted check line, which is in a TU that is not a
  matched unit. The ddkobj counts above were taken on the measured tree.
- No G-CTRL, no B, no 12-capture check (top-down row). No menu walk (no menu code changed).
- No tracker or C4 change.

## Raw data (`D:/MashedWT/OT-TIMER-VS2022-FORM_data/`, SHA-256)

- ddk_timer_strict_fix.log `2b6e68614588d2b826603a6b8f748c6b44bbe81ef64b49de8d528a8579465c54`
- ddk_timer_strict_lo.log `d7efcdc6d2ae49206e18ec6b58765d4ab3d216d68e0f44723406125007e97142`
- vs_timer_strict_fix.log `d88dc66d56a42dc94662d5550e223174e5da48c1c6d4b5ebc6fb6f2fdd3b5baa`
- vs_timer_strict_fix_b.log `c65326715efb80cd6e154e530fe32b01b2b15892911c95449e89493241c606d5`
- vs_timer_strict_fix_hi.log `e9fd28e2826a956fdcc4cb5835bfb48d518443b9880b454640b33cc4b6ef4acd`
- vs_timer_strict_nofix.log `fe77a626f3c9f91cce3deba7f683dbebe73a53d580fee50b67fe068f0522b772`
