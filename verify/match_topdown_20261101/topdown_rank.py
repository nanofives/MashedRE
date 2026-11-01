"""MATCH-TOPDOWN-MAP, step 2: rank top-down swaps from the graphs cg_build.py wrote.

usage: py -3.12 topdown_rank.py <cg.json> <out_dir>

Definitions (all static, from the two graphs):
  M          matched set: byte-identical sources of MATCH-PILOT and S01..S04 (each slice's results.tsv).
  reach      the RVA's exe body (scripts/rva_body_scan.py, exe target, bound to its /MAP symbol) is reachable
             from _WinMain@16 in mashed_re.exe. An RVA with no RVA-bound exe body is never reached: the port
             runs that part of the game through RVA-less port code (exe_main, race/frontend TUs).
  platform   an original node the port keeps behind a bridge (third-party library rows, the audio / input /
             boot / video subsystems, file/piz I/O by name) or an import. Descent stops there.
  root r     a matched game node. U(r) = matched game nodes the exe does not run, reachable from r through
             such nodes only (r itself when unreached, or when reached with a port-only body). Swapping r and
             every port body in U(r) for its matched source makes the whole of U(r) called once r is called.
             Only maximal roots (no matched caller already holds r in its U) are kept.
  connector  cheapest path upward from r to an anchor: a node the exe runs (reached) or an original top (no game
             caller; its port counterpart is RVA-less port code). Cost = unmatched nodes on the path (anchor
             included when unmatched). Those are the gaps that block the swap: until they are matched, nothing
             the exe runs calls r.
  deep gaps  unmatched game callees of U(r): the swapped chain calls their existing port body (or needs one);
             each hides more matched functions below it.
Ranking: greedy set cover over U (each pick removes its nodes from the rest), keyed on unlock / (1 + connector
gaps), so every matched function is counted once and a ready swap outranks a bigger blocked one.
"""
import csv
import json
import os
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
sys.path.insert(0, str(WT / "scripts"))
import rva_body_scan  # noqa: E402

cg = json.load(open(sys.argv[1]))
OUT = Path(sys.argv[2])
nodes = cg["orig_nodes"]
oedges = cg["orig_edges"]


def norm(s):
    n = int(s, 16)
    return f"0x{(n + 0x400000 if n < 0x400000 else n):08x}"


# ---------------------------------------------------------------- matched set
M = {}


def addm(rva, src, slice_):
    M.setdefault(norm(rva), (slice_, src))


V = WT / "verify"
for r in csv.DictReader(open(V / "matchpilot_20261030/results.tsv"), delimiter="\t"):
    if r["best%"] == "100.0":
        addm(r["rva"], r["source"], "PILOT")
for r in csv.DictReader(open(V / "matchs01_20261031/results.tsv"), delimiter="\t"):
    if r["verdict"] == "BYTES-OK":
        addm(r["rva"], r["source"], "S01")
for s in ("02", "04"):
    for r in csv.DictReader(open(V / f"matchs{s}_20261031/results.tsv"), delimiter="\t"):
        if r["best%"] == "100.0":
            addm(r["rva"], r["source"], "S" + s)
for r in csv.DictReader(open(V / "matchs03_20261031/results.tsv"), delimiter="\t"):
    if r["byte_identical"] == "y":
        addm(r["rva"], r["file"], "S03")
reloc = {}
for s in ("01", "02", "04"):
    p = V / f"matchs{s}_20261031/relocheck.tsv"
    for ln in open(p):
        f = ln.rstrip("\n").split("\t")
        if f and f[0].startswith("0x"):
            reloc[norm(f[0])] = f[1]

# ---------------------------------------------------------------- exe bodies -> reach
scan = rva_body_scan.scan_all()
exe_tus = scan["targets"]["exe"]
sym_by_obj = defaultdict(dict)
for s in cg["exe_syms"]:
    obj = s["obj"].split(":")[-1].lower()
    for n in s["names"]:
        if n.startswith("?") and not n.startswith("??"):
            u = n[1:].split("@")[0]
        else:
            u = n.lstrip("_@").split("@")[0]
        sym_by_obj[obj].setdefault(u, s)
body = {}       # rva -> (tu, name, reach)
unmapped = []
for tu, b in scan["bodies"].items():
    if tu not in exe_tus:
        continue
    obj = (os.path.splitext(os.path.basename(tu))[0] + ".obj").lower()
    for rva, (nm, _ln) in b.items():
        key = f"0x{rva:08x}"
        s = sym_by_obj.get(obj, {}).get(nm)
        if s is None:
            unmapped.append((key, tu, nm))
            r = False
        else:
            r = s["reach"]
        prev = body.get(key)
        if prev is None or (r and not prev[2]):
            body[key] = (tu, nm, r)
reach = {k for k, v in body.items() if v[2]}

# exe callers of each reached body (objs), for the entry-point column
exe_va_of = {}
for key, (tu, nm, r) in body.items():
    obj = (os.path.splitext(os.path.basename(tu))[0] + ".obj").lower()
    s = sym_by_obj.get(obj, {}).get(nm)
    if s:
        exe_va_of[key] = s["va"]
sym_at = {s["va"]: s for s in cg["exe_syms"]}
rev = defaultdict(set)
for a, ts in cg["exe_edges"].items():
    for t in ts:
        rev[t].add(a)

# ---------------------------------------------------------------- platform classes
LIB = [("renderware]", "librw renderer (LibRw/RwBridge, rw shims)"),
       ("RenderWare-Physics", "RW Physics 3.7 library (port Physics TUs, x87)"),
       ("msvc-", "platform CRT (L-67 waived)"),
       ("lua-4.0", "Lua 4.0 (libid, vendored)"),
       ("qhull", "qhull 2002.1 (QhullBridge, libid)"),
       ("d3dx9", "D3DX9 (d3d9 renderer bridge)"),
       ("directshow", "DirectShow video (port video layer)"),
       ("libpng", "libpng/zlib"), ("zlib", "libpng/zlib")]
SUB = {"audio": "audio backend (port Audio/*)", "input": "input (port Input/*)",
       "boot": "window/boot (exe_main, Boot/*)", "video": "video (DirectShow layer)"}
RE_FILE = re.compile(r"piz|file|fopen|fread|fwrite|fclose|fseek", re.I)
DLL = {"d3d9.dll": "D3D9 device (d3d9 renderer bridge)", "dinput8.dll": "input (port Input/*)",
       "dsound.dll": "audio backend (port Audio/*)", "winmm.dll": "window/boot (timer, winmm)",
       "ole32.dll": "DirectShow video (port video layer)", "user32.dll": "window/boot (user32)",
       "gdi32.dll": "window/boot (gdi32)", "advapi32.dll": "window/boot (registry)"}


def plat(k):
    if not k.startswith("0x"):
        dll, fn = k.split("!", 1)
        if dll == "kernel32.dll":
            if re.search(r"File|Read|Write|SetFilePointer|FindFirst|FindNext|Directory", fn):
                return "file/piz I/O (port Util file layer)"
            return "window/boot (kernel32)"
        return DLL.get(dll, f"import {dll}")
    n = nodes.get(k)
    if n is None:
        return "unknown"
    sub = n["sub"]
    if sub.startswith("third-party"):
        for pat, lab in LIB:
            if pat in sub:
                return lab
        return "third-party " + sub
    if sub in SUB:
        return SUB[sub]
    if RE_FILE.search(n["name"]) and sub in ("util", "boot", "save"):
        return "file/piz I/O (port Util file layer)"
    return None


game = {k for k in nodes if plat(k) is None}
X87 = WT / "mashedmod" / "x87_tus.txt"
x87_subs = {"physics", "vehicle", "smplfzx"}


def callees(k):
    return [t for t in oedges.get(k, {}) if t != k]


# ---------------------------------------------------------------- pointer-installer edges
# re/analysis/pointer_callers_2026-10-11.tsv: installer function -> function reached through a stored pointer
# (vtable, fn table, callback arg, store_mem ...). Added as 'pc:<kind>' edges.
for r in csv.DictReader(open(WT / "re/analysis/pointer_callers_2026-10-11.tsv"), delimiter="\t"):
    if r["installer_fn"].startswith("0x"):
        a, t = norm(r["installer_fn"]), norm(r["target_rva"])
        if a in oedges and t != a:
            oedges[a].setdefault(t, "pc:" + r["kind"])

callers = defaultdict(set)
for a, e in oedges.items():
    for t in e:
        if t != a:
            callers[t].add(a)
mm = set(M)
orph = mm - reach                      # matched, the exe does not run their body
TOPS = [k for k in sorted(game) if not [c for c in callers.get(k, ()) if c in game]]


def gcallees(k):
    return [t for t in oedges.get(k, {}) if t != k and t in game]


def ready_set(r):
    """matched unreached game nodes reachable from r via matched unreached nodes (r included when unreached)."""
    s = {r} if r not in reach else set()
    st = [r]
    seen = {r}
    while st:
        a = st.pop()
        for t in gcallees(a):
            if t in seen or t in reach or t not in mm:
                continue
            seen.add(t)
            s.add(t)
            st.append(t)
    return s


# ---------------------------------------------------------------- connector: anchor -> r, fewest unmatched nodes
# 0-1 BFS upward from r over game callers. Cost of a node = 1 when unmatched (it is a gap: no matched source to
# swap in), 0 when matched. Stops at a reached node (the exe runs it: the anchor) or at an original top (a node
# with no game caller: 0x00492d30-style roots whose port counterpart is RVA-less port code, exe_main etc.).
from collections import deque


def connector(r):
    """anchor = a reached node or an original top. The anchor is the port entry point that gets the call
    (its port body, RVA-bound or RVA-less, is edited or swapped to call the chain), so it is not a gap."""
    dist = {r: 0}
    prev = {}
    anchors = []
    dq = deque([r])
    while dq:
        a = dq.popleft()
        d = dist[a]
        if a != r and a in reach:
            anchors.append(a)
            continue
        cs = sorted(c for c in callers.get(a, ()) if c in game)
        if not cs and a != r:
            anchors.append(a)
            continue
        for c in cs:
            w = 0 if (c in mm or c in reach) else 1
            nd = d + w
            if c not in dist or nd < dist[c]:
                dist[c] = nd
                prev[c] = a
                (dq.appendleft if w == 0 else dq.append)(c)
    best = None
    for anc in sorted(set(anchors)):
        path = [anc]
        ok = True
        while path[-1] != r:
            nx = prev.get(path[-1])
            if nx is None or nx in path:
                ok = False
                break
            path.append(nx)
        if not ok:
            continue
        gaps = [p for p in path[1:-1] if p not in mm]
        kind = "reached" if anc in reach else "top"
        key = (len(gaps), 0 if kind == "reached" else 1, len(path))
        if best is None or key < best[0]:
            best = (key, {"anchor": anc, "kind": kind, "path": path, "gaps": gaps})
    return best[1] if best else None


# ---------------------------------------------------------------- candidate roots and set cover
# A root is a matched node whose matched callers do not already contain it in their ready set (maximal).
cands = {}
for r in sorted(mm & game):
    U = ready_set(r)
    if r in reach and not body[r][0].startswith("Match/"):
        U = U | {r}       # reached, port-only body: the swap itself makes r's matched body the called one
    if not U:
        continue
    cands[r] = U
maximal = {}
for r, U in sorted(cands.items()):
    if any(r in cands[c] for c in callers.get(r, ()) if c in cands and c != r):
        continue
    maximal[r] = U

conn_cache = {r: connector(r) for r in maximal}
units = {}
for r, U in sorted(maximal.items()):
    cn = conn_cache[r]
    # key = (call site E, anchor): E is r's parent on the connector, i.e. the unmatched function (or the entry
    # point itself) whose port counterpart must call the matched chain. Gaps = unmatched nodes on the path
    # strictly above E up to the anchor, plus E when E is unmatched and not the anchor.
    if cn is None:
        key = (r, r)          # r has no game caller: dispatched indirectly; its own port call site is the entry
        gl = []
    else:
        E = cn["path"][-2]
        key = (E, cn["anchor"])
        gl = list(cn["gaps"])
    u = units.setdefault(key, {"roots": [], "U": set(), "gaps": set(), "paths": []})
    u["roots"].append(r)
    u["U"] |= U
    u["gaps"] |= set(gl)
    if cn:
        u["paths"].append(cn["path"])
taken = set()
order = []
pool = dict(maximal)
while pool:
    best, key = None, None
    for r, U in sorted(pool.items()):
        if r not in conn_cache:
            conn_cache[r] = connector(r)
        cn = conn_cache[r]
        g = len(cn["gaps"]) if cn else 99
        u = len(U - taken)
        if u == 0:
            continue
        kk = (u / (1 + g), u, -g)
        if key is None or kk > key:
            best, key = r, kk
    if best is None:
        break
    U = pool.pop(best) - taken
    order.append((best, U))
    taken |= U


def below(U):
    """unmatched game callees of the ready set, platform calls by bridge, and what each gap hides."""
    gaps, platc, platn = set(), Counter(), defaultdict(Counter)
    for a in sorted(U):
        for t in oedges.get(a, {}):
            if t == a:
                continue
            p = plat(t)
            if p is not None:
                platc[p] += 1
                platn[p][nodes[t]["name"] if t in nodes else t] += 1
            elif t not in mm and t not in reach:
                gaps.add(t)
    return gaps, platc, platn


def hidden_behind(g):
    s, st = set(), [g]
    seen = {g}
    while st:
        a = st.pop()
        for t in gcallees(a):
            if t in seen or t in reach:
                continue
            seen.add(t)
            if t in orph:
                s.add(t)
            st.append(t)
    return s


def exe_callers(r):
    va = exe_va_of.get(r)
    if not va:
        return []
    objs = Counter()
    for c in rev.get(va, ()):
        s = sym_at.get(c)
        if s and s["reach"]:
            objs[s["obj"].split(":")[-1]] += 1
    return [o for o, _ in objs.most_common(3)]


OUT.mkdir(parents=True, exist_ok=True)

# port TUs (exe target) that mention an RVA in text: where the RVA-less port counterpart of a call site lives.
RE_MENTION = re.compile(r"(?:0x|FUN_|sub_|_)00(4[0-9a-fA-F]{5}|5[0-9a-cA-C][0-9a-fA-F]{4})\b")
mention = defaultdict(Counter)
for tu in sorted(exe_tus):
    pth = rva_body_scan.SRC / tu
    if not pth.exists():
        continue
    for m_ in RE_MENTION.finditer(pth.read_text(encoding="utf-8", errors="replace")):
        mention["0x00" + m_.group(1).lower()][tu] += 1


def port_site(k):
    if k in body:
        return body[k][0] + (" (reached)" if k in reach else " (unreached)")
    c = mention.get(k)
    if not c:
        return "no exe mention"
    return "RVA-less; mentioned in " + ",".join(f"{t}x{n}" for t, n in c.most_common(2))


def any_callers(k):
    out = []
    for c in sorted(callers.get(k, ())):
        out.append(f"{c}:{nm(c)}[{nodes[c]['sub'] if c in nodes else '?'}]")
    return out


_x87 = {}
import capstone as _cs  # noqa: E402
import pefile as _pe  # noqa: E402

_ORIG = _pe.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
_MEM = _ORIG.get_memory_mapped_image()
_STARTS = sorted(int(k, 16) for k in nodes)
_MD = _cs.Cs(_cs.CS_ARCH_X86, _cs.CS_MODE_32)
_MD.detail = True


def is_x87(k):
    """the ORIGINAL function's bytes hold an x87 FPU instruction (capstone group FPU): its port wrapper TU
    must be listed in mashedmod/x87_tus.txt (the port's cl 19 default is SSE2)."""
    if k not in _x87:
        a = int(k, 16)
        i = _STARTS.index(a)
        end = _STARTS[i + 1] if i + 1 < len(_STARTS) else a + 0x1000
        code = _MEM[a - 0x400000:min(end, a + 0x10000) - 0x400000]
        _x87[k] = any(_cs.x86.X86_GRP_FPU in ins.groups for ins in _MD.disasm(code, a))
    return _x87[k]


def nm(k):
    return nodes[k]["name"] if k in nodes else k


rows = []
for i, (r, U) in enumerate(order, 1):
    cn = conn_cache[r]
    gaps, platc, platn = below(U)
    deep = sorted(gaps, key=lambda g: (-len(hidden_behind(g)), g))
    hid = {g: len(hidden_behind(g) - taken) for g in deep}
    no_body = [x for x in sorted(U) if x not in body]
    not_swapped = [x for x in sorted(U) if x in body and not body[x][0].startswith("Match/")]
    swapped = [x for x in sorted(U) if x in body and body[x][0].startswith("Match/")]
    subs = Counter(nodes[x]["sub"] for x in sorted(U))
    rows.append({
        "rank": i, "root": r, "root_name": nm(r), "root_sub": nodes[r]["sub"], "root_slice": M[r][0],
        "root_exe_body": body[r][0] if r in body else "-", "root_reached": "y" if r in reach else "n",
        "unlock": len(U),
        "anchor": cn["anchor"] if cn else "-", "anchor_name": nm(cn["anchor"]) if cn else "-",
        "anchor_kind": cn["kind"] if cn else "-",
        "anchor_exe_body": (body[cn["anchor"]][0] if cn and cn["anchor"] in body else "-"),
        "anchor_exe_callers": ",".join(exe_callers(cn["anchor"])) if cn else "",
        "connector_gaps": len(cn["gaps"]) if cn else -1,
        "connector_path": " > ".join(f"{p}{'' if p in mm else '*'}" for p in cn["path"]) if cn else "-",
        "deep_gaps": len(gaps),
        "deep_gaps_top": " ".join(f"{g}({hid[g]})" for g in deep[:6]),
        "new_exe_bodies": len(no_body), "replace_port_bodies": len(not_swapped), "already_swapped": len(swapped),
        "x87": "y" if any(nodes[x]["sub"] in x87_subs for x in U) else "n",
        "subsystems": " ".join(f"{k}:{v}" for k, v in subs.most_common()),
        "platform": "; ".join(f"{k} x{v}" for k, v in platc.most_common()),
        "platform_names": " | ".join(f"{k}: " + ",".join(n for n, _ in platn[k].most_common(3)) for k, _ in platc.most_common(4)),
        "unlock_rvas": " ".join(sorted(U)),
    })
cols = list(rows[0].keys())
with open(OUT / "topdown_roots.tsv", "w", newline="", encoding="utf-8") as f:
    w = csv.DictWriter(f, cols, delimiter="\t")
    w.writeheader()
    w.writerows(rows)

# ---------------------------------------------------------------- swap units (the Tnn candidates)
# unit = (entry point E, first original function R below E on the connector). The swap replaces the port's
# body for R (RVA-bound or RVA-less) with R's matched chain and makes E's port code call it. All matched
# functions in the unit's roots' U become called together. Greedy cover: largest new unlock first, ties on
# fewer blocking gaps.
utaken = set()
uorder = []
upool = dict(units)
while upool:
    best, key = None, None
    for k, u in sorted(upool.items()):
        n_ = len(u["U"] - utaken)
        if n_ == 0:
            continue
        kk = (n_, -len(u["gaps"]))
        if key is None or kk > key:
            best, key = k, kk
    if best is None:
        break
    u = upool.pop(best)
    uorder.append((best, u, u["U"] - utaken))
    utaken |= u["U"]
urows = []
for i, ((R, anc), u, newU) in enumerate(uorder, 1):
    gaps_deep, platc, platn = below(u["U"])
    deep = sorted(gaps_deep, key=lambda g: (-len(hidden_behind(g)), g))
    U = u["U"]
    urows.append({
        "unit_rank": i, "entry": anc, "entry_name": nm(anc),
        "entry_kind": ("reached" if anc in reach else ("matched-top" if anc == R else "top")) if anc != "-" else "-",
        "entry_port": port_site(anc) if anc != "-" else "-", "entry_matched": "y" if anc in mm else "n",
        "entry_callers": " ".join(any_callers(anc)[:4]) if anc != "-" else "",
        "call_site": R, "call_site_name": nm(R), "call_site_sub": nodes[R]["sub"] if R in nodes else "?",
        "call_site_matched": "y" if R in mm else "n", "call_site_port": port_site(R),
        "call_site_reached": "y" if R in reach else "n",
        "replaced_roots": " ".join(f"{x}:{nm(x)}" for x in sorted(u["roots"], key=lambda x: -len(maximal[x]))[:6]),
        "connector": " > ".join(f"{p}{'' if p in mm else '*'}" for p in (u["paths"][0][:-1] if u["paths"] else [])),
        "unlock_new": len(newU), "unlock_total": len(U), "roots": len(u["roots"]),
        "blocking_gaps": len(u["gaps"]), "blocking_gap_list": " ".join(f"{g}:{nm(g)}" for g in sorted(u["gaps"])),
        "deep_gaps": len(gaps_deep), "deep_gaps_top": " ".join(f"{g}({len(hidden_behind(g))})" for g in deep[:6]),
        "new_exe_bodies": sum(1 for x in U if x not in body),
        "replace_port_bodies": sum(1 for x in U if x in body and not body[x][0].startswith("Match/")),
        "already_swapped": sum(1 for x in U if x in body and body[x][0].startswith("Match/")),
        "x87_sources": sum(1 for x in U if is_x87(x)),
        "subsystems": " ".join(f"{k}:{v}" for k, v in Counter(nodes[x]["sub"] for x in sorted(U)).most_common()),
        "slices": " ".join(f"{k}:{v}" for k, v in Counter(M[x][0] for x in sorted(U)).most_common()),
        "platform": "; ".join(f"{k} x{v}" for k, v in platc.most_common()),
        "platform_names": " | ".join(f"{k}: " + ",".join(n for n, _ in platn[k].most_common(3)) for k, _ in platc.most_common(4)),
        "root_rvas": " ".join(sorted(u["roots"])),
        "unlock_rvas": " ".join(sorted(U)),
    })
with open(OUT / "topdown_ranked.tsv", "w", newline="", encoding="utf-8") as f:
    w = csv.DictWriter(f, list(urows[0].keys()), delimiter="	")
    w.writeheader()
    w.writerows(urows)

# gap value: unmatched game nodes ranked by matched orphans that sit directly under them through matched-only
# chains (what matching the gap would add to some swap). Feeds S05-S28 priority ("gaps under a matched caller first").
gv = []
for g in sorted(game - mm):
    s = set()
    for t in gcallees(g):
        if t in orph:
            s |= ready_set(t)
    s -= {g}
    if not s:
        continue
    mc = [c for c in callers.get(g, ()) if c in mm]
    gv.append((len(s), g, len(mc), s))
gv.sort(key=lambda x: (-x[0], x[1]))
SL = [(int(x["rva_first"], 16), int(x["rva_last"], 16), x["id"])
      for x in csv.DictReader(open(V / "matchscope_20261031/slices.tsv"), delimiter="\t")]


def slice_of(k):
    a = int(k, 16)
    return next((i for f_, l_, i in SL if f_ <= a <= l_), "none")

with open(OUT / "gap_value.tsv", "w", encoding="utf-8") as f:
    f.write("gap\tname\tsub\texe_body\treached\tmatched_callers\tunlocks_below\tslice_hint\n")
    for n_, g, nmc, s in gv:
        f.write(f"{g}\t{nm(g)}\t{nodes[g]['sub']}\t{body[g][0] if g in body else '-'}\t{'y' if g in reach else 'n'}\t{nmc}\t{n_}\t{slice_of(g)}\n")

with open(OUT / "reached_matched.tsv", "w", encoding="utf-8") as f:
    f.write("rva\tname\tsub\tslice\texe_body\tmatch_source\trelocheck\n")
    for k in sorted(mm & reach):
        f.write(f"{k}\t{nm(k)}\t{nodes[k]['sub'] if k in nodes else '?'}\t{M[k][0]}\t{body[k][0]}\t{M[k][1]}\t{reloc.get(k, '-')}\n")

summ = {
    "orig_nodes": len(nodes), "orig_edges": sum(len(e) for e in oedges.values()),
    "edge_kinds": dict(Counter(v if not v.startswith("pc:") else "pc" for e in oedges.values() for v in e.values())),
    "game_nodes": len(game), "platform_nodes": len(nodes) - len(game),
    "matched": len(mm), "matched_game": len(mm & game), "matched_by_slice": dict(Counter(v[0] for v in M.values())),
    "exe_bodies": len(body), "exe_bodies_reached": len(reach), "exe_body_unmapped": len(unmapped),
    "game_exe_bodies_reached": len(reach & game),
    "matched_with_exe_body": len(mm & set(body)), "matched_reached": len(mm & reach),
    "matched_unreached": len(orph), "matched_no_exe_body": len(mm - set(body)),
    "matched_swapped_in_exe": sum(1 for k in mm if k in body and body[k][0].startswith("Match/")),
    "matched_swapped_reached": sum(1 for k in mm if k in reach and body[k][0].startswith("Match/")),
    "roots_maximal": len(maximal), "units": len(urows), "units_ready": sum(1 for x in urows if x["blocking_gaps"] == 0), "ranked": len(rows), "covered": len(taken),
    "gaps_with_value": len(gv),
    "tops_with_matched_below": sorted([t for t in TOPS if hidden_behind(t)], key=lambda t: -len(hidden_behind(t)))[:10],
}
json.dump(summ, open(OUT / "summary.json", "w"), indent=1)
print(json.dumps(summ, indent=1))
