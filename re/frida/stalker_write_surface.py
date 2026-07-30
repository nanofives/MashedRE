#!/usr/bin/env py -3.12
# stalker_write_surface.py — DYNAMIC write-surface capture for one live call.
#
# WHY THIS EXISTS
# ---------------
# scripts/write_surface.py derives a mutator's write surface STATICALLY and is
# blocked the moment any function in the call tree dispatches through a
# register or vtable slot: 1,126 of 1,446 mutators (77%) classify
# "indirect - NOT reachable" (commit b77f6b42), which caps the snapshot/restore
# A/B lane at ~142 targets. This tool closes that gap dynamically: follow ONE
# natural in-game call of the target with Stalker, record the effective
# address of every memory-writing instruction in the whole dynamic call tree
# (through every indirect dispatch, because at runtime the dispatch has a
# concrete destination), and report the observed write set alongside the
# resolved indirect targets.
#
# EVIDENCE STATUS: the captured set is the write surface OF THE OBSERVED PATH
# only. A branch not taken on this call contributes nothing — so the output is
# a LOWER bound per call, to be unioned over multiple captures and guarded at
# restore time. It complements (never replaces) the static tool: static gives
# the sound upper scaffold where it can see; dynamic fills the holes static
# cannot enter.
#
# HOT-PATH RULES (CLAUDE.md): a single Interceptor.attach is armed and the
# FIRST call is captured, after which the listener detaches itself. Stalker
# runs only between onEnter and onLeave of that one call.
#
# Usage:
#   py -3.12 re\frida\stalker_write_surface.py 0x00495110 [--nav] [--wait 45]
#     --nav    drive toward a Quick Battle race (statenav) if the target has
#              not fired at the menu by half the wait budget
#     --wait   total seconds to wait for the target to fire (default 90)
#
# Output: a JSON report to log/stalker_ws_<rva>.json + a console summary
# classifying writes into image-globals / heap / stack(excluded), and the
# concrete destinations observed for each statically-opaque indirect site.

import json
import os
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
sys.path.insert(0, str(ROOT / 're' / 'orchestrator'))

import statenav                              # nav agent + Nav class
from run_diff import _find_original
from mashed_lock import MashedLock

MASHED_EXE = _find_original(ROOT)
ORIG = MASHED_EXE.parent
LOG_DIR = ROOT / 'log'

AGENT_JS = r'''
'use strict';
const IMG = 0x00400000;
const TARGET_RVA = $TARGET$;
let DELTA = 0;
function abs(rva) { return ptr(rva + DELTA); }

const writes = Object.create(null);   // eaHex -> {size, n, insn}
let stackSkipped = 0;
let indirects = Object.create(null);  // siteHex -> destHex
let captured = false;
let listener = null;

function classify(ea, esp) {
    // Stack writes are transient frame state, not surface: anything within
    // 1 MiB below-or-above the captured ESP is counted separately.
    const d = ea - esp;
    return (d > -0x100000 && d < 0x100000) ? 'stack' : 'other';
}

rpc.exports = {
    init: function () {
        const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
        DELTA = m.base.toUInt32() - IMG;
        const target = abs(TARGET_RVA);
        listener = Interceptor.attach(target, {
            onEnter: function (args) {
                if (captured) return;
                captured = true;
                this.doStalk = true;
                this.tid = Process.getCurrentThreadId();
                send({ ev: 'fired', tid: this.tid });
                Stalker.follow(this.tid, {
                    transform: function (iterator) {
                        let insn;
                        while ((insn = iterator.next()) !== null) {
                            try {
                                const ops = insn.operands || [];
                                // record resolved destinations of indirect call/jmp
                                if ((insn.mnemonic === 'call' || insn.mnemonic === 'jmp')
                                    && ops.length && ops[0].type !== 'imm') {
                                    const site = insn.address.toUInt32();
                                    const op = ops[0];
                                    iterator.putCallout(function (ctx) {
                                        let dest = -1;
                                        try {
                                            if (op.type === 'reg') {
                                                dest = ctx[op.value].toUInt32();
                                            } else if (op.type === 'mem') {
                                                const mv = op.value;
                                                let ea = mv.disp >>> 0;
                                                if (mv.base)  ea = (ea + ctx[mv.base].toUInt32()) >>> 0;
                                                if (mv.index) ea = (ea + ctx[mv.index].toUInt32() * mv.scale) >>> 0;
                                                dest = ptr(ea).readU32();
                                            }
                                        } catch (e) {}
                                        if (dest !== -1)
                                            indirects['0x' + site.toString(16)] = '0x' + (dest >>> 0).toString(16);
                                    });
                                }
                                for (let k = 0; k < ops.length; k++) {
                                    const op = ops[k];
                                    if (op.type === 'mem' && op.access && op.access.indexOf('w') !== -1) {
                                        const mv = op.value;
                                        const size = op.size | 0;
                                        const insnAddr = insn.address.toUInt32();
                                        iterator.putCallout(function (ctx) {
                                            try {
                                                let ea = mv.disp >>> 0;
                                                if (mv.base)  ea = (ea + ctx[mv.base].toUInt32()) >>> 0;
                                                if (mv.index) ea = (ea + ctx[mv.index].toUInt32() * mv.scale) >>> 0;
                                                if (classify(ea, ctx.esp.toUInt32()) === 'stack') { stackSkipped++; return; }
                                                const key = '0x' + ea.toString(16);
                                                if (!writes[key]) writes[key] = { size: size, n: 0, insn: '0x' + insnAddr.toString(16) };
                                                writes[key].n++;
                                            } catch (e) {}
                                        });
                                        break;
                                    }
                                }
                            } catch (e) {}
                            iterator.keep();
                        }
                    }
                });
            },
            onLeave: function (retval) {
                if (!this.doStalk) return;
                Stalker.unfollow(this.tid);
                Stalker.flush();
                try { listener.detach(); } catch (e) {}
                send({ ev: 'done', writes: writes, indirects: indirects,
                       stack_skipped: stackSkipped });
            }
        });
        return DELTA;
    }
};
'''


def main():
    if len(sys.argv) < 2:
        sys.exit('usage: stalker_write_surface.py 0xRVA [--nav] [--wait N]')
    rva = int(sys.argv[1], 16)
    do_nav = '--nav' in sys.argv
    wait_s = int(sys.argv[sys.argv.index('--wait') + 1]) if '--wait' in sys.argv else 90

    env = dict(os.environ)
    env['MASHED_RE_NO_AUTO_HOOK'] = '1'
    _gamelock = MashedLock(f'stalker:{rva:#010x}')   # wait our turn on the game
    _gamelock.acquire()
    dev = frida.get_local_device()
    pid = dev.spawn(str(MASHED_EXE), cwd=str(ORIG), env=env)
    print(f'  spawned pid={pid} (this session kills ONLY this pid)')
    sess = dev.attach(pid)

    result = {}

    def on_msg(m, d):
        if m.get('type') == 'send':
            p = m['payload']
            if p.get('ev') == 'fired':
                print(f"  >>> target fired on tid={p['tid']} — stalking this call")
            elif p.get('ev') == 'done':
                result.update(p)

    scr = sess.create_script(AGENT_JS.replace('$TARGET$', str(rva)))
    scr.on('message', on_msg)
    scr.load()

    nav_scr = sess.create_script(statenav.AGENT)
    nav_scr.on('message', lambda m, d: None)
    nav_scr.load()
    nav_scr.exports_sync.init()
    scr.exports_sync.init()

    t0 = time.time()
    try:
        dev.resume(pid)
        nav = statenav.Nav(nav_scr, pid)
        half = wait_s / 2
        navved = False
        while time.time() - t0 < wait_s and not result:
            if not nav.alive():
                print('  process exited before capture'); break
            if do_nav and not navved and time.time() - t0 > half:
                print('  not fired at menu — driving toward Quick Battle...')
                navved = True
                try:
                    nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 18.0, 'title')
                    nav.confirm_to_depth(2)
                    nav.advance_past_load_modal(3); nav.press(12)
                    nav.confirm_to_depth(4, tries=4)
                    nav.confirm_to_depth(5, tries=4)
                    nav.press(4); time.sleep(1.5)
                    for _ in range(5):
                        if nav.phase() != 3: break
                        nav.press(4); time.sleep(1.5)
                except Exception as e:
                    print(f'  nav error (continuing to wait): {e}')
            time.sleep(0.25)
    finally:
        try: dev.kill(pid)
        except Exception: pass
        print(f'  killed pid={pid}')
        _gamelock.release()

    if not result:
        print(f'NOT-FIRED within {wait_s}s — target did not execute on this path')
        return 3

    writes = result.get('writes', {})
    inds = result.get('indirects', {})
    img = {k: v for k, v in writes.items() if 0x00400000 <= int(k, 16) < 0x00b60000}
    other = {k: v for k, v in writes.items() if k not in img}
    elapsed = time.time() - t0
    print(f'\n=== DYNAMIC WRITE SURFACE 0x{rva:08x} (one call, {elapsed:.0f}s wall) ===')
    print(f'  distinct non-stack write EAs: {len(writes)} '
          f'(image-globals {len(img)}, heap/other {len(other)}); '
          f'stack writes excluded: {result.get("stack_skipped", 0)}')
    for k in sorted(img, key=lambda x: int(x, 16)):
        w = img[k]
        print(f'    IMAGE {k}  size={w["size"]}  hits={w["n"]}  insn={w["insn"]}')
    for k in sorted(other, key=lambda x: int(x, 16))[:20]:
        w = other[k]
        print(f'    OTHER {k}  size={w["size"]}  hits={w["n"]}  insn={w["insn"]}')
    if len(other) > 20:
        print(f'    ... {len(other) - 20} more heap/other EAs (full set in the JSON)')
    print(f'  indirect dispatches RESOLVED this call: {len(inds)}')
    for site, dest in inds.items():
        print(f'    {site} -> {dest}')

    LOG_DIR.mkdir(exist_ok=True)
    out = LOG_DIR / f'stalker_ws_{rva:08x}.json'
    out.write_text(json.dumps({'rva': f'0x{rva:08x}', 'elapsed_s': elapsed,
                               **result}, indent=1))
    print(f'  report -> {out}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
