# drawlist_diff.py — align and diff two Im2D draw streams (original MASHED vs
# standalone mashed_re.exe) at a nav-matched screen/frame.
#
# This is the composition-layer verifier the per-function bit-identity diffs
# cannot provide: a C4 function called with the wrong texture, at the wrong
# place, in the wrong order, or not at all shows up here as a structured
# MISSING / EXTRA / MISMATCH row instead of needing a human eye on a screenshot.
#
# Accepted input record shapes (auto-detected per record):
#   decoded  {"x":..,"y":..,"w":..,"h":..,"argb":"..","corner_colors":[..],"rets":[..]}
#            (log/menu_draw_dump.json — re/frida/menu_draw_dump.py output)
#   raw      {"v":"<hex of N*0x1c vert bytes>","r":[..],"s":[tex,alpha,src,dst]}
#            (log/drawstream_re.json — MASHED_DBG_DRAWSTREAM standalone dump,
#             and re/frida/menu_draw_burst.py original bursts)
#
# Coordinate spaces: the original renders 640x480 (d3d9 shim) virtual space;
# the standalone renders 800x600 (kVScale = 1.25). Default --scale-b 0.8 maps
# the standalone (side B) back into the original's virtual space. Pass
# --scale-b 1 when comparing like-vs-like (e.g. two standalone builds).
#
# Color bytes are compared RAW (both sides' draw code writes the identical RW
# Im2D vertex layout, so channel order matches across sides by construction).
#
# Usage:
#   py -3.12 re/tools/drawlist_diff.py A.json B.json
#       [--label-a scr1] [--label-b f200]      # default: pair labels in order
#       [--scale-a 1.0] [--scale-b 0.8]
#       [--tol-pos 1.0] [--tol-size 1.0] [--tol-color 0] [--tol-alpha 0]
#       [--min-alpha 0]                        # drop fully-faded draws first
#       [--map mashedmod/build/mashed_re.map]  # resolve side-B retaddrs
#       [--json out.json] [--max-rows 40]
#
# Exit code 0 = GREEN (every draw matched within tolerances), 1 = divergence,
# 2 = usage/input error. GREEN here is the acceptance gate for composition
# fixes (re/analysis/parity_tooling.md).
import argparse
import bisect
import json
import struct
import sys
from pathlib import Path

VERT_STRIDE = 0x1c


# ---------------------------------------------------------------- normalize

def decode_raw_blob(hexstr):
    raw = bytes.fromhex(hexstr)
    n = len(raw) // VERT_STRIDE
    verts = []
    for i in range(n):
        x, y, z, rhw, col, u, v = struct.unpack_from("<ffffIff", raw, i * VERT_STRIDE)
        verts.append((x, y, col, u, v))
    xs = [v[0] for v in verts]
    ys = [v[1] for v in verts]
    return {
        "x": min(xs), "y": min(ys),
        "w": max(xs) - min(xs), "h": max(ys) - min(ys),
        "colors": [v[2] for v in verts],
        "uv": [(round(v[3], 4), round(v[4], 4)) for v in verts],
    }


def normalize(rec, scale):
    """-> {x,y,w,h, colors[N], uv|None, s|None, rets[list[str]]}"""
    if "v" in rec:                                  # raw form
        d = decode_raw_blob(rec["v"])
        d["rets"] = list(rec.get("r", []))
        d["s"] = rec.get("s")
    else:                                           # decoded form
        d = {
            "x": float(rec["x"]), "y": float(rec["y"]),
            "w": float(rec["w"]), "h": float(rec["h"]),
            "colors": [int(c, 16) for c in rec.get("corner_colors", [])]
                      or [int(rec["argb"], 16)] * 4,
            "uv": None,
            "rets": list(rec.get("rets", [])),
            "s": None,
        }
    d["x"] *= scale
    d["y"] *= scale
    d["w"] *= scale
    d["h"] *= scale
    return d


def max_alpha(d):
    return max((c >> 24) & 0xFF for c in d["colors"]) if d["colors"] else 0


# ------------------------------------------------------------------- match

def geom_match(a, b, tp, ts):
    return (abs(a["x"] - b["x"]) <= tp and abs(a["y"] - b["y"]) <= tp
            and abs(a["w"] - b["w"]) <= ts and abs(a["h"] - b["h"]) <= ts)


def color_match(a, b, tc, ta):
    ca, cb = a["colors"], b["colors"]
    if len(ca) != len(cb):
        return False
    for u, v in zip(ca, cb):
        if abs(((u >> 24) & 0xFF) - ((v >> 24) & 0xFF)) > ta:
            return False
        for sh in (16, 8, 0):
            if abs(((u >> sh) & 0xFF) - ((v >> sh) & 0xFF)) > tc:
                return False
    return True


def lcs_align(A, B, eq):
    """Order-preserving LCS alignment. Returns (pairs, onlyA, onlyB)."""
    n, m = len(A), len(B)
    L = [[0] * (m + 1) for _ in range(n + 1)]
    for i in range(n - 1, -1, -1):
        Li, Li1 = L[i], L[i + 1]
        for j in range(m - 1, -1, -1):
            if eq(A[i], B[j]):
                Li[j] = Li1[j + 1] + 1
            else:
                Li[j] = max(Li1[j], Li[j + 1])
    pairs, onlyA, onlyB = [], [], []
    i = j = 0
    while i < n and j < m:
        if eq(A[i], B[j]):
            pairs.append((i, j)); i += 1; j += 1
        elif L[i + 1][j] >= L[i][j + 1]:
            onlyA.append(i); i += 1
        else:
            onlyB.append(j); j += 1
    onlyA.extend(range(i, n))
    onlyB.extend(range(j, m))
    return pairs, onlyA, onlyB


# ------------------------------------------------------------------ symbols

def load_msvc_map(path):
    """-> (sorted_addrs, names) of module-relative publics from an MSVC .map."""
    base = None
    entries = []
    for line in Path(path).read_text(errors="replace").splitlines():
        if "Preferred load address is" in line:
            base = int(line.rsplit(None, 1)[-1], 16)
            continue
        parts = line.split()
        if len(parts) >= 3 and ":" in parts[0]:
            sec_off, name = parts[0], parts[1]
            try:
                addr = int(parts[2], 16)
            except ValueError:
                continue
            if base is None or addr < base:
                continue
            entries.append((addr - base, undecorate(name)))
    entries.sort()
    return [e[0] for e in entries], [e[1] for e in entries]


def undecorate(name):
    if name.startswith("?"):
        head = name[1:].split("@@", 1)[0]
        parts = [p for p in head.split("@") if p]
        return "::".join(reversed(parts))
    return name.lstrip("_")


def resolve(rets, sym):
    if not sym:
        return rets
    addrs, names = sym
    out = []
    for r in rets:
        try:
            v = int(r, 16)
        except ValueError:
            out.append(r); continue
        k = bisect.bisect_right(addrs, v) - 1
        out.append(f"{r}<{names[k]}+0x{v - addrs[k]:x}>" if k >= 0 else r)
    return out


# ------------------------------------------------------------------- report

def fmt(d, sym=None):
    cc = d["colors"]
    col = f"{cc[0]:08x}" if cc and all(c == cc[0] for c in cc) else \
          "/".join(f"{c:08x}" for c in cc)
    s = f"x={d['x']:7.2f} y={d['y']:7.2f} w={d['w']:7.2f} h={d['h']:6.2f} col={col}"
    if d.get("s") is not None:
        s += f" tex={d['s'][0]} blend={d['s'][1]}:{d['s'][2]}/{d['s'][3]}"
    r = resolve(d["rets"][:2], sym)
    if r:
        s += "  @" + ",".join(r)
    return s


def diff_label(A, B, args, sym):
    tp, ts, tc, ta = args.tol_pos, args.tol_size, args.tol_color, args.tol_alpha
    strict = lambda a, b: geom_match(a, b, tp, ts) and color_match(a, b, tc, ta)
    pairs, onlyA, onlyB = lcs_align(A, B, strict)

    # Second pass over leftovers: geometry-only pairing. If the colors also
    # match, the draw exists on both sides but at a different STREAM POSITION
    # (LCS order conflict) -> "reordered"; otherwise it is a color divergence.
    mismatches = []
    usedB = set()
    still_a = []
    for ia in onlyA:
        hit = None
        for jb in onlyB:
            if jb in usedB:
                continue
            if geom_match(A[ia], B[jb], tp, ts):
                hit = jb
                break
        if hit is not None:
            usedB.add(hit)
            kind = "reordered" if color_match(A[ia], B[hit], tc, ta) else "color"
            mismatches.append((ia, hit, kind))
        else:
            still_a.append(ia)
    still_b = [j for j in onlyB if j not in usedB]

    # Third pass: same color, nearby geometry (within 8x tolerance) -> "moved"
    usedB2 = set()
    rem_a = []
    for ia in still_a:
        hit = None
        for jb in still_b:
            if jb in usedB2:
                continue
            if geom_match(A[ia], B[jb], tp * 8, ts * 8) and \
               color_match(A[ia], B[jb], tc, max(ta, 16)):
                hit = jb
                break
        if hit is not None:
            usedB2.add(hit)
            # --tol-anim N (opt-in): a same-color pair displaced/resized by at
            # most N px is animation PHASE, not composition — e.g. the checker
            # cells' fsin corner jitter runs off each side's own frame counter,
            # so unsynced captures always disagree by the jitter amplitude
            # (~2.5px). Counted as matched, not failed. Off by default.
            if args.tol_anim > 0 and \
               geom_match(A[ia], B[hit], args.tol_anim, args.tol_anim):
                pairs.append((ia, hit))
            else:
                mismatches.append((ia, hit, "moved"))
        else:
            rem_a.append(ia)
    rem_b = [j for j in still_b if j not in usedB2]
    return pairs, mismatches, rem_a, rem_b


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("file_a", help="original-side capture (reference)")
    ap.add_argument("file_b", help="standalone-side capture (under test)")
    ap.add_argument("--label-a", action="append", default=None)
    ap.add_argument("--label-b", action="append", default=None)
    ap.add_argument("--scale-a", type=float, default=1.0)
    ap.add_argument("--scale-b", type=float, default=0.8,
                    help="0.8 maps the 800x600 standalone into the original's "
                         "640x480 virtual space (default); use 1 for like-vs-like")
    ap.add_argument("--tol-pos", type=float, default=1.0)
    ap.add_argument("--tol-size", type=float, default=1.0)
    ap.add_argument("--tol-anim", type=float, default=0.0,
                    help="opt-in: same-color pairs displaced/resized by <= N px "
                         "count as matched (animation phase, e.g. checker fsin "
                         "jitter from unsynced frame counters). 0 = strict")
    ap.add_argument("--tol-color", type=int, default=0)
    ap.add_argument("--tol-alpha", type=int, default=0)
    ap.add_argument("--min-alpha", type=int, default=0,
                    help="drop draws whose max corner alpha is below this")
    ap.add_argument("--exclude-tex", type=int, action="append", default=[],
                    help="drop side-B draws with this texture handle (repeatable); "
                         "use to filter the standalone's font glyphs (tex 9) when "
                         "comparing chrome, since the original-side capture does "
                         "not see the RtCharset text pipe")
    ap.add_argument("--map", dest="mapfile", default=None,
                    help="MSVC .map to resolve side-B retaddrs to names")
    ap.add_argument("--rotate-a", default=None,
                    help="retaddr hex (e.g. 0x42e65a): rotate each side-A frame "
                         "so the first draw whose retaddr chain contains this "
                         "RVA becomes index 0. Frida bursts split frames at "
                         "Present, which can land mid-composition; rotating to "
                         "a stable per-frame anchor (the ShellB video quad at "
                         "0x42e65a) removes the bogus MISMATCH(reordered) wrap")
    ap.add_argument("--json", dest="json_out", default=None)
    ap.add_argument("--max-rows", type=int, default=40)
    args = ap.parse_args()

    try:
        da = json.loads(Path(args.file_a).read_text())
        db = json.loads(Path(args.file_b).read_text())
    except (OSError, ValueError) as e:
        print(f"input error: {e}", file=sys.stderr)
        return 2

    las = args.label_a or list(da.keys())
    lbs = args.label_b or list(db.keys())
    if len(las) != len(lbs):
        n = min(len(las), len(lbs))
        print(f"note: pairing first {n} labels ({len(las)} in A, {len(lbs)} in B)")
        las, lbs = las[:n], lbs[:n]

    sym = None
    if args.mapfile:
        try:
            sym = load_msvc_map(args.mapfile)
        except OSError as e:
            print(f"warning: cannot read map ({e}); raw retaddrs only")

    grand = {"match": 0, "mismatch": 0, "missing": 0, "extra": 0}
    jout = {}
    for la, lb in zip(las, lbs):
        if la not in da or lb not in db:
            print(f"label missing: A[{la}] or B[{lb}]", file=sys.stderr)
            return 2
        A = [normalize(r, args.scale_a) for r in da[la]]
        B = [normalize(r, args.scale_b) for r in db[lb]]
        if args.rotate_a:
            anchor = args.rotate_a.lower()
            pivot = next((i for i, d in enumerate(A)
                          if any(r.lower() == anchor for r in d.get("rets", []))),
                         None)
            if pivot is None:
                print(f"note: --rotate-a {args.rotate_a} not found in {la}; "
                      f"frame left unrotated")
            else:
                A = A[pivot:] + A[:pivot]
        if args.min_alpha:
            A = [d for d in A if max_alpha(d) >= args.min_alpha]
            B = [d for d in B if max_alpha(d) >= args.min_alpha]
        if args.exclude_tex:
            B = [d for d in B if not (d.get("s") and d["s"][0] in args.exclude_tex)]

        pairs, mism, miss, extra = diff_label(A, B, args, sym)
        grand["match"] += len(pairs)
        grand["mismatch"] += len(mism)
        grand["missing"] += len(miss)
        grand["extra"] += len(extra)

        print(f"== {la} (A: {len(A)} draws, x{args.scale_a}) vs "
              f"{lb} (B: {len(B)} draws, x{args.scale_b})")
        print(f"   matched {len(pairs)}  mismatched {len(mism)}  "
              f"missing {len(miss)}  extra {len(extra)}")
        for ia, jb, kind in mism[:args.max_rows]:
            print(f"  MISMATCH({kind})")
            print(f"    A[{ia:3}] {fmt(A[ia])}")
            print(f"    B[{jb:3}] {fmt(B[jb], sym)}")
        for ia in miss[:args.max_rows]:
            print(f"  MISSING  A[{ia:3}] {fmt(A[ia])}")
        for jb in extra[:args.max_rows]:
            print(f"  EXTRA    B[{jb:3}] {fmt(B[jb], sym)}")
        if args.json_out:
            jout[f"{la}|{lb}"] = {
                "matched": len(pairs),
                "mismatched": [
                    {"kind": k, "a": A[i], "b": B[j]} for i, j, k in mism],
                "missing": [A[i] for i in miss],
                "extra": [B[j] for j in extra],
            }

    if args.json_out:
        Path(args.json_out).write_text(json.dumps(jout, indent=1, default=str))
        print(f"-> {args.json_out}")

    red = grand["mismatch"] + grand["missing"] + grand["extra"]
    print(f"VERDICT: {'GREEN' if red == 0 else 'RED'} "
          f"(match={grand['match']} mismatch={grand['mismatch']} "
          f"missing={grand['missing']} extra={grand['extra']})")
    return 0 if red == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
