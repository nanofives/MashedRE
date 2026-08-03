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
    ap.add_argument("--modes", default="model,ltm,both")
    args = ap.parse_args()

    os.makedirs(args.out, exist_ok=True)
    sess = frida.attach(args.pid)
    script = sess.create_script(probe.AGENT)
    script.load()
    rpc = script.exports_sync

    for which in args.modes.split(","):
        t0 = time.time()
        while rpc.status()["phase"] != 3 and time.time() - t0 < 90:
            time.sleep(1.0)
        if rpc.status()["phase"] != 3:
            print(f"{which}: SKIP (no race)", flush=True)
            continue
        on_p  = os.path.abspath(os.path.join(args.out, f"root_{which}_on.bmp"))
        off_p = os.path.abspath(os.path.join(args.out, f"root_{which}_off.bmp"))
        c_p   = os.path.abspath(os.path.join(args.out, f"root_{which}_off2.bmp"))
        for attempt in range(8):
            for p in (on_p, off_p, c_p):
                if os.path.exists(p):
                    os.remove(p)
            n = rpc.pairroot(which, args.dy, os.path.abspath(args.req),
                             on_p, off_p, c_p)
            t0 = time.time()
            while time.time() - t0 < 8.0:
                if all(os.path.exists(p) and os.path.getsize(p) > 54
                       for p in (on_p, off_p, c_p)):
                    break
                time.sleep(0.1)
            time.sleep(0.2)
            if not all(os.path.exists(p) for p in (on_p, off_p, c_p)):
                print(f"root_{which}: dump fail (attempt {attempt})", flush=True)
                time.sleep(2)
                continue
            noise = probe.diff_report(off_p, c_p)
            sig   = probe.diff_report(on_p, off_p)
            if "px" in noise and noise["px"] > 3000:
                print(f"root_{which}: noisy (noise={noise['px']}), retry", flush=True)
                time.sleep(3)
                continue
            verdict = ""
            if "px" in sig and "px" in noise:
                verdict = ("RESPONDER" if sig["px"] > 3 * noise["px"] + 500
                           else "quiet")
            print(f"root_{which}: lifted={n} signal={sig} "
                  f"noise={noise.get('px')} {verdict}", flush=True)
            break
        time.sleep(0.5)

    script.unload()
    sess.detach()
    return 0


if __name__ == "__main__":
    sys.exit(main())
