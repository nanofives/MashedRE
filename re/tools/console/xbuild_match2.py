#!/usr/bin/env python3
r"""xbuild_match2.py — feature-based PC<->Xbox function matching (pass 2).

Builds on xbuild_match.py (pass-1 byte-identity anchors) with feature tiers
over DumpFuncFeatures.java output:

  "mnem"          — mnemonic-sequence hash unique on both sides
  "string"        — unique-string anchors (string referenced by exactly one
                    function per side), majority vote, size sanity check
  "const"         — same machinery over unusual scalar immediates + 8-byte
                    data fingerprints (handling tables, float constants)
  "prop-*"        — callee-list gap propagation from matched pairs
  "caller-*"      — caller-context votes: unmatched callees of matched pairs,
                    reciprocal-best with vote/sanity thresholds
  "interval-*"    — address-interval positional pairing between consecutive
                    strong anchors (per-TU locality; Xbox link order is
                    REVERSED vs PC, detected at runtime)

"-mnem" variants carry an exact mnemonic-stream witness. After fixpoint, an
ordinal check flags pairs whose Xbox VA contradicts both neighbors in the
dominant link order; contradicted WEAK-tier pairs are dropped.

One-to-one mapping enforced; higher tiers win.

Output: re/console/match/xbuild_match_v2.csv (ordinal_ok column last so
ApplyTwinNames.java column indices stay valid) + coverage summary.
"""
import bisect
import csv
import sys
from collections import Counter, defaultdict

ROOT = "."
PC_FEAT = f"{ROOT}/re/console/pc_features.csv"
XB_FEAT = f"{ROOT}/re/console/xbox/toast_features.csv"
PASS1 = f"{ROOT}/re/console/match/xbuild_match.csv"
HOOKS = f"{ROOT}/hooks.csv"
OUT = f"{ROOT}/re/console/match/xbuild_match_v2.csv"

RATIO = 1.30   # size / instruction-count sanity bound (strict)
RATIO_LOOSE = 1.60  # interval tier bound
MAX_TU_SPAN = 0x8000  # xbox bytes; ascending strong bracket wider than this is
                      # treated as crossing TUs (coincidental), not same-TU
STRONG = {"byte-full", "byte-prefix", "mnem", "string", "const",
          "prop-mnem", "caller-mnem", "interval-mnem"}
WEAK = {"prop-weak", "caller", "interval"}


def load_features(path):
    funcs = {}
    with open(path, newline="", encoding="utf-8", errors="replace") as f:
        for row in csv.DictReader(f):
            entry = int(row["entry"], 16)
            imms = [s for s in row.get("imms", "").split("|") if s]
            fps = [s for s in row.get("datafps", "").split("|") if s]
            funcs[entry] = {
                "size": int(row["size"]),
                "nmnem": int(row["nmnem"]),
                "mnemhash": row["mnemhash"],
                "strings": [s for s in row["strings"].split("|") if s],
                "callees": [int(c, 16) for c in row["callees"].split(";") if c],
                "consts": ["I:" + s for s in imms] + ["D:" + s for s in fps],
            }
    return funcs


def load_hooks():
    info = {}
    with open(HOOKS, newline="", encoding="utf-8", errors="replace") as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#") or row[0] == "rva":
                continue
            try:
                va = int(row[0], 16)
            except ValueError:
                continue
            if va < 0x400000:  # 21 legacy file-offset rows
                va += 0x400000
            info[va] = (row[1], row[2], row[3])
    return info


def ratio_ok(a, b, bound=RATIO):
    if a == 0 or b == 0:
        return a == b
    return max(a, b) / min(a, b) <= bound


class Matcher:
    def __init__(self, pc, xb):
        self.pc, self.xb = pc, xb
        self.p2x, self.x2p, self.tier = {}, {}, {}

    def add(self, p, x, tier):
        if p in self.p2x or x in self.x2p:
            return False
        if p not in self.pc or x not in self.xb:
            return False
        self.p2x[p], self.x2p[x] = x, p
        self.tier[p] = tier
        return True

    def remove(self, p):
        x = self.p2x.pop(p)
        del self.x2p[x]
        del self.tier[p]

    def tier_mnem(self):
        by_pc, by_xb = defaultdict(list), defaultdict(list)
        for e, f in self.pc.items():
            if e not in self.p2x and f["nmnem"] >= 8:
                by_pc[f["mnemhash"]].append(e)
        for e, f in self.xb.items():
            if e not in self.x2p and f["nmnem"] >= 8:
                by_xb[f["mnemhash"]].append(e)
        n = 0
        for h, ps in by_pc.items():
            xs = by_xb.get(h)
            if xs and len(ps) == 1 and len(xs) == 1:
                n += self.add(ps[0], xs[0], "mnem")
        return n

    def tier_unique_tokens(self, field, tier):
        def uniq_map(funcs, skip):
            owner = {}
            for e, f in funcs.items():
                if e in skip:
                    continue
                for s in f[field]:
                    owner[s] = None if s in owner else e
            return {s: e for s, e in owner.items() if e is not None}

        pc_u = uniq_map(self.pc, self.p2x)
        xb_u = uniq_map(self.xb, self.x2p)
        votes = Counter()
        for s, pe in pc_u.items():
            xe = xb_u.get(s)
            if xe is not None:
                votes[(pe, xe)] += 1
        best = defaultdict(lambda: (None, 0))
        for (pe, xe), v in votes.items():
            if v > best[pe][1]:
                best[pe] = (xe, v)
        n = 0
        for pe, (xe, v) in best.items():
            if pe in self.p2x or xe in self.x2p:
                continue
            if ratio_ok(self.pc[pe]["nmnem"], self.xb[xe]["nmnem"], 1.5) or v >= 2:
                n += self.add(pe, xe, tier)
        return n

    def tier_propagate(self):
        total = 0
        for rounds in range(30):
            cand = Counter()
            for pe, xe in list(self.p2x.items()):
                gaps = self._gaps(self.pc[pe]["callees"], self.xb[xe]["callees"])
                for gp, gx in gaps:
                    if len(gp) == len(gx):
                        for a, b in zip(gp, gx):
                            cand[(a, b)] += 1
            added = 0
            for (a, b), v in cand.most_common():
                if a in self.p2x or b in self.x2p:
                    continue
                fa, fb = self.pc.get(a), self.xb.get(b)
                if not fa or not fb:
                    continue
                if fa["mnemhash"] == fb["mnemhash"] and fa["nmnem"] >= 4:
                    added += self.add(a, b, "prop-mnem")
                elif (v >= 2 or rounds == 0) and ratio_ok(fa["size"], fb["size"]) \
                        and ratio_ok(fa["nmnem"], fb["nmnem"]):
                    added += self.add(a, b, "prop-weak")
            total += added
            if added == 0:
                break
        return total

    def _gaps(self, cp, cx):
        """Align two callee lists on matched pairs; yield unmatched gap pairs."""
        anchors = []
        xi_used = set()
        xi = 0
        for pi, p in enumerate(cp):
            mx = self.p2x.get(p)
            if mx is None:
                continue
            for xj in range(xi, len(cx)):
                if cx[xj] == mx and xj not in xi_used:
                    anchors.append((pi, xj))
                    xi_used.add(xj)
                    xi = xj + 1
                    break
        gaps = []
        prev_p, prev_x = -1, -1
        for pi, xj in anchors + [(len(cp), len(cx))]:
            gp = [c for c in cp[prev_p + 1:pi] if c not in self.p2x]
            gx = [c for c in cx[prev_x + 1:xj] if c not in self.x2p]
            if gp and gx:
                gaps.append((gp, gx))
            prev_p, prev_x = pi, xj
        return gaps

    def tier_callers(self):
        cand = Counter()
        for pe, xe in list(self.p2x.items()):
            ap = [c for c in dict.fromkeys(self.pc[pe]["callees"])
                  if c not in self.p2x and c in self.pc]
            ax = [c for c in dict.fromkeys(self.xb[xe]["callees"])
                  if c not in self.x2p and c in self.xb]
            if not ap or not ax or len(ap) * len(ax) > 100:
                continue
            for a in ap:
                for b in ax:
                    cand[(a, b)] += 1
        best_a, best_b = {}, {}
        for (a, b), v in cand.items():
            if a not in best_a or v > best_a[a][1]:
                best_a[a] = (b, v)
            if b not in best_b or v > best_b[b][1]:
                best_b[b] = (a, v)
        n = 0
        for a, (b, v) in best_a.items():
            if best_b.get(b, (None, 0))[0] != a:
                continue
            if a in self.p2x or b in self.x2p:
                continue
            fa, fb = self.pc[a], self.xb[b]
            if fa["mnemhash"] == fb["mnemhash"] and fa["nmnem"] >= 4:
                n += self.add(a, b, "caller-mnem")
            elif v >= 2 and ratio_ok(fa["size"], fb["size"]) \
                    and ratio_ok(fa["nmnem"], fb["nmnem"]):
                n += self.add(a, b, "caller")
        return n

    def tier_interval(self):
        """Positionally place unmatched gap functions between consecutive strong
        anchors, using PER-BRACKET direction (NOT a global flag):

          tight ascending bracket (xb_L < xb_R, width <= MAX_TU_SPAN) = same TU,
            functions ascend on both sides -> forward 1:1 pairing.
          everything else (descending = cross-TU boundary, or wide ascending =
            coincidental) -> skipped; no reliable positional constraint.

        This is what catches small getters (no strings / generic mnemonics) that
        the feature tiers miss, while staying inside a verified same-TU span.
        """
        strong = sorted((p, self.p2x[p]) for p in self.p2x
                        if self.tier[p] in STRONG)
        if len(strong) < 3:
            return 0
        pc_sorted = sorted(self.pc)
        xb_sorted = sorted(self.xb)
        n = 0
        for (p1, x1), (p2, x2) in zip(strong, strong[1:]):
            if not (x1 < x2 and (x2 - x1) <= MAX_TU_SPAN):
                continue  # only trust tight ascending same-TU brackets
            i1 = bisect.bisect_right(pc_sorted, p1)
            i2 = bisect.bisect_left(pc_sorted, p2)
            gp = [e for e in pc_sorted[i1:i2] if e not in self.p2x]
            j1 = bisect.bisect_right(xb_sorted, x1)
            j2 = bisect.bisect_left(xb_sorted, x2)
            gx = [e for e in xb_sorted[j1:j2] if e not in self.x2p]
            if not gp or len(gp) != len(gx):
                continue
            pairs = list(zip(gp, gx))  # both ascending -> forward
            if not all(ratio_ok(self.pc[a]["size"], self.xb[b]["size"], RATIO_LOOSE)
                       and ratio_ok(self.pc[a]["nmnem"], self.xb[b]["nmnem"],
                                    RATIO_LOOSE)
                       for a, b in pairs):
                continue
            for a, b in pairs:
                t = "interval-mnem" \
                    if self.pc[a]["mnemhash"] == self.xb[b]["mnemhash"] \
                    else "interval"
                n += self.add(a, b, t)
        return n

    def scaffold_status(self):
        """Classify every pair against the strong-pair layout scaffold.

        Empirical layout: PC-ascending walks TU blocks in DESCENDING xbox-base
        order, but functions within a TU ascend together. So a strong pair
        bracket [L,R] with xb_L < xb_R is a same-TU span — any pair whose PC
        lies inside must have xbox in (xb_L, xb_R). Descending brackets straddle
        a TU boundary and give no tight constraint.

        Returns pc -> one of:
          "ok-asc"    inside an ascending (same-TU) strong bracket, fits  -> corroborated
          "flag"      inside an ascending strong bracket, xbox out of range -> suspect
          "ambiguous" only a descending/edge bracket available -> no constraint
        """
        strong = sorted((p, self.p2x[p]) for p in self.p2x
                        if self.tier[p] in STRONG)
        if len(strong) < 3:
            return {p: "ambiguous" for p in self.p2x}
        spc = [p for p, _ in strong]
        sxb = [x for _, x in strong]
        out = {}
        for p, x in self.p2x.items():
            i = bisect.bisect_left(spc, p)
            li = i - 1
            if li >= 0 and spc[li] == p:
                li -= 1
            ri = i
            while ri < len(spc) and spc[ri] == p:
                ri += 1
            if li < 0 or ri >= len(spc):
                out[p] = "ambiguous"
                continue
            xl, xr = sxb[li], sxb[ri]
            if xl < xr and (xr - xl) <= MAX_TU_SPAN:  # tight ascending = same TU
                out[p] = "ok-asc" if xl <= x <= xr else "flag"
            else:  # descending, or ascending-but-wide (coincidental cross-TU)
                out[p] = "ambiguous"
        return out


def main():
    pc = load_features(PC_FEAT)
    xb = load_features(XB_FEAT)
    m = Matcher(pc, xb)

    counts = Counter()
    with open(PASS1, newline="") as f:
        for row in csv.DictReader(f):
            counts["byte"] += m.add(int(row["pc_va"], 16),
                                    int(row["xbox_va"], 16),
                                    "byte-" + row["tier"])
    counts["mnem"] += m.tier_mnem()
    counts["string"] += m.tier_unique_tokens("strings", "string")
    counts["const"] += m.tier_unique_tokens("consts", "const")
    for _ in range(8):
        added = m.tier_mnem()
        added += m.tier_unique_tokens("consts", "const")
        added += m.tier_propagate()
        added += m.tier_callers()
        added += m.tier_interval()
        counts["fixpoint"] += added
        if added == 0:
            break

    status = m.scaffold_status()
    dropped = 0
    for p, st in list(status.items()):
        if st == "flag" and m.tier[p] in WEAK:
            m.remove(p)
            dropped += 1
    status = m.scaffold_status()  # re-classify on the cleaned set

    hooks = load_hooks()
    by_sub = Counter()
    import os
    os.makedirs(f"{ROOT}/re/console/match", exist_ok=True)
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["pc_va", "xbox_va", "tier", "pc_name", "subsystem",
                    "confidence", "scaffold"])
        for pe in sorted(m.p2x):
            name, sub, conf = hooks.get(pe, ("", "", ""))
            if sub:
                by_sub[sub] += 1
            w.writerow([f"0x{pe:08x}", f"0x{m.p2x[pe]:08x}", m.tier[pe],
                        name, sub, conf, status.get(pe, "ambiguous")])

    total = len(m.p2x)
    st_counts = Counter(status.values())
    weak_corrob = sum(1 for p in m.p2x
                      if m.tier[p] in WEAK and status.get(p) == "ok-asc")
    print(f"anchors byte={counts['byte']} mnem={counts['mnem']} "
          f"string={counts['string']} const={counts['const']} "
          f"fixpoint=+{counts['fixpoint']}  dropped(scaffold-flag)={dropped}  "
          f"TOTAL={total}")
    for t, n in Counter(m.tier.values()).most_common():
        print(f"  {t:14} {n}")
    print(f"scaffold: ok-asc(corroborated)={st_counts['ok-asc']}  "
          f"flag(remaining)={st_counts['flag']}  ambiguous={st_counts['ambiguous']}")
    print(f"weak-tier pairs corroborated by scaffold: {weak_corrob}")
    fp_total = sum(1 for va, (_, s, _) in hooks.items()
                   if not s.startswith("third-party"))
    fp_matched = sum(1 for pe in m.p2x
                     if pe in hooks and not hooks[pe][1].startswith("third-party"))
    print(f"first-party hooks.csv coverage: {fp_matched}/{fp_total} "
          f"({100 * fp_matched / max(1, fp_total):.1f}%)")
    print("top subsystems:")
    for sub, n in by_sub.most_common(15):
        print(f"  {sub:45} {n}")
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
