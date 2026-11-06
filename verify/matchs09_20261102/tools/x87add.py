"""Insert the MATCH-S09 x87 block after "RwpSolverCore17" in mashedmod/x87_tus.txt (CRLF), mid-file so it does not
collide with the other MATCH slices' end-of-file appends.   py -3.12 x87add.py <va8> ..."""
import pathlib, sys

p = pathlib.Path("mashedmod/x87_tus.txt")
lines = p.read_bytes().decode("ascii").split("\r\n")
HDR = "# MATCH-S09 2026-11-02: port bodies compiled from byte-matched game code that uses float (re/match/s09/)"
if HDR not in lines:
    i = lines.index("RwpSolverCore17") + 1
    lines[i:i] = [HDR]
i = lines.index(HDR) + 1
j = i
while j < len(lines) and lines[j].startswith("MatchS09_"):
    j += 1
lines[i:j] = sorted(set(lines[i:j]) | {f"MatchS09_{v}" for v in sys.argv[1:]})
p.write_bytes("\r\n".join(lines).encode("ascii"))
print(lines[i - 1:i + (j - i) + len(sys.argv)])
