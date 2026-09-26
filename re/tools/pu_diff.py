"""pu_diff.py -- diff a power-up DECISION trace: original vs port (D3 WS-D, 2026-09-26).

  original : <out>.puhook.csv   from re/frida/scenario_launch.py --statediff-puhook
  port     : MASHED_PU_STEPDUMP from the standalone / re/tools/pu_replay (same columns)

Rows are aligned per activation ("act" column) by t = frames since that activation.
Compared at every t where either side has a row:
  code_post       held type after the call (deactivation timing)
  fire_modes      the fire_mode FUN_0045bba0 passed to FIRE (0x0045bdbb)
  canfire_rets    CANFIRE return (0x0045bd58)
  deact           whether FUN_0045bac0 ran, and via which branch
                  (0x45bd67 CANFIRE, 0x45be52 discard edge)
  prev3           the shadow byte (checks the port's cook-shadow latch)
  per-type fields decoded from the original pool record vs the port slot state:
    GUN ammo/timer/charge, OIL supply, FLASH state, SHOTGUN pellets/refire,
    MISSILE ammo/in-flight/life, MORTAR ammo/cooldown, DRUM|P_MINE drops,
    R_FLAME major/sub/jet/cooldown
Floats compare as float32, exactly by default (--ftol to relax).

Usage: py -3.12 re/tools/pu_diff.py <orig.csv> <port.csv> [--slot 0] [--ftol 1e-6]
Exit 0 = clean, 1 = mismatches.
"""
import argparse, csv, struct, sys
from collections import defaultdict

# (name, kind, pool-record offset, port field)
FIELDS = {
    9:  [("ammo", "i", 0x10, "ammo"), ("timer", "f", 0x04, "cooldown"), ("charge", "f", 0x18, "charge")],
    19: [("supply", "f", 0x00, "charge")],
    18: [("state", "i", 0x0c, "sub")],
    17: [("pellets", "i", 0x10, "ammo"), ("refire", "i", 0x0c, "counter")],
    11: [("ammo", "i", 0x08, "ammo"), ("inflight", "i", 0x1c, "jet"), ("life", "f", 0x28, "life")],
    7:  [("ammo", "i", 0x0c, "ammo"), ("cooldown", "f", 0x10, "cooldown")],
    10: [("drops", "i", 0x08, "ammo")],
    12: [("drops", "i", 0x08, "ammo")],
    16: [("major", "i", 0x14, "ammo"), ("sub", "i", 0x18, "sub"), ("jet", "i", 0x1c, "jet"),
         ("cooldown", "f", 0x08, "cooldown")],
}
NAMES = {9: "GUN", 10: "DRUM", 11: "MISSILE", 12: "P_MINE", 16: "R_FLAME", 17: "SHOTGUN",
         18: "FLASH", 19: "OIL", 7: "MORTAR"}
PORT_KEYS = ["ammo", "cooldown", "charge", "jet", "sub", "counter", "life"]


def orig_fields(code, hexrec):
    if code not in FIELDS or not hexrec or hexrec == "ERR":
        return {}
    b = bytes.fromhex(hexrec)
    out = {}
    for name, kind, off, pk in FIELDS[code]:
        if off + 4 <= len(b):
            out[pk] = struct.unpack_from("<i" if kind == "i" else "<f", b, off)[0]
    return out


def port_fields(code, rec):
    if code not in FIELDS or not rec:
        return {}
    v = rec.split("|")
    d = dict(zip(PORT_KEYS, v))
    out = {}
    for name, kind, off, pk in FIELDS[code]:
        out[pk] = int(d[pk]) if kind == "i" else float(d[pk])
    return out


def load(path, slot, is_port):
    acts = []           # list of (code, {t: row})
    cur = None
    t0 = None
    idle = 0
    for r in csv.DictReader(open(path)):
        if r["slot"] != str(slot) or r["state"] != "6":
            continue
        call = int(r["call"])
        act = r["act"]
        if act:
            code = int(r["code_post"])
            cur = {}
            acts.append((code, cur))
            t0 = call
        if cur is None:
            continue
        cur[call - t0] = r
    return acts


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("orig"); ap.add_argument("port")
    ap.add_argument("--slot", default="0")
    ap.add_argument("--ftol", type=float, default=0.0)
    ap.add_argument("--max", type=int, default=12, help="mismatch lines printed per type")
    a = ap.parse_args()
    O = load(a.orig, a.slot, False)
    P = load(a.port, a.slot, True)
    if len(O) != len(P):
        print(f"ACTIVATION COUNT differs: orig {len(O)} port {len(P)}")
    total_bad = 0
    summary = []
    for i, ((oc, orows), (pc, prows)) in enumerate(zip(O, P)):
        name = NAMES.get(oc, str(oc))
        bad = []
        n = 0
        checked = defaultdict(int)
        for t in sorted(set(orows) | set(prows)):
            o, p = orows.get(t), prows.get(t)
            n += 1
            if o is None or p is None:
                bad.append(f"t+{t}: row only in {'port' if o is None else 'orig'} "
                           f"({(p or o)['code_pre']}->{(p or o)['code_post']} "
                           f"F[{(p or o)['fire_modes']}] D[{(p or o)['deact_ra']}])")
                continue
            for col in ("code_post", "fire_modes", "canfire_rets", "deact_ra", "prev3"):
                checked[col] += 1
                if o[col] != p[col]:
                    bad.append(f"t+{t}: {col} orig={o[col]!r} port={p[col]!r}")
            code = int(o["code_pre"]) if int(o["code_pre"]) >= 0 else int(o["code_post"])
            of = orig_fields(code, o["rec_post"]) if int(o["code_post"]) >= 0 else {}
            pf = port_fields(code, p["rec_post"]) if int(p["code_post"]) >= 0 else {}
            for k in of:
                if k not in pf:
                    continue
                checked[k] += 1
                ov, pv = of[k], pf[k]
                if isinstance(ov, float):   # compare as float32 (port prints %.9g = round-trip)
                    pv = struct.unpack('<f', struct.pack('<f', pv))[0]
                    ok = abs(ov - pv) <= a.ftol
                else:
                    ok = ov == pv
                if not ok:
                    bad.append(f"t+{t}: {k} orig={ov!r} port={pv!r}")
        total_bad += len(bad)
        verdict = "CLEAN" if not bad else f"{len(bad)} MISMATCH"
        summary.append(f"  [{i}] {name:8s} frames={n:4d} {verdict:14s} checked="
                       + ",".join(f"{k}:{v}" for k, v in sorted(checked.items())))
        for line in bad[:a.max]:
            summary.append("        " + line)
        if len(bad) > a.max:
            summary.append(f"        ... {len(bad) - a.max} more")
    print("\n".join(summary))
    print("VERDICT:", "CLEAN" if total_bad == 0 and len(O) == len(P) else f"{total_bad} mismatches")
    sys.exit(0 if total_bad == 0 and len(O) == len(P) else 1)


if __name__ == "__main__":
    main()
