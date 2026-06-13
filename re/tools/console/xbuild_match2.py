#!/usr/bin/env python3
r"""xbuild_match2.py — feature-based PC<->Xbox function matching (pass 2).

Builds on xbuild_match.py (pass-1 byte-identity anchors) with three stronger
tiers over DumpFuncFeatures.java output:

  T1 "mnem"     — mnemonic-sequence hash unique on both sides
  T2 "string"   — unique-string anchors (string referenced by exactly one
                  function per side), majority vote, size sanity check
  T3 "prop-*"   — call-graph propagation: align callee lists of matched pairs
                  on already-matched anchors; equal-length unmatched gaps pair
                  positionally ("prop-mnem" if mnemonic hashes agree, else
                  "prop-weak" with size/instruction-count sanity bounds)

One-to-one mapping enforced; higher tiers win. Iterates propagation to fixpoint.

Output: re/console/match/xbuild_match_v2.csv + coverage summary.
"""
import csv
import sys
from collections import Counter, defaultdict

ROOT = "."
PC_FEAT = f"{ROOT}/re/console/pc_features.csv"
XB_FEAT = f"{ROOT}/re/console/xbox/toast_features.csv"
PASS1 = f"{ROOT}/re/console/match/xbuild_match.csv"
HOOKS = f"{ROOT}/hooks.csv"
OUT = f"{ROOT}/re/console/match/xbuild_match_v2.csv"

RATIO = 1.30  # size / instruction-count sanity bound for weak acceptance


def load_features(path):
    funcs = {}
    with open(path, newline="", encoding="utf-8", errors="replace") as f:
        for row in csv.DictReader(f):
            entry = int(row["entry"], 16)
            funcs[entry] = {
                "size": int(row["size"]),
                "nmnem": int(row["nmnem"]),
                "mnemhash": row["mnemhash"],
                "strings": [s for s in row["strings"].split("|") if s],
                "callees": [int(c, 16) for c in row["callees"].split(";") if c],
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

    def tier1_mnem(self):
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

    def tier2_strings(self):
        def uniq_map(funcs):
            owner = {}
            for e, f in funcs.items():
                for s in f["strings"]:
                    owner[s] = None if s in owner else e
            return {s: e for s, e in owner.items() if e is not None}

        pc_u, xb_u = uniq_map(self.pc), uniq_map(self.xb)
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
                n += self.add(pe, xe, "string")
        return n

    def tier3_propagate(self):
        total = 0
        for rounds in range(30):
            cand = Counter()
            for pe, xe in list(self.p2x.items()):
                cp = [c for c in self.pc[pe]["callees"]]
                cx = [c for c in self.xb[xe]["callees"]]
                # split both lists on matched anchor pairs, pair equal gaps
                gaps = self._gaps(cp, cx)
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


def main():
    pc = load_features(PC_FEAT)
    xb = load_features(XB_FEAT)
    m = Matcher(pc, xb)

    n0 = 0
    with open(PASS1, newline="") as f:
        for row in csv.DictReader(f):
            n0 += m.add(int(row["pc_va"], 16), int(row["xbox_va"], 16),
                        "byte-" + row["tier"])
    n1 = m.tier1_mnem()
    n2 = m.tier2_strings()
    n3 = m.tier3_propagate()
    # string + propagation may unlock new mnem uniques; one more sweep
    n1b = m.tier1_mnem()
    n3b = m.tier3_propagate()

    hooks = load_hooks()
    by_sub = Counter()
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["pc_va", "xbox_va", "tier", "pc_name", "subsystem", "confidence"])
        for pe in sorted(m.p2x):
            name, sub, conf = hooks.get(pe, ("", "", ""))
            if sub:
                by_sub[sub] += 1
            w.writerow([f"0x{pe:08x}", f"0x{m.p2x[pe]:08x}", m.tier[pe],
                        name, sub, conf])

    total = len(m.p2x)
    print(f"pass1 anchors={n0}  +mnem={n1 + n1b}  +string={n2}  "
          f"+prop={n3 + n3b}  TOTAL={total}")
    tiers = Counter(m.tier.values())
    for t, n in tiers.most_common():
        print(f"  {t:12} {n}")
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
