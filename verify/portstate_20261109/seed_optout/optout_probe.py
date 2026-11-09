"""optout_probe.py --exe <mashed_re.exe> --map <mashed_re.map> --root <root> --cwd <dir> --out <json> [--env K=V ...]
[--limit 60]

PORT-SEED-OPTOUT arm 1 (PREREG_SEED_OPTOUT.md section 2). Spawns the exe (MASHED_* cleared, MASHED_ROOT=<root>, the
--env pairs), attaches Frida before resume and logs:
  ConfigLoad / ConfigSave / ConfigLogDebug entries (matched bodies, Match/PortSave01_Config.cpp),
  _fsopen entries: the file name, the mode's first 4 bytes, the caller,
  UCRT _invalid_parameter / _invalid_parameter_internal / _invalid_parameter_noinfo / _invoke_watson /
  __acrt_call_reportfault entries with a backtrace (symbolised from the map).
Then waits for the process to exit (or --limit seconds) and records the exit code (GetExitCodeProcess). Spawned pid only.
Run inside test-slot and mashed_lock.py run.
"""
import argparse, ctypes, json, os, time
from pathlib import Path
import frida

ap = argparse.ArgumentParser()
ap.add_argument('--exe', required=True)
ap.add_argument('--map', required=True)
ap.add_argument('--root', required=True)
ap.add_argument('--cwd', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--env', action='append', default=[])
ap.add_argument('--limit', type=float, default=60)
a = ap.parse_args()

sym, PREF = {}, 0x00400000
for l in Path(a.map).read_text(errors='replace').splitlines():
    p = l.split()
    if 'Preferred load address is' in l:
        PREF = int(l.split()[-1], 16)
    if len(p) >= 4 and ':' in p[0] and len(p[2]) == 8:
        try:
            sym.setdefault(p[1], int(p[2], 16))
        except ValueError:
            pass
WANT = ['_ConfigLoad', '_ConfigSave', '_ConfigLogDebug', '__fsopen', '__invalid_parameter',
        '__invalid_parameter_internal', '__invalid_parameter_noinfo', '__invoke_watson', '___acrt_call_reportfault']
targets = {n: sym[n] - PREF for n in WANT if n in sym}
funcs = sorted((v - PREF, k) for k, v in sym.items())

AGENT = r'''
'use strict';
rpc.exports = { arm: function(mod, t) {
  const m = Process.findModuleByName(mod); const out = [];
  for (const n in t) {
    const p = m.base.add(t[n]);
    Interceptor.attach(p, { onEnter: function(args) {
      const ev = {fn: n, ret: this.returnAddress.sub(m.base).toInt32()};
      if (n === '__fsopen') {
        try { ev.name = args[0].readCString(); } catch (e) { ev.name = String(e); }
        try { ev.mode = Array.from(new Uint8Array(args[1].readByteArray(4))); } catch (e) { ev.mode = String(e); }
        ev.mode_ptr = args[1].toString();
      }
      if (n.indexOf('invalid') >= 0 || n.indexOf('watson') >= 0 || n.indexOf('reportfault') >= 0)
        ev.bt = Thread.backtrace(this.context, Backtracer.ACCURATE).map(x => x.sub(m.base).toInt32());
      send(ev);
    }});
    out.push(n);
  }
  return out;
}};
'''


def name_of(rva):
    lo, hi, best = 0, len(funcs) - 1, None
    while lo <= hi:
        mid = (lo + hi) // 2
        if funcs[mid][0] <= rva:
            best = funcs[mid]; lo = mid + 1
        else:
            hi = mid - 1
    return f'{best[1]}+0x{rva - best[0]:x}' if best else hex(rva)


def main():
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env['MASHED_ROOT'] = a.root
    for kv in a.env:
        k, v = kv.split('=', 1)
        env[k] = v
    Path(a.cwd).mkdir(parents=True, exist_ok=True)
    res = {'argv': [a.exe] + a.env, 'env': {k: v for k, v in env.items() if k.startswith('MASHED_')}, 'events': []}
    dev = frida.get_local_device()
    pid = dev.spawn(a.exe, cwd=a.cwd, env=env)
    res['pid'] = pid
    print('spawned pid', pid, flush=True)
    k32 = ctypes.WinDLL('kernel32', use_last_error=True)
    h = k32.OpenProcess(0x1000 | 0x00100000, False, pid)   # QUERY_LIMITED_INFORMATION | SYNCHRONIZE
    try:
        s = dev.attach(pid)
        sc = s.create_script(AGENT)

        def on(msg, data):
            if msg['type'] == 'send':
                ev = msg['payload']
                ev['ret_sym'] = name_of(ev['ret'])
                if 'bt' in ev:
                    ev['bt_sym'] = [name_of(x) for x in ev['bt']]
                res['events'].append(ev)
                print('EV', json.dumps(ev)[:400], flush=True)
            else:
                print('MSG', msg, flush=True)
        sc.on('message', on)
        sc.load()
        print('armed', sc.exports_sync.arm(Path(a.exe).name, targets), flush=True)
        dev.resume(pid)
        r = k32.WaitForSingleObject(h, int(a.limit * 1000))
        code = ctypes.c_ulong(0)
        k32.GetExitCodeProcess(h, ctypes.byref(code))
        res['exited'] = (r == 0)
        res['exit_code'] = '0x%08x' % code.value
        print('exited', res['exited'], 'code', res['exit_code'], flush=True)
    finally:
        if not res.get('exited'):
            try:
                dev.kill(pid); print('killed own pid', pid, flush=True)
            except Exception as e:
                print('kill', e, flush=True)
        Path(a.out).write_text(json.dumps(res, indent=1))
        print('->', a.out, flush=True)


if __name__ == '__main__':
    main()
