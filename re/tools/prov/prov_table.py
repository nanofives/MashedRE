"""PROV-INSITU verdict table: one row per blocking function with a match source, from the provlink bodies JSONs, the
census run and the group-test state files (prov_bisect.py). Writes a TSV and prints the counts.

    py -3.12 re/tools/prov/prov_table.py --census D:/.../census --ref D:/.../ref_a --ref2 D:/.../ref_b \
        --bodies a.json,b.json --state t95_state.jsonl,... [--dwords dwords.json] --out PROV_VERDICTS.tsv

Write-set leg (CONFIDENCE.md, L-38 / L-40 / L-54), from the body's own stores (provlink write_set, static):
  empty      no store outside its frame: nothing to compare (L-54)
  covered    every store is to an absolute VA in the per-tick compared image, outside the blocks two unmodified runs
             already disagree on
  not-covered  an indirect store (register base: heap or static, not decided statically; the per-tick hash covers the
             static image only and hashes heap-pointer-valued dwords as 0), an absolute store outside the image, or a
             store into an allowed (masked) block
"""
import argparse, json, pathlib, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "re" / "frida"))
from detorig import DATA_LO  # noqa: E402
from prov_compare import verdict  # noqa: E402


DATA = pathlib.Path("D:/MashedWT/PROV-INSITU_data")


def one(s, r, dw):
    """(kind, verdict, detail) of one run record."""
    import re as _re
    if s["verdict"] == "IDENTICAL-FOR-N":
        return "IDENTICAL", f"IDENTICAL-FOR-{s['n']}", f"run {s['run']} (group of {s['group']}, {s.get('calls_total')} calls)"
    if s["verdict"] == "DIVERGED":
        fi = s["first"] or {}
        # FLAKE-T0 (measured 2026-11-11): a tick-0 divergence whose raw words show 0x006181a0..a8 = 0x3f000000 against
        # 0 (with 0x00623d34 0 against 1 and a zero 0x006cf0a0 at the first checkpoint) appeared in 4 of 208 runs, for
        # 4 different bodies, and never again for the same body: 0x005a9e60 then replayed identically 2 of 2,
        # 0x00448220 diverged at tick 1116 2 of 2, 0x00443440 at tick 196 2 of 2. Such a run is a harness run.
        if fi.get("tick") == 0 and any(w[0] == "0x6181a0" and w[1] == "3f000000" and w[2] == "00000000" for w in fi.get("raw", [])):
            return "HARNESS", "HARNESS", f"run {s['run']}: FLAKE-T0 signature"
        det = f"run {s['run']} tick {fi.get('tick')}: {fi.get('nblocks')} blocks, first {', '.join(x[0] for x in fi.get('blocks', [])[:3])}"
        raw = [w for w in fi.get("raw", []) if w[0] != "0x6147d8"][:3]
        if raw:
            det += "; at that tick " + "; ".join(f"{w[0]} body {w[1]} orig {w[2]}" for w in raw)
        d = dw.get(r)
        if d and d.get("run") in (None, s["run"]) and d.get("words"):
            det += f"; checkpoint tick {d.get('ckpt_tick')}: " + "; ".join(f"{x[0]} body {x[1]} orig {x[2]}" for x in d["words"][:3])
        return "DIVERGED", f"DIVERGED@{fi.get('tick')}", det
    try:
        log = (DATA / f"{s['run']}.log").read_text(errors="replace")
        m = json.loads((DATA / s["run"] / "meta.json").read_text())
    except Exception:
        return "CRASH", "CRASH", f"run {s['run']} (no meta)"
    if "attach failed" in log:
        return "HARNESS", "HARNESS", f"run {s['run']}: Frida attach failed"
    anchor = "after the race anchor" if "race anchor" in log else "before the race anchor"
    if m.get("exit") is not None:
        return "CRASH", "CRASH", f"run {s['run']}: exit 0x{m['exit'] & 0xffffffff:08x} {anchor}"
    ns = _re.findall(r"'N': (\d+)", log)
    return "HANG", "HANG", f"run {s['run']}: timeout 400 s {anchor}, main-thread clock calls stopped at N={ns[-1] if ns else '?'}"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--census", required=True)
    ap.add_argument("--ref", required=True)
    ap.add_argument("--ref2", required=True)
    ap.add_argument("--bodies", required=True)
    ap.add_argument("--state", required=True)
    ap.add_argument("--dwords", default="")
    ap.add_argument("--out", required=True)
    a = ap.parse_args()
    rr = verdict(pathlib.Path(a.ref2), pathlib.Path(a.ref), pathlib.Path(a.ref2))
    allowed = set(int(x, 16) for x in rr["allowed_blocks"])
    cen = {r["rva"].lower(): r for r in json.loads((pathlib.Path(a.census) / "prov.json").read_text())["rows"]}
    st = {}
    for f in a.state.split(","):
        p = pathlib.Path(f)
        if p.exists():
            for ln in p.read_text().splitlines():
                x = json.loads(ln)
                st.setdefault(x["rva"], []).append(x)
    dw = json.loads(pathlib.Path(a.dwords).read_text()) if a.dwords and pathlib.Path(a.dwords).exists() else {}
    rows = []
    for f in a.bodies.split(","):
        for b in json.loads(pathlib.Path(f).read_text())["bodies"]:
            r = b["rva"].lower()
            c = cen.get(r, {})
            w = b.get("writes")
            if w:
                bad_abs = [x for x in w["abs_in"] if (int(x.split("@")[0].split("[")[0], 16) & ~1023) in allowed]
                if not (w["abs_in"] or w["abs_out"] or w["indirect"]):
                    leg = "empty"
                elif w["indirect"] or w["abs_out"] or bad_abs or any("[" in x for x in w["abs_in"]):
                    leg = "not-covered"
                else:
                    leg = "covered"
                wsum = f"frame {w['frame']} abs_in {len(w['abs_in'])} abs_out {len(w['abs_out'])} indirect {len(w['indirect'])}"
            else:
                leg, wsum = "-", "-"
            runs = st.get(r, [])
            s = runs[-1] if runs else None
            calls = (c.get("calls_total") or 0) - (c.get("calls_before_warp") or 0)   # race window: warp to end
            menu_calls = c.get("calls_before_warp") or 0
            if b["status"] != "OK":
                v, det = "NOT-WIRED", b["status"] + (" " + ",".join(b.get("unresolved", [])) if b.get("unresolved") else "")
            elif not calls:
                v, det = "NOT-EXECUTED", "census: 0 calls from the warp to the end of the replay" + (f" ({menu_calls} calls in the menu before the warp)" if menu_calls else "")
            elif not runs:
                v, det = "UNTESTED", ""
            elif not [x for x in runs if one(x, r, dw)[0] != "HARNESS"]:
                v, det = "UNTESTED", "harness failure only"
            else:
                outs = [o for o in (one(x, r, dw) for x in runs) if o[0] != "HARNESS"]
                runs = [x for x in runs if one(x, r, dw)[0] != "HARNESS"]
                kinds = [o[0] for o in outs]
                if all(k == "IDENTICAL" for k in kinds):
                    s = runs[0]
                    if s.get("calls_total"):
                        v, det = f"IDENTICAL-FOR-{min(x['n'] for x in runs)}", "; ".join(o[2] for o in outs)
                    else:
                        v, det = "NOT-EXECUTED", f"0 calls in run {s['run']} (census {calls})"
                elif len(set(o[1] for o in outs)) == 1 and len(outs) >= 2:
                    v, det = outs[0][1], outs[-1][2] + f" (confirmed in {len(outs)} runs)"
                elif len(outs) == 1:
                    v, det = outs[0][1], outs[0][2] + " (1 run)"
                elif "IDENTICAL" in kinds:
                    v, det = "MIXED", " | ".join(f"{o[1]}: {o[2]}" for o in outs)
                else:
                    # every run differs from the reference, at different ticks: the earliest is reported, all listed
                    v = sorted((o[1] for o in outs), key=lambda z: int(z.split("@")[1]) if "@" in z else 1 << 30)[0]
                    det = " | ".join(f"{o[1]}: {o[2]}" for o in outs)
            inst = "-"
            if s:
                jo = s.get("jmp_ok")
                if jo is None:      # the run ended before the report: the install check was sent at install time
                    try:
                        pi = json.loads((DATA / s["run"] / "prov_install.json").read_text())
                        jo = all(x.get("jmp_ok") for x in pi["installed"] if x["rva"].lower() == r)
                    except Exception:
                        jo = None
                inst = "ok" if jo else f"FAIL {jo}"
            rows.append([r, b.get("source", ""), f"{b.get('pct', 0):.1f}", b.get("equiv", ""), inst,
                         str(calls), str((s or {}).get("calls_total", "")), wsum, leg, v, det])
    hdr = ["rva", "source", "best_pct", "equiv_v4", "install", "calls_census", "calls_run", "stores", "write_set_leg",
           "verdict", "detail"]
    pathlib.Path(a.out).write_text("\t".join(hdr) + "\n" + "\n".join("\t".join(x) for x in rows) + "\n")
    from collections import Counter
    k = Counter(x[9].split("-FOR-")[0].split("@")[0] for x in rows)
    print(dict(k))
    print("identical + covered/empty:", sum(1 for x in rows if x[9].startswith("IDENTICAL") and x[8] in ("covered", "empty")))


if __name__ == "__main__":
    main()
