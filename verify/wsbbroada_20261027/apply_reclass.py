# D4-WSB-C4-BROAD-A re-classify transaction (C3 -> C4) for the rows named on the command line, plus the U-9026
# resolution when 0055deb0 is among them. Edits only those hooks.csv rows (csv round-trip of the line, every other
# line byte-identical), the U-9026 row of UNCERTAINTIES.md, and inserts one CHANGELOG line per row right below
# <!-- ENTRIES -->. Refuses if a row is not C3, a marker is missing, or a row has no evidence entry below.
# Run once from the worktree root:  py -3.12 verify/wsbbroada_20261027/apply_reclass.py 0055deb0 0055bb70 ...
import csv, io, sys
from pathlib import Path

R = "verify/wsbbroada_20261027/RESULT_D4-WSB-C4-BROAD-A.md"
S = "verify/wsbbroada_20261027/racediff_summary.tsv"
D = "2026-10-27"
EV = {
    "0055deb0": ("c4_racediff:quickbattle-a4-300-1200-regs", "Quick Battle racediff C4-CLEAN, tool v8 (own body "
                 "_RwpWorldSolverHandle behind the naked shim RwpWorldSolverHandle_RegAbi, b5cSelfTestEnabled transparent, "
                 "ECX/EDX leg): 6/6 calls, 1 sig each side 'eax=h0 ecx=kept edx=kept', ON byte e9 (shim INSTALLED). "
                 "EFLAGS: U-9026 resolved statically, all 22 references are direct calls and every one rewrites the six "
                 "status flags before any read (scan_flags_after_call.txt)"),
    "0055bb70": ("c4_racediff:census-track0-mode10-cars4-hold400-a4-300-30000", "census racediff C4-CLEAN, tool v8, "
                 "C4_HOLD=400, A4 calls 300-30000 (the row first runs late in the race; NOT-EXEC in the 45 s census and in "
                 "Quick Battle up to A4 call ~5800): 6/6 calls, 1 sig each side, ON byte e9"),
    "0055e050": ("c4_racediff:quickbattle-a4-300-1200-deref", "Quick Battle racediff C4-CLEAN, tool v9 with field-source "
                 "tokens (C4_DEREF): 6/6 calls, 1 sig each side, param_1[9] named A0@24 on both sides, ON byte e9. The v8 "
                 "run's BEHAVIOR-DIFF was the heap distance param_1 -> param_1[9], which also differs between two original "
                 "boots (X1 CONTROL-DIFF); with field-source tokens two original boots agree (X2 CONTROL-SAME)"),
    "0055abb0": ("c4_racediff:census-track0-mode10-cars4-hold400-a4-300-30000-argw", "census racediff C4-CLEAN, tool v10, "
                 "C4_HOLD=400, A4 calls 300-30000, declared-width mask 0xffff on arg 1 of the 0x00565200 call-out "
                 "(original pushes EDX after mov dx at 0x0055abb7, callee masks at 0x00565208): 6/6 calls, ON byte e9"),
    "0055c230": ("c4_racediff:census-track0-mode10-cars4-hold400-a4-300-30000", "census racediff C4-CLEAN, tool v10, "
                 "C4_HOLD=400, A4 calls 300-30000, static-init helpers transparent (Z2): 6/6 calls, 2 sigs each side, ON byte e9; "
                 "OFF-vs-OFF control Z1 CONTROL-SAME. Caveat: 1 of 4 original boots in this window gave a different indirect-"
                 "target set (the v8 OFF boot), so the window is not fully reproducible on the original; the body part of "
                 "every signature is identical on all 6 boots. The 3 floats passed by pointer to the [edx+0x18] callee are "
                 "not compared by the racediff"),
}
rows = [a.lower().replace("0x", "").zfill(8) for a in sys.argv[1:]]
if not rows or any(r not in EV for r in rows):
    sys.exit(f"refuse: rows must be among {sorted(EV)}")

hp = Path("hooks.csv")
lines = hp.read_bytes().decode("utf-8").split("\r\n")
names = {}
for i, ln in enumerate(lines):
    k = ln.split(",", 1)[0]
    if k not in rows:
        continue
    r = next(csv.reader([ln]))
    if r[3] != "C3":
        sys.exit(f"refuse: {k} is {r[3]}, not C3")
    sc, ev = EV[k]
    r[3], r[4], r[6], r[7] = "C4", "verified", sc, S
    r[8] += (f" || C4 {D} (D4-WSB-C4-BROAD-A): {ev}. Out-memory leg A: no store outside the own frame "
             f"(verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md BROAD table). Hook installed alone, inline JMP live. "
             f"No stub in the body. {R}")
    b = io.StringIO(); csv.writer(b, lineterminator="").writerow(r)
    lines[i] = b.getvalue(); names[k] = r[1]
missing = [r for r in rows if r not in names]
if missing:
    sys.exit(f"refuse: rows missing {missing}")

if "0055deb0" in rows:
    up = Path("UNCERTAINTIES.md")
    u = up.read_bytes().decode("utf-8")
    nlu = "\r\n" if "\r\n" in u else "\n"
    ul = u.split(nlu)
    hit = [j for j, x in enumerate(ul) if x.startswith("| U-9026 | cosmetic |")]
    if len(hit) != 1:
        sys.exit("refuse: U-9026 open row not found exactly once")
    j = hit[0]
    x = ul[j].replace("| U-9026 | cosmetic |", f"| U-9026 | ~~cosmetic~~ **RESOLVED {D}** (D4-WSB-C4-BROAD-A: no caller "
                      "reads flags across the call) |", 1)
    assert x.endswith(" |")
    x = x[:-2] + (f" **RESOLVED {D} (D4-WSB-C4-BROAD-A, {R}):** all 22 references to 0x0055deb0 in the anchored image "
                  "are direct calls (no dword reference); at every one all six status flags are rewritten before any "
                  "instruction tests one, following direct callees with a return stack "
                  "(verify/wsbbroada_20261027/scan_flags_after_call.py / .txt). No PUSHFD/POPFD needed in the shim. |")
    ul[j] = x
    up.write_bytes(nlu.join(ul).encode("utf-8"))

hp.write_bytes("\r\n".join(lines).encode("utf-8"))

cp = Path("re/analysis/CHANGELOG.md")
t = cp.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in t else "\n"
m = "<!-- ENTRIES -->" + nl
if m not in t:
    sys.exit("refuse: no ENTRIES marker")
add = ""
for k in rows:
    add += f"{D}  {k}  {names[k]}  C3->C4  D4-WSB-C4-BROAD-A: {EV[k][0]} C4-CLEAN, hook installed alone, leg A no out-of-frame store; {R}{nl}"
if "0055deb0" in rows:
    add += f"{D}  0055deb0  U-9026  RESOLVED  D4-WSB-C4-BROAD-A: 22/22 direct callers rewrite all six flags before any read; {R}{nl}"
cp.write_bytes(t.replace(m, m + add, 1).encode("utf-8"))
print("promoted:", ", ".join(f"{k} {names[k]}" for k in rows))
