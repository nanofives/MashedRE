# report.py — merge a parity-sweep run into one ranked, emitter-attributed
# difference report.
#
# Input:  a run dir written by re/parity/sweep.py (index.json + per-scenario
#         bundles with re.draw.json / orig.draw.json / re.png / orig.png).
# Output: <run>/report.json (machine) + <run>/report.md (human ranked list).
#
# The draw-list channel is diffed with re/tools/drawlist_diff.py used as a
# LIBRARY (same LCS-align + tolerances as the CLI), and every divergence is
# attributed to the RE-side emitter function via the retaddr chain resolved
# through mashedmod/build/mashed_re.map. The pixel channel is an inline PIL
# region-grid diff (texture-decode / font-raster divergence the draw list
# cannot encode). report.json also carries `emitter_stats` (per-RVA match vs
# mismatch counts across the whole sweep) — the input to re/parity/target_c4.py.
#
# A standalone-vs-original match here is a TARGETING signal, never C4 evidence:
# C4 still requires re/frida/run_diff.py (hook installed in the original).
#
# Usage:  py -3.12 re/parity/report.py <run-dir> [--map PATH] [--top 40]
#                 [--pixel-grid 8x6] [--pixel-cell-threshold 24]
import argparse
import json
import re as _re
import sys
import types
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "tools"))
import drawlist_diff as dld   # noqa: E402

RE_MAP = ROOT / "mashedmod" / "build" / "mashed_re.map"

# Severity base by divergence kind (0-100 before weighting). MISSING/EXTRA are
# the loudest (a draw the original makes that RE does not, or vice versa);
# reordered is benign-ish; an unattributed pixel-only cell is lowest (often font
# / video noise the draw list deliberately cannot see).
BASE = {
    "missing": 90, "extra": 80, "mismatch_color": 60,
    "mismatch_moved": 50, "mismatch_reordered": 30, "pixel_region": 20,
}
DET_FACTOR = {"deterministic": 1.0, "settled": 0.9, "nondeterministic": 0.3}


def load_sym(map_path):
    try:
        return dld.load_msvc_map(map_path)
    except OSError:
        return None


def re_emitter(rec, sym):
    """First resolved RE-side emitter for a normalized draw record.
    -> {"rva": "0x..", "name": str|None} or None."""
    rets = rec.get("rets") or []
    for r in dld.resolve(rets[:3], sym):
        m = _re.match(r"(0x[0-9a-fA-F]+)(?:<([^+>]+))?", r)
        if m:
            return {"rva": m.group(1).lower(), "name": m.group(2)}
    return None


def last_label(data, screen=None):
    """Pick the most-settled frame label from a draw dump (last key)."""
    keys = list(data.keys())
    if not keys:
        return None
    if screen is not None:
        pref = [k for k in keys if k.startswith(f"scr{screen}")]
        if pref:
            return pref[-1]
    return keys[-1]


def prep(records, sc, side):
    """Normalize + apply the same rotate_a / exclude_tex / min_alpha / scale
    pre-processing drawlist_diff's main() does, per side."""
    tol = sc.get("tolerances", {})
    scale = tol.get("scale_a", 1.0) if side == "a" else tol.get("scale_b", 0.8)
    norm = [dld.normalize(r, scale) for r in records]
    if side == "a" and sc.get("rotate_a"):
        anchor = str(sc["rotate_a"]).lower()
        pivot = next((i for i, d in enumerate(norm)
                      if any(str(x).lower() == anchor for x in d.get("rets", []))),
                     None)
        if pivot is not None:
            norm = norm[pivot:] + norm[:pivot]
    if tol.get("min_alpha"):
        norm = [d for d in norm if dld.max_alpha(d) >= tol["min_alpha"]]
    if side == "b" and tol.get("exclude_tex"):
        ex = set(tol["exclude_tex"])
        norm = [d for d in norm if not (d.get("s") and d["s"][0] in ex)]
    return norm


def diff_draw2d(sc, orig_json, re_json, sym, stats, tol_anim_override=None):
    """-> list of report rows for the draw2d channel."""
    rows = []
    da = json.loads(Path(orig_json).read_text())
    db = json.loads(Path(re_json).read_text())
    la = last_label(da, sc.get("screen"))
    lb = last_label(db)
    if la is None or lb is None:
        return rows
    A = prep(da[la], sc, "a")
    B = prep(db[lb], sc, "b")
    # tol_anim override (CLI) lets us retune animated-chrome pairing at REPORT
    # time without re-running the (expensive) capture sweep.
    tol_anim = (tol_anim_override if tol_anim_override is not None
                else sc["tolerances"].get("tol_anim", 0.0))
    args = types.SimpleNamespace(
        tol_pos=sc["tolerances"].get("tol_pos", 1.0),
        tol_size=sc["tolerances"].get("tol_size", 1.0),
        tol_color=sc["tolerances"].get("tol_color", 0),
        tol_alpha=sc["tolerances"].get("tol_alpha", 0),
        tol_anim=tol_anim)
    pairs, mism, miss, extra = dld.diff_label(A, B, args, sym)
    det = sc.get("determinism", "settled")

    # Matched pairs feed the per-emitter MATCH tally (the C4-targeting signal).
    for _ia, jb in pairs:
        bump_stat(stats, re_emitter(B[jb], sym), sc, matched=True)

    def row(kind, geom_rec, emit_rec):
        emit = re_emitter(emit_rec, sym) if emit_rec is not None else None
        bump_stat(stats, emit, sc, matched=False)
        sev = min(100.0, BASE[kind] * sc.get("severity_weight", 1.0)
                  * DET_FACTOR.get(det, 0.9))
        return {
            "scenario_id": sc["id"], "phase": sc.get("phase"), "channel": "draw2d",
            "kind": kind, "severity": round(sev, 1), "determinism": det,
            "geom": {k: round(geom_rec[k], 2) for k in ("x", "y", "w", "h")},
            "emitter_rvas": [dict(emit, side="re")] if emit else [],
        }
    for ia, jb, k in mism:
        rows.append(row(f"mismatch_{k}", B[jb], B[jb]))
    for ia in miss:
        # original draws this, RE does not -> RE emitter unknown (A-only)
        rows.append(row("missing", A[ia], None))
    for jb in extra:
        rows.append(row("extra", B[jb], B[jb]))
    return rows


def bump_stat(stats, emit, sc, matched):
    if not emit:
        return
    rva = emit["rva"]
    e = stats.setdefault(rva, {"name": emit.get("name"), "match": 0,
                               "mismatch": 0, "scenarios": [], "phases": []})
    if emit.get("name") and not e["name"]:
        e["name"] = emit["name"]
    e["match" if matched else "mismatch"] += 1
    if sc["id"] not in e["scenarios"]:
        e["scenarios"].append(sc["id"])
    ph = sc.get("phase")
    if ph and ph not in e["phases"]:
        e["phases"].append(ph)


def diff_pixel(sc, orig_png, re_png, grid, cell_thr, ink_mask=False, ink_thr=90):
    rows = []
    try:
        from PIL import Image, ImageChops
    except Exception:
        return rows
    a = Image.open(orig_png).convert("RGB")
    b = Image.open(re_png).convert("RGB")
    if b.size != a.size:
        b = b.resize(a.size, Image.BILINEAR)
    diff = ImageChops.difference(a, b)
    px = diff.load()
    # ink-mask (chrome-focused): only count pixels where at least one side is
    # dark (chrome/text ink), so the playing video backdrop — bright on both
    # sides and perpetually out of frame-sync — does not drown the chrome
    # signal. Mirrors imgdiff.py's brightness<90 ink-map idea.
    pa = a.load() if ink_mask else None
    pb = b.load() if ink_mask else None
    w, h = diff.size
    cols, rows_n = grid
    cw, ch = w / cols, h / rows_n
    det = sc.get("determinism", "settled")
    for ry in range(rows_n):
        for rx in range(cols):
            s = cnt = 0
            for y in range(int(ry * ch), int((ry + 1) * ch)):
                for x in range(int(rx * cw), int((rx + 1) * cw)):
                    if ink_mask:
                        ra, ga, ba = pa[x, y]
                        rb, gb, bb = pb[x, y]
                        if min(max(ra, ga, ba), max(rb, gb, bb)) >= ink_thr:
                            continue   # both sides bright here -> video, skip
                    r, g, bl = px[x, y]
                    s += r + g + bl
                    cnt += 3
            # require enough sampled (ink) pixels for a cell to be meaningful
            mean = s / cnt if cnt else 0.0
            if cnt >= (60 if ink_mask else 1) and mean > cell_thr:
                sev = min(100.0, BASE["pixel_region"] * sc.get("severity_weight", 1.0)
                          * DET_FACTOR.get(det, 0.9) * min(2.0, mean / cell_thr))
                rows.append({
                    "scenario_id": sc["id"], "phase": sc.get("phase"),
                    "channel": "pixel", "kind": "pixel_region",
                    "severity": round(sev, 1), "determinism": det,
                    "geom": {"x": round(rx * cw, 1), "y": round(ry * ch, 1),
                             "w": round(cw, 1), "h": round(ch, 1)},
                    "detail": {"mean_abs_diff": round(mean, 1)},
                    "emitter_rvas": [],
                })
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("run_dir")
    ap.add_argument("--map", default=str(RE_MAP))
    ap.add_argument("--top", type=int, default=40)
    ap.add_argument("--pixel-grid", default="8x6")
    ap.add_argument("--pixel-cell-threshold", type=float, default=24.0)
    ap.add_argument("--tol-anim", type=float, default=None,
                    help="override per-scenario tol_anim at report time (absorbs "
                         "animated-chrome phase offset; retune without re-sweeping)")
    ap.add_argument("--pixel-ink", action="store_true",
                    help="chrome-focused pixel diff: ignore pixels bright on both "
                         "sides (the playing video backdrop)")
    ap.add_argument("--pixel-ink-threshold", type=int, default=90)
    args = ap.parse_args()

    run = Path(args.run_dir)
    index = json.loads((run / "index.json").read_text())
    sym = load_sym(args.map)
    if sym is None:
        print(f"note: no map at {args.map} — emitter names will be raw RVAs")
    grid = tuple(int(v) for v in args.pixel_grid.lower().split("x"))

    all_rows = []
    emitter_stats = {}
    scen_out = {}
    n_green = n_total = 0
    for sid, entry in index["scenarios"].items():
        sc = entry["scenario"]
        re_c, orig_c = entry.get("re"), entry.get("orig")
        rows = []
        status = "OK"
        # capture_ok gate (plan R7): a side that failed to reach state is an
        # ERROR, never silently GREEN.
        if entry.get("error"):
            status = "ERROR"
        draw_ok = bool(re_c and orig_c and re_c.get("ok_draw2d")
                       and orig_c.get("ok_draw2d"))
        pix_ok = bool(re_c and orig_c and re_c.get("ok_pixel")
                      and orig_c.get("ok_pixel"))
        if "draw2d" in sc.get("capture", []) and draw_ok:
            rows += diff_draw2d(sc, orig_c["draw2d"], re_c["draw2d"], sym,
                                emitter_stats, args.tol_anim)
        elif "draw2d" in sc.get("capture", []):
            status = "ERROR"
        if "pixel" in sc.get("capture", []) and pix_ok:
            rows += diff_pixel(sc, orig_c["pixel"], re_c["pixel"], grid,
                               args.pixel_cell_threshold, args.pixel_ink,
                               args.pixel_ink_threshold)
        elif "pixel" in sc.get("capture", []) and status != "ERROR":
            status = "PIXEL-MISSING"

        n_total += 1
        verdict = ("ERROR" if status == "ERROR"
                   else "GREEN" if not rows else "RED")
        if verdict == "GREEN":
            n_green += 1
        scen_out[sid] = {"verdict": verdict, "status": status,
                         "n_rows": len(rows),
                         "by_kind": _count_kinds(rows)}
        all_rows.extend(rows)

    all_rows.sort(key=lambda r: r["severity"], reverse=True)
    # finalize emitter_stats (lists already de-duped)
    report = {
        "run": str(run), "ts": index.get("ts"),
        "summary": {
            "scenarios": n_total, "green": n_green,
            "pct_green": round(100.0 * n_green / n_total, 1) if n_total else 0.0,
            "total_rows": len(all_rows),
        },
        "scenarios": scen_out,
        "rows": all_rows,
        "emitter_stats": emitter_stats,
    }
    (run / "report.json").write_text(json.dumps(report, indent=1, default=str))
    _write_md(run, report, args.top)
    print(f"-> {run / 'report.json'}")
    print(f"-> {run / 'report.md'}")
    print(f"VERDICT: {n_green}/{n_total} scenarios GREEN, "
          f"{len(all_rows)} divergence rows")
    return 0


def _count_kinds(rows):
    out = {}
    for r in rows:
        out[r["kind"]] = out.get(r["kind"], 0) + 1
    return out


def _write_md(run, report, top):
    s = report["summary"]
    L = [f"# Parity sweep report — {report.get('ts','')}", "",
         f"**{s['green']}/{s['scenarios']} scenarios GREEN** "
         f"({s['pct_green']}%) — {s['total_rows']} divergence rows", "",
         "## Per-scenario", "",
         "| scenario | verdict | rows | breakdown |",
         "|---|---|---|---|"]
    for sid, v in report["scenarios"].items():
        bk = ", ".join(f"{k}:{n}" for k, n in v["by_kind"].items()) or "—"
        L.append(f"| {sid} | {v['verdict']} | {v['n_rows']} | {bk} |")
    L += ["", f"## Top {top} divergences (by severity)", "",
          "| sev | scenario | channel | kind | geom (x,y,w,h) | emitter |",
          "|---|---|---|---|---|---|"]
    for r in report["rows"][:top]:
        g = r["geom"]
        emit = ", ".join(f"{e.get('name') or e['rva']}" for e in r["emitter_rvas"]) or "—"
        L.append(f"| {r['severity']} | {r['scenario_id']} | {r['channel']} | "
                 f"{r['kind']} | {g['x']},{g['y']},{g['w']},{g['h']} | {emit} |")
    L += ["", "## C4-targeting note", "",
          "Per-emitter match/mismatch tallies are in `report.json` → "
          "`emitter_stats`; run `target_c4.py <run>` to turn the clean-match "
          "set into a `run_diff` queue. A sweep match is a targeting signal, "
          "**not** C4 evidence — `re/frida/run_diff.py` (hook installed in the "
          "original) remains the gate."]
    (run / "report.md").write_text("\n".join(L), encoding="utf-8")


if __name__ == "__main__":
    sys.exit(main())
