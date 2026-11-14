# RESULT IMG-5-ORIGTOP-DEFAULT: image default ON on the MASHED_ORIG_TOP path (L-149 (a) step 5, L-151 (a))

Row `IMG-5-ORIGTOP-DEFAULT` (D4, run 46). Branch `orch/IMG-5-ORIGTOP-DEFAULT` from `orch/integration` @ `d1e5ac586`.

## Verdict: HANDOFF (code change and default build done; the OFF/ON/OFF/ON captures were NOT completed, so P1 is unproven)

Nothing here may be read as "captures unchanged". No ON boot has produced a log yet.

## Caveat (verbatim from the lead)

IMG-5's image-ON boots reach AppDestroy with no AV only under the run-45 null guard, with 130 of 286 teardown destroys called with 0
([UNCERTAIN], cause open in IMG-5-TEARDOWN-NULLS). Do not read "no AV" as "no init or load stand-in left". The null guard is untouched
(separate row RWC-NULL-DESTROY-GUARD).

## What changed (2 files, knob path only)

- `Compat/PortStateImage.cpp` `PortStateImage_On()`: unset now means ON; `MASHED_PORTSTATE_IMAGE=0` opts out. Its only caller is WinMain's
  `origTop` branch (`exe_main.cpp:10649`), so the default (hand) path never consults it.
- `Compat/PortTopCodePtr.cpp` `PortTopCodePtr_Apply()`: `MASHED_OT_CODEPTR` unset now means ON; `=0` opts out. It is also called only under `origTop`
  (`exe_main.cpp:10657`). Reason it moves with the image: RESULT_IMG-5-TEARDOWN-BODIES shows image-ON boots need the bound slots
  (without them slot 0x005f99b0 keeps its original `.text` address, which is zero pad bytes in the exe).
- Scope note: with the image OFF, a default `MASHED_OT_CODEPTR` now binds the 188 slots on the seed path too. The OFF arm therefore sets both opt-outs
  (`MASHED_PORTSTATE_IMAGE=0 MASHED_OT_CODEPTR=0`), which equals the previous default.
- Not changed: `MASHED_CODE_TRAP` (stays default OFF; its default is a separate decision), the null guard, hand path, gates.

## Checks run

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, log `D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/build_default.log` |
| OFF arm 1 (`MASHED_ORIG_TOP=1`, both opt-outs, 3000 frames) | rc 0 after 32.5 s, log `D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots/off1.log` |
| ON arm 1, OFF arm 2, ON arm 2, hand-path arm | not completed: ON arm 1 was queued behind `mashed_lock` (held by SOAK-P0, pid 10636) when this worker stopped |
| object build (`MASHED_DDKOBJ=1`), `ddkobj.py check` | not run |
| self-tests (`[OT_CODEPTR]`, `[portstate-image]`, witness) | not run |
| G-CTRL / B / 12x4 captures | not run: top-down row |

## Pending (resume with these)

1. `D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots.sh` (arms off1 on1 off2 on2 hand) was still running in the background under `test-slot` with
   `mashed_lock.py run` inside, output `boots.out`, logs in `boots/`. Exe: `D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/mashed_re_default.exe`
   sha256 `FBAAEA5E454F3AAF4CC268BA7E24BB2F2D6F302EAFAF064B1E1E6622417EF4A8`. Private root `D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_root`.
2. Compare off1/off2 against on1/on2: rc, `[ORIG_TOP]` lines, end-of-log sequence with 8-digit hex masked. P1 needs ON == OFF in the captures and logs.
   Expectation to test, not a finding: ON logs will differ from OFF in the image and code-pointer lines, since the loader and the slot binding log
   themselves. If the program's visible sequence differs, this is a DECISION row, not P1.
3. Then the object build, `ddkobj.py check`, and the self-test lines on ON.
4. If the stale `boots.sh` run is still alive, its PIDs belong to this row only (command lines contain `IMG-5-ORIGTOP-DEFAULT`).

## Raw files (not committed)

- `D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/build_default.log`, `boots.sh`, `boots.out`, `boots/off1.log`, `mashed_re_default.exe`.
