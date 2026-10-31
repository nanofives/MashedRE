#!/usr/bin/env python3
"""Library-identity tracker operations (re-classify skill: "CRT rows (L-67)" and "Library rows from libid").

  libid_reclass.py crt-waive  --tsv <libidentity tsv> --lib <libcmt.lib> --evidence <RESULT> [--date] [--dry-run]
  libid_reclass.py add-rows   --tsv <tsv> --library lua-4.0|qhull-2002.1|msvc-crt --evidence <RESULT>
                              [--lib <libcmt.lib>] [--not-c4 "<why>"] [--date] [--dry-run]
  libid_reclass.py refile-lib --tsv <tsv> --library lua-4.0|qhull-2002.1 --evidence <RESULT> [--date] [--dry-run]

The TSV is libmatch's output (verify/libidentity_20261030/libidentity_functions.tsv layout: library, rva, hooks_name,
subsystem, confidence, status, library_object, lib_function, lib_size, first_diff, category, note).

crt-waive  Every hooks.csv row in a `third-party-library[msvc-crt*` subsystem, plus every row at an address the TSV
           lists as a CRT `identical-unlisted` function (a CRT function filed under another subsystem): status becomes
           `waived-platform-crt`, the confidence cell is kept, notes start `identified, waived: platform CRT (L-67)`
           (identical or ambiguous, with member, `@comp.id` and the libmatch command) or `waived: platform CRT (L-67)`
           (anything else). A row under another subsystem is refiled to the CRT bucket of its address
           (0x004a0000..0x004b4000 -> msvc-crt-main, 0x005c0000..0x005d0000 -> msvc-crt-tail, else msvc-crt).
add-rows   Inserts a C1 row (appended to hooks.csv) for every TSV `identical-unlisted` address of that library that
           has no hooks.csv row. Lua / qhull rows get status `mapped` and need --not-c4 (why the row is not libid C4);
           CRT rows get `waived-platform-crt` and the L-67 notes. A name the identity does not determine (several
           functions with the same bytes, MASM local labels) becomes FUN_<rva>, with the candidates in notes.
refile-lib Moves a Lua / qhull row filed under a non-library subsystem to the library's subsystem when the TSV lists
           its address as `identical-unlisted` with one candidate. Confidence and status are kept.

Every operation prepends one CHANGELOG line per row below <!-- ENTRIES -->. Refuses: a missing evidence file, a
hooks.csv that does not round-trip through the csv module byte for byte, a CRT refile whose identity is not
identical / ambiguous, a Lua / qhull add without --not-c4. Idempotent: rows already waived, present or refiled are
skipped, so a second run writes nothing. Nothing is written on --dry-run or on a refusal.
"""
import argparse, csv, datetime, io, pathlib, re, sys
from collections import defaultdict

ROOT = pathlib.Path(__file__).resolve().parents[2]
HOOKS = ROOT / "hooks.csv"
CHANGELOG = ROOT / "re" / "analysis" / "CHANGELOG.md"
MARKER = "<!-- ENTRIES -->"
WAIVED = "waived-platform-crt"
CRT_PREFIX = "third-party-library[msvc-crt"
LIBMATCH_CMD = ("py -3.12 re/tools/match/libmatch.py --objs tools/vc7ddk/lib/wxp/i386/libcmt.lib "
                "--subsystem third-party-library[msvc-crt")
LIBS = {
    "lua-4.0": ("third-party-library[lua-4.0]", "Lua 4.0 /Og /Oi /Ot /Oy /Ob1 /GF /G5"),
    "qhull-2002.1": ("third-party-library[qhull-2002.1]", "qhull 2002.1 /Og /Oi /Ot /Oy /Ob2 /GF /G5"),
}
QHULL_CONT = (0x00591710, 0x0059b455)
COMPILER = "XP DDK cl 13.00.9176 / c2 13.00.9178"


def read_raw(path):
    with open(path, encoding="utf-8", newline="") as f:
        return f.read()


def write_raw(path, text):
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(text)


def load_hooks(raw):
    rows = list(csv.reader(io.StringIO(raw, newline="")))
    if dump_hooks(rows) != raw:
        sys.exit("REFUSED: hooks.csv does not round-trip through the csv module; nothing written")
    hi = next((i for i, r in enumerate(rows) if r and r[0] == "rva"), None)
    if hi is None:
        sys.exit("REFUSED: hooks.csv header row missing")
    col = {n: k for k, n in enumerate(rows[hi])}
    return rows, hi, col


def dump_hooks(rows):
    out = io.StringIO(newline="")
    csv.writer(out, lineterminator="\r\n").writerows(rows)
    return out.getvalue()


def data_rows(rows, hi):
    for i, r in enumerate(rows):
        if i > hi and len(r) > 4 and re.fullmatch(r"[0-9a-fA-F]{8}", r[0]):
            yield i, int(r[0], 16), r


def load_tsv(path):
    with open(path, encoding="utf-8", newline="") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def comp_ids(lib):
    sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
    import coffpe  # noqa: E402
    out = {}
    for name, data in coffpe.archive_members(pathlib.Path(lib).read_bytes()):
        try:
            o = coffpe.Obj(data, name)
        except Exception:
            continue
        if o.comp_id:
            out[name] = f"{o.comp_id[0]}/{o.comp_id[1]}"
    return out


def member(obj):
    return obj.split(":", 1)[1] if ":" in obj else obj


def crt_bucket(va):
    if 0x004a0000 <= va < 0x004b4000:
        return CRT_PREFIX + "-main]"
    if 0x005c0000 <= va < 0x005d0000:
        return CRT_PREFIX + "-tail]"
    return CRT_PREFIX + "]"


def lib_bucket(library, va):
    sub = LIBS[library][0]
    if library == "qhull-2002.1" and QHULL_CONT[0] <= va < QHULL_CONT[1]:
        return sub[:-1] + "-continuation]"
    return sub


def candidates(entries):
    """[(func, obj)] for a set of TSV entries at one address (ambiguous listed rows add the 'same bytes as' names)."""
    out = []
    for e in entries:
        out.append((e["lib_function"], e["library_object"]))
        m = re.search(r"same bytes as ([^;]+)", e.get("note", ""))
        if m:
            out += [(n.strip(), "") for n in m.group(1).split(",") if n.strip()]
    seen, uniq = set(), []
    for c in out:
        if c[0] not in seen:
            seen.add(c[0])
            uniq.append(c)
    return uniq


def crt_note(entries, status, cids, tsv):
    if status in ("identical", "identical-ambiguous", "identical-unlisted"):
        cs = candidates(entries)
        mems = sorted({member(e["library_object"]) for e in entries})
        ids = ", ".join(f"{m} @comp.id {cids.get(m, 'none')}" for m in mems)
        if len(cs) > 1:
            what = "libid identical-ambiguous, candidates " + " / ".join(c[0] for c in cs)
        else:
            what = f"libid identical {cs[0][0]}"
        return (f"identified, waived: platform CRT (L-67); {what} (libcmt.lib {ids}); "
                f"libmatch: {LIBMATCH_CMD} ({tsv})")
    e = entries[0] if entries else None
    if e and status == "differs":
        return (f"waived: platform CRT (L-67); libid differs vs libcmt.lib {member(e['library_object'])} "
                f"{e['lib_function']} at +{e['first_diff']} ({e['category']}) ({tsv})")
    return f"waived: platform CRT (L-67); libid {status or 'not in TSV'} ({tsv})"


def row_name(va, cs):
    if len(cs) == 1:
        n = cs[0][0]
        if n.startswith("_") and not n.startswith("_$") and "?" not in n and "@" not in n:
            return n[1:]
    return f"FUN_{va:08x}"


def prepend_changelog(ctext, entries):
    ceol = "\r\n" if "\r\n" in ctext else "\n"
    cl = ctext.split(ceol)
    idx = next((i for i, l in enumerate(cl) if l.strip() == MARKER), None)
    if idx is None:
        sys.exit("REFUSED: CHANGELOG marker line missing")
    cl[idx + 1:idx + 1] = entries
    return ceol.join(cl)


def op_crt_waive(rows, hi, col, tsv_rows, cids, tsv, date, ev):
    listed = {int(t["rva"], 16): t for t in tsv_rows if "crt" in t["library"] and t["status"] != "identical-unlisted"}
    unl = defaultdict(list)
    for t in tsv_rows:
        if "crt" in t["library"] and t["status"] == "identical-unlisted":
            unl[int(t["rva"], 16)].append(t)
    log, skipped = [], 0
    S, ST, N, C, NM = col["subsystem"], col["status"], col["notes"], col["confidence"], col["name"]
    for i, va, r in data_rows(rows, hi):
        sub = r[S]
        if sub.startswith(CRT_PREFIX):
            new_sub = sub
            ents = [listed[va]] if va in listed else []
            status = listed[va]["status"] if va in listed else ""
        elif va in unl:
            new_sub = crt_bucket(va)
            ents = unl[va]
            status = "identical-unlisted"
        else:
            continue
        if r[ST] == WAIVED:
            skipped += 1
            continue
        note = crt_note(ents, status, cids, tsv)
        r[ST] = WAIVED
        r[N] = note + (" | " + r[N] if r[N] else "")
        moved = ""
        if new_sub != sub:
            r[S] = new_sub
            moved = f", refiled {sub}->{new_sub}"
        log.append(f"{date}  {va:08x}  {r[NM]}  {r[C]}->{r[C]}  {WAIVED} (L-67){moved} evidence: {ev}")
    return log, skipped


def op_add_rows(rows, hi, col, tsv_rows, library, cids, tsv, date, ev, not_c4):
    have = {va for _, va, _ in data_rows(rows, hi)}
    key = "crt" if library == "msvc-crt" else library
    unl = defaultdict(list)
    for t in tsv_rows:
        if key in t["library"] and t["status"] == "identical-unlisted" and int(t["rva"], 16) not in have:
            unl[int(t["rva"], 16)].append(t)
    log = []
    width = len(rows[hi])
    for va in sorted(unl):
        ents = unl[va]
        cs = candidates(ents)
        name = row_name(va, cs)
        r = [""] * width
        r[col["rva"]] = f"{va:08x}"
        r[col["name"]] = name
        r[col["confidence"]] = "C1"
        if library == "msvc-crt":
            r[col["subsystem"]] = crt_bucket(va)
            r[col["status"]] = WAIVED
            r[col["notes"]] = crt_note(ents, "identical-unlisted", cids, tsv) + " | C1 by libid, row added MATCH-LIB"
            tail = f"added by libid, {WAIVED} (L-67)"
        else:
            r[col["subsystem"]] = lib_bucket(library, va)
            r[col["status"]] = "mapped"
            objs = sorted({e["library_object"] for e in ents})
            if len(cs) > 1:
                what = f"identical-ambiguous {' / '.join(c[0] for c in cs)} in {', '.join(objs)}; name not determined"
            else:
                what = f"identical {objs[0]} {cs[0][0]}"
            r[col["notes"]] = (f"C1 by libid: {what} ({LIBS[library][1]}, {COMPILER}; {tsv}); row added MATCH-LIB; "
                               f"not C4: {not_c4}")
            tail = f"added by libid ({library})"
        rows.append(r)
        log.append(f"{date}  {va:08x}  {name}  (none)->C1  {tail} evidence: {ev}")
    return log


def op_refile_lib(rows, hi, col, tsv_rows, library, tsv, date, ev):
    unl = defaultdict(list)
    for t in tsv_rows:
        if library in t["library"] and t["status"] == "identical-unlisted":
            unl[int(t["rva"], 16)].append(t)
    log, skipped = [], 0
    S, N, C, NM = col["subsystem"], col["notes"], col["confidence"], col["name"]
    for i, va, r in data_rows(rows, hi):
        if va not in unl or r[S].startswith("third-party-library["):
            continue
        cs = candidates(unl[va])
        if len(cs) != 1:
            skipped += 1
            continue
        old, new = r[S], lib_bucket(library, va)
        e = unl[va][0]
        r[S] = new
        r[N] = (f"libid identical {e['library_object']} {e['lib_function']} ({LIBS[library][1]}, {COMPILER}; {tsv}); "
                f"refiled {old}->{new} MATCH-LIB" + (" | " + r[N] if r[N] else ""))
        log.append(f"{date}  {va:08x}  {r[NM]}  {r[C]}->{r[C]}  refiled {old}->{new} by libid evidence: {ev}")
    return log, skipped


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("op", choices=["crt-waive", "add-rows", "refile-lib"])
    ap.add_argument("--tsv", required=True)
    ap.add_argument("--evidence", required=True)
    ap.add_argument("--library", choices=["lua-4.0", "qhull-2002.1", "msvc-crt"])
    ap.add_argument("--lib", help="libcmt.lib, for each member's @comp.id (CRT notes)")
    ap.add_argument("--not-c4", default="")
    ap.add_argument("--date", default=datetime.date.today().isoformat())
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()

    ev = a.evidence.strip().replace("\\", "/")
    tsv = a.tsv.strip().replace("\\", "/")
    if not (ROOT / ev).is_file():
        sys.exit(f"REFUSED: evidence file not found: {ev}")
    if not (ROOT / tsv).is_file():
        sys.exit(f"REFUSED: TSV not found: {tsv}")
    crt = a.op == "crt-waive" or a.library == "msvc-crt"
    if a.op != "crt-waive" and not a.library:
        sys.exit("REFUSED: --library is required")
    if a.op == "refile-lib" and a.library == "msvc-crt":
        sys.exit("REFUSED: CRT rows are refiled by crt-waive")
    if a.op == "add-rows" and not crt and not a.not_c4.strip():
        sys.exit("REFUSED: a Lua / qhull add needs --not-c4 (why the row is not libid C4)")
    if crt and not (a.lib and pathlib.Path(a.lib).is_file()):
        sys.exit("REFUSED: CRT notes need --lib <libcmt.lib> for @comp.id provenance")
    for c in (a.not_c4,):
        if "\n" in c or "\r" in c:
            sys.exit("REFUSED: a cell argument contains a newline")

    rows, hi, col = load_hooks(read_raw(HOOKS))
    tsv_rows = load_tsv(ROOT / tsv)
    cids = comp_ids(a.lib) if crt else {}
    skipped = 0
    if a.op == "crt-waive":
        log, skipped = op_crt_waive(rows, hi, col, tsv_rows, cids, tsv, a.date, ev)
    elif a.op == "add-rows":
        log = op_add_rows(rows, hi, col, tsv_rows, a.library, cids, tsv, a.date, ev, a.not_c4.strip())
    else:
        log, skipped = op_refile_lib(rows, hi, col, tsv_rows, a.library, tsv, a.date, ev)
    refiled = sum(1 for l in log if "refiled" in l)
    print(f"{a.op}: {len(log)} row(s) changed ({refiled} refiled), {skipped} skipped")
    if a.dry_run or not log:
        return log
    write_raw(HOOKS, dump_hooks(rows))
    write_raw(CHANGELOG, prepend_changelog(read_raw(CHANGELOG), log))
    return log


if __name__ == "__main__":
    main()
