# PREREG RWC-THUNK-BIND-AUDIT: 12_City red-merge diagnosis (run 44)

Committed before any arm runs.

## Observation (train 20261113_114635, merged tree 413243edc)

- 12 of 13 tracks: `tracks_score.txt` rows identical to the reference `verify/run13p3_20261029/tracks_score_sub_p3.txt`
  and `stand/cam_dump.csv` md5 identical to trains 20261113_105809 and 20261113_121927.
- 12_City only: matched 486/469/417 (reference 489/472/420), one more cited row. `cam_dump.csv` differs from the 4th line
  (seq 3, replay row 1): column `flag` (= `*(int*)0x00897fe0`, TrackRenderer.cpp:10001) is 0 where the GREEN runs read 1,
  `cd` (countdown_) 0 where they read 3, and the pose columns differ from seq 116 on.
- 12_City row values were identical in every train pass from 20261101_114821 to 20261113_121927 (~130 passes, many
  exes), so City has no history of layout flips.

## Arms (all: `verify/rsky_20261016/run13.py run --tracks 12`, private root `D:/MashedWT/RWC-THUNK-BIND-AUDIT_root`,
test-slot outside, `mashed_lock.py run` inside; scored with `run13.py score --tracks 12` and the train's cites file)

| arm | exe |
|---|---|
| BASE | default build of 69cd03777 (branch base) |
| TIP | default build of 06d2c9ad6 (branch tip) |
| TIPRSP | 06d2c9ad6 sources, `mashedmod/exe_sources.rsp` from 69cd03777 plus the one new file `RwCore_004c7a60.cpp` appended (link order of the base, code of the tip) |

Order: BASE, TIP, BASE, TIP, then TIPRSP x2 (only if TIP differs from BASE).

## Measures

- City rows from `tracks_score.txt` (matched / m.tri / rows) and md5 of `stand/cam_dump.csv`; `flag`/`cd` columns at seq 3.

## Decision rules (fixed before the arms)

1. BASE x2 reproduce the reference rows and TIP x2 reproduce the GREEN rows -> the train result was an environment event
   (not reproduced). Re-queue GREEN with these arms as evidence.
2. BASE x2 = reference and TIP x2 = the RED rows -> the branch changes the default-path City run. Then:
   - TIPRSP x2 = reference -> the cause is link order / code layout of the default exe (layout-sensitivity class),
     not the thunk binding code. Report DECISION (gate table moves; options: accept the new City table as reference,
     or keep the base link order in `exe_sources.rsp`).
   - TIPRSP x2 = RED rows -> the changed code content reaches the default path. Report DECISION / RED with the site.
3. Any arm with mixed results within itself (BASE != BASE or TIP != TIP) -> the City run is not deterministic on this
   machine now. Report that and add arms before any verdict.

## Amendment 1 (before any arm ran)

Map comparison of the three builds (`mapcmp.py`, maps kept in the data folder): TIP and TIPRSP move the same 6,976 of
28,865 non-RwCore symbols, each by +0x10, against BASE. The shift comes from the new 16-byte body
`RwCore_004c7a60.cpp`, not from the order of `exe_sources.rsp`, so TIPRSP does not separate layout from code. It is
dropped. New arm:

| arm | exe |
|---|---|
| BASE7A60 | 69cd03777 sources plus `RwCore_004c7a60.cpp` from the tip, appended to the generated block of the base rsp (no caller binds it: the base `RwCore_004c32b0.cpp` still names `thunk_FUN_004cfa00`). Accepted only if its non-RwCore symbol addresses equal TIP's. |

Order: BASE, TIP, BASE, TIP, then BASE7A60 x2 (only if TIP differs from BASE).
Rule 2 now reads: BASE7A60 x2 = RED rows -> layout (default-path code shifted by 0x10) is the cause.
BASE7A60 x2 = reference -> the rebound call targets themselves reach the default path.
