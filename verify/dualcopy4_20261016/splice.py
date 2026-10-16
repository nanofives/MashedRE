"""splice.py <file> <first-line> <last-line> <replacement-file> : replace 1-indexed lines [first,last] of <file> with the
contents of <replacement-file> (bytes, original newline style kept). Prints the lines it removed (first/last) as a guard."""
import pathlib, sys

p = pathlib.Path(sys.argv[1])
a, b = int(sys.argv[2]), int(sys.argv[3])
rep = pathlib.Path(sys.argv[4]).read_bytes().replace(b"\r\n", b"\n")
raw = p.read_bytes()
nl = b"\r\n" if b"\r\n" in raw else b"\n"
ls = raw.split(nl)
print("first removed:", ls[a - 1][:100])
print("last  removed:", ls[b - 1][:100])
new = rep.rstrip(b"\n").split(b"\n")
ls[a - 1:b] = new
p.write_bytes(nl.join(ls))
print("replaced", b - a + 1, "lines with", len(new))
