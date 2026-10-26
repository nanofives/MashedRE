# RESULT D4-WSJ-80E: the 0x80E sub-sound directory record, decoded from the exe's reader

**Verdict: GREEN.** Branch `orch/D4-WSJ-80E` off `orch/integration` @ `86bdd5121`.
Static only, inside the solo window: Ghidra headless decomp on pool slot `Mashed_pool10` (`-readOnly`), and the bytes of
the banks in the finished private root `D:/MashedWT/D4-WSJ-REF_root/original/toastaudio` (no MASHED process was running,
nothing under `original/` was opened). No build, no boot.

Scope: gap G2 of `verify/d4scope_20261026/SCOPING_D4-WSJ.md`. L-24 (a) asks for the stand-in to be filled by data,
including the wave table with voice counts.

Full layout with every field's reader: `re/analysis/structs/rws_80e_stream_header.md`.

## Answer in four lines

1. The sub-sound record is **record A, stride `0x20`**: `+0x18` byte length and `+0x1c` byte offset in the 0x80F data,
   `+0x0c` a per-layer dword, `+0x00`/`+0x04` UUID/name present flags. Count = BODY `+0x0c` (chunk data `+0x20`).
   The old "~53 B/record" in the REmap note was wrong.
2. The walk is the exe's own: `FUN_005b15e0` reads a `0x14`-byte prefix then `prefix+0x00` bytes of body
   (`0x005b17e9`, `0x005b1812`), and `FUN_005b8200` lays the body out. `re/tools/rws_80e.py` replays it.
   On all 31 0x80D banks it ends exactly on `prefix+0x00`, and the A records tile the 0x80F data exactly.
3. **There is no voice-count field in the 0x80E header.** The voice counts are in exe `.data`:
   - the permdict table `0x006044f0` (stride `0x94`, 43 rows, 139 voices);
   - the course tables `PTR 0x0060c820[track id]` (stride `0x90`, 13 tables, one per track).
   Field `+0x84` is the count, read by `FUN_0045d460` and `FUN_00462950`. All 232 rows are dumped in `wave_tables.tsv`.
4. **`cdaudio.rws` holds one sub-sound** (`Segment0`, offset 0, length 8,460,288 = the whole 0x80F).
   No per-track music can be selected from it.

## Check results

| check | result |
|---|---|
| walk end == prefix `+0x00` (A = `0x10`, `FUN_005a6190` floor) | 31/31 banks (`rws_80e_banks.md`) |
| A records tile 0x80F (offset 0, contiguous, last end = 0x80F size) | 31/31 |
| name-advance rule (`strlen` vs `strlen+1`) | both land on the same end in every file (no name is 15+ chars). The files do not tell them apart, and the decode does not depend on it |
| 98 A records in every character bank, same names modulo prefix | 30/30 (1 distinct name list) |
| A `+0x08`, `+0x10` (nC), `+0x14` = 0 | all 2,941 A records |
| wave-table names present in their bank (`name\0` in the .rws bytes) | 43/43 permdict, 189/189 course rows |
| permdict `+0x84` sum | 139 = `DAT_0068fc84` measured in D4-WSJ-REF |
| TRAINING course table (`0x006085c0`, track id 30) | 12 rows, 35 voices = REF's 139..173 |
| REF per-wave voice ranges (o_train1, 43 permdict + 12 TRAINING rows) | 55/55 names and counts equal `+0x84` |

## What a follow-up can use directly

- **Wave table by data** (L-24 a): `wave_tables.tsv`.
  - Per row: name, count `+0x84`, flag `+0x88` (selects `FUN_005a6dc0(v,8,1,0|1)` and voice `+0x34` bit 2), and the
    dword `+0x8c` (written to voice `+0x74` and passed as `FUN_005a6dc0(v,3,1,x)`).
  - The bases are cumulative at load, permdict first, then the course table of the loaded track.
  - `track_course_tables.tsv` maps the 13 track records (`0x005f33f8`, stride `0x48`, id at `+0x10`) to their course table.
  - The largest total is 139 + 54 (highway) = 193, inside the 200 virtual voices `FUN_0045d460` creates (`0x640/8`).
- **Character taunts (G4)**:
  - `rws_80e_records.csv` gives every clip's offset and length, per bank.
  - The original selects a clip by index: `FUN_004656e0` builds a selector `{index = *(DAT_0088f680[slot]+8), 3, 0, 2}` and
    calls `FUN_005a86a0`. Selector `+0x04` = 3 is neither the name path (1) nor the UUID path (2).
  - The chooser is `FUN_0045cd30`. It compares the group size against `0xd` / `0x19` / `0x23`, the same 13 / 25 / 35 as the
    name groups `1_1` / `1_2` / `1_3` / `1_4` (13, 25, 25, 35).
  - Decoding that chooser is the G4 follow-up, not done here.
- **Codec facts** (raw, not interpreted further):
  - Every bank's codec UUID is the bytes at `DAT_005e64b4`.
  - Record B `+0x1c` = `+0x20` / (`+0x1a` x fmt `+0x0d`) in every file. That is 228 = 8208/36 for the character banks and
    2304 = 18432/(4x2) for cdaudio.
  - For cdaudio `FUN_005b8200` rewrites B `+0x0c`/`+0x1a`/`+0x18` to `0x40`/`0x24`/4 at load, because fmt `+0x0d` = 2.

## [UNCERTAIN] items left open

- Which reader consumes format descriptor `+0x00`. Its values are 44100, and 11028 in english bluejay/melon/pink/red/shadow.
  No reader was traced. The port decodes these banks at 44100 (`RwsBank.cpp:119` scans for 44100/22050). Whether that
  matters for the five english banks needs the `+0x00` reader. Path: xrefs into the stream object's copy of the
  descriptor (`FUN_005a8890` copies 7 dwords to stream `+0xc0`).
- Meaning of BODY `+0x1c` (`0x800`), PREFIX `+0x04` (`0x14`) and `+0x0c` (`0x24`). No reader was found in the functions read.
- Permdict row `+0x90` (2000.0 as f32 on eng1/eng1d/eng3*/eng4*, 0 elsewhere): not read by `FUN_0045d460`. Another reader
  may exist.

## Notes for the lead

- **D4-WSJ-MUSIC's premise does not hold.** "Per-track cdaudio selection" has nothing to select: cdaudio has one sub-sound,
  and D4-WSJ-REF measured that the original never opens `cdaudio.rws` on TRAINING. The track loops that do exist are
  course-table rows (for example `musicloop1`/`musicloop2` in city, `0x0060c550`/`0x0060c5e0`). The row needs re-scoping.
  That is the lead's call, so no decision row is filed.
- `re/tools/rws_voice_index.py` read the right offsets for a different reason (its "cut table at 120" is A `+0x18` when the
  A array starts at chunk `+0x60`). Its clip names are the `<PP>_<n>` tail of `1_<g>_1_<PP>_<n>`. Its docstring now
  points here. Its numbers are unchanged.

## Gates

Not run. No exe or `.asi` source changed: this branch adds a Python tool, two read-only Ghidra scripts, notes and tables.

## Committed

- `re/tools/rws_80e.py` (the walk + checks).
- `re/tools/ghidra_scripts/WaveTables.java` and `TrackIds.java` (headless, read-only dumps).
- `re/analysis/structs/rws_80e_stream_header.md`, plus a pointer in `re/analysis/audio_character_banks_REmap_20260616.md`.
- This directory:
  - `rws_80e_banks.md` (one row per bank);
  - `rws_80e_records.csv` (2,941 A records);
  - `wave_tables.tsv`;
  - `track_course_tables.tsv`;
  - `decomp_005b8200.c`.

Reproduce:
```
py -3.12 -I re\tools\rws_80e.py <root>\toastaudio\pc\audio\pcdics\cdaudio.rws <root>\...\english\*.rws --md out.md --json out.json
analyzeHeadless <pool> Mashed_poolN -process MASHED.exe -readOnly -noanalysis -scriptPath re/tools/ghidra_scripts -postScript WaveTables.java out.tsv -postScript TrackIds.java tracks.txt
```

## Raw (not committed), SHA-256

Root `D:/MashedWT/D4-WSJ-80E_data/`:

| file | SHA-256 |
|---|---|
| 80e_all.json (full walk output, 31 banks) | f63d8de91d9f7d56494aa08ca2a5d945f6b3d68f45be92cedb367b86c9afd435 |
| f5b15e0.c | 79b87291373ca39ddca1c773ba0590fc91374ada6bfaf14b27d9e8889ef65fe1 |
| f45d460.c | 97650723d101a0e7d6954ced23f7db2a022abe89b218b503d46e33cfc3c3b01e |
| f_0x00462950.c | 8b8bf27d37aaf51ec5d012848e124f1831047c4d68bf7c7a0cd93b187f2cd37b |
| f_0x005b30e0.c | 83f0d89a79f8cbb90ebd21183c26c4f6fd8d070aff76878030736e3d7c71b54d |
| f_0x005b3020.c | ab10e299c9ef40bb5ad053e4fbe8ac4203cb3c80bd024881d994f4cd0195ed14 |
| f_0x004656e0.c | 0788c8b5ea0cc195fe84a94b6b7c892cdaae670ec1e0138f1c90a602abce9404 |
| f_0x0045cd30.c | 0a3bfd1b68ef7236df47ab5a36ef894cca91135195ec74cab1f7ccad31f728c0 |

Input banks (`D:/MashedWT/D4-WSJ-REF_root/original/toastaudio/pc/audio/pcdics/`):

| file | SHA-256 |
|---|---|
| cdaudio.rws | fb715e6acc1fbfd392dbfe338eca02127d629ff80284e26294d584a8f292cd02 |
| english/red.rws | ef5627eb77b2ab2d6fd91a84da36b180243060cc4d4bef918116580458d2b419 |
| english/gold.rws | 40438a9d931baa2529c2634a3271094fda31113785eccf7a92be2e3760a6f6dc |
