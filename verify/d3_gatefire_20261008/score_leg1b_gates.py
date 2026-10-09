"""D-11073 leg 1b step 5 scorer (PREREG_LEG1B.md section 5 + addenda A/B). Read-only.

usage: py -3.12 score_leg1b_gates.py  (run from verify/d3_gatefire_20261008)

Per knob arm (L1B_SUB, L1B_M5, L1B_M5CD), from <arm>.inputs.csv (one row per frame, written at the
TOP of the frame, so a row shows the state left by the previous frame) and <arm>.log:
  G-PHASE5  every pre-race segment: 1 row at ba8c 4, 112 rows at 5, then 6; the handler's exit read
            (log line) is 0x3feeeee2 every round.
  G-SEED    cd29b8: 12000 on the state-4 row and the first state-5 row, then -50 per row, 0 on the
            first state-6 row.
  G-HOLD    TrackRenderer positions (tx/ty/tz) bit-identical over every pre-race row of a segment
            and on the first state-6 row.
  G-EXIT    bf4/bf8 on the first state-6 row of each segment = what the exit arm copied into
            0x007f0a04/08 (nothing writes the record between the copy and that row: the exit
            frame returns before physics). Derived witness, stated as such.
Report-only: bias374 histogram / reversals from <arm>.gates.csv vs L1B_CTRL; D2 WATCH = first
differing ptrace line vs L1B_CTRL (both start at the first post-GO physics frame).
"""
import csv
import re
from collections import Counter

ARMS = ["L1B_SUB", "L1B_M5", "L1B_M5CD"]


def segments(rows):
    segs, cur = [], None
    for i, r in enumerate(rows):
        pre = float(r["countdown"]) > 0
        if pre and cur is None:
            cur = [i, None]
        if not pre and cur is not None:
            cur[1] = i
            segs.append(cur)
            cur = None
    if cur is not None:
        segs.append([cur[0], None])
    return segs


def bias(path):
    try:
        rs = list(csv.DictReader(open(path)))
    except FileNotFoundError:
        return None
    # one row per (frame, v); take v == 1 for the per-frame series
    seq = [int(r["bias374"]) for r in rs if r.get("v") == "1" and r.get("bias374", "") != ""]
    rev = sum(1 for a, b in zip(seq, seq[1:]) if b < a)
    return len(seq), Counter(seq), rev


def ptrace_lines(path):
    try:
        return [l.rstrip("\n") for l in open(path, errors="replace") if l.startswith("f=")]
    except FileNotFoundError:
        return []


ctrl_b = bias("L1B_CTRL.gates.csv")
ctrl_p = ptrace_lines("L1B_CTRL.ptrace.log")
print("L1B_CTRL bias374: frames %d hist %s reversals %d" % (ctrl_b[0], dict(ctrl_b[1]), ctrl_b[2]))

for arm in ARMS:
    print("\n==================== %s" % arm)
    try:
        R = list(csv.DictReader(open(arm + ".inputs.csv")))
    except FileNotFoundError:
        print("  MISSING inputs.csv")
        continue
    log = open(arm + ".log", errors="replace").read() if True else ""
    exits = re.findall(r"\[substate\] exit d584=(\d+) d588_read=\S+ \((0x[0-9a-f]+)\)", log)
    unexpected = log.count("UNEXPECTED arm")
    print("  log: [substate] armed=%d  exit lines=%d %s  cd-seed lines=%d  UNEXPECTED=%d  [l1b-inputs]=%d"
          % (log.count("MASHED_SUBSTATE=1"), len(exits), Counter(exits).most_common(3),
             log.count("MASHED_SUBSTATE_CD="), unexpected, log.count("[l1b-inputs]")))
    segs = segments(R)
    ok5 = okseed = okhold = 0
    for si, (a, b) in enumerate(segs):
        pre = R[a:b] if b is not None else R[a:]
        st = Counter(r["ba8c"] for r in pre)
        post = R[b] if b is not None else None
        g5 = (st.get("4", 0) == 1 and st.get("5", 0) == 112 and post is not None and post["ba8c"] == "6")
        # seed sequence
        s4 = [int(r["cd29b8"]) for r in pre if r["ba8c"] == "4"]
        s5 = [int(r["cd29b8"]) for r in pre if r["ba8c"] == "5"]
        seed = (s4 == [12000] and len(s5) == 112 and s5[0] == 12000
                and all(y - x == -50 for x, y in zip(s5, s5[1:]))
                and post is not None and int(post["cd29b8"]) == 0)
        hold = True
        for v in range(4):
            ps = set((r["tx%d" % v], r["ty%d" % v], r["tz%d" % v]) for r in pre + ([post] if post else []))
            hold = hold and len(ps) == 1
        ok5 += g5; okseed += seed; okhold += hold
        if si < 3 or not (g5 and seed and hold):
            print("  seg %d rows %d..%s states %s  G-PHASE5 %s  G-SEED %s (s5 %s..%s)  G-HOLD %s"
                  % (si, a, b, dict(st), g5, seed, s5[:1], s5[-1:], hold))
            if post is not None:
                print("     first state-6 row f=%s d584=%s d588=%s bf4=%s bf8=%s acc(last pre)=%s"
                      % (post["frame"], post["d584"], post["d588"],
                         [post["bf4r%d" % v] for v in range(4)], [post["bf8r%d" % v] for v in range(4)],
                         [pre[-1]["acc%d" % v] for v in range(4)]))
    print("  SEGMENTS %d: G-PHASE5 %d/%d  G-SEED %d/%d  G-HOLD %d/%d  exit reads 0x3feeeee2: %d/%d"
          % (len(segs), ok5, len(segs), okseed, len(segs), okhold, len(segs),
             sum(1 for e in exits if e[1] == "0x3feeeee2"), len(exits)))
    b = bias(arm + ".gates.csv")
    if b:
        print("  bias374 (post-GO rows only): frames %d hist %s reversals %d   [CTRL reversals %d]"
              % (b[0], dict(b[1]), b[2], ctrl_b[2]))
    P = ptrace_lines(arm + ".ptrace.log")
    first = next((i for i, (x, y) in enumerate(zip(ctrl_p, P)) if x != y), None)
    print("  D2 WATCH ptrace: %d lines (CTRL %d); first differing f-line index %s" % (len(P), len(ctrl_p), first))
    if first is not None:
        print("     CTRL: %s" % ctrl_p[first][:200])
        print("     ARM : %s" % P[first][:200])
