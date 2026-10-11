"""C1->C2 batch under the generated-plate clause (re/CONFIDENCE.md, ruling 2026-10-11).

    py -3.12 re/tools/c2_batch.py stage --rvas <file> --stage <dir>
        generate plates (no U-ids) into <dir>/plates, capture Ghidra's decompilation of the same rows from a
        pool slot (<dir>/decomp_witness.json), and write <dir>/verdict.tsv. A row PASSES only when the
        checker reports zero missing callees and the lint is clean; anything else FAILS with the reason.

    py -3.12 re/tools/c2_batch.py apply --stage <dir> --uid-start 9301 --out-root re/analysis/c2gen --date 2026-10-11
        for every PASS row: assign one U-id to each plate whose Uncertainties list is non-empty, write the
        final plate to <out-root>/<subsystem>/<rva>.md carrying `[UNCERTAIN U-NNNN]`, and apply the re-classify
        transaction: hooks.csv C1->C2 (file -> plate, note appended), one UNCERTAINTIES.md row per U-id
        (top of the Active table), one CHANGELOG line per row inserted below `<!-- ENTRIES -->`.
        Refuses if any assigned U-id already appears in UNCERTAINTIES.md, or a row is no longer C1.

Tracker bytes outside the edited rows are preserved exactly (latin-1 round trip, original line endings).
"""
import argparse, csv, io, json, pathlib, re, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import c2_note, c2_note_check, xref  # noqa: E402

ROOT = xref.ROOT


def witness(vas):
    import decomp_pc as d
    d.POOL_DIR = xref.MAIN / "mashed_pool"                     # a worktree has no pool of its own
    d.POOL_SH_ARG = (xref.MAIN / "scripts" / "ghidra_pool.sh").as_posix()
    name, idx = d.acquire_slot()
    try:
        return d.run_headless(name, [c2_note.hexa(v) for v in vas], ["decomp", "callees"]), name
    finally:
        d.release_slot(idx)


def unc_items(text):
    sec = text.split("## Uncertainties", 1)[1].split("## Stubs encountered", 1)[0]
    return [l[2:] for l in sec.splitlines() if l.startswith("- ")]


def stage(a):
    st = pathlib.Path(a.stage)
    (st / "plates").mkdir(parents=True, exist_ok=True)
    vas = [int(l.split()[0], 16) for l in open(a.rvas, encoding="utf-8") if l.strip() and not l.startswith("#")]
    ctx = c2_note.Ctx()
    texts = {}
    for va in vas:
        try:
            res = c2_note.note(ctx, va, date=a.date)
        except SystemExit as e:                                  # lint refusal
            texts[va] = ("LINT", str(e))
            continue
        if res is None:
            texts[va] = ("NOFN", "not a Ghidra function entry in the facts dump")
            continue
        (st / "plates" / f"{va:08x}.md").write_text(res[0], encoding="utf-8")
        texts[va] = ("OK", res[0])
    data, slot = witness(vas)
    (st / "decomp_witness.json").write_text(json.dumps(data, indent=1), encoding="utf-8")
    fns = {int(f["requested"], 16): f for f in data["functions"]}
    out = ["rva\tname\tsubsystem\tverdict\treason\tn_uncertain\tslot"]
    npass = 0
    for va in vas:
        r = ctx.hooks.get(va, {})
        kind, text = texts[va]
        n = 0
        if kind != "OK":
            verdict, reason = "FAIL", f"{kind}: {text}"
        elif va not in fns or "error" in fns[va]:
            verdict, reason = "FAIL", "no decompiler witness: " + (fns.get(va, {}).get("error") or "missing")
        else:
            miss = [m for m in c2_note_check.check_decomp(text, fns[va]) if "callee" in m]
            n = len(unc_items(text)) if "None found by the generator." not in text else 0
            verdict, reason = ("PASS", "0 callee misses, lint clean") if not miss else ("FAIL", "; ".join(miss))
        npass += verdict == "PASS"
        out.append(f"{c2_note.hexa(va)}\t{ctx.name(va)}\t{r.get('subsystem', '')}\t{verdict}\t{reason}\t{n}\t{slot}")
    (st / "verdict.tsv").write_text("\n".join(out) + "\n", encoding="utf-8")
    print(f"{npass} PASS / {len(vas) - npass} FAIL of {len(vas)} -> {st / 'verdict.tsv'}")


# ---------------------------------------------------------------- tracker transaction
def _read(path):
    b = path.read_bytes()
    nl = "\r\n" if b"\r\n" in b[:4096] else "\n"
    return b.decode("latin-1"), nl


def _write(path, text):
    path.write_bytes(text.encode("latin-1"))


def resolution(items):
    kinds = []
    if any("target not determined statically" in i for i in items):
        kinds.append("indirect call/jump targets: capture them at run time (entry probe on the call site) or "
                     "trace the pointer's writer statically")
    if any("esp depth" in i for i in items):
        kinds.append("esp depth: read the callee cleanup of the listed calls (`ret N` or caller `add esp`) by hand")
    if any("not a Ghidra function entry" in i for i in items):
        kinds.append("callee without a Ghidra function: define it (CreateMissedFunctions.java) and regenerate")
    if any("fall-through not followed" in i for i in items):
        kinds.append("no-return callee: confirm the callee never returns (its last call) before relying on the cut")
    if any("address order" in i for i in items):
        kinds.append("register input: confirm execution-order read-before-write on every path from the entry")
    if any("capstone stopped" in i or "jump table" in i or "below the entry" in i for i in items):
        kinds.append("body coverage: disassemble the untranscribed bytes and regenerate")
    return "; ".join(kinds) or "hand-read each listed item against the disassembly"


def apply(a):
    st = pathlib.Path(a.stage)
    rows = list(csv.DictReader(open(st / "verdict.tsv", encoding="utf-8"), delimiter="\t"))
    passing = [r for r in rows if r["verdict"] == "PASS"]
    ctx = c2_note.Ctx()
    unc_text, nl_u = _read(ROOT / "UNCERTAINTIES.md")
    hooks_text, nl_h = _read(ROOT / "hooks.csv")
    log_text, nl_c = _read(ROOT / "re" / "analysis" / "CHANGELOG.md")

    uid, uids, new_u, new_log = a.uid_start, {}, [], []
    for r in passing:
        va = int(r["rva"], 16)
        if ctx.hooks.get(va, {}).get("confidence") != "C1":
            raise SystemExit(f"{r['rva']} is not C1 in hooks.csv; refusing")
        if int(r["n_uncertain"]):
            u = f"U-{uid}"
            if re.search(rf"\b{u}\b", unc_text):
                raise SystemExit(f"{u} already exists in UNCERTAINTIES.md; pick another --uid-start")
            uids[va] = u
            uid += 1
    edits = {}
    for r in passing:
        va = int(r["rva"], 16)
        sub = ctx.hooks[va]["subsystem"]
        text, _, _ = c2_note.note(ctx, va, date=a.date, uid=uids.get(va))
        p = pathlib.Path(a.out_root) / c2_note.slug(sub) / f"{va:08x}.md"
        full = ROOT / p
        if full.exists() and c2_note.GEN not in full.read_text(encoding="utf-8", errors="replace")[:2000]:
            raise SystemExit(f"{p} exists and is hand-written; refusing")
        full.parent.mkdir(parents=True, exist_ok=True)
        full.write_text(text, encoding="utf-8")
        items = unc_items(text)
        items = [i for i in items if not i.startswith("[UNCERTAIN")]
        tag = f"; {uids[va]}" if va in uids else ""
        edits[r["rva"][2:]] = (p.as_posix(), f"C1->C2 {a.date} generated plate (c2_note.py; c2_note_check 0 callee "
                                             f"misses vs {r['slot']} decomp, lint clean){tag}")
        new_log.append(f"{a.date}  {r['rva']}  {ctx.name(va)}  C1->C2  generated plate {p.as_posix()} "
                       f"(generated-plate clause: 0 callee misses vs {r['slot']} decomp, lint clean"
                       + (f"; open items filed {uids[va]}" if va in uids else "") + ")")
        if va in uids:
            stmt = (f"Generated C2 plate `{p.as_posix()}` lists {len(items)} statically unresolved item(s): "
                    + " ".join(i.replace("|", "/") for i in items))
            new_u.append(f"| {uids[va]} | structural | {r['rva']} | {stmt} | Per item, as listed in the plate's "
                         f"`## Uncertainties` (generator output, no hand-read). | {resolution(items)} | nothing at C2; "
                         f"the C3 hand-read must close or carry these |")

    # hooks.csv: rewrite only the edited rows
    lines = hooks_text.split(nl_h)
    done = set()
    for k, line in enumerate(lines):
        key = line.split(",", 1)[0]
        if key in edits and key not in done:
            row = next(csv.reader([line]))
            hdr = ["rva", "name", "subsystem", "confidence", "status", "file", "scenario", "frida_diff", "notes"]
            assert row[3] == "C1", (key, row[3])
            row[3] = "C2"
            row[5] = edits[key][0]
            row[8] = (row[8] + " | " if row[8] else "") + edits[key][1]
            buf = io.StringIO()
            csv.writer(buf, lineterminator="").writerow(row)
            lines[k] = buf.getvalue()
            done.add(key)
    missing = set(edits) - done
    if missing:
        raise SystemExit(f"hooks.csv rows not found: {sorted(missing)}")
    hooks_text = nl_h.join(lines)

    # UNCERTAINTIES.md: insert below the Active table's separator line
    if new_u:
        m = re.search(r"## Active uncertainties.*?\n\|----\|[^\n]*\n", unc_text, re.S)
        if not m:
            raise SystemExit("Active uncertainties table separator not found")
        unc_text = unc_text[:m.end()] + nl_u.join(new_u) + nl_u + unc_text[m.end():]

    # CHANGELOG: insert immediately below the exact marker line, never rewrite
    marker = "<!-- ENTRIES -->" + nl_c
    i = log_text.find(nl_c + marker)
    if i < 0:
        raise SystemExit("CHANGELOG <!-- ENTRIES --> marker line not found")
    j = i + len(nl_c + marker)
    log_text = log_text[:j] + nl_c.join(new_log) + nl_c + log_text[j:]

    _write(ROOT / "hooks.csv", hooks_text)
    _write(ROOT / "UNCERTAINTIES.md", unc_text)
    _write(ROOT / "re" / "analysis" / "CHANGELOG.md", log_text)
    (st / "uids.tsv").write_text("".join(f"{c2_note.hexa(v)}\t{u}\n" for v, u in uids.items()), encoding="utf-8")
    print(f"C1->C2: {len(passing)} rows; U-ids {', '.join(uids.values()) if len(uids) < 12 else f'{min(uids.values())}..{max(uids.values())} ({len(uids)})'}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sp = ap.add_subparsers(dest="cmd", required=True)
    s = sp.add_parser("stage")
    s.add_argument("--rvas", required=True)
    s.add_argument("--stage", required=True)
    s.add_argument("--date")
    s = sp.add_parser("apply")
    s.add_argument("--stage", required=True)
    s.add_argument("--uid-start", type=int, required=True)
    s.add_argument("--out-root", default="re/analysis/c2gen")
    s.add_argument("--date", required=True)
    a = ap.parse_args()
    {"stage": stage, "apply": apply}[a.cmd](a)


if __name__ == "__main__":
    main()
