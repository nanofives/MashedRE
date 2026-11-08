"""ns.py <file.c> 0xVA SIZE [--show]: score with an explicit original size (row missing from pc_funcs.csv)."""
import pathlib, sys
sys.path.insert(0, "re/tools/match")
import match
src, va, n = pathlib.Path(sys.argv[1]), int(sys.argv[2], 16), int(sys.argv[3])
match.function_table()[va] = n
r = match.compare(src, "", show='--show' in sys.argv, only=va)
for x in r:
    print('bytes_n', x['bytes_n'], 'orig', n, 'bytes_bad', x['bytes_bad'][:10])
