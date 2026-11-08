"""Score a MATCH row at an explicit original size, and each `// HIDDEN: 0x<va> <sym> <size>` function in the same
file, with match.py's own score_function / byte_compare and relocheck.py's check (C4R-NEARMISS, 2026-11-07).

Why: match.py sizes the original from re/console/pc_funcs.csv body_size. For 0x0040ac80 that is 228 B, Ghidra's
address set with the four tail-jumped case handlers folded in, so the scorer reads 228 contiguous bytes and runs
past the jump table into FUN_0040acd0. body_end in the same row is 0x0040acb5 (code = 0x35 B).

    py -3.12 verify/c4rnearmiss_20261107/hidden_check.py <src.c> 0x<match_va>=<size> [...]
"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match  # noqa: E402
import relocheck  # noqa: E402

RE_H = re.compile(r"//\s*HIDDEN:\s*(0x[0-9a-fA-F]+)\s+(\S+)\s+(\d+)")


def main():
    src = pathlib.Path(sys.argv[1])
    text = src.read_text()
    sizes = {int(k, 16): int(v, 0) for k, v in (a.split("=") for a in sys.argv[2:])}
    hidden = [(int(a, 16), s, int(n)) for a, s, n in RE_H.findall(text)]
    for a, s, n in hidden:
        sizes[a] = n
    fl = match.file_flags(text) or match.DEFAULT_FLAGS
    secs, syms = match.coff(match.compile_obj(src, fl, match.file_lang(text), match.file_toolchain(text)))
    rows = [(a, s) for a, s in match.parse_annotations(text) if a in sizes] + [(a, s) for a, s, _ in hidden]
    for a, s in rows:
        acc, regs, ops, x, y, full, sm = match.score_function(a, secs, syms, s, sizes[a])
        n, bad = match.byte_compare(a, secs, syms, s)
        print(f"0x{a:08x}\t{full}\tsize={sizes[a]}\tinsn={100 * acc:.1f}% ({len(x)}/{len(y)})\tbytes={n}\tbad={len(bad)}\tflags={fl}")
        if acc < 1:
            match.print_diff(x, y, sm)
    # relocheck over MATCH + HIDDEN rows: same check() with the HIDDEN lines read as annotations
    orig_pa = match.parse_annotations
    match.parse_annotations = lambda t: orig_pa(t) + [(a, s) for a, s, _ in hidden]
    try:
        for line in relocheck.check(src):
            print("relocheck\t" + "\t".join(str(c) for c in line) if isinstance(line, (list, tuple)) else "relocheck\t" + str(line))
    finally:
        match.parse_annotations = orig_pa


if __name__ == "__main__":
    main()
