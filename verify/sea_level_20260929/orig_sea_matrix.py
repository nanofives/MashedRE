# Cold read of the ORIGINAL's course clump table + the sea clump's RW frame matrices.
#
# NO Interceptor anywhere (project rule: Interceptor on hot paths destabilises MASHED in
# ~6 s). This attaches to an already-running MASHED.exe by an EXPLICIT pid, reads memory
# on a 1 Hz timer, and detaches. It never writes.
#
# Addresses (all verified in this session against Mashed_pool0, read-only):
#   0x00646e58  course object base. FUN_00426ab0 (per-frame, called from FUN_0040fc00)
#               passes &DAT_00646e58 as param_1 of FUN_00426700, which indexes
#               param_1 + 0x10110 + idx*4.
#   +0x10110    clump-pointer array, 0x40 entries, idx = the COURSE.LUA Clump_Filename
#               index. Written by the loader FUN_00479330 (puVar5[-0xa8] with
#               puVar5 = param_1 + 0x103b0), read back by the world-registration walk.
#   +0x103b0    per-clump "loaded" flag (loader writes 1).
#   +0x105d8    the Sky_Filename clump (loader: param_2[0x1980] branch).
#   clump + 4   RwObject.parent == the clump's RwFrame (RwObject is
#               {u8 type, u8 subType, u8 flags, u8 privateFlags, void *parent}).
#               Corroborated by FUN_004756e0, which passes *(clump+4) to
#               FUN_004c1480(frame, matrix, 0).
#
# Frame layout is NOT assumed: the script dumps raw bytes and prints every 0x40-byte
# window that looks like an RwMatrix so the offsets can be read off the data.
#
# Usage:  py -3.12 verify/sea_level_20260929/orig_sea_matrix.py --pid <pid> [--clump 2]
import argparse
import json
import struct
import sys
import time

import frida

AGENT = r"""
'use strict';
const IMG = 0x400000;
function ga(va){ const m = Process.findModuleByName('MASHED.exe'); return m ? m.base.add(va - IMG) : null; }
const COURSE = 0x00646e58;
const CLUMPS = 0x10110;
const FLAGS  = 0x103b0;
const SKY    = 0x105d8;

function rd(p, n){ try { return Array.from(new Uint8Array(p.readByteArray(n))); } catch(e){ return null; } }

function sample(idx){
    const base = ga(COURSE);
    if (!base) return {err:'no module'};
    const out = {idx: idx, slots: [], clump: 0, frame: 0};
    for (let i = 0; i < 0x40; i++) {
        const p = base.add(CLUMPS + i*4).readU32();
        const f = base.add(FLAGS  + i*4).readU32();
        if (p !== 0 || f !== 0) out.slots.push([i, p, f]);
    }
    out.sky = base.add(SKY).readU32();
    const cp = base.add(CLUMPS + idx*4).readU32();
    out.clump = cp;
    if (cp !== 0) {
        out.clump_bytes = rd(ptr(cp), 0x40);
        const fr = ptr(cp).add(4).readU32();
        out.frame = fr;
        if (fr !== 0) out.frame_bytes = rd(ptr(fr), 0xA0);
    }
    return out;
}

rpc.exports = {
    grab: function (idx) { return sample(idx); }
};
"""


def mats(b, label):
    """Print every 0x40 window of `b` as an RwMatrix (right/up/at/pos, 4 floats each)."""
    for off in range(0, len(b) - 0x3F, 0x10):
        f = struct.unpack_from("<16f", bytes(b), off)
        rows = [f[0:4], f[4:8], f[8:12], f[12:16]]
        print(f"  {label}+0x{off:02x}  "
              + " | ".join("(%9.4f %9.4f %9.4f)" % (r[0], r[1], r[2]) for r in rows))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, required=True, help="explicit MASHED.exe pid (never guess)")
    ap.add_argument("--clump", type=int, default=2, help="COURSE.LUA Clump_Filename index (Arctic sea = 2)")
    ap.add_argument("--samples", type=int, default=6)
    ap.add_argument("--interval", type=float, default=1.0)
    ap.add_argument("--out", default="")
    a = ap.parse_args()

    sess = frida.attach(a.pid)
    scr = sess.create_script(AGENT)
    scr.load()
    recs = []
    try:
        for n in range(a.samples):
            r = scr.exports_sync.grab(a.clump)
            recs.append(r)
            print(f"--- sample {n} ---")
            if r.get("err"):
                print("  ", r["err"]); time.sleep(a.interval); continue
            print("  populated clump slots (idx, ptr, loadedflag):",
                  [(i, hex(p), f) for (i, p, f) in r["slots"]])
            print("  sky clump:", hex(r.get("sky", 0)))
            print(f"  clump[{a.clump}] = {hex(r['clump'])}  frame = {hex(r['frame'])}")
            if r.get("frame_bytes"):
                mats(r["frame_bytes"], "frame")
            time.sleep(a.interval)
    finally:
        try:
            scr.unload()
            sess.detach()
        except Exception:
            pass
    if a.out:
        with open(a.out, "w") as fh:
            json.dump(recs, fh, indent=1)
        print("wrote", a.out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
