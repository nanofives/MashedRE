"""Insert the MATCH-S14 x87 block after "ViewportScaledRectDraw" in mashedmod/x87_tus.txt (CRLF), mid-file so it does not
collide with the other MATCH slices' end-of-file appends.   py -3.12 x87add.py <va8> ..."""
import pathlib, sys

p = pathlib.Path("mashedmod/x87_tus.txt")
lines = p.read_bytes().decode("ascii").split("\r\n")
HDR = "# MATCH-S14 2026-11-02: port bodies compiled from byte-matched game code that uses float (re/match/s14/)"
if HDR not in lines:
    i = lines.index("ViewportScaledRectDraw") + 1
    lines[i:i] = [HDR]
i = lines.index(HDR) + 1
j = i
while j < len(lines) and lines[j].startswith("MatchS14_"):
    j += 1
lines[i:j] = sorted(set(lines[i:j]) | {f"MatchS14_{v}" for v in sys.argv[1:]})
p.write_bytes("\r\n".join(lines).encode("ascii"))
print(lines[i - 1:i + (j - i) + len(sys.argv)])
