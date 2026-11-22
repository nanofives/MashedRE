# RACE-LOAD-LUA-2 (run 56): one watch on VFS_Open 0x005507b0 (RwCore_005507b0 on the exe) for the COURSE.LUA call.
#
# Per call: the two arguments (name, mode string), the caller, the return value, and the RW file-system manager state the
# body reads (re/match/rwl04/005507b0.c: DAT_007dc75c open flag, DAT_007dc754 device list (next at [0], name at [3]),
# DAT_007dc76c default device, DAT_007dc768 error callback), plus the piz state at that moment (DAT_007d3e48 piz handle
# (0x004b6710), DAT_007d3e4c, DAT_008ad9a8 entry count and the entry table 0x0090dac0 stride 0x80 (0x004b65b0 / 0x004b68f0)).
# Also logs every piz open (0x00495280 name, 0x004b6570 path + return) and piz shutdown (0x004b67a0) before the call.
#
# Two sides, explicit PID only (never "first process by name"):
#   --side orig : runs re/frida/scenario_launch.py (track 0, mode 10, 4 cars = replay_trace.json's scenario), reads the pid
#                 it prints, attaches a second Frida session to that pid.
#   --side exe  : runs verify/savelink_20261023/run_arms.py <arm>, reads the pid it prints, attaches; hooks are resolved from
#                 the exe's map file (--map).
# Globals are read at their original VAs (the exe keeps the original's globals at their original addresses, L-96 / L-111).
#
#   py -3.12 verify/racelua2_20261122/vfs_watch.py --side orig --out D:/MashedWT/RACE-LOAD-LUA-2_data/vfs_orig.jsonl
#   py -3.12 verify/racelua2_20261122/vfs_watch.py --side exe --arm RO_ROLOADRWCC-vfs --map <map> --out ...
import argparse, json, os, re, subprocess, sys, threading, time
from pathlib import Path
import frida

W = Path(__file__).resolve().parents[2]

JS = r'''
'use strict';
const H = %HOOKS%;          // [{name, addr, kind}] kind: vfs | pizmount | pizopen | pizshut
const DELTA = %DELTA%;
const EXED = %EXED%;      // exe: live module base - the map's preferred base (0x00010000); 0 on the original      // module base - 0x400000 for the original's globals (0 on the exe)
function g(va){ return ptr(va).add(DELTA); }
function rd32(p){ try { return p.readU32(); } catch(e){ return null; } }
function rptr(p){ try { return p.readPointer(); } catch(e){ return null; } }
function cstr(p, n){ try { if (p === null || p.isNull()) return null; return p.readCString(n || 128); } catch(e){ return '<bad ' + p + '>'; } }
function hx(v){ return v === null ? null : ('0x' + (v >>> 0).toString(16)); }
let seq = 0;
function fsState(){
  const s = {};
  s.fs_open_007dc75c = rd32(g(0x007dc75c));
  s.errcb_007dc768 = hx(rd32(g(0x007dc768)));
  const dd = rptr(g(0x007dc76c));
  s.defdev_007dc76c = dd ? dd.toString() : null;
  if (dd && !dd.isNull()) s.defdev_name = cstr(rptr(dd.add(12)), 64);
  const devs = [];
  let n = rptr(g(0x007dc754)), k = 0;
  while (n && !n.isNull() && k < 16) { devs.push({node: n.toString(), name: cstr(rptr(n.add(12)), 64)}); n = rptr(n); k++; }
  s.devlist_007dc754 = devs;
  s.piz_handle_007d3e48 = hx(rd32(g(0x007d3e48)));
  s.piz_007d3e4c = hx(rd32(g(0x007d3e4c)));
  const cnt = rd32(g(0x008ad9a8));
  s.piz_count_008ad9a8 = cnt;
  const ents = [];
  let hit = -1;
  if (cnt !== null && cnt < 4096) {
    for (let i = 0; i < cnt; i++) {
      const e = g(0x0090dac0).add(i * 0x80);
      const nm = cstr(e, 0x78);
      if (i < 4) ents.push({i: i, name: nm, size: rd32(e.add(0x78))});
      if (nm && /course\.lua/i.test(nm)) { hit = i; s.piz_course_entry = {i: i, name: nm, size: rd32(e.add(0x78))}; }
    }
  }
  s.piz_first_entries = ents;
  s.piz_course_index = hit;
  return s;
}
for (const h of H) {
  Interceptor.attach(ptr(h.addr).add(EXED), {
    onEnter(args) {
      if (h.kind === 'fs') { h.n = (h.n || 0) + 1; if (h.n > 4) return;
        this.k = ++seq; this.fsk = 1;
        send({seq: this.k, ev: 'enter', fn: h.name, kind: 'fs', caller: this.returnAddress.toString(), a0: args[0].toString(),
              fs_open: rd32(g(0x007dc75c)), defdev: hx(rd32(g(0x007dc76c))), devhead: hx(rd32(g(0x007dc754)))}); return; }
      if (h.kind === 'count') { h.n = (h.n || 0) + 1;
        if (h.name.indexOf('FrameTimer') === 0 && h.n % 60 === 0) send({ev: 'beat', n: h.n, phase: rd32(g(0x00771968)) & 0xff});
        if (h.n > 3) return; }
      this.k = ++seq;
      const ev = {seq: this.k, ev: 'enter', fn: h.name, kind: h.kind, caller: this.returnAddress.toString(),
                  tid: this.threadId};
      if (h.kind === 'vfs') {
        ev.a0 = args[0].toString(); ev.name = cstr(args[0]); ev.a1 = args[1].toString(); ev.mode = cstr(args[1], 16);
        this.detail = ev.name !== null && /\.lua/i.test(ev.name);
        if (this.detail) ev.state = fsState();
      } else if (h.kind === 'pizmount' || h.kind === 'pizopen') {
        ev.a0 = args[0].toString(); ev.name = cstr(args[0]);
      }
      this.ev = ev;
      send(ev);
    },
    onLeave(rv) {
      if (this.fsk) { send({seq: this.k, ev: 'leave', fn: h.name, ret: rv.toString(), fs_open: rd32(g(0x007dc75c)),
                            defdev: hx(rd32(g(0x007dc76c))), devhead: hx(rd32(g(0x007dc754)))}); return; }
      if (h.kind === 'count' && h.n > 3) return;
      const ev = {seq: this.k, ev: 'leave', fn: h.name, ret: rv.toString()};
      if (this.detail) ev.state = fsState();
      send(ev);
    }
  });
}
send({ev: 'installed', hooks: H, delta: DELTA, exed: EXED});
'''


def map_hooks(mp):
    want = {'005507b0': 'vfs', '00495280': 'pizmount', '004b6570': 'pizopen', '004b6940': 'pizopen', '004b67a0': 'pizshut'}
    out = []
    for l in open(mp, encoding='latin-1'):
        f = l.split()
        if len(f) < 4 or not re.match(r'^[0-9a-fA-F]{4}:[0-9a-fA-F]{8}$', f[0]) or f[3] != 'f':
            continue
        for r, kind in want.items():
            if r in f[1].lower():
                out.append({'name': f[1], 'addr': '0x' + f[2], 'kind': kind})
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--side', choices=['orig', 'exe'], required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--arm', default='RO_ROLOADRWCC-vfs')
    ap.add_argument('--map', default='')
    ap.add_argument('--hold', type=int, default=25)
    ap.add_argument('--fs-rvas', default='', help='orig: comma list of rvas hooked as kind fs (first 4 calls, FS-manager globals)')
    ap.add_argument('--fs-syms', default='', help='exe: comma list of map symbol names hooked as kind fs')
    ap.add_argument('--boot', action='store_true', help='orig only: frida-spawn and watch from the first instruction, no warp')
    a = ap.parse_args()
    if a.side == 'orig':
        hooks = [{'name': 'VFS_Open_005507b0', 'addr': '0x005507b0', 'kind': 'vfs'},
                 {'name': 'PizMount_00495280', 'addr': '0x00495280', 'kind': 'pizmount'},
                 {'name': 'PizOpen_004b6570', 'addr': '0x004b6570', 'kind': 'pizopen'},
                 {'name': 'PizShutdown_004b67a0', 'addr': '0x004b67a0', 'kind': 'pizshut'},
                 {'name': 'FrameTimer_00493390', 'addr': '0x00493390', 'kind': 'count'},
                 {'name': 'CourseLoad_0040d270', 'addr': '0x0040d270', 'kind': 'count'},
                 {'name': 'ScriptOpen_0047b8d0', 'addr': '0x0047b8d0', 'kind': 'count'},
                 {'name': 'Thunk_004b5360', 'addr': '0x004b5360', 'kind': 'count'},
                 {'name': 'CourseLoadCur_0040d440', 'addr': '0x0040d440', 'kind': 'count'},
                 {'name': 'ScriptRun_0047b9b0', 'addr': '0x0047b9b0', 'kind': 'count'},
                 {'name': 'LuaInit_0047b860', 'addr': '0x0047b860', 'kind': 'count'},
                 {'name': 'Registrar_00440bc0', 'addr': '0x00440bc0', 'kind': 'count'},
                 {'name': 'RegisterCmd_0047b980', 'addr': '0x0047b980', 'kind': 'count'},
                 {'name': 'LuaDobuffer_004b7a70', 'addr': '0x004b7a70', 'kind': 'count'},
                 {'name': 'PizOpenParse_004b6940', 'addr': '0x004b6940', 'kind': 'count'},
                 {'name': 'Seek_005509b0', 'addr': '0x005509b0', 'kind': 'count'},
                 {'name': 'Script_0047b8a0', 'addr': '0x0047b8a0', 'kind': 'count'}]
        cmd = [sys.executable, str(W / 're/frida/scenario_launch.py'), '--track', '0', '--mode', '10', '--cars', '4',
               '--hold', str(a.hold)]
        pidre = re.compile(r'scenario_launch\s+pid=(\d+)')
    else:
        hooks = map_hooks(a.map)
        cmd = [sys.executable, str(W / 'verify/savelink_20261023/run_arms.py'), a.arm]
        pidre = re.compile(r'^=== \S+ pid=(\d+)')
    for r in [x for x in a.fs_rvas.split(',') if x]:
        hooks.append({'name': 'fs_' + r, 'addr': '0x' + r, 'kind': 'fs'})
    if a.fs_syms:
        want = set(a.fs_syms.split(','))
        for l in open(a.map, encoding='latin-1'):
            f = l.split()
            if len(f) >= 4 and f[1] in want and f[3] == 'f':
                hooks.append({'name': f[1], 'addr': '0x' + f[2], 'kind': 'fs'})
    print('hooks', json.dumps(hooks), flush=True)
    if not any(h['kind'] == 'vfs' for h in hooks):
        sys.exit('no VFS_Open hook resolved')
    outf = open(a.out, 'w', encoding='utf-8')
    lock = threading.Lock()

    def on_msg(m, d):
        rec = m.get('payload') if m.get('type') == 'send' else m
        with lock:
            outf.write(json.dumps(rec) + '\n'); outf.flush()

    if a.boot:
        # spawn suspended (hooks in place before the first instruction), boot to the menu, watch --hold s, kill own pid
        exe = Path(os.environ.get('MASHED_ROOT', W)) / 'original' / 'MASHED.exe'
        env = dict(os.environ, MASHED_RE_NO_AUTO_HOOK='1', MASHED_MUTE='1', MASHED_FPS_CAP='60')
        pid = frida.spawn([str(exe)], cwd=str(exe.parent), env=env)
        sess = frida.attach(pid)
        delta = "(function(){const m=Process.findModuleByName('MASHED.exe');return m?m.base.sub(ptr(0x400000)).toInt32():0;})()"
        sc = sess.create_script(JS.replace('%HOOKS%', json.dumps(hooks)).replace('%DELTA%', delta).replace('%EXED%', '0'))
        sc.on('message', on_msg); sc.load(); frida.resume(pid)
        print('SPAWNED pid', pid, flush=True)
        time.sleep(a.hold)
        try:
            frida.kill(pid)
        except Exception as ex:
            print('kill', ex)
        print('killed own pid', pid, flush=True)
        outf.close()
        return
    p = subprocess.Popen(cmd, cwd=str(W), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
    sess = None
    for line in p.stdout:
        sys.stdout.write(line); sys.stdout.flush()
        m = pidre.search(line)
        if m and sess is None:
            pid = int(m.group(1))
            for _ in range(400):
                try:
                    sess = frida.attach(pid); break
                except Exception as ex:
                    err = ex; time.sleep(0.05)
            if sess is None:
                print('ATTACH FAILED pid', pid, err, flush=True); continue
            delta = ("(function(){const m=Process.findModuleByName('MASHED.exe');return m?m.base.sub(ptr(0x400000)).toInt32():0;})()"
                     if a.side == 'orig' else '0')
            exed = ("(function(){const m=Process.enumerateModules()[0];return m.base.sub(ptr(0x10000)).toInt32();})()" if a.side == 'exe' else '0')
            js = JS.replace('%HOOKS%', json.dumps(hooks)).replace('%DELTA%', delta).replace('%EXED%', exed)
            sc = sess.create_script(js); sc.on('message', on_msg); sc.load()
            print('ATTACHED pid', pid, flush=True)
    rc = p.wait()
    print('child rc', rc, flush=True)
    outf.close()


if __name__ == '__main__':
    main()
