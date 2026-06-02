# sweep_apply_eval.py — Phase B of the scripted ghidra-sweep (Ghidra 12 / PyGhidra).
#
# Run via the MCP `ghidra_eval` tool against an already-open master session:
#
#   _ns = {}
#   exec(open(r"<repo>/scripts/ghidra/sweep_apply_eval.py").read(), _ns)
#   _ns["run"](sessions, "<session_id>", r"<repo>/sweep_manifest.json")
#
# `sessions` is the eval namespace's live backend session dict (session_id ->
# SessionRecord). The whole batch runs inside ONE program transaction, so the
# ~5-MCP-round-trips-per-RVA loop in the legacy sweep collapses to a single eval.
#
# Supersedes scripts/ghidra/BulkPlateBookmark.py, which targeted the pre-12
# `CodeUnit.*_COMMENT` constants (removed/relocated to
# ghidra.program.model.listing.CommentType in Ghidra 12.0.3) and the Ghidra
# *script* globals (currentProgram/getFunctionAt/createBookmark) that the
# `ghidra_eval` namespace does not expose.
#
# Manifest schema (nested-by-bucket; produced by scripts/sweep_build_manifest.py):
#   { "date": "YYYY-MM-DD", "level": "C2",
#     "buckets": [ { "bucket": "bucket_xxxx", "bm_category": "promote-c2",
#                    "rvas": [ {"rva":"0x00xxxxxx", "plate":"[C2 ...] ...",
#                               "rename": null } ],
#                    "missing_md": [...] } ] }
#
# Protocol semantics preserved verbatim from ghidra-sweep SKILL.md step 7:
#   - PLATE comment via CommentType.PLATE.
#   - bookmark NOTE, category = manifest bm_category, comment
#       "<level> <date> session:<bucket>".
#   - C0 fallback: function_at None -> listing.getCodeUnitAt; if present write the
#     plate at listing level (no bookmark); else record missing.
#   - Conservative rename: only when (a) the manifest entry carries an explicit
#     `rename`, OR (b) a pre-existing comment is a single-line
#     "Library Function: <sym>" attestation — AND the Ghidra name still starts
#     with FUN_. Audio promote-c2 declares none -> zero renames here.
#   - Any per-RVA exception is captured in errors[]; the caller HALTS on errors
#     (it does not silently continue), matching the skill's halt-on-error rule.

import json
import re

_LIBPREFIX = re.compile(r"^Library Function:\s*([A-Za-z_][A-Za-z0-9_:?@$]*)\s*$")


def _lib_attestation(text):
    if not text:
        return None
    lines = [l for l in text.strip().splitlines() if l.strip()]
    if len(lines) != 1:
        return None
    m = _LIBPREFIX.match(lines[0].strip())
    return m.group(1) if m else None


def run(sessions, session_id, manifest_path):
    from ghidra.program.model.listing import CommentType
    from ghidra.program.model.symbol import SourceType

    # The ghidra_eval `sessions` map yields the ProgramDB directly; the backend's
    # internal SessionRecord wraps it as `.program`. Handle both shapes.
    obj = sessions[session_id]
    prog = getattr(obj, "program", obj)
    listing = prog.getListing()
    fm = prog.getFunctionManager()
    bmgr = prog.getBookmarkManager()
    af = prog.getAddressFactory()

    with open(manifest_path, "r", encoding="utf-8") as f:
        man = json.load(f)
    date = man.get("date", "")
    level = man.get("level", "C2")

    scan_types = [CommentType.PRE, CommentType.POST, CommentType.EOL,
                  CommentType.REPEATABLE, CommentType.PLATE]

    totals = {"plates": 0, "bookmarks": 0, "renames": 0, "c0_listing": 0,
              "missing_rvas": [], "missing_md": [], "errors": []}
    out_buckets = []

    tx = prog.startTransaction("scribe-sweep apply %s" % date)
    committed = False
    try:
        for b in man["buckets"]:
            bucket = b["bucket"]
            cat = b.get("bm_category", "promote-c2")
            bcomment = "%s %s session:%s" % (level, date, bucket)
            bs = {"bucket": bucket, "plates": 0, "bookmarks": 0, "renames": 0,
                  "c0_listing": [], "missing_rvas": [], "errors": []}
            for e in b["rvas"]:
                rva = e["rva"]
                plate = e.get("plate", "")
                if not plate:
                    totals["missing_md"].append(rva)
                    continue
                try:
                    addr = af.getAddress(rva)
                    if addr is None:
                        bs["missing_rvas"].append(rva)
                        totals["missing_rvas"].append(rva)
                        continue
                    fn = fm.getFunctionAt(addr)
                    if fn is not None:
                        # Collect library attestations BEFORE overwriting the plate.
                        cands = set()
                        c = _lib_attestation(fn.getComment() or "")
                        if c:
                            cands.add(c)
                        for ct in scan_types:
                            c = _lib_attestation(listing.getComment(ct, addr) or "")
                            if c:
                                cands.add(c)

                        listing.setComment(addr, CommentType.PLATE, plate)
                        bs["plates"] += 1
                        totals["plates"] += 1

                        bmgr.setBookmark(addr, "NOTE", cat, bcomment)
                        bs["bookmarks"] += 1
                        totals["bookmarks"] += 1

                        target = e.get("rename")
                        if not target and len(cands) == 1:
                            target = list(cands)[0]
                        if target and fn.getName().startswith("FUN_"):
                            try:
                                fn.setName(target, SourceType.USER_DEFINED)
                                bs["renames"] += 1
                                totals["renames"] += 1
                            except Exception as ex:  # noqa: BLE001
                                msg = "rename %s->%s: %s" % (rva, target, ex)
                                bs["errors"].append(msg)
                                totals["errors"].append(msg)
                    else:
                        cu = listing.getCodeUnitAt(addr)
                        if cu is None:
                            bs["missing_rvas"].append(rva)
                            totals["missing_rvas"].append(rva)
                            continue
                        listing.setComment(addr, CommentType.PLATE, plate)
                        bs["plates"] += 1
                        totals["plates"] += 1
                        bs["c0_listing"].append(rva)
                        totals["c0_listing"] += 1
                except Exception as ex:  # noqa: BLE001
                    msg = "rva=%s err=%s" % (rva, ex)
                    bs["errors"].append(msg)
                    totals["errors"].append(msg)
            out_buckets.append(bs)
        committed = len(totals["errors"]) == 0
    finally:
        # Commit only when clean; on any error roll the whole batch back so the
        # caller can fix-and-retry without partial state on master.
        prog.endTransaction(tx, committed)

    return {"date": date, "level": level, "committed": committed,
            "buckets": out_buckets, "totals": totals}
