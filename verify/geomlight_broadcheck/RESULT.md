# Class-scoped fold — broad-track check (2026-08-31, branch race/arctic-cap)

Closes the PROTOTYPE.md caveat ("other cup tracks not checked") for the two remaining
Bronze-Cup-1 tracks. **Result: the scoped fold (`MASHED_LIBRW_AMBFOLD_SEA=1`) is a
byte-identical NO-OP on EGYPT and NEUSTEIN — it changes nothing there, so it cannot
over-brighten them. The scope key `numTexCoordSets<=1` does not over-catch their terrain.**

## Method

Pose-matched originals captured (`race_draw_burst.py --challenge {1,2} --settle 8`, wrapped
in `run_with_unlocked_save.py`); standalone (`race/geomlight` prototype build) run with the
transplanted basis in geomON (current default) and geomSEA (scoped). `MASHED_TRACK_SEL`:
EGYPT=1, NEUSTEIN=5 (`kAreas[]`).

## Result — geomON vs geomSEA (what the scoped fold changes on each track)

| track | scoped fold changes | verdict |
|---|---|---|
| EGYPT (challenge 1) | 0.00% (mean abs 0.000) | NO-OP — no water-class geom in view |
| NEUSTEIN (challenge 2) | 0.00% (mean abs 0.000) | NO-OP — no water-class geom in view |
| Arctic (challenge 3) | 68.7% / 56.5% (from PROTOTYPE.md) | folds the sea, matches original |

Both EGYPT and NEUSTEIN frames carry substantial track geometry — EGYPT's sandy canyon,
pillars and dirt road; NEUSTEIN's snow road, bridge and mountains (see
`egypt/`, `neustein/`) — and all of it already renders matching the original under geomON.
The scoped fold leaves every pixel untouched, so none of that terrain is the water class
(`0x1000f`, numTexCoordSets<=1) that the scope folds. The scope correctly targets only the
Arctic sea.

## Verdict

The class-scoped fold is safe on the two additional cup tracks: it is identical to the
current geomlight default there (which already matches the original). Combined with
PROTOTYPE.md (TRAINING win kept byte-identical; Arctic sea fixed), the fix is validated on
4 tracks: TRAINING (road, win kept), Arctic (sea, fixed), EGYPT + NEUSTEIN (no-op, safe).

## [UNCERTAIN] / remaining scope

- Validated on the 4 Bronze-Cup-1 tracks only (reachable via the col1 span unlock). The
  other 9 `kAreas[]` tracks (City, Forest, Highway, Storm, SuperG, Warzone, Roundabout,
  Sands, Dump) are NOT checked — they need a wider save unlock to reach on the original
  side. Any of them with a water body (`0x1000f`) would be folded by the scope; if one has
  a non-lit prelit `numTexCoordSets<=1` surface that the original renders DARK, the scope
  would over-brighten it. Before shipping broadly, the parent should run the parity harness
  across those tracks (or at least any known to have water).
- Single-frame-per-track: a different section of EGYPT/NEUSTEIN could contain water not in
  these views. The no-op holds for the captured vantages.
