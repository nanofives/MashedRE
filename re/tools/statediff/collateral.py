#!/usr/bin/env python3
"""collateral.py - ONE generic whole-state, two-arm, per-frame collateral diff.

STANDING TOOL. Do not write another per-attempt `aN_*.py` for "what else moved?".
Every D2 attempt ends with a retroactive collateral review over the captures it
already produced; this is the instrument for it. Read-only: it never launches a
game and never writes outside the paths you give it.

WHY IT EXISTS (checked 2026-10-01, attempt 13 step 0a, before writing a line):
  re/tools/statediff/statediff.py  two-arm, but BIT-EXACT and frame-index aligned.
                                   Useful only same-side (A/B of one build). A
                                   cross-side original-vs-port run is RED on
                                   frame 1 for ~every field and says nothing.
  re/tools/statediff/field_trace.py  ONE capture, a fixed 11-field name list.
  re/tools/statediff/msd_fields.py   ONE capture, offsets you already suspect.
None of the three does whole-record + CSV/log channels + two arms + a measured
noise floor + per-sample alignment + speed banding + magnitude ranking, so this
file adds exactly that and nothing else.

CHANNELS (`kind:path`, any number per arm, merged into one per-frame dict):
  msd:P   MSD1 capture (re/frida/scenario_launch.py --statediff-out). Every
          dword of the 0xd04 vehicle record becomes field `msd+0xNNN` as f32.
          Frame key = the record's own frame index.
  a6a:P   MASHED_A6ADUMP a6a_dump.log. `f=N k=v ... |w0 k=v ... |w3 k=v`.
          Fields `snap.vel[0]`, `act.l60`, `w0.a.ld4`, ... Frame key = `f`.
  kv:P    whitespace-separated `k=v` lines (motion_diag.log, friction_diag.log,
          a5g_diag.log). `[a,b,c]` / `(a,b,c)` expand to `k[0]`, `k[1]`, ...
          Frame key = `f`/`i` if present, else the 1-based line number.
  csv:P   CSV with a header row. Frame key = `frame` column if present, else the
          1-based row number. A `site` column, if present, is appended to the
          field name (`site2.speed`) so multi-site probes do not collide.

NOISE FLOOR. `--floor-a` / `--floor-b` take a SAME-ARM REPEAT of the arm (two
runs of the identical configuration). Per field the floor is the p99 of
|a1 - a2| over their aligned frames. A cross-arm difference is only called a
divergence when it exceeds that field's floor. With no repeat pair supplied the
floor is 0 and EVERY field diverges on frame 1 -- the tool says so instead of
pretending the table means something.

ALIGNMENT. `--anchor FIELD:OP:VALUE` re-indexes BOTH arms so frame 0 is the
first frame satisfying the predicate (bounce / race-GO / first-grounded), which
is what makes two different boots comparable at all. `--speed FIELD` bands every
statistic, so no number is ever reported without the band it was taken in.

MODE, and getting this wrong invalidates the whole table:
  --mode paired  (default) pairs the two arms FRAME BY FRAME and ranks by the
        first frame past the floor. Correct for two runs of the SAME side (an
        A/B knob, a before/after of one build), where the trajectories stay on
        top of each other.
  --mode banded  bands EACH arm on ITS OWN speed and compares band medians.
        Required for a cross-side (original vs port) review: the two
        trajectories separate, so at frame 1200 the original is at 1281 and the
        port at 32, and a frame-paired "divergence" is just that mismatch
        re-reported per field. Memory `band-scored-off-regime-is-not-a-measurement`.
        There is no first-divergent-frame column in this mode and the tool does
        not print one.

SCOPE (`--scope FILE`, lines `glob<TAB>writeset|downstream|outside`, first match
wins, default `outside`). Lets a caller mark which fields the function under
study is allowed to move, so the output can be read for COLLATERAL: fields that
moved and are NOT in the write set.

Usage (the attempt-12 retroactive review, verbatim):
  py -3.12 re/tools/statediff/collateral.py \
      --a msd:verify/d2_bounce_20260930/orig_fp2.msd \
      --floor-a msd:verify/d2_bounce_20260930/orig_fp1.msd \
      --b a6a:verify/d2_sink_20261001/arm/a6a_dump.log \
      --floor-b a6a:verify/d2_sink_20261001/ctl/a6a_dump.log \
      --map snap.vel[0]=msd+0x9b0 --speed msd+0x9e4 \
      --anchor msd+0x9e0:ge:4.0 --scope re/tools/statediff/scope_a6a.txt
"""
import argparse
import csv as _csv
import fnmatch
import math
import os
import re
import struct
import statistics
import sys

MAGIC = b"MSD1"
DEFAULT_BANDS = [(40, 70), (70, 100), (100, 150), (150, 260), (260, 500),
                 (500, 1000), (1000, 1500), (1500, 2000), (2000, 2600)]


# ---------------------------------------------------------------- readers

def _num(tok):
    """Return a float for a token, or None if it is not numeric."""
    t = tok.strip()
    if not t:
        return None
    try:
        if t.lower().startswith(("0x", "-0x", "+0x")):
            return float(int(t, 16))
        v = float(t)
    except ValueError:
        return None
    return v if v == v else None          # drop NaN; it poisons every median


def read_msd(path):
    """MSD1 -> {frame: {field: float}} over every dword of the record."""
    out = {}
    with open(path, "rb") as f:
        hdr = f.read(16)
        if len(hdr) != 16 or hdr[:4] != MAGIC:
            sys.exit("%s: not an MSD1 capture" % path)
        rec, _base, _ = struct.unpack_from("<III", hdr, 4)
        ndw = rec // 4
        names = ["msd+%#05x" % (i * 4) for i in range(ndw)]
        while True:
            fh = f.read(4)
            if len(fh) < 4:
                break
            (idx,) = struct.unpack("<I", fh)
            p = f.read(rec)
            if len(p) < rec:
                break
            vals = struct.unpack_from("<%df" % ndw, p, 0)
            out[idx] = {n: (v if v == v else 0.0) for n, v in zip(names, vals)}
    return out


_TOK = re.compile(r"([A-Za-z_][\w.\[\]]*)=(\[[^\]]*\]|\([^)]*\)|\S+)")


def _expand(prefix, key, raw, dst):
    body = raw
    if body[:1] in "[(" and body[-1:] in "])":
        body = body[1:-1]
    parts = body.split(",")
    if len(parts) == 1:
        v = _num(parts[0])
        if v is not None:
            dst[prefix + key] = v
        return
    for i, p in enumerate(parts):
        v = _num(p)
        if v is not None:
            dst["%s%s[%d]" % (prefix, key, i)] = v


def read_kv(path, frame_keys=("f", "i", "frame")):
    out, auto = {}, 0
    for ln in open(path, "r", errors="replace"):
        auto += 1
        row = {}
        for k, raw in _TOK.findall(ln):
            _expand("", k, raw, row)
        if not row:
            continue
        fi = None
        for fk in frame_keys:
            if fk in row:
                fi = int(row[fk])
                break
        out[auto if fi is None else fi] = row
    return out


def read_a6a(path):
    """a6a_dump.log: `f=N k=v ... |w0 k=v ... |w3 k=v`."""
    out = {}
    for auto, ln in enumerate(open(path, "r", errors="replace"), 1):
        segs = ln.rstrip("\n").split("|")
        row, fi = {}, None
        for si, seg in enumerate(segs):
            prefix = ""
            if si:
                m = re.match(r"\s*(w\d+)\s", seg)
                if m:
                    prefix = m.group(1) + "."
                    seg = seg[m.end():]
            for k, raw in _TOK.findall(seg):
                _expand(prefix, k, raw, row)
        if not row:
            continue
        if "f" in row:
            fi = int(row["f"])
        out[auto if fi is None else fi] = row
    return out


def read_csv(path):
    out = {}
    with open(path, newline="", errors="replace") as fh:
        for auto, r in enumerate(_csv.DictReader(fh), 1):
            site = r.get("site")
            pre = ("site%s." % site) if site not in (None, "") else ""
            row = {}
            for k, raw in r.items():
                if k is None or raw is None or k in ("site",):
                    continue
                v = _num(raw)
                if v is not None:
                    row[pre + k] = v
            if not row:
                continue
            fi = r.get("frame")
            key = int(float(fi)) if fi not in (None, "") else auto
            # several probe sites share one frame index; merge, do not overwrite
            out.setdefault(key, {}).update(row)
    return out


READERS = {"msd": read_msd, "a6a": read_a6a, "kv": read_kv, "csv": read_csv}


def load_arm(specs):
    """specs: list of 'kind:path'. Returns ({frame: {field: v}}, [paths])."""
    merged, paths = {}, []
    for sp in specs:
        if ":" not in sp:
            sys.exit("bad channel spec %r (want kind:path)" % sp)
        kind, path = sp.split(":", 1)
        if kind not in READERS:
            sys.exit("unknown channel kind %r (have %s)"
                     % (kind, "/".join(sorted(READERS))))
        if not os.path.exists(path):
            sys.exit("missing channel file: %s" % path)
        ch = READERS[kind](path)
        if not ch:
            sys.exit("channel %s produced 0 frames" % path)
        for fi, row in ch.items():
            merged.setdefault(fi, {}).update(row)
        paths.append("%s(%d frames)" % (sp, len(ch)))
    return merged, paths


# ---------------------------------------------------------------- shaping

def apply_map(arm, mapping):
    if not mapping:
        return arm
    for row in arm.values():
        for src, dst in mapping.items():
            if src in row:
                row[dst] = row.pop(src)
    return arm


def anchor(arm, field, op, value, label):
    """Re-index so frame 0 is the first frame satisfying the predicate."""
    cmp = {"ge": lambda a, b: a >= b, "gt": lambda a, b: a > b,
           "le": lambda a, b: a <= b, "lt": lambda a, b: a < b,
           "eq": lambda a, b: a == b, "ne": lambda a, b: a != b}[op]
    for fi in sorted(arm):
        v = arm[fi].get(field)
        if v is not None and cmp(v, value):
            print("# anchor %s: %s %s %g first at frame %d -> rebased to 0"
                  % (label, field, op, value, fi))
            return {k - fi: r for k, r in arm.items() if k >= fi}
    sys.exit("anchor %s: %s never satisfies %s %g" % (label, field, op, value))


def med(xs):
    return statistics.median(xs) if xs else float("nan")


def pct(xs, q):
    if not xs:
        return float("nan")
    s = sorted(xs)
    return s[min(len(s) - 1, int(q * len(s)))]


def floor_of(f1, f2):
    """Per-field p99 of |a-b| over the aligned frames of a same-arm repeat."""
    common = sorted(set(f1) & set(f2))
    fields = set()
    for fi in common:
        fields |= set(f1[fi]) & set(f2[fi])
    out = {}
    for name in fields:
        d = [abs(f1[fi][name] - f2[fi][name])
             for fi in common if name in f1[fi] and name in f2[fi]]
        if d:
            out[name] = pct(d, 0.99)
    return out, len(common)


def band_meds(arm, fields, spd, bands, min_n):
    """{field: {(lo,hi): (n, median)}} banded on the arm's OWN speed field."""
    acc = {}
    for row in arm.values():
        s = row.get(spd)
        if s is None:
            continue
        s = abs(s)
        for b in bands:
            if b[0] <= s < b[1]:
                for name in fields:
                    v = row.get(name)
                    if v is not None:
                        acc.setdefault(name, {}).setdefault(b, []).append(v)
                break
    return {n: {b: (len(v), med(v)) for b, v in d.items() if len(v) >= min_n}
            for n, d in acc.items()}


def band_speeds(arm, spd, bands):
    """{(lo,hi): (median |speed|, median FRAME INDEX)} for the arm.

    The frame index is not decoration. Banding on speed silently compares
    different POINTS IN TIME whenever the two arms traverse a band at different
    moments -- and anything that ramps in time (a steer ramp, a gear, a warm-up
    counter) then shows a fake, speed-dependent cross-side defect. Measured
    2026-10-01: in the 260-500 band the port's median frame was 22 (still on a
    120-frame steer ramp, angle 19.9 deg) and the original's was 1102
    (saturated, 33.867 deg). Four separately-reported "defects" were that one
    artefact. If the two median frames below are far apart, the row is
    OFF-REGIME and is not a measurement -- filter to a common regime first.
    """
    acc = {}
    for fi, row in arm.items():
        s = row.get(spd)
        if s is None:
            continue
        s = abs(s)
        for b in bands:
            if b[0] <= s < b[1]:
                acc.setdefault(b, []).append((s, fi))
                break
    return {b: (med([x[0] for x in v]), med([x[1] for x in v]))
            for b, v in acc.items()}


def scope_table(path):
    rules = []
    if not path:
        return rules
    for ln in open(path, errors="replace"):
        ln = ln.split("#", 1)[0].strip()
        if not ln:
            continue
        parts = re.split(r"\s*\t\s*|\s{2,}", ln, 1)
        if len(parts) != 2:
            sys.exit("scope: bad line %r (want glob<TAB>class)" % ln)
        rules.append((parts[0].strip(), parts[1].strip()))
    return rules


def classify(name, rules):
    for pat, cls in rules:
        if fnmatch.fnmatch(name, pat):
            return cls
    return "outside"


# ---------------------------------------------------------------- main

def banded_report(A, B, A2, B2, fields, spd, bands, rules, args, onlyA, onlyB):
    """Cross-side review: band EACH arm on its OWN speed, compare band medians.

    The noise floor here is the right one for this statistic: how far the SAME
    band's median moves between two repeats of the same arm. A cross-side gap
    smaller than that is not a measurement.
    """
    mA = band_meds(A, fields, spd, bands, args.min_n)
    mB = band_meds(B, fields, spd, bands, args.min_n)
    sA, sB = band_speeds(A, spd, bands), band_speeds(B, spd, bands)
    fA = band_meds(A2, fields, spd, bands, args.min_n) if A2 else {}
    fB = band_meds(B2, fields, spd, bands, args.min_n) if B2 else {}
    if not (fA or fB):
        print("# !! NO NOISE FLOOR supplied: every gap below is unguarded.")

    rows = []
    for name in sorted(fields):
        cls = classify(name, rules)
        if args.only_outside and cls != "outside":
            continue
        ent, worst = [], 0.0
        for b in bands:
            if b not in mA.get(name, {}) or b not in mB.get(name, {}):
                continue
            na, va = mA[name][b]
            nb, vb = mB[name][b]
            # floor is None only when NEITHER repeat pair populated this band.
            # A floor of exactly 0.0 is the STRONGEST result there is (the two
            # repeats put the band median on the same bit) and must never be
            # displayed as "no data" -- memory absent-log-proves-nothing.
            floor = None
            if b in fA.get(name, {}):
                floor = abs(va - fA[name][b][1])
            if b in fB.get(name, {}):
                f2 = abs(vb - fB[name][b][1])
                floor = f2 if floor is None else max(floor, f2)
            gap = abs(vb - va)
            if floor is None:
                mult = float("nan")
            elif floor > 0:
                mult = gap / floor
            else:
                mult = float("inf") if gap > 0 else 0.0
            worst = max(worst, 0.0 if mult != mult else min(mult, 1e9))
            ent.append((b, na, nb, va, vb, gap, floor, mult))
        if ent:
            rows.append((worst, name, cls, ent))
    rows.sort(key=lambda r: -r[0])

    div = [r for r in rows if r[0] > 1.0]
    print("\n=== CROSS-SIDE BAND MEDIANS, ranked by worst gap / noise floor "
          "(%d of %d paired fields exceed their floor in some band) ==="
          % (len(div), len(rows)))
    hdr = ("field                  scope      band          nA   nB | medSpd A"
           "  medSpd B | medFrm A medFrm B |        med A        med B   gap/floor")
    print(hdr)
    print("-" * len(hdr))
    offreg = set()
    for worst, name, cls, ent in rows[:args.top]:
        first = True
        for b, na, nb, va, vb, gap, floor, mult in ent:
            if floor is None:
                tag = "no-floor"
            elif floor == 0.0:
                tag = "0-floor" if gap > 0 else "exact"
            else:
                tag = "%.1fx" % mult
            spa, fra = sA.get(b, (float('nan'),) * 2)
            spb, frb = sB.get(b, (float('nan'),) * 2)
            # off-regime flag: the two arms sit in this band at very different
            # times, so the row compares two moments, not two implementations.
            far = (fra == fra and frb == frb
                   and abs(fra - frb) > 0.5 * max(fra, frb, 1))
            if far:
                offreg.add(b)
            print("%-22s %-10s %-11s %4d %4d | %8.1f %9.1f | %8.0f %8.0f%s| "
                  "%12.5g %12.5g %11s"
                  % (name[:22] if first else "", cls if first else "",
                     "%g-%g" % b, na, nb, spa, spb, fra, frb,
                     " !! " if far else " ", va, vb, tag))
            first = False
        print("")
    if offreg:
        print("!! OFF-REGIME BANDS (median frame indices differ by >50%%): %s"
              % ", ".join("%g-%g" % b for b in sorted(offreg)))
        print("   Those rows compare two different MOMENTS at a matched speed, "
              "not two implementations.")
        print("   Filter both arms to a common regime before reading any number "
              "from them.")
    if len(rows) > args.top:
        print("  ... %d more fields (raise --top)" % (len(rows) - args.top))

    inside = [r for r in rows if r[0] <= 1.0]
    print("=== WITHIN THE NOISE FLOOR in every shared band: %d fields ==="
          % len(inside))
    if inside:
        print("  " + "  ".join(r[1] for r in inside))

    if onlyA or onlyB:
        print("\n=== UNPAIRED (one arm only; not comparable) ===")
        if onlyA:
            print("  A-only: %d fields" % len(onlyA))
        if onlyB:
            print("  B-only: %d fields: %s%s"
                  % (len(onlyB), "  ".join(sorted(onlyB)[:24]),
                     " ..." if len(onlyB) > 24 else ""))

    if args.out:
        with open(args.out, "w", newline="") as fh:
            w = _csv.writer(fh)
            w.writerow(["field", "scope", "band_lo", "band_hi", "n_A", "n_B",
                        "med_speed_A", "med_speed_B", "med_frame_A",
                        "med_frame_B", "med_A", "med_B",
                        "gap", "noise_floor", "gap_over_floor"])
            for worst, name, cls, ent in rows:
                for b, na, nb, va, vb, gap, floor, mult in ent:
                    spa, fra = sA.get(b, (float('nan'),) * 2)
                    spb, frb = sB.get(b, (float('nan'),) * 2)
                    w.writerow([name, cls, b[0], b[1], na, nb,
                                "%.6g" % spa, "%.6g" % spb,
                                "%.6g" % fra, "%.6g" % frb,
                                "%.9g" % va, "%.9g" % vb, "%.9g" % gap,
                                "" if floor is None else "%.9g" % floor,
                                "%.6g" % mult])
        print("\ncsv -> %s" % args.out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--a", required=True, help="arm A channels, comma separated")
    ap.add_argument("--b", required=True, help="arm B channels, comma separated")
    ap.add_argument("--floor-a", default="", help="same-arm repeat of A")
    ap.add_argument("--floor-b", default="", help="same-arm repeat of B")
    ap.add_argument("--map", action="append", default=[],
                    help="BFIELD=AFIELD, rename a B field onto an A field "
                         "(repeatable)")
    ap.add_argument("--speed", default=None, help="field to band by")
    ap.add_argument("--anchor", default=None, help="FIELD:OP:VALUE")
    ap.add_argument("--scope", default=None)
    ap.add_argument("--bands", default=None, help='"lo-hi,lo-hi,..."')
    ap.add_argument("--top", type=int, default=40)
    ap.add_argument("--min-n", type=int, default=10)
    ap.add_argument("--only-outside", action="store_true")
    ap.add_argument("--mode", choices=("paired", "banded"), default="paired",
                    help="paired = frame-by-frame (same side only); "
                         "banded = each arm on its own speed (cross-side)")
    ap.add_argument("--out", default=None)
    args = ap.parse_args()

    bands = DEFAULT_BANDS
    if args.bands:
        bands = [tuple(float(x) for x in b.split("-"))
                 for b in args.bands.split(",")]

    mapping = dict(m.split("=", 1) for m in args.map)
    split = lambda s: [x for x in s.split(",") if x]

    A, pa = load_arm(split(args.a))
    B, pb = load_arm(split(args.b))
    B = apply_map(B, mapping)
    print("# arm A: %s" % "  ".join(pa))
    print("# arm B: %s" % "  ".join(pb))

    fl, nfa, nfb = {}, 0, 0
    A2 = B2 = None
    if args.floor_a:
        A2, _ = load_arm(split(args.floor_a))
        fa, nfa = floor_of(A, A2)
        for k, v in fa.items():
            fl[k] = max(fl.get(k, 0.0), v)
    if args.floor_b:
        B2, _ = load_arm(split(args.floor_b))
        B2 = apply_map(B2, mapping)
        fb, nfb = floor_of(B, B2)
        for k, v in fb.items():
            fl[k] = max(fl.get(k, 0.0), v)
    if not fl:
        print("# !! NO NOISE FLOOR supplied. Every field will read as divergent "
              "on its first\n#    aligned frame; the first-frame column below is "
              "NOT a measurement.")
    else:
        print("# noise floor: %d fields, from %d A-pair frames and %d B-pair "
              "frames" % (len(fl), nfa, nfb))

    if args.anchor:
        f, op, v = args.anchor.split(":")
        A = anchor(A, f, op, float(v), "A")
        B = anchor(B, f, op, float(v), "B")

    common = sorted(set(A) & set(B))
    allA = set().union(*(set(r) for r in A.values()))
    allB = set().union(*(set(r) for r in B.values()))
    fields = allA & allB
    onlyA, onlyB = allA - fields, allB - fields
    if not common and args.mode == "paired":
        sys.exit("no common frame index between the two arms "
                 "(use --anchor, or --mode banded for a cross-side review)")
    print("# aligned frames %d [%d..%d]   paired fields %d   "
          "A-only %d   B-only %d"
          % (len(common), common[0], common[-1], len(fields),
             len(onlyA), len(onlyB)))

    rules = scope_table(args.scope)

    spd = args.speed
    if spd and spd not in fields:
        print("# !! --speed %s is not a paired field; banding disabled" % spd)
        spd = None

    if args.mode == "banded":
        if not spd:
            sys.exit("--mode banded needs a --speed field present in both arms")
        return banded_report(A, B, A2, B2, fields, spd, bands, rules, args,
                             onlyA, onlyB)

    rows = []
    for name in sorted(fields):
        f = fl.get(name, 0.0)
        first, n, diffs, av, bv, bandrows = None, 0, [], [], [], {}
        for fi in common:
            if name not in A[fi] or name not in B[fi]:
                continue
            a, b = A[fi][name], B[fi][name]
            d = abs(a - b)
            n += 1
            diffs.append(d)
            av.append(a)
            bv.append(b)
            if d > f and first is None:
                first = fi
            if spd:
                s = A[fi].get(spd)
                if s is not None:
                    for lo, hi in bands:
                        if lo <= abs(s) < hi:
                            bandrows.setdefault((lo, hi), []).append((a, b, s))
                            break
        if n < args.min_n:
            continue
        cls = classify(name, rules)
        if args.only_outside and cls != "outside":
            continue
        rows.append(dict(name=name, cls=cls, first=first, n=n,
                         floor=f, medd=med(diffs), p95d=pct(diffs, 0.95),
                         meda=med(av), medb=med(bv), bands=bandrows))

    div = [r for r in rows if r["first"] is not None]
    div.sort(key=lambda r: (r["first"], -r["medd"]))
    print("\n=== DIVERGENT FIELDS, ranked by first frame past the noise floor "
          "(%d of %d paired) ===" % (len(div), len(rows)))
    hdr = ("field                  scope       first      n |      med A"
           "      med B    med|A-B|      floor")
    print(hdr)
    print("-" * len(hdr))
    for r in div[:args.top]:
        print("%-22s %-10s %7s %6d | %10.4g %10.4g %11.4g %10.4g"
              % (r["name"][:22], r["cls"], r["first"], r["n"],
                 r["meda"], r["medb"], r["medd"], r["floor"]))
    if len(div) > args.top:
        print("  ... %d more (raise --top)" % (len(div) - args.top))

    same = [r for r in rows if r["first"] is None]
    print("\n=== WITHIN THE NOISE FLOOR on every aligned frame: %d fields ==="
          % len(same))
    if same:
        print("  " + "  ".join(r["name"] for r in same[:24])
              + (" ..." if len(same) > 24 else ""))

    if spd and div:
        print("\n=== PER-BAND medians for the top %d divergent fields "
              "(banded on %s) ===" % (min(args.top, len(div)), spd))
        for r in div[:args.top]:
            printed = False
            for lo, hi in bands:
                br = r["bands"].get((lo, hi))
                if not br or len(br) < args.min_n:
                    continue
                ma, mb = med([x[0] for x in br]), med([x[1] for x in br])
                ratio = (mb / ma) if ma not in (0.0,) else float("nan")
                print("  %-22s %-11s n=%4d  med speed %8.1f  A %12.5g  "
                      "B %12.5g  B/A %8.4f"
                      % (r["name"][:22] if not printed else "",
                         "%g-%g" % (lo, hi), len(br),
                         med([abs(x[2]) for x in br]), ma, mb, ratio))
                printed = True
            if printed:
                print("")

    if onlyA or onlyB:
        print("=== UNPAIRED (present in one arm only; not comparable) ===")
        if onlyA:
            print("  A-only: %s%s" % ("  ".join(sorted(onlyA)[:16]),
                                      " ..." if len(onlyA) > 16 else ""))
        if onlyB:
            print("  B-only: %s%s" % ("  ".join(sorted(onlyB)[:16]),
                                      " ..." if len(onlyB) > 16 else ""))

    if args.out:
        with open(args.out, "w", newline="") as fh:
            w = _csv.writer(fh)
            w.writerow(["field", "scope", "first_divergent_frame", "n",
                        "med_A", "med_B", "med_abs_diff", "p95_abs_diff",
                        "noise_floor"])
            for r in div + same:
                w.writerow([r["name"], r["cls"],
                            "" if r["first"] is None else r["first"],
                            r["n"], "%.9g" % r["meda"], "%.9g" % r["medb"],
                            "%.9g" % r["medd"], "%.9g" % r["p95d"],
                            "%.9g" % r["floor"]])
        print("\ncsv -> %s" % args.out)


if __name__ == "__main__":
    main()
