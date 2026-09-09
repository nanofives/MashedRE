#!/usr/bin/env python3
r"""Diff a reimplementation against TTD-captured ORIGINAL behavior (the in->out CSV).

extract_calls.py captures the original's (input_bits -> output_bits) from a real
canonical run. This harness feeds each captured input to a reimpl backend and compares
its output to the captured original output BIT-EXACTLY. That's the canonical-scenario
reimpl diff -- sourced from real captured inputs instead of synthetic vectors.

Backends
--------
  sqrt / invsqrt / identity   pure-Python stand-ins. `sqrt` models the STANDALONE's
                              no-RW-LUT path (std::sqrt fallback, see WS-PHYS-CRASH-FIX),
                              so diffing it against the captured original RW fast-sqrt
                              LUT output quantifies the standalone's residual per call,
                              to the bit/ULP. Fully offline.

  asi:<Export>                THE REAL REIMPL DIFF (added 2026-09-09). Spawns MASHED,
                              lets the dinput8 proxy auto-load mashed_re_dev.asi, waits
                              for the RW engine to come up, and calls the .asi export
                              once per captured input through Frida. This is the only
                              backend that exercises the SHIPPED code with the live RW
                              LUT, which is the whole point for LUT-dependent leaves.
                              NOT offline: it needs a game boot (one boot for the whole
                              CSV, not one per call).

What a clean result IS and IS NOT
---------------------------------
All-bit-identical over N captured inputs is strong canonical-input evidence: the inputs
are real, not synthetic, so the domain is the one the game actually produces. It is still
NOT an automatic C4 -- promotion goes through re-classify, and this harness calls the
export directly rather than proving the inline-JMP is installed (path1, not path2; see
memory `path1-green-does-not-prove-install`).

Non-degeneracy is checked, not assumed: if the captured inputs or the produced outputs
collapse to a single distinct value, the comparison proves nothing and the tool says so.

Usage:
    py -3.12 scripts/ttd/ttd_reimpl_diff.py log/ttd/calls_004c3b30.csv --reimpl sqrt
    py -3.12 scripts/ttd/ttd_reimpl_diff.py log/ttd/calls_004c3b30.csv --reimpl asi:FastSqrt
"""
import argparse, csv, math, os, struct, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"
ASI = ORIG / "mashed_re_dev.asi"


def to_f32(v):
    try: return struct.unpack("<f", struct.pack("<f", v))[0]
    except OverflowError: return float("inf") if v > 0 else float("-inf")

def f32_bits(v):
    try: b = struct.pack("<f", v)
    except OverflowError: b = struct.pack("<f", float("inf") if v > 0 else float("-inf"))
    return struct.unpack("<I", b)[0]

BACKENDS = {
    "sqrt":     lambda x: math.sqrt(x) if x >= 0 else 0.0,          # standalone std::sqrt path
    "invsqrt":  lambda x: 1.0 / math.sqrt(x) if x > 0 else 0.0,
    "identity": lambda x: x,
}

def ulp_distance(a, b):
    """Signed-magnitude float32 bits -> monotonic key; distance = representable steps apart."""
    key = lambda u: u if not (u & 0x80000000) else (0x80000000 - (u & 0x7fffffff))
    return abs(key(a) - key(b))


# ---------------------------------------------------------------------------
# asi:<Export> backend
# ---------------------------------------------------------------------------
# Constraints honoured here, each one a scar from a previous session:
#   * do NOT Module.load the .asi -- the dinput8 proxy already auto-loads it, and a
#     double load corrupts state (memory `no-explicit-module-load-asi`). Load is a
#     LAST resort, reported when it happens.
#   * launch muted (memory `always-launch-muted`): MASHED_MUTE=1 skips DirectSound.
#   * kill ONLY the pid we spawned, never by image name -- other sessions run their
#     own MASHED (memory `multisession-mashed-kill-by-pid`).
#   * keep the scratch buffer in a module-scope var: Frida reclaims Memory.alloc
#     buffers referenced only by a raw pointer (memory `frida-keepalive-scratch-buffers`).
ASI_AGENT = r"""
var ASI = null, FN = null, SCRATCH = null, LOADED = false;
var PHASE = ptr('0x00771968');          // session phase: 1=menu 2=load 3=race

rpc.exports = {
  init: function (exportName, asiPath) {
    SCRATCH = Memory.alloc(8);          // module-scope: must outlive the call
    ASI = Process.findModuleByName('mashed_re_dev.asi');
    if (ASI === null) {
      try { ASI = Module.load(asiPath); LOADED = true; }
      catch (e) { return 'no-asi:' + e.message; }
    }
    var a = ASI.findExportByName(exportName);
    if (a === null) return 'no-export';
    FN = new NativeFunction(a, 'float', ['float']);   // __cdecl float(float), st0 return
    return (LOADED ? 'ok-loaded@' : 'ok-auto@') + a;
  },
  phase: function () { try { return PHASE.readU8(); } catch (e) { return -1; } },
  modules: function () {
    return Process.enumerateModules().map(function (m) { return m.name; }).join(',');
  },
  callmany: function (bitsList) {
    var out = [];
    for (var i = 0; i < bitsList.length; i++) {
      SCRATCH.writeU32(bitsList[i] >>> 0);
      var r;
      try { r = FN(SCRATCH.readFloat()); }
      catch (e) { out.push(null); continue; }
      SCRATCH.writeFloat(r);
      out.push(SCRATCH.readU32() >>> 0);
    }
    return out;
  }
};
"""


def asi_outputs(in_bits, export, wait_phase=1, timeout=90.0, verbose=True):
    """Spawn MASHED, call the .asi export once per captured input, return out_bits.

    Returns (out_bits list, meta dict). Any element may be None (call raised).
    """
    try:
        import frida
    except ImportError:
        sys.exit("frida not installed -- the asi: backend needs it (py -3.12 -m pip install frida)")
    if not EXE.exists():
        sys.exit(f"missing {EXE}")
    if not ASI.exists():
        sys.exit(f"missing {ASI} -- run mashedmod\\build.bat (it deploys the .asi to original\\)")

    env = dict(os.environ)
    env["MASHED_MUTE"] = "1"                      # never wake the speakers on a harness run
    env.pop("MASHED_RE_NO_AUTO_HOOK", None)       # we WANT the dinput8 proxy to load the .asi

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    meta = {"pid": pid, "export": export}
    if verbose:
        print(f"  [asi] spawned pid={pid} (muted); waiting for the RW engine")
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(ASI_AGENT)
        scr.on("message", lambda m, d: None)
        scr.load()
        dev.resume(pid)

        # The LUT-backed leaves need RwEngineOpen to have run. Poll the session-phase
        # global rather than sleeping a fixed amount, and RECORD the phase we called at
        # so a divergent result can be told apart from "we called too early".
        t0 = time.time()
        phase = -1
        while time.time() - t0 < timeout:
            try:
                phase = scr.exports_sync.phase()
            except Exception:
                phase = -1
            if phase >= wait_phase:
                break
            time.sleep(0.5)
        meta["phase"] = phase
        meta["waited_s"] = round(time.time() - t0, 1)
        if phase < wait_phase:
            print(f"  [asi] WARNING: phase={phase} after {meta['waited_s']}s "
                  f"(wanted >={wait_phase}). Calling anyway; a LUT-dependent leaf may "
                  f"take its CPU fallback and diverge for that reason, not a port defect.")

        status = scr.exports_sync.init(export, str(ASI))
        meta["init"] = status
        if not str(status).startswith("ok"):
            mods = ""
            try:
                mods = scr.exports_sync.modules()
            except Exception:
                pass
            sys.exit(f"  [asi] init failed: {status}\n  modules: {mods[:400]}")
        if verbose:
            print(f"  [asi] {status}  phase={phase} after {meta['waited_s']}s")

        # chunk so one bad call cannot lose the whole batch, and so a wedge shows up
        out = []
        CH = 256
        for i in range(0, len(in_bits), CH):
            out.extend(scr.exports_sync.callmany(in_bits[i:i + CH]))
        return out, meta
    finally:
        try:
            dev.kill(pid)          # ONLY our pid -- never by image name
        except Exception:
            pass


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv")
    ap.add_argument("--reimpl", default="sqrt",
                    help="sqrt | invsqrt | identity | asi:<ExportName>")
    ap.add_argument("--wait-phase", type=int, default=1,
                    help="asi: backend -- session phase to reach before calling (1=menu)")
    ap.add_argument("--timeout", type=float, default=90.0)
    ap.add_argument("--limit", type=int, default=0, help="use only the first N captured calls")
    a = ap.parse_args()

    rows = list(csv.DictReader(open(a.csv)))
    if not rows or "in_f32" not in rows[0]:
        sys.exit("CSV has no in_f32/out_bits columns (scalar extract_calls.py output expected)")
    if a.limit:
        rows = rows[:a.limit]

    is_asi = a.reimpl.startswith("asi:")
    if not is_asi and a.reimpl not in BACKENDS:
        sys.exit(f"unknown backend {a.reimpl!r} (choose {', '.join(BACKENDS)} or asi:<Export>)")

    meta = {}
    if is_asi:
        export = a.reimpl.split(":", 1)[1]
        if not export:
            sys.exit("asi: backend needs an export name, e.g. --reimpl asi:FastSqrt")
        in_bits = [int(r["in_bits"], 16) if str(r["in_bits"]).startswith("0x")
                   else int(r["in_bits"]) for r in rows]
        re_bits_all, meta = asi_outputs(in_bits, export, a.wait_phase, a.timeout)
    else:
        fn = BACKENDS[a.reimpl]
        re_bits_all = [f32_bits(to_f32(fn(float(r["in_f32"])))) for r in rows]

    exact, divs, first, errs = 0, [], None, 0
    for r, re_bits in zip(rows, re_bits_all):
        orig_bits = int(r["out_bits"], 16)
        if re_bits is None:
            errs += 1
            continue
        if re_bits == orig_bits:
            exact += 1
        else:
            u = ulp_distance(re_bits, orig_bits)
            divs.append(u)
            if first is None:
                first = (r["i"], r["in_f32"], r["out_bits"], f"0x{re_bits:08x}", u)

    n = len(rows)
    print(f"reimpl='{a.reimpl}'  vs  TTD-captured original  ({a.csv})")
    if meta:
        print(f"  backend: pid={meta.get('pid')} {meta.get('init')} "
              f"phase={meta.get('phase')} waited={meta.get('waited_s')}s")
    print(f"  {exact}/{n} bit-identical" + (f"   ({errs} call errors)" if errs else ""))

    # Non-degeneracy: a comparison over one distinct input, or one distinct output,
    # distinguishes nothing. Say so rather than reporting a vacuous 100%.
    din = len({r["in_bits"] for r in rows})
    dout = len({b for b in re_bits_all if b is not None})
    dorig = len({r["out_bits"] for r in rows})
    print(f"  domain: {din} distinct inputs, {dorig} distinct captured outputs, "
          f"{dout} distinct reimpl outputs")
    if din < 2 or dorig < 2:
        print("  [DEGENERATE] the captured domain has <2 distinct values -- this comparison")
        print("               proves nothing. Re-capture with a wider input domain.")
    elif dout < 2:
        print("  [DEGENERATE] the reimpl returned a single value for every input -- the")
        print("               harness is probably broken (wrong export/ABI), not the port.")

    if divs:
        print(f"  divergent: {len(divs)}   max {max(divs)} ULP   mean {sum(divs)/len(divs):.1f} ULP")
        print(f"  first diff: call[{first[0]}]  in={first[1]}  orig={first[2]}  reimpl={first[3]}  ({first[4]} ULP)")
        if is_asi:
            print("  -> the .asi export does NOT reproduce the original on captured inputs.")
            if meta.get("phase", -1) < a.wait_phase:
                print("     Check the phase warning above BEFORE reading this as a port defect.")
        else:
            print("  -> reimpl does NOT reproduce the original bit-exactly (expected for std::sqrt vs the RW LUT)")
    elif exact == n and n:
        print("  ALL bit-identical -> reimpl reproduces the original on every captured input")
        print("     Real captured inputs, so the domain is the one the game produces.")
        print("     Still path1 (the export is called directly): this does NOT prove the")
        print("     inline-JMP is installed. Promote via re-classify, never auto-C4.")
    # exit 0 always: this is a measurement tool, not a gate

if __name__ == "__main__":
    main()
