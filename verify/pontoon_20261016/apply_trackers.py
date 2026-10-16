"""re-classify transaction for pontoon (no confidence change). Python, newline-preserving (memory: PowerShell escapes corrupt
tracker text). Idempotent guards: refuses to run twice.

  hooks.csv          existing rows of the ported RVAs: exe_file -> Track/WarzonePontoon.cpp (where that TU holds the exe body)
                     and a [pontoon] note; new C1 rows for the object's uncatalogued RVAs
  UNCERTAINTIES.md   U-9719 RESOLVED inline; U-9738..U-9742 added at the top of the table
  CHANGELOG.md       one entry under <!-- ENTRIES -->
"""
import csv
import io
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
EV = "verify/pontoon_20261016/RESULT_pontoon.md"
TU = "mashedmod/src/mashed_re/Track/WarzonePontoon.cpp"
DATE = "2026-10-16"
NOTE = (f"[pontoon {DATE}] exe body in {TU} (MASHED_PONTOON_ANIM/_STATE/_CARRY, default OFF); replay of the original's "
        f"per-call capture bit-identical (OP3 1073/1073 calls, OP1/OP2/OP4 38/38), {EV}. Exe-only, no RH_ScopedInstall: "
        f"C-level unchanged.")


def rw(path: Path):
    raw = path.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in raw else "\n"
    return raw, nl


# ---------------------------------------------------------------- hooks.csv
hp = WT / "hooks.csv"
raw, nl = rw(hp)
if "[pontoon " in raw:
    raise SystemExit("hooks.csv already carries the pontoon transaction")
rows = list(csv.reader(io.StringIO(raw, newline="")))
hdr = rows[0]
ix = {k: i for i, k in enumerate(hdr)}
EXE_TU = {   # existing rows whose exe body is now WarzonePontoon.cpp (the TU's own function in brackets)
    "00451820": "Trigger", "00451990": "BoxCarry", "00451ad0": "Coupling", "00451ce0": "Sample",
    "00451cc0": "Deactivate", "0052de60": "SetCurrentAnim", "0052e0e0": "AddAnimTime", "0052df70": "SubAnimTime",
    "0052e2c0": "SetCurrentTime", "00533ec0": "UpdateMatrices", "00532a60": "KfToMatrix",
}
NOTE_ONLY = {   # a second or partial exe body, or a field write only: note, exe_file unchanged
    "00532b80": "KfInterpolate is a second exe body next to TrackRenderer.cpp LinearAnimMatrix (dot order reversed there); "
                "allowlisted, U-9741",
    "004c4680": "OrthoNormalize is a second exe body next to Vehicle/BodyOrientationIntegrate.cpp (not a transcription, D2); "
                "allowlisted, U-9741",
    "00451730": "box = RwBBox of stream atomic 1's geometry, transcribed inline in ParseBoat (FUN_004c1740 order: start "
                "from vertex 0, strict > / <)",
    "0046bf50": "only the record branch (+0x14 == 0: +0x9b0..b8 += movement) is in Coupling; the RWP branch FUN_00481780 is "
                "not portable (U-9738)",
    "0046be10": "Coupling / Trigger write car marker slot 0 (+0xbfc = 0xff804040, +0xc7c = on) as FUN_0046be10 does; "
                "the reader FUN_0047d640 is the RWP contact filter, no exe consumer (U-9739)",
    "00452140": "the same box test as FUN_00451990 on the 0x00684d18 object; its six callers are not ported (U-9742)",
    "004516d0": "atomic slots of the 0x00684d18 object: list order = DFF stream atomics 2, 1, 0 (captured LTMs)",
}
seen = set()
for r in rows[1:]:
    if not r:
        continue
    rva = r[ix["rva"]]
    if rva in EXE_TU:
        r[ix["exe_file"]] = TU
        r[ix["notes"]] = (r[ix["notes"]] + " | " if r[ix["notes"]] else "") + NOTE + f" Exe function: {EXE_TU[rva]}."
        seen.add(rva)
    elif rva in NOTE_ONLY:
        r[ix["notes"]] = (r[ix["notes"]] + " | " if r[ix["notes"]] else "") + f"[pontoon {DATE}] {NOTE_ONLY[rva]}. {EV}"
        seen.add(rva)
missing = (set(EXE_TU) | set(NOTE_ONLY)) - seen
if missing:
    raise SystemExit(f"hooks.csv rows missing: {sorted(missing)}")
NEW = [   # rva, name, subsystem, notes (C1: name, xrefs and subsystem; decode in the PREREG, exe body in the TU)
    ("00451760", "WarzonePontoonInit", "gameplay", "FUN_00451760(obj, clump, anim): object init (hierarchy, loop cb 0x00451690, atomic slots, box, node-1 sample); one caller slot 0 0x004527f0"),
    ("00451680", "WarzonePontoonSetResetCb", "gameplay", "obj+0xc4 = param_2; caller slot 0 0x004527f0 installs 0x00452170"),
    ("00452170", "WarzonePontoonResetBodies", "gameplay", "the reset callback: 0x684de8 -> 0x684cd8 (Tower), 0x684c58 -> 0x684c98 (Deck), FUN_0047d180 on both bodies"),
    ("00451d80", "WarzonePontoonTick", "gameplay", "6-case state machine (jump table 0x00452118), one caller 0x00452ba2 in slot 1 0x00452b20"),
    ("00451a50", "WarzonePontoonTrackBody", "gameplay", "out = ortho(in * inv(obj+0x54) * [obj+0x94]); slot 1 Tower 0x684cd8 / Deck 0x684c98"),
    ("00451690", "WarzonePontoonAnimLoopCb", "gameplay", "RtAnim loop callback: SubAnimTime(obj+0x44), update, state = 5"),
    ("004527f0", "WarzoneSlot0", "render", "per-track hook record 0x005f3570 slot 0 (Course_Id 0x21); object part ported (Tower/Deck labels 1000/2000, FUN_00451760, FUN_00451680); tree groups / sprites / water / LOD hook were rhooks'"),
    ("00452b20", "WarzoneSlot1", "render", "record 0x005f3570 slot 1, units = int ticks; object part ported (FUN_00451d80, FUN_00451a50 x2); craters / water UV / trees not here"),
    ("00452ad0", "WarzoneSlot7", "render", "record slot 7 (called from FUN_004111c0 0x0040fefe): FUN_00451cc0(&obj) + crater switch reset"),
    ("00452af0", "WarzoneSlot6", "render", "record slot 6 (FUN_004111c0 0x00410195): FUN_00451cc0(&obj), Tower/Deck RENDER off, crater switch reset"),
    ("00452130", "WarzonePontoonDraw", "render", "slot-2 boat draw: each atomic of the clump list with the RENDER bit (atomic+2 & 4), frame LTM sync, render callback"),
    ("0052df20", "RtAnimInterpolatorSetAnimLoopCallBack", "third-party-library[renderware]", "interp+0x18 = cb, +0x1c = data (0x0052df2c / 0x0052df2f)"),
]
have = {r[ix["rva"]] for r in rows[1:] if r}
for rva, name, sub, note in NEW:
    if rva in have:
        raise SystemExit(f"{rva} already has a row")
    row = [""] * len(hdr)
    row[ix["rva"]] = rva; row[ix["name"]] = name; row[ix["subsystem"]] = sub; row[ix["confidence"]] = "C1"
    row[ix["status"]] = "mapped"; row[ix["file"]] = "verify/pontoon_20261016/PREREG_pontoon.md"
    row[ix["notes"]] = f"[pontoon {DATE}] {note}. {NOTE}"
    row[ix["exe_file"]] = TU if sub != "render" or rva in ("004527f0", "00452b20", "00452ad0", "00452af0") else ""
    pos = next((i for i in range(1, len(rows)) if rows[i] and rows[i][ix["rva"]] > rva), len(rows))   # keep file order
    rows.insert(pos, row)
out = io.StringIO(newline="")
w = csv.writer(out, lineterminator=nl)
for r in rows:
    w.writerow(r)
hp.write_bytes(out.getvalue().encode("utf-8"))
print(f"hooks.csv: {len(EXE_TU)} exe_file+note, {len(NOTE_ONLY)} note, {len(NEW)} new C1 rows")

# ---------------------------------------------------------------- UNCERTAINTIES.md
up = WT / "UNCERTAINTIES.md"
raw, nl = rw(up)
lines = raw.split(nl)
if any(l.startswith("| U-9738 ") for l in lines):
    raise SystemExit("U-9738 exists")
res = (f"~~structural~~ **RESOLVED {DATE}** (pontoon: the 0 -> 1 write is FUN_00451820 (car matrix * inv([obj+0x94]), "
       f"x in (+0x20,+0x14), z in (+0x28,+0x1c) strictly), and the whole object is ported in {TU}; the original's per-call "
       f"capture replays bit-identically through it (OP3: one 0->1->2->3->0 cycle, 1073/1073 calls). {EV})")
hit = 0
for i, l in enumerate(lines):
    if l.startswith("| U-9719 | structural |"):
        lines[i] = l.replace("| U-9719 | structural |", f"| U-9719 | {res} |", 1)
        hit += 1
if hit != 1:
    raise SystemExit(f"U-9719 rows matched {hit}")
NEWU = [
    ("U-9742", "structural", "the 0x00684d18 object's other readers: FUN_00452140 (box test of the object, callers 0x00455c5b / 0x00457e6b / 0x0045ae34 / 0x0045c451 / 0x00486b44 / 0x00486b6d) and the slot-2 boat draw's state on moving frames",
     "The six callers of FUN_00452140 (weapons / pickups on the platform, by their RVA ranges) are not read; the moving-platform draw is verified as LTMs (replay) and on capfix frames only at rest.",
     "decomp of the six callers; an original draw capture with the platform in state 2", "decompile the callers; a capfix-style capture whose shot rows fall in state 2", "nothing ported here"),
    ("U-9741", "structural", "two exe bodies each for 0x004c4680 (Track/WarzonePontoon.cpp OrthoNormalize vs Vehicle/BodyOrientationIntegrate.cpp MatrixOrthoNormalize) and 0x00532b80 (WarzonePontoon.cpp KfInterpolate vs D3d9Render/TrackRenderer.cpp LinearAnimMatrix)",
     "The pontoon copies follow the listings (disasm_ortho.txt, disasm_slerp.txt) and replay bit-identically; the D2 copy uses std::sqrt and its own pair selection, the Dump-train copy sums the slerp dot in the reverse order. Both feed default-ON paths, so unifying them moves closed gates.",
     "a D2 / Dump measurement with the shared body", "switch each caller to the listing body behind a knob, measure D2 (score_d2.py) and the Dump capfix rows, user decision", "re/tools/dual_copy_allowlist.txt (two lines added by this lane)"),
    ("U-9740", "structural", "state 5 of FUN_00451d80 (0x00684d18): FUN_004671a0(0) + FUN_004c1b40 on atomic +8's sphere",
     "Reached only through the loop callback 0x00451690 when the 33.33 s BoatAnim wraps in state 2. The exe tick has no camera frustum handle, so Track/WarzonePontoon.cpp resets at once (PontoonHost::sphere_visible unbound).",
     "a tick-side frustum test (the last render's planes, memory tick-frustum-uses-last-render-planes)", "bind sphere_visible to RwRaceFx's plane test", "state 5 only"),
    ("U-9739", "structural", "the Tower (label 1000) and Deck (label 2000) RWP bodies and the per-car contact filter (record +0xbfc / +0xc7c, reader FUN_0047d640)",
     "In the original the platform's surface colour 0xff804040 is filtered out of a car's RWP contacts while it is outside the box and kept inside; RWP-mode cars ride the Deck through those contacts. The exe has no scene-object physics: the bodies are tracked matrices only, the marker fields are written with no reader. The Tower body's rest matrix differs from TOWER.MTS by 3 dwords (1 after FUN_004c4680): the RWP body creation path that produces it is not decoded.",
     "an exe RWP world with the Deck body and the contact filter", "port the RWP scene-object bodies (D3 / physics lane)", "RWP-mode cars on the platform"),
    ("U-9738", "structural", "FUN_00451ad0 exit path for a car leaving the 0x00684d18 box: FUN_00481780 (RWP-mode velocity), FUN_00481750 x4 (wheel impulses (0,-75,0)), FUN_00480720 (hand-off to the car's RWP rigid body, record +0x14 = 1, +0x18 = 600, velocity zeroed)",
     "The exe has no per-car RWP rigid-body mode (Vehicle/VehicleCouplingBridge.cpp is inert in the exe). The port adds the movement to the record velocity only when +0x14 == 0 and keeps the car in normal mode. OP3: record +0x14 toggles on all four cars in a Warzone battle (600-unit timers), so the gap is general, not pontoon-specific.",
     "an exe RWP car mode", "port FUN_00480720 / FUN_00481780 with the coupling bridge in the exe (D3 / physics), then re-run the pontoon replay with the exit rows", "MASHED_PONTOON_CARRY default (stays OFF)"),
]
hdr_i = next(i for i, l in enumerate(lines) if l.startswith("|----"))
ins = [f"| {u} | {t} | {where} | {stmt} | {miss} | {path} | {blocks} |" for u, t, where, stmt, miss, path, blocks in NEWU]
lines[hdr_i + 1:hdr_i + 1] = ins
up.write_bytes(nl.join(lines).encode("utf-8"))
print(f"UNCERTAINTIES.md: U-9719 resolved, {len(ins)} rows added")

# ---------------------------------------------------------------- CHANGELOG
cp = WT / "re" / "analysis" / "CHANGELOG.md"
raw, nl = rw(cp)
marker = "<!-- ENTRIES -->"
if raw.count(marker) != 1:
    raise SystemExit("CHANGELOG marker count != 1")
entry = (f"{DATE}  0x00451d80 + 0x00684d18 object  C-level unchanged  pontoon: Warzone track object ported to the exe "
         f"(Track/WarzonePontoon.cpp, knobs MASHED_PONTOON_ANIM/_STATE/_CARRY default OFF), replay vs original capture "
         f"bit-identical (OP3 1073/1073); U-9719 resolved; U-9738..U-9742 filed; {EV}")
pre, post = raw.split(marker, 1)
cp.write_bytes((pre + marker + nl + entry + post).encode("utf-8"))
print("CHANGELOG: 1 entry")
