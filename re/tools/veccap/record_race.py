#!/usr/bin/env python3
# re/tools/veccap/record_race.py
#
# Record REAL calls of veccap registry functions while the ORIGINAL plays a
# canonical Quick-Battle race (Arctic, nav_agent.js recipe = nav_to_race.py),
# for the replay-as-C3 lane (replay_c3.py; clause in re/CONFIDENCE.md).
#
# Per recorded call (one function hooked at a time, every --every-th call,
# short window, detached after: hot-path rule respected):
#   in_bits    input floats as the function received them (in1 | in2 | scalar)
#   out_pre    the out buffer BEFORE the call (slots it never writes compare equal)
#   out_live   the out buffer AFTER the call, read in onLeave = the game's result
#   ret_live   f32 of ST0 at onLeave via `fst dword` (float-return kinds), else null
#   cw, mxcsr  the calling thread's x87 control word and MXCSR at entry (the race
#              loop runs PC_24, memory track-load-runs-at-pc24; the emulator must too)
#   ptrs       the live out/in pointers, so aliasing (out == in) replays exactly
#
# Nothing here calls the original a second time: every leg-1 value is what the
# game itself computed.
#
# Usage: py -3.12 re/tools/veccap/record_race.py [--funcs a,b] [--every 7]
#            [--max 600] [--window 6] [--settle 12]
# Writes re/replay/veccap/<rva>_<name>.json (committed evidence, small).
# Spawns its OWN MASHED and kills ONLY that PID.
import argparse
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

import frida

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from veccap_registry import FUNCS, n_in, n_out, n_a, has_scalar, mashed_root  # noqa: E402

ROOT = HERE.parents[2]
GAME = mashed_root() / 'original'
EXE = GAME / 'MASHED.exe'
NAV = (ROOT / 're' / 'frida' / 'nav_agent.js').read_text(encoding='utf-8')
BUNDLE_DIR = ROOT / 're' / 'replay' / 'veccap'
NPG, DET, ABOVE = 0x00000200, 0x00000008, 0x00008000

REC_JS = r"""
'use strict';
// mov eax,[esp+4] ; fnstcw [eax] ; stmxcsr [eax+4] ; ret
const cwStub = Memory.alloc(Process.pageSize);
cwStub.writeByteArray([0x8b,0x44,0x24,0x04, 0xd9,0x38, 0x0f,0xae,0x58,0x04, 0xc3]);
// mov eax,[esp+4] ; fst dword [eax] ; ret        (ST0 left in place)
const stStub = cwStub.add(0x40);
stStub.writeByteArray([0x8b,0x44,0x24,0x04, 0xd9,0x10, 0xc3]);
Memory.protect(cwStub, Process.pageSize, 'r-x');
const readCw = new NativeFunction(cwStub, 'void', ['pointer']);
const readSt = new NativeFunction(stStub, 'void', ['pointer']);
const tmp = Memory.alloc(16);

const L = {};   // key -> { recs, seen, listener }
function rd(p, n) { const v = []; for (let i = 0; i < n; i++) v.push(p.add(i * 4).readU32()); return v; }

function hex(buf) { const u = new Uint8Array(buf); let o = ''; for (let i = 0; i < u.length; i++) o += u[i].toString(16).padStart(2, '0'); return o; }
rpc.exports = {
    // RW fast-sqrt LUT regions (same shape as capture_vectors.py snapshot())
    snapshot() {
        const g = ptr('0x007d3ff8').readU32(), off = ptr('0x007d3ffc').readU32();
        const slot = g + off, root = ptr(slot).readU32(), inv = ptr(slot + 4).readU32();
        return { rw_globals: g, rw_offset: off, slot_addr: slot, lut_root: root, lut_inv_root: inv,
                 lut_hex: hex(ptr(root).readByteArray(0x4000)), lut_inv_hex: hex(ptr(inv).readByteArray(0x4000)) };
    },
    start(key, rva, kind, nA, nIn, nOut, sflag, every, max) {
        const st = { recs: [], seen: 0, listener: null }; L[key] = st;
        st.listener = Interceptor.attach(ptr(rva), {
            onEnter(args) {
                st.seen++;
                this.r = null;
                if ((st.seen % every) !== 0) return;
                // reservoir sampling over the eligible calls: the bundle is a uniform sample
                // of the whole window, not its first `max` calls.
                const k = st.seen / every;
                let slot = st.recs.length;
                if (slot >= max) { slot = Math.floor(Math.random() * k); if (slot >= max) return; }
                this.slot = slot;
                readCw(tmp);
                const cw = tmp.readU16(), mx = tmp.add(4).readU32();
                const r = { cw: cw, mxcsr: mx, ptrs: [], in_bits: [], out_pre: [], out_live: [], ret_live: null };
                if (kind === 'f_f') {
                    r.in_bits = [args[0].toUInt32()];
                } else if (kind === 'f_ptrN') {
                    r.ptrs = [args[0].toUInt32()];
                    r.in_bits = rd(args[0], nIn);
                } else if (kind === 'v_out_2in') {
                    const nB = nIn - nA - sflag;
                    r.ptrs = [args[0].toUInt32(), args[1].toUInt32(), args[2].toUInt32()];
                    r.in_bits = rd(args[1], nA).concat(rd(args[2], nB));
                    if (sflag) r.in_bits.push(args[3].toUInt32());
                    r.out_pre = rd(args[0], nOut);
                } else {   // f_out_in / v_out_in : (out, in)
                    r.ptrs = [args[0].toUInt32(), args[1].toUInt32()];
                    r.in_bits = rd(args[1], nIn);
                    r.out_pre = rd(args[0], nOut);
                }
                this.r = r; this.out = (kind === 'f_f' || kind === 'f_ptrN') ? null : args[0];
            },
            onLeave(ret) {
                const r = this.r; if (!r) return;
                if (this.out) r.out_live = rd(this.out, nOut);
                if (kind === 'f_f' || kind === 'f_ptrN' || kind === 'f_out_in') {
                    readSt(tmp); r.ret_live = tmp.readU32();
                }
                st.recs[this.slot] = r;
            }
        });
        return true;
    },
    stop(key) {
        const st = L[key];
        if (st.listener) { st.listener.detach(); st.listener = null; }
        delete L[key];
        return { seen: st.seen, recs: st.recs };
    },
};
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--funcs', default='', help='comma list recorded IN RACE, one at a time '
                    '(default: every record=True entry not in --preload)')
    ap.add_argument('--preload', default='', help='comma list attached TOGETHER at the menu, before the '
                    'race is launched, and recorded through track load + --settle + --window (cold functions only)')
    ap.add_argument('--preload-until-race', action='store_true',
                    help='stop the --preload group at race entry (title + menu + track load only) and merge '
                         'its calls into the same function race window when it is also in --funcs')
    ap.add_argument('--every', type=int, default=1)
    ap.add_argument('--max', type=int, default=1200)
    ap.add_argument('--window', type=float, default=6.0)
    ap.add_argument('--menu-window', type=float, default=0.0,
                    help='also record each --funcs entry for this many seconds at the main menu, '
                         'before the race; merged into the same bundle (calls tagged w=menu|race)')
    ap.add_argument('--settle', type=float, default=12.0, help='seconds in race before recording')
    a = ap.parse_args()
    pre = [n for n in a.preload.split(',') if n]
    names = [n for n in a.funcs.split(',') if n] or         [n for n, c in FUNCS.items() if c.get('record') and n not in pre]
    for n in names + pre:
        if n not in FUNCS:
            sys.exit(f'unknown registry name {n}')
    BUNDLE_DIR.mkdir(parents=True, exist_ok=True)

    canon = mashed_root() / 'scripts' / 'canonical' / 'videocfg_windowed.bin'
    if canon.exists():
        shutil.copy2(str(canon), str(GAME / 'videocfg.bin'))
    env = dict(os.environ)
    env['MASHED_RE_NO_AUTO_HOOK'] = '1'
    dev = frida.get_local_device()
    proc = sess = None
    for _ in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(GAME), env=env, stdin=subprocess.DEVNULL,
                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                creationflags=NPG | DET | ABOVE)
        time.sleep(0.2)
        try:
            sess = dev.attach(proc.pid)
            break
        except Exception:
            try:
                proc.kill()
            except Exception:
                pass
            time.sleep(1.0)
    if sess is None:
        sys.exit('attach failed after 5 retries')
    print(f'spawned MASHED pid={proc.pid} (killing ONLY this pid at exit)')
    try:
        nav = sess.create_script(NAV)
        nav.load()
        E = nav.exports_sync
        E.init()

        def wait(pred, t):
            end = time.time() + t
            while time.time() < end:
                if pred():
                    return True
                time.sleep(0.1)
            return False

        def press(c, ms=180):
            E.press(c, ms)
            time.sleep(ms / 1000.0 + 0.3)

        def confirm_to(target, tries=6):
            for _ in range(tries):
                if E.depth() >= target:
                    return True
                press(4)
                if wait(lambda: E.depth() >= target, 2.0):
                    return True
            return E.depth() >= target

        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30):
            sys.exit('never reached title')
        time.sleep(1.0)
        rec = sess.create_script(REC_JS)
        rec.load()
        R = rec.exports_sync

        def begin(name):
            cfg = FUNCS[name]
            R.start(name, f"0x{cfg['rva']:08x}", cfg['kind'], n_a(cfg), n_in(cfg), n_out(cfg),
                    has_scalar(cfg), a.every, a.max)
            return time.time()

        menu_acc = {}

        def collect(name, t0, when):
            res = R.stop(name)
            dt = time.time() - t0
            res['recs'] = [r for r in res['recs'] if r]   # a slot whose onLeave never ran
            for r in res['recs']:
                r['w'] = when
            print(f"{name:<18} [{when}] seen={res['seen']:<7} recorded={len(res['recs']):<5} "
                  f"({res['seen'] / dt:.0f}/s)")
            return {'when': when, 'seen': res['seen'], 'recorded': len(res['recs']),
                    'window_s': round(dt, 2), 'recs': res['recs']}

        def finish(name, t0, when):
            cfg = FUNCS[name]
            wins = ([menu_acc[name]] if name in menu_acc else []) + [collect(name, t0, when)]
            res = {'seen': sum(w['seen'] for w in wins), 'recs': [r for w in wins for r in w['recs']]}
            dt = sum(w['window_s'] for w in wins)
            line = f"{name:<18} total recorded={len(res['recs'])}"
            if not res['recs']:
                print(line + '  (no bundle written)')
                return
            bundle = {
                'name': name, 'rva': cfg['rva'], 'kind': cfg['kind'], 'export': cfg['export'],
                'n_in': n_in(cfg), 'n_out': n_out(cfg), 'n_a': n_a(cfg), 'scalar': has_scalar(cfg),
                'scenario': ('boot -> main menu -> quick-battle arctic race (nav_agent.js recipe), hooks OFF '
                             f"(MASHED_RE_NO_AUTO_HOOK=1), windows={'+'.join(w['when'] for w in wins)}"),
                'windows': [{k: v for k, v in w.items() if k != 'recs'} for w in wins],
                'recorded_at': time.strftime('%Y-%m-%d %H:%M:%S'), 'pid': proc.pid,
                'calls_seen': res['seen'], 'every': a.every, 'window_s': round(dt, 2),
                'game_phase': E.phase(), 'calls': res['recs'],
            }
            if cfg.get('rw_lut'):
                bundle['regions'] = R.snapshot()
            out = BUNDLE_DIR / f"{cfg['rva']:08x}_{name}.json"
            out.write_text(json.dumps(bundle))
            print(f'{line} -> {out.relative_to(ROOT)}')

        for n in names if a.menu_window > 0 else []:
            t0 = begin(n)
            time.sleep(a.menu_window)
            menu_acc[n] = collect(n, t0, 'menu')
        t_pre = {n: begin(n) for n in pre}
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(1); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3:
                break
            press(4); time.sleep(1.5)
        if E.phase() == 3:
            sys.exit('NOT in race')
        if pre and a.preload_until_race:
            for n in pre:
                if n in names:
                    menu_acc[n] = collect(n, t_pre[n], 'title+menu+load')
                else:
                    finish(n, t_pre[n], 'title+menu+load')
        print(f'in race (phase={E.phase()}); settling {a.settle}s')
        time.sleep(a.settle)
        if pre and not a.preload_until_race:
            time.sleep(a.window)
            for n in pre:
                finish(n, t_pre[n], 'menu->load->race')

        for name in names:
            if proc.poll() is not None:
                sys.exit(f'MASHED exited (code {proc.returncode}) before {name}')
            t0 = begin(name)
            time.sleep(a.window)
            finish(name, t0, 'race')
        rec.unload()
        nav.unload()
        sess.detach()
    finally:
        try:
            proc.kill()
            print(f'killed own pid {proc.pid}')
        except Exception:
            pass


if __name__ == '__main__':
    main()
