# RESULT D4-WSJ-MUSIC: the original plays no race cdaudio, so the stand-in's cdaudio stream and its 120 s cap have no original counterpart

**Verdict: GREEN (re-scoped).** Branch `orch/D4-WSJ-MUSIC` off `orch/integration` @ `86bdd5121` (ancestry checked with `merge-base --is-ancestor`, exit 0).
PREREG `PREREG_D4-WSJ-MUSIC.md` was committed alone (`74b094b76`) before any arm.

## Answer

1. **"Per-track cdaudio selection" has nothing to select.** `cdaudio.rws` holds one sub-sound (D4-WSJ-80E, `verify/wsj80e_20261026/RESULT_D4-WSJ-80E.md`, branch `orch/D4-WSJ-80E` @ `dec04fb72`).
2. **The original never opens `cdaudio.rws`.** Static evidence below covers all modes and tracks, not only TRAINING. The dynamic capture covers TRAINING only (D4-WSJ-REF: `cdaudio.rws` never opened).
3. **The 120 s cap has no original counterpart.** It is the `maxSeconds` argument of the stand-in's `MusicStart(kCdaudio, 0.45f, 120)` (`AudioEngine.cpp`, decode cap `maxSeconds * 44100` samples). The original has no stream to cap.
4. **L-24 (a) "no race cdaudio on this mode" is confirmed**, and broader: the exe has no code path that can open it on any mode.

## Static evidence (original `MASHED.exe.unpatched`, Ghidra headless pool slot `Mashed_pool11`, `-readOnly -noanalysis`)

| claim | evidence |
|---|---|
| Only three code sites build a `toastaudio\pc\audio\pcdics\` path | string `0x005ce770` has refs at `0x004625ec` (`FUN_004625b0`) and `0x00462991` (`FUN_00462950`). String `0x005ce68c` (`...pcdics\PERMDICT.RWS`) has one ref at `0x0045d470` (`FUN_0045d460`). Suffix `.rws` at `0x005ce734`: refs `0x0046271a`, `0x00462a2a`. No other `pcdics`, `toastaudio`, `.rws` or `cdda` string has a reference |
| `FUN_004625b0` builds six paths, none is cdaudio | loop `iVar11 += 0x80` until `> 0x2ff` (6 slots into `DAT_00690158`), name from `DAT_006041f0 + iVar11` (decomp lines 105-135). Row 0 at `0x006041f0` is `RED`, then BLUEJAY, MELON, GOLD, PINK, SHADOW (`0x00604270`..`0x00604470`) |
| `FUN_00462950` builds the course bank | path = `pcdics\` + `FUN_0041e980(track)` + `.rws` (decomp lines 42-95), table `PTR 0x0060c820[track]` (`DAT_0088e670`, line 93) |
| The `CDAUDIO` string has no reader | `CDAUDIO\0` at `0x00604170`, one `0x80` row before `RED`. A scan of every dword in the image for values `0x00603000..0x00604500` finds no reference to `0x00604170`, `0x006040f0`, `0x00604070` or `0x00603ff0`. The only hits in that range are `0x006041f0` (`0x004626d4`), `0x006044e8`, `0x006044f0` (`0x0045d548`) and data constants `0x006036b8`..`0x006037e8`, `0x00603868`, `0x00603c24` (`0x005bad9b`, inside `FUN_005bad30`, DirectSound init), `0x006040c7` (`0x00456f1b`, inside `FUN_00456eb0`, particle spawn). The two odd ones are false positives (decomp in `d3.txt`) |
| The race tick starts no music voice | `FUN_00466b50` dispatch case 3/7 (race, decomp lines 86-169) calls `FUN_0045db50(3)` and ends with `FUN_004647f0`, `FUN_004631f0`, `FUN_004623e0`, `FUN_0045dd60` (lines 238-241). No call starts a stream |
| What the race tick does to the menu loop | `FUN_0045db50(3)`: `_DAT_0068fc8c -= _DAT_005cd0ec` (`0x005cd0ec` = `0x3ba3d70a` = 0.005, line `case 3`). `FUN_004623e0`: `DAT_0068fc94 = _DAT_006900d4 * _DAT_0068fc8c`, and when that is `<= DAT_005d757c` (`0x005d757c` = 0.0) it calls `FUN_005a66d0(voice, 0)` on voice `DAT_00605db8` (stop). That voice is the permdict `musicloop1` row (`0x00605d38`, voice 138 in D4-WSJ-REF). So the race path fades the menu loop to 0 and stops it, and starts nothing |
| Per-track loops that do exist are course-table rows | `wave_tables.tsv` (80E): `musicloop1` / `musicloop2` at `0x0060c550` / `0x0060c5e0` in the city course table `0x60bf20` (track id 26), field `+0x8c` = 70.0. No other track has a music-named row |

## Dynamic evidence (carried from D4-WSJ-REF, not re-captured)

D4-WSJ-REF: `cdaudio.rws` never opened in `o_train1`, `o_train2`, `o_pu1` (TRAINING, track 0, mode 10). `musicloop1` started in phases 0 and 2, then stopped every race frame from `0x00462415` (5560 / 2721 / 6472 calls).

## Port change (default OFF)

`mashedmod/src/mashed_re/Audio/AudioEngine.cpp`, `MusicSetState`: knob `MASHED_MUSIC_NO_RACE_CDAUDIO=1`. ON: Race and Results call `MusicStop()` and start no `cdaudio.rws` stream. OFF: unchanged code path.

### Arms (standalone `mashed_re.exe`, sha256 prefix `40cdf2dc`, menu path of `run_port.py`, 80 s wall, `MASHED_MUTE=1`, `MASHED_AUDIO_EVENT_LOG`, under `mashed_lock.py run`, explicit PIDs 7264 / 18148)

| arm | race entry rows | result |
|---|---|---|
| A (knob OFF) | `music_state` val 2 at 2474.5 ms, then `music_start` `cdaudio.rws` maxSeconds 120 at 2474.6 ms | stream starts, as the PREREG predicted |
| B (knob ON) | `music_state` val 2 at 3380.1 ms, no `music_start` | no stream, as predicted |

Both arms return to `music_state` 1 plus `music_bankloop musicloop1` at the end (A 32665.2 ms, B 33664.3 ms). Event logs: `A_off.audio_events.csv`, `B_on.audio_events.csv` (this directory).

## What WIRE must do about music

1. Remove the `cdaudio.rws` race stream and its 120 s cap: `MusicSetState(Race)` becomes `MusicStop()` only (what the knob does). The default flip is a decision row, not made here.
2. Results: the stand-in's ducked `cdaudio` stream has no original counterpart either. Under the knob it starts nothing.
3. Menu loop in the race: the original fades `musicloop1` down by 0.005 per `FUN_00466b50` call in state 3/7 (`0x0045db50` case 3) and stops the voice when the product reaches 0. The knob stops it at race entry instead. Whether to model the fade is an open choice, see below.
4. `MusicStart` and `RwsStreamDecode` stay for the character-bank work (D4-WSJ-80E clip table). `cdaudio.rws` itself has no consumer in the original and none is needed in the port.

## [UNCERTAIN] items

- Fade length in wall time: `FUN_00466b50` call rate in the race was not measured here. The fade is `1.0 / 0.005 = 200` calls from full scale, but `_DAT_0068fc8c` starts wherever the menu/countdown phase left it. Evidence needed: a per-call trace of `0x0045db50` across the countdown, or the `FUN_00466b50` tick rate from the REF captures.
- Which phase value is the Results screen: D4-WSJ-REF says `musicloop1` starts in phases 0 and 2. Dispatch cases 0, 2, 4, 5, 6 of `FUN_00466b50` were read for volume steps only (`FUN_0045db50` / `FUN_0045dbe0` arguments), not mapped to screens. G7 (win/lose) stays open.
- Only TRAINING was captured dynamically. The static argument (no reader of `0x00604170`, three path builders) is what extends the claim to all modes.
- `FUN_0045dbe0` drives a second channel (`FUN_00493f70(0)` / `FUN_004943f0`, volume `_DAT_006900d4 * _DAT_0068fcbc`). Its target was not read.

## Gates

Not run. The change is one default-OFF audio branch (`GetEnvironmentVariableA` read once, no sim state). With the knob unset the code path is the pre-change one and no sim, physics or AI state is touched, so the `gctrl.ref` / `b.ref` values in `merge_train.json` are not in the diff's reach. The merge train's own gate run covers it.

## Raw data (not committed), SHA-256

Root `D:/MashedWT/D4-WSJ-MUSIC_data/` (decompiles from `re/tools/decomp_pc.py` run in the main checkout, output only):

| file | content | SHA-256 |
|---|---|---|
| d1.txt | `FUN_004625b0`, `FUN_004623e0`, `FUN_00462500` | 54730BA4DE13E2D33E7EDF077EFBCE13C7D871968BC7D2E2197740FD85AA976C |
| d2.txt | `FUN_004669b0`, `FUN_0045dd60`, `FUN_004647f0` | B4B3DB60D25017170CA9AD208F501DEFDA20AFEB47FE3DEA012BC634DD95847F |
| d3.txt | `FUN_00456eb0`, `FUN_005bad30` (false positives) | 0760BDFE08FEE2579A9364A12EFA8BFD46434881ABA7F537F3C014ABA36788D6 |
| d4.txt | `FUN_00462950`, `FUN_004623e0` + callers | 81D6CCC06B0F8404F59B1E118289896E94C17461CCCB5874F23FEDB2058AE8A1 |
| d5.txt | `FUN_00466b50`, `FUN_0045e2a0`, `FUN_0045e160`, `FUN_0045de80` | BC8E8E9664D0BAD795FEB1B79C403C937E490F71C5E2222B6B38060AB071F158 |
| d6.txt | `FUN_0045db50`, `FUN_0045dbe0`, `FUN_0045df70` | 78378E66525C9C5900A3D069A994FA13F1411BDD2FDCAC86A042FC0743CDC01B |
| port/A_off.audio_events.csv | arm A | 7FEB3ED19AEE79D1A9FAADC5FE5E7A195E1AD9FB7C7AAB7DD6E8E02D2A3BAF4A |
| port/B_on.audio_events.csv | arm B | D18A775F72EF2903F9F3F89A28C928D7E51FE82FCDB84B40699CCF65048F3641 |

Private root `D:/MashedWT/D4-WSJ-MUSIC_root` (robocopy of `original/`). Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, `Build OK`, no deploy.

## Committed

`PREREG_D4-WSJ-MUSIC.md`, this RESULT, `run_port.py` (copy of the REF runner, `ARM_KNOB`), the two event logs, and the `AudioEngine.cpp` knob.
