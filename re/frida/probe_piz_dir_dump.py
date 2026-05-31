# Decisive runtime dump for the ~94s title crash (VFS / font-TXD root cause).
#
# Static scoping (2026-05-30, re/analysis/menu_crash_scoping/REPORT.md addendum)
# proved the piz FS handler's OPEN (LAB_004b6c00) and EXISTS (LAB_004b6ba0)
# methods are code-symmetric: both read count DAT_008ad9a8, walk dir DAT_0090dac0
# (stride 0x80), compare via __stricmp (FUN_004b6b10). So "open finds fgdc20.rwf
# (dir idx 16) but exists fails on fgdc20.txd (dir idx 17)" can ONLY mean the
# in-memory directory is truncated/garbled at/after idx 17 — OR the default mount
# isn't the piz handler at query time.
#
# This probe captures, at the first VfsFileExists("...fgdc20...") call:
#   - default mount DAT_007dc76c vs piz handler DAT_007d3e54 (did the swap happen?)
#   - piz-open flag DAT_007d3e4c, saved-mount DAT_007d3e58, current piz name DAT_008ab7e0
#   - in-memory piz header: magic/version/count (DAT_008ad9a0/a4/a8) — should be
#     PIZ\0 / 3 / 26 if the first 2KB read was correct
#   - first 28 directory entry names from DAT_0090dac0 (stride 0x80, name @+0) —
#     reveals whether idx 17 == FGDC20.TXD or where truncation/garbage starts
# Also confirms whether the piz EXISTS method (LAB_004b6ba0) is even reached for
# fgdc20 (proves the piz handler is the current mount).
#
# Read-only observation. Does NOT touch original/. Light interceptors on cold,
# once-per-boot paths (NOT hot-path) per CLAUDE.md Frida rules.
# Usage: run this, then launch MASHED.exe. Font load fires ~90s in.
import json, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    sys.exit("psutil required")

ROOT     = Path(__file__).resolve().parent.parent.parent
OUT_FILE = ROOT / 'log' / 'piz_dir_dump.txt'

AGENT_JS = r'''
'use strict';
function rdstr(p, n){ try { return p.readCString(n); } catch(e){ return '<unreadable '+p+'>'; } }
function rdu32(a){ try { return ptr(a).readU32(); } catch(e){ return -1; } }
function rdptr(a){ try { return ptr(a).readPointer().toString(); } catch(e){ return '<unreadable>'; } }

function dumpState(){
  var st = {
    default_mount_DAT_007dc76c: rdptr('0x007dc76c'),
    piz_handler_DAT_007d3e54:   rdptr('0x007d3e54'),
    saved_mount_DAT_007d3e58:   rdptr('0x007d3e58'),
    piz_open_flag_DAT_007d3e4c: rdu32('0x007d3e4c'),
    cur_piz_name_DAT_008ab7e0:  rdstr(ptr('0x008ab7e0'), 0x80),
    hdr_magic_DAT_008ad9a0:     '0x' + (rdu32('0x008ad9a0')>>>0).toString(16),
    hdr_version_DAT_008ad9a4:   rdu32('0x008ad9a4'),
    hdr_count_DAT_008ad9a8:     rdu32('0x008ad9a8')
  };
  st.swap_happened = (st.default_mount_DAT_007dc76c === st.piz_handler_DAT_007d3e54);
  var base = ptr('0x0090dac0');
  var ents = [];
  for (var i=0; i<28; i++){
    var e = base.add(i*0x80);
    ents.push({ idx:i, name: rdstr(e, 64) });
  }
  st.dir_entries = ents;
  return st;
}

var dumped = false;
// VfsFileExists (FUN_00550b00): single arg = filename @ [esp+4]
Interceptor.attach(ptr('0x00550b00'), {
  onEnter: function(){
    this.fg = false;
    var nm;
    try { nm = this.context.esp.add(4).readPointer().readCString(); } catch(e){ return; }
    if (!nm) return;
    if (nm.toLowerCase().indexOf('fgdc20') === -1) return;
    this.fg = true; this.nm = nm;
    if (!dumped){ dumped = true; send({ kind:'dump', name:nm, state:dumpState() }); }
  },
  onLeave: function(ret){
    if (this.fg) send({ kind:'vfs', name:this.nm, ret:ret.toString(),
                        found:(ret.toInt32()>>>0)!==0 });
  }
});
// Piz EXISTS method (LAB_004b6ba0), reached only when the piz handler is the
// current mount. Called as (self, filename): filename @ [esp+8].
Interceptor.attach(ptr('0x004b6ba0'), {
  onEnter: function(){
    this.fg = false;
    var nm;
    try { nm = this.context.esp.add(8).readPointer().readCString(); } catch(e){ return; }
    if (!nm || nm.toLowerCase().indexOf('fgdc20') === -1) return;
    this.fg = true; this.nm = nm;
  },
  onLeave: function(ret){
    if (this.fg) send({ kind:'pizexists', name:this.nm, ret:ret.toString(),
                        found:(ret.toInt32()>>>0)!==0 });
  }
});

const FATAL = { 'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1 };
Process.setExceptionHandler(function(d){
  if (!FATAL[d.type]) return false;
  send({kind:'av', pc:d.context.pc.toString(), esi:d.context.esi.toString()});
  return false;
});
send({kind:'ready'});
'''

def find_pid():
    for p in psutil.process_iter(['name','pid']):
        try:
            if p.info['name'] and p.info['name'].lower()=='mashed.exe': return p.info['pid']
        except Exception: continue
    return None

def main():
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    print("waiting for MASHED.exe — launch it now (font TXD load fires ~90s in)")
    dl=time.time()+120; pid=None
    while time.time()<dl:
        pid=find_pid()
        if pid: print(f"  found pid={pid}"); break
        time.sleep(0.1)
    if not pid: sys.exit("timeout waiting for MASHED.exe")
    time.sleep(0.05)
    session=frida.get_local_device().attach(pid); print("  attached")
    events=[]
    def on_message(m,d):
        if m['type']=='error': print("  agent error:", m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='ready':
            print("  probes armed (VfsFileExists 0x00550b00 + piz-exists 0x004b6ba0)")
        elif k=='dump':
            s=p['state']; events.append(p)
            print(f"\n  === STATE SNAPSHOT at VfsFileExists({p['name']!r}) ===")
            print(f"    default mount DAT_007dc76c = {s['default_mount_DAT_007dc76c']}")
            print(f"    piz handler   DAT_007d3e54 = {s['piz_handler_DAT_007d3e54']}")
            print(f"    swap happened (mount==piz handler)? {s['swap_happened']}")
            print(f"    piz-open flag DAT_007d3e4c = {s['piz_open_flag_DAT_007d3e4c']}")
            print(f"    current piz name           = {s['cur_piz_name_DAT_008ab7e0']!r}")
            print(f"    hdr magic/ver/count        = {s['hdr_magic_DAT_008ad9a0']} / {s['hdr_version_DAT_008ad9a4']} / {s['hdr_count_DAT_008ad9a8']}   (expect 0x5a4950 / 3 / 26)")
            print(f"    directory entries (DAT_0090dac0, stride 0x80, name@+0):")
            for e in s['dir_entries']:
                print(f"      [{e['idx']:2d}] {e['name']!r}")
        elif k=='vfs':
            print(f"  VfsFileExists({p['name']!r}) -> {p['ret']}" + ("" if p['found'] else "   <-- NOT FOUND")); events.append(p)
        elif k=='pizexists':
            print(f"  piz-exists LAB_004b6ba0({p['name']!r}) -> {p['ret']}" + ("" if p['found'] else "   <-- NOT FOUND (piz handler IS the mount)")); events.append(p)
        elif k=='av':
            print(f"\n  AV pc={p['pc']} esi={p['esi']}"); events.append(p)
    script=session.create_script(AGENT_JS); script.on('message', on_message); script.load()
    dl=time.time()+150
    while time.time()<dl:
        try:
            if psutil.Process(pid).name().lower()!='mashed.exe': break
        except Exception: print("  process gone"); break
        time.sleep(0.3)
    try: session.detach()
    except Exception: pass
    OUT_FILE.write_text(json.dumps(events, indent=2), encoding='utf-8')
    print(f"\nwrote {len(events)} event(s) -> {OUT_FILE}")
    return 0

if __name__=='__main__':
    sys.exit(main())
