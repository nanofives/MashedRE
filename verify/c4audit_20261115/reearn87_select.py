"""C4-NOTAG-87-REEARN: pick the 87 weak untagged C4 rows from c4_notag_audit.tsv and find their re/match sources.

Writes verify/c4audit_20261115/reearn87_rows.tsv (rva, class, in_admitted, in_provisional, sources, frida_diff, confidence).
Read-only on the trackers.
"""
import csv, pathlib, re

ROOT = pathlib.Path(__file__).resolve().parents[2]
AUD = ROOT / "verify/c4audit_20261115/c4_notag_audit.tsv"
OUT = ROOT / "verify/c4audit_20261115/reearn87_rows.tsv"
WEAK = {"install manifest plus survival", "diff with the hook bypassed", "file missing", "other"}


def norm(s):
    s = s.strip().lower()
    s = s[2:] if s.startswith("0x") else s
    return f"{int(s, 16):08x}"


def rvas_in(path):
    out = set()
    if not path.is_file():
        return out
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        for m in re.finditer(r"0x([0-9a-fA-F]{6,8})\b", line.split("\t")[0] if "\t" in line else line):
            out.add(norm(m.group(1)))
    return out


aud = list(csv.DictReader(open(AUD, encoding="utf-8", newline=""), delimiter="\t"))
rows = [r for r in aud if r["class"] in WEAK]
adm = rvas_in(ROOT / "re/match/ADMITTED.tsv")
prov = rvas_in(ROOT / "re/match/PROVISIONAL.tsv")
srcmap = {}
for p in (ROOT / "re/match").rglob("*"):
    if p.suffix.lower() not in (".c", ".cpp"):
        continue
    t = p.read_text(encoding="utf-8", errors="replace")
    for m in re.finditer(r"//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x([0-9a-fA-F]+)", t):
        srcmap.setdefault(norm(m.group(1)), []).append(p.relative_to(ROOT).as_posix())
hooks = {r["rva"].lower().replace("0x", "").zfill(8): r
         for r in csv.DictReader(open(ROOT / "hooks.csv", encoding="utf-8", newline=""))}
with open(OUT, "w", encoding="utf-8", newline="") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "class", "in_admitted", "in_provisional", "sources", "frida_diff", "confidence", "name"])
    for r in rows:
        k = norm(r["rva"])
        h = hooks.get(k, {})
        w.writerow([f"0x{k}", r["class"], int(k in adm), int(k in prov), ",".join(sorted(set(srcmap.get(k, [])))),
                    (h.get("frida_diff") or "")[:80], h.get("confidence", ""), h.get("name", "")])
print(len(rows), "weak rows;", sum(norm(r["rva"]) in adm for r in rows), "admitted;",
      sum(norm(r["rva"]) in prov for r in rows), "provisional;",
      sum(bool(srcmap.get(norm(r["rva"]))) for r in rows), "with a re/match source")
