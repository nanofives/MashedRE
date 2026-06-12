#!/usr/bin/env python3
"""One-shot re-classify transaction for frida-sweep-20260612-1932 (stale
af-queue drain). Raw-line editing: only target lines change, everything else
stays byte-identical. Idempotent: refuses to run if the sweep tag is already
present in hooks.csv."""

import csv
import io
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SWEEP = "frida-sweep-20260612-1932"

PROMOS = {
    "00473540": {
        "name": "GradientQuadHorizAlpha",
        "confidence": "C3",
        "status": "impl",
        "file": "mashedmod/src/mashed_re/Frontend/MenuLeaves_af1.cpp",
        "frida_diff": "log/diff_gradient_quad_horiz_alpha.csv",
        "note": (" | C2->C3 " + SWEEP + " (stale af-queue drain): Frida 10/10 GREEN "
                 "non-trivial vs canonical .asi (non-degeneracy assertion active); "
                 "caller MenuDrawLoop 0x0043c5b0 C4 (Ghidra pool1 function_callers), "
                 "callees ScreenWidthGet/ScreenHeightGet 0x0042b8b0/0x0042b8c0 C3; "
                 "analysis note re/analysis/hud_frontend/0x00473540.md; U-0457 "
                 "(Blocks: none) stays open - caller context now known"),
    },
    "004736c0": {
        "name": "BorderQuadFourAlpha",
        "confidence": "C3",
        "status": "impl",
        "file": "mashedmod/src/mashed_re/Frontend/MenuLeaves_af1.cpp",
        "frida_diff": "log/diff_border_quad_four_alpha.csv",
        "note": (" | C2->C3 " + SWEEP + ": Frida 10/10 GREEN non-trivial vs canonical "
                 ".asi; caller FUN_00434720 C2, callees 0x0042b8b0/0x0042b8c0 C3; "
                 "U-5706/U-5707/U-5708 RESOLVED via vendored RW headers (rwplcore.h: "
                 "RwGlobals.dOpenDevice@+0x10 -> +0x18=zBufferNear, "
                 "+0x20=fpRenderStateSet(RwRenderState), "
                 "+0x30=fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP=4, verts, "
                 "numVerts=4)); S-4398/S-4399 remain (block C4, not C3); analysis "
                 "note re/analysis/font_pools_frontend_ae6/0x004736c0.md"),
    },
    "00401ee0": {
        "name": "EntityTableSelectUpdate",
        "confidence": "C3",
        "status": "impl",
        "file": "mashedmod/src/mashed_re/Frontend/MenuLeaves_af4.cpp",
        "frida_diff": "log/diff_entity_table_select_update.csv",
        "note": (" | C2->C3 " + SWEEP + ": Frida 4/4 GREEN non-trivial vs canonical "
                 ".asi; caller FUN_00428d30 C2 (Ghidra pool1 function_callers), "
                 "callees EntryTableScanByKey 0x00401570 C3 + FUN_00401da0 C2 + "
                 "RpClumpRender 0x004e6680 C2; U-4199 RESOLVED (FUN_004e6680 "
                 "identified as RpClumpRender): guarded call renders clump at "
                 "*DAT_00636ac0 when non-null; analysis note "
                 "re/analysis/frontend_c1_to_c2_s1/00401ee0.md"),
    },
    "0043aee0": {
        "note": (" | re-verified 7/7 GREEN 2026-06-12 " + SWEEP +
                 " (canonical .asi, non-degeneracy assertion active); no status change"),
    },
}

U_RESOLUTIONS = {
    "U-4199": ("RESOLVED 2026-06-12 (" + SWEEP + "): `FUN_004e6680` = RpClumpRender "
               "(hooks.csv 004e6680, render C2) - the guarded call renders the RpClump "
               "at `*DAT_00636ac0` when non-null. ~~`FUN_004e6680()` called only when "
               "`*DAT_00636ac0 != 0`; purpose unknown.~~"),
    "U-5706": ("RESOLVED 2026-06-12 (" + SWEEP + "): +0x18 = "
               "RwGlobals.dOpenDevice.zBufferNear (dOpenDevice at RwGlobals+0x10, "
               "rwplcore.h:6282-6298; zBufferNear at RwDevice+0x8, rwplcore.h:5838) - "
               "the Im2D near-screen Z for RHW vertices, exactly the "
               "RWIM2DGETNEARSCREENZ macro (rwplcore.h:5523). Three offsets "
               "(+0x18/+0x20/+0x30) match this one base consistently. ~~vtable+0x18 "
               "field read: exact purpose not confirmed~~"),
    "U-5707": ("RESOLVED 2026-06-12 (" + SWEEP + "): +0x20 = "
               "dOpenDevice.fpRenderStateSet (RwDevice+0x10, rwplcore.h:5842); indices "
               "are RwRenderState (rwplcore.h:5069): 0x1=rwRENDERSTATETEXTURERASTER "
               "(value NULL), 0xa=rwRENDERSTATESRCBLEND (5=rwBLENDSRCALPHA, "
               "rwplcore.h:5309), 0xb=rwRENDERSTATEDESTBLEND (6=rwBLENDINVSRCALPHA), "
               "0xc=rwRENDERSTATEVERTEXALPHAENABLE (TRUE). The originally proposed "
               "D3D9-RS mapping was incorrect. ~~map to D3D9 SetRenderState indices "
               "but not verified~~"),
    "U-5708": ("RESOLVED 2026-06-12 (" + SWEEP + "): +0x30 = "
               "dOpenDevice.fpIm2DRenderPrimitive (RwDevice+0x20, rwplcore.h:5848); "
               "typedef rwplcore.h:5798 (RwPrimitiveType primType, RwIm2DVertex* "
               "vertices, RwInt32 numVertices) -> args (4, &buf, 4) = "
               "(rwPRIMTYPETRISTRIP [=4, rwplcore.h:5503], vertex buffer, "
               "numVertices=4 - a VERTEX COUNT, not a byte stride). ~~stride/count "
               "arg semantics not confirmed~~"),
}

CHANGELOG_LINES = [
    "2026-06-12  00473540  GradientQuadHorizAlpha  C2->C3  " + SWEEP +
    " stale-af drain: Frida 10/10 GREEN non-trivial (canonical .asi, non-degeneracy "
    "assertion active); caller MenuDrawLoop C4; U-0457 Blocks=none",
    "2026-06-12  004736c0  BorderQuadFourAlpha  C2->C3  " + SWEEP +
    ": Frida 10/10 GREEN non-trivial; U-5706/U-5707/U-5708 RESOLVED via rwplcore.h "
    "RwGlobals.dOpenDevice cross-ref (CLAUDE.md-sanctioned RW headers)",
    "2026-06-12  00401ee0  EntityTableSelectUpdate  C2->C3  " + SWEEP +
    ": Frida 4/4 GREEN non-trivial; U-4199 RESOLVED (FUN_004e6680 = RpClumpRender)",
    "2026-06-12  0043aee0  MenuSlotFlagSetCurrent  C3->C3  " + SWEEP +
    " re-verified 7/7 GREEN; no status change",
]


def edit_hooks_csv():
    p = ROOT / "hooks.csv"
    lines = p.read_text(encoding="utf-8").splitlines(keepends=True)
    if any(SWEEP in ln for ln in lines):
        sys.exit("hooks.csv already contains this sweep tag - refusing to re-run")
    header = next(csv.reader([lines[0]]))
    idx = {c: i for i, c in enumerate(header)}
    done = set()
    for n, ln in enumerate(lines):
        rva = ln.split(",", 1)[0]
        if rva not in PROMOS or rva in done:
            continue
        row = next(csv.reader([ln.rstrip("\r\n")]))
        spec = PROMOS[rva]
        for col in ("name", "confidence", "status", "file", "frida_diff"):
            if col in spec:
                row[idx[col]] = spec[col]
        row[idx["notes"]] += spec["note"]
        buf = io.StringIO()
        csv.writer(buf, lineterminator="\n").writerow(row)
        lines[n] = buf.getvalue()
        done.add(rva)
    missing = set(PROMOS) - done
    if missing:
        sys.exit(f"hooks.csv rows not found: {missing}")
    p.write_text("".join(lines), encoding="utf-8")
    print(f"hooks.csv: {len(done)} rows updated")


def edit_uncertainties():
    p = ROOT / "UNCERTAINTIES.md"
    lines = p.read_text(encoding="utf-8").splitlines(keepends=True)
    done = set()
    for n, ln in enumerate(lines):
        for uid, res in U_RESOLUTIONS.items():
            if ln.lstrip().startswith(f"| {uid} ") and uid not in done:
                cells = ln.split("|")
                # cells: ['', ' U-NNNN ', ' type ', ' where ', ' statement ',
                #         ' evidence ', ' path ', ' blocks ', '\n']
                cells[4] = " " + res + " "
                cells[7] = " — resolved " + SWEEP + " "
                lines[n] = "|".join(cells)
                done.add(uid)
    missing = set(U_RESOLUTIONS) - done
    if missing:
        sys.exit(f"UNCERTAINTIES.md rows not found: {missing}")
    p.write_text("".join(lines), encoding="utf-8")
    print(f"UNCERTAINTIES.md: {len(done)} rows resolved")


def edit_changelog():
    p = ROOT / "re" / "analysis" / "CHANGELOG.md"
    with p.open("a", encoding="utf-8") as f:
        for ln in CHANGELOG_LINES:
            f.write(ln + "\n")
    print(f"CHANGELOG.md: {len(CHANGELOG_LINES)} lines appended")


def edit_promotion_queue():
    p = ROOT / "re" / "PROMOTION_QUEUE.md"
    text = p.read_text(encoding="utf-8")
    moved = 0
    tag = ("  drained-by=" + SWEEP + " (STALE rows: content was already on main; "
           "evidence re-validated 2026-06-12, integration diff GREEN 4/4 hooks; "
           "0x0043aee0 was already C3 - re-verified only)")
    out_lines = []
    moved_rows = []
    in_queued = False
    for ln in text.splitlines(keepends=True):
        s = ln.strip()
        if s.startswith("## "):
            in_queued = s == "## Queued"
        if in_queued and ("c3-batch-af-s1" in s or "c3-batch-af-s4" in s) \
                and s.startswith("2026-06-04"):
            moved_rows.append(ln.rstrip("\r\n") + tag + "\n")
            moved += 1
            continue
        out_lines.append(ln)
    if moved != 2:
        sys.exit(f"expected to move 2 queue rows, found {moved}")
    text = "".join(out_lines)
    anchor = "## Merged"
    if anchor not in text:
        sys.exit("no '## Merged' section in PROMOTION_QUEUE.md")
    pos = text.index(anchor) + len(anchor)
    text = text[:pos] + "\n\n" + "".join(moved_rows) + text[pos:]
    p.write_text(text, encoding="utf-8")
    print(f"PROMOTION_QUEUE.md: {moved} rows moved to Merged")


if __name__ == "__main__":
    edit_hooks_csv()
    edit_uncertainties()
    edit_changelog()
    edit_promotion_queue()
    print("transaction complete")
