#!/usr/bin/env python3
# re/tools/veccap/capture_vectors.py
#
# Registry-driven captured-vector recorder (generalized 2026-07-16; pilot
# history in README.md). For every function in veccap_registry.FUNCS:
#   * records real argument vectors from live gameplay — ONE function hooked
#     at a time, short window, auto-detached (hot-path rule respected);
#   * takes ground-truth outputs by calling the ORIGINAL in-process via
#     NativeFunction (handles x87 ST0 float returns; out-buffers captured);
# plus one shared snapshot of the runtime-built RW LUT regions (sqrt +
# inv-sqrt tables). Spawns its OWN MASHED (PID-tracked kill).
#
# Output: out/vectors.json (one file for the whole registry).
import json
import os
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path

import frida

sys.path.insert(0, str(Path(__file__).resolve().parent))
from veccap_registry import FUNCS, synth_inputs, n_in, n_out  # noqa: E402

ROOT = Path(__file__).resolve().parents[3]
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
OUT_DIR = Path(__file__).resolve().parent / 'out'

MENU_WAIT_S = 22
COLLECT_MAX = 1500
COLLECT_WINDOW_S = 5          # per function, hot-path rule: short + one at a time

AGENT_JS = r"""
'use strict';
function _hex(buf) {
    const u8 = new Uint8Array(buf); const parts = [];
    for (let i = 0; i < u8.length; i++) parts.push(u8[i].toString(16).padStart(2, '0'));
    return parts.join('');
}

const scratch = Memory.alloc(0x100);
const _keepAlive = [scratch];
let liveArgs = [];
let listener = null;
let collectMax = 0;

rpc.exports = {
    snapshot() {
        const rwGlobals = ptr('0x007d3ff8').readU32();
        const rwOffset  = ptr('0x007d3ffc').readU32();
        const slotAddr  = rwGlobals + rwOffset;
        const root      = ptr(slotAddr).readU32();
        const rootInv   = ptr(slotAddr + 4).readU32();
        return {
            rw_globals: rwGlobals, rw_offset: rwOffset, slot_addr: slotAddr,
            lut_root: root, lut_inv_root: rootInv,
            lut_hex: _hex(ptr(root).readByteArray(0x1000 * 4)),
            lut_inv_hex: _hex(ptr(rootInv).readByteArray(0x1000 * 4))
        };
    },
    // kind: 'f_f' captures raw f32 bits of the scalar arg; ptr kinds read n floats.
    start(rvaStr, kind, nIn, max) {
        liveArgs = []; collectMax = max;
        const target = ptr(rvaStr);
        listener = Interceptor.attach(target, {
            onEnter(args) {
                if (liveArgs.length >= collectMax) return;
                if (kind === 'f_f') {
                    liveArgs.push([args[0].toUInt32()]);          // raw f32 bits
                } else {
                    const p = (kind === 'f_out_in') ? args[1] : args[0];
                    const v = [];
                    for (let i = 0; i < nIn; i++) v.push(p.add(i * 4).readU32());
                    liveArgs.push(v);                              // raw f32 bits
                }
            }
        });
        return true;
    },
    stop() {
        if (listener) { listener.detach(); listener = null; }
        return liveArgs;
    },
    // vectors arrive as arrays of raw f32 bit-patterns (u32); returns
    // per-vector { ret_bits, out_bits[] }.
    truth(rvaStr, kind, nIn, nOut, vectors) {
        const target = ptr(rvaStr);
        const inBuf  = scratch;
        const outBuf = scratch.add(0x40);
        const retBuf = scratch.add(0x80);
        const results = [];
        let fn;
        if (kind === 'f_f')            fn = new NativeFunction(target, 'float', ['uint32']);
        else if (kind === 'f_ptrN')    fn = new NativeFunction(target, 'float', ['pointer']);
        else /* f_out_in */            fn = new NativeFunction(target, 'float', ['pointer', 'pointer']);
        for (const v of vectors) {
            for (let i = 0; i < nIn; i++) inBuf.add(i * 4).writeU32(v[i]);
            for (let i = 0; i < nOut; i++) outBuf.add(i * 4).writeU32(0xcccccccc);
            let r;
            if (kind === 'f_f')         r = fn(v[0]);               // bits pass through as u32 arg
            else if (kind === 'f_ptrN') r = fn(inBuf);
            else                        r = fn(outBuf, inBuf);
            retBuf.writeFloat(r);
            const rec = { ret_bits: retBuf.readU32(), out_bits: [] };
            for (let i = 0; i < nOut; i++) rec.out_bits.push(outBuf.add(i * 4).readU32());
            results.push(rec);
        }
        return results;
    },
};
"""


def f2bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    cfg_bin = ROOT / 'scripts' / 'canonical' / 'videocfg_windowed.bin'
    if cfg_bin.exists():
        shutil.copy2(str(cfg_bin), str(MASHED_EXE.parent / 'videocfg.bin'))

    env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=0x00000200 | 0x00000008)
    print(f'spawned MASHED pid={proc.pid} (killing ONLY this pid at exit)')
    try:
        print(f'waiting {MENU_WAIT_S}s for main menu...')
        time.sleep(MENU_WAIT_S)
        if proc.poll() is not None:
            sys.exit(f'FATAL: MASHED exited early (code {proc.returncode})')

        session = frida.get_local_device().attach(proc.pid)
        script = session.create_script(AGENT_JS)
        script.load()

        regions = script.exports_sync.snapshot()
        print(f"regions: rw_globals=0x{regions['rw_globals']:08x} "
              f"lut_root=0x{regions['lut_root']:08x} inv=0x{regions['lut_inv_root']:08x}")

        funcs_out = {}
        for name, cfg in FUNCS.items():
            rva = f"0x{cfg['rva']:08x}"
            ni, no = n_in(cfg), n_out(cfg)
            live_bits = []
            if cfg.get('live_capture'):
                script.exports_sync.start(rva, cfg['kind'], ni, COLLECT_MAX)
                time.sleep(COLLECT_WINDOW_S)
                live_bits = script.exports_sync.stop()
            synth_bits = [[f2bits(x) for x in v] for v in synth_inputs(cfg)]
            vectors = []
            for src, batch in (('live', live_bits), ('synth', synth_bits)):
                if not batch:
                    continue
                truths = script.exports_sync.truth(rva, cfg['kind'], ni, no, batch)
                vectors += [{'v_bits': v, 'ret_bits': t['ret_bits'],
                             'out_bits': t['out_bits'], 'src': src}
                            for v, t in zip(batch, truths)]
            funcs_out[name] = {'rva': cfg['rva'], 'kind': cfg['kind'],
                               'n_in': ni, 'n_out': no, 'vectors': vectors}
            print(f'{name}: live={len(live_bits)} synth={len(synth_bits)}')

        payload = {'captured': time.strftime('%Y-%m-%d %H:%M:%S'), 'pid': proc.pid,
                   'regions': regions, 'funcs': funcs_out}
        out = OUT_DIR / 'vectors.json'
        out.write_text(json.dumps(payload))
        print(f'wrote {out}')
        session.detach()
    finally:
        try:
            proc.kill()
            print(f'killed own pid {proc.pid}')
        except Exception:
            pass


if __name__ == '__main__':
    main()
