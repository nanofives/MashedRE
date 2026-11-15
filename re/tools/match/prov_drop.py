"""Drop PROVISIONAL.tsv rows whose source is byte-identical and included by the exe (MATCH-PROV-DROP-RULE, 2026-11-15).

Rule (L-126 bookkeeping, proposed by MATCH-RWT-PORT-FOLLOWUPS, which found 92 stale rows): a provisional row describes a
NOT-PROVEN source. Once that same source is byte-identical (match.py --bytes 0 differing bytes with the toolchain and
flags recorded in the source's own `// TOOLCHAIN` / `// FLAGS:` lines, ddk or rw9044 only) AND relocheck.py reports OK
for the row's RVA AND a translation unit listed in mashedmod/exe_sources.rsp includes that source (`#include` of the
re/match path, followed through quoted includes under mashedmod/src), the row no longer describes the body the exe
runs, and it is dropped. ddkobj.py load_provisional() keeps every listed RVA out of the object build, so a stale row
also keeps a byte-identical unit out of mashed_re_ddk.exe.

A row is KEPT, with its reason, when any test fails or cannot run:
  no-re-match-source   the source column is not a re/match/*.c path (lifted RwCore bodies, L-137 / L-141)
  source-missing       the file is not in the tree
  naked                the source has __declspec(naked) (a naked transcription is never the C4 match route)
  toolchain            the recorded toolchain is not ddk / rw9044
  admitted             the RVA is also listed in re/match/ADMITTED.tsv (resolve the pair by hand)
  not-included         no exe TU includes the source
  match-error          match.py stopped (compile failure, or no Ghidra size for the RVA)
  no-symbol            the source has no `// MATCH:` function at the row's RVA
  not-byte-identical   match.py --bytes reports differing bytes
  relocheck            relocheck.py does not report OK for the RVA

Dry run by default: prints one line per row (DROP / KEEP) and, with --tsv, writes the evidence table. --apply rewrites
PROVISIONAL.tsv without the DROP rows, keeping every other byte (comment line, header, line endings).

    py -3.12 re/tools/match/prov_drop.py [--tsv out.tsv] [--apply] [--jobs N] [--only 0xRVA,...]
"""
import argparse
import concurrent.futures as cf
import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

ROOT = HERE.parents[2]
PROV = ROOT / "re" / "match" / "PROVISIONAL.tsv"
ADMITTED = ROOT / "re" / "match" / "ADMITTED.tsv"
SRC_ROOT = ROOT / "mashedmod" / "src" / "mashed_re"
EXE_RSP = ROOT / "mashedmod" / "exe_sources.rsp"
MATCH_TOOLCHAINS = ("ddk", "rw9044")

RE_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')
RE_NAKED = re.compile(r"__declspec\s*\(\s*naked\s*\)")
RE_TC = re.compile(r"^\s*//\s*TOOLCHAIN\s+(\S+)\s*$", re.M)


# ------------------------------------------------------------------ tables
def read_tsv(path: pathlib.Path):
    """(header columns, [(line_index, row dict)]) skipping '#' comment lines."""
    # split on "\n" only (str.splitlines also splits on \x0b, \x1c.. and would shift indices against --apply)
    lines = [ln[:-1] if ln.endswith("\r") else ln for ln in path.read_text(encoding="utf-8").split("\n")]
    header, rows = None, []
    for i, ln in enumerate(lines):
        if not ln.strip() or ln.startswith("#"):
            continue
        cols = ln.split("\t")
        if header is None:
            header = cols
            continue
        rows.append((i, dict(zip(header, cols + [""] * (len(header) - len(cols))))))
    return header, rows


def admitted_rvas(path: pathlib.Path):
    if not path.exists():
        return set()
    return {int(r["rva"], 16) for _, r in read_tsv(path)[1] if r.get("rva", "").startswith("0x")}


# ------------------------------------------------------------------ include map
def rel(p: pathlib.Path, root: pathlib.Path) -> str:
    return p.resolve().relative_to(root.resolve()).as_posix()


def exe_tus(rsp: pathlib.Path, src_root: pathlib.Path):
    out = []
    for ln in rsp.read_text(encoding="utf-8").splitlines():
        ln = ln.strip().strip('"')
        if ln and not ln.startswith("#"):
            out.append(src_root / ln.replace("\\", "/"))
    return out


def include_map(root: pathlib.Path, src_root: pathlib.Path, rsp: pathlib.Path):
    """repo-relative re/match path -> [(tu, 'file:line' of the include that reaches it)].

    Follows quoted includes from each exe TU, recursing only into files under mashedmod/src (wrapper headers), not into
    re/match sources themselves. Preprocessor conditions are not evaluated: an include under #if is counted
    [UNCERTAIN]: no wrapper is known to guard its include this way.
    """
    mroot = (root / "re" / "match").resolve()
    found = {}
    cache = {}

    def scan(f: pathlib.Path):
        if f not in cache:
            hits = []
            try:
                text = f.read_text(encoding="utf-8", errors="replace").splitlines()
            except OSError:
                text = []
            for n, ln in enumerate(text, 1):
                m = RE_INCLUDE.match(ln)
                if m:
                    hits.append((n, (f.parent / m.group(1)).resolve()))
            cache[f] = hits
        return cache[f]

    for tu in exe_tus(rsp, src_root):
        if not tu.exists():
            continue
        seen, stack = set(), [tu.resolve()]
        while stack:
            f = stack.pop()
            if f in seen:
                continue
            seen.add(f)
            for n, tgt in scan(f):
                if mroot in tgt.parents:
                    found.setdefault(rel(tgt, root), []).append((rel(tu, root), f"{rel(f, root)}:{n}"))
                elif (root / "mashedmod" / "src").resolve() in tgt.parents and tgt.exists():
                    stack.append(tgt)
    return found


# ------------------------------------------------------------------ per-source checks
def check_source(src: pathlib.Path):
    """{rva: (pct, bytes_n, n_bad, reloc_status, reloc_detail)} or raises with a reason string."""
    import match
    import relocheck
    # quiet=True: compare prints nothing, so no stdout redirect (redirect_stdout is process-wide and races in threads)
    try:
        res = match.compare(src, quiet=True, bytes_mode=True)
    except SystemExit as e:
        raise RuntimeError(("match-error: " + str(e).splitlines()[0][:160]) if str(e) else "match-error")
    try:
        rl = relocheck.check(src)
    except SystemExit as e:
        rl = []
        rerr = str(e).splitlines()[0][:160] if str(e) else "error"
    else:
        rerr = ""
    rmap = {}
    for r in rl:
        if r[1] == "NOSYM":
            rmap[r[0]] = ("NOSYM", "")
        else:
            addr, st, bad, unch, n = r
            rmap[addr] = (st, f"checked={n} unchecked={len(unch)}" + (f" {'; '.join(bad)[:200]}" if bad else ""))
    out = {}
    for r in res:
        st, det = rmap.get(r["addr"], ("ERROR", rerr or "no relocheck line"))
        out[r["addr"]] = (100 * r["acc"], r["bytes_n"], len(r["bytes_bad"]), st, det)
    return out


def classify(rows, root, inc, admitted, checker=check_source, jobs=4):
    """Returns [(line_index, row, verdict, reason, evidence dict)]."""
    prelim, need = [], {}
    for li, r in rows:
        rva = int(r["rva"], 16)
        s = r.get("source", "")
        ev = {"rva": f"0x{rva:08x}", "source": s, "include": "", "toolchain": "", "flags": "", "match": "", "relocheck": ""}
        reason = ""
        if not re.fullmatch(r"re/match/[\w./-]+\.(c|cpp)", s):
            reason = "no-re-match-source"
        elif not (root / s).exists():
            reason = "source-missing"
        else:
            text = (root / s).read_text(encoding="utf-8", errors="replace")
            m = RE_TC.search(text)
            ev["toolchain"] = m.group(1) if m else "ddk"
            fm = re.search(r"//\s*FLAGS(?:\s+MASHED\.exe)?:\s*(.+)", text)
            ev["flags"] = fm.group(1).strip() if fm else "/O2 (match.py default)"
            hits = inc.get(s, [])
            ev["include"] = "; ".join(sorted({h[1] for h in hits}))
            if RE_NAKED.search(text):
                reason = "naked"
            elif ev["toolchain"] not in MATCH_TOOLCHAINS:
                reason = "toolchain"
            elif rva in admitted:
                reason = "admitted"
            elif not hits:
                reason = "not-included"
            need.setdefault(s, None)
        prelim.append([li, r, rva, reason, ev])

    # byte + relocheck evidence for every row with a re/match source that exists, kept or not, so the keep list shows
    # whether a not-included row is already byte-identical (0x00551a50)
    results = {}
    with cf.ThreadPoolExecutor(max_workers=max(1, jobs)) as ex:
        futs = {ex.submit(checker, root / s): s for s in need}
        for f in cf.as_completed(futs):
            s = futs[f]
            try:
                results[s] = f.result()
            except Exception as e:  # noqa: BLE001 - reason goes in the table
                results[s] = str(e) or "match-error"

    out = []
    for li, r, rva, reason, ev in prelim:
        s = r.get("source", "")
        if s in results:
            res = results[s]
            if isinstance(res, str):
                ev["match"] = res
                reason = reason or "match-error"
            elif rva not in res:
                ev["match"] = "no // MATCH: function at this RVA"
                reason = reason or "no-symbol"
            else:
                pct, n, nbad, st, det = res[rva]
                ev["match"] = f"{pct:.1f}%, {n} B, {nbad} differing bytes" + (" BYTE-IDENTICAL" if n and not nbad else "")
                ev["relocheck"] = f"{st} {det}".strip()
                if not reason:
                    if not n or nbad:
                        reason = "not-byte-identical"
                    elif st != "OK":
                        reason = "relocheck"
        out.append((li, r, "KEEP" if reason else "DROP", reason, ev))
    return out


CHECKER = check_source  # tests replace this with a fake


def apply_drop(path: pathlib.Path, drop_lines):
    parts = path.read_bytes().split(b"\n")  # same indices as read_tsv
    keep = [p for i, p in enumerate(parts) if i not in drop_lines]
    path.write_bytes(b"\n".join(keep))
    return len(parts) - len(keep)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--root", default=str(ROOT), help="repo root (tests point this at a fixture)")
    ap.add_argument("--tsv", default="", help="write the per-row evidence table here")
    ap.add_argument("--apply", action="store_true", help="rewrite PROVISIONAL.tsv without the DROP rows")
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--only", default="", help="comma-separated RVAs to examine (others are left alone)")
    a = ap.parse_args(argv)
    root = pathlib.Path(a.root).resolve()
    prov = root / "re" / "match" / "PROVISIONAL.tsv"
    header, rows = read_tsv(prov)
    if a.only:
        want = {int(x, 16) for x in a.only.split(",") if x}
        rows = [(li, r) for li, r in rows if int(r["rva"], 16) in want]
    inc = include_map(root, root / "mashedmod" / "src" / "mashed_re", root / "mashedmod" / "exe_sources.rsp")
    res = classify(rows, root, inc, admitted_rvas(root / "re" / "match" / "ADMITTED.tsv"), checker=CHECKER, jobs=a.jobs)
    res.sort(key=lambda x: (x[2] != "DROP", x[3], x[4]["rva"]))
    cols = ["verdict", "reason", "rva", "status", "source", "toolchain", "flags", "match", "relocheck", "include"]
    lines = ["\t".join(cols)]
    for li, r, v, why, ev in res:
        lines.append("\t".join([v, why or "-", ev["rva"], r.get("status", ""), ev["source"], ev["toolchain"] or "-",
                                ev["flags"] or "-", ev["match"] or "-", ev["relocheck"] or "-", ev["include"] or "-"]))
    for ln in lines:
        print(ln)
    nd = sum(1 for x in res if x[2] == "DROP")
    counts = {}
    for x in res:
        if x[2] == "KEEP":
            counts[x[3]] = counts.get(x[3], 0) + 1
    print(f"# rows examined {len(res)}: DROP {nd}, KEEP {len(res) - nd} "
          + " ".join(f"{k}={v}" for k, v in sorted(counts.items())), file=sys.stderr)
    if a.tsv:
        pathlib.Path(a.tsv).write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    if a.apply:
        n = apply_drop(prov, {x[0] for x in res if x[2] == "DROP"})
        print(f"# --apply: {n} rows removed from {prov}", file=sys.stderr)
    else:
        print("# dry run: PROVISIONAL.tsv not changed (pass --apply to drop the DROP rows)", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
