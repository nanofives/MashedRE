"""Label one original (b) run by regime, from the capture's OWN DAT_0089a368 witnesses. Read-only.

usage: py -3.12 label_run.py <stem>      (reads <stem>.msd.aistep.csv and <stem>.poll.csv)
Last line printed is the run label: A / B / C / MIXED / NO-AISTEP.

Rule (ROADMAP.md §D3 (b) amendment 2026-10-09, re/tools/ai_ctrl_window.py window_regime): per car, the
FIRST non-zero `flag_a368` value inside the car's 220-call window (window = from the first call with
c4 != 0): none -> A, 1 -> B, 2 -> C. Second witness: the orig_rampwatch poll column `mode368`
(same address, polled at 60 Hz), reported but not used for the label."""
import csv, os, sys, importlib.util
from collections import Counter
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.abspath(os.path.join(here, "..", ".."))
spec = importlib.util.spec_from_file_location("w", os.path.join(root, "re", "tools", "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
stem = sys.argv[1]
ai, poll = stem + ".msd.aistep.csv", stem + ".poll.csv"
if os.path.exists(poll):
    rows = list(csv.DictReader(open(poll, newline="")))
    print("poll mode368 counts %s" % dict(Counter(r["mode368"] for r in rows)))
if not os.path.exists(ai):
    print("NO-AISTEP"); sys.exit(1)
rg = w.window_regime(ai, 220)
st = w.window_stats(ai, 220)
rows = list(csv.DictReader(open(ai, newline="")))
for v in sorted(st):
    if v < 1:
        continue
    r = [x for x in rows if int(x["v"]) == v]
    fz = next(((x["frame"], x["flag_a368"]) for x in r if x["flag_a368"] not in ("0", "")), None)
    s = st[v]
    print("v%d regime=%s window_start=%s calls=%s%s first_nonzero_flag(frame,value)=%s" % (
        v, rg.get(v), s.get("start_frame"), s.get("calls"), " SHORT" if s.get("short") else "", fz))
labs = {rg[v] for v in rg if v >= 1}
print(labs.pop() if len(labs) == 1 else ("MIXED" if labs else "NO-GO"))
