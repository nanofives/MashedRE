"""re-classify transaction for seek414f00 (0x00414f00 C2 -> C3), one run, idempotent guards. Python, newline-preserving
(memory: PowerShell escapes corrupt tracker text).

  hooks.csv          00414f00 -> AiTrackObjSeek, C3, file == exe_file = Ai/AiTrackObjSeek.cpp, evidence pointers;
                     a correction note on 00452160 / 00452ea0 / 00452eb0 (their "powerup" names claim more than the reads show)
  UNCERTAINTIES.md   U-7574, U-0422, U-0423, U-0424 resolved inline; U-9713 added after U-9712
  CHANGELOG.md       one entry under <!-- ENTRIES -->
"""
import csv
import io
import sys
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
EV = "verify/seek414f00_20261016/RESULT_seek414f00.md"
EXE_LINE = sys.argv[1] if len(sys.argv) > 1 else ""


def rw(path: Path):
    raw = path.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in raw else "\n"
    return raw, nl


# ---------------------------------------------------------------- hooks.csv
hp = WT / "hooks.csv"
raw, nl = rw(hp)
rows = list(csv.reader(io.StringIO(raw, newline="")))
hdr = rows[0]
ix = {k: i for i, k in enumerate(hdr)}
FILE = "mashedmod/src/mashed_re/Ai/AiTrackObjSeek.cpp"
done = False
for r in rows[1:]:
    if not r:
        continue
    if r[ix["rva"]] == "00414f00":
        if r[ix["confidence"]] == "C3":
            raise SystemExit("00414f00 already C3")
        r[ix["name"]] = "AiTrackObjSeek"
        r[ix["confidence"]] = "C3"
        r[ix["status"]] = "impl"
        r[ix["file"]] = FILE
        r[ix["exe_file"]] = FILE
        r[ix["scenario"]] = ("run_diff ai_track_obj_seek (cache_setter_observe+fold_ret, 53 vectors: 29 natural ret=1 rows of the "
                             "original's Warzone rule-8 capture + contrived); run_verify_hook ai_track_obj_seek; live: scenario_launch "
                             "--track 11 --rule 8 --hooks 0x00414f00 --seek-probe -20000")
        r[ix["frida_diff"]] = ("verify/seek414f00_20261016/w2/diff_ai_track_obj_seek.csv;"
                               "verify/seek414f00_20261016/w2/path2_ai_track_obj_seek.txt")
        r[ix["notes"]] = (r[ix["notes"]] + " | C2->C3 seek414f00 2026-10-16: READS the course-0x21 track object 0x00684d18 "
                          "(+0xc8 = 0x00684de0, +0x94 = 0x00684dac, state machine FUN_00451d80 from per-track slot 0x005f3578) and "
                          "DAT_0088ff50[v] (FUN_00451ad0 box test); not a powerup seek and not their producer. Gate 1 is 0.5 <= h <= 30.0, "
                          "NaN passes. One body in both targets (exe: record-mirror ring row, LutSqrt, Host::car_alive; MASHED_NO_SEEK414F00 "
                          "opt-out). path1 GREEN 53/53, path2 PASS (final .asi c13db8af). Live with the JMP: H1 177 committed mode-10 frames "
                          "on car 3, all gate-consistent. U-7574 resolved (dead calls), U-9713 open (state 0->1 trigger). "
                          + EXE_LINE + " Evidence " + EV)
        done = True
    elif r[ix["rva"]] in ("00452160", "00452ea0", "00452eb0") and "seek414f00" not in r[ix["notes"]]:
        what = {"00452160": "[0x00684dac]+0x30 = the matrix translation row of the course-0x21 track object 0x00684d18 (+0x94)",
                "00452ea0": "DAT_0088ff50[v] = FUN_00451990's box test of car v against the course-0x21 track object (writer FUN_00451ad0)",
                "00452eb0": "0x00684de0 = +0xc8 of the course-0x21 track object 0x00684d18 (stepped in states 2/3 of FUN_00451d80)"}
        r[ix["notes"]] += " | seek414f00 2026-10-16: the read is " + what[r[ix["rva"]]] + "; the 'powerup' gloss is not shown by the bytes. " + EV
if not done:
    raise SystemExit("00414f00 row not found")
buf = io.StringIO(newline="")
w = csv.writer(buf, lineterminator=nl)
for r in rows:
    w.writerow(r)
out = buf.getvalue()
if raw.endswith(nl) is False and out.endswith(nl):
    out = out[: -len(nl)]
hp.write_bytes(out.encode("utf-8"))
print("hooks.csv updated")

# ---------------------------------------------------------------- UNCERTAINTIES.md
up = WT / "UNCERTAINTIES.md"
raw, nl = rw(up)
lines = raw.split(nl)
RES = {
    "U-7574": "~~data~~ **RESOLVED 2026-10-16** (seek414f00: both callees write only their out-param, a stack local of 0x00414f00: "
              "0x0046d6ec `mov [ecx],eax`; 0x0046d580 / 0x0046d647 `[edi]`; FUN_0046d570's normalize writes its own `[esp+0xc]` "
              "0x0046d5d6. No persistent writes, so the calls are dead artifacts; elided in Ai/AiTrackObjSeek.cpp. " + EV + ")",
    "U-0422": "~~[UNRECORDED]~~ **RESOLVED 2026-10-16** (seek414f00: FUN_00452eb0 = `fld [0x00684de0]` = +0xc8 of the course-0x21 "
              "track object 0x00684d18, written by its state machine FUN_00451d80 (0 in states 0/1/5, += dt*0.000666 in state 2, "
              "-= in state 3); measured 0..4.3956 on the original. _DAT_005cc72c = 30.0 is the upper gate on it. " + EV + ")",
    "U-0423": "~~[UNRECORDED]~~ **RESOLVED 2026-10-16** (seek414f00: FUN_00452ea0(v) = `[0x0088ff50 + v*4]`, written only by "
              "FUN_00451ad0 (0x00451b47 = 1, 0x00451b5b = 0) from FUN_00451990, an XZ box test of car v's matrix in the frame of "
              "the course-0x21 track object 0x00684d18; non-zero on 321 / 182 frames of two original Warzone runs. " + EV + ")",
    "U-0424": "~~[UNRECORDED]~~ **RESOLVED 2026-10-16** (seek414f00: FUN_00452160 = `[0x00684dac] + 0x30`; 0x00684dac = +0x94 of "
              "the track object 0x00684d18 = FUN_00550bc0([obj+0x50]) + 0x40 (FUN_00451ce0), so the result is that matrix's "
              "translation row, one float3, not a vehicle array. " + EV + ")",
}
n_res = 0
for i, ln in enumerate(lines):
    for uid, text in RES.items():
        if ln.startswith(f"| {uid} | ") and "RESOLVED 2026-10-16" not in ln:
            cells = ln.split(" | ")
            cells[1] = text
            lines[i] = " | ".join(cells)
            n_res += 1
NEW = ("| U-9713 | structural | 0x00451d80 / course-0x21 track object 0x00684d18 (producer of 0x0088ff50, 0x00684de0, 0x00684dac) | "
       "What moves the object from state 0 to 1 is not decoded. Measured: every observed cycle (O2 1, O3 2, H1 3) changed state "
       "0->1 INSIDE one FUN_00451d80 call (probe state_in 0, state_out 1), each time with a car's 0x0088ff50 word = 1 in that row. "
       "Case 0's callees that could write +0x48 are unread: FUN_00451820(obj, dt), FUN_00451ad0's callees FUN_0046bf50 / "
       "FUN_0046be10 / FUN_0041f1e0, and the callback 0x00452170 that FUN_00451680 installs at obj+0xc4. FUN_00452140 (a box test "
       "of the same object, six callers 0x00455c5b / 0x00457e6b / 0x0045ae34 / 0x0045c451 / 0x00486b44 / 0x00486b6d) is unread too. | "
       "decomp of FUN_00451820, 0x00452170, FUN_00451760 (init with course+0x10114 / +0x105b0) and FUN_00452140's callers | "
       "decompile the four; then capture obj+0x48/+0x4c/+0xc4 across a trigger with --seek-probe | porting the producer of "
       "0x0088ff50 / 0x00684de0 to the exe (without it 0x00414f00 and 0x00417640 cannot fire there). No C-level. |")
if not any(ln.startswith("| U-9713 |") for ln in lines):
    at = next(i for i, ln in enumerate(lines) if ln.startswith("| U-9712 |"))
    lines.insert(at + 1, NEW)
up.write_bytes(nl.join(lines).encode("utf-8"))
print("UNCERTAINTIES.md: resolved", n_res, "added U-9713")

# ---------------------------------------------------------------- CHANGELOG.md
cp = WT / "re" / "analysis" / "CHANGELOG.md"
raw, nl = rw(cp)
ENTRY = ("2026-10-16  00414f00  AiTrackObjSeek  C2->C3  One body Ai/AiTrackObjSeek.cpp (both targets). Reads the course-0x21 "
         "track object 0x00684d18 (+0xc8/+0x94) and DAT_0088ff50[v]; not a powerup seek, not their producer (FUN_00451d80 / "
         "FUN_00451ad0 are). path1 GREEN 53/53 incl. 29 natural ret=1 rows captured from the original on Warzone rule 8; path2 PASS; "
         "live H1 with the JMP: 177 mode-10 frames, all gate-consistent. U-7574/U-0422/U-0423/U-0424 resolved, U-9713 opened. "
         + EXE_LINE + " Evidence " + EV + ".")
if "00414f00  AiTrackObjSeek  C2->C3" in raw:
    raise SystemExit("changelog entry exists")
marker = "<!-- ENTRIES -->" + nl
i = raw.index(marker) + len(marker)
cp.write_bytes((raw[:i] + ENTRY + nl + raw[i:]).encode("utf-8"))
print("CHANGELOG.md entry added")
