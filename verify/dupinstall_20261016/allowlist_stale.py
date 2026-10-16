"""List (and with --apply remove) allowlist lines that no longer correspond to a live lint finding.

    py -3.12 allowlist_stale.py            # print stale lines
    py -3.12 allowlist_stale.py --apply    # drop them from re/tools/dual_copy_allowlist.txt (CRLF-safe)
"""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import lint_rva_bodies as L  # noqa: E402

ALLOW = L.ALLOWLIST
# The 27 DUP-INSTALL RVAs of ROADMAP D4 item (3). Only entries for these RVAs are pruned; a stale entry
# for any other RVA (00417640, 00418860, 0042b8c0, 005aea00 at the time of writing) is somebody else's.
MINE = {int(m.split(":")[1], 16) for m in
        (ln.split("#", 1)[0].strip() for ln in ALLOW.read_text(encoding="utf-8").splitlines())
        if m.startswith("DUP-INSTALL:")}
live = {L.key(c, r, f) for c, r, f, _ in L.findings()}
raw = ALLOW.read_bytes().decode("utf-8")
lines = raw.splitlines(keepends=True)
keep, stale, foreign = [], [], []
for ln in lines:
    body = ln.split("#", 1)[0].strip()
    if body and body not in live:
        if int(body.split(":")[1], 16) in MINE:
            stale.append(body)
        else:
            foreign.append(body)
            keep.append(ln)
    else:
        keep.append(ln)
for f in foreign:
    print("STALE-BUT-NOT-MINE (left)", f)
for s in stale:
    print("STALE", s)
print(f"{len(stale)} stale, {len(live)} live findings, allowlist entries kept = "
      f"{sum(1 for l in keep if l.split('#', 1)[0].strip())}")
new = [k for k in live if k not in {l.split('#', 1)[0].strip() for l in lines}]
print("NEW (not allowlisted):", new)
if "--apply" in sys.argv:
    ALLOW.write_bytes("".join(keep).encode("utf-8"))
    print("applied")
