# carpos_root_test.py — paired lift test on the car ROOT RwFrame matrices
# (modelling vs LTM vs both) to determine which matrix the wheels / shadows /
# attached icons actually derive from during render. Companion to
# carpos_source_probe.py (same agent).
import argparse, importlib.util, os, sys, time

import frida

spec = importlib.util.spec_from_file_location(
    "probe", os.path.join(os.path.dirname(__file__), "carpos_source_probe.py"))
probe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(probe)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, required=True)
    ap.add_argument("--req", required=True)
    ap.add_argument("--out", default="verify\\carpos_probe\\root")
    ap.add_argument("--dy", type=float, default=80.0)
    args = ap.parse_args()

    os.makedirs(args.out, exist_ok=True)
    sess = frida.attach(args.pid)
    script = sess.create_script(probe.AGENT)
    script.load()
    rpc = script.exports_sync

    for which in ("model", "ltm", "both"):
        t0 = time.time()
        while rpc.status()["phase"] != 3 and time.time() - t0 < 90:
            time.sleep(1.0)
        if rpc.status()["phase"] != 3:
            print(f"{which}: SKIP (no race)", flush=True)
            continue
        on_p  = os.path.abspath(os.path.join(args.out, f"root_{which}_on.bmp"))
        off_p = os.path.abspath(os.path.join(args.out, f"root_{which}_off.bmp"))
        for p in (on_p, off_p):
            if os.path.exists(p):
                os.remove(p)
        n = rpc.pairroot(which, args.dy, os.path.abspath(args.req), on_p, off_p)
        t0 = time.time()
        while time.time() - t0 < 8.0:
            if (os.path.exists(on_p) and os.path.getsize(on_p) > 54 and
                    os.path.exists(off_p) and os.path.getsize(off_p) > 54):
                break
            time.sleep(0.1)
        time.sleep(0.2)
        ok = os.path.exists(on_p) and os.path.exists(off_p)
        rep = probe.diff_report(on_p, off_p) if ok else {"error": "dump fail"}
        print(f"root_{which}: lifted={n} pair={ok} diff={rep}", flush=True)
        time.sleep(0.5)

    script.unload()
    sess.detach()
    return 0


if __name__ == "__main__":
    sys.exit(main())
