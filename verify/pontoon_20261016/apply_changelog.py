"""CHANGELOG step of apply_trackers.py, exact-line marker match (memory changelog-marker-exact-match: the header prose quotes the
marker too, so a substring search finds two). Inserts one entry directly below the line that is exactly `<!-- ENTRIES -->`."""
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
cp = WT / "re" / "analysis" / "CHANGELOG.md"
raw = cp.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in raw else "\n"
lines = raw.split(nl)
idx = [i for i, l in enumerate(lines) if l.strip() == "<!-- ENTRIES -->"]
if len(idx) != 1:
    raise SystemExit(f"exact marker lines: {len(idx)}")
if any("pontoon: Warzone track object ported" in l for l in lines):
    raise SystemExit("entry exists")
entry = ("2026-10-16  0x00451d80 + 0x00684d18 object  C-level unchanged  pontoon: Warzone track object ported to the exe "
         "(Track/WarzonePontoon.cpp, knobs MASHED_PONTOON_ANIM/_STATE/_CARRY default OFF; Tower/Deck hide default ON, "
         "MASHED_NO_PONTOON_HIDE), replay vs original capture bit-identical (OP3 1073/1073); U-9719 resolved; U-9738..U-9742 "
         "filed; verify/pontoon_20261016/RESULT_pontoon.md")
lines.insert(idx[0] + 1, entry)
cp.write_bytes(nl.join(lines).encode("utf-8"))
print("CHANGELOG: 1 entry below line", idx[0] + 1)
