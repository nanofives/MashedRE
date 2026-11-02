"""Add MATCH-S07 wrapper TUs to the rsp lists (CRLF, quoted, backslashed).
    py -3.12 rspadd.py exe|asi|both <va8> ...
The S07 block sits right after the "Race\\CameraEntries.cpp" line, not at the end of the file: the other MATCH slices
append at the end, and a mid-file block keeps the merges of the slices apart. Entries stay sorted inside the block."""
import sys, pathlib

BS = chr(92)
ANCHOR = f'"Race{BS}CameraEntries.cpp"'
t, vas = sys.argv[1], sys.argv[2:]
for tgt in (["exe", "asi"] if t == "both" else [t]):
    p = pathlib.Path(f"mashedmod/{tgt}_sources.rsp")
    lines = p.read_bytes().decode("utf-8").split("\r\n")
    i = lines.index(ANCHOR) + 1
    j = i
    while j < len(lines) and lines[j].startswith(f'"Match{BS}MatchS07_'):
        j += 1
    block = set(lines[i:j]) | {f'"Match{BS}MatchS07_{v.lower().replace("0x", "")}.cpp"' for v in vas}
    lines[i:j] = sorted(block)
    p.write_bytes("\r\n".join(lines).encode("utf-8"))
    print(f"{p}: {len(block)} S07 entries after {ANCHOR}")
