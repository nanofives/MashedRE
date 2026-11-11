"""PORT-ORIGTOP-SIZE (D4): how many C4-by-match rows the MASHED_ORIG_TOP route can run, per binding family.

  py -3.12 verify/portorigtop_20261109/origtop_size.py [--exe <MASHED.exe.unpatched>] [--order]

Static, read-only. Inputs:
  re/console/pc_funcs.csv      function entries + bounds of the original
  re/console/pc_features.csv   direct callees per function (E8 rel32)
  the original binary          base relocations: a relocated dword inside function F that points at a function entry
                               is an address-taken edge F -> G (callback, stored pointer); one that points into
                               .rdata/.data starts a table, and every following relocated dword of that table that is a
                               function entry is an edge F -> entry (vtables, state / jump tables of function pointers)
  hooks.csv                    confidence, subsystem / library, notes, exe_file
  re/match/**                  `// MATCH: 0x<rva>` bodies (re/tools/port_closed_sets.load_matched)
  verify/porttop_20261108/BINDINGS.tsv   what the ORIG_TOP path binds today

Node classes:
  bind:<kind>  BINDINGS kind bound / port / rw / shim / crt / stand-in: satisfied, not expanded (the exe supplies it)
  match        hooks C4 and a matched body (BINDINGS kind `matched` included): satisfied when every callee is
  family F     a library family (lua, rwcore, rt2d, d3d-dshow, crt, qhull, png-zlib, rwphysics-nomatch): satisfied only
               when F is bound; never expanded (library internals)
  game-c4-hand C4 game code with no matched body and an exe hand body (could bind to it, as BINDINGS `port` rows do)
  game-c4      C4 game code with no matched body and no exe body
  game-c0c3    C0..C3 game code (needs matching first)
  (admitted)   EQUIV-ADMIT-1, L-126: a row of re/match/ADMITTED.tsv (equiv.py proved its source's DDK body equivalent)
               is classed `match`, satisfied when every callee is, and counted apart in SIZE.tsv. Never C4.
               --no-admitted reproduces the counts before L-126; --out <dir> writes somewhere else.
  norow       a function entry with no hooks.csv row
Reach = closure from 0x00492370 over the edges, not expanding bind / family nodes.
Runnable = greatest fixpoint: a `match` node in reach is runnable when every callee is runnable, bind, or a bound family.
"""
from __future__ import annotations

import collections
import csv
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import port_closed_sets as P      # noqa: E402

TOP = 0x00492370
# the run loop and race path under ORIG_TOP, in call order (BINDINGS.tsv rows 3, 20-23; hooks.csv notes)
RACE = [0x00492290, 0x004929d0, 0x00492d30, 0x0043d7c0, 0x0043dfd0, 0x004111c0, 0x00492e90]
EXE_DEF = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
BIND_SAT = {"bound", "port", "rw", "shim", "crt"}   # stand-in: replaced by the route, so expanded
OUT = ROOT / "verify" / "portorigtop_20261109"
ADMITTED = ROOT / "re" / "match" / "ADMITTED.tsv"


def load_admitted(path=ADMITTED):
    """RVAs of the L-126 admission record (first column of its 0x... rows)."""
    out = set()
    if path.exists():
        for ln in path.read_text(encoding="utf-8").splitlines():
            if ln.startswith("0x"):
                out.add(int(ln.split("\t", 1)[0], 16))
    return out


def load_bounds():
    ent, bounds = [], {}
    with open(ROOT / "re" / "console" / "pc_funcs.csv", newline="") as fh:
        for r in csv.DictReader(fh):
            a, e = int(r["entry"], 16), int(r["body_end"], 16)
            ent.append(a)
            bounds[a] = e
    ent.sort()
    return ent, bounds


def owner(ent, bounds, va):
    import bisect
    i = bisect.bisect_right(ent, va) - 1
    if i >= 0 and ent[i] <= va < bounds[ent[i]]:
        return ent[i]
    return None


def direct_edges():
    e = collections.defaultdict(set)
    with open(ROOT / "re" / "console" / "pc_features.csv", newline="") as fh:
        for r in csv.DictReader(fh):
            a = int(r["entry"], 16)
            for c in (r["callees"] or "").split(";"):
                if c:
                    e[a].add(int(c, 16))
    return e


def reloc_edges(exe, ent, bounds):
    """Address-taken edges. MASHED.exe has no base relocations (fixed base), so each function is disassembled
    (capstone, its pc_funcs bounds) and every imm32 / disp32 operand is checked: a function entry is an edge F -> G
    (callback, stored pointer); an address in .rdata/.data starts a table, whose consecutive dwords that are function
    entries are edges F -> entry (vtables, state tables; the walk stops at the first dword that is not a function
    entry, at most 512)."""
    import capstone
    import pefile
    pe = pefile.PE(str(exe), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    fset = set(ent)
    data = [(base + s.VirtualAddress, base + s.VirtualAddress + s.Misc_VirtualSize) for s in pe.sections
            if s.Name.rstrip(b"\0") in (b".rdata", b".data")]

    def indata(v):
        return any(lo <= v < hi for lo, hi in data)

    def dword(va):
        o = va - base
        return int.from_bytes(img[o:o + 4], "little") if 0 <= o < len(img) - 4 else None
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    e = collections.defaultdict(set)
    tables = 0
    tcache = {}
    for f in ent:
        code = img[f - base:bounds[f] - base]
        for ins in md.disasm(code, f):
            vals = []
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_IMM:
                    vals.append(op.imm & 0xffffffff)
                elif op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0 and op.mem.disp:
                    vals.append(op.mem.disp & 0xffffffff)
            for v in vals:
                if v in fset:
                    if ins.mnemonic != "call" and v != f:
                        e[f].add(v)
                elif indata(v):
                    if v not in tcache:
                        got, p = [], v
                        while len(got) < 512:
                            w = dword(p)
                            if w is None or w not in fset:
                                break
                            got.append(w)
                            p += 4
                        tcache[v] = got
                    if tcache[v]:
                        e[f].update(tcache[v])
    tables = sum(1 for v in tcache.values() if v)
    return e, tables


def load_bindings():
    b = {}
    with open(ROOT / "verify" / "porttop_20261108" / "BINDINGS.tsv", newline="", encoding="utf-8") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            try:
                b[int(r["rva"], 16)] = r["kind"]
            except ValueError:
                pass
    return b


def family_of(r):
    lib = P.lib_of(r)
    notes = r.get("notes", "") or ""
    if not lib:
        return ""
    if "lua" in lib:
        return "lua"
    if lib == "renderware":
        return "rt2d" if re.search(r"\bRt2d", notes) else "rwcore"
    if "directshow" in lib or "d3dx9" in lib:
        return "d3d-dshow"
    if "msvc" in lib:
        return "crt"
    if "qhull" in lib:
        return "qhull"
    if "libpng" in lib or "zlib" in lib:
        return "png-zlib"
    if "RenderWare-Physics" in lib:
        return "rwphysics"
    return "lib-" + lib


def main(argv):
    global OUT
    if "--out" in argv:
        OUT = pathlib.Path(argv[argv.index("--out") + 1])
    adm = set() if "--no-admitted" in argv else load_admitted()
    exe =pathlib.Path(argv[argv.index("--exe") + 1]) if "--exe" in argv else EXE_DEF
    ent, bounds = load_bounds()
    fset = set(ent)
    dedge = direct_edges()
    redge, ntab = reloc_edges(exe, ent, bounds)
    edges = collections.defaultdict(set)
    for src in (dedge, redge):
        for a, cs in src.items():
            edges[a] |= {c for c in cs if c in fset and c != a}
    H = P.load_hooks()
    _files, by_rva = P.load_matched()
    B = load_bindings()
    plat = set()
    with open(ROOT / "re" / "console" / "match" / "platform_candidates.csv", newline="") as fh:
        for r in csv.DictReader(fh):
            if r["signal"] == "platform-api":
                plat.add(int(r["pc_va"], 16))

    def cls(a):
        k = B.get(a)
        if k in BIND_SAT:
            return "bind:" + k
        r = H.get(a)
        if k == "matched":
            return "match"
        if a in adm:
            return "match"      # L-126 admitted body (re/match/ADMITTED.tsv)
        if a in plat:
            return "fam:d3d-dshow"
        if r is None:
            return "norow"
        fam = family_of(r)
        if r["confidence"] == "C4" and a in by_rva:
            if fam and fam != "rwphysics":
                return "fam:" + fam      # a matched library row is still a library row: bound by family
            return "match"
        if fam:
            return "fam:" + fam
        if r["confidence"] == "C4":
            return "game-c4-hand" if (r.get("exe_file") or "").strip() else "game-c4"
        return "game-c0c3"
    C = {a: cls(a) for a in fset}
    if "--assume-bound" in argv:   # PORT-OT-BIND-CRT: treat the listed rvas (one 0x... per line) as bound
        for l in open(argv[argv.index("--assume-bound") + 1], encoding="utf-8"):
            if l.strip().startswith("0x"):
                C[int(l.split()[0], 16)] = "bind:crt"

    # reach
    reach, stack = {TOP}, [TOP]
    while stack:
        a = stack.pop()
        if C[a].startswith("bind:") or C[a].startswith("fam:"):
            continue
        for c in edges.get(a, ()):
            if c not in reach:
                reach.add(c)
                stack.append(c)
    # reach by direct edges only (for the report)
    dreach, stack = {TOP}, [TOP]
    while stack:
        a = stack.pop()
        if C[a].startswith("bind:") or C[a].startswith("fam:"):
            continue
        for c in dedge.get(a, ()):
            if c in fset and c not in dreach:
                dreach.add(c)
                stack.append(c)

    if "--list" in argv:   # PORT-OT-BIND-CRT: print the reached members of one class, e.g. --list fam:crt
        want = argv[argv.index("--list") + 1]
        callers = collections.Counter(c for a in reach for c in edges.get(a, ()) if C[a] not in (want,))
        for a in sorted(x for x in reach if C[x] == want):
            r = H.get(a) or {}
            print("0x%08x	%s	%s	%s	callers_in_reach=%d" % (a, r.get("name", "-"), r.get("subsystem", "-"),
                  r.get("confidence", "-"), callers[a]))
        return 0
    match_r = {a for a in reach if C[a] == "match"}

    def runnable(sat_classes):
        ok = set(match_r)
        changed = True
        while changed:
            changed = False
            for a in list(ok):
                for c in edges.get(a, ()):
                    if c not in reach:
                        continue
                    k = C[c]
                    if k == "match":
                        if c not in ok:
                            break
                    elif k.startswith("bind:") or k in sat_classes:
                        continue
                    else:
                        break
                else:
                    continue
                ok.discard(a)
                changed = True
        return ok

    base = runnable(set())
    classes = sorted({C[a] for a in reach} - {"match"} - {k for k in C.values() if k.startswith("bind:")})
    cost = {k: sum(1 for a in reach if C[a] == k) for k in classes}
    alone = {k: len(runnable({k})) - len(base) for k in classes}
    named = ["fam:lua", "fam:rwcore", "fam:rt2d", "fam:d3d-dshow"]
    fams = [k for k in classes if k in named or alone[k] >= 20]
    # cumulative, cheapest first (cost = rows of the family in reach that need a binding)
    order, cum, sat = [], [], set()
    for k in sorted(fams, key=lambda k: (cost[k], -alone[k])):
        sat.add(k)
        order.append(k)
        cum.append(len(runnable(sat)))
    # dual copies: rows runnable once every binding family is bound (matching work for C0..C3 NOT assumed)
    final = runnable(set(fams) - {"game-c0c3"})
    # caveats on the runnable rows: RwGlobals readers, and readers of original initialised data (.data/.rdata bytes
    # that are nonzero in MASHED.exe: the exe pad must carry them on this path)
    pe = P.PE()
    rwg, seedr = set(), set()
    for a in final:
        for f in by_rva.get(a, [])[:1]:
            txt, bodies = _files[f]
            body = bodies.get(a, "")
            if "DAT_007d3ff8" in body:
                rwg.add(a)
            for _n, ga, sz in P.globals_of(txt, body):
                sec, kind = pe.classify(ga, sz)
                if sec in (".data", ".rdata") and kind == "nonzero":
                    seedr.add(a)
                    break
    dual = collections.Counter()
    dual_rows = collections.defaultdict(list)
    for a in sorted(final):
        r = H.get(a)
        ef = ((r or {}).get("exe_file") or "").strip()
        if not ef or B.get(a) == "matched":
            continue
        txt = ""
        p = ROOT / ef.split("|")[0]
        if p.exists():
            txt = p.read_text(encoding="utf-8", errors="replace")
        if any(f.split("/")[-1] in txt for f in by_rva.get(a, [])):
            continue   # the exe already compiles the matched source for it
        sub = (r or {}).get("subsystem", "?")
        dual[sub] += 1
        dual_rows[sub].append("0x%08x" % a)

    lines = ["section\tkey\tvalue\tdetail"]
    lines.append("reach\tfunctions reached from 0x00492370 (direct + reloc edges)\t%d\tdirect calls only: %d; reloc tables used: %d" % (len(reach), len(dreach), ntab))
    for k, v in sorted(collections.Counter(C[a] for a in reach).items(), key=lambda x: -x[1]):
        lines.append("reach_class\t%s\t%d\t" % (k, v))
    lines.append("runnable\tC4-by-match rows in reach\t%d\tL-126 admitted rows (re/match/ADMITTED.tsv) included: %d" % (
        len(match_r), sum(1 for a in match_r if a in adm)))
    lines.append("runnable\talready running (BINDINGS matched)\t%d\t" % sum(1 for a in match_r if B.get(a) == "matched"))
    lines.append("runnable\trunnable with no new family bound\t%d\t" % len(base))
    for k in sorted(classes, key=lambda k: -alone[k]):
        lines.append("family_alone\t%s\t%d\tfunctions to bind in reach: %d%s" % (k, alone[k], cost[k], "  (named / >=20)" if k in fams else ""))
    for k, n in zip(order, cum):
        lines.append("cumulative\t+%s\t%d\tcost %d" % (k, n, cost[k]))
    lines.append("final\trunnable with every binding family bound (no C0..C3 matching)\t%d\t" % len(final))
    lines.append("caveat\tof those, read RwGlobals (DAT_007d3ff8)\t%d\t%s" % (len(rwg), " ".join("0x%08x" % a for a in sorted(rwg))))
    lines.append("caveat\tof those, read nonzero original .data/.rdata\t%d\t" % len(seedr))
    lines.append("dual\tTOTAL\t%d\t" % sum(dual.values()))
    for s, n in dual.most_common():
        lines.append("dual\t%s\t%d\t%s" % (s, n, " ".join(dual_rows[s])))
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "SIZE.tsv").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))
    if "--race" in argv:
        # per race-path node: its subtree (expanding match and non-bound nodes, the node itself even when bound), the
        # matched rows in it, and what blocks it with every named family bound
        satall = set(fams) - {"game-c0c3"}
        ok_all = runnable(satall)
        rl = ["node	name	class	subtree	matched	matched_runnable	blockers_by_class	c0c3_blockers"]
        for n in RACE:
            sub, st = {n}, [n]
            while st:
                a = st.pop()
                if a != n and (C[a].startswith("bind:") or C[a].startswith("fam:")):
                    continue
                for c in edges.get(a, ()):
                    if c not in sub:
                        sub.add(c)
                        st.append(c)
            m = [a for a in sub if C[a] == "match"]
            blk = collections.Counter(C[a] for a in sub if C[a] not in ("match",) and not C[a].startswith("bind:")
                                      and C[a] not in satall)
            cc = sorted((a for a in sub if C[a] == "game-c0c3"), key=lambda a: -(bounds[a] - a))
            rl.append("0x%08x	%s	%s	%d	%d	%d	%s	%s" % (
                n, (H.get(n) or {}).get("name", "?"), C[n], len(sub), len(m), sum(1 for a in m if a in ok_all),
                " ".join("%s:%d" % kv for kv in blk.most_common()),
                " ".join("0x%08x(%s,%dB)" % (a, (H.get(a) or {}).get("confidence", "?"), bounds[a] - a) for a in cc[:12])))
        (OUT / "race_path.tsv").write_text("\n".join(rl) + "\n", encoding="utf-8")
        print("\n".join(rl))
    if "--runtime" in argv:
        # re/coverage/reach.tsv: the original's own INT3 census (menu / race_load / race_run, majority of 3 runs).
        # The race point = every function the original runs in race_load / race_run that the route cannot run yet.
        rt = {}
        with open(ROOT / "re" / "coverage" / "reach.tsv", newline="") as fh:
            for r in csv.DictReader(fh, delimiter="\t"):
                try:
                    a = int(r["rva"], 16)
                except ValueError:
                    continue
                if a < 0x00400000:
                    a += 0x00400000
                rt[a] = (r["reached_menu"] == "1", r["reached_race_load"] == "1", r["reached_race_run"] == "1")
        # call order index: DFS preorder from TOP over direct edges first, then address-taken edges
        idx, seen, st = {}, set(), [TOP]
        while st:
            a = st.pop()
            if a in seen:
                continue
            seen.add(a)
            idx[a] = len(idx)
            kids = sorted(dedge.get(a, ())) + sorted(edges.get(a, set()) - dedge.get(a, set()))
            for c in reversed([c for c in kids if c in fset]):
                if c not in seen:
                    st.append(c)
        satall = set(fams) - {"game-c0c3"}
        ok_all = runnable(satall)
        rows = ["phase\tclass\tcount\trows (call order; [name, confidence, bytes])"]
        for ph, sel in (("menu", lambda v: v[0]), ("race_load", lambda v: v[1] and not v[0]),
                        ("race_run", lambda v: v[2] and not v[0] and not v[1])):
            ins = [a for a, v in rt.items() if sel(v) and a in reach]   # beneath a bound barrier = supplied by the binding
            by = collections.defaultdict(list)
            for a in ins:
                k = C[a]
                if k == "match":
                    k = "match-runnable" if a in ok_all else "match-blocked"
                by[k].append(a)
            for k in sorted(by, key=lambda k: -len(by[k])):
                lst = sorted(by[k], key=lambda a: idx.get(a, 10 ** 9))
                det = " ".join("0x%08x[%s,%s,%dB]" % (a, (H.get(a) or {}).get("name", "?"),
                                                      (H.get(a) or {}).get("confidence", "?"), bounds[a] - a)
                               for a in lst) if not k.startswith("match") and not k.startswith("bind:") else ""
                rows.append("%s\t%s\t%d\t%s" % (ph, k, len(lst), det))
        (OUT / "race_point.tsv").write_text("\n".join(rows) + "\n", encoding="utf-8")
        for l in rows:
            print(l[:400])
    if "--order" in argv:
        # call order (DFS preorder over direct edges, then reloc edges) from TOP, first non-runnable nodes
        seen, outl = set(), []

        def dfs(a, d):
            if a in seen or d > 40:
                return
            seen.add(a)
            k = C[a]
            st = "RUN" if a in final else ("BIND" if k.startswith("bind:") else k)
            outl.append("%s0x%08x %s %s" % ("  " * d, a, (H.get(a) or {}).get("name", "?"), st))
            if k.startswith("bind:") or k.startswith("fam:"):
                return
            for c in sorted(dedge.get(a, ()), key=lambda x: x) + sorted(redge.get(a, set()) - dedge.get(a, set())):
                if c in fset:
                    dfs(c, d + 1)
        dfs(TOP, 0)
        (OUT / "order_full.txt").write_text("\n".join(outl) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
