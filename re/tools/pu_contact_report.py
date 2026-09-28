"""pu_contact_report.py -- per-activation view of the power-up CONTACT chain (D3-CONTACT, 2026-09-27).

Pairs the two CSVs that one `re/frida/scenario_launch.py --statediff-puhook
--puhook-contacts` run writes:

  <out>.puhook.csv     one row per dispatcher call per slot (decision trace)
  <out>.pucontact.csv  frame,call,rva,name,ret_addr,a1,a2,a3,ret

and reports, per activation on the subject slot, which contact-chain CALL SITES
fired while that type was held. This is an ORIGINAL-SIDE reference capture: the
port has no counterpart for any of these RVAs (hooks.csv holds 0x004b4b60,
0x004b4cd0, 0x004b4d10, 0x004b5080, 0x00455910, 0x00455100 at C2 `mapped` and
0x0045c350 at C2 `new`), so there is nothing to diff against yet. It exists so a
port of the chain has a measured target.

TWO RULES THIS TOOL ENFORCES, both learned the hard way on this capture:

1. ATTRIBUTE BY CALL SITE, NOT BY RVA. Each of these RVAs has several callers.
   0x0045c350 was called 18 times in c2, but only ONCE from the dispatcher
   (return address 0x45bcea); the other 17 came from 0x48708c and 0x4537bb. A
   per-RVA count inside an activation window therefore reads as a contact that
   the dispatcher never made. Every line below is keyed on (name, ret_addr).

2. ATTRIBUTE THE DISPATCHER SWEEP BY SLOT, NOT BY WINDOW. The dispatcher runs
   its per-slot pass over all 4 slots, so a sweep call inside the subject's
   activation window is usually another slot's. MEASURED: arg2 of 0x004b4b60 at
   the dispatcher call site is slot_base + 0x80 (observed values 0x88fc60,
   0x88fd14, 0x88fdc8 against slot bases 0x0088fbe0 + s*0xb4 = 0x88fbe0,
   0x88fc94, 0x88fd48, 0x88fdfc), so the slot is recoverable exactly.

The dispatcher's armed sweep, disassembled from original/MASHED.exe.unpatched
(the pinned anchor) at 0x0045bcc8..0x0045bd11:
  0x0045bcd3  CALL 0x004b4b60   -> sweep_query    (returns to 0x45bcd8)
  0x0045bcdb  TEST EAX,EAX / JE 0x45bd14          (0 => no contact, branch skipped)
  0x0045bce5  CALL 0x0045c350   -> sweep_confirm  (returns to 0x45bcea)
  0x0045bced  TEST EAX,EAX / JNE 0x45bd14         (nonzero => keep the power-up)
  0x0045bcf7  CALL 0x0045bac0   -> deactivate, then 0x0045bd0c CALL 0x00476880
So "the armed sweep deactivated the power-up" == a dispatcher sweep_query with
ret != 0 whose dispatcher sweep_confirm, in the same dispatcher call, returns 0.

Usage: py -3.12 re/tools/pu_contact_report.py <out.msd> [--slot 0]
"""
import argparse
import csv
import sys
from collections import defaultdict, OrderedDict

NAMES = {9: "GUN", 10: "DRUM", 11: "MISSILE", 12: "P_MINE", 16: "R_FLAME", 17: "SHOTGUN",
         18: "FLASH", 19: "OIL", 7: "MORTAR"}

SWEEP_QUERY_RA = 0x0045bcd8     # 0x0045bcd3 CALL 0x004b4b60
SWEEP_CONFIRM_RA = 0x0045bcea   # 0x0045bce5 CALL 0x0045c350
SLOT_BASE = 0x0088fbe0
SLOT_STRIDE = 0xb4
SWEEP_ARG2_DELTA = 0x80         # measured: arg2 == slot_base + 0x80


def read_csv(path):
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def slot_of(a2):
    off = a2 - SLOT_BASE - SWEEP_ARG2_DELTA
    if off < 0 or off % SLOT_STRIDE or off // SLOT_STRIDE > 15:
        return None
    return off // SLOT_STRIDE


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base", help="the --statediff-out path (without .puhook.csv)")
    ap.add_argument("--slot", type=int, default=0)
    args = ap.parse_args()

    pu = read_csv(args.base + ".puhook.csv")
    cx = read_csv(args.base + ".pucontact.csv")

    acts = []
    for r in pu:
        if int(r["slot"]) != args.slot:
            continue
        if r["act"].startswith("A"):
            acts.append((int(r["call"]), int(r["act"][1:])))
    if not acts:
        print("no activations on slot %d -- nothing to attribute" % args.slot)
        return 1

    # window end = first later call where the slot no longer holds a type,
    # else the next activation.
    ends = {}
    for i, (call, _code) in enumerate(acts):
        nxt = acts[i + 1][0] if i + 1 < len(acts) else 10 ** 9
        end = nxt
        for r in pu:
            if int(r["slot"]) != args.slot:
                continue
            c = int(r["call"])
            if call < c < nxt and int(r["code_post"]) == -1:
                end = c
                break
        ends[call] = end

    by_call = defaultdict(list)
    for r in cx:
        by_call[int(r["call"])].append(r)

    print("capture: %s   (subject slot %d)" % (args.base, args.slot))
    print("contact rows: %d" % len(cx))

    # ---- global: every (name, ret_addr) call site actually seen --------------
    sites = OrderedDict()
    for r in cx:
        k = (r["name"], r["rva"], r["ret_addr"])
        v = sites.setdefault(k, [0, 0])
        v[0] += 1
        if int(r["ret"]) != 0:
            v[1] += 1
    print()
    print("ALL CALL SITES (name, rva, return address) -> calls, ret!=0")
    for (nm, rva, ra), (n, nz) in sorted(sites.items(), key=lambda kv: -kv[1][0]):
        print("  %-17s %-10s from %-10s  calls=%-6d ret!=0=%d" % (nm, rva, ra, n, nz))

    # ---- the dispatcher armed sweep, per slot -------------------------------
    print()
    print("DISPATCHER ARMED SWEEP (ret_addr 0x%x / 0x%x only), per slot"
          % (SWEEP_QUERY_RA, SWEEP_CONFIRM_RA))
    q_n = defaultdict(int)
    q_hit = defaultdict(int)
    deact = defaultdict(list)
    for call, rows in by_call.items():
        hit_slots = []
        for r in rows:
            if r["name"] != "sweep_query" or int(r["ret_addr"], 16) != SWEEP_QUERY_RA:
                continue
            s = slot_of(int(r["a2"], 16))
            key = s if s is not None else "?"
            q_n[key] += 1
            if int(r["ret"]) != 0:
                q_hit[key] += 1
                hit_slots.append(key)
        if not hit_slots:
            continue
        conf = [r for r in rows
                if r["name"] == "sweep_confirm" and int(r["ret_addr"], 16) == SWEEP_CONFIRM_RA]
        for i, s in enumerate(hit_slots):
            if i < len(conf) and int(conf[i]["ret"]) == 0:
                deact[s].append(call)
    if not q_n:
        print("  no dispatcher sweep call recorded")
    for s in sorted(q_n, key=str):
        print("  slot %-3s query calls=%-6d ret!=0=%-4d  deactivations=%d %s"
              % (s, q_n[s], q_hit[s], len(deact[s]), deact[s][:8]))

    # ---- per activation window ---------------------------------------------
    for call, code in acts:
        end = ends[call]
        agg = defaultdict(lambda: [0, 0])
        for c in range(call, end):
            for r in by_call.get(c, ()):
                # the dispatcher sweep is reported per slot above, not per window
                if int(r["ret_addr"], 16) in (SWEEP_QUERY_RA, SWEEP_CONFIRM_RA):
                    continue
                v = agg[(r["name"], r["rva"], r["ret_addr"])]
                v[0] += 1
                if int(r["ret"]) != 0:
                    v[1] += 1
        print()
        print("[%-7s code=%2d] activated at call %d, held %d calls (to %s)"
              % (NAMES.get(code, "?"), code, call, end - call,
                 end if end < 10 ** 9 else "end of capture"))
        if not agg:
            print("    no non-dispatcher contact call in the window")
        for (nm, rva, ra), (n, nz) in sorted(agg.items(), key=lambda kv: -kv[1][0]):
            print("    %-17s %-10s from %-10s calls=%-6d ret!=0=%d" % (nm, rva, ra, n, nz))
    return 0


if __name__ == "__main__":
    sys.exit(main())
