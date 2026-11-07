"""Full regression of match.py byte_compare OLD (HEAD copy match_old.py) vs NEW (working tree) over every re/match source.
Each source compiled once; both rules scored on the same obj. Run from the worktree root:
    py -3.12 verify/c4rmatchpyrel32_20261107/regress.py > verify/c4rmatchpyrel32_20261107/regress.tsv"""
import importlib.util, pathlib, sys, multiprocessing as mp
ROOT = pathlib.Path.cwd()
D = ROOT / "verify/c4rmatchpyrel32_20261107"
sys.path.insert(0, str(ROOT / "re/tools/match"))
import match as new
spec = importlib.util.spec_from_file_location("match_old", D / "match_old.py"); old = importlib.util.module_from_spec(spec)
spec.loader.exec_module(old)
for k in ('ROOT','MAIN','EXE','FUNCS','TOOLCHAINS'):
    setattr(old, k, getattr(new, k))  # old copy lives under verify/, so its parents[3] paths are wrong

def pct(n, bad): return f"{100*(n-len(bad))/n:.2f}" if n else "0"

def work(src):
    out = []
    try:
        text = src.read_text()
        anns = new.parse_annotations(text)
        if not anns: return out
        fl = new.file_flags(text) or new.DEFAULT_FLAGS
        secs, syms = new.coff(new.compile_obj(src, fl, new.file_lang(text), new.file_toolchain(text)))
        for addr, sym in anns:
            try:
                acc = new.score_function(addr, secs, syms, sym)[0]
                n0, b0 = old.byte_compare(addr, secs, syms, sym)
                n1, b1 = new.byte_compare(addr, secs, syms, sym)
                out.append((f"0x{addr:08x}", src.as_posix(), f"{100*acc:.1f}", n0, pct(n0, b0), len(b0), n1, pct(n1, b1), len(b1),
                            "yes" if (b0 != b1) else "no"))
            except BaseException as e:
                out.append((f"0x{addr:08x}", src.as_posix(), "ERR", 0, "", "", 0, "", "", str(e).splitlines()[0][:60] if str(e) else type(e).__name__))
    except BaseException as e:
        out.append(("-", src.as_posix(), "COMPILE", 0, "", "", 0, "", "", (str(e).splitlines() or [type(e).__name__])[0][:60]))
    return out

if __name__ == "__main__":
    srcs = sorted(p for p in (ROOT / "re/match").rglob("*") if p.suffix in (".c", ".cpp"))
    print("rva\tsource\tinsn%\tbytes_n\told%\told_bad\tbytes_n_new\tnew%\tnew_bad\tchanged")
    with mp.Pool(8) as pool:
        for i, res in enumerate(pool.imap(work, srcs, chunksize=4)):
            for r in res: print("\t".join(map(str, r)), flush=True)
            if i % 200 == 0: print(f"[{i}/{len(srcs)}] progress", file=sys.stderr, flush=True)
