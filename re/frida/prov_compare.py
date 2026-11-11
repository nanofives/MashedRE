# PROV-INSITU per-tick verdict (2026-11-11): one detorig.py run with DDK bodies installed (--prov) against a reference
# run of the same configuration with no body, using a second reference run to name the blocks that differ between two
# unmodified runs (the MASK / RULE blocks of RO-WARP-NORMALIZE).
#
# Unlike detorig_ticks.py (which does not end the count on a CONSTANT block, one that differs from tick 0 on), every
# block outside the allowed set counts from tick 0: the bodies go live at the warp, before the load, so a divergence
# made during the load shows as a block that already differs at tick 0.
#
#   py -3.12 re/frida/prov_compare.py RUN REF_A REF_B [--json OUT]
# Verdict: IDENTICAL-FOR-N (N = ticks compared) or DIVERGED at tick t with the blocks (and raw words) that differ.
import argparse, json, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from detorig import DATA_LO, is_ptr_pair, region_of
from detorig_ticks import load


def diffs(ra, rb, nblk, n):
    return [{j for j in range(2, 2 + nblk) if ra[i][j] != rb[i][j]} for i in range(n)]


# LOAD-LENGTH rule (PROV-INSITU, measured 2026-11-11): the float 0x007f1010 in the frame-timer block holds a value set
# during the load, so at tick 0 it differs between two runs whose load took a different number of main-thread clock
# calls (x_ref 119 calls 0x43a26719, x_b1b / g95 118 calls 0x43a264f7), and the block is equal again from tick 1.
# Applied only at tick 0, only when the load lengths differ, only when that block is the one bad block and the raw
# copy of 0x007f0c00..0x007f1400 shows 0x007f1010 as the only differing dword.
def _masked():
    """MASK:* rows of re/frida/warp_normalize.tsv (OS-assigned, library-owned, set before any hook) plus the RULE rows
    that hold OS handles / messages / the stdout cursor (HWND, WINMSG, FILEPTR). The STACKROW / RINGSTALE / RWPAD /
    RING4 rules are not masked: a body under test can write those structures."""
    out = set()
    for ln in (Path(__file__).resolve().parent / "warp_normalize.tsv").read_text(encoding="utf-8").splitlines():
        f = ln.split("	")
        if len(f) < 5 or not f[0].startswith("0x"):
            continue
        act = f[4]
        if act.startswith("MASK:") or act in ("RULE:HWND", "RULE:WINMSG", "RULE:FILEPTR"):
            va, cnt, stride, width = int(f[0], 16), int(f[1]), int(f[2], 16), int(f[3])
            for k in range(cnt):
                for o in range(0, max(width, 4), 4):
                    out.add(va + k * stride + o)
    return out


MASKED = _masked()
MASKED_LIST = sorted(MASKED)
LOADLEN_BLOCK, LOADLEN_DWORDS = 0x007f1000, {0x007f1010}


def loadlen_rule(x, y, bad, raw_va, nblk, blk, mr, ma, notes):
    if mr["race"].get("dn_warp") == ma["race"].get("dn_warp"):
        return False
    if [DATA_LO + (j - 2) * blk for j in bad] != [LOADLEN_BLOCK]:
        return False
    words = {raw_va[j - 2 - nblk] for j in range(2 + nblk, len(x)) if x[j] != y[j]
             and LOADLEN_BLOCK <= raw_va[j - 2 - nblk] < LOADLEN_BLOCK + blk}
    if not raw_va or not any(LOADLEN_BLOCK <= v < LOADLEN_BLOCK + blk for v in raw_va) or not words or not words <= LOADLEN_DWORDS:
        return False
    notes.append(f"tick 0: {sorted(hex(w) for w in words)} differ with load lengths {mr['race'].get('dn_warp')} / {ma['race'].get('dn_warp')} (LOAD-LENGTH rule)")
    return True


def verdict(run, ref_a, ref_b, extra_pairs=()):
    try:
        mr, tr, rr = load(run)
    except (SystemExit, FileNotFoundError, KeyError) as e:
        m = {}
        try:
            m = json.loads((Path(run) / "meta.json").read_text())
        except Exception:
            pass
        return {"run": str(run), "verdict": "NO-TICKS", "identical_for": 0, "ticks": 0, "first": None,
                "rc": m.get("rc"), "exit": m.get("exit"), "detail": str(e)[:200]}
    ma, ta, ra = load(ref_a)
    mb, tb, rb = load(ref_b)
    nblk, blk = ta["nblk"], ta["block"]
    assert tr["recw"] == ta["recw"] == tb["recw"]
    n0 = min(len(ra), len(rb))
    allowed = set().union(*diffs(ra, rb, nblk, n0)) if n0 else set()
    for pa, pb in extra_pairs:
        _, tpa, rpa = load(pa)
        _, _, rpb = load(pb)
        assert tpa["nblk"] == nblk
        allowed |= set().union(*diffs(rpa, rpb, nblk, min(len(rpa), len(rpb))))
    n = min(len(rr), len(ra))
    raw_va = []
    for lo, hi in ta.get("raw") or []:
        raw_va += list(range(lo, hi, 4))
    va = lambda j: DATA_LO + (j - 2) * blk
    rawpos = {v: 2 + nblk + k for k, v in enumerate(raw_va)}
    first = None
    notes = []
    for i in range(n):
        x, y = rr[i], ra[i]
        if (x[1] - mr["race"]["n"]) != (y[1] - ma["race"]["n"]):
            # the per-tick clock-call count is not a divergence by itself under --clock tick (REPLAY-ORACLE-DESIGN);
            # recorded only
            pass
        bad = []
        for j in range(2, 2 + nblk):
            if x[j] == y[j]:
                continue
            lo = DATA_LO + (j - 2) * blk
            if lo in rawpos and all(lo + 4 * k in rawpos for k in range(blk // 4)):
                # block copied raw every tick: decided per dword, MASK / OS dwords dropped, heap-pointer pairs dropped
                if any(x[rawpos[w]] != y[rawpos[w]] and w not in MASKED and not is_ptr_pair(x[rawpos[w]], y[rawpos[w]])
                       for w in range(lo, lo + blk, 4)):
                    bad.append(j)
            elif j not in allowed and not any(lo <= w < lo + blk for w in MASKED_LIST):
                bad.append(j)
        if i == 0 and bad and loadlen_rule(x, y, bad, raw_va, nblk, blk, mr, ma, notes):
            bad = []
        if bad:
            raw = [(hex(raw_va[j - 2 - nblk]), f"{x[j]:08x}", f"{y[j]:08x}") for j in range(2 + nblk, len(x))
                   if x[j] != y[j] and not is_ptr_pair(x[j], y[j])]
            first = {"tick": i, "blocks": [[hex(va(j)), region_of(va(j))] for j in bad[:24]], "nblocks": len(bad), "raw": raw[:24]}
            break
    rep = {"run": str(run), "ref": str(ref_a), "ref2": str(ref_b), "ticks": n, "ref_ticks": n0,
           "allowed_blocks": [hex(va(j)) for j in sorted(allowed)], "complete": n >= 2724,
           "verdict": ("IDENTICAL-FOR-N" if n >= n0 else "INCOMPLETE") if first is None else "DIVERGED", "identical_for": n if first is None else first["tick"],
           "first": first, "rc": mr.get("rc"), "exit": mr.get("exit"), "notes": notes,
           "dn_warp": [mr["race"].get("dn_warp"), ma["race"].get("dn_warp")]}
    return rep


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("run"); ap.add_argument("ref_a"); ap.add_argument("ref_b")
    ap.add_argument("--json", default="")
    ap.add_argument("--pair", action="append", default=[], help="A,B: another reference pair whose differing blocks are allowed")
    a = ap.parse_args()
    r = verdict(a.run, a.ref_a, a.ref_b, [p.split(",") for p in a.pair])
    print(f"{a.run}: {r['verdict']} {r['identical_for']} of {r['ticks']} (allowed blocks {len(r['allowed_blocks'])}, rc {r['rc']})")
    if r["first"]:
        print(f"  first divergence tick {r['first']['tick']}: {r['first']['nblocks']} blocks {r['first']['blocks'][:8]}")
        for w in r["first"]["raw"][:10]:
            print("   raw", w)
    if a.json:
        Path(a.json).write_text(json.dumps(r, indent=1))
