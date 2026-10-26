# RESULT D4-WSJ-ENGCLASS: vehicle to engine class (eng1..eng4)

**Verdict: GREEN (static).** Branch `orch/D4-WSJ-ENGCLASS` off `orch/integration` @ `86bdd5121`. G1 of SCOPING_D4-WSJ.md is closed
and the `[UNCERTAIN]` at `AudioEngine.h:47-50` / REmap note §4 is resolved: the assignment is **code, not data**: a switch in the
per-car engine tick `FUN_00463640`. Ghidra headless decomp + disassembly on pool slot `Mashed_pool10` (`-readOnly`), exe bytes read for
tables. No build, no boot, nothing written to `original/`.

## Answer

Chain (all addresses in `MASHED.exe`):

1. Per-car tick `FUN_00463640(car)` (called at `0x00466b50`'s race loop, `FUN_00463640(iVar2)`) reads `FUN_0041efc0(car)` = `*(DAT_0063dc48 + car*0x2ac) + 0x80` (0x0041efc0).
   That pointer comes from `FUN_0041f330` (0x0041f330): a lookup over the vehicle record table at `0x005f37a8`, stride `0x84`, count `DAT_005f5fe0` = 78, id at record `+0x80` (first id 0 at `0x005f3828`).
   Ids run 0..77, **6 records per vehicle, 13 vehicles** (name string at record `+0x20`), not 12 as the REmap note says.
2. Vehicle index = id / 6 (`IMUL 0x2aaaaaab`, 0x00463653..0x00463662, result in EDI).
3. `CMP EDI,0xb; JA 0x004636d9` (0x0046367f, 0x00463687), else `JMP [EDI*4 + 0x463c34]` (0x00463689). Jump table dwords (read from exe):
   `0x463690 0x4636a6 0x4636c3 0x4636a6 0x463690 0x463690 0x4636ad 0x4636d9 0x4636c3 0x4636c3 0x4636d9 0x463690`.
4. Targets write `DAT_0069055c` (class) and `DAT_006130c4` (min rpm): `0x463690`: 3 / 1500.0; `0x4636a6`: 1 then falls to `0x4636e3` 2000.0; `0x4636ad` and `0x4636c3`: 2 / 1500.0; `0x4636d9`: 0 then 2000.0.
5. Row = class * 2 (`IMUL EAX,EAX,0x128` at 0x0046370b = 2 rows of 0x94), so class 0..3 = permdict `eng1`/`eng2`/`eng3`/`eng4` (`wave_tables.tsv` rows 0,2,4,6) and the `d` row is row+1.

| vehicle idx | name | class | permdict row | d row |
|---|---|---|---|---|
| 0 | Shuriken | 3 | eng4 | eng4d |
| 1 | Advantage | 1 | eng2 | eng2d |
| 2 | Atmos | 2 | eng3 | eng3d |
| 3 | Stallion | 1 | eng2 | eng2d |
| 4 | Bullet | 3 | eng4 | eng4d |
| 5 | Sputter | 3 | eng4 | eng4d |
| 6 | Cooler | 2 | eng3 | eng3d |
| 7 | Shorty | 0 | eng1 | eng1d |
| 8 | Crusader | 2 | eng3 | eng3d |
| 9 | Creeper | 2 | eng3 | eng3d |
| 10 | BamBam | 0 | eng1 | eng1d |
| 11 | Kustom | 3 | eng4 | eng4d |
| 12 | Formula | 0 | eng1 | eng1d (index > 0xb, the JA at 0x00463687) |

Full per-vehicle table with record VAs and constants: `engine_class_table.tsv`. Code form: `mashedmod/src/mashed_re/Audio/EngineClassTable.h` (data only, no caller, no behaviour change).

## Per-car engine voices (the other half of the request)

- **Voice slot = `DAT_00604570[class*0x4a] + car`** (0x00463725 loads the row's base slot, 0x00463731 adds `car`), one of the row's **4** voices (`+0x84` = 4 on all eight eng rows, `wave_tables.tsv`). So the car index (0..3), not the vehicle, picks the voice inside the class row.
- **Second layer, always on**: `DAT_006130bc` is set to 1 at 0x00463682 and nothing in this function clears it. The loop therefore starts and drives a second voice at slot **+4** (0x00463768 `DAT_0068f660 + slot*8`, `FUN_0045daf0(iVar5+4,car)`, `FUN_0045e0f0(iVar5+4, ...)`), which is the next row (`eng<N>d`) because each eng row has 4 voices and the rows are consecutive. [UNCERTAIN] whether `+4` slot and the `d` row base are the same voice: the bases are cumulative at load (80E RESULT), so base(row+1) = base(row)+4 follows from that, but the cumulative rule is the 80E finding and was not re-measured here.
- **Pitch**: voice `+0x3c` = rpm * `_DAT_005ce26c` + fVar4, with fVar4 = car * `_DAT_005ce194` + `(&DAT_00604580)[class*0x4a]` (0x0046371f, 0x0046372b). `DAT_00604580` is permdict row `+0x90`: 2000.0 on eng1/eng3/eng4 and 0.0 on eng2. **This is the reader the 80E RESULT listed as not found** (row `+0x90`, rows 0..7 values in `wave_tables.tsv`). Values read from exe at class rows: 2000.0 / 0.0 / 2000.0 / 2000.0.
- **Volume**: `rpm * _DAT_005cc990 + DAT_00613020[class]` (0x00463b3a...). `DAT_00613020` floats = 0.2, 0.2, 0.2, 0.3.
- Min rpm `DAT_006130c4` = (1500.0 or 2000.0, per class above) + `FUN_00472650(0, 250.0)` (0x004636ed..0x004636f9, `0x437a0000`), max `DAT_006130c0` = 9500.0.
- Consts: `_DAT_005ce194` 200.0, `_DAT_005ce26c` 3.5 (first dwords as read; they are indexed tables in the exe, not re-traced here).

## What this does not cover

- The rpm state machine (`DAT_0069054c` states 0..4, gear `DAT_0069052c`, tables at `0x00613030`..`0x006130c8`) is not ported by this step. Values are in the decomp (`D:/MashedWT/D4-WSJ-ENGCLASS_data/f463640.c`) for a follow-up.
- Which vehicle a given TRAINING run uses is a cfg matter, not measured here. `RaceSession.cpp:183` still calls `Audio::EngineStart(0.5f)` with no vehicle argument. Wiring the table needs that parameter and a default-OFF knob + PREREG (D4-WSJ-WIRE-shaped), not done.
- Cars 4..15 index past the 4-voice row; the original only ticks the cars `FUN_0040e370(i)` accepts, and `DAT_006904b4` etc. are sized for 16. [UNCERTAIN] whether a >4-car race overruns the row into the `d` row's slots, no count measured.
- No Frida diff, so nothing here moves a C-level.

## Raw files (not committed)
Under `D:/MashedWT/D4-WSJ-ENGCLASS_data/`: `out.json` 5e8ccfdd4cdf79c5e9f9ba9871092726df53dfa6f46d270426e7c004c20da361, `out2.json` e7010cc18b99f6e7e874603747549dd6db20f4e5c5e93030b4649c8cb8dd1bf1, `out3.json` ad52a66c42f2127e12d3e9050fff93adabd7fe2431fffe239727c2c7ce2bc773, `dis.txt` e268cc5c878ae877121a5f917156487f9d9a50034053b156ea0f5c75dbe2f0d0, `rd.py` (exe table reader), `xr.txt` (xrefs into 0x006044f0..0x00605dd0).

## Gates
G-CTRL and B: not run, no behaviour change (data header with no includer, notes).
