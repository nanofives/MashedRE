"""x87_cw_probe.py --target orig|exe --root <root> --out <json> [--exe <mashed_re.exe> --map <map> --env K=V ...]

PORT-X87-PC24-AUDIT, CW leg (PREREG_X87AUDIT.md section 2). The PORT-STATE-03-FPU arm-1 recipe on every RVA of
x87_inventory.tsv at once: one native onEnter stub per function (no JS on the call path) that reads the x87 control
word with FNSTCW and counts it in a per-function slot [n(0x007f), n(0x027f), n(other), last other]. Counters are
snapshotted at named points of the run, so each window's calls are the difference of two snapshots.

  orig  spawns <root>/original/MASHED.exe (hooks OFF, MASHED_RE_NO_AUTO_HOOK=1), stubs placed before resume, then the
        record_race.py Quick-Battle recipe (re/frida/nav_agent.js): snapshots 'title' (phase 3 reached), 'menu' (the
        race-confirm press), 'race' (after --race-s seconds of race).
  exe   spawns --exe with MASHED_ROOT=<root> and the --env pairs, stubs on the symbols of x87_names.tsv resolved in
        --map; snapshot every 5 s until the process exits or --limit.

Spawned pid only, killed by pid. Run under test-slot (outside) and mashed_lock.py run (inside).
"""
import argparse, json, os, sys, time
from pathlib import Path
import frida

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
ap = argparse.ArgumentParser()
ap.add_argument('--target', choices=['orig', 'exe'], required=True)
ap.add_argument('--root', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--exe', default='')
ap.add_argument('--map', default='')
ap.add_argument('--env', action='append', default=[])
ap.add_argument('--limit', type=float, default=300)
ap.add_argument('--race-s', type=float, default=40)
ap.add_argument('--skip', default='', help='comma list of RVAs not to stub')
a = ap.parse_args()

INV = [l.split('\t') for l in (HERE / 'x87_inventory.tsv').read_text().splitlines()[1:]]
NAMES = {(p[0], p[1]): p[2] for p in (l.split('\t') for l in (HERE / 'x87_names.tsv').read_text().splitlines()) if len(p) >= 3}
skip = {s.lower() for s in a.skip.split(',') if s}

targets = []   # (key, address in the module's preferred image)
if a.target == 'orig':
    seen = set()
    for tu, sect, src, rva, how in INV:
        if rva and rva not in seen and rva not in skip:
            seen.add(rva); targets.append((rva, int(rva, 16)))
    PREF = 0x00400000
    MOD = 'MASHED.exe'
else:
    sym = {}
    for l in Path(a.map).read_text(errors='replace').splitlines():
        p = l.split()
        if len(p) >= 4 and ':' in p[0] and len(p[2]) == 8:
            try:
                sym.setdefault(p[1], int(p[2], 16))
            except ValueError:
                pass
        if 'Preferred load address is' in l:
            PREF = int(l.split()[-1], 16)
    seen = set()
    for tu, sect, src, rva, how in INV:
        if not rva:
            continue
        nm = NAMES.get((tu, rva)) or 'FUN_' + rva[2:]
        va = sym.get('_' + nm) or sym.get(nm)
        if va and va not in seen and rva not in skip:
            seen.add(va); targets.append((rva + ':' + nm, va))
    MOD = Path(a.exe).name

AGENT = r'''
'use strict';
let BUF=null, KEEP=[], ADDR=[];
function le(n){ n=n>>>0; return [n&0xff,(n>>>8)&0xff,(n>>>16)&0xff,(n>>>24)&0xff]; }
function lep(p){ return le(parseInt(p.toString(),16)); }
rpc.exports={
  arm:function(mod, pref, list){
    const m=Process.findModuleByName(mod)||Process.enumerateModules()[0];
    const delta=m.base.toUInt32()-pref;
    BUF=Memory.alloc(list.length*16); KEEP.push(BUF);
    for(let i=0;i<list.length*4;i++) BUF.add(i*4).writeU32(0);
    const page=Memory.alloc(Math.max(Process.pageSize, list.length*64)); KEEP.push(page);
    Memory.protect(page, Math.max(Process.pageSize, list.length*64), 'rwx');
    const res=[];
    for(let i=0;i<list.length;i++){
      const S=BUF.add(i*16), st=page.add(i*64);
      const code=[].concat([0x83,0xEC,0x04, 0xD9,0x3C,0x24, 0x0F,0xB7,0x04,0x24, 0x83,0xC4,0x04,
        0x3D],le(0x7f),[0x75,0x07, 0xFF,0x05],lep(S),[0xC3,
        0x3D],le(0x27f),[0x75,0x07, 0xFF,0x05],lep(S.add(4)),[0xC3,
        0xFF,0x05],lep(S.add(8)),[0xA3],lep(S.add(12)),[0xC3]);
      st.writeByteArray(code);
      const va=ptr(list[i]+delta);
      try { Interceptor.attach(va,{onEnter:st}); res.push(1); } catch(e){ res.push(String(e)); }
    }
    Interceptor.flush();
    return [delta, res];
  },
  snap:function(n){ const r=[]; for(let i=0;i<n;i++){ const p=BUF.add(i*16); r.push([p.readU32(),p.add(4).readU32(),p.add(8).readU32(),p.add(12).readU32()]); } return r; }
};
'''


def main():
    res = {'target': a.target, 'targets': [t[0] for t in targets], 'snaps': [], 'argv': sys.argv}
    dev = frida.get_local_device()
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    if a.target == 'orig':
        exe = Path(a.root) / 'original' / 'MASHED.exe'
        cwd = exe.parent
        env.update({'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_MUTE': '1'})
    else:
        exe = Path(a.exe)
        cwd = Path(a.root)
        env['MASHED_ROOT'] = str(Path(a.root))
        for kv in a.env:
            k, v = kv.split('=', 1) if '=' in kv else (kv, '1')
            env[k] = v
    res['exe'] = str(exe)
    res['env'] = {k: v for k, v in env.items() if k.startswith('MASHED_')}
    pid = dev.spawn(str(exe), cwd=str(cwd), env=env)
    print(f'spawned pid={pid} {exe}', flush=True)
    res['pid'] = pid
    n = len(targets)
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT); scr.load(); E = scr.exports_sync
        delta, att = E.arm(MOD, PREF, [t[1] for t in targets])
        res['delta'] = delta
        res['attach'] = att
        bad = [(targets[i][0], x) for i, x in enumerate(att) if x != 1]
        print(f'stubs {n - len(bad)}/{n} attached; failures {bad}', flush=True)
        t0 = time.time()

        def snap(tag):
            res['snaps'].append({'tag': tag, 't': round(time.time() - t0, 1), 'c': E.snap(n)})
            tot = [sum(s[i] for s in res['snaps'][-1]['c']) for i in range(3)]
            print(f'snap {tag} t={res["snaps"][-1]["t"]} calls 0x7f={tot[0]} 0x27f={tot[1]} other={tot[2]}', flush=True)

        if a.target == 'orig':
            nav = sess.create_script((REPO / 're' / 'frida' / 'nav_agent.js').read_text(encoding='utf-8'))
            nav.load(); N = nav.exports_sync; N.init()
            dev.resume(pid)

            def wait(pred, t):
                end = time.time() + t
                while time.time() < end:
                    if pred():
                        return True
                    time.sleep(0.1)
                return False

            def press(c, ms=180):
                N.press(c, ms); time.sleep(ms / 1000.0 + 0.3)

            def confirm_to(target, tries=6):
                for _ in range(tries):
                    if N.depth() >= target:
                        return True
                    press(4)
                    if wait(lambda: N.depth() >= target, 2.0):
                        return True
                return N.depth() >= target

            if not wait(lambda: N.phase() == 3 and N.depth() >= 1, 40):
                snap('boot-timeout'); raise SystemExit('never reached title')
            time.sleep(1.0)
            snap('title')
            confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
            confirm_to(3)
            N.setsel(1); time.sleep(0.3)
            confirm_to(4, 4); confirm_to(5, 4)
            snap('menu')
            press(4); time.sleep(1.5)
            for _ in range(5):
                if N.phase() != 3:
                    break
                press(4); time.sleep(1.5)
            res['race_phase'] = N.phase()
            print('phase after confirm', N.phase(), flush=True)
            time.sleep(a.race_s)
            snap('race')
        else:
            dev.resume(pid)
            end = time.time() + a.limit
            k = 0
            while time.time() < end:
                time.sleep(1)
                k += 1
                try:
                    snap("t%d" % k)
                except Exception as e:
                    print('snap failed (process gone?)', e, flush=True); break
    finally:
        Path(a.out).write_text(json.dumps(res))
        print('->', a.out, flush=True)
        try:
            dev.kill(pid); print(f'killed own pid {pid}', flush=True)
        except Exception as e:
            print('kill', e, flush=True)


if __name__ == '__main__':
    main()
