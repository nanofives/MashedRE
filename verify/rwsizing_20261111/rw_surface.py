"""RW-SIZING item 3: the option (A) binding surface and the hidden inline-offset readers.

  py -3.12 verify/rwsizing_20261111/rw_surface.py

Static, read-only. Needs RW_FUNCS.tsv (rw_census.py) for the RW band. Writes, beside this file:
  RW_SURFACE.tsv        every RW-band function a non-RW function in ORIG_TOP reach calls (direct or address-taken
                        edge), with its origtop_size class, BINDINGS kind, executed phase, component, RE state
  RW_BIND_CLASS.tsv     every MashedRw_* function in LibRw/RwApiBind.cpp: code lines, librw calls, offset accesses,
                        and the forward / hand-layout / hand-semantics / shim class (heuristic, rules below)
  RW_INLINE_READERS.tsv matched game functions in reach that dereference an RW object by a constant offset

Reach and classes: verify/portorigtop_20261109/origtop_size.py's (same graph, same BINDINGS / hooks inputs, same cls).

Inline-offset readers (two counted lower bounds, from the disassembly of the original, not the decompiler):
  T1 return taint  after `call <RW-band fn>` eax holds the RW return value; it is followed through register moves
                   (mov reg,reg) until overwritten or a call clobbers it. A memory operand [tainted_reg + disp] is an
                   inline field access of the returned object. Stack spills are not followed, nor are objects that
                   arrive as arguments or from game structs, so this is a lower bound.
  T2 RW globals    the function references an absolute address that only RW-band code writes (RwGlobals 0x007d3ff8,
                   the plugin-offset globals and other RW module state) and is not a constant: reading RW engine
                   state or a plugin offset inline.
"""
from __future__ import annotations

import collections
import csv
import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
sys.path.insert(0, str(ROOT / "verify" / "portorigtop_20261109"))
import port_closed_sets as P          # noqa: E402
import origtop_size as O              # noqa: E402

BIND_CPP = ROOT / "mashedmod" / "src" / "mashed_re" / "LibRw" / "RwApiBind.cpp"


def classes(H, B, by_rva, plat):
    def cls(a):
        k = B.get(a)
        if k in O.BIND_SAT:
            return "bind:" + k
        r = H.get(a)
        if k == "matched":
            return "match"
        if a in plat:
            return "fam:d3d-dshow"
        if r is None:
            return "norow"
        fam = O.family_of(r)
        if r["confidence"] == "C4" and a in by_rva:
            if fam and fam != "rwphysics":
                return "fam:" + fam
            return "match"
        if fam:
            return "fam:" + fam
        if r["confidence"] == "C4":
            return "game-c4-hand" if (r.get("exe_file") or "").strip() else "game-c4"
        return "game-c0c3"
    return cls


def bind_classes():
    src = BIND_CPP.read_text(encoding="utf-8", errors="replace").splitlines()
    out = []
    i = 0
    rx = re.compile(r"^(?:extern \"C\" )?[A-Za-z_][\w\s\*]*?\b(MashedRw_\w+)\s*\(([^;]*)$")
    while i < len(src):
        m = rx.match(src[i])
        if not m or src[i].rstrip().endswith(";"):
            i += 1
            continue
        name = m.group(1)
        depth, body, j, started = 0, [], i, False
        while j < len(src):
            line = src[j]
            depth += line.count("{") - line.count("}")
            if "{" in line:
                started = True
            body.append(line)
            if started and depth <= 0:
                break
            j += 1
        code = [l for l in body[1:] if l.strip() and not l.strip().startswith("//") and l.strip() not in ("{", "}")]
        text = "\n".join(code)
        rwcalls = len(re.findall(r"\brw::\w+(?:::\w+)*\s*\(|->\w+\(|\.\w+\(", text))
        librw = len(re.findall(r"\brw::", text))
        offs = len(re.findall(r"\+\s*0x[0-9a-fA-F]+\s*\)|\[\s*0x[0-9a-fA-F]+\s*\]|Off\w*\b|\bkRw\w*Off", text))
        # RW-layout record types this file defines (struct MashedRwCamera / MashedRwLight / MashedRwGlobals /
        # MashedRwImageRW / MashedRw*View / MashedRwPTankAtomic), or a raw read of the RwGlobals pointer 0x007d3ff8
        rwlayout = bool(re.search(r"\bMashedRw(Camera|Light|Globals|ImageRW|PTankAtomic|\w*View)\b|0x007d3ff8", text))
        if librw == 0 and not rwlayout:
            k = "shim"          # no librw call: stores, getters, no-ops, exe bodies
        elif offs or rwlayout:
            k = "hand-layout"
        elif len(code) <= 8:
            k = "forward"
        else:
            k = "hand-semantics"
        rva = re.search(r"_([0-9a-f]{8})$", name)
        out.append(dict(func=name, rva=("0x" + rva.group(1)) if rva else "-", line=i + 1, code_lines=len(code),
                        librw_refs=librw, offset_refs=offs, rwlayout_words="y" if rwlayout else "n", klass=k))
        i = j + 1
    return out, len(src)


def main(argv):
    ent, bounds = O.load_bounds()
    fset = set(ent)
    dedge = O.direct_edges()
    redge, _ = O.reloc_edges(O.EXE_DEF, ent, bounds)
    edges = collections.defaultdict(set)
    for src in (dedge, redge):
        for a, cs in src.items():
            edges[a] |= {c for c in cs if c in fset and c != a}
    H = P.load_hooks()
    files, by_rva = P.load_matched()
    B = O.load_bindings()
    plat = set()
    with open(ROOT / "re" / "console" / "match" / "platform_candidates.csv", newline="") as fh:
        for r in csv.DictReader(fh):
            if r["signal"] == "platform-api":
                plat.add(int(r["pc_va"], 16))
    cls = classes(H, B, by_rva, plat)
    C = {a: cls(a) for a in fset}
    RW = {}
    with open(HERE / "RW_FUNCS.tsv", newline="", encoding="utf-8") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            RW[int(r["rva"], 16)] = r
    reach, st = {O.TOP}, [O.TOP]
    while st:
        a = st.pop()
        if C[a].startswith("bind:") or C[a].startswith("fam:"):
            continue
        for c in edges.get(a, ()):
            if c not in reach:
                reach.add(c)
                st.append(c)
    print("reach:", len(reach), " fam:rwcore in reach:", sum(1 for a in reach if C[a] == "fam:rwcore"),
          " fam:rt2d:", sum(1 for a in reach if C[a] == "fam:rt2d"),
          " fam:rwphysics:", sum(1 for a in reach if C[a] == "fam:rwphysics"))
    # surface: RW-band callee of a non-RW-band function in reach that the route expands (not bind / fam)
    surf = collections.defaultdict(set)
    for a in reach:
        if a in RW or C[a].startswith("bind:") or C[a].startswith("fam:"):
            continue
        for c in edges.get(a, ()):
            if c in RW:
                surf[c].add(a)
    # RW-band nodes the route expands as if they were game code (matched RW rows labelled render/hud/...)
    expanded_rw = sorted(a for a in reach if a in RW and not C[a].startswith("bind:") and not C[a].startswith("fam:"))
    rows = []
    for c in sorted(surf):
        r = RW[c]
        rows.append(dict(rva="0x%08x" % c, name=r["name"], tracker_sub=r["tracker_sub"], component=r["component"],
                         route_class=C[c], binding=B.get(c, "-"), phase=r["phase"], conf=r["conf"],
                         mstate=r["mstate"], size=r["size"], game_callers_in_reach=len(surf[c]),
                         callers=" ".join("0x%08x" % x for x in sorted(surf[c])[:8])))
    with open(HERE / "RW_SURFACE.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, list(rows[0].keys()), delimiter="\t")
        w.writeheader()
        w.writerows(rows)
    print("surface (distinct RW-band callees of non-RW code in reach):", len(rows))
    print("  by route class:", collections.Counter(r["route_class"] for r in rows).most_common())
    print("  by binding kind:", collections.Counter(r["binding"] for r in rows).most_common())
    print("  by component:", collections.Counter(r["component"] for r in rows).most_common())
    print("  executed (any phase):", sum(1 for r in rows if r["phase"] not in ("not-run", "uncensused")))
    unb = [r for r in rows if r["route_class"] in ("fam:rwcore", "fam:rt2d")]
    print("  fam:rwcore+rt2d not bound:", len(unb), " of them executed:",
          sum(1 for r in unb if r["phase"] not in ("not-run", "uncensused")))
    print("RW-band nodes the route expands as game code (matched RW rows not labelled renderware):", len(expanded_rw),
          collections.Counter(C[a] for a in expanded_rw).most_common())

    def grp(a):
        if RW[a]["component"] == "rwphysics":
            return "rwphysics"
        if 0x004c3700 <= a < 0x004c5800:
            return "math-band"
        return RW[a]["component"]
    with open(HERE / "RW_EXPANDED_ON_ROUTE.tsv", "w", newline="", encoding="utf-8") as fh:
        fh.write("rva\tname\ttracker_sub\tgroup\troute_class\tbinding\tphase\n")
        for a in expanded_rw:
            fh.write("0x%08x\t%s\t%s\t%s\t%s\t%s\t%s\n" % (a, RW[a]["name"], RW[a]["tracker_sub"], grp(a), C[a],
                                                        B.get(a, "-"), RW[a]["phase"]))
    obj = [a for a in expanded_rw if grp(a) not in ("rwphysics", "math-band", "rtmath")]
    print("  object-touching (not math / physics):", len(obj), collections.Counter(C[a] for a in obj).most_common(),
          " executed:", sum(1 for a in obj if RW[a]["phase"] not in ("not-run", "uncensused")))
    # ---- bindings ----
    bc, nlines = bind_classes()
    with open(HERE / "RW_BIND_CLASS.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, list(bc[0].keys()), delimiter="\t")
        w.writeheader()
        w.writerows(bc)
    print("\nRwApiBind.cpp lines:", nlines, " MashedRw_ functions:", len(bc),
          collections.Counter(b["klass"] for b in bc).most_common(),
          " code lines in them:", sum(b["code_lines"] for b in bc))
    # ---- inline readers ----
    import capstone
    import pefile
    pe = pefile.PE(str(O.EXE_DEF), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    X = capstone.x86
    # writers of absolute addresses, over every function
    writers = collections.defaultdict(set)
    refs = collections.defaultdict(set)
    dis = {}
    for f in ent:
        ins_l = list(md.disasm(img[f - base:bounds[f] - base], f))
        dis[f] = ins_l
        for ins in ins_l:
            for k, op in enumerate(ins.operands):
                if op.type == X.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and op.mem.disp:
                    d = op.mem.disp & 0xffffffff
                    refs[f].add(d)
                    if k == 0 and ins.mnemonic.startswith(("mov", "add", "sub", "inc", "dec", "or", "and", "xor",
                                                           "fst", "fist", "set")):
                        writers[d].add(f)
    rw_owned = {d for d, ws in writers.items() if ws and all(w in RW for w in ws) and 0x00600000 <= d < 0x00a00000}
    clobber = (X.X86_REG_EAX, X.X86_REG_ECX, X.X86_REG_EDX)
    out = []
    game_matched = [a for a in reach if C[a] == "match" and a not in RW]
    t1_src = {}
    for f in sorted(game_matched):
        taint = {}
        sites = collections.Counter()
        offs = collections.defaultdict(set)
        for ins in dis[f]:
            ops = ins.operands
            # uses
            for op in ops:
                if op.type == X.X86_OP_MEM and op.mem.base in taint and op.mem.index == 0 and ins.mnemonic != "lea":
                    src = taint[op.mem.base]
                    sites[src] += 1
                    offs[src].add(op.mem.disp)
            if ins.mnemonic == "call":
                tgt = ops[0].imm & 0xffffffff if ops and ops[0].type == X.X86_OP_IMM else None
                for r in clobber:
                    taint.pop(r, None)
                if tgt in RW:
                    taint[X.X86_REG_EAX] = tgt
                continue
            if ins.mnemonic == "mov" and len(ops) == 2 and ops[0].type == X.X86_OP_REG:
                if ops[1].type == X.X86_OP_REG and ops[1].reg in taint:
                    taint[ops[0].reg] = taint[ops[1].reg]
                elif ops[1].type == X.X86_OP_MEM and ops[1].mem.base == 0 and ops[1].mem.index == 0 and                         (ops[1].mem.disp & 0xffffffff) in rw_owned:
                    taint[ops[0].reg] = -(ops[1].mem.disp & 0xffffffff)   # negative = an RW-owned global's value
                else:
                    taint.pop(ops[0].reg, None)
                continue
            if ops and ops[0].type == X.X86_OP_REG and ins.mnemonic not in ("cmp", "test", "push"):
                taint.pop(ops[0].reg, None)
        g = sorted(d for d in refs[f] if d in rw_owned)
        t1_src[f] = {k for k in sites if k > 0}
        gsites = {k: v for k, v in sites.items() if k < 0}
        sites = collections.Counter({k: v for k, v in sites.items() if k > 0})
        if sites or g:
            r = H.get(f) or {}
            out.append(dict(rva="0x%08x" % f, name=r.get("name", "-"), subsystem=r.get("subsystem", "-"),
                            size=bounds[f] - f, t1_sites=sum(sites.values()),
                            t1_objects=" ".join("%s%s" % (RW[s]["name"] if RW[s]["name"] != "-" else "0x%08x" % s,
                                                          "[" + ",".join("+0x%x" % (o & 0xffffffff) for o in sorted(offs[s])) + "]")
                                                for s in sorted(sites)),
                            t2_rw_globals=" ".join("0x%08x" % d for d in g),
                            t2_deref=" ".join("0x%08x[%s]" % (-k, ",".join("+0x%x" % (o & 0xffffffff) for o in sorted(offs[k])))
                                              for k in sorted(gsites)) or "-",
                            binding=B.get(f, "-")))
    # ---- T3: matched C source (re/match bodies). An identifier that is passed to an RW-band function or assigned
    # from one, and is dereferenced at a constant offset in the same body (estimate: identifier level, reassignment
    # not tracked, so it can over- and under-count; reported separately from T1/T2).
    rwhex = {"%08x" % a for a in RW}
    deref_rx = r"\(\s*(?:\([^()]*\)\s*)?{id}\s*\+\s*(0x[0-9a-fA-F]+|\d+)\s*\)|\b{id}\s*\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]|\b{id}->(\w+)"
    t3 = {}
    for f in sorted(game_matched):
        fl = by_rva.get(f, [])
        if not fl:
            continue
        body = files[fl[0]][1].get(f, "")
        calls = list(re.finditer(r"\b(?:FUN|sub|thunk_FUN)_([0-9a-fA-F]{8})\s*\(([^;]*)", body))
        ids = collections.defaultdict(set)
        for m in calls:
            if m.group(1).lower() not in rwhex:
                continue
            for idm in re.finditer(r"\b(param_\d+|[a-zA-Z_]\w*Var\d+|local_\w+|[a-z]\w{0,12})\b", m.group(2)):
                ids[idm.group(1)].add(m.group(1).lower())
            pre = body[max(0, m.start() - 80):m.start()]
            am = re.search(r"\b(\w+)\s*=\s*(?:\([^()]*\)\s*)?$", pre)
            if am:
                ids[am.group(1)].add(m.group(1).lower())
        hits = {}
        for idn, srcs in ids.items():
            if idn in ("int", "void", "char", "float", "return", "if", "while", "for", "uint", "byte", "undefined4",
                       "short", "ushort", "dword", "bool", "unsigned", "sizeof", "struct"):
                continue
            o = set()
            for dm in re.finditer(deref_rx.format(id=re.escape(idn)), body):
                v = dm.group(1) or dm.group(2) or dm.group(3)
                o.add(v)
            if o:
                hits[idn] = (srcs, o)
        if hits:
            t3[f] = hits
    for r in out:
        f = int(r["rva"], 16)
        r["t3_ids"] = " ".join("%s{%s}[%s]" % (k, ",".join(sorted(v[0])), ",".join(sorted(v[1])[:6]))
                               for k, v in sorted(t3.get(f, {}).items())) or "-"
    have = {r["rva"] for r in out}
    for f, hits in t3.items():
        if "0x%08x" % f in have:
            continue
        r = H.get(f) or {}
        out.append(dict(rva="0x%08x" % f, name=r.get("name", "-"), subsystem=r.get("subsystem", "-"),
                        size=bounds[f] - f, t1_sites=0, t1_objects="", t2_rw_globals="", t2_deref="-",
                        binding=B.get(f, "-"),
                        t3_ids=" ".join("%s{%s}[%s]" % (k, ",".join(sorted(v[0])), ",".join(sorted(v[1])[:6]))
                                        for k, v in sorted(hits.items()))))
    print("T3 (matched C source: RW-flow identifier dereferenced at a constant offset):", len(t3),
          " new beyond T1/T2:", sum(1 for f in t3 if "0x%08x" % f not in have))
    out.sort(key=lambda r: -r["size"])
    with open(HERE / "RW_INLINE_READERS.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, list(out[0].keys()), delimiter="\t")
        w.writeheader()
        w.writerows(out)
    # librw-relevant: the RW source is an object API, not the plain-data math band [0x004c3700, 0x004c5800)
    # (RwV3d* / RwMatrix* / FastSqrt: same layout in librw) and not RW-Physics / RtQuat-RtSpline-RtRandom (not in
    # librw: ported like game code, L-98)
    def objapi(c):
        rr = RW.get(c)
        return rr is not None and not (0x004c3700 <= c < 0x004c5800) and rr["component"] not in ("rwphysics", "rtmath")
    for r in out:
        f = int(r["rva"], 16)
        srcs = set()
        for o in r["t1_objects"].split():
            pass
        t1src = {k for k in t1_src.get(f, ())}
        t3src = {int(c, 16) for v in t3.get(f, {}).values() for c in v[0]}
        r["obj_t1"] = " ".join("0x%08x" % c for c in sorted(c for c in t1src if objapi(c))) or "-"
        r["obj_t3"] = " ".join("0x%08x" % c for c in sorted(c for c in t3src if objapi(c))) or "-"
        r["phys_or_math_only"] = "y" if (r["obj_t1"] == "-" and r["obj_t3"] == "-" and not r["t2_rw_globals"]) else "n"
        r["librw_relevant"] = "n" if r["phys_or_math_only"] == "y" else "y"
    lr = [r for r in out if r["librw_relevant"] == "y"]
    print("librw-relevant inline readers (object API or RW-owned global):", len(lr),
          " via T1 obj:", sum(1 for r in lr if r["obj_t1"] != "-"), " via T2:", sum(1 for r in lr if r["t2_rw_globals"]),
          " via T3 obj:", sum(1 for r in lr if r["obj_t3"] != "-"),
          " only RwGlobals (T2, no object):", sum(1 for r in lr if r["obj_t1"] == "-" and r["obj_t3"] == "-"),
          " bytes:", sum(r["size"] for r in lr))
    print("physics / math only (not a librw layout question):", sum(1 for r in out if r["librw_relevant"] == "n"))
    t1 = [r for r in out if r["t1_sites"]]
    t3r = [r for r in out if r.get("t3_ids", "-") != "-"]
    print("union T1|T2|T3:", len(out), " T3 rows:", len(t3r))
    t2 = [r for r in out if r["t2_rw_globals"]]
    print("\nmatched game functions in reach:", len(game_matched))
    print("RW-owned globals (written only by RW-band code):", len(rw_owned))
    print("inline readers: T1 (deref of an RW return value):", len(t1), " sites:", sum(r["t1_sites"] for r in t1))
    print("                T2 (reads RW-owned globals):", len(t2), " either:", len(out),
          " both:", sum(1 for r in out if r["t1_sites"] and r["t2_rw_globals"]))
    g2 = collections.Counter(d for r in t2 for d in r["t2_rw_globals"].split())
    print("  most-read RW globals:", g2.most_common(12))
    rg = collections.Counter()
    for r in t2:
        for x in r["t2_deref"].split():
            if x.startswith("0x007d3ff8["):
                for o in x[11:-1].split(","):
                    rg[o] += 1
    print("  RwGlobals offsets read inline (functions per offset):", sorted(rg.items(), key=lambda kv: int(kv[0], 16)))
    print("  distinct RwGlobals offsets:", len(rg))
    for k in ("0x004b3f90", "0x0048fef0", "0x004b5190", "0x004b52f0", "0x004b5300"):
        hit = [r for r in out if r["rva"] == k]
        print("  check", k, "->", (hit[0]["t1_objects"] + " | " + hit[0]["t2_deref"] + " | " + hit[0]["t3_ids"]) if hit else "not found",
              "class", C.get(int(k, 16)), "binding", B.get(int(k, 16), "-"))
    lr.sort(key=lambda r: -r["size"])
    with open(HERE / "RW_INLINE_READERS.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, list(out[0].keys()), delimiter="	")
        w.writeheader()
        w.writerows(sorted(out, key=lambda r: (r["librw_relevant"] != "y", -r["size"])))
    print("  game functions calling any RW-band fn (candidate set):",
          sum(1 for f in game_matched if any(c in RW for c in edges.get(f, ()))))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
