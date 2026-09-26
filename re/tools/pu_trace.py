"""pu_trace.py -- compact per-activation trace of a --statediff-puhook capture (D3 WS-D).

Reads <out>.puhook.csv written by re/frida/scenario_launch.py --statediff-puhook (original)
or by the standalone's MASHED_PU_STEPDUMP (same columns). For one slot, prints every
dispatcher call on which something happened: activation, FIRE (with the mode the
dispatcher passed), CANFIRE return, deactivation (with the caller return address), and
the pool-record dwords that CHANGED across the call (offset:old->new).

Usage: py -3.12 re/tools/pu_trace.py <csv> [--slot 0] [--code 19] [--state 6]
"""
import argparse, csv, struct


def words(h):
    if not h or h == "ERR":
        return []
    b = bytes.fromhex(h)
    return [struct.unpack_from("<I", b, i)[0] for i in range(0, len(b) - len(b) % 4, 4)]


def fmt(w):
    f = struct.unpack("<f", struct.pack("<I", w))[0]
    if 1e-6 < abs(f) < 1e7:
        return f"{w:08x}({f:.6g})"
    return f"{w:08x}"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv")
    ap.add_argument("--slot", default="0")
    ap.add_argument("--code", type=int, default=None)
    ap.add_argument("--state", default="6")
    a = ap.parse_args()
    rows = [r for r in csv.DictReader(open(a.csv)) if r["slot"] == a.slot]
    t0 = None
    for r in rows:
        if a.state and r["state"] != a.state:
            continue
        cp, cq = int(r["code_pre"]), int(r["code_post"])
        if a.code is not None and a.code not in (cp, cq):
            continue
        if r["act"]:
            t0 = int(r["call"])
            print(f"=== {r['act']} at call {r['call']}  rec0: "
                  + " ".join(f"+{i*4:02x}:{fmt(w)}" for i, w in enumerate(words(r["rec_post"]))))
        wa, wb = words(r["rec_pre"]), words(r["rec_post"])
        diff = [f"+{i*4:02x}:{fmt(x)}->{fmt(y)}" for i, (x, y) in enumerate(zip(wa, wb)) if x != y]
        ev = r["fire_modes"] or r["deact_ra"] or r["canfire_rets"] not in ("", "0") or diff \
            or cp != cq
        if not ev or r["act"]:
            continue
        rel = int(r["call"]) - t0 if t0 is not None else -1
        print(f"  t+{rel:<4} c{r['cur3']:>3} p{r['prev3']:>3} code {cp}->{cq} "
              f"F[{r['fire_modes']}] C[{r['canfire_rets']}] D[{r['deact_ra']}] dt={r['dt']} "
              + " ".join(diff))


if __name__ == "__main__":
    main()
