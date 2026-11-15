"""C4-NOTAG-87-REEARN: build the two promotion TSVs from the probe reports (reearn87_probe<k>_report.tsv).

re/promotions/C4-NOTAG-87-REEARN-match.tsv  rows whose match gate PASSed (route match, reearn=1). Source choice: the
                                            passing source whose file stem is the rva; else the first passing round.
re/promotions/C4-NOTAG-87-REEARN.tsv        every other row of the 87: route demote, evidence = the audit TSV (names
                                            every rva), reason = audit class + the match refusal; the 5 rows whose
                                            frida_diff cell is `green` get the dcphys RESULT in the frida_diff column.
"""
import csv, glob, pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
D = ROOT / "verify/c4audit_20261115"
AUDIT = "verify/c4audit_20261115/c4_notag_audit.tsv"
RESULT = "verify/c4audit_20261115/RESULT_C4-NOTAG-87-REEARN.md"
# Cited from the promotion commits (fadf5eb63, 2d9cd2b2f, 22e9bc96d) and the RVA lines of each RESULT.
DCPHYS = {"0x0046b540": "verify/dcphys_20261021/RESULT_dcphys.md",
          "0x00467650": "verify/dcphys_20261021/RESULT_dcphys.md",
          "0x0046ddb0": "verify/dcphys2_20261022/RESULT_dcphys2.md",
          "0x00468980": "verify/dcphys2_20261022/RESULT_dcphys2.md",
          "0x00470670": "verify/dcphys2_20261022/RESULT_dcphys2.md"}

rows = list(csv.DictReader(open(D / "reearn87_rows.tsv", encoding="utf-8", newline=""), delimiter="\t"))
passes, refusals = {}, {}
for f in sorted(glob.glob(str(D / "reearn87_probe[0-9]_report.tsv"))):
    for x in csv.DictReader(open(f, encoding="utf-8", newline=""), delimiter="\t"):
        (passes if x["verdict"] == "PASS" else refusals).setdefault(x["rva"], []).append((x["evidence"], x["detail"]))
match_rows, demote_rows = [], []
for r in rows:
    rva = r["rva"]
    if rva in passes:
        srcs = [e for e, _ in passes[rva]]
        own = [s for s in srcs if pathlib.Path(s).stem == rva[2:]]
        match_rows.append((rva, "match", (own or srcs)[0], "1", RESULT))
    else:
        why = "; ".join(sorted({d for _, d in refusals.get(rva, [])}))
        reason = f"weak C4 evidence, audit class '{r['class']}' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: {why}"
        fd = ""
        if rva in DCPHYS:
            fd = DCPHYS[rva]
            reason += (f"; frida_diff was the bare word green, now {fd}: hook-live in-process A/B, canonical recipe, "
                       "no write-set leg cited (L-38 / L-40 / L-54)")
        demote_rows.append((rva, "demote", AUDIT, RESULT, reason, fd))
for name, hdr, data in [("C4-NOTAG-87-REEARN-match.tsv", ["rva", "route", "evidence", "reearn", "result"], match_rows),
                        ("C4-NOTAG-87-REEARN.tsv", ["rva", "route", "evidence", "result", "reason", "frida_diff"], demote_rows)]:
    with open(ROOT / "re/promotions" / name, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n", quoting=csv.QUOTE_NONE, escapechar="\\")
        w.writerow(hdr)
        w.writerows(data)
    print(f"{name}: {len(data)} rows")
