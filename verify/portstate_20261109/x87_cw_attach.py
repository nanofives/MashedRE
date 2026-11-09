"""x87_cw_attach.py --out <json> [--wait-pid S] -- <harness command ...>

PORT-X87-UNREACHED. The PORT-X87-PC24-AUDIT CW probe (x87_cw_probe.py, the same native FNSTCW onEnter stub per RVA and
the same [n(0x007f), n(0x027f), n(other), last other] slot) attached to an ORIGINAL-side scenario that an existing
harness drives. The harness is run as a child; the first `pid=<n>` it prints is the MASHED.exe it spawned, and this
tool attaches to that explicit pid (never by name), arms the stubs on every RVA of x87_inventory.tsv (as the audit's
orig target) and snapshots the counters every 2 s until the harness exits. The harness keeps ownership of its process
(it kills its own pid). Counters are cumulative from the attach; the last snapshot that succeeded is the result.

Run under test-slot (outside) and mashed_lock.py run (inside).
"""
import argparse, json, re, subprocess, sys, threading, time
from pathlib import Path
import frida

HERE = Path(__file__).resolve().parent
ap = argparse.ArgumentParser()
ap.add_argument('--out', required=True)
ap.add_argument('--skip', default='0x00496400', help='comma list of RVAs not to stub (0x00496400: disable_log patch region)')
ap.add_argument('--late', default='', help='RVAs to stub only --late-s seconds after the attach (harnesses that check a pristine entry first)')
ap.add_argument('--late-s', type=float, default=25)
ap.add_argument('cmd', nargs=argparse.REMAINDER)
a = ap.parse_args()
cmd = a.cmd[1:] if a.cmd and a.cmd[0] == '--' else a.cmd

INV = [l.split('\t') for l in (HERE / 'x87_inventory.tsv').read_text().splitlines()[1:]]
skip = {s.lower() for s in a.skip.split(',') if s}
targets, seen = [], set()
for tu, sect, src, rva, how in INV:
    if rva and rva not in seen and rva.lower() not in skip:
        seen.add(rva); targets.append(rva)
late = [t for t in targets if t.lower() in {x.lower() for x in a.late.split(',') if x}]
targets = [t for t in targets if t not in late] + late   # late ones last: their slots follow the early block
n_early = len(targets) - len(late)

AGENT = r'''
'use strict';
let BUFS=[], KEEP=[];
function le(n){ n=n>>>0; return [n&0xff,(n>>>8)&0xff,(n>>>16)&0xff,(n>>>24)&0xff]; }
function lep(p){ return le(parseInt(p.toString(),16)); }
rpc.exports={
  arm:function(mod, pref, list){
    const m=Process.findModuleByName(mod);
    const delta=m.base.toUInt32()-pref;
    const BUF=Memory.alloc(list.length*16); KEEP.push(BUF); BUFS.push([BUF,list.length]);
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
  snap:function(n){ const r=[]; for(const [B,m] of BUFS){ for(let i=0;i<m;i++){ const p=B.add(i*16); r.push([p.readU32(),p.add(4).readU32(),p.add(8).readU32(),p.add(12).readU32()]); } } while(r.length<n) r.push([0,0,0,0]); return r; }
};
'''

res = {'target': 'orig-attach', 'targets': targets, 'cmd': cmd, 'snaps': [], 'log': []}
state = {'E': None, 'stop': False, 'pid': None}


def snapper():
    t0 = time.time()
    while not state['stop']:
        time.sleep(2)
        E = state['E']
        if E is None:
            continue
        try:
            c = E.snap(len(targets))
            res['snaps'] = [{'t': round(time.time() - t0, 1), 'c': c}]   # keep the newest good one
        except Exception:
            pass


th = threading.Thread(target=snapper, daemon=True); th.start()
p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace')
dev = frida.get_local_device()
for line in p.stdout:
    sys.stdout.write(line); sys.stdout.flush(); res['log'].append(line.rstrip('\n'))
    if state['pid'] is None:
        m = re.search(r'pid[= ](\d+)', line)
        if m:
            pid = int(m.group(1)); state['pid'] = pid; res['pid'] = pid
            sess = None
            for k in range(150):
                try:
                    sess = dev.attach(pid); break
                except Exception as e:
                    time.sleep(0.2)
            if sess is None:
                print(f'[cw-attach] pid={pid} attach FAILED after 30 s', flush=True)
                continue
            scr = sess.create_script(AGENT); scr.load()
            delta, att = scr.exports_sync.arm('MASHED.exe', 0x00400000, [int(t, 16) for t in targets[:n_early]])
            if late:
                def arm_late(E=scr.exports_sync):
                    time.sleep(a.late_s)
                    try:
                        r = E.arm('MASHED.exe', 0x00400000, [int(t, 16) for t in late])
                        res['attach_late'] = r[1]; print(f'[cw-attach] late stubs armed {late} -> {r[1]}', flush=True)
                    except Exception as e:
                        print('[cw-attach] late arm failed', e, flush=True)
                threading.Thread(target=arm_late, daemon=True).start()
            res['attach'] = att
            bad = [(targets[i], x) for i, x in enumerate(att) if x != 1]
            print(f'[cw-attach] pid={pid} stubs {len(targets) - len(bad)}/{len(targets)} failures {bad}', flush=True)
            state['E'] = scr.exports_sync
p.wait()
state['stop'] = True
res['harness_rc'] = p.returncode
Path(a.out).write_text(json.dumps(res))
tot = [sum(c[i] for c in res['snaps'][0]['c']) for i in range(3)] if res['snaps'] else None
print(f'[cw-attach] -> {a.out} harness rc={p.returncode} last snap t={res["snaps"][0]["t"] if res["snaps"] else None} totals 0x7f/0x27f/other={tot}', flush=True)
