# Audio — character voice banks, engine source, music streams (WS-J map)

**Session:** WS-J (audio remainder), 2026-06-16
**Binary:** MASHED.exe, anchor SHA-256 BDCAE093… , Ghidra image_base 0x00400000
(read-only pool9). Every address below was verified live this session.

This note resolves WS-J's "vehicle→character engine-bank map (6 char banks vs 12
vehicles)" and corrects two **wrong assumptions** baked into the 2026-06-15 race
scaffold (`Audio/AudioEngine` + `Race/RaceSession`).

---

## 1. The 6 `english/<name>.rws` banks are per-CHARACTER VOICE banks, not engine banks

Evidence (data, this session):

- They live under the **language** folders
  `toastaudio/pc/audio/pcdics/{english,french,german,italian,spanish}/` — i.e.
  they are *localized*. Engine sound would not be localized.
- The 0x80E sub-sound directory of each bank names every clip
  `<g>_<s>_<k>_<CODE>_<n>` where CODE is a 2-letter **character** tag, one tag
  per bank (parsed from the 0x80E header, offset +5192 within the 0x80E):

  | bank file   | char tag | clips (0x80E) |
  |-------------|----------|---------------|
  | bluejay.rws | `BJ`     | 88            |
  | gold.rws    | `GD`     | 82            |
  | melon.rws   | `MN`     | (few unique)  |
  | pink.rws    | `PK`     | (few unique)  |
  | red.rws     | `RD`     | 98            |
  | shadow.rws  | `SW`     | 98            |

  e.g. red.rws clip names are `1_1_1_RD_1 … 1_4_1_RD_35` (context-grouped
  dialogue lines). These are **driver taunt/commentary** clips.

→ The scaffold's `EngineStart(…, "english/red.rws")` decoding "sub-sound 0
(RD_1)" as "a tonal engine loop" is **wrong**: `1_1_1_RD_1` is a Red voice line.

## 2. Character index → bank name table (DAT_006041f0)

`FUN_004625b0` (0x004625b0–0x0046273f, the localized pcdics path builder) builds
**6** path buffers at `DAT_00690158` (stride 0x80), one per character, from a
6-entry base-name table at **DAT_006041f0** (stride 0x80). Raw bytes read this
session:

| char index | address    | name (ASCII, .data) |
|-----------:|------------|---------------------|
| 0          | 0x006041f0 | `RED`               |
| 1          | 0x00604270 | `BLUEJAY`           |
| 2          | 0x006042f0 | `MELON`             |
| 3          | 0x00604370 | `GOLD`              |
| 4          | 0x006043f0 | `PINK`              |
| 5          | 0x00604470 | `SHADOW`            |

(Filesystem is case-insensitive; these resolve to `bluejay.rws` etc.)

Path format built by FUN_004625b0:
`toastaudio\pc\audio\pcdics\` (0x005ce770) + `<lang>\` + `<name>` + `.rws`
(extension bytes 0x005ce734 = `.rws`, 0x005ce738 = `\0`).

Language switch on `param_1` (corrects the old C1 note's UNCERTAIN cases):

| param_1 | folder string        | value     |
|--------:|----------------------|-----------|
| 0       | `english\` 0x005ce764| english   |
| 1       | `french\`  0x005ce75c| french    |
| 2       | `german\`  0x005ce748| german    |
| 3       | `spanish\` 0x005ce73c| spanish   |
| 4       | `italian\` 0x005ce750| italian   |
| default | `english\`           | english   |

Language source is `DAT_007f0f60` (per the existing note's caller `FUN_004669b0`;
not re-walked this session).

## 3. There is NO vehicle→bank map — the bank follows the racer's COLOUR

`FUN_00462dd0` (0x00462dd0–0x00462ebf; source string
`\toast\Code\src\AppCode\AUDIO\soundengine.c: CreateStream`) creates **4**
streams (`DAT_0069045c[0..3]` via `FUN_005a7f70`) and binds each to a path
buffer chosen **per racer**:

```c
piVar3 = &DAT_007f1a1c;                 // racer roster, stride 0x10
do {
    _DAT_0068fc98 = &DAT_00690158 + *piVar3 * 0x80;   // buffer[ charIndex ]
    FUN_005a8890(*puVar4, &DAT_0068fc98);
    piVar3 = piVar3 + 4;                // +0x10 bytes -> next racer
    puVar4 = puVar4 + 1;
} while ((int)piVar3 < 0x7f1a5c);        // 0x7f1a1c..0x7f1a4c = 4 racers
```

So **`DAT_007f1a1c[racer].field0` is a character/colour index 0..5** into the
6-bank table — there are exactly 4 streamed voice channels (the 4 racers). The
buffer base only has 6 entries (FUN_004625b0 builds indices 0..5), confirming
the index is the 6-colour index, **not** the 12-vehicle index. `DAT_007f1a1c`
is the per-racer roster (62 xrefs; written by the colour/car-select at e.g.
0x0042baf8 / 0x0043e575 / 0x0043f8d5).

**Conclusion for WS-J:** the voice bank is selected by the racer's **Player
Colour** (6 colours = RED/BLUEJAY/MELON/GOLD/PINK/SHADOW), an axis independent
of the 12-vehicle car selection. "6 char banks vs 12 vehicles" has no direct
edge; the correct relation is **racer-slot → colour index (DAT_007f1a1c) →
bank name (DAT_006041f0)**.

(Separate, unrelated: the `NFL<Char>` strings at 0x005cd864 — Shadow/Pink/Gold/
Melon/Bluejay/Red — are Lua *event* tokens (note neighbours `NFLSurvival/
NFLFugitive/NFLBomb/NFLCopter` = game modes), referenced once from FUN_0042fab0,
NOT the audio loader.)

## 4. Real ENGINE sounds live in permdict.rws (the 0x809 SFX bank)

`permdict.rws` (already loaded by the standalone as the SFX bank) contains the
real engine loops — wave names from its 0x803 descriptors:

`eng1` (1.09s) / `eng1d` · `eng2` (0.72s) / `eng2d` · `eng3` (0.71s) / `eng3d` ·
`eng4` (0.54s) / `eng4d`  → **4 engine classes**, each base + a `d` variant.

permdict also holds the real collision FX (`impact with other vehicle 1`,
`impact with barrier`, `impact with crate`, `impact with snowplough`,
`collision with pod`, `bullethitscar`, `carlands`, `backfire`) and a music loop
(`musicloop1`, 2.23s).

- No `engine`/`rev`/`idle` and no `eng1`..`eng4` strings appear in MASHED.exe's
  defined strings → the **vehicle→engine-class (eng1..eng4) assignment is NOT an
  exe string literal** and was not found in the vehicle `.piz` (models/textures
  only). It lives in vehicle stat/handling data not yet RE'd. **[UNCERTAIN]
  which of eng1..eng4 each of the 12 vehicles uses** — OPEN (WS-J refinement /
  WS-A vehicle-data harvest). The standalone now uses the real `eng1` for all
  cars (data-verified sound; per-vehicle class is the remaining edge).

## 5. Music streams

`FUN_00462dd0`'s 4 streams are the per-racer **dialogue** streams. The music
bank is `cdaudio.rws` (top-level 0x80D streamed, 8.4 MB — the largest bank;
cracked as continuous IMA ADPCM @44100, autocorr 0.96). `cdaudio` is built from
a data table (no `cdaudio` string literal in the exe), and the per-track offset
table inside its 0x80E header (which selects a specific music track) is **not
yet parsed** → the standalone plays the cdaudio stream as race music and uses
permdict `musicloop1` as menu music. State-edge transition discipline (one music
voice, switched on menu/race/results entry) is wired in GameFlow; exact
per-state track selection from cdaudio's 0x80E is the remaining edge.

---

## What changed in code (this session)

- `Audio::EngineStart(vol)` now loops the real permdict **`eng1`** (pitched by
  RPM), replacing the synth/voice-clip stand-in. (§4)
- `Audio::CharacterBankName/CharacterBankPath` encode the cited §2 map
  (charIndex 0..5 → RED/BLUEJAY/MELON/GOLD/PINK/SHADOW → localized `.rws` path).
  `RaceSession::Begin` resolves the player's character bank from
  `cfg.cars[0].colour` and logs the faithful path (ready for taunt playback once
  the 0x80E clip directory is parsed — deferred). (§1–§3)
- `Audio::MusicSetState(Silent/Menu/Race/Results)` — one music voice, switched
  on GameFlow state entry; menu=`musicloop1`, race=`cdaudio`. (§5)

## Open / deferred (each needs its own evidence, not a guess)

1. **0x80E sub-sound directory record format** (per-clip offset/length/rate into
   the 0x80D continuous IMA stream) → enables real per-clip character taunts AND
   per-track cdaudio music selection. ~53 B/record, count at +0x20, bank name at
   +0x50; full layout unreversed.
2. **Vehicle → engine class (eng1..eng4)** assignment — vehicle stat/handling
   data, not in the exe strings or the vehicle `.piz`.
3. Collision-FX wiring (skid/impact one-shots on real contact events) — blocked
   on WS-B (real collision events).
