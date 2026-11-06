"""MATCH-S16 tracker-only re-classify transaction (re-classify skill, L-68 callerless rule).

For each swapped row in swaps.tsv (minus the two GFFU-owned RVAs): file = the wrapper, exe_file = the wrapper for
exe+asi rows (kept as is for asi-only rows), notes += the match note. Confidence and status unchanged. One CHANGELOG
line per row below <!-- ENTRIES -->. Run from the worktree root: py -3.12 verify/matchs16_20261102/tools/reclass.py
"""
import csv, hashlib, io, sys

D = "verify/matchs16_20261102"
RESULT = f"{D}/RESULT_MATCH-S16.md"
DATE = "2026-11-06"
SKIP = {"0x00496900", "0x00497450"}  # bodies owned by MatchGFFU_00497450.cpp on orch/integration
OWED = {
    "0x00495080": " Owed before any promotion: a diff-original run (the gamma-ramp math now runs as VS2022 x87 port code).",
    "0x00494a80": " Second implementation: Lane2/L2_00494a80.cpp keeps an active install (follow-up MATCH-S16-FU-L2-494A80).",
}

reloc = {}
for ln in open(f"{D}/relocheck.tsv", encoding="utf-8"):
    p = ln.rstrip("\n").split("\t")
    if len(p) >= 4 and p[0].startswith("0x"):
        reloc.setdefault(p[0], (p[1], p[2], p[3], p[4] if len(p) > 4 else ""))

swaps = []
with open(f"{D}/swaps.tsv", encoding="utf-8") as f:
    for r in csv.DictReader(f, delimiter="\t"):
        if r["rva"] not in SKIP:
            swaps.append(r)
assert len(swaps) == 41, len(swaps)

raw = open("hooks.csv", "rb").read()
nl = "\r\n" if b"\r\n" in raw[:4000] else "\n"
rows = list(csv.reader(io.StringIO(raw.decode("utf-8"), newline="")))
hdr = rows[0]
ix = {k: i for i, k in enumerate(hdr)}
byrva = {r[0]: r for r in rows[1:] if r}

log = []
for s in swaps:
    k = s["rva"][2:]
    r = byrva.get(k)
    if r is None:
        sys.exit(f"no hooks.csv row for {s['rva']}")
    while len(r) < len(hdr):
        r.append("")
    st, chk, unchk, srcf = reloc[s["rva"]]
    assert st == "OK", (s["rva"], st)
    src = s["source"]
    sha = hashlib.sha256(open(src, "rb").read()).hexdigest()
    wrap = f"mashedmod/src/mashed_re/Match/{s['wrapper']}"
    tg = s["targets"]
    r[ix["file"]] = wrap
    if tg == "exe+asi":
        r[ix["exe_file"]] = wrap
    note = (f"MATCH-S16 {DATE}: match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling); "
            f"match.py {src} 100% (/O2 /GS /Ob1, XP DDK cl 13.00.9176/c2 13.00.9178); relocheck OK {chk} {unchk}; "
            f"src sha256 {sha}; port body Match/{s['wrapper']} {tg} (only body); {RESULT}" + OWED.get(s["rva"], ""))
    r[ix["notes"]] = (r[ix["notes"]] + " | " + note) if r[ix["notes"]] else note
    c = r[ix["confidence"]]
    log.append(f"{DATE}  {s['rva']}  {r[ix['name']]}  {c}={c}  notes/file only: match identical, swapped to "
               f"Match/{s['wrapper']} ({tg}), "
               f"exe-callers=0, no level from the match route (L-68 ruling); evidence: {RESULT}")

lines = raw.decode("utf-8").split(nl)
assert len(lines) == len(rows) + (1 if lines[-1] == "" else 0)
touched = {s["rva"][2:] for s in swaps}
for n, r in enumerate(rows):
    if r and r[0] in touched:
        o = io.StringIO(newline="")
        csv.writer(o, lineterminator="").writerow(r)
        lines[n] = o.getvalue()
open("hooks.csv", "wb").write(nl.join(lines).encode("utf-8"))

cl = open("re/analysis/CHANGELOG.md", "rb").read().decode("utf-8")
cnl = "\r\n" if "\r\n" in cl[:4000] else "\n"
m = "\n<!-- ENTRIES -->"
i = cl.index(m) + len(m)
j = cl.index("\n", i) + 1
cl = cl[:j] + "".join(x + cnl for x in log) + cl[j:]
open("re/analysis/CHANGELOG.md", "wb").write(cl.encode("utf-8"))
print(f"rows={len(log)}")
