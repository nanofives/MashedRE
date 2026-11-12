"""Closed sets of C4-by-match rows that can be swapped into mashed_re.exe as one unit (PORT-BATCH-TOOL, D4).

A row is a CANDIDATE when hooks.csv says C4, a byte-matched source under re/match/ holds its body (`// MATCH: 0x<rva>`),
its library is in the P-DoD port count (game code and RenderWare-Physics 3.7; CRT, libpng, zlib, RW core, Lua, qhull,
d3dx9, DirectShow are out: L-67 / L-70 / L-97 / L-98), and the exe does not already compile that matched source.

The unit is a matched FILE, not a row: `#include` of a matched .c compiles every `// MATCH:` body in it, so all of
them must link. Per RVA the file with the fewest MATCH bodies is used. A file that also holds the body of an RVA the
exe already compiles elsewhere is marked DUPFILE (including it makes a second exe body).

Callees of a unit = every `FUN_<rva>` its bodies name (re/match text, filtered to function entries of
re/console/pc_funcs.csv). Each callee is one of:
  set       a candidate RVA (its unit must stay in the closed set)
  bound     the exe has a C-linkage, non-static function for it: the body rva_body_scan / backfill_port resolve to
            a map symbol, or a `#define FUN_<rva> <Name>` in an exe-compiled TU whose Name is such a symbol, or a
            BINDINGS.tsv row (kind matched / port / bound / shim / rw / crt) whose exe_symbol is in the map, or one
            of the 6 RVA thunks. BINDINGS kind `provisional` (L-126, 6a, rows in re/match/PROVISIONAL.tsv) is never
            bound: such a callee is open
  rw        RenderWare core (L-98, librw) bound by name in the exe (LibRw/RwApiBind.cpp or BINDINGS.tsv): bound too
  open      anything else (C2/C3 or unmatched game code, unbound CRT or RW core, a callee only a stand-in binds).
            A unit with an open callee is removed, and the removal repeats to a fixpoint.
Groups = connected components of the surviving units over their call edges.

Reach. The linked exe's call graph (backfill_port.build_graph: rel32 calls + base relocations) is walked twice:
from _WinMain@16 with _Orig_00492370 as a barrier (the DEFAULT path: exe_main.cpp only enters the matched WinMain
when MASHED_ORIG_TOP=1) and from _Orig_00492370 (the TOP path). A member is a ROOT when
  inplace   the exe already runs a hand body for it on that path (the swap replaces that body: the switch then needs
            the hand body and the matched body side by side, a second exe body for the RVA -> lint conflict DUP)
  rebind    a reached exe function that already compiles a matched body binds FUN_<member> to a stand-in or to a
            name with no RVA body (the swap changes that `#define`; no second body)
and reach then follows the matched call edges inside the group.

Blockers per group (reported, not removed): DUP (inplace roots), CROSS (a member has an asi-only body: an exe-only
matched body makes a CROSS-TARGET pair), ALLOW (a member is already on re/tools/dual_copy_allowlist.txt), DUPFILE,
SEED (a nonzero file-backed .rdata/.data address a member reads that Compat/PortStateSeed.inc does not seed: the exe
pad maps it as 0), STATE (a bss/zero global the exe names only as a shadow copy or comment, per
verify/portstate_20261108/inventory.py's rule).

  py -3.12 re/tools/port_closed_sets.py [--build-dir <dir>] [--out <dir>]
writes <out>/closed_<subsystem>.tsv (group, rows, reached, blockers + detail columns), <out>/closed_summary.tsv and
<out>/closed_members.tsv (one line per member), and prints the per-subsystem table. Static analysis: the swap's
build, lint and gates are the authority.
"""
from __future__ import annotations

import collections
import csv
import os
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import backfill_port as BP          # noqa: E402
import rva_body_scan as S           # noqa: E402

SRC = ROOT / "mashedmod" / "src" / "mashed_re"
THUNKS = {0x0042b8b0, 0x0042b8c0, 0x0042d5a0, 0x00472f40, 0x004730b0, 0x00472c60}
OUT_LIBS = re.compile(r"msvc-crt|libpng|zlib|^renderware$|lua|qhull|d3dx9|directshow")
TOP_SYM = "_Orig_00492370@16"
MARK = re.compile(r"^//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x([0-9a-fA-F]{6,8})", re.M)
FUNREF = re.compile(r"\bFUN_([0-9a-fA-F]{8})\b")
DEFINE = re.compile(r"^\s*#\s*define\s+FUN_([0-9a-fA-F]{8})\s+([A-Za-z_]\w*)\b", re.M)
INC = re.compile(r'^\s*#\s*include\s+"([^"]*re/match/[^"]+\.c)"', re.M)
GDECL = re.compile(r"MGLOBAL\(\s*([^,]+?)\s*,\s*(\w+)\s*,\s*(0x[0-9a-fA-F]+)\s*\)")
INDIRECT = re.compile(r"->\s*\w+\s*\(|\)\s*\)\s*\(")   # ->m(...) or (*(T *)(x))(...)
ADECL = re.compile(r"MGLOBALA\(\s*([^,]+?)\s*,\s*(\w+)\s*,\s*([^,]+?)\s*,\s*(0x[0-9a-fA-F]+)\s*\)")


def rel(p) -> str:
    return pathlib.Path(p).resolve().relative_to(ROOT.resolve()).as_posix()


# ----------------------------------------------------------------------------------------------- inputs
def load_hooks():
    h = {}
    with open(ROOT / "hooks.csv", newline="", encoding="utf-8") as fh:
        for r in csv.DictReader(fh):
            try:
                h[int(r["rva"], 16)] = r
            except (ValueError, TypeError):
                pass
    return h


def lib_of(r) -> str:
    s = r.get("subsystem", "") or ""
    return s[len("third-party-library["):-1] if s.startswith("third-party-library[") else ""


def load_funcs():
    out = set()
    with open(ROOT / "re" / "console" / "pc_funcs.csv", newline="") as fh:
        for r in csv.DictReader(fh):
            out.add(int(r["entry"], 16))
    return out


def load_matched():
    """file(rel) -> {rva: body text}; rva -> [files]."""
    files, by_rva = {}, collections.defaultdict(list)
    for dp, _dn, fn in os.walk(ROOT / "re" / "match"):
        if os.sep + "raw" in dp or "/raw" in dp.replace("\\", "/"):
            continue
        for f in fn:
            if not f.endswith(".c"):
                continue
            p = pathlib.Path(dp) / f
            txt = p.read_text(encoding="utf-8", errors="replace")
            marks = [(m.start(), int(m.group(1), 16)) for m in MARK.finditer(txt)]
            if not marks:
                continue
            bodies = {}
            for i, (pos, a) in enumerate(marks):
                end = marks[i + 1][0] if i + 1 < len(marks) else len(txt)
                bodies[a] = txt[pos:end]
            r = rel(p)
            files[r] = (txt, bodies)
            for a in bodies:
                by_rva[a].append(r)
    return files, by_rva


def load_map_full(mp: pathlib.Path):
    """va -> [(sym, static)]."""
    out = collections.defaultdict(list)
    static = False
    for ln in mp.open(encoding="utf-8", errors="replace"):
        if "Static symbols" in ln:
            static = True
        m = BP.RE_MAP.match(ln)
        if not m:
            continue
        va = int(m.group(4), 16)
        if int(m.group(1), 16) == 0 or va == 0:
            continue
        out[va].append((m.group(3), static))
    return out


def c_public(syms, name) -> bool:
    pat = re.compile(r"^(?:_%s(?:@\d+)?|@%s@\d+)$" % (re.escape(name), re.escape(name)))
    return any(pat.match(s) and not st for s, st in syms)


def walk(edges, root, barriers):
    seen = {root}
    q = collections.deque([root])
    while q:
        a = q.popleft()
        if a != root and a in barriers:
            continue
        for t in edges.get(a, ()):
            if t not in seen:
                seen.add(t)
                q.append(t)
    return seen


def parse_seed():
    seeded = []
    p = SRC / "Compat" / "PortStateSeed.inc"
    if p.exists():
        for m in re.finditer(r"//\s*0x([0-9a-fA-F]{8})\s+\S+\s+\((\S+),\s*(\d+)\)", p.read_text(encoding="utf-8")):
            seeded.append((int(m.group(1), 16), int(m.group(3))))
    return seeded


class PE:
    def __init__(self):
        import pefile
        orig = os.environ.get("MASHED_UNPATCHED", str(ROOT.parents[1] / "Mashed" / "original" / "MASHED.exe.unpatched"))
        if not os.path.exists(orig):
            orig = r"C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched"
        pe = pefile.PE(orig, fast_load=True)
        base = pe.OPTIONAL_HEADER.ImageBase
        self.raw = open(orig, "rb").read()
        self.secs = [(s.Name.rstrip(b"\0").decode(), base + s.VirtualAddress, s.Misc_VirtualSize, s.SizeOfRawData,
                      s.PointerToRawData) for s in pe.sections]

    def classify(self, a, size):
        for name, va, vs, rs, pr in self.secs:
            if va <= a < va + max(vs, rs):
                off = a - va
                if off >= rs:
                    return name, "bss"
                b = self.raw[pr + off: pr + min(off + size, rs)]
                return name, ("nonzero" if any(b) else "zero")
        return "none", "none"


SIZES = {"char": 1, "uchar": 1, "byte": 1, "undefined": 1, "undefined1": 1, "bool": 1, "sbyte": 1, "short": 2,
         "ushort": 2, "undefined2": 2, "word": 2, "double": 8, "undefined8": 8, "longlong": 8, "ulonglong": 8,
         "__int64": 8, "float10": 10}


def tsize(t):
    t = t.strip()
    if "*" in t:
        return 4
    t = t.replace("unsigned", "").replace("signed", "").replace("const", "").strip() or "int"
    return SIZES.get(t, 4)


def globals_of(txt, body):
    decl = {}
    for t, n, a in GDECL.findall(txt):
        decl[n] = (int(a, 16), tsize(t))
    for t, n, cnt, a in ADECL.findall(txt):
        try:
            c = int(eval(cnt, {}, {}))
        except Exception:
            c = 1
        decl[n] = (int(a, 16), tsize(t) * c)
    return [(n, a, sz) for n, (a, sz) in decl.items() if re.search(r"\b%s\b" % re.escape(n), body)]


# ----------------------------------------------------------------------------------------------- analysis
def analyse(build: pathlib.Path):
    hooks = load_hooks()
    funcs = load_funcs()
    mfiles, m_by_rva = load_matched()

    scan = S.scan_all()
    exe_tus, asi_tus = scan["targets"]["exe"], scan["targets"]["asi"]
    asi_only = asi_tus - exe_tus
    eb, ab = S.rva_to_tus(scan, "exe"), S.rva_to_tus(scan, "asi")

    # which matched files the exe compiles, and the defines of every exe TU
    compiled_files, exe_defines = set(), collections.defaultdict(set)
    compiled_by = collections.defaultdict(set)   # matched file -> exe TUs that include it
    define_site = {}
    for t in sorted(exe_tus):
        p = SRC / t
        if not p.exists():
            continue
        txt = p.read_text(encoding="utf-8", errors="replace")
        for inc in INC.findall(txt):
            try:
                compiled_files.add(rel((p.parent / inc).resolve()))
                compiled_by[rel((p.parent / inc).resolve())].add(t)
            except ValueError:
                pass
        for a, nm in DEFINE.findall(txt):
            exe_defines[int(a, 16)].add(nm)
            define_site.setdefault((int(a, 16), nm), t)
    exe_compiled_rvas = {a for f in compiled_files if f in mfiles for a in mfiles[f][1]}

    # BINDINGS.tsv
    bindings = {}
    bp = ROOT / "verify" / "porttop_20261108" / "BINDINGS.tsv"
    if bp.exists():
        with open(bp, newline="", encoding="utf-8") as fh:
            for r in csv.DictReader(fh, delimiter="\t"):
                try:
                    bindings[int(r["rva"], 16)] = (r["kind"], r["exe_symbol"])
                except (ValueError, TypeError, KeyError):
                    pass

    # exe graph and symbols
    exe, mp = build / "mashed_re.exe", build / "mashed_re.map"
    ents, names_at, edges, _nrel, (t_lo, t_hi) = BP.build_graph(exe, mp)
    symmap = load_map_full(mp)
    by_sym = {}
    for va, s, _sect, _obj in ents:
        by_sym.setdefault(s, va)
    win = by_sym.get("_WinMain@16")
    top = by_sym.get(TOP_SYM)
    reach_def = walk(edges, win, {top} if top else set())
    reach_top = walk(edges, top, set()) if top else set()
    bodies = BP.exe_bodies()
    res, _by_key = BP.resolve(bodies, ents, t_lo, t_hi)
    all_syms = collections.defaultdict(list)
    for va, lst in symmap.items():
        for s, st in lst:
            all_syms[BP.base_name(s)].append((va, s, st))

    def name_bindable(nm):
        for va, s, st in all_syms.get(nm, ()):
            if not st and re.match(r"^(?:_%s(?:@\d+)?|@%s@\d+)$" % (re.escape(nm), re.escape(nm)), s):
                return va
        return None

    exe_body = {}   # rva -> (name, va, reach)
    for a, items in res.items():
        for _r, nm, vas, _how in items:
            for va in vas:
                if c_public(symmap.get(va, ()), nm):
                    exe_body.setdefault(a, (nm, va))
    # every exe body VA (for reach), bindable or not
    body_vas = {a: [va for _r, _n, lst, _h in items for va in lst] for a, items in res.items()}

    def reach_of_rva(a):
        vas = body_vas.get(a, [])
        d = any(v in reach_def for v in vas)
        t = any(v in reach_top for v in vas)
        return d, t

    seeded = parse_seed()

    def is_seeded(a, sz):
        return any(s <= a and a + sz <= s + n for s, n in seeded)

    pe = PE()

    # ---- candidates
    def in_count(r):
        lb = lib_of(r)
        return not (lb and OUT_LIBS.search(lb))

    cand = set()
    for a, r in hooks.items():
        if not (r.get("confidence") or "").startswith("C4"):
            continue
        if a not in m_by_rva or not in_count(r):
            continue
        if a in exe_compiled_rvas:
            continue
        cand.add(a)

    # unit per rva
    unit_of, units = {}, collections.defaultdict(set)
    for a in sorted(cand):
        f = min(m_by_rva[a], key=lambda x: (len(mfiles[x][1]), x))
        unit_of[a] = f
    for a, f in unit_of.items():
        units[f] |= set(mfiles[f][1])
    # a file's other bodies must be candidates or the file duplicates an exe body
    unit_flags = collections.defaultdict(set)
    for f, rvas in units.items():
        for a in rvas:
            if a in exe_compiled_rvas:
                unit_flags[f].add("DUPFILE:%08x" % a)
    # rvas of a unit that are not candidates themselves (non-C4 statics in the file)
    unit_extra = {f: {a for a in rvas if a not in cand} for f, rvas in units.items()}

    def classify_callee(c):
        if c in THUNKS:
            return "bound", "thunk"
        r = hooks.get(c)
        lb = lib_of(r) if r else ""
        if c in exe_body:
            return ("rw" if lb == "renderware" else "bound"), exe_body[c][0]
        for nm in sorted(exe_defines.get(c, ())):
            if "StandIn" in nm:
                continue
            if name_bindable(nm) is not None:
                return ("rw" if lb == "renderware" else "bound"), nm
        if c in bindings:
            k, sym = bindings[c]
            if k == "provisional":
                # L-126 (6a): a provisional body is no port-column binding; the callee stays open here
                return "open", "provisional"
            if k in ("matched", "port", "bound", "shim", "rw", "crt") and sym and name_bindable(sym) is not None:
                return ("rw" if lb == "renderware" or k in ("rw", "bound", "shim") else "bound"), sym
        if c in cand:
            return "set", ""
        why = "nohook" if r is None else (lb or (r.get("confidence") or "C?"))
        if any("StandIn" in nm for nm in exe_defines.get(c, ())):
            why += "/standin-only"
        return "open", why

    # unit callees
    unit_callees = {}
    for f, rvas in units.items():
        txt, bodies_ = mfiles[f]
        cs = set()
        for a in rvas:
            cs |= {int(x, 16) for x in FUNREF.findall(bodies_[a])}
        cs = {c for c in cs if c in funcs and c not in rvas}
        unit_callees[f] = cs

    # fixpoint
    alive = set(units)
    open_why = collections.defaultdict(list)
    callee_cls = {}
    for f in units:
        for c in unit_callees[f]:
            if c not in callee_cls:
                callee_cls[c] = classify_callee(c)
    changed = True
    while changed:
        changed = False
        live_rvas = {a for f in alive for a in units[f]}
        for f in sorted(alive):
            bad = []
            for c in unit_callees[f]:
                k, why = callee_cls[c]
                if k == "open" or (k == "set" and c not in live_rvas):
                    bad.append((c, why if k == "open" else "set-removed"))
            if unit_extra[f] - cand:
                for a in unit_extra[f]:
                    r = hooks.get(a)
                    if a in exe_compiled_rvas:
                        continue   # DUPFILE flagged
                    if r is None or not (r.get("confidence") or "").startswith("C4"):
                        pass       # a non-C4 static in the file compiles with it: allowed, noted
            if bad:
                alive.discard(f)
                open_why[f] = bad
                changed = True

    # ---- call edges between surviving units (rva level; same-file statics follow their caller)
    live_rvas = {a for f in alive for a in units[f]}
    file_of = {}
    for f in alive:
        for a in units[f]:
            file_of.setdefault(a, f)
    inner = collections.defaultdict(set)
    for f in alive:
        bodies_ = mfiles[f][1]
        for a in units[f]:
            for x in FUNREF.findall(bodies_[a]):
                c = int(x, 16)
                if c != a and c in live_rvas:
                    inner[a].add(c)
            inner[a] |= {b for b in units[f] if b != a}   # one include compiles the whole file

    def closure(roots):
        seen = set(roots)
        q = collections.deque(roots)
        while q:
            x = q.popleft()
            for y in inner.get(x, ()):
                if y not in seen:
                    seen.add(y)
                    q.append(y)
        return seen

    # ---- roots
    # inplace: the exe runs a hand body for the member on that path.
    # rebind: an exe-compiled matched body that the exe reaches names FUN_<member>, and the member has no exe body:
    #         that wrapper's `#define FUN_<member> <Name>` (a stand-in, or a name with no RVA body) is the wiring point.
    rebind = collections.defaultdict(list)   # member -> [(wrapper TU, caller rva, bound name, path)]
    for f, tus in compiled_by.items():
        if f not in mfiles:
            continue
        for a, body in mfiles[f][1].items():
            d, t = reach_of_rva(a)
            if not (d or t):
                continue
            for x in set(FUNREF.findall(body)):
                c = int(x, 16)
                if c in live_rvas and c not in eb:
                    for tu in sorted(tus):
                        nm = next((n for n in sorted(exe_defines.get(c, ())) if define_site.get((c, n)) == tu), "")
                        rebind[c].append((tu, a, nm, ("default" if d else "") + ("+top" if t else "")))

    allow = set()
    for ln in (ROOT / "re" / "tools" / "dual_copy_allowlist.txt").read_text(encoding="utf-8").splitlines():
        ln = ln.split("#", 1)[0].strip()
        if ln.count(":") >= 2:
            allow.add(int(ln.split(":")[1], 16))

    # ---- per-member facts
    mfact = {}
    for a in sorted(live_rvas):
        r = hooks.get(a, {})
        f = file_of[a]
        txt, bodies_ = mfiles[f]
        d, t = reach_of_rva(a)
        miss = []
        for n, ga, sz in globals_of(txt, bodies_.get(a, "")):
            _sec, init = pe.classify(ga, sz)
            if init == "nonzero" and not is_seeded(ga, sz):
                miss.append("0x%08x" % ga)
        blk = []
        if a in eb:
            blk.append("DUP")             # a second exe body under the switch
        if any(tu in asi_only for tu in ab.get(a, ())):
            blk.append("CROSS")           # an asi-only body vs the exe-only batch body
        if a in allow:
            blk.append("ALLOW")
        if unit_flags.get(f):
            blk.append("DUPFILE")
        if miss:
            blk.append("SEED")
        if not (r.get("confidence") or "").startswith("C4"):
            blk.append("NONC4")           # a non-C4 body that the file compiles with its C4 neighbours
        body = bodies_.get(a, "")
        if "DAT_007d3ff8" in body:
            blk.append("RWGLOBALS")       # reads the original RwGlobals pointer: 0 in the exe pad (hard)
        if INDIRECT.search(body):
            blk.append("INDIRECT")        # calls through a pointer it reads (vtable / callback): note only
        rb_def = any("default" in x[3] for x in rebind.get(a, ()))
        rb_top = any("top" in x[3] for x in rebind.get(a, ()))
        shape = "inplace" if (a in eb and (d or t)) else ("rebind" if (rb_def or rb_top) else "")
        mfact[a] = dict(rva="0x%08x" % a, name=r.get("name", ""), subsystem=r.get("subsystem", ""),
                        confidence=r.get("confidence", ""), port=r.get("port", ""), file=f,
                        exe_body="+".join(sorted(set(eb.get(a, ())) | set(ab.get(a, ())))), shape=shape,
                        root_def=int((a in eb and d) or rb_def), root_top=int((a in eb and t) or rb_top),
                        blockers=",".join(blk) or "none",
                        rebind=" ".join("%s:%s->%s" % (tu, "0x%08x" % c, nm or "?") for tu, c, nm, _p in rebind.get(a, ())),
                        callees=" ".join("0x%08x:%s" % (c, callee_cls[c][0]) for c in sorted(unit_callees[f])),
                        seeds_missing=" ".join(miss))

    # ---- groups: one per root (its downward closure), plus one per unreached top unit
    called_by = collections.defaultdict(set)
    for a, cs in inner.items():
        for c in cs:
            called_by[c].add(a)
    heads = [a for a in sorted(live_rvas) if mfact[a]["root_def"] or mfact[a]["root_top"]]
    covered = set()
    for h in heads:
        covered |= closure([h])
    for a in sorted(live_rvas):
        if a not in covered and not (called_by[a] - {b for b in units[file_of[a]]}):
            heads.append(a)
            covered |= closure([a])
    out_groups = []
    for h in heads:
        cl = closure([h])
        rd = closure([a for a in cl if mfact[a]["root_def"]]) & cl
        rt = closure([a for a in cl if mfact[a]["root_top"]]) & cl
        blk = collections.Counter()
        for a in cl:
            for b in mfact[a]["blockers"].split(","):
                if b != "none":
                    blk[b] += 1
        hard = sum(blk[k] for k in ("DUP", "CROSS", "ALLOW", "DUPFILE", "RWGLOBALS"))
        out_groups.append(dict(head=h, sub=hooks.get(h, {}).get("subsystem", "?"), rows=len(cl),
                               reached=("default" if rd else "") + ("+" if rd and rt else "") + ("top" if rt else "")
                               or "no", n_def=len(rd), n_top=len(rt),
                               head_shape=mfact[h]["shape"] or "-",
                               blockers=",".join("%s:%d" % kv for kv in sorted(blk.items())) or "none",
                               lint_clean=int(hard == 0), seed_clean=int(blk["SEED"] == 0),
                               rvas=" ".join("0x%08x" % a for a in sorted(cl))))
    return dict(hooks=hooks, cand=cand, units=units, alive=alive, groups=out_groups, mfact=mfact,
                removed=dict(open_why), callee_cls=callee_cls, reach_def=len(reach_def), reach_top=len(reach_top),
                closure=closure, live_rvas=live_rvas)


def safe(s):
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", s)


def subsystem_sets(A, sub, clean_only):
    """The swap set of one subsystem: the union of the closures of its reached groups (default path), optionally
    only the lint- and seed-clean ones. Returns (rvas, heads)."""
    heads, rv = [], set()
    for g in A["groups"]:
        if g["sub"] != sub or not g["n_def"]:
            continue
        if clean_only and not (g["lint_clean"] and g["seed_clean"]):
            continue
        heads.append(g["head"])
        rv |= {int(x, 16) for x in g["rvas"].split()}
    return rv, heads


TAB, NL = "\t", "\n"


def main(argv):
    build = ROOT / "mashedmod" / "build"
    if "--build-dir" in argv:
        build = pathlib.Path(argv[argv.index("--build-dir") + 1]).resolve()
    out = ROOT / "verify" / "portbatch_20261109" / "closed"
    if "--out" in argv:
        out = pathlib.Path(argv[argv.index("--out") + 1]).resolve()
    out.mkdir(parents=True, exist_ok=True)
    A = analyse(build)
    groups, mfact = A["groups"], A["mfact"]
    by_sub = collections.defaultdict(list)
    for g in groups:
        by_sub[g["sub"]].append(g)
    for sub, gs in by_sub.items():
        gs.sort(key=lambda g: (-g["n_def"], -g["n_top"], -g["rows"], g["head"]))
        for i, g in enumerate(gs, 1):
            g["group"] = "%s-%03d" % (safe(sub), i)
    cols = ["group", "rows", "reached", "blockers", "head", "head_shape", "n_def", "n_top", "lint_clean",
            "seed_clean", "rvas"]
    for old in out.glob("closed_*.tsv"):
        old.unlink()
    for sub, gs in sorted(by_sub.items()):
        with open(out / ("closed_%s.tsv" % safe(sub)), "w", newline="", encoding="utf-8") as fh:
            w = csv.DictWriter(fh, cols, delimiter=TAB, lineterminator=NL, extrasaction="ignore")
            w.writeheader()
            for g in gs:
                w.writerow(dict(g, head="0x%08x" % g["head"]))
    mcols = ["rva", "name", "subsystem", "confidence", "port", "shape", "root_def", "root_top", "blockers",
             "exe_body", "file", "rebind", "callees", "seeds_missing"]
    with open(out / "closed_members.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, mcols, delimiter=TAB, lineterminator=NL)
        w.writeheader()
        for a in sorted(mfact):
            w.writerow(mfact[a])
    with open(out / "closed_removed.tsv", "w", newline="", encoding="utf-8") as fh:
        fh.write(TAB.join(["file", "rvas", "open_callees"]) + NL)
        for f, bad in sorted(A["removed"].items()):
            fh.write(TAB.join([f, " ".join("0x%08x" % a for a in sorted(A["units"][f])),
                               " ".join("0x%08x:%s" % (c, w_) for c, w_ in sorted(bad)[:12])]) + NL)
    srows = []
    for sub, gs in sorted(by_sub.items()):
        allr, _ = subsystem_sets(A, sub, False)
        clr, clh = subsystem_sets(A, sub, True)
        best = max(gs, key=lambda g: (g["n_def"], g["rows"]))
        srows.append(dict(subsystem=sub, groups=len(gs),
                          closed_rows=len({int(x, 16) for g in gs for x in g["rvas"].split()}),
                          reached_default_groups=sum(1 for g in gs if g["n_def"]),
                          reached_top_groups=sum(1 for g in gs if g["n_top"]),
                          default_set_rows=len(allr), largest_default_group=best["group"] if best["n_def"] else "-",
                          largest_default_rows=best["n_def"],
                          clean_default_set_rows=len(clr), clean_heads=" ".join("0x%08x" % h for h in clh)))
    sc = list(srows[0].keys()) if srows else []
    with open(out / "closed_summary.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, sc, delimiter=TAB, lineterminator=NL)
        w.writeheader()
        w.writerows(srows)
    print("candidates %d rows in %d units; closed %d units / %d rows; %d groups; exe symbols reached: default %d, "
          "top %d" % (len(A["cand"]), len(A["units"]), len(A["alive"]), len(A["live_rvas"]), len(groups),
                      A["reach_def"], A["reach_top"]))
    print(TAB.join(sc))
    for r in srows:
        print(TAB.join(str(r[k]) for k in sc))
    if "--sub" in argv:
        sub = argv[argv.index("--sub") + 1]
        rv, hs = subsystem_sets(A, sub, "--all" not in argv)
        print("SET %s %s rows=%d heads=%s" % (sub, "all" if "--all" in argv else "clean", len(rv),
                                             " ".join("0x%08x" % h for h in hs)))
        for a in sorted(rv):
            m = mfact[a]
            print("  %s %-28s %-7s %-7s %-14s %s %s" % (m["rva"], m["name"][:28], m["port"], m["shape"] or "-",
                                                    m["blockers"], m["file"], m["rebind"][:140]))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
