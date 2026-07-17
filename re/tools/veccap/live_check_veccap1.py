#!/usr/bin/env python3
# re/tools/veccap/live_check_veccap1.py
#
# VECCAP-1 fix live confirmation (2026-07-16): in a REAL game process (dev .asi
# auto-loaded by dinput8, hooks BYPASSED via MASHED_RE_NO_AUTO_HOOK), call our
# exported reimpls side-by-side against the original RVAs on synthetic vectors.
# Before the fix, the exports silently took the CPU fallback (LUT above the old
# kImgHi bound) and mismatched on non-zero inputs; after the fix the guard must
# find the live heap LUT and match bit-for-bit.
#
# Checks: Vec3Magnitude (0x004c3ac0), FastSqrt (0x004c3b30), FastInvSqrt (0x004c3b90).
import os
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parents[3]
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'

AGENT_JS = r"""
'use strict';
const CASES = [
    { name: 'Vec3Magnitude', rva: ptr('0x004c3ac0'), kind: 'vec3' },
    { name: 'FastSqrt',      rva: ptr('0x004c3b30'), kind: 'f' },
    { name: 'FastInvSqrt',   rva: ptr('0x004c3b90'), kind: 'f' },
];
const scratch = Memory.alloc(32);
const _keepAlive = [scratch];

// Under MASHED_RE_NO_AUTO_HOOK=1 the dinput8 proxy does NOT load the .asi, so
// load it explicitly — same pattern as diff_template.js runDiff() (and safe
// here for the same reason: no auto-loaded copy to double-load).
const ASI = Module.load('%ASI_PATH%');

function findExport(name) {
    return ASI.findExportByName(name);
}

rpc.exports = {
    check(vectors) {
        const out = [];
        for (const c of CASES) {
            const exp = findExport(c.name);
            if (!exp) { out.push({ name: c.name, error: 'export not found' }); continue; }
            let mism = 0, total = 0, first = null;
            if (c.kind === 'vec3') {
                const orig = new NativeFunction(c.rva, 'float', ['pointer']);
                const ours = new NativeFunction(exp,   'float', ['pointer']);
                for (const v of vectors) {
                    scratch.writeFloat(v[0]); scratch.add(4).writeFloat(v[1]); scratch.add(8).writeFloat(v[2]);
                    const a = orig(scratch);
                    scratch.writeFloat(v[0]); scratch.add(4).writeFloat(v[1]); scratch.add(8).writeFloat(v[2]);
                    const b = ours(scratch);
                    scratch.add(16).writeFloat(a); scratch.add(20).writeFloat(b);
                    total++;
                    if (scratch.add(16).readU32() !== scratch.add(20).readU32()) {
                        mism++;
                        if (!first) first = [v, scratch.add(16).readU32(), scratch.add(20).readU32()];
                    }
                }
            } else {
                const orig = new NativeFunction(c.rva, 'float', ['float']);
                const ours = new NativeFunction(exp,   'float', ['float']);
                for (const v of vectors) {
                    const x = Math.abs(v[0]) + Math.abs(v[2]);   // positive scalar per vector
                    const a = orig(x), b = ours(x);
                    scratch.writeFloat(a); scratch.add(4).writeFloat(b);
                    total++;
                    if (scratch.readU32() !== scratch.add(4).readU32()) {
                        mism++;
                        if (!first) first = [x, scratch.readU32(), scratch.add(4).readU32()];
                    }
                }
            }
            out.push({ name: c.name, total: total, mismatches: mism, first: first });
        }
        return out;
    }
};
"""


def synth_vectors(n=512):
    out = []
    for i in range(n):
        a = struct.unpack('<f', struct.pack('<I', (0x3d800000 + i * 0x00123457) & 0x7f7fffff))[0]
        out.append([a, -a * 0.5, a * 2.0 if i % 3 else 0.0])
    out.append([0.0, 0.0, 0.0])
    return out


def main():
    cfg = ROOT / 'scripts' / 'canonical' / 'videocfg_windowed.bin'
    if cfg.exists():
        shutil.copy2(str(cfg), str(MASHED_EXE.parent / 'videocfg.bin'))
    env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=0x00000200 | 0x00000008)
    print(f'spawned MASHED pid={proc.pid} (killing ONLY this pid at exit)')
    try:
        time.sleep(15)
        if proc.poll() is not None:
            sys.exit(f'FATAL: MASHED exited early (code {proc.returncode})')
        session = frida.get_local_device().attach(proc.pid)
        asi_path = str(MASHED_EXE.parent / 'mashed_re_dev.asi').replace('\\', '\\\\')
        script = session.create_script(AGENT_JS.replace('%ASI_PATH%', asi_path))
        script.load()
        results = script.exports_sync.check(synth_vectors())
        fail = 0
        for r in results:
            if 'error' in r:
                print(f"{r['name']}: ERROR {r['error']}")
                fail = 1
                continue
            verdict = 'PASS' if r['mismatches'] == 0 else 'FAIL'
            print(f"{r['name']}: {r['total']} vectors, {r['mismatches']} mismatches -> {verdict}")
            if r['first']:
                print(f"   first mismatch: in={r['first'][0]} orig=0x{r['first'][1]:08x} ours=0x{r['first'][2]:08x}")
            fail |= (1 if r['mismatches'] else 0)
        session.detach()
        return fail
    finally:
        try:
            proc.kill()
            print(f'killed own pid {proc.pid}')
        except Exception:
            pass


if __name__ == '__main__':
    sys.exit(main())
