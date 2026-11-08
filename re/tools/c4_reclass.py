#!/usr/bin/env python3
"""C4 by the static routes, batch mode (re-classify skill, "C4 by match or libid (L-93)").

L-93 (Mariano, 2026-11-07): C4 certifies the reverse engineering, not the port. A row is C4 by route `match` when its
reconstructed source is byte-identical under re/tools/match/match.py with the recorded compiler (XP DDK cl 13.00.9176 /
c2 13.00.9178) and the source's recorded flags, relocheck.py reports OK, and the source is not a naked inline-asm
transcription nor a body that is only inline __asm inside a C frame (SimGolf G26). A row is C4 by route `libid` when
the function is identical to one vendored library function (libmatch). Body in the exe, exe callers and gates are the
port axis (hooks.csv port column, C4R-PORTCOL), not part of this gate. Rubric: re/CONFIDENCE.md, "C4".

  c4_reclass.py batch    --tsv <batch.tsv> [--result <RESULT.md>] [--apply] [--strict] [--jobs N] [--report <out.tsv>]
  c4_reclass.py make-tsv --results <slice results.tsv> [...] --out <batch.tsv>

batch     Reads a TSV with columns `rva`, `route`, `evidence` (header required; extra columns ignored):
            route match   evidence = the matched source (.c, repo-relative, carrying `// MATCH: 0x<rva> <sym>`).
                          The gate recompiles it now (match.compare, --bytes) and runs relocheck on it, so the evidence
                          is the source as it is in the tree, not a score recorded earlier.
            route libid   evidence = a libmatch TSV (libidentity_functions.tsv layout: rva, status, library_object,
                          lib_function, ...). The row must be `identical` / `identical-unlisted` with one candidate.
          Every row is gated on its own; a refused row is reported with the failing criterion and skipped. Rows
          already at C4 are skipped (no write). Dry run by default: prints the verdict per row and the CHANGELOG lines
          it would write. --apply writes hooks.csv (confidence C4, notes appended with `c4=match` / `c4=libid` and
          the evidence) and prepends one CHANGELOG line per promoted row below <!-- ENTRIES -->, all in one write per
          file. --apply needs --result (the RESULT the CHANGELOG cites). --strict writes nothing if any row is refused.
make-tsv  Turns MATCH slice results.tsv files into a batch TSV: every row whose recorded score is 100 becomes
          `rva  match  <source>`. The batch gate still recompiles each one.

Line numbering: `[N/M] 0x<rva> <route> PASS|REFUSED|SKIP <detail>`.
"""
import argparse, csv, datetime, hashlib, io, pathlib, re, sys
from concurrent.futures import ProcessPoolExecutor

ROOT = pathlib.Path(__file__).resolve().parents[2]
HOOKS = ROOT / "hooks.csv"
CHANGELOG = ROOT / "re" / "analysis" / "CHANGELOG.md"
MATCHDIR = pathlib.Path(__file__).resolve().parent / "match"
MARKER = "<!-- ENTRIES -->"
COMPILER = "XP DDK cl 13.00.9176 / c2 13.00.9178"
LIB_FLAGS = {   # recorded build of each vendored library (libid_reclass.py LIBS / PNGZLIB, RESULT_LIB-IDENTITY)
    "lua-4.0": "Lua 4.0 /Og /Oi /Ot /Oy /Ob1 /GF /G5",
    "qhull-2002.1": "qhull 2002.1 /Og /Oi /Ot /Oy /Ob2 /GF /G5",
    "libpng-1.0.12": "libpng 1.0.12 /O2 /G5",
    "zlib-1.1.3": "zlib 1.1.3 /O2 /G5",
    "msvc-crt": "DDK libcmt.lib member",
    "d3dx9-sum2003": "DirectX 9.0b SDK Summer 2003 d3dx9.lib member",
    "d3dx9-sum2004": "DirectX 9.0c SDK Summer 2004 d3dx9.lib member",
    "directshow-baseclasses-ddk": "XP DDK SP1 strmbase.lib member",
    # C4R-DSHOW-BASECLASSES: DX 9.0b SDK BaseClasses source, its baseclasses.dsp Release line + /Ob1 (159/159 rows)
    "directshow-baseclasses-sum2003": ("DirectShow BaseClasses source (DirectX 9.0b SDK Summer 2003) /Gz /MT /GX /O2 /Ob1 "
                                       "-D NDEBUG WIN32 _MBCS _LIB _WIN32_DCOM WINVER=0x400"),
}
LIBID_OK = {"identical", "identical-unlisted"}


# ---------------------------------------------------------------- tracker I/O (same rules as libid_reclass.py)
def read_raw(path):
    with open(path, encoding="utf-8", newline="") as f:
        return f.read()


def write_raw(path, text):
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(text)


def load_hooks(raw):
    """Line-based view of hooks.csv: only the lines this run promotes are re-serialized, so a file that does not
    round-trip through the csv module as a whole (an over-quoted cell on some other row) is still safe to edit."""
    eol = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(eol)
    hi = next((i for i, l in enumerate(lines) if l.startswith("rva,")), None)
    if hi is None:
        sys.exit("REFUSED: hooks.csv header row missing")
    col = {n: k for k, n in enumerate(next(csv.reader([lines[hi]])))}
    index = {}
    for i, l in enumerate(lines[hi + 1:], hi + 1):
        k = l.split(",", 1)[0].lower()
        if re.fullmatch(r"[0-9a-f]{8}", k):
            if k in index:
                sys.exit(f"REFUSED: hooks.csv has two rows for {k}; nothing written")
            index[k] = i
    return lines, col, index, eol


def parse_line(line, col):
    row = next(csv.reader([line]))
    if len(row) < len(col):
        row += [""] * (len(col) - len(row))
    return row


def dump_line(row):
    buf = io.StringIO()
    csv.writer(buf, lineterminator="").writerow(row)
    return buf.getvalue()


def prepend_changelog(ctext, entries):
    ceol = "\r\n" if "\r\n" in ctext else "\n"
    cl = ctext.split(ceol)
    idx = next((i for i, l in enumerate(cl) if l.strip() == MARKER), None)
    if idx is None:
        sys.exit("REFUSED: CHANGELOG marker line missing")
    cl[idx + 1:idx + 1] = entries
    return ceol.join(cl)


def load_tsv(path):
    with open(path, encoding="utf-8", newline="") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def norm_rva(s):
    s = s.strip().lower()
    s = s[2:] if s.startswith("0x") else s
    if not re.fullmatch(r"[0-9a-f]{1,8}", s):
        raise ValueError(f"bad rva {s!r}")
    return f"{int(s, 16):08x}"


# ---------------------------------------------------------------- source checks (static, no compiler)
def strip_comments(t):
    return re.sub(r"//[^\n]*|/\*.*?\*/", " ", t, flags=re.S)


def brace_block(t, i):
    """t[i] == '{'; return the index just past the matching '}' (or len(t))."""
    d = 0
    for k in range(i, len(t)):
        if t[k] == "{":
            d += 1
        elif t[k] == "}":
            d -= 1
            if d == 0:
                return k + 1
    return len(t)


def function_text(text, rva):
    """(header, body) of the definition that follows `// MATCH: 0x<rva>`; None when there is no such annotation."""
    m = re.search(r"//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x0*" + rva.lstrip("0") + r"\b[^\n]*\n", text, re.I)
    if not m:
        return None
    rest = text[m.end():]
    clean = strip_comments(rest)
    i = clean.find("{")
    if i < 0:
        return None
    return clean[:i], clean[i:brace_block(clean, i)]


DECL = re.compile(r"(?:(?:const|volatile|unsigned|signed|static|register|struct|union|enum)\s+)*[A-Za-z_]\w*"
                  r"(?:\s*\*+\s*|\s+)[A-Za-z_]\w*(?:\s*\[[^\]]*\])?")


def asm_verdict(header, body):
    """None when the body is compiled C (inline __asm inside real C work is allowed), else the refusal reason."""
    if re.search(r"\bnaked\b", header):
        return "naked function (__declspec(naked)): an inline-asm transcription matches by construction"
    inner = body[1:-1]
    if not re.search(r"\b_{1,2}asm\b", inner):
        return None
    rest, n = inner, 0
    while True:
        m = re.search(r"\b_{1,2}asm\b\s*\{", rest)
        if not m:
            break
        j = brace_block(rest, m.end() - 1)
        rest, n = rest[:m.start()] + " ; " + rest[j:], n + 1
    rest, k = re.subn(r"\b_{1,2}asm\b[^\n]*", " ; ", rest)
    n += k
    work = []
    for st in (s.strip() for s in rest.split(";")):
        if not st or DECL.fullmatch(st) or re.fullmatch(r"return(?:\s+[A-Za-z_]\w*|\s+-?\d+)?", st):
            continue
        work.append(st)
    if not work:
        return (f"body is {n} inline __asm block(s) in a C frame with no other C statement "
                f"(SimGolf G26 rule: not match evidence)")
    return None


# ---------------------------------------------------------------- compiler evaluation (one process per source)
def eval_source(src_abs):
    """Recompile one source: {addr_hex8: {...}} from match.compare (bytes mode) and relocheck.check."""
    sys.path.insert(0, str(MATCHDIR))
    import match, relocheck  # noqa: E402
    p = pathlib.Path(src_abs)
    text = p.read_text()
    out = {"_toolchain": match.file_toolchain(text), "_flags": match.file_flags(text) or match.DEFAULT_FLAGS,
           "_lang": match.file_lang(text)}
    try:
        for r in match.compare(p, quiet=True, bytes_mode=True):
            out[f"{r['addr']:08x}"] = {"acc": r["acc"], "n_mine": r["n_mine"], "n_orig": r["n_orig"],
                                       "bytes_n": r["bytes_n"], "bytes_bad": len(r["bytes_bad"]),
                                       "flags": r["flags"], "toolchain": r["toolchain"]}
        for r in relocheck.check(p):
            d = out.setdefault(f"{r[0]:08x}", {})
            if r[1] == "NOSYM":
                d["reloc"] = ("NOSYM", [], [], 0)
            else:
                d["reloc"] = (r[1], list(r[2]), sorted(set(r[3])), r[4])
    except BaseException as e:  # match.py / relocheck exit via SystemExit on a compile error
        out["_error"] = str(e).splitlines()[0][:160] if str(e) else type(e).__name__
    return out


def evaluate(sources, jobs):
    if jobs <= 1 or len(sources) <= 1:
        return {s: eval_source(str(ROOT / s)) for s in sources}
    with ProcessPoolExecutor(max_workers=jobs) as ex:
        return dict(zip(sources, ex.map(eval_source, [str(ROOT / s) for s in sources])))


# ---------------------------------------------------------------- gates
def gate_match(rva, src, ev):
    """(ok, detail, note_fragment) for route match. ev = eval_source() result for src."""
    path = ROOT / src
    if not path.is_file():
        return False, f"source {src} not found", None
    text = path.read_text()
    ft = function_text(text, rva)
    if ft is None:
        return False, f"no `// MATCH: 0x{rva}` definition in {src}", None
    header, body = ft
    bad = asm_verdict(header, body)
    if bad:
        return False, bad, None
    if "STUB" in body:
        return False, "STUB marker in the function body", None
    if ev.get("_error"):
        return False, f"match/relocheck failed: {ev['_error']}", None
    if ev.get("_toolchain") != "ddk":
        return False, f"toolchain {ev.get('_toolchain')} is not the recorded compiler (ddk = {COMPILER})", None
    r = ev.get(rva)
    if not r or "acc" not in r:
        return False, f"match.py produced no result for 0x{rva} (symbol not in the obj)", None
    if r["bytes_bad"]:
        return False, (f"not byte-identical: {r['bytes_bad']} differing unmasked bytes of {r['bytes_n']} "
                       f"(insn {100 * r['acc']:.1f}%)"), None
    if not (r["acc"] == 1 or r["n_mine"] > r["n_orig"]):
        return False, (f"whole symbol bytes equal but shorter than the original body "
                       f"({r['n_mine']}/{r['n_orig']} insns, {100 * r['acc']:.1f}%)"), None
    rc = r.get("reloc")
    if not rc:
        return False, "relocheck produced no line for this function", None
    st, rbad, unch, n = rc
    if st != "OK":
        return False, f"relocheck {st}" + (f": {'; '.join(rbad)[:200]}" if rbad else ""), None
    sha = hashlib.sha256(path.read_bytes()).hexdigest()[:16]
    how = "insn 100.0%" if r["acc"] == 1 else f"insn {100 * r['acc']:.1f}% (Ghidra body short, {r['n_mine']}/{r['n_orig']})"
    note = (f"match.py {src} --bytes BYTE-IDENTICAL {r['bytes_n']} B, {how} ({COMPILER}, {r['flags']}, "
            f"{ev.get('_lang', 'c')}); relocheck OK checked={n} unchecked={len(unch)}"
            + (f" [{','.join(unch)}]" if unch else "") + f"; src sha256 {sha}")
    return True, f"byte-identical {r['bytes_n']} B, relocheck OK ({n}/{len(unch)})", note


def gate_libid(rva, tsv_rel, cache):
    path = ROOT / tsv_rel
    if not path.is_file():
        return False, f"libmatch TSV {tsv_rel} not found", None
    if tsv_rel not in cache:
        cache[tsv_rel] = load_tsv(path)
    rows = [r for r in cache[tsv_rel] if r.get("rva") and norm_rva(r["rva"]) == rva]
    if not rows:
        return False, f"0x{rva} not in {tsv_rel}", None
    st = {r.get("status", "") for r in rows}
    ok = [r for r in rows if r.get("status") in LIBID_OK]
    if not ok:
        return False, f"libmatch status {','.join(sorted(st))} (needs identical, one candidate)", None
    names = sorted({r.get("lib_function", "") for r in ok})
    if len(names) != 1 or "identical-ambiguous" in st:
        return False, f"ambiguous identity: {','.join(names)} (several library functions with these bytes)", None
    r = ok[0]
    lib = r.get("library", "") or next((k for k in LIB_FLAGS if k.split("-")[0] in tsv_rel.lower()), "")
    key = next((k for k in LIB_FLAGS if lib.startswith(k)), None)
    flags = LIB_FLAGS.get(key, f"library {lib or '?'}")
    note = (f"libmatch {tsv_rel}: {r['status']} {r.get('library_object', '')} {r.get('lib_function', '')} "
            f"({r.get('lib_size', '?')} B; {flags}" + ("" if flags.startswith("DirectX") else f"; {COMPILER}") + ")")
    return True, f"{r['status']} {r.get('lib_function', '')}", note


# ---------------------------------------------------------------- batch
def cmd_batch(a):
    if a.apply and not a.result:
        sys.exit("REFUSED: --apply needs --result <RESULT.md> (the CHANGELOG evidence column)")
    if a.result and not (ROOT / a.result).is_file():
        sys.exit(f"REFUSED: --result {a.result} does not exist")
    batch = load_tsv(pathlib.Path(a.tsv))
    if not batch or not {"rva", "route", "evidence"} <= set(batch[0]):
        sys.exit("REFUSED: batch TSV needs a header with columns rva, route, evidence")
    raw = read_raw(HOOKS)
    lines, col, index, eol = load_hooks(raw)
    sources = sorted({b["evidence"].strip().replace("\\", "/") for b in batch if b["route"].strip() == "match"})
    evals = evaluate(sources, a.jobs) if sources else {}
    date = a.date or datetime.date.today().isoformat()
    cache, log, report, seen = {}, [], [], set()
    npass = nref = nskip = 0
    total = len(batch)
    for k, b in enumerate(batch, 1):
        route, evid = b["route"].strip(), b["evidence"].strip().replace("\\", "/")
        try:
            rva = norm_rva(b["rva"])
        except ValueError as e:
            ok, detail, note, rva = False, str(e), None, b["rva"]
        else:
            ri = index.get(rva)
            if rva in seen:
                ok, detail, note = False, "duplicate rva in the batch", None
            elif ri is None:
                ok, detail, note = False, "no hooks.csv row (add it first: C1 by the identity)", None
            elif parse_line(lines[ri], col)[col["confidence"]] == "C4":
                ok, detail, note = None, "already C4 (no write)", None
            elif route == "match":
                ok, detail, note = gate_match(rva, evid, evals.get(evid, {}))
            elif route == "libid":
                ok, detail, note = gate_libid(rva, evid, cache)
            else:
                ok, detail, note = False, f"unknown route {route!r} (match | libid)", None
        seen.add(rva)
        verdict = "PASS" if ok else ("SKIP" if ok is None else "REFUSED")
        print(f"[{k}/{total}] 0x{rva} {route} {verdict} {detail}", flush=True)
        report.append((f"0x{rva}", route, evid, verdict, detail))
        if ok is None:
            nskip += 1
            continue
        if not ok:
            nref += 1
            continue
        npass += 1
        r = parse_line(lines[index[rva]], col)
        old = r[col["confidence"]] or "C0"
        res = a.result or "<RESULT>"
        frag = f"{date} C4 by route {route} (L-93): c4={route}; {note}; {res}"
        r[col["confidence"]] = "C4"
        r[col["notes"]] = (r[col["notes"]] + " | " if r[col["notes"]] else "") + frag
        lines[index[rva]] = dump_line(r)
        why = ("byte-identical + relocheck OK" if route == "match" else "library identity") + f", {evid}"
        log.append(f"{date}  0x{rva}  {r[col['name']]}  {old}->C4  route {route} (L-93): {why}; evidence: {res}")
    print(f"batch: {npass} pass, {nref} refused, {nskip} skipped (already C4), {total} rows")
    if a.report:
        with open(a.report, "w", encoding="utf-8", newline="") as f:
            w = csv.writer(f, delimiter="\t", lineterminator="\n")
            w.writerow(["rva", "route", "evidence", "verdict", "detail"])
            w.writerows(report)
    if not a.apply:
        print("dry run: nothing written. CHANGELOG lines that --apply would prepend:")
        for l in log:
            print("  " + l)
        return 0
    if a.strict and nref:
        print(f"--strict: {nref} refused rows, nothing written")
        return 1
    if not log:
        print("nothing to write")
        return 0
    write_raw(HOOKS, eol.join(lines))
    write_raw(CHANGELOG, prepend_changelog(read_raw(CHANGELOG), log))
    print(f"wrote hooks.csv ({npass} rows -> C4) and {len(log)} CHANGELOG lines")
    return 0


def cmd_make_tsv(a):
    out = []
    for res in a.results:
        for r in load_tsv(pathlib.Path(res)):
            pct = next((r[c] for c in ("best%", "pct", "exact_pct") if r.get(c)), "")
            src = next((r[c] for c in ("source", "file") if r.get(c)), "")
            try:
                ok = float(pct) >= 100.0
            except ValueError:
                ok = False
            if ok and src and r.get("rva"):
                out.append((f"0x{norm_rva(r['rva'])}", "match", src.replace("\\", "/")))
    with open(a.out, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["rva", "route", "evidence"])
        w.writerows(out)
    print(f"{len(out)} rows -> {a.out}")
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("batch")
    b.add_argument("--tsv", required=True)
    b.add_argument("--result")
    b.add_argument("--apply", action="store_true")
    b.add_argument("--strict", action="store_true")
    b.add_argument("--jobs", type=int, default=4)
    b.add_argument("--report")
    b.add_argument("--date")
    m = sub.add_parser("make-tsv")
    m.add_argument("--results", nargs="+", required=True)
    m.add_argument("--out", required=True)
    a = ap.parse_args(argv)
    return cmd_batch(a) if a.cmd == "batch" else cmd_make_tsv(a)


if __name__ == "__main__":
    sys.exit(main())
