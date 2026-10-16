"""Remove allowlist lines for the given RVAs, but ONLY lines the lint no longer reports (stale).

    py -3.12 allowlist_prune.py 0046cbb0 00417640 00418860 0042b8c0 005aea00          # dry run
    py -3.12 allowlist_prune.py --apply 0046cbb0 ...
"""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import lint_rva_bodies as L  # noqa: E402

args = [a for a in sys.argv[1:] if not a.startswith("--")]
want = {int(a, 16) for a in args}
live = {L.key(c, r, f) for c, r, f, _ in L.findings()}
raw = L.ALLOWLIST.read_bytes().decode("utf-8")
lines = raw.splitlines(keepends=True)
keep, gone, still = [], [], []
for ln in lines:
    body = ln.split("#", 1)[0].strip()
    if body and int(body.split(":")[1], 16) in want:
        if body in live:
            still.append(body)
            keep.append(ln)
        else:
            gone.append(body)
    else:
        keep.append(ln)
for g in gone:
    print("STALE (remove)", g)
for s in still:
    print("STILL LIVE (kept)", s)
print(f"{len(gone)} stale removed, {len(still)} live kept")
if "--apply" in sys.argv:
    L.ALLOWLIST.write_bytes("".join(keep).encode("utf-8"))
    print("applied")
