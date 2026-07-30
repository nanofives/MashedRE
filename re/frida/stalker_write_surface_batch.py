#!/usr/bin/env py -3.12
# stalker_write_surface_batch.py — MANY mutators' dynamic write surface / ONE boot.
#
# The single-target stalker_write_surface.py costs one boot per mutator; the 85
# exercised mutators would be 85 boots. This reuses ONE booted+navigated race
# and captures each target's FIRST natural in-game call in turn: arm one
# Interceptor, Stalker-follow that single call, record the writes and the
# concrete destinations of every indirect dispatch, detach, move to the next.
#
# ONE follow is ever active at a time (targets processed sequentially), so there
# is no concurrent-Stalker hazard, and each target is armed only for a short
# window then detached — never left attached across the race (CLAUDE.md
# hot-path rule). Exercised mutators fire many times a second, so a target's
# first call typically lands inside ~1s of arming.
#
# Purpose: convert "indirect - NOT reachable" (77% of mutators, static) into an
# OBSERVED write surface. Static write_surface.py cannot enter an indirect
# dispatch; at runtime the dispatch has a concrete destination, which is either
# a system-DLL leaf (no game write -> the block was a false blocker) or an
# image function (resolvable). Output feeds the snapshot/restore A/B lane.
#
# Usage:
#   py -3.12 re/frida/stalker_write_surface_batch.py 0xRVA 0xRVA ... \
#       [--dwell 18] [--window 3000] [--round 130]
#   py -3.12 re/frida/stalker_write_surface_batch.py --file targets.txt ...
#     (--file reads leading-0x tokens, one per line, '#'/blank ignored)

import json
import os
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import statenav
from run_diff_scenario import drive_to_results
from run_diff import _find_original

MASHED_EXE = _find_original(ROOT)
ORIG = MASHED_EXE.parent
LOG_DIR = ROOT / 'log'

# Arm-ALL-then-wait agent: attach every target before resume, and let each
# capture its FIRST natural call whenever it happens (prerace loading OR
# in-race) across the whole boot->race->dwell window. A single global follow
# lock keeps exactly one Stalker.follow active at a time (a target that fires
# while another is being followed simply catches its next call), so there is no
# concurrent-Stalker hazard even with N listeners live. collect() returns the
# captured map. (The earlier one-target-at-a-time / 2.5s-window design missed
# every prerace mutator, whose firing was already past by the time it armed.)
BATCH_JS = r'''
'use strict';
const IMG = 0x00400000;
const TARGETS = $TARGETS$;
let DELTA = 0;
function abs(r){ return ptr(r + DELTA); }
const captured = Object.create(null);   // '0xrva' -> {writes,indirects,stack_skipped}
let following = false, curW = null, curI = null, curSS = 0;

function xform(iterator){
    let insn;
    while ((insn = iterator.next()) !== null){
        try {
            const ops = insn.operands || [];
            if ((insn.mnemonic === 'call' || insn.mnemonic === 'jmp')
                && ops.length && ops[0].type !== 'imm'){
                const site = insn.address.toUInt32(); const op = ops[0];
                iterator.putCallout(function(ctx){
                    if (!curI) return;
                    let dest = -1;
                    try {
                        if (op.type === 'reg') dest = ctx[op.value].toUInt32();
                        else if (op.type === 'mem'){
                            const mv = op.value; let ea = mv.disp >>> 0;
                            if (mv.base)  ea = (ea + ctx[mv.base].toUInt32()) >>> 0;
                            if (mv.index) ea = (ea + ctx[mv.index].toUInt32()*mv.scale) >>> 0;
                            dest = ptr(ea).readU32();
                        }
                    } catch(e){}
                    if (dest !== -1) curI['0x'+site.toString(16)] = '0x'+(dest>>>0).toString(16);
                });
            }
            for (let k = 0; k < ops.length; k++){
                const op = ops[k];
                if (op.type === 'mem' && op.access && op.access.indexOf('w') !== -1){
                    const mv = op.value; const size = op.size|0;
                    const ia = insn.address.toUInt32();
                    iterator.putCallout(function(ctx){
                        if (!curW) return;
                        try {
                            let ea = mv.disp >>> 0;
                            if (mv.base)  ea = (ea + ctx[mv.base].toUInt32()) >>> 0;
                            if (mv.index) ea = (ea + ctx[mv.index].toUInt32()*mv.scale) >>> 0;
                            const d = ea - ctx.esp.toUInt32();
                            if (d > -0x100000 && d < 0x100000){ curSS++; return; }
                            const key = '0x'+ea.toString(16);
                            if (!curW[key]) curW[key] = { size:size, n:0, insn:'0x'+ia.toString(16) };
                            curW[key].n++;
                        } catch(e){}
                    });
                    break;
                }
            }
        } catch(e){}
        iterator.keep();
    }
}

rpc.exports = {
    init: function(){
        const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
        DELTA = m.base.toUInt32() - IMG;
        TARGETS.forEach(function(rva){
            const key = '0x'+rva.toString(16);
            Interceptor.attach(abs(rva), {
                onEnter: function(){
                    if (captured[key] || following) return;   // done, or someone follows
                    following = true; this.mine = true;
                    this.tid = Process.getCurrentThreadId();
                    curW = Object.create(null); curI = Object.create(null); curSS = 0;
                    Stalker.follow(this.tid, { transform: xform });
                },
                onLeave: function(){
                    if (!this.mine) return;
                    Stalker.unfollow(this.tid); Stalker.flush();
                    captured[key] = { writes: curW, indirects: curI, stack_skipped: curSS };
                    curW = null; curI = null; following = false;
                }
            });
        });
        return DELTA;
    },
    collect: function(){ return captured; }
};
'''


def _flag(name, default=None, cast=str):
    return cast(sys.argv[sys.argv.index(name) + 1]) if name in sys.argv else default


def load_targets():
    if '--file' in sys.argv:
        p = Path(sys.argv[sys.argv.index('--file') + 1])
        toks = []
        for line in p.read_text().splitlines():
            line = line.strip()
            if line and not line.startswith('#'):
                toks.append(line.split()[0])
        return toks
    return [a for a in sys.argv[1:] if a.startswith('0x')]


def classify(rva, writes, inds):
    img = {k: v for k, v in writes.items() if 0x00400000 <= int(k, 16) < 0x00b60000}
    heap = {k: v for k, v in writes.items() if k not in img}
    sys_ind = {s: d for s, d in inds.items() if int(d, 16) >= 0x60000000}
    img_ind = {s: d for s, d in inds.items() if int(d, 16) < 0x60000000}
    # Reachable = every write EA is an image global (snapshot directly) and every
    # indirect either lands in a system DLL (no game write) or an image fn.
    reachable = (len(heap) == 0)
    return {'img_writes': len(img), 'heap_writes': len(heap),
            'sys_indirect': len(sys_ind), 'img_indirect': len(img_ind),
            'reachable_now': reachable}


def main():
    targets = load_targets()
    if not targets:
        sys.exit('usage: stalker_write_surface_batch.py 0xRVA ... | --file targets.txt '
                 '[--dwell 18] [--window 3000]')
    dwell = _flag('--dwell', 18.0, float)
    window = _flag('--window', 3000, int)
    round_secs = _flag('--round', 130, int)
    shotdir = 'verify/stalker_batch'
    (ROOT / shotdir).mkdir(parents=True, exist_ok=True)

    env = dict(os.environ)
    env['MASHED_RE_NO_AUTO_HOOK'] = '1'
    dev = frida.get_local_device()
    pid = dev.spawn(str(MASHED_EXE), cwd=str(ORIG), env=env)
    print(f'  spawned pid={pid} (kills ONLY this pid); {len(targets)} targets, one boot')
    sess = dev.attach(pid)
    nav_scr = sess.create_script(statenav.AGENT)
    nav_scr.on('message', lambda m, d: None)
    nav_scr.load(); nav_scr.exports_sync.init()

    # arm ALL targets BEFORE resume, so prerace mutators are caught too.
    tlist = '[' + ','.join(str(int(r, 16)) for r in targets) + ']'
    cap_scr = sess.create_script(BATCH_JS.replace('$TARGETS$', tlist))
    cap_scr.on('message', lambda m, d: None)
    cap_scr.load(); cap_scr.exports_sync.init()

    def sentinel_nonzero():
        return any(isinstance(nav_scr.exports_sync.peek(f'0x{0x006c71d8 + w*4:08x}'), int)
                   and nav_scr.exports_sync.peek(f'0x{0x006c71d8 + w*4:08x}') != 0
                   for w in range(8))

    results = {}
    try:
        dev.resume(pid)
        nav = statenav.Nav(nav_scr, pid)
        reached = drive_to_results(nav, nav_scr, pid, shotdir, 'race',
                                   round_secs, sentinel_nonzero)
        if nav.alive() and reached and dwell > 0:
            print(f'  in race — dwelling {dwell:.0f}s (driving) so race-gated targets fire...')
            end = time.time() + dwell
            while time.time() < end and nav.alive():
                nav.press(4); time.sleep(0.4)
        # collect whatever fired during boot->race->dwell (works even after the
        # race self-exits, as long as the process is alive to answer the RPC).
        cap = cap_scr.exports_sync.collect() if nav.alive() else {}
        for rva in targets:
            key = f'0x{int(rva, 16):x}'
            r = cap.get(key)
            if r:
                cls = classify(rva, r.get('writes', {}), r.get('indirects', {}))
                results[rva] = {'status': 'captured', **cls,
                                'writes': r.get('writes', {}),
                                'indirects': r.get('indirects', {})}
                tag = 'REACHABLE' if cls['reachable_now'] else 'heap-relative'
                print(f'  {rva} CAPTURED  img_w={cls["img_writes"]} '
                      f'heap_w={cls["heap_writes"]} '
                      f'ind(sys/img)={cls["sys_indirect"]}/{cls["img_indirect"]}  {tag}')
            else:
                results[rva] = {'status': 'not-fired'}
                print(f'  {rva} NOT-FIRED')
    finally:
        try: dev.kill(pid)
        except Exception: pass
        print(f'  killed pid={pid}')

    cap = [r for r in results.values() if r.get('status') == 'captured']
    reach = [r for r in cap if r.get('reachable_now')]
    print(f'\n=== BATCH STALKER VERDICT ===')
    print(f'  targets: {len(targets)}   captured: {len(cap)}   '
          f'REACHABLE-now (all writes = image globals): {len(reach)}')
    print(f'  timeouts/not-fired: {len(targets) - len(cap)}')
    out = LOG_DIR / 'stalker_ws_batch.json'
    out.write_text(json.dumps(results, indent=1))
    print(f'  report -> {out}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
