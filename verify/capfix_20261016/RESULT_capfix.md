# RESULT capfix: original-side in-race capture path

**Verdict: cause found and fixed. 10 of 10 U-9702 tracks captured, 3 shots each, first attempt each.**

## Cause

`re/frida/camera_probe.py` built the shot-request path as `out_csv.parent / "orig_bbdump.req"`, which is relative
when `--out` is relative. The game is spawned with CWD = `original\`, so the agent's `new File(reqPath, 'w')`
(director onLeave, at the first shot row = race start + 30) threw `Error: No such file or directory`. The throw
came before `trace.push(row)`, so `trace.length` never advanced and every later director call threw at the same
row. The trace froze 30 racing rows in while `FUN_00446520` kept firing. The probe dropped every script message
(`scr.on("message", lambda m, d: None)`), so it read as "director no longer called". No shot fired because no
request file was ever written. d1r1 worked because its `--out` was absolute.

Not the d3d9 shim, the `--track` / `0x0067f17c` write, load, audio, the round ending, or the dupinstall2
`Fwd494fd0_DoublePlusHalf` hook: the probe sets `MASHED_RE_NO_AUTO_HOOK=1`, so no `.asi` hooks are installed.

## Evidence, one variable at a time (logs under `log/capfix/`, not committed)

| run | probe | args | racing rows | shots |
|---|---|---|---:|---|
| a1 | HEAD | d1r1 Arctic command, relative `--out` | 30 | none |
| e1 | HEAD | no shots, no drawrec | 128 (cap 1200 rows) | n/a, healthy |
| e2 | HEAD | shots, no drawrec | 30 | none (so not the D3D9 draw hooks) |
| e3 | HEAD + error print | shots, no drawrec | 30 | none, `ENOENT` at the `File()` line, 5+ times |
| b-rel | d1r1 commit `4096785a` | relative `--out` | 30 | none |
| b-abs | d1r1 commit `4096785a` | absolute `--out` | 146 | 3 |
| a3 | fixed | d1r1 Arctic command, relative `--out` | 146 (d1r1: 146) | 3, 360/355/260 draws |

(a2 timed out at the 20 s title wait on a loaded machine; `--boot-wait`, default 60 s, was added.)

## Fix (`re/frida/camera_probe.py`)

- request path `.resolve()`d to absolute
- the `File()` write is in try/catch: a failure marks the row `shot = -1` and keeps the row
- agent `error` messages are printed; the stall message says whether the director is still firing
- a run whose fired shots != requested shots prints `SHOT CAPTURE FAILED` and exits 2
- `--boot-wait` (default 60 s, was a fixed 20 s)

## Captures (`run_capture.py`, U-9702 command, `--rows 1600`)

| idx | track | piz | race start | racing rows | shot rows |
|---:|---|---|---:|---:|---|
| 1 | Egypt | EGYPT.PIZ | 710 | 146 | 740, 780, 815 |
| 2 | Neustein | NEUSTEIN.PIZ | 1064 | 144 | 1094, 1134, 1169 |
| 4 | Highway | HIGHWAY.PIZ | 823 | 198 | 853, 893, 928 |
| 5 | Sands | SANDS.PIZ | 1054 | 165 | 1084, 1124, 1159 |
| 6 | SuperG | SUPERG.PIZ | 713 | 498 | 743, 783, 818 |
| 7 | Roundabout | ROUABOUT.PIZ | 1061 | 159 | 1091, 1131, 1166 |
| 8 | Storm | STORM.PIZ | 898 | 156 | 928, 968, 1003 |
| 9 | Forest | FOREST.PIZ | 790 | 141 | 820, 860, 895 |
| 10 | Dump | DUMP.PIZ | 705 | 167 | 735, 775, 810 |
| 11 | Warzone | WARZONE.PIZ | 554 | 132 | 584, 624, 659 |

Per track dir `NN_<Track>/`: `trace.csv` (plain text), `static.json`, `orig_row_<K>.draws.json` x3,
`trace.tracks.json`, `trace.texalpha.json`, `summary.json`, `attempts.log`, `stdout.txt`. The BMPs are
gitignored (`verify/**/*.bmp`) and stay local beside them.

The probe's car-Y self-check prints FAILED on these runs as on d1r1's (the trace's `c*` columns read 0 for
car 0 / Y in this mode). It is not a gate for the draw-record use; the `F{i}_*` car frames are what the replay uses.

## Trackers

None changed. U-9702's "evidence missing" is a capture **and** the matched replay (`run_arms.py` with the track
added). This run supplies the capture half only, so U-9702 stays open for the render children's replay/scoring.
