"""Score G-STEP / G-STEP-CTL (PREREG_LEG1A.md section 4). Pure reads of the two poller CSVs."""
import csv, statistics, sys
import numpy as np
D = r"C:\Users\maria\Desktop\Proyectos\Mashed\verify\d3_gatefire_20261008"
STEP_BITS = 0x3c888888
step = np.uint32(STEP_BITS).view(np.float32)
acc = [np.float32(0)]
for _ in range(20000):
    acc.append(np.float32(acc[-1] + step))
accbits = [int(a.view(np.uint32)) for a in acc]

def rows(name):
    return list(csv.DictReader(open(f"{D}\\{name}")))

sub = rows("L1A_step_SUB.csv")
live = [r for r in sub if int(r["n_d584"]) > 0]
first = live[0] if live else None
a_ok = sum(1 for r in live[1:] if int(r["dt_100c_bits"], 16) == STEP_BITS)
a_n = len(live) - 1
exact = torn = bad = 0
bad_rows = []
for r in live:
    n = int(r["n_d584"]); b = int(r["t_d588_bits"], 16)
    if n < len(accbits) and b == accbits[n]: exact += 1
    elif n - 1 >= 0 and b == accbits[n - 1]: torn += 1
    else:
        bad += 1
        if len(bad_rows) < 5: bad_rows.append((n, r["t_d588"], float(acc[n]) if n < len(acc) else None))
ratios = []
for p, q in zip(live, live[1:]):
    dn = int(q["n_d584"]) - int(p["n_d584"])
    if dn > 0: ratios.append((float(q["t_d588"]) - float(p["t_d588"])) / dn)
med = statistics.median(ratios) if ratios else None
print("SUB samples", len(sub), "live (n_d584>0)", len(live), "first live n", first["n_d584"] if first else None)
print("  (a) dt_100c == 0x3c888888 after first live: %d/%d" % (a_ok, a_n))
print("  (b) t_d588 == float32 sum(n): exact %d, torn(n-1) %d, other %d  -> exact share %.4f" %
      (exact, torn, bad, exact / max(1, len(live))))
if bad_rows: print("      first non-matching:", bad_rows)
print("  (c) median d(t)/d(n) = %r over %d pairs (|diff| = %.3g)" % (med, len(ratios), abs(med - 0.0166667) if med else -1))
print("  last sample n=%s t=%s" % (live[-1]["n_d584"], live[-1]["t_d588"]))
G_STEP = (a_n > 0 and a_ok == a_n and bad == 0 and exact / max(1, len(live)) >= 0.90
          and med is not None and abs(med - 0.0166667) <= 1e-6)
print("G-STEP:", "PASS" if G_STEP else "FAIL")

off = rows("L1A_step_OFF.csv")
nz = [r for r in off if int(r["n_d584"]) != 0 or int(r["t_d588_bits"], 16) != 0 or int(r["dt_100c_bits"], 16) != 0]
clk_adv = len({r["clk_0ff4"] for r in off})
print("OFF samples", len(off), "non-zero rows", len(nz), "distinct clk_0ff4 values", clk_adv)
print("G-STEP-CTL:", "PASS" if not nz and clk_adv > 1 else "FAIL")
