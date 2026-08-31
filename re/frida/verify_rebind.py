r"""verify_rebind.py — prove a hand-written contcfg%d.bin actually rebinds a control.

Two claims, tested separately, because passing one and failing the other means
very different things:

  CLAIM 1 (load): the file we wrote is read into DAT_007e95c0 + slot*0x200.
      Checked by reading the record back out of the live process and comparing
      it byte-for-byte with the file.
  CLAIM 2 (honoured): the NEW key actually drives the action.
      Checked by setting that scancode's bit in the DirectInput keyboard bitmap
      at DAT_0077313c for exactly one frame and watching the frontend move.

Plus a NEGATIVE CONTROL for claim 2: inject an UNBOUND scancode the same way.
Without it, "the screen changed" proves nothing -- the title screen advances on
several inputs and the menu background animates on its own.

Injection point is FUN_00496530 (the per-player cook, ~60/s per active player),
because the bitmap at 0x0077313c is filled earlier in the frame by FUN_004972b0
and read later by FUN_00497310. Writing it on entry to the cook lands between
those two. This is NOT the FUN_00497310 return-override used by
probe_action_semantics.py: that one bypasses the binding table entirely, which
is exactly what must NOT be bypassed here.

PID hygiene (CLAUDE.md): spawns its own MASHED, kills ONLY that pid.

Usage:
  py -3.12 re\frida\verify_rebind.py --cfg <path-to-contcfg0.bin> --key 0x39 --control 0x14
"""
import argparse
import os
import shutil
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parents[2]
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"

AGENT = r"""
const CFG_BASE   = 0x007e95c0;   // controller-config table, stride 0x200
const KBD_BITMAP = 0x0077313c;   // DirectInput keyboard bitmap, 32 bytes
const CURSCREEN  = 0x0067ecb0;
const RVA_COOK   = 0x00496530;   // FUN_00496530, per-player cook

let injectKey = -1;      // scancode to force for one frame; consumed on use
let injected  = 0;
let armed     = false;

rpc.exports = {
  record: function (slot) {
    return Array.from(ptr(CFG_BASE + slot * 0x200).readByteArray(0x200)
                      ? new Uint8Array(ptr(CFG_BASE + slot * 0x200).readByteArray(0x200))
                      : []);
  },
  screen: function () { return ptr(CURSCREEN).readS32(); },
  arm: function () {
    if (armed) return 1;
    Interceptor.attach(ptr(RVA_COOK), {
      onEnter() {
        if (injectKey < 0) return;
        const k = injectKey;
        const p = ptr(KBD_BITMAP + (k >> 3));
        p.writeU8(p.readU8() | (1 << (k & 7)));
        injectKey = -1;          // exactly one frame -> exactly one rising edge
        injected++;
      }
    });
    armed = true;
    return 1;
  },
  inject: function (k) { injectKey = k; return 1; },
  injected: function () { return injected; }
};
send({kind: 'ready'});
"""


def dump_live(out_path, settle, slot=0):
    """Boot, settle, and write slot 0's record to disk.

    This is the correct starting point for an edited config, not a fabricated
    record: FUN_00498510 validates a loaded file by strcmp-ing its name field
    at +0x004 against the name its own defaults builder wrote (a LOCALIZED
    resource string, LoadStringA of lang+0x82a into DAT_007730b4). A wrong name
    makes the game silently restore the defaults over the loaded record --
    the load "succeeds" and is then discarded, which looks exactly like the
    file never being read.
    """
    env = dict(os.environ)
    env["MASHED_WIN_POS"] = "left-bl"
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"spawned MASHED pid={pid}  (this session owns ONLY this pid)")
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: None)
        scr.load()
        dev.resume(pid)
        print(f"settling {settle}s ...")
        time.sleep(settle)
        if slot < 0:
            # slot -1 = dump all four. Which slot is the KEYBOARD is not fixed:
            # FUN_00498510 gives slots 0..joycount-1 to joypads and slot
            # joycount to the keyboard, so it depends on how many devices
            # enumerate as DirectInput GAMECTRL on this machine.
            names = {0: 'inactive', 1: 'joypad', 2: 'keyboard'}
            for i in range(4):
                r = bytes(scr.exports_sync.record(i))
                Path(str(out_path) + '.slot' + str(i)).write_bytes(r)
                nm = r[4:4 + 0x104].split(bytes([0]), 1)[0].decode('ascii', 'replace')
                dt = int.from_bytes(r[0x13c:0x140], 'little')
                print('  slot %d: type=%d (%s) name=%r'
                      % (i, dt, names.get(dt, '?'), nm))
            return 0
        rec = bytes(scr.exports_sync.record(slot))
        Path(out_path).write_bytes(rec)
        name = rec[4:4 + 0x104].split(bytes([0]), 1)[0].decode("ascii", "replace")
        print(f"wrote {out_path} ({len(rec)} bytes); live device name = {name!r}")
    finally:
        try:
            dev.kill(pid); print(f"killed pid={pid}")
        except Exception as e:
            print(f"could not kill pid={pid}: {e}")
    return 0


def run(cfg_path, key, control_key, settle, dwell, keep, slot=0):
    installed = ORIG / f"contcfg{slot}.bin"
    pre_existing = installed.exists()
    if pre_existing:
        print(f"!! {installed} already exists -- refusing to overwrite it.")
        print("   Move it aside yourself if this is stale; it may belong to another session.")
        return 2
    shutil.copyfile(cfg_path, installed)
    print(f"installed {installed}  (will be removed unless --keep)")
    file_bytes = installed.read_bytes()

    env = dict(os.environ)
    env["MASHED_WIN_POS"] = "left-bl"
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"spawned MASHED pid={pid}  (this session owns ONLY this pid)")
    rc = 1
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: None)
        scr.load()
        dev.resume(pid)
        print(f"settling {settle}s ...")
        time.sleep(settle)
        scr.exports_sync.arm()

        # ---- CLAIM 1: did the loader actually read our file? ----
        live = bytes(scr.exports_sync.record(slot))
        same = live == file_bytes
        print(f"\nCLAIM 1 (file loaded into DAT_007e95c0): "
              f"{'PASS' if same else 'FAIL'}")
        if not same:
            fb = [i for i in range(0x200) if live[i] != file_bytes[i]]
            print(f"   {len(fb)} differing bytes; first at +0x{fb[0]:03x} "
                  f"(file 0x{file_bytes[fb[0]]:02x} vs live 0x{live[fb[0]]:02x})")
            b_file = file_bytes[0x108:0x108 + 52]
            b_live = live[0x108:0x108 + 52]
            print(f"   bindings match: {b_file == b_live}")

        # ---- CLAIM 2: does the NEW key drive the action? ----
        s0 = scr.exports_sync.screen()
        n0 = scr.exports_sync.injected()
        scr.exports_sync.inject(key)
        time.sleep(dwell)
        s1 = scr.exports_sync.screen()
        n1 = scr.exports_sync.injected()
        moved = s0 != s1
        print(f"\nCLAIM 2 (rebound key 0x{key:02x} drives the action): "
              f"screen {s0} -> {s1}   injected={n1 - n0}   "
              f"{'PASS' if moved else 'FAIL'}")

        # ---- NEGATIVE CONTROL: an unbound scancode, same mechanism ----
        s2 = scr.exports_sync.screen()
        n2 = scr.exports_sync.injected()
        scr.exports_sync.inject(control_key)
        time.sleep(dwell)
        s3 = scr.exports_sync.screen()
        n3 = scr.exports_sync.injected()
        ctrl_moved = s2 != s3
        print(f"CONTROL   (unbound key 0x{control_key:02x}): "
              f"screen {s2} -> {s3}   injected={n3 - n2}   "
              f"{'FAIL (moved -- claim 2 is not attributable)' if ctrl_moved else 'PASS (no move)'}")

        rc = 0 if (same and moved and not ctrl_moved) else 1
        print(f"\nOVERALL: {'PASS' if rc == 0 else 'NOT PROVEN'}")
    finally:
        try:
            dev.kill(pid)
            print(f"killed pid={pid}")
        except Exception as e:
            print(f"could not kill pid={pid}: {e}")
        if not keep:
            installed.unlink(missing_ok=True)
            print(f"removed {installed}")
    return rc


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--slot", type=int, default=0,
                    help="config slot / contcfg<N>.bin index. NOTE slot 0 is not necessarily the keyboard: slots 0..joycount-1 are joypads and the keyboard is slot joycount (FUN_00498510).")
    ap.add_argument("--dump-live", help="boot and write slot 0's live record here, then exit")
    ap.add_argument("--cfg", help="contcfg0.bin to install and test")
    ap.add_argument("--key", type=lambda x: int(x, 0),
                    help="the REBOUND scancode to inject (e.g. 0x39 SPACE)")
    ap.add_argument("--control", type=lambda x: int(x, 0), default=0x14,
                    help="an UNBOUND scancode for the negative control (default 0x14 = T)")
    ap.add_argument("--settle", type=float, default=25.0)
    ap.add_argument("--dwell", type=float, default=3.0)
    ap.add_argument("--keep", action="store_true",
                    help="leave contcfg0.bin in original\\ afterwards")
    a = ap.parse_args()
    if a.dump_live:
        return dump_live(a.dump_live, a.settle, a.slot)
    if not a.cfg or a.key is None:
        print("need --cfg and --key (or --dump-live)", file=sys.stderr)
        return 2
    return run(Path(a.cfg), a.key, a.control, a.settle, a.dwell, a.keep, a.slot)


if __name__ == "__main__":
    sys.exit(main())
