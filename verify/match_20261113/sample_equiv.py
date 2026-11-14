"""Run equiv.py main() with a Python-level stack sampler. argv: <outfile> <interval_s> <equiv args...>
Each sample: depth of the stack, count of show() frames, and the frames below the first show()/__repr__ frame."""
import runpy, sys, threading, time, traceback
out = open(sys.argv[1], "w", buffering=1)
iv = float(sys.argv[2])
main_id = threading.get_ident()


def sampler():
    n = 0
    while True:
        time.sleep(iv)
        n += 1
        f = sys._current_frames().get(main_id)
        if f is None:
            return
        st = traceback.extract_stack(f)
        shows = sum(1 for fr in st if fr.name in ("show", "__repr__", "<genexpr>"))
        outer = [fr for fr in st if fr.name not in ("show", "__repr__", "<genexpr>")]
        out.write(f"--- sample {n} t={n*iv:.0f}s depth={len(st)} show_frames={shows}\n")
        for fr in outer[-8:]:
            out.write(f"  {fr.name} line {fr.lineno}: {fr.line}\n")


threading.Thread(target=sampler, daemon=True).start()
sys.argv = ["re/tools/match/equiv.py"] + sys.argv[3:]
sys.path.insert(0, "re/tools/match")
t0 = time.time()
try:
    runpy.run_path("re/tools/match/equiv.py", run_name="__main__")
finally:
    out.write(f"=== done after {time.time()-t0:.1f}s\n")
