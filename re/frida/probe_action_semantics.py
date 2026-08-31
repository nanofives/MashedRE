r"""probe_action_semantics.py — name the unlabelled contcfg action indices.

Static analysis (re/analysis/structs/contcfg_record.md) placed each action index
0..12 in a lane but did not name actions 2..8. This forces one clean rising edge
per action and records what the frontend state machine does in response.

Mechanism: Interceptor on FUN_00497310 (ReadInputForAction), overriding the
return value to 0xff for exactly one call of one (player, action) pair. This
touches NOTHING on disk and does not write the binding table at DAT_007e95c0 or
the keyboard bitmap -- it only changes what one lookup reports for one frame.
Same technique as canonical_c4_navigate.py.

The frontend consumers (FUN_0042ae10/aeb0/af50/b770) are rising-edge detectors
(cur != 0 && prev == 0), and FUN_00496530 snapshots prev at the top of each
frame, so a single overridden call produces exactly one edge.

Observables: CURSCREEN 0x0067ecb0 and PHASE 0x0067eca4.

Usage:
  py -3.12 re\frida\probe_action_semantics.py                # actions 2..8
  py -3.12 re\frida\probe_action_semantics.py --actions 4,5,6,7
  py -3.12 re\frida\probe_action_semantics.py --settle 22 --dwell 3.0

PID hygiene (CLAUDE.md): this spawns its own MASHED and kills ONLY that pid.
It never enumerates or touches MASHED processes owned by other sessions.
"""
import argparse
import json
import os
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parents[2]
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"
SHOT_DIR = ROOT / "verify" / "action_sem"
REQ_FILE = ROOT / "log" / "action_sem_bbdump.req"

AGENT = r"""
const RVA_RESOLVER = 0x00497310;   // FUN_00497310(slot, action)
const RVA_CURSCREEN = 0x0067ecb0;
const RVA_PHASE     = 0x0067eca4;

let force = {player: -1, action: -1};   // consumed after one successful override
let fired = 0;
let armed = false;

function abs(rva) { return ptr(rva); }   // image base 0x400000, no relocation

rpc.exports = {
  arm: function () {
    if (armed) return 1;
    Interceptor.attach(abs(RVA_RESOLVER), {
      onEnter(a) {
        const sp = this.context.esp;
        this.p = sp.add(4).readS32();
        this.c = sp.add(8).readS32();
      },
      onLeave(ret) {
        if (force.action >= 0 && this.p === force.player && this.c === force.action) {
          ret.replace(ptr(0xff));
          force.action = -1;          // exactly one edge
          fired++;
        }
      }
    });
    armed = true;
    send({kind: 'info', msg: 'armed on FUN_00497310'});
    return 1;
  },
  press: function (player, action) { force = {player: player, action: action}; return 1; },
  pending: function () { return force.action; },
  fired: function () { return fired; },
  // Action-7 gate (0x004409c8..0x004409e8 inside FUN_0043dfd0):
  //   idx = [0x0067e9f8]; if (*(0x0067ed3c + idx*0x40) == 5) then [0x0067ec28] = 1
  // The table is populated at RUNTIME -- the static .data at 0x0067ed38 holds
  // unrelated graphics bytes, so this cannot be answered from the PE.
  gate7: function (n) {
    const out = {idx: -1, field: [], flag: -1};
    try {
      out.idx = ptr(0x0067e9f8).readS32();
      out.flag = ptr(0x0067ec28).readS32();
      for (let i = 0; i < n; i++) {
        out.field.push(ptr(0x0067ed3c + i * 0x40).readS32());
      }
    } catch (e) { out.err = '' + e; }
    return out;
  },
  // Action-8 gate + effect, per U-9052 / re/analysis/structs/contcfg_record.md.
  // Gate  (FUN_0045d0e0): DAT_007f0f10 == 2, FUN_0040e470(player) < 2, rising edge on +0x3f.
  // Gate2 (FUN_0045d1e0): DAT_0088f0c0[player] == 1 AND DAT_008aa2e0[player] == 0.
  // Effect: DAT_008aa2e0[player] = 1 and rec+0xc = 1, where rec = *(DAT_0088f6a0 + player*4).
  a8state: function (player) {
    const o = {};
    try {
      o.gate_7f0f10   = ptr(0x007f0f10).readS32();
      o.f0c0          = ptr(0x0088f0c0 + player * 4).readS32();
      o.aa2e0         = ptr(0x008aa2e0 + player).readU8();
      o.f680          = ptr(0x0088f680 + player * 4).readU32();
      const rec       = ptr(0x0088f6a0 + player * 4).readU32();
      o.rec           = rec;
      o.rec_0c        = rec ? ptr(rec + 0xc).readS32() : -1;
      o.desc_3f       = ptr(0x007f1077 + player * 0x4c).readU8();
    } catch (e) { o.err = '' + e; }
    return o;
  },
  state: function () {
    try {
      return {screen: abs(RVA_CURSCREEN).readS32(), phase: abs(RVA_PHASE).readS32()};
    } catch (e) { return {screen: -999, phase: -999}; }
  }
};
send({kind: 'ready'});
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--actions", default="2,3,4,5,6,7,8")
    ap.add_argument("--settle", type=float, default=25.0,
                    help="seconds to wait for the main menu before probing")
    ap.add_argument("--dwell", type=float, default=3.0,
                    help="seconds to observe after each press")
    ap.add_argument("--out", default=str(ROOT / "log" / "action_semantics.json"))
    ap.add_argument("--attach", type=int, default=0,
                    help="attach to an EXISTING pid instead of spawning. The probe will "
                         "NOT kill it -- ownership stays with whoever launched it "
                         "(CLAUDE.md PID hygiene).")
    ap.add_argument("--a8-action", type=int, default=8,
                    help="action index to force in --a8 mode. Pass 99 for a NEGATIVE CONTROL: "
                         "no call ever matches, so nothing is forced, but the same globals are "
                         "sampled over the same dwell. Race globals move on their own; without "
                         "this control a change is not attributable.")
    ap.add_argument("--a8", type=int, default=-1, metavar="PLAYER",
                    help="probe action 8 for this player: report the gate globals, force "
                         "one edge, and report the effect flags")
    ap.add_argument("--gate7", action="store_true",
                    help="dump the action-7 screen-table gate (0x0067ed3c stride 0x40)")
    ap.add_argument("--shots", action="store_true",
                    help="PrintWindow capture at each step (verify/action_sem/)")
    ap.add_argument("--prefix", default="",
                    help="comma-separated actions to press BEFORE the test press, "
                         "to walk the frontend into a screen where the test is "
                         "discriminating (screen 33 accepts both 4 and 6)")
    args = ap.parse_args()

    actions = [int(x) for x in args.actions.split(",") if x.strip()]
    prefix = [int(x) for x in args.prefix.split(",") if x.strip()]

    def shot(pid, tag):
        """Backbuffer dump via the d3d9 shim's on-demand request protocol.

        NOT PrintWindow: window screenshots are untrustworthy on this machine
        (multi-monitor Present issue documented in d3d9_shim.cpp:213). They come
        back all-white for D3D9 windows. The shim's backbuffer copy is the
        project's truth channel -- it bypasses DWM entirely.

        Protocol: write the target .bmp path into the request file named by
        MASHED_ORIG_BBDUMP_REQ; the shim dumps at the next Present and deletes
        the request. We wait for that deletion as the completion signal.
        """
        if not args.shots:
            return None
        out = SHOT_DIR / f"{tag}.bmp"
        out.parent.mkdir(parents=True, exist_ok=True)
        REQ_FILE.write_text(str(out) + "\n")
        for _ in range(80):                       # ~4 s at 60 fps is many frames
            if not REQ_FILE.exists():
                break
            time.sleep(0.05)
        if REQ_FILE.exists():
            REQ_FILE.unlink(missing_ok=True)
            print(f"  shot FAILED (shim never consumed the request) -> {tag}")
            return None
        print(f"  shot -> {out}")
        return str(out)

    def trace(scr, seconds, step=0.25):
        """Sample the screen trajectory instead of just before/after -- a screen
        that moves and comes back looks identical to no movement at the edges."""
        seq = []
        t_end = time.time() + seconds
        while time.time() < t_end:
            s = scr.exports_sync.state()
            if not seq or seq[-1] != (s["screen"], s["phase"]):
                seq.append((s["screen"], s["phase"]))
            time.sleep(step)
        return seq

    env = dict(os.environ)
    env["MASHED_ORIG_BBDUMP_REQ"] = str(REQ_FILE)
    env["MASHED_WIN_POS"] = "left-bl"      # memory: directional selector, not monitor number
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"    # stock behaviour only, no port hooks

    REQ_FILE.parent.mkdir(parents=True, exist_ok=True)
    REQ_FILE.unlink(missing_ok=True)          # stale request would fire instantly
    dev = frida.get_local_device()
    if args.attach:
        pid = args.attach
        owned = False
        print(f"attaching to EXISTING pid={pid} (NOT spawned by us -- will NOT be killed)")
    else:
        pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
        owned = True
        print(f"spawned MASHED pid={pid}  (this session owns ONLY this pid)")
    results = {"pid": pid, "settle": args.settle, "dwell": args.dwell, "probes": []}
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: (
            print("   ", m.get("payload", {}).get("msg", m))
            if m.get("type") != "error" else print("   JS ERROR:", m)))
        scr.load()
        if owned:
            dev.resume(pid)

        # Arm AFTER settle, not before: FUN_00497310 runs ~13x per active player
        # per frame, which is close to the hot-path threshold in CLAUDE.md
        # ("Frida overhead on hot paths"). Keep the Interceptor live for the
        # probe window only, not across the whole boot.
        print(f"settling {args.settle}s for main menu (no Interceptor yet) ...")
        time.sleep(args.settle)
        scr.exports_sync.arm()

        base = scr.exports_sync.state()
        print(f"baseline: screen={base['screen']} phase={base['phase']}")
        if base["screen"] == -999:
            print("!! could not read CURSCREEN -- aborting before drawing conclusions")
            return 2
        results["baseline"] = base
        shot(pid, f"baseline_s{base['screen']}")

        # negative control first: observe an identical dwell with NO press.
        time.sleep(args.dwell)
        ctrl = scr.exports_sync.state()
        results["control_no_press"] = ctrl
        print(f"control (no press): screen={ctrl['screen']} phase={ctrl['phase']}"
              f"  moved={ctrl != base}")

        for p in prefix:
            scr.exports_sync.press(0, p)
            seq = trace(scr, args.dwell)
            print(f"  prefix {p}: {seq}")
            shot(pid, f"prefix{p}_s{scr.exports_sync.state()['screen']}")
        if prefix:
            results["after_prefix"] = scr.exports_sync.state()
            print(f"  at test screen: {results['after_prefix']}")

        if args.gate7:
            g = scr.exports_sync.gate7(48)
            results["gate7"] = g
            fives = [i for i, v in enumerate(g["field"]) if v == 5]
            print(f"  gate7: screen idx={g['idx']}  flag[0x67ec28]={g['flag']}")
            print(f"  gate7: field==5 at indices {fives}")
            print(f"  gate7: field[idx]={g['field'][g['idx']] if 0 <= g['idx'] < len(g['field']) else 'oob'}")

        if args.a8 >= 0:
            pl = args.a8
            b = scr.exports_sync.a8state(pl)
            print(f"  a8 forcing action {args.a8_action}" + ("  [NEGATIVE CONTROL - nothing forced]" if args.a8_action > 12 else ""))
            print(f"  a8 BEFORE (player {pl}): {b}")
            gate_ok = (b.get("gate_7f0f10") == 2 and b.get("f0c0") == 1
                       and b.get("aa2e0") == 0 and b.get("f680", 0) != 0
                       and b.get("rec", 0) != 0 and b.get("rec_0c") == 0)
            print(f"  a8 gate satisfiable: {gate_ok}"
                  + ("" if gate_ok else "   <-- effect CANNOT fire; a null below proves nothing"))
            f0 = scr.exports_sync.fired()
            scr.exports_sync.press(pl, args.a8_action)
            time.sleep(args.dwell)
            a = scr.exports_sync.a8state(pl)
            print(f"  a8 AFTER  (player {pl}): {a}")
            print(f"  a8 override landed: {scr.exports_sync.fired() > f0}")
            changed = {k: (b.get(k), a.get(k)) for k in a if b.get(k) != a.get(k)}
            print(f"  a8 CHANGED: {changed if changed else 'nothing'}")
            results["a8"] = {"player": pl, "before": b, "after": a,
                             "gate_ok": gate_ok, "changed": changed}

        for act in actions:
            before = scr.exports_sync.state()
            f0 = scr.exports_sync.fired()
            scr.exports_sync.press(0, act)
            seq = trace(scr, args.dwell)
            after = scr.exports_sync.state()
            f1 = scr.exports_sync.fired()
            landed = (f1 > f0)
            rec = {"action": act, "before": before, "after": after, "trace": seq,
                   "override_landed": landed,
                   "screen_moved": before["screen"] != after["screen"],
                   "phase_moved": before["phase"] != after["phase"]}
            rec["shot"] = shot(pid, f"act{act}_s{after['screen']}")
            results["probes"].append(rec)
            print(f"action {act:2d}: {before['screen']}/{before['phase']}"
                  f" -> {after['screen']}/{after['phase']}"
                  f"   landed={landed}   trace={seq}")
    finally:
        if owned:
            try:
                dev.kill(pid)      # ONLY our pid, never by name
                print(f"killed pid={pid}")
            except Exception as e:
                print(f"could not kill pid={pid}: {e}")
        else:
            print(f"leaving pid={pid} running (owned by whoever launched it)")

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(results, indent=2))
    print(f"wrote {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
