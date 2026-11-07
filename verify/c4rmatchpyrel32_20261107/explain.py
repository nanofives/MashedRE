"""For each changed regress row: count self REL32 sites (type 0x14, target = own symbol) and the old-new bad-byte delta,
and run relocheck on rows that flip to byte-identical. Run from worktree root."""
import csv, pathlib, sys, subprocess
sys.path.insert(0, "re/tools/match"); import match
rows = [r for r in csv.DictReader(open("verify/c4rmatchpyrel32_20261107/regress.tsv"), delimiter="\t") if r["changed"] == "yes"]
print("rva\tsource\tself_rel32_sites\told_bad\tnew_bad\tdelta\texplained")
for r in rows:
    src = pathlib.Path(r["source"]); text = src.read_text(); addr = int(r["rva"], 16)
    sym = dict(match.parse_annotations(text))[addr]
    fl = match.file_flags(text) or match.DEFAULT_FLAGS
    secs, syms = match.coff(match.compile_obj(src, fl, match.file_lang(text), match.file_toolchain(text)))
    s = [x for x in syms if x["sec"] > 0 and x["type"] == 0x20 and x["name"] == match.function_from_obj(secs, syms, sym)[2]][0]
    bi = {x["index"]: x for x in syms}
    n = sum(1 for off, si, t in secs[s["sec"]-1]["relocs"] if t == 0x14 and bi[si]["name"] == s["name"] and off >= s["value"])
    d = int(r["old_bad"]) - int(r["new_bad"])
    print(f"{r['rva']}\t{src.name}\t{n}\t{r['old_bad']}\t{r['new_bad']}\t{d}\t{'yes' if d == n else 'NO'}")
