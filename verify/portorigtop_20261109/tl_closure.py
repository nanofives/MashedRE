"""PORT-OT-TRACKLOAD: closure of a root under the origtop_size.py edges and classes. Static, read-only.

  py -3.12 verify/portorigtop_20261109/tl_closure.py 0x00426e10 0x0040cfd0 0x0040d270

Same edges as origtop_size.py (E8 rel32 + relocated function pointers / tables). Classes as there, plus:
  bind:matched  BINDINGS kind `matched` (already an exe body on the ORIG_TOP path): not expanded
  standin       BINDINGS kind `stand-in` (expanded: the stand-in is what would be replaced)
  match         hooks C4 with a matched body and no exe binding yet
Expansion stops at bind:* and fam:* nodes, except the root itself.
"""
import collections
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "verify" / "portorigtop_20261109"))
sys.path.insert(0, str(ROOT / "re" / "tools"))
import origtop_size as S  # noqa: E402

P = S.P
ent, bounds = S.load_bounds()
fset = set(ent)
dedge = S.direct_edges()
redge, _ = S.reloc_edges(S.EXE_DEF, ent, bounds)
edges = collections.defaultdict(set)
for src in (dedge, redge):
    for a, cs in src.items():
        edges[a] |= {c for c in cs if c in fset and c != a}
H = P.load_hooks()
_f, by_rva = P.load_matched()
B = S.load_bindings()


def cls(a):
    k = B.get(a)
    if k in S.BIND_SAT:
        return "bind:" + k
    if k == "matched":
        return "bind:matched"
    if k == "stand-in":
        return "standin"
    r = H.get(a)
    if r is None:
        return "norow"
    fam = S.family_of(r)
    if r["confidence"] == "C4" and a in by_rva:
        if fam and fam != "rwphysics":
            return "fam:" + fam
        return "match"
    if fam:
        return "fam:" + fam
    if r["confidence"] == "C4":
        return "game-c4-hand" if (r.get("exe_file") or "").strip() else "game-c4"
    return "game-c0c3"


for root in sys.argv[1:]:
    R = int(root, 16)
    reach, st = {R}, [R]
    while st:
        a = st.pop()
        if a != R and (cls(a).startswith("bind:") or cls(a).startswith("fam:")):
            continue
        for c in edges.get(a, ()):
            if c not in reach:
                reach.add(c)
                st.append(c)
    cnt = collections.Counter(cls(a) for a in reach)
    print(f"== root 0x{R:08x}: {len(reach)} nodes  " + "  ".join(f"{k}={v}" for k, v in sorted(cnt.items())))
    direct = sorted(edges.get(R, ()))
    print("  direct callees:", len(direct))
    for c in direct:
        r = H.get(c) or {}
        print(f"   0x{c:08x} {cls(c):14s} {r.get('confidence', '-'):3s} {r.get('name', '')}")
    for k in ("game-c0c3", "game-c4", "norow", "standin"):
        xs = sorted(a for a in reach if cls(a) == k)
        print(f"  {k} ({len(xs)}):", " ".join(f"0x{a:08x}" for a in xs))
    print("  families:", sorted({cls(a) for a in reach if cls(a).startswith("fam:")}))
