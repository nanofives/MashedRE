"""C4R-DXLIB-FU tracker transaction (2026-11-07): C4R-DXLIB-REFILE, C4R-INITTIME, C4R-DXLIB-NEWROWS (row adds only;
the libid C4 of the added rows goes through re/tools/c4_reclass.py batch afterwards).

  py -3.12 verify/c4rdxlibfu_20261107/apply.py [--apply]

Line-based on hooks.csv (only the touched lines are re-serialized, new rows are appended), because hooks.csv does not
round-trip through the csv module as a whole (row 0041d820), so re/tools/libid_reclass.py refuses it. Same note
formats as libid_reclass.py refile-lib / crt-waive / add-rows. Idempotent: a row already refiled / waived / present
is skipped. Dry run by default.
"""
import csv, io, pathlib, re, sys

W = pathlib.Path(__file__).resolve().parents[2]
HERE = pathlib.Path(__file__).resolve().parent
REL = "verify/c4rdxlibfu_20261107"
RESULT = f"{REL}/RESULT_C4R-DXLIB-FU.md"
TSV = "verify/c4rdxlib_20261107/d3dx_sum2003.tsv"
DX = "DirectX 9.0b SDK Summer 2003 d3dx9.lib member"
DATE = "2026-11-07"
MARKER = "<!-- ENTRIES -->"
PSGP = "third-party-library[d3dx9-psgp]"
REFILE = ["004fb5a0", "004ec839", "004ec915", "004ec9cd", "004ecaab", "004ecb97", "004ecc6f", "004f022d", "004f0307"]
CRT_RVA = "005c2a0e"
CRT_SUB = "third-party-library[msvc-crt-tail]"
CRT_NOTE = ("identified, waived: platform CRT (L-67); libid identical ___inittime (libcmt.lib obj\\i386\\clock.obj "
            "@comp.id 28/9178); libmatch: py -3.12 re/tools/match/libmatch.py --objs tools/vc7ddk/lib/wxp/i386/libcmt.lib "
            "--subsystem third-party-library[msvc-crt (verify/c4rlibid_20261107/crt_libcmt.tsv)")


def rd(p):
    with open(p, encoding="utf-8", newline="") as f:
        return f.read()


def wr(p, t):
    with open(p, "w", encoding="utf-8", newline="") as f:
        f.write(t)


def parse(line, n):
    r = next(csv.reader([line]))
    return r + [""] * (n - len(r))


def dump(r):
    b = io.StringIO()
    csv.writer(b, lineterminator="").writerow(r)
    return b.getvalue()


def short_name(sym):
    """Library symbol -> row name: `?fn@@...` -> fn, `?fn@Cls@@...` -> Cls::fn, `_fn@12` -> fn."""
    m = re.fullmatch(r"\?([A-Za-z_]\w*)@(?:([A-Za-z_]\w*)@)?@.*", sym)
    if m:
        return f"{m.group(2)}::{m.group(1)}" if m.group(2) else m.group(1)
    m = re.fullmatch(r"_([A-Za-z_]\w*)@\d+", sym)
    if m:
        return m.group(1)
    sys.exit(f"REFUSED: no name rule for symbol {sym!r}")


def main():
    do = "--apply" in sys.argv
    raw = rd(W / "hooks.csv")
    eol = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(eol)
    trail = lines[-1] == ""
    if trail:
        lines.pop()
    hi = next(i for i, l in enumerate(lines) if l.startswith("rva,"))
    hdr = next(csv.reader([lines[hi]]))
    col = {n: k for k, n in enumerate(hdr)}
    idx = {}
    for i, l in enumerate(lines[hi + 1:], hi + 1):
        k = l.split(",", 1)[0].lower()
        if re.fullmatch(r"[0-9a-f]{8}", k):
            idx[k] = i
    tsv = list(csv.DictReader(open(W / TSV, encoding="utf-8"), delimiter="\t"))
    unl = {}
    for t in tsv:
        if t["status"] == "identical-unlisted":
            unl.setdefault(f"{int(t['rva'], 16):08x}", []).append(t)
    S, N, C, NM, ST = col["subsystem"], col["notes"], col["confidence"], col["name"], col["status"]
    log, out = [], []

    # 1. C4R-DXLIB-REFILE
    for k in REFILE:
        r = parse(lines[idx[k]], len(hdr))
        ents = unl.get(k, [])
        if len({e["lib_function"] for e in ents}) != 1:
            sys.exit(f"REFUSED: {k} is not identical-unlisted with one candidate in {TSV}")
        e = ents[0]
        obj_tags = {t["subsystem"] for t in tsv if t["library_object"] == e["library_object"] and t["status"] == "identical"
                    and t["subsystem"].startswith("third-party-library[d3dx9")}
        if obj_tags != {PSGP}:
            sys.exit(f"REFUSED: {k} object {e['library_object']} sub-tags {obj_tags}")
        if r[S] == PSGP:
            out.append(f"refile {k} SKIP already {PSGP}")
            continue
        if r[S] != "render":
            sys.exit(f"REFUSED: {k} subsystem {r[S]!r}, expected render")
        r[S] = PSGP
        r[N] = (f"libid identical {e['library_object']} {e['lib_function']} ({e['lib_size']} B; {DX}; {TSV}); "
                f"refiled render->{PSGP} C4R-DXLIB-REFILE {DATE}, U-9135 correction (D3DX body, not a D3DX client)"
                + (" | " + r[N] if r[N] else ""))
        lines[idx[k]] = dump(r)
        log.append(f"{DATE}  {k}  {r[NM]}  {r[C]}->{r[C]}  refiled render->{PSGP} by libid "
                   f"({short_name(e['lib_function'])}, d3dx9 9.0b Summer 2003) evidence: {RESULT}")
        out.append(f"refile {k} {r[NM]} {r[C]} -> {PSGP} ({e['lib_function']})")

    # 2. C4R-INITTIME
    r = parse(lines[idx[CRT_RVA]], len(hdr))
    if r[S] == CRT_SUB and r[ST] == "waived-platform-crt":
        out.append(f"inittime {CRT_RVA} SKIP already {CRT_SUB}")
    else:
        old = r[S]
        if old != "boot" or r[NM] != "___inittime":
            sys.exit(f"REFUSED: {CRT_RVA} is {r[NM]} / {old}")
        r[S], r[ST] = CRT_SUB, "waived-platform-crt"
        r[N] = CRT_NOTE + f" | refiled {old}->{CRT_SUB} C4R-INITTIME {DATE} | " + r[N]
        lines[idx[CRT_RVA]] = dump(r)
        log.append(f"{DATE}  {CRT_RVA}  ___inittime  {r[C]}->{r[C]}  waived-platform-crt (L-67), refiled {old}->{CRT_SUB} "
                   f"evidence: {RESULT}")
        out.append(f"inittime {CRT_RVA} {old} -> {CRT_SUB}, waived-platform-crt, {r[C]} kept")

    # 3. C4R-DXLIB-NEWROWS (adds; libid C4 by c4_reclass.py batch afterwards)
    cand = list(csv.DictReader(open(HERE / "candidates.tsv", encoding="utf-8"), delimiter="\t"))
    added = []
    for c in cand:
        if c["verdict"] != "add":
            continue
        k = c["rva"][2:]
        if k in idx:
            out.append(f"add {k} SKIP row present")
            continue
        name = short_name(c["lib_function"])
        r = [""] * len(hdr)
        r[col["rva"]] = k
        r[NM] = name
        r[S] = c["subsystem"]
        r[C] = "C1"
        r[ST] = "new"
        r[N] = (f"C1 by libid: identical {c['library_object']} {c['lib_function']} ({c['lib_size']} B; {DX}; {TSV}); "
                f"function boundary re/console/pc_funcs.csv {c['pc_name']} {c['pc_body_size']} B; sub-tag from the "
                f"object's identical d3dx9 rows; row added C4R-DXLIB-NEWROWS {DATE}; not C4: pending c4_reclass batch")
        if "port" in col:
            r[col["port"]] = "none"
        lines.append(dump(r))
        added.append(k)
        log.append(f"{DATE}  {k}  {name}  (none)->C1  added by libid (d3dx9 9.0b Summer 2003, {c['subsystem']}) "
                   f"evidence: {RESULT}")
        out.append(f"add {k} {name} {c['subsystem']}")

    # 4. U-9135 correction note
    up = W / "UNCERTAINTIES.md"
    ut = rd(up)
    tag = "**CORRECTION 2026-11-07 (C4R-DXLIB-REFILE):**"
    anchor = "| U-9135 | structural | ~~OPEN~~ **RESOLVED 2026-09-12**"
    if tag in ut:
        out.append("U-9135 SKIP correction present")
    else:
        if ut.count(anchor) != 1:
            sys.exit("REFUSED: U-9135 anchor not found exactly once")
        note = (f" {tag} the per-row `render` keep is wrong for 9 rows. Library identity against DirectX 9.0b SDK "
                f"Summer 2003 `d3dx9.lib` (C4R-DXLIB, `{TSV}`) shows `0x004fb5a0` is byte-identical to `psgpmesh.obj` "
                f"`c_D3DXPSGPUpdateSkinnedMesh` (1034 B, one candidate). This row's per-row note called it a CALLER "
                f"of PSGP bodies, a client of D3DX: the bytes show it is the D3DX function itself, which calls other "
                f"PSGP bodies (`0x004ecf29`, `0x004ecf53`). 8 more `render` rows are byte-identical to `d3dxmath.obj` "
                f"functions: `0x004ec839` `c_D3DXFloat16To32Array`, `0x004ec915` `c_D3DXVec2Hermite`, `0x004ec9cd` "
                f"`c_D3DXVec2CatmullRom`, `0x004ecaab` `c_D3DXVec2BaryCentric`, `0x004ecb97` `c_D3DXVec3Hermite`, "
                f"`0x004ecc6f` `c_D3DXVec3CatmullRom`, `0x004f022d` `c_D3DXMatrixShadow`, `0x004f0307` "
                f"`c_D3DXMatrixReflect`. All 9 refiled `render` -> `{PSGP}`, confidence kept (`{RESULT}`).")
        ut = ut.replace(anchor, anchor + note, 1)
        out.append("U-9135 correction note added")
        log.insert(0, f"{DATE}  U-9135  correction: 0x004fb5a0 is d3dx9 c_D3DXPSGPUpdateSkinnedMesh itself, not a D3DX "
                      f"client; 9 render rows refiled to {PSGP} evidence: {RESULT}")

    for o in out:
        print(o)
    print(f"refiled {sum(1 for o in out if o.startswith('refile') and 'SKIP' not in o)}, "
          f"inittime {sum(1 for o in out if o.startswith('inittime') and 'SKIP' not in o)}, added {len(added)}, "
          f"CHANGELOG lines {len(log)}")
    if not do:
        print("dry run: nothing written")
        return
    if added:
        with open(HERE / "added_batch.tsv", "w", encoding="utf-8", newline="") as f:
            f.write("rva\troute\tevidence\n")
            for k in added:
                f.write(f"0x{k}\tlibid\t{TSV}\n")
    wr(W / "hooks.csv", eol.join(lines) + (eol if trail else ""))
    wr(up, ut)
    ct = rd(W / "re/analysis/CHANGELOG.md")
    ceol = "\r\n" if "\r\n" in ct else "\n"
    cl = ct.split(ceol)
    mi = next(i for i, l in enumerate(cl) if l.strip() == MARKER)
    cl[mi + 1:mi + 1] = log
    wr(W / "re/analysis/CHANGELOG.md", ceol.join(cl))
    print("written: hooks.csv, UNCERTAINTIES.md, re/analysis/CHANGELOG.md, added_batch.tsv")


if __name__ == "__main__":
    main()
