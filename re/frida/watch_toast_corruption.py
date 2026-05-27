# Catch the stray write that corrupts the Toast title string at 0x005f6560.
#
# Context: the ~94s MASHED crash is strlen(NULL) reached because
# strrchr(0x005f6560 "Toast..", 'T') returns NULL — i.e. the Toast string in
# writable .data is corrupted by a stray write/overflow (it has ZERO intentional
# writers; see re/analysis/menu_crash_scoping/REPORT.md). This arms a hardware
# WRITE watchpoint on 0x005f6560 (4 bytes) on every thread; the first write there
# is the corruptor. On trigger we dump the writer EIP + regs + the MASHED-range
# return chain so the culprit function + its callers are identified.
#
# HW watchpoint (debug register) — NOT Interceptor, NOT page-guard — so it does
# not trap the per-frame READS of the Toast string and adds ~no steady overhead.
#
# Usage: run this, then launch original\MASHED.exe normally.
import json
import sys
import time
from pathlib import Path

import frida

try:
    import psutil
except ImportError:
    sys.exit("psutil required: py -3.12 -m pip install psutil")

ROOT     = Path(__file__).resolve().parent.parent.parent
LOG_DIR  = ROOT / 'log'
OUT_FILE = LOG_DIR / 'toast_corruptor.txt'
WATCH    = 0x005f6560

AGENT_JS = r'''
'use strict';
var ADDR  = ptr('0x5f6560');
var MX_LO = 0x400000, MX_HI = 0x995000;          // MASHED.exe .text
function cls(v){ v = v >>> 0; return (v >= MX_LO && v < MX_HI) ? ('MASHED+0x' + (v - MX_LO).toString(16)) : ('0x' + v.toString(16)); }

// Arm a hardware WRITE watchpoint (slot 0, 4 bytes) on every current thread.
var armed = 0, total = 0, errs = [];
try {
    var threads = Process.enumerateThreads();
    total = threads.length;
    threads.forEach(function (t) {
        try { t.setHardwareWatchpoint(0, ADDR, 4, 'w'); armed++; }
        catch (e) { if (errs.length < 4) errs.push('' + e); }
    });
} catch (e) { errs.push('enum: ' + e); }
send({ kind: 'armed', armed: armed, total: total, errs: errs });

var fired = false;
Process.setExceptionHandler(function (details) {
    var ctx = details.context;
    // The final strlen(NULL) AV: report and let it die.
    var isAV = (details.type === 'access-violation');
    // Snapshot the Toast bytes now (was it just corrupted?).
    var toast = '';
    try {
        var b = ADDR.readByteArray(16);
        toast = Array.prototype.map.call(new Uint8Array(b), function (x){ return x.toString(16).padStart(2,'0'); }).join(' ');
    } catch (e) { toast = '??'; }
    // MASHED-range return chain from ESP.
    var code = [];
    try {
        var esp = ptr(ctx.esp.toString());
        for (var i = 0; i < 64; i++) {
            var v = esp.add(i*4).readU32() >>> 0;
            if (v >= MX_LO && v < MX_HI) code.push({ off: i*4, val: cls(v) });
        }
    } catch (e) {}
    send({
        kind: isAV ? 'av' : 'watch',
        type: details.type,
        pc:   ctx.pc.toString(), pc_cl: cls(ctx.pc.toInt32()),
        eax: ctx.eax.toString(), ebx: ctx.ebx.toString(), ecx: ctx.ecx.toString(),
        edx: ctx.edx.toString(), esi: ctx.esi.toString(), edi: ctx.edi.toString(),
        esp: ctx.esp.toString(), ebp: ctx.ebp.toString(),
        mem:   (details.memory && details.memory.address)  ? details.memory.address.toString()  : '?',
        memop: (details.memory && details.memory.operation)? details.memory.operation : '?',
        toast_now: toast,
        code_chain: code,
    });
    if (isAV) return false;       // real crash — let it propagate
    fired = true;
    return true;                  // watchpoint hit — log and continue
});
send({ kind: 'ready' });
'''


def find_pid():
    for p in psutil.process_iter(['name', 'pid']):
        try:
            if p.info['name'] and p.info['name'].lower() == 'mashed.exe':
                return p.info['pid']
        except Exception:
            continue
    return None


def main():
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    print("waiting for MASHED.exe — launch it now")
    deadline = time.time() + 120
    pid = None
    while time.time() < deadline:
        pid = find_pid()
        if pid:
            print(f"  found pid={pid}")
            break
        time.sleep(0.1)
    if not pid:
        sys.exit("timeout waiting for MASHED.exe")

    time.sleep(0.05)
    session = frida.get_local_device().attach(pid)
    print("  attached")

    events = []
    def on_message(message, data):
        if message['type'] == 'error':
            print("  agent error:", message.get('description'))
            return
        p = message.get('payload', {})
        k = p.get('kind')
        if k == 'armed':
            print(f"  watchpoint armed on {p['armed']}/{p['total']} threads; errs={p.get('errs')}")
        elif k == 'ready':
            print("  ready — watching for the write to 0x5f6560")
        elif k == 'watch':
            print("\n=== CORRUPTOR WRITE CAUGHT ===")
            print(f"  writer pc : {p['pc']}  ({p['pc_cl']})")
            print(f"  regs      : eax={p['eax']} ecx={p['ecx']} edx={p['edx']} esi={p['esi']} edi={p['edi']}")
            print(f"  mem/op    : {p['mem']} {p['memop']}")
            print(f"  toast_now : {p['toast_now']}")
            print(f"  callchain : {p['code_chain']}")
            events.append(p)
        elif k == 'av':
            print("\n=== FINAL AV (strlen NULL) ===")
            print(f"  pc={p['pc']} ({p['pc_cl']}) ecx={p['ecx']} mem={p['mem']} {p['memop']}")
            print(f"  toast_now : {p['toast_now']}")
            events.append(p)

    script = session.create_script(AGENT_JS)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 130
    while time.time() < deadline:
        try:
            st = psutil.Process(pid).status()
            nm = psutil.Process(pid).name()
            if nm.lower() != 'mashed.exe':
                break
        except Exception:
            print("  process gone")
            break
        time.sleep(0.3)

    try: session.detach()
    except Exception: pass
    OUT_FILE.write_text(json.dumps(events, indent=2), encoding='utf-8')
    print(f"\nwrote {len(events)} event(s) -> {OUT_FILE}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
