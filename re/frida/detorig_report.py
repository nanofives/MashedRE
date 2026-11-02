# Scores two detorig.py runs (PROMOTE-DETORIG-PILOT, 2026-11-01). Three verdicts per checkpoint pair:
#
#   RAW         the whole writable image (.data + _rwdseg, 0x005ea000..0x00915008) byte-identical
#   NORMALIZED  every differing dword falls in an explained class:
#                 PTR       both values heap addresses (detorig.is_ptr_pair)
#                 RESIDUE   one value through every checkpoint in both runs (last written before the race window)
#                 OFFSET    a - b the same at every checkpoint (counters started at different values)
#                 MODOFFSET (a - b) mod p the same at every checkpoint, p >= every value (wrapping counters)
#                 TOGGLE    both runs only take values 0..3
#   RACE-STATE  the named regions of detorig.REGIONS (frame timer from 0x007f0ff4, since 0x007f0ff0 counts menu
#               frames before the warp) plus both RNG states, the race clock 0x007f0ff4, the frame index
#               0x007f101c and the checkpoint's N, compared with PTR dwords and 0x007f102c (RESIDUE) excluded.
#
#   RULE:<name> an UNEXPLAINED dword that a writer-specific rule accounts for at every checkpoint where it
#               differs (PROMOTE-DETORIG-LOADHOLD, writers cited in RULES below).
#
#   py -3.12 re/frida/detorig_report.py DIR_A DIR_B [--list N]
import argparse, json, struct, sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from detorig import DATA_LO, REGIONS, is_ptr_pair, region_of


def modperiod(vals):
    m = max(max(a, b) for a, b in vals)
    if m > 0x200000:
        return None
    for p in range(max(m, 2), 2 * m + 2):
        if len({(a - b) % p for a, b in vals}) == 1:
            return p
    return None


W = lambda va: (va - DATA_LO) // 4


def rule_ring4(wa, wb, k, i):
    # FUN_004dcbb0 stores the fill offset of dynamic-VB ring slot DAT_007d70c8 at [slot*4 + 0x007d70cc]
    # (0x004dccc8) and its capacity at [slot*4 + 0x007d70dc]; FUN_004dc8a0 advances the slot mod 4. The slot
    # index is a boot-phase counter (MODOFFSET/4), so equal fills land in different slots. Explained when
    # both 4-slot arrays agree after rotating each run to its own current slot.
    va = DATA_LO + 4 * i
    if not (0x007d70cc <= va < 0x007d70ec):
        return False
    sa, sb = wa[k][W(0x007d70c8)] & 3, wb[k][W(0x007d70c8)] & 3
    for base in (0x007d70cc, 0x007d70dc):
        if [wa[k][W(base) + (sa + j) % 4] for j in range(4)] != [wb[k][W(base) + (sb + j) % 4] for j in range(4)]:
            return False
    return True


def rule_acc60(wa, wb, k, i):
    # FUN_004926c0 adds the QPC delta (FUN_004950b0 units, 4,500 per virtual QPC call: 15,000 counts x
    # 3,000,000 / 10,000,000) of each frame to DAT_00771994 (0x00492731), counts frames in DAT_00771978
    # (0x00492737), and at 0x3c stores the average in DAT_0077197c and zeroes both (0x00492758 / 0x00492762).
    # The counter is a boot-phase counter (MODOFFSET/60), so the two runs sum different frame windows.
    # Explained when the counters differ at this checkpoint and the values are whole virtual QPC steps.
    va = DATA_LO + 4 * i
    if va not in (0x00771994, 0x0077197c):
        return False
    if wa[k][W(0x00771978)] == wb[k][W(0x00771978)]:
        return False
    q = 4500 if va == 0x00771994 else 75
    return wa[k][i] % q == 0 and wb[k][i] % q == 0


def rule_winmsg(wa, wb, k, i):
    # The main loop passes the 28-byte MSG at 0x007e95a0 to PeekMessageA (IAT 0x005cc270, call at 0x0049969d).
    # Windows fills it from the thread's message queue: +0x10 time and +0x14 / +0x18 cursor position are the
    # OS's real-time values, not computed by the game.
    return 0x007e95a0 <= DATA_LO + 4 * i < 0x007e95bc


RULES = [("RING4", rule_ring4), ("ACC60", rule_acc60), ("WINMSG", rule_winmsg)]


def load(d):
    meta = json.loads((Path(d) / "meta.json").read_text())
    ck = sorted(meta["ckpt"], key=lambda c: c["k"])
    words = []
    for c in ck:
        b = (Path(d) / c["file"]).read_bytes()
        words.append(struct.unpack(f"<{len(b) // 4}I", b))
    return meta, ck, words


def report(A, B, nlist=20):
    ma, ca, wa = load(A); mb, cb, wb = load(B)
    K = min(len(wa), len(wb)); n = len(wa[0])
    out = {"a": str(A), "b": str(B), "checkpoints": K}
    raw_same = [all(wa[k][i] == wb[k][i] for i in range(n)) for k in range(K)]
    cls = {}
    for i in range(n):
        vals = [(wa[k][i], wb[k][i]) for k in range(K)]
        if all(a == b for a, b in vals): continue
        if all(a == b or is_ptr_pair(a, b) for a, b in vals): cls[i] = "PTR"; continue
        rule = next((name for name, fn in RULES
                     if all(wa[k][i] == wb[k][i] or is_ptr_pair(wa[k][i], wb[k][i]) or fn(wa, wb, k, i) for k in range(K))), None)
        if rule: cls[i] = "RULE:" + rule; continue
        if len({a for a, _ in vals}) == 1 and len({b for _, b in vals}) == 1: cls[i] = "RESIDUE"; continue
        if len({(a - b) & 0xffffffff for a, b in vals}) == 1: cls[i] = "OFFSET"; continue
        p = modperiod(vals)
        if p: cls[i] = f"MODOFFSET/{p}"; continue
        if all(a <= 3 and b <= 3 for a, b in vals): cls[i] = "TOGGLE"; continue
        cls[i] = "UNEXPLAINED"
    unexpl = [i for i, c in cls.items() if c == "UNEXPLAINED"]
    per_k = [sum(1 for i in unexpl if wa[k][i] != wb[k][i] and not is_ptr_pair(wa[k][i], wb[k][i])) for k in range(K)]
    out["raw_identical"] = sum(raw_same)
    out["class_counts"] = dict(Counter(c.split("/")[0] for c in cls.values()))
    out["by_class_and_page"] = dict(Counter(f"{c.split('/')[0]} {region_of(DATA_LO + 4 * i)}" for i, c in cls.items()).most_common())
    out["modoffset_periods"] = dict(Counter(c.split("/")[1] for c in cls.values() if c.startswith("MODOFFSET")))
    out["unexplained_per_checkpoint"] = per_k
    out["normalized_identical"] = sum(1 for u in per_k if u == 0)
    out["unexplained_by_page"] = dict(Counter(region_of(DATA_LO + 4 * i) for i in unexpl).most_common())
    out["unexplained_sample"] = [
        {"va": hex(DATA_LO + 4 * i), "a": [f"{wa[k][i]:08x}" for k in range(K)], "b": [f"{wb[k][i]:08x}" for k in range(K)]}
        for i in unexpl[:nlist]]
    out["residue"] = [{"va": hex(DATA_LO + 4 * i), "a": f"{wa[0][i]:08x}", "b": f"{wb[0][i]:08x}"}
                      for i, c in cls.items() if c == "RESIDUE"]
    # race-state verdict
    regs = [(0x007f0ff4 if "frame timer" in name else lo, hi, name) for lo, hi, name in REGIONS if "phase" not in name]
    rs = []
    for k in range(K):
        bad = {}
        for lo, hi, name in regs:
            c = 0
            for i in range((lo - DATA_LO) // 4, (hi - DATA_LO) // 4):
                x, y = wa[k][i], wb[k][i]
                if x != y and not is_ptr_pair(x, y) and DATA_LO + 4 * i != 0x007f102c: c += 1
            if c: bad[name] = c
        for f in ("holdrand", "rw_ring", "rw_cur", "clk_7f0ff4", "frames_7f101c", "phase", "dn"):
            if ca[k].get(f) != cb[k].get(f): bad[f] = "differs"
        rs.append(bad)
    out["race_state_identical"] = sum(1 for b in rs if not b)
    out["race_state_diffs"] = {k: b for k, b in enumerate(rs) if b}
    out["witness"] = {"a": (ma.get("race") or {}).get("witness"), "b": (mb.get("race") or {}).get("witness")}
    out["race_anchor"] = {"a": {f: (ma.get("race") or {}).get(f) for f in ("n", "dn_warp", "frames_7f101c", "clk_7f0ff0")},
                          "b": {f: (mb.get("race") or {}).get(f) for f in ("n", "dn_warp", "frames_7f101c", "clk_7f0ff0")}}
    return out


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("a"); ap.add_argument("b")
    ap.add_argument("--list", type=int, default=20)
    ap.add_argument("--json", default="")
    a = ap.parse_args()
    r = report(a.a, a.b, a.list)
    if a.json:
        Path(a.json).write_text(json.dumps(r, indent=1))
    K = r["checkpoints"]
    print(f"{Path(a.a).name} vs {Path(a.b).name}: raw {r['raw_identical']}/{K}, normalized {r['normalized_identical']}/{K}, "
          f"race-state {r['race_state_identical']}/{K}")
    print("  classes", r["class_counts"], "periods", r["modoffset_periods"])
    print("  unexplained per checkpoint", r["unexplained_per_checkpoint"])
    print("  unexplained by page", r["unexplained_by_page"])
    if r["race_state_diffs"]:
        print("  race-state diffs", json.dumps(r["race_state_diffs"])[:600])
    print("  witness", r["witness"], "anchor", r["race_anchor"])
