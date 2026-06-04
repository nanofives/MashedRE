"""Close the 8 non-library C1 stragglers (2026-06-04 straggler-mopup).

After the gameplay + render C1->C2 campaigns closed, exactly 8 non-library rows
remained at C1. Each was read from live decomp (Mashed_pool1, read-only) this pass:

PROMOTE C1->C2 (7 — genuine game code, fully mechanically mapped):
  00410510  Race::EvaluateResult [save]            820B race-end evaluator, game-state globals
  00443090  FUN_00443090       [unknown]           trivial getter `return &DAT_00897ff0`
  004495d0  FUN_004495d0       [unknown]           trivial getter `*(DAT_00896278+4)+0x40`
  004a1790  FUN_004a1790       [util]              COM Release thunk `if(*p) (*p)->vtbl[2](*p)`
  005ab040  FUN_005ab040       [util]              QPC/timeGetTime dispatcher on DAT_007dce00
  00496e40  FUN_00496e40       [boot]              dual RW-pipeline install glue (atomic + world-sector)
  0043dfd0  FUN_0043dfd0       [util -> frontend]  10969B Frontend/menu per-frame tick; full decomp
                                                   read end-to-end (subagent); resolves D-3282/D-4721/U-1136

RECLASS-OUT (1 — vendored, kept C1):
  005507b0  VFS_Open           [io -> third-party-library[renderware]]  RW RtFS RwFopen-shape open:
            only RW engine fn-table (DAT_007d3ff8 +0xf0/+0xf4) + RW VFS module globals
            (DAT_007dc754 FS-handler list / 768 err-cb / 76c default / 75c enable) + CRT; ZERO game
            state. Dispatch target FUN_00550580 + neighbours were reclassed renderware by ar_s5.

Subsystem reclass among promotions:
  0043dfd0 util -> frontend (evidence: cursor clamps, menu page-transition switches, opening/closing
           fade state machine, dialog/string-ID mailbox calls; 146 game-state globals 0x0067/007f/0089).

00496e40's prior D-10770 deferral ("7/7 callees unmapped") was over-conservative: unmapped callees are
STUBS, which do not block C2 of the body; the body is fully mechanically mapped.

Run: py -3.12 scripts/reclassify_stragglers.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DATE = "2026-06-04"
SESS = "straggler-mopup-2026-06-04"
APPLY = "--apply" in sys.argv

# rva -> (new_subsystem_or_None, plate_file)
PROMOTE = {
    "00410510": (None, "re/analysis/profile_career_d2/FUN_00410510.md"),
    "00443090": (None, "re/analysis/unknown_getters/0x00443090.md"),
    "004495d0": (None, "re/analysis/unknown_getters/0x004495d0.md"),
    "004a1790": (None, "re/analysis/video_mci/0x004a1790.md"),
    "005ab040": (None, "re/analysis/timer/0x005ab040.md"),
    "00496e40": (None, "re/analysis/boot_subsystem_d3/0x00496e40.md"),
    "0043dfd0": ("frontend", "re/analysis/timer_d2/0x0043dfd0.md"),
}
PROMOTE_NOTE = {
    "00410510": " | C1->C2 straggler-mopup: full 820B race-end evaluator decomp (game-state globals DAT_0063b90c/007f0fcc, switch DAT_007f0fd0); genuine game/save code",
    "00443090": " | C1->C2 straggler-mopup: trivial getter `return &DAT_00897ff0` (mechanically complete; subsystem left unknown pending caller FUN_004847d0 analysis)",
    "004495d0": " | C1->C2 straggler-mopup: trivial getter `*(DAT_00896278+4)+0x40` (mechanically complete; subsystem left unknown pending caller FUN_00460350 analysis)",
    "004a1790": " | C1->C2 straggler-mopup: COM Release thunk `p=*p; if(p) (*p)->vtbl[2](p)`; mechanically complete",
    "005ab040": " | C1->C2 straggler-mopup: timer dispatcher `DAT_007dce00==1 ? QPC.LowPart : timeGetTime()`; mechanically complete",
    "00496e40": " | C1->C2 straggler-mopup: dual RW-pipeline install glue (atomic-all-in-one + world-sector-all-in-one), wires .bss handles DAT_00773084/88/8c/94 + callbacks LAB_00496e00/e20; body fully mapped, 7 callees are stubs (prior D-10770 over-conservative - callee-unmapped does not block body C2)",
    "0043dfd0": " | C1->C2 straggler-mopup: util->frontend; 10969B Frontend/menu per-frame tick, full 64KB decomp read end-to-end via subagent (decompile_completed=true, no recovery gaps); 146 game-state globals (0x0067/007f/0089), 60 game callees, ZERO RW-engine globals; resolves D-3282/D-4721/U-1136, residual narrow hole U-8911",
}
RECLASS_OUT = {
    "005507b0": ("third-party-library[renderware]",
                 " | reclass-OUT straggler-mopup io->third-party-library[renderware]: RW RtFS RwFopen-shape open dispatcher; only RW engine fn-table (DAT_007d3ff8 +0xf0/+0xf4) + RW VFS module globals (DAT_007dc754/768/76c/75c) + CRT, ZERO game state; dispatch target FUN_00550580 + neighbours reclassed renderware by ar_s5; kept-C1 (vendored, not a reimpl target)"),
}

hooks_path = ROOT / "hooks.csv"
hk = hooks_path.read_text(encoding="utf-8").split("\n")
def parse(line): return next(csv.reader([line]))
def serialize(fields):
    sio = io.StringIO(); csv.writer(sio, lineterminator="").writerow(fields); return sio.getvalue()
idx = {}
for i, ln in enumerate(hk):
    if not ln or ln.startswith("#") or ln.startswith("rva,"): continue
    f0 = ln.split(",", 1)[0].strip().lower()
    if f0: idx.setdefault(f0, i)

edits = {"promote": 0, "reclass_out": 0, "subsys_reclass": 0, "drift": []}

for rva, (newsub, plate) in PROMOTE.items():
    i = idx.get(rva)
    if i is None: sys.exit(f"FATAL {rva} not in hooks.csv")
    f = parse(hk[i])
    if f[3] != "C1": edits["drift"].append(f"{rva}:conf={f[3]}")
    f[3] = "C2"
    if f[4] in ("new", "", "unmapped", "C1"): f[4] = "mapped"
    f[5] = plate
    while len(f) < 9: f.append("")
    if newsub and f[2] != newsub:
        edits["subsys_reclass"] += 1
        f[2] = newsub
    f[8] = (f[8] or "") + PROMOTE_NOTE[rva]
    hk[i] = serialize(f); edits["promote"] += 1

for rva, (newsub, note) in RECLASS_OUT.items():
    i = idx.get(rva)
    if i is None: sys.exit(f"FATAL {rva} not in hooks.csv")
    f = parse(hk[i])
    if f[3] != "C1": edits["drift"].append(f"{rva}:conf={f[3]} (reclass-out)")
    f[2] = newsub                      # kept C1
    while len(f) < 9: f.append("")
    f[8] = (f[8] or "") + note
    hk[i] = serialize(f); edits["reclass_out"] += 1

clog = (f"{DATE}  re-classify straggler-mopup  {SESS}: closed the 8 non-library C1 stragglers. "
    f"7 C1->C2 (00410510 save race-eval; 00443090/004495d0 unknown getters; 004a1790 util COM-release; "
    f"005ab040 util timer; 00496e40 boot RW-pipeline-install; 0043dfd0 util->frontend 10969B menu tick "
    f"via full subagent decomp read -> resolves D-3282/D-4721/U-1136, mints U-8911). "
    f"1 reclass-OUT (005507b0 io->third-party-library[renderware] RW RtFS open, kept-C1). "
    f"Read live from Mashed_pool1 read-only. NON-LIBRARY C1 NOW = 0 (all remaining C1 is "
    f"third-party-library residue kept by design). C2 4389->4396.")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"edits: promote={edits['promote']} reclass_out={edits['reclass_out']} subsys_reclass={edits['subsys_reclass']}")
if edits["drift"]: print(f"DRIFT: {edits['drift']}")
if APPLY:
    hooks_path.write_text("\n".join(hk), encoding="utf-8")
    c = (ROOT / "re/analysis/CHANGELOG.md").read_text(encoding="utf-8")
    if not c.endswith("\n"): c += "\n"
    (ROOT / "re/analysis/CHANGELOG.md").write_text(c + clog + "\n", encoding="utf-8")
    print("\nWROTE hooks.csv, CHANGELOG.md")
else:
    print("\nDRY RUN — nothing written.")
