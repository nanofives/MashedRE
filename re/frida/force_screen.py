"""Force the frontend to push a specific screen id, then capture it.

U-9050 needs the identity of frontend screen id 5: the action-7 gate at
0x004409df fires only when the top menu-stack entry's type equals 5, and
`entry.type` IS the screen id (FUN_0043d2a0 mode 0 stores param_1 verbatim at
0x0043d463).  Navigation cannot find it -- no literal push of 5 exists in either
menu tick, and the message bank that would name it is an external asset with no
reader (see re/analysis/structs/contcfg_record.md).

So instead of hunting for the screen, we push it:  call

    FUN_0043d2a0(screen_id, 0)      // cdecl, mode 0 = push

from inside a hook on the menu tick, i.e. on the GAME's thread at a frame
boundary, not from a Frida thread.  Then read back the stack and dump the
backbuffer.

This is an INTERVENTION, not an observation of natural play.  What it can
establish is "screen id N renders as <image>", which is exactly what U-9050
needs.  It does NOT establish that the screen is reachable in normal play, and a
capture here is not evidence about how the player gets there.

PID hygiene (CLAUDE.md): spawns its own MASHED and kills ONLY that pid, never by
name.  Capture goes through the d3d9 shim's MASHED_ORIG_BBDUMP_REQ protocol --
capture_window.ps1 returns all-white on this machine (d3d9_shim.cpp:213).
"""

import argparse
import json
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parents[2]
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"
SHOT_DIR = ROOT / "verify" / "screen_id"
REQ_FILE = ROOT / "log" / "force_screen_bbdump.req"

AGENT = r"""
const RVA_RESOLVER  = 0x00497310;   // FUN_00497310(slot, action) -- input override
const RVA_MENUTICK  = 0x0043dfd0;   // FUN_0043dfd0, the menu-phase tick
const RVA_PUSH      = 0x0043d2a0;   // FUN_0043d2a0(screen_id, mode)
const RVA_DEPTH     = 0x0067e9f8;   // menu stack depth
const RVA_ENTRY0    = 0x0067ed7c;   // entry[k] base, stride 0x40; +0x00 = type
const RVA_CURSCREEN = 0x0067ecb0;
const RVA_PHASE     = 0x0067eca4;
const RVA_GATEFLAG  = 0x0067ec28;   // action-7 effect flag

const pushFn = new NativeFunction(ptr(RVA_PUSH), 'void', ['int', 'int'], 'mscdecl');

let force   = {player: -1, action: -1};
let held    = {player: -1, action: -1};
let fired   = 0;
let armed   = false;
let tickHook = null;
let want    = -1;          // screen id to push, -1 = idle
let pushed  = 0;
let lastErr = null;

function readStack(n) {
  const o = {depth: -1, types: [], curscreen: -1, phase: -1, flag: -1, gate: -1};
  try {
    o.depth     = ptr(RVA_DEPTH).readS32();
    o.curscreen = ptr(RVA_CURSCREEN).readS32();
    o.phase     = ptr(RVA_PHASE).readS32();
    o.flag      = ptr(RVA_GATEFLAG).readS32();
    const lim = (n > 0) ? n : Math.max(0, Math.min(o.depth + 2, 16));
    for (let i = 0; i < lim; i++) o.types.push(ptr(RVA_ENTRY0 + i * 0x40).readS32());
    // the action-7 gate expression, verbatim: *(0x0067ed3c + depth*0x40)
    o.gate = ptr(0x0067ed3c + o.depth * 0x40).readS32();
    // FUN_0042c960 (the sole consumer of the action-7 flag) runs only when this == 1
    o.e7c8 = ptr(0x0067e7c8).readS32();
    o.ea08 = ptr(0x0067ea08).readS32();   // page index advanced by FUN_0042c510
    o.ebc8 = ptr(0x0067ebc8).readS32();   // queue write index rebuilt by FUN_0042c960
    o.eab0 = ptr(0x0067eab0).readS32();   // FUN_0042af50 entry guard term
    o.a8ab0 = ptr(0x00898ab0).readS32();  // FUN_0042af50 entry guard term
    o.sphase = ptr(0x00771968).readU8();   // session phase: 1=menu 2=load+spawn 3=race
    o.f0f10 = ptr(0x007f0f10).readS32();  // g_itemSelectorP3 (frontend_state.md:145) -- NOT a race flag
    o.d6 = ptr(0x007f1041).readU8();      // live descriptor +0x09 (action 6)
    o.s6 = ptr(0x007f1501).readU8();      // snapshot +0x09
  } catch (e) { o.err = '' + e; }
  return o;
}

rpc.exports = {
  arm: function () {
    if (armed) return 1;
    Interceptor.attach(ptr(RVA_RESOLVER), {
      onEnter(a) {
        const sp = this.context.esp;
        this.p = sp.add(4).readS32();
        this.c = sp.add(8).readS32();
      },
      onLeave(ret) {
        if (force.action >= 0 && this.p === force.player && this.c === force.action) {
          ret.replace(ptr(0xff));
          force.action = -1;
          fired++;
        } else if (held.action >= 0 && this.p === held.player && this.c === held.action) {
          ret.replace(ptr(0xff));
        }
      }
    });
    armed = true;
    return 1;
  },
  press: function (player, action) { force = {player: player, action: action}; return 1; },
  hold:  function (player, action) { held  = {player: player, action: action}; return 1; },
  release: function () { held = {player: -1, action: -1}; return 1; },
  fired: function () { return fired; },

  // Arm a one-shot push executed on the game thread at the top of the menu tick.
  armpush: function () {
    if (tickHook) return 1;
    tickHook = Interceptor.attach(ptr(RVA_MENUTICK), {
      onEnter() {
        if (want < 0) return;
        const id = want; want = -1;
        try { pushFn(id, 0); pushed++; }
        catch (e) { lastErr = '' + e; }
      }
    });
    return 1;
  },
  pushscreen: function (id) { want = id; return 1; },
  pushed: function () { return pushed; },
  lasterr: function () { return lastErr; },
  detach: function () { Interceptor.detachAll(); tickHook = null; armed = false; return 1; },
  pokeflag: function () { ptr(0x0067ec28).writeS32(1); return ptr(0x0067ec28).readS32(); },
  page: function () { try { return ptr(0x0067ea08).readS32(); } catch (e) { return -999; } },
  stack: function (n) { return readStack(n); }
};
send({kind: 'ready'});
"""


def shot(name):
    """Ask the d3d9 shim to dump the backbuffer at the next Present."""
    SHOT_DIR.mkdir(parents=True, exist_ok=True)
    dst = SHOT_DIR / name
    if dst.exists():
        dst.unlink()
    REQ_FILE.parent.mkdir(parents=True, exist_ok=True)
    REQ_FILE.write_text(str(dst), encoding="ascii")
    for _ in range(60):                      # up to ~6 s
        time.sleep(0.1)
        if dst.exists() and not REQ_FILE.exists():
            return str(dst)
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--screens", default="5",
                    help="comma-separated screen ids to push and capture")
    ap.add_argument("--settle", type=float, default=25.0)
    ap.add_argument("--prefix", default="4,4",
                    help="actions pressed before pushing, to leave the title screen")
    ap.add_argument("--dwell", type=float, default=3.0,
                    help="seconds between the push and the capture")
    ap.add_argument("--out", default=str(ROOT / "log" / "force_screen.json"))
    ap.add_argument("--after", default="",
                    help="actions pressed AFTER the push, to clear modals")
    ap.add_argument("--attach", type=int, default=0,
                    help="attach to an EXISTING pid (e.g. a scenario_launch race). "
                         "The probe will NOT kill it -- ownership stays with the launcher.")
    ap.add_argument("--watch", type=float, default=0.0,
                    help="seconds to sample state before AND after the fire (control window)")
    ap.add_argument("--to-race", type=int, default=0, metavar="MAXPRESS",
                    help="drive into a race through the FRONTEND with action-4 confirms, "
                         "stop pressing on DAT_007f0f10==2, then run the control/fire windows")
    ap.add_argument("--poke-flag", action="store_true",
                    help="after the push, write DAT_0067ec28=1 directly. Tests the "
                         "CONSUMER half of action 7 (flag -> FUN_0042c960 -> "
                         "FUN_0042c510 -> page advance) without needing the producer "
                         "gate [ESP+0x34], which a forced push does not set up.")
    ap.add_argument("--race-seq", default="",
                    help="actions fired IN ORDER once in-race, before --await-gate; "
                         "e.g. 6,12,12,12,4 = pause, down x3, select (= Quit Race)")
    ap.add_argument("--seq-delay", type=float, default=1.4)
    ap.add_argument("--hold", type=int, default=-1, metavar="ACTION",
                    help="hold this action continuously during the await window "
                         "(0 = accelerate) so the race actually progresses")
    ap.add_argument("--await-gate", type=int, default=-1, metavar="TYPE",
                    help="after --to-race, wait until the menu-stack gate reads TYPE "
                         "(e.g. 5 = Race Results reached NATURALLY), then fire")
    ap.add_argument("--await-secs", type=float, default=180.0,
                    help="how long to wait for --await-gate")
    ap.add_argument("--speed", default="",
                    help="MASHED_SPEED for the run (needs the qol asi); speeds the race up")
    ap.add_argument("--fire", type=int, default=-1,
                    help="action fired on the pushed screen; shots taken before and after")
    args = ap.parse_args()

    ids = [int(x) for x in args.screens.split(",") if x.strip() != ""]

    env = dict(**__import__("os").environ)
    env["MASHED_ORIG_BBDUMP_REQ"] = str(REQ_FILE)
    env["MASHED_WIN_POS"] = "left-bl"
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if args.speed:
        # MASHED_SPEED only works with the qol asi AND with the dev hooks off --
        # mashed_re_dev.asi replaces the quantizer at 0x00493480 (FrameDispatch.cpp)
        # and would silently bypass it. MASHED_RE_NO_AUTO_HOOK=1 is already set above.
        env["MASHED_QOL"] = "1"
        env["MASHED_DECOUPLE"] = "1"
        env["MASHED_SPEED"] = args.speed

    if REQ_FILE.exists():
        REQ_FILE.unlink()

    dev = frida.get_local_device()
    owned = not args.attach
    if args.attach:
        pid = args.attach
        print(f"attaching to EXISTING pid={pid} (NOT spawned by us -- will NOT be killed)")
    else:
        pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
        print(f"spawned MASHED pid={pid}  (this session owns ONLY this pid)")
    rec = {"pid": pid, "screens": {}}
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: None)
        scr.load()
        if owned:
            dev.resume(pid)
        print(f"settling {args.settle}s ...")
        time.sleep(args.settle)

        rec["baseline"] = scr.exports_sync.stack(0)
        print("baseline:", rec["baseline"])

        scr.exports_sync.arm()
        for a in [int(x) for x in args.prefix.split(",") if x.strip() != ""]:
            scr.exports_sync.press(0, a)
            time.sleep(1.2)
        time.sleep(1.5)
        rec["after_prefix"] = scr.exports_sync.stack(0)
        print("after prefix:", rec["after_prefix"])

        if args.to_race > 0:
            scr.exports_sync.arm()
            path_log = []
            in_race = False
            for k in range(args.to_race):
                st = scr.exports_sync.stack(0)
                path_log.append({"press": k, **{q: st.get(q) for q in
                                 ("depth","types","curscreen","phase","sphase","e7c8")}})
                print(f"  [{k:02d}] depth={st.get('depth')} types={st.get('types')} "
                      f"cur={st.get('curscreen')} ph={st.get('phase')} sphase={st.get('sphase')}")
                if st.get("sphase") == 3:
                    in_race = True
                    print(f"  *** in race after {k} confirms -- STOP pressing ***")
                    break
                scr.exports_sync.press(0, 4)
                time.sleep(1.8)
            rec["path"] = path_log
            rec["in_race"] = in_race
            if not in_race:
                print("  did NOT reach a race; not firing (would be an invalid test)")
                Path(args.out).write_text(json.dumps(rec, indent=2, default=str), encoding="utf-8")
                return
            print("  settling 6s in-race with NO input ...")
            time.sleep(6.0)

        if args.await_gate >= 0:
            if args.race_seq:
                scr.exports_sync.arm()
                for a in [int(x) for x in args.race_seq.split(",") if x.strip()]:
                    scr.exports_sync.press(0, a)
                    time.sleep(args.seq_delay)
                    st = scr.exports_sync.stack(0)
                    print(f"    seq {a}: depth={st.get('depth')} gate={st.get('gate')} "
                          f"sphase={st.get('sphase')} fired={scr.exports_sync.fired()}")
            if args.hold >= 0:
                scr.exports_sync.arm()
                scr.exports_sync.hold(0, args.hold)
                print(f"  holding action {args.hold} (player 0) for the whole wait")
            print(f"  waiting up to {args.await_secs}s for gate == {args.await_gate} ...")
            t0 = time.perf_counter()
            seen, hit = [], False
            while time.perf_counter() - t0 < args.await_secs:
                st = scr.exports_sync.stack(0)
                key = (st.get("depth"), st.get("gate"), st.get("sphase"))
                if not seen or seen[-1] != key:
                    seen.append(key)
                    print(f"    t={time.perf_counter()-t0:6.1f}s depth={st.get('depth')} "
                          f"gate={st.get('gate')} sphase={st.get('sphase')} "
                          f"page={st.get('ea08')}")
                if st.get("gate") == args.await_gate:
                    hit = True
                    print(f"    *** gate == {args.await_gate} reached naturally ***")
                    break
                time.sleep(0.3)
            rec["await_path"] = [list(k) for k in seen]
            rec["await_hit"] = hit
            if not hit:
                print("    never reached the target gate; not firing (invalid test)")
                Path(args.out).write_text(json.dumps(rec, indent=2, default=str),
                                          encoding="utf-8")
                return
            scr.exports_sync.release()
            time.sleep(0.6)
            rec["at_gate"] = scr.exports_sync.stack(0)
            rec["shot_at_gate"] = shot(f"await{args.await_gate}_before.bmp")

        if args.watch > 0:
            import copy
            def sample(tag, secs):
                seen, t0 = [], time.time()
                while time.time() - t0 < secs:
                    st = scr.exports_sync.stack(0)
                    key = (st.get("depth"), tuple(st.get("types") or []), st.get("curscreen"),
                           st.get("phase"), st.get("e7c8"), st.get("eab0"), st.get("flag"), st.get("sphase"))
                    if not seen or seen[-1][0] != key:
                        seen.append((key, st))
                        print(f"  [{tag}] {st}")
                    time.sleep(0.25)
                return [s for _, s in seen]
            rec["control"] = sample("control", args.watch)
            scr.exports_sync.arm()
            print(f"--- firing action {args.fire}")
            n0 = scr.exports_sync.fired()
            scr.exports_sync.press(0, args.fire)
            rec["after"] = sample("after", args.watch)
            rec["fired_delta"] = scr.exports_sync.fired() - n0
            rec["settled"] = scr.exports_sync.stack(0)
            rec["settled_shot"] = shot(f"race_fire{args.fire}_settled.bmp")
            print("settled:", rec["settled"])
            print("  shot:", rec["settled_shot"])
            print(f"fired_delta={rec['fired_delta']}")
            Path(args.out).write_text(json.dumps(rec, indent=2, default=str), encoding="utf-8")
            print(f"wrote {args.out}")
            return

        scr.exports_sync.armpush()
        for sid in ids:
            print(f"--- pushing screen {sid}")
            scr.exports_sync.pushscreen(sid)
            time.sleep(args.dwell)
            for a in [int(x) for x in args.after.split(",") if x.strip() != ""]:
                scr.exports_sync.press(0, a)
                time.sleep(1.5)
            time.sleep(1.0)
            st = scr.exports_sync.stack(0)
            path = shot(f"screen_{sid}.bmp")
            st["shot"] = path
            if args.poke_flag:
                def tl(tag, secs, step=0.3):
                    out, t0 = [], time.perf_counter()
                    while time.perf_counter() - t0 < secs:
                        st2 = scr.exports_sync.stack(0)
                        out.append((round(time.perf_counter() - t0, 2),
                                    scr.exports_sync.page(), st2.get('flag'),
                                    st2.get('ebc8')))
                        time.sleep(step)
                    uniq = [out[0]]
                    for r in out[1:]:
                        if r[1:] != uniq[-1][1:]:
                            uniq.append(r)
                    print('    [' + tag + '] t,page,flag,queue: ' + str(uniq))
                    return out
                # long control FIRST: does the page auto-advance on its own?
                ctrl = tl('control 12s', 12.0)
                pages_ctrl = sorted(set(r[1] for r in ctrl))
                wrote = scr.exports_sync.pokeflag()
                print('    poked DAT_0067ec28 = ' + str(wrote))
                after = tl('after poke 6s', 6.0)
                pages_after = sorted(set(r[1] for r in after))
                st['poke'] = {'control_pages': pages_ctrl,
                              'after_pages': pages_after,
                              'control': ctrl, 'after': after}
                print('    control saw pages ' + str(pages_ctrl)
                      + ' ; after poke ' + str(pages_after))
                st['shot_after_poke'] = shot('poke_screen' + str(sid) + '.bmp')
            if args.fire >= 0:
                st["pre_page"] = scr.exports_sync.page()
                st["fired_before"] = scr.exports_sync.fired()
                scr.exports_sync.press(0, args.fire)
                time.sleep(2.5)
                st["fired_after"] = scr.exports_sync.fired()
                st["post_page"] = scr.exports_sync.page()
                st["post_stack"] = scr.exports_sync.stack(0)
                st["shot_after"] = shot(f"screen_{sid}_fire{args.fire}.bmp")
            st["push_count"] = scr.exports_sync.pushed()
            st["err"] = scr.exports_sync.lasterr()
            rec["screens"][str(sid)] = st
            print(f"    {st}")
        Path(args.out).write_text(json.dumps(rec, indent=2), encoding="utf-8")
        print(f"wrote {args.out}")
    finally:
        if not owned:
            print(f"leaving pid={pid} alive (owned by the launcher)")
            return
        try:
            dev.kill(pid)          # ONLY our pid, never by name
            print(f"killed pid={pid}")
        except Exception as e:
            print(f"could not kill pid={pid}: {e}")


if __name__ == "__main__":
    sys.exit(main() or 0)
