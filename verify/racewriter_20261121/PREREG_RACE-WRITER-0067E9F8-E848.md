# PREREG RACE-WRITER-0067E9F8-E848 (run 54, 2026-11-21)

Committed alone, before any arm. Follow-up of `verify/ro_20261116/RESULT_RACE-WRITER-0067EA74.md` section 3 / next 2.

## Question
Which instruction writes `0x0067e9f8` (ref1 value 1 from tick 17; exe 0) and `0x0067e848` (ref1 value 2 at tick 299; exe 0)
on the original, and can that writer be bound on ORIG_TOP under the existing default-OFF knob `MASHED_OT_RACELOAD`
(profile data only, RULE 1; no binder code edit)?

## Arm W2 (original, measurement only)
`test-slot bash -c 'py -3.12 verify/racewriter_20261121/writer_arms.py w2 -- --watch 0x0067e9f8,0x0067e848 --watch-from install --watch-stack 16'`
Root `D:\MashedWT\RACE-WRITER-0067E9F8-E848_root` (robocopy of `RACE-WRITER-0067EA74_root`), data
`D:/MashedWT/RACE-WRITER-0067E9F8-E848_data`. Recipe = ref1 / W1 (detorig boot, tick clock, warp 300, cap 600).

## Predictions
1. `0x0067e9f8` gets a write of value 1 at or before tick 17 (ckpt 17 = 1, as ref1).
2. `0x0067e848` gets a write of value 2 at or before tick 299 (ckpt 299 = 2, as ref1).
3. Each writer pc lies inside a game function (not RW, not CRT). If a writer is an RVA owned by another row
   (PORT-OT-RACE-BIND-HANDLER, RO-AUDIOINIT, PORT-TOP-LAND-06-RF closures), 0x00458dd0, or a library function, the row stops
   and reports it without binding.

## Then (only if prediction 3 holds and the writer is unowned)
Bind the writer on ORIG_TOP under `MASHED_OT_RACELOAD=1` as profile data; both builds, `ddkobj.py check` against
`log\orch\run54_headcheck\`, knob boot, self-tests, replay oracle arms on the object build (`RO_ROIMG-pr1`, `RO_ROLOAD-pr1`).
Pass: the word that the bound writer produces matches ref1 at its tick on the knob-on arm, knob-off arm unchanged.
