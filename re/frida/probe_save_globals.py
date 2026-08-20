# Runtime observation of the two open save-subsystem uncertainties.
#
#   U-3558  identity of the 12 bytes stride-gathered from 0x007F105C (stride
#           0x4C) into DAT_007F0F54[0..11]. SerializeToBuffer (0x00404EE0)
#           takes the LOW BYTE of the first dword of each of 12 records;
#           DeserializeFromBuffer (0x00404E80) scatters them back.
#   U-3560  what *DAT_008A94A8 points at -- a 0x2443C (150,076) byte block,
#           copied as 0x928F dwords when the pointer is non-null.
#
# Both uncertainties record a Ghidra MCP call as their resolution path. This
# account has no Ghidra MCP, so this probe gathers BEHAVIOURAL evidence
# instead: what the bytes hold, when they change, and who writes the pointer.
#
# Method: spawn SUSPENDED so the pointer-write at init cannot be missed, arm a
# hardware write-watchpoint on 0x008A94A8 before resuming, then snapshot both
# regions at labelled navigation checkpoints.
#
# Usage:
#   py -3.12 re\frida\probe_save_globals.py [--wait 120] [--depth 3]
#
# Writes log/probe_save_globals.json (raw records) and prints a digest.
# Spawns its own MASHED and kills ONLY that pid.
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
OUT = ROOT / 'log' / 'probe_save_globals.json'
sys.path.insert(0, str(ROOT / 're' / 'frida'))
sys.path.insert(0, str(ROOT / 're' / 'orchestrator'))

import statenav
from run_diff import _find_original
from mashed_lock import MashedLock

MASHED_EXE = _find_original(ROOT)
ORIG = MASHED_EXE.parent

STRIDE_BASE = 0x007F105C
STRIDE_STEP = 0x4C
STRIDE_COUNT = 12
PACKED_BYTES = 0x007F0F54
PROFILE_PTR = 0x008A94A8
PROFILE_SIZE_GLOBAL = 0x008A94AC   # size operand pushed to the allocator at 0x0041144F
PROFILE_DWORDS = 0x928F
SERIALIZE = 0x00404EE0
DESERIALIZE = 0x00404E80

AGENT = r'''
'use strict';
var LO = 0x401000, HI = 0x995000;
var STRIDE_BASE = %d, STRIDE_STEP = %d, STRIDE_COUNT = %d;
var PACKED_BYTES = %d, PROFILE_PTR = %d, PROFILE_SIZE_GLOBAL = %d, PROFILE_BYTES = %d * 4;
var SERIALIZE = %d, DESERIALIZE = %d;

function hx(v){ return '0x' + (v >>> 0).toString(16); }
function rdU32(a){ try { return ptr(a).readU32() >>> 0; } catch (e) { return null; } }

function hexdump_at(addr, len){
  try { return ptr(addr).readByteArray(len); } catch (e) { return null; }
}

// Map which 1 KB chunks of the profile block are non-zero, so we can describe
// the live regions mechanically without dumping 150 KB.
function nonzeroMap(base, total){
  var CH = 1024, out = [], nz = 0;
  for (var off = 0; off < total; off += CH) {
    var n = Math.min(CH, total - off), any = false;
    try {
      var b = new Uint8Array(ptr(base).add(off).readByteArray(n));
      for (var i = 0; i < n; i++) { if (b[i] !== 0) { any = true; break; } }
    } catch (e) { out.push([off, 'unreadable']); continue; }
    if (any) { out.push([off, n]); nz++; }
  }
  return { chunks: out, nonzeroChunks: nz, totalChunks: Math.ceil(total / CH) };
}

function snapshot(label){
  var rec = { label: label, t: Date.now() };

  // --- U-3558: the 12 stride records ---
  var first = [], low = [], recs = {};
  for (var i = 0; i < STRIDE_COUNT; i++) {
    var a = STRIDE_BASE + i * STRIDE_STEP;
    var v = rdU32(a);
    first.push(v === null ? null : hx(v));
    low.push(v === null ? null : (v & 0xff));
  }
  rec.strideFirstDword = first;
  rec.strideLowByte = low;
  // full record dump for indices 0 and 1 so the byte's position is documented
  for (var j = 0; j < 2; j++) {
    var d = hexdump_at(STRIDE_BASE + j * STRIDE_STEP, STRIDE_STEP);
    recs['rec' + j] = d ? Array.prototype.slice.call(new Uint8Array(d)) : null;
  }
  rec.strideRecords = recs;

  // Array A base is 0x007F1038 (from 0x0049653B lea esi,[ebp+0x7f1038]), stride
  // 0x4C, and array B at 0x007F14F8 -- so A holds (0x7f14f8-0x7f1038)/0x4c = 16
  // entries. The save persists only the first 12. Dump the gathered field
  // (+0x24) for all 16, plus the parallel array and the active-slot counter.
  var A = 0x007F1038, B = 0x007F14F8, C = 0x007E95C0;
  var a24 = [], b24 = [], c13c = [];
  for (var q = 0; q < 16; q++) {
    a24.push(rdU32(A + q * 0x4C + 0x24));
    b24.push(rdU32(B + q * 0x4C + 0x24));
    c13c.push(rdU32(C + q * 0x200 + 0x13C));
  }
  rec.arrayA_at24 = a24;
  rec.arrayB_at24 = b24;
  rec.arrayC_at13c = c13c;
  rec.activeCounter = rdU32(0x00772FFC);

  // the packed destination
  var pk = hexdump_at(PACKED_BYTES, STRIDE_COUNT);
  rec.packedBytes = pk ? Array.prototype.slice.call(new Uint8Array(pk)) : null;

  // --- U-3560: the profile pointer ---
  var p = rdU32(PROFILE_PTR);
  rec.profilePtr = p === null ? null : hx(p);
  rec.profileNull = (p === 0);
  var sz = rdU32(PROFILE_SIZE_GLOBAL);
  rec.profileSizeGlobal = sz === null ? null : hx(sz);
  if (p && p !== 0) {
    rec.profileMap = nonzeroMap(p, PROFILE_BYTES);
    var head = hexdump_at(p, 128);
    rec.profileHead = head ? Array.prototype.slice.call(new Uint8Array(head)) : null;
  }
  return rec;
}

rpc.exports = {
  snap: function (label) { return snapshot(label); },
  ptr: function () { var v = rdU32(PROFILE_PTR); return v === null ? null : hx(v); }
};

// --- watch the pointer slot so we catch WHO populates it ---
function btOf(esp){
  var bt = [];
  try {
    for (var k = 0; k < 32 && bt.length < 12; k++) {
      var x = esp.add(k * 4).readU32() >>> 0;
      if (x >= LO && x < HI) bt.push(hx(x));
    }
  } catch (e) {}
  return bt;
}

// Record index 1: the scatter should write 1 here (packed[1]==1 in the shipped
// save) yet the live value is observed as 0 after the load -- so watch it to
// find every writer and their order.
var REC1 = STRIDE_BASE + STRIDE_STEP;
var wpHits = 0, WP_CAP = 40, capped = false;
var armed = [];
Process.enumerateThreads().forEach(function (t) {
  try { t.setHardwareWatchpoint(0, ptr(PROFILE_PTR), 4, 'w'); armed.push(t.id); }
  catch (e) { send({ kind: 'armerr', tid: t.id, err: e.message }); }
  try { t.setHardwareWatchpoint(1, ptr(REC1), 4, 'w'); }
  catch (e) { send({ kind: 'armerr', tid: t.id, err: 'rec1: ' + e.message }); }
});
send({ kind: 'armed', tids: armed });

Process.setExceptionHandler(function (d) {
  var c = d.context;
  if (d.type === 'breakpoint' || d.type === 'single-step') {
    // 0x00496568 writes the record array continuously (hot). Frida watchpoints
    // on a hot path destabilise Mashed in seconds, so disarm slot 1 once we
    // have enough distinct writers -- we only need identity, not every hit.
    wpHits++;
    if (wpHits > WP_CAP) {
      Process.enumerateThreads().forEach(function (t) {
        try { t.unsetHardwareWatchpoint(1); } catch (e) {}
      });
      if (!capped) { capped = true; send({ kind: 'wpcapped', hits: wpHits }); }
      return true;
    }
    send({ kind: 'ptrwrite', pc: c.pc.toString(),
           val: hx(rdU32(PROFILE_PTR) || 0),
           rec1: hx(rdU32(REC1) || 0),
           esp: c.esp.toString(), eax: c.eax.toString(), bt: btOf(c.esp) });
    return true;
  }
  return false;
});
// The two save entry points are COLD (a handful of calls per session at most),
// so Interceptor here is safe -- the hot-path rule does not apply.
var mod = Process.findModuleByName('MASHED.exe');
var slide = mod.base.sub(ptr(0x400000));
function hookSave(rva, name){
  try {
    Interceptor.attach(mod.base.add(ptr(rva).sub(0x400000)), {
      onEnter: function (args) {
        send({ kind: 'savecall', fn: name, rva: hx(rva),
               ret: this.returnAddress.sub(slide).toString(),
               ptrNow: hx(rdU32(PROFILE_PTR) || 0) });
      }
    });
  } catch (e) { send({ kind: 'hookerr', fn: name, err: e.message }); }
}
hookSave(SERIALIZE, 'SerializeToBuffer');
hookSave(DESERIALIZE, 'DeserializeFromBuffer');

send({ kind: 'ready' });
''' % (STRIDE_BASE, STRIDE_STEP, STRIDE_COUNT, PACKED_BYTES, PROFILE_PTR,
       PROFILE_SIZE_GLOBAL, PROFILE_DWORDS, SERIALIZE, DESERIALIZE)


def main():
    wait_s = int(sys.argv[sys.argv.index('--wait') + 1]) if '--wait' in sys.argv else 120
    want_depth = int(sys.argv[sys.argv.index('--depth') + 1]) if '--depth' in sys.argv else 3

    env = dict(os.environ)
    env['MASHED_RE_NO_AUTO_HOOK'] = '1'      # stock binary; we only observe
    env.setdefault('MASHED_WIN_POS', 'left-bl')

    lock = MashedLock('probe_save_globals')
    lock.acquire()

    dev = frida.get_local_device()
    pid = dev.spawn(str(MASHED_EXE), cwd=str(ORIG), env=env)
    print(f'  spawned pid={pid} (this session kills ONLY this pid)')

    events, snaps = [], []
    sess = dev.attach(pid)

    def on_msg(m, _):
        if m['type'] == 'error':
            print('  AGENT ERR:', m.get('description')); return
        p = m.get('payload', {})
        k = p.get('kind')
        if k == 'armed':
            print('  watchpoint armed on tids:', p['tids'])
        elif k == 'armerr':
            print(f"  ARM ERR tid={p['tid']}: {p['err']}")
        elif k == 'ready':
            print('  agent ready (spawn-suspended, pre-resume)')
        elif k == 'ptrwrite':
            print(f"  WP-HIT pc={p['pc']} ptr={p['val']} rec1={p.get('rec1')} eax={p['eax']} bt={p['bt']}")
            events.append(p)
        elif k == 'savecall':
            print(f"  SAVE-CALL {p['fn']} @{p['rva']} ret={p['ret']} ptr={p['ptrNow']}")
            events.append(p)
        elif k == 'wpcapped':
            print(f"  rec1 watchpoint DISARMED after {p['hits']} hits (hot writer)")
        elif k == 'hookerr':
            print(f"  HOOK ERR {p['fn']}: {p['err']}")

    scr = sess.create_script(AGENT)
    scr.on('message', on_msg)
    scr.load()

    # snapshot BEFORE resume: the pristine pre-init state
    try:
        snaps.append(scr.exports_sync.snap('t0_suspended_pre_resume'))
        print('  snap: t0_suspended_pre_resume')
    except Exception as e:
        print('  pre-resume snap failed:', e)

    nav_scr = sess.create_script(statenav.AGENT)
    nav_scr.on('message', lambda m, d: None)
    nav_scr.load()
    nav_scr.exports_sync.init()

    def snap(label):
        try:
            r = scr.exports_sync.snap(label)
            snaps.append(r)
            print(f"  snap: {label}  ptr={r.get('profilePtr')} low={r.get('strideLowByte')}")
            return r
        except Exception as e:
            print(f'  snap {label} failed: {e}')
            return None

    t0 = time.time()
    try:
        dev.resume(pid)
        nav = statenav.Nav(nav_scr, pid)

        # wait for the frontend to come up, then sample at each depth we reach
        nav.wait(lambda: nav.depth() >= 1, 45.0, 'reach menu')
        snap('t1_main_menu')

        if nav.confirm_to_depth(2, tries=6):
            snap('t2_depth2')
        if nav.advance_past_load_modal(target=want_depth):
            snap(f't3_depth{want_depth}')

        # push deeper toward an actual race: the profile block and the stride
        # records may only populate once career/race state exists.
        if '--race' in sys.argv:
            for d in range(want_depth + 1, want_depth + 5):
                if not nav.alive():
                    break
                if nav.confirm_to_depth(d, tries=4):
                    snap(f't3b_depth{d}')
                else:
                    print(f'  could not reach depth {d} (at {nav.depth()})')
                    break

        # hold a while so any later writer shows up
        end = min(t0 + wait_s, time.time() + 25)
        while time.time() < end and nav.alive():
            time.sleep(0.5)
        snap('t4_hold_end')
    finally:
        try: dev.kill(pid)
        except Exception: pass
        print(f'  killed pid={pid}')
        lock.release()

    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps({'snaps': snaps, 'ptrWrites': events}, indent=2), encoding='utf-8')
    print('  wrote', OUT)

    # ---- digest ----
    print('\n=== U-3558: stride low bytes per checkpoint ===')
    for s in snaps:
        print(f"  {s['label']:<28} low={s.get('strideLowByte')}  packed={s.get('packedBytes')}")
    print('\n=== U-3560: profile pointer per checkpoint ===')
    for s in snaps:
        m = s.get('profileMap')
        extra = f" nonzero={m['nonzeroChunks']}/{m['totalChunks']} KB-chunks" if m else ''
        print(f"  {s['label']:<28} ptr={s.get('profilePtr')}{extra}")
    print(f"\n  pointer writes observed: {len(events)}")


if __name__ == '__main__':
    main()
