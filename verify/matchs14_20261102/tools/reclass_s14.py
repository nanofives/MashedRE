"""MATCH-S14 re-classify transaction (re-classify skill, route match). Run from the worktree.
Edits only the hooks.csv lines of the 38 swapped RVAs and prepends one CHANGELOG line per row below <!-- ENTRIES -->."""
import csv, io, sys

DATE = "2026-11-06"
RES = "verify/matchs14_20261102/RESULT_MATCH-S14.md"
TC = "XP DDK cl 13.00.9176 / c2 13.00.9178 /O2 /GS /Ob1 C"
C4 = {"0x00484c70", "0x00484c90", "0x00484cf0"}
HELD = {"0x00485070"}
EXE_RAN = {"0x00484c70": "WorldObjectsBaseGet <- TG_414c30 (AiTargetClosest Body7)",
           "0x00484c90": "Zero484c90 <- Race::Registry_RaceInit <- TrackRenderer::Load <- WinMain",
           "0x00484cf0": "RG_484cf0 <- Registry_Register / CarRegistrant_naked / PickupTick_naked",
           "0x00485070": "RG_485070 <- Race::Registry_FrameTick"}

sw = {r["rva"]: r for r in csv.DictReader(open("verify/matchs14_20261102/swaps.tsv"), delimiter="\t")}
ev = {r["rva"]: r for r in csv.DictReader(open("verify/matchs14_20261102/c4_evidence.tsv"), delimiter="\t")}

raw = open("hooks.csv", "rb").read().decode("utf-8")
lines = raw.split("\r\n")
hdr = next(csv.reader([lines[0]]))
ix = {k: i for i, k in enumerate(hdr)}
log = []
done = set()
for n, ln in enumerate(lines):
    key = ln.split(",", 1)[0]
    a = "0x" + key
    if a not in sw or not ln:
        continue
    row = next(csv.reader([ln]))
    s, e = sw[a], ev[a]
    wrapper = "mashedmod/src/mashed_re/Match/" + s["wrapper"]
    old = row[ix["confidence"]]
    rc = e["relocheck"].split(" ")
    rcs = f"relocheck OK {rc[2]} {rc[3]}"
    base = (f"match.py {e['source']} --bytes {e['match']} BYTE-IDENTICAL ({TC}); {rcs}; src sha256 {e['sha256']}; "
            f"port body {s['wrapper']} {s['targets']} (only body, lint NEW=0); gates unchanged G-CTRL 8284be46/ba407a16/31fe043e "
            f"B 1d5bf4f9 x3; {RES}")
    if a in C4:
        new = "C4"
        note = (f"{DATE} MATCH-S14 C4 by route match: c4=match; {base}; exe body executes in the default H_DEF/B_DEF_K0 arms "
                f"(uncommitted counter build 5827b640, no replay env): {EXE_RAN[a]}")
        why = "route match: identical + relocheck OK, only port body, exe body runs in default arms, gates unchanged"
    elif a in HELD:
        new = old
        note = (f"{DATE} MATCH-S14: level stays {old}, NOT C4. match evidence: identical, swapped into the exe "
                f"(exe body runs in default arms: {EXE_RAN[a]}); {base}; C4 blocked by rule (2): the .asi still hooks "
                f"0x00485070 with a different body (Lane2/L2_00485070.cpp L2_FUN_00485070). C4 when the .asi install "
                f"uses the matched source or is dropped")
        why = "no change: matched + swapped in exe, .asi hooks the RVA with a different body (Lane2/L2_00485070.cpp)"
    else:
        new = old
        tail = "C4 stays on the frida route" if old == "C4" else f"level stays {old}, NOT C4"
        note = (f"{DATE} MATCH-S14 (L-68): {tail}. match evidence: identical, swapped, exe-callers=0; C4 when a caller "
                f"lands (L-68 ruling); {base}")
        why = ("notes/file only: match evidence added (identical, swapped, exe-callers=0), C4 stays on the frida route"
               if old == "C4" else "no change (L-68): match evidence: identical, swapped, exe-callers=0")
    row[ix["confidence"]] = new
    row[ix["file"]] = wrapper
    if "exe_file" in ix and len(row) > ix["exe_file"]:
        row[ix["exe_file"]] = ""   # the wrapper in `file` is the only body (RC precedent: file = wrapper, exe_file cleared)
    nt = row[ix["notes"]]
    row[ix["notes"]] = (nt + " | " if nt else "") + note
    buf = io.StringIO()
    csv.writer(buf, lineterminator="").writerow(row)
    lines[n] = buf.getvalue()
    done.add(a)
    log.append(f"{DATE}  {a}  {row[ix['name']]}  {old}->{new}  {why}; evidence: {RES}")

missing = set(sw) - done
if missing:
    sys.exit(f"no hooks.csv row for {sorted(missing)}")
open("hooks.csv", "wb").write("\r\n".join(lines).encode("utf-8"))

cl = open("re/analysis/CHANGELOG.md", "rb").read().decode("utf-8")
i = cl.index("<!-- ENTRIES -->")
j = cl.index("\n", i) + 1
eol = "\r\n" if cl[j - 2:j] == "\r\n" else "\n"
cl = cl[:j] + "".join(l + eol for l in log) + cl[j:]
open("re/analysis/CHANGELOG.md", "wb").write(cl.encode("utf-8"))
print(f"{len(done)} rows, {sum(1 for a in done if a in C4)} C4")
