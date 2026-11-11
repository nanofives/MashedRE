"""RW-SIZING item 1 + 2: every RenderWare-band function, whether the original executes it, its component, its RE /
match state, and whether it calls d3d9 through a COM vtable or calls d3dx.

  py -3.12 verify/rwsizing_20261111/rw_census.py

Static, read-only. Writes verify/rwsizing_20261111/RW_FUNCS.tsv and prints the summary tables.

RW band (by address, per re/analysis/library_relabel_2026-10-11.md "Method": the library side is decided by range):
  core      [0x004c0700, 0x004ec760)                       every function (game-labelled rows there are RW mislabels)
  mid       [0x004ec760, 0x00516000)                       rows not labelled d3dx9-* / libpng (the RW D3D9 backend
                                                           rows between the d3dx9 bands); no-row functions skipped
  upper     [0x0052df70, 0x00558c50)                       every function (first `renderware` row is 0x0052df70; the
                                                           rows below it are zlib / image-library residue)
  physics   [0x00558c50, 0x0057c5b0)                       RenderWare-Physics-3.7 (qhull starts at 0x0057c5b0)
  plus any row labelled third-party-library[renderware] / [RenderWare-Physics-3.7] outside those bands.

Executed = re/coverage/reach.tsv (INT3 on every function entry the 2026-10-11 census knew, majority of 3 runs).
That census is not bounded by bound entry points: it covers RW internals directly. Functions with no reach.tsv row
(created in Ghidra after the census) are `uncensused`; for them `static_below` says whether they are in the static
callee closure of an executed RW function (direct E8 + address-taken edges, origtop_size.py's graph).

Components (heuristic, an estimate; keywords over hooks.csv name + notes, then band):
  rwphysics  physics band or RenderWare-Physics label
  d3d9       body makes a COM vcall (global -> object -> vtable -> call [vtbl+disp]) on a D3D global 0x007d41xx,
             or references the device pointer 0x007d4110, or calls the Direct3DCreate9 import
  rt2d       Rt2d / RtCharset / font toolkit (keywords Rt2d, Font, Charset, glyph), or the 0x00552700..0x005585ff band
  rtfs       RtFS file system toolkit (RtFS, FsManager, Vfs, RWS stream)
  rtmath     RtQuat / RtSlerp / RtSpline / RtRandom (keywords Quat, SLERP, spline, RNG)
  skin-anim  RpSkin / RpHAnim / RtAnim (keywords skin, HAnim, anim, keyframe)
  world-bsp  RpWorld / sectors / BSP / collision (keywords World, Sector, BSP, collision)
  im2d-im3d  RwIm2D / RwIm3D (keywords Im2D, Im3D, immediate)
  rwcore     everything else in the band
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

EXE = O.EXE_DEF
DEVICE = 0x007d4110
D3D_GLOBALS = range(0x007d4100, 0x007d4200)

KW = [
    ("rt2d", r"Rt2d|Font|Charset|glyph"),
    ("rtfs", r"RtFS|FsManager|\bVfs|Rws?Stream|RtFSHandler"),
    ("rtmath", r"Quat|SLERP|[Ss]pline|\bRNG\b|RtRandom|RtSlerp"),
    ("skin-anim", r"[Ss]kin\b|HAnim|RpSkin|RtAnim|keyframe|Hierarchy"),
    ("im2d-im3d", r"Im2D|Im3D|im2d|im3d|[Ii]mmediate"),
    ("world-bsp", r"RpWorld|[Ww]orldSector|\bBSP\b|[Ss]ector\b|RpCollision|collision"),
]


def band(a, r):
    s = (r or {}).get("subsystem", "")
    if 0x004c0700 <= a < 0x004ec760:
        return "core"
    if 0x004ec760 <= a < 0x00516000:
        if r and ("d3dx9" in s or "libpng" in s or "msvc" in s):
            return ""
        return "mid" if r else ""
    if 0x0052df70 <= a < 0x00558c50:
        return "upper"
    if 0x00558c50 <= a < 0x0057c5b0:
        return "physics"
    if "renderware" in s or "RenderWare-Physics" in s:
        return "labelled"
    return ""


def load_ledger():
    m = {}
    with open(ROOT / "re" / "analysis" / "match_ledger.tsv", newline="", encoding="utf-8") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            try:
                a = int(r["rva"], 16)
                p = float(r["best_pct"] or 0)
            except ValueError:
                continue
            if a not in m or p > m[a][0]:
                m[a] = (p, r["bytes_n"], r["verdict"], r["relocheck"])
    return m


def load_reach():
    rt = {}
    with open(ROOT / "re" / "coverage" / "reach.tsv", newline="") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            a = int(r["rva"], 16)
            if a < 0x00400000:
                a += 0x00400000
            rt[a] = (r["reached_menu"] == "1", r["reached_race_load"] == "1", r["reached_race_run"] == "1")
    return rt


def com_scan(exe, ent, bounds, fns):
    """Per function: set of globals G used as `mov r1,[G]; mov r2,[r1]; call [r2+disp]` (COM object in a global),
    whether it references DEVICE, and IAT-call targets."""
    import capstone
    import pefile
    pe = pefile.PE(str(exe), fast_load=True)
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]])
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    iat = {}
    for d in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
        for i in d.imports:
            iat[i.address] = "%s!%s" % (d.dll.decode(), (i.name or b"?").decode())
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    out = {}
    for f in fns:
        code = img[f - base:bounds[f] - base]
        regsrc = {}      # reg -> ("g", G) loaded from global | ("o", G) object deref of a global
        com, dev, imps, vcalls = set(), False, set(), 0
        for ins in md.disasm(code, f):
            ops = ins.operands
            for op in ops:
                if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                    d = op.mem.disp & 0xffffffff
                    if d == DEVICE:
                        dev = True
                    if d in iat and ins.mnemonic in ("call", "jmp"):
                        imps.add(iat[d])
                elif op.type == capstone.x86.X86_OP_IMM and (op.imm & 0xffffffff) == DEVICE:
                    dev = True
            if ins.mnemonic == "mov" and len(ops) == 2 and ops[0].type == capstone.x86.X86_OP_REG:
                dst = ops[0].reg
                s = ops[1]
                if s.type == capstone.x86.X86_OP_MEM and s.mem.base == 0 and s.mem.index == 0:
                    regsrc[dst] = ("g", s.mem.disp & 0xffffffff)
                elif s.type == capstone.x86.X86_OP_MEM and s.mem.index == 0 and s.mem.disp == 0 and \
                        regsrc.get(s.mem.base, ("", 0))[0] == "g":
                    regsrc[dst] = ("o", regsrc[s.mem.base][1])
                else:
                    regsrc.pop(dst, None)
            elif ins.mnemonic == "call" and len(ops) == 1 and ops[0].type == capstone.x86.X86_OP_MEM and ops[0].mem.base:
                vcalls += 1
                src = regsrc.get(ops[0].mem.base)
                if src and src[0] == "o":
                    com.add(src[1])
            elif ins.mnemonic == "call":
                for rr in (capstone.x86.X86_REG_EAX, capstone.x86.X86_REG_ECX, capstone.x86.X86_REG_EDX):
                    regsrc.pop(rr, None)
        out[f] = (com, dev, imps, vcalls)
    return out


def component(a, r, bd, comscan):
    s = (r or {}).get("subsystem", "")
    if bd == "physics" or "RenderWare-Physics" in s:
        return "rwphysics"
    com, dev, imps, _ = comscan.get(a, (set(), False, set(), 0))
    if dev or any(g in D3D_GLOBALS for g in com) or any("d3d9" in i.lower() for i in imps):
        return "d3d9"
    txt = ((r or {}).get("name", "") or "") + " " + ((r or {}).get("notes", "") or "")[:400]
    if 0x00552700 <= a < 0x00558600:
        return "rt2d"
    for k, rx in KW:
        if re.search(rx, txt):
            return k
    return "rwcore"


def main(argv):
    ent, bounds = O.load_bounds()
    fset = set(ent)
    H = P.load_hooks()
    dedge = O.direct_edges()
    redge, _nt = O.reloc_edges(EXE, ent, bounds)
    edges = collections.defaultdict(set)
    for src in (dedge, redge):
        for a, cs in src.items():
            edges[a] |= {c for c in cs if c in fset and c != a}
    _files, by_rva = P.load_matched()
    L = load_ledger()
    rt = load_reach()
    B = O.load_bindings()
    rw = {a: band(a, H.get(a)) for a in ent}
    rw = {a: b for a, b in rw.items() if b}
    cs = com_scan(EXE, ent, bounds, sorted(rw))
    d3dx = {a for a, r in H.items() if "d3dx9" in r.get("subsystem", "")}
    executed = {a for a in rw if a in rt and any(rt[a])}
    # static closure below every executed RW function (RW-band nodes only)
    below, st = set(executed), list(executed)
    while st:
        a = st.pop()
        for c in edges.get(a, ()):
            if c in rw and c not in below:
                below.add(c)
                st.append(c)
    rev = collections.defaultdict(set)
    for x, cs_ in edges.items():
        for c in cs_:
            rev[c].add(x)
    rows = []
    for a in sorted(rw):
        r = H.get(a)
        v = rt.get(a)
        if v is None:
            ph = "uncensused"
        else:
            ph = "+".join(n for n, x in zip(("menu", "race_load", "race_run"), v) if x) or "not-run"
        comp = component(a, r, rw[a], cs)
        com, dev, imps, vc = cs.get(a, (set(), False, set(), 0))
        lp = L.get(a)
        conf = (r or {}).get("confidence", "") or "NOROW"
        matched = a in by_rva and conf == "C4"
        if conf == "C4" and lp and lp[0] >= 100.0:
            mstate = "C4-match"
        elif conf == "C4" and matched:
            mstate = "C4-match"
        elif conf == "C4":
            mstate = "C4-other" if not lp else ("C4-near" if lp[0] >= 95 else "C4-lower")
        elif lp and lp[0] >= 95:
            mstate = "near95"
        elif lp:
            mstate = "lower"
        elif conf == "NOROW":
            mstate = "norow"
        else:
            mstate = "never"
        callers = sorted(rev.get(a, ()))
        game_callers = [x for x in callers if x not in rw]
        dx = sorted(c for c in edges.get(a, ()) if c in d3dx)
        rows.append(dict(
            rva="0x%08x" % a, name=(r or {}).get("name", "-"), tracker_sub=(r or {}).get("subsystem", "-"),
            band=rw[a], component=comp, conf=conf, phase=ph,
            static_below="y" if a in below else "n",
            size=bounds[a] - a, best_pct="%.1f" % lp[0] if lp else "-", match_bytes=lp[1] if lp else "-",
            mstate=mstate, binding=B.get(a, "-"),
            d3d_com=" ".join("0x%08x" % g for g in sorted(com)) or "-", device_ref="y" if dev else "n",
            imports=" ".join(sorted(imps)) or "-", vcalls=vc,
            d3dx_callees=" ".join("0x%08x" % c for c in dx) or "-",
            game_callers=len(game_callers),
            game_callers_executed=sum(1 for x in game_callers if x in rt and any(rt[x])),
        ))
    cols = list(rows[0].keys())
    with open(HERE / "RW_FUNCS.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, cols, delimiter="\t")
        w.writeheader()
        w.writerows(rows)
    ex = [r for r in rows if r["phase"] not in ("not-run", "uncensused")]
    print("RW-band functions:", len(rows), " executed:", len(ex),
          " uncensused:", sum(1 for r in rows if r["phase"] == "uncensused"),
          " uncensused and static_below:", sum(1 for r in rows if r["phase"] == "uncensused" and r["static_below"] == "y"))
    print("\nexecuted by component x mstate (count / bytes)")
    t = collections.defaultdict(lambda: [0, 0])
    for r in ex:
        t[(r["component"], r["mstate"])][0] += 1
        t[(r["component"], r["mstate"])][1] += r["size"]
    comps = sorted({r["component"] for r in ex})
    ms = ["C4-match", "C4-near", "C4-lower", "C4-other", "near95", "lower", "never", "norow"]
    print("component\t" + "\t".join(ms) + "\ttotal")
    for c in comps:
        tot = [sum(t[(c, m)][0] for m in ms), sum(t[(c, m)][1] for m in ms)]
        print(c + "\t" + "\t".join("%d/%d" % tuple(t[(c, m)]) for m in ms) + "\t%d/%d" % tuple(tot))
    print("\nexecuted by phase:", collections.Counter(r["phase"] for r in ex))
    print("executed d3d9 COM callers:", sum(1 for r in ex if r["component"] == "d3d9"),
          " executed calling d3dx:", sum(1 for r in ex if r["d3dx_callees"] != "-"))
    g = collections.Counter()
    for r in rows:
        for x in r["d3d_com"].split():
            if x != "-":
                g[x] += 1
    print("COM globals (all RW-band fns):", g.most_common(20))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
