# `.AI` opponent-AI path-data format — CRACKED + data-verified (WS-C2 step 2, 2026-06-16)

The race-line splines + track tile grid the opponent-AI controller
(`FUN_00418860` cluster) follows. Parser/validator: `re/tools/ai_data.py`
(`validate` = 13/13 members GREEN, exact byte consumption).

Anchored to `original/MASHED.exe` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
NO-GUESSING: every constant cites the RVA it was read from.

## Container

`TOASTART/Common/AI.piz` (PIZ archive, 13 entries) holds members `AI%d.AI`
where `%d` = track Course_Id (`FUN_00426c00`; filename built by `FUN_00423480`
as `"AI%d.AI"`). Members present: 0, 2, 3, 11, 25, 26, 30, 33, 34, 36, 37, 38, 39
(matches the cracked Course_Id table — [[project_track_course_ids]]).

Each member is exactly **0x11890 bytes** = a single RW-style chunk:

```
off 0x00  u32  type    = 0x13269902      (FUN_004235b0 FUN_004cc5e0 tag arg)
off 0x04  u32  size    = 0x11884         (payload length)
off 0x08  u32  version = 0x1c02000a      (RW lib-version; 0x1803ffff on AI30.AI)
off 0x0c  ..   payload  (0x11884 bytes)  -> loaded verbatim to DAT_007f1a9c
```

- Writer `FUN_00423540`: `FUN_004cc580(h, 0x13269902, 0x11884, 0x37002, 10)` then
  `FUN_004cbe80(h, &DAT_007f1a9c, 0x11884)` (header then payload).
- Reader `FUN_004235b0`: `FUN_004cc5e0(h, 0x13269902, &size, &ver)` then
  `FUN_004cbd30(h, &DAT_007f1a9c, size)`. **The reader ignores `version`** — so
  AI30.AI's `0x1803ffff` stamp is harmless (verified: it parses fine).

## Round-trip identity (D4-WSF-TRACKDATA-RT, 2026-10-26)

`FUN_004cc580` packs its args 4/5 as
`((p4 + 0x10000 & 0x3ff00) << 0xe) | ((p4 & 0x3f) << 0x10) | (p5 & 0xffff)` and writes
`[type][size][version]` as one 0xc-byte `FUN_004cbe80` call (`0x004cc580` body), so the
writer stamps `(0x37002, 10)` = **`0x1c02000a`** on every file. Emit =
`AiData_Emit` (`mashedmod/src/mashed_re/Ai/AiData.h`, test-only) and
`ai_data.py roundtrip`.

- parse -> emit over the 13 shipped members (C++ `Ai/tests/aidata_rt_test.cpp` and
  Python, same result): **12/13 byte-identical**. AI30.AI differs in bytes 8..11 only,
  its shipped stamp `0x1803ffff` against the writer's `0x1c02000a`.
- Original-side witness: `scenario_launch.py --ai-save-capture` loads a race in the
  stock original, dumps `DAT_007f1a9c`, and calls `FUN_00423540` (returns 1, writes
  `DAT_00644110` = `AI<n>.AI` in the game directory). Track 0 wrote `AI30.AI`, track 2
  wrote `AI39.AI`. Both files are byte-identical to `AiData_Emit(DAT_007f1a9c)` (0 of
  71,824 bytes differ). The loaded image equals the shipped payload, and the call
  leaves it unchanged. The original's own `AI30.AI` carries `0x1c02000a`, so the
  stamp difference above is the writer's behaviour, which the emit reproduces.
  Evidence: `verify/trackdata_rt_20261026/RESULT_D4-WSF-TRACKDATA-RT.md`.

## Payload (= the `DAT_007f1a9c` monolithic image, 0x11884 B)

| payload off | abs addr | region | layout |
|---|---|---|---|
| 0x00000 | 0x007f1a9c | **tile grid** | 128×128 `int16`; empty cell = `0xffff` (cited LOS `FUN_00416060`: `grid16[((iz+0x1f0>>3)*0x80)+((ix+0x1f0>>3))]`) |
| 0x08000 | 0x007f9a9c | **sub-cell grid** | 0x200 tiles × 8×8 `char`; wall tile-type = 0 or 3 (cited `FUN_00416060`/`FUN_004150e0`: `grid8[((sub_iz)+(tile*8))*8 + sub_ix]`) |
| 0x10004 | 0x00801aa0 | **RACE** line array | 3 splines × 0x204 |
| 0x10610 | 0x008020ac | **INSIDE** line array | 3 splines × 0x204 |
| 0x10c1c | 0x008026b8 | **SLOW** line array | 3 splines × 0x204 |
| 0x11228 | 0x00802cc4 | **CHEAT** line array | 3 splines × 0x204 |
| 0x11834 | 0x008032d0 | (0x50 trailing bytes) | — |

The 0x4000-byte region at payload 0x04000..0x08000 (abs 0x7f5a9c..0x7f9a9c) sits
between the two grids; not exercised by the ported leaves, carried unlabeled
(debug overlay `FUN_00444ff0` names "danger data"/"save data" sub-modes — `[U]`).

### Spline (stride 0x204, cited `FUN_00418560` 0x0041861f / 0x00418625)

```
off 0x000  64 × { f32 X, f32 Z }   (point i at +i*8 / +i*8+4)
off 0x200  u32 count               (active point count, 0..64)
```

A bank is valid only if `count >= 4` (`FUN_00418560`); the whole AI tick is
gated on `race[0].count > 3` (`FUN_00418860` `DAT_00801ca0`).

## Validation (data-verified, 2026-06-16)

`py -3.12 re/tools/ai_data.py validate` — **13/13** members: header
type/size/version OK, all line-array `count ∈ [0,64]`, exact 0x11890 size. Sample
race[0] counts: AI0=31, AI11=32, AI37=54 (all ≥4 → tick enabled). Tracks with a
single race line (AI11) correctly show inside/slow/cheat = 0.

## Port status

- Format **CRACKED + data-verified** (this note + `re/tools/ai_data.py`).
- C++ structs/constants: `mashedmod/src/mashed_re/Ai/AiData.h` (header-only;
  RVA-cited; `AiData_LoadInto` copies a validated payload to the controller's
  expected base). Consumer = the ported controller once it runs in the standalone
  (the dev `.asi` uses the original `FUN_004235b0`); wire then.
