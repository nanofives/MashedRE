#!/usr/bin/env python3
"""Enumerate the two live sprite dictionaries and resolve the Challenge-Select
icon names against them, in the original, at nav screen 6.

WHY THIS EXISTS
---------------
`re/analysis/race_hud_capture_20260902.md` Finding 36 left it `[UNCERTAIN]`
whether `FUN_0040bb50("check", ...)` resolves to anything, reasoning that
`INTERFACE.TXD` holds Lock/Star/Tick and no "check". That reasoning checked the
wrong dictionary. The game has TWO named-sprite dictionaries behind TWO
forwarders (both ending in `FUN_004c5c00`):

    0x0040bb50  ->  DAT_0063b8fc   (loaded by FUN_0040bbb0 via
                                    FUN_0042a6b0("badges.txd",0,0))
    0x0040bb90  ->  DAT_0063b904   (INTERFACE.TXD)

and a slot gate in front of each, with disjoint name sets:

    0x0042ee00 -> bb50: slot0 "lock" @0x005cd7b8, slot1 "dot" @0x005cd7b4,
                        slot2 "check" @0x005cd7ac
    0x004391b0 -> bb90: slot0 "Lock" @0x005cda44, slot1/3 "Star" @0x005cd970,
                        slot2 "tick" @0x005cda3c (gated on FUN_00430760 and
                        DAT_0067e9fc not in {2, 0xa})

The Challenge-Select mode checklist in `FUN_00439210` picks between the first
pair and calls the BADGES forwarder -- read off the anchored binary:

    0x004395c6  mov  eax,[edx + 0x7f0a50]     ; the per-mode unlock flag
    0x004395cc  test eax,eax
    0x004395d1  je   0x4395e0
    0x004395d9  push 0x5cd7ac                 ; flag != 0  -> "check"
    0x004395e6  push 0x5cd7b8                 ; flag == 0  -> "lock"
    0x004395eb  call 0x40bb50                 ; BADGES dictionary
    0x004395f4  call 0x473870                 ; TextSpriteUVExplicit(tex, ...)

(same shape again at 0x0043966e/0x0043967b and 0x00439703/0x00439715.)

An offline dump of `SFX.piz::BADGES.TXD` shows it holds `lock`, `dot` and
`check` as adjacent 16x16 PAL4 textures (indices 11-13) -- the gate's whole
name set. But `FUN_004c5c00` folds case, so `"lock"` would ALSO match
INTERFACE's `Lock`; only the runtime value of `DAT_0063b8fc` distinguishes
"unlocked rows draw a check" from "unlocked rows draw nothing". That is what
this probe reads.

WHAT IT DOES
------------
1. Walks both dictionaries' linked lists and prints every node name. The layout
   is `FUN_004c5c00` verbatim: `sentinel = *(head + 8)`, `node = *sentinel`,
   iterate while `node != sentinel`, name at `*(node + 8)`, next at `*node`.
2. Calls `FUN_0040bb50` and `FUN_0040bb90` directly with a control matrix of
   names. Safe: `FUN_004c5c00` is a pure list walk with no stores (transcribed
   at `Frontend/SpriteCluster.cpp:167-218`), so calling it has no side effect
   on the frame -- verified before calling, not after.
3. Attaches an Interceptor to `FUN_0040bb50` and logs (name -> retval) for the
   calls the game makes on its own. `0x0040bb50` runs a handful of times per
   menu frame, well under the hot-path threshold in CLAUDE.md.

This is a research read, not a hook diff -- it deliberately does not go through
`hooks_registry.py`, and it installs none of our code (`MASHED_RE_NO_AUTO_HOOK=1`).

Usage:
    py -3.12 re/frida/chal_icon_probe.py [--screen 6] [--settle 3]
"""
import argparse
import json
import os
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
if not (ROOT / "original" / "MASHED.exe").exists() and os.environ.get("MASHED_ROOT"):
    ROOT = Path(os.environ["MASHED_ROOT"])
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_PHASE=0x0067eca4;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_CURSCREEN=0x0067ecb0;
// The two dictionary heads and their forwarders.
const RVA_DICT_BADGES=0x0063b8fc, RVA_DICT_IFACE=0x0063b904;
const RVA_LOOKUP_BADGES=0x0040bb50, RVA_LOOKUP_IFACE=0x0040bb90;
// --- row-icon trace -------------------------------------------------------
// The two slot GATES in front of the forwarders. FUN_00439210 picks between
// them per row: selected row -> the INTERFACE gate, other rows -> the BADGES
// gate. NOTE THE CONVENTIONS DIFFER, which is the whole reason to trace rather
// than read: 0x0042ee00 takes its slot on the STACK (push ecx / call), while
// 0x004391b0 takes it in EAX (0x0043a184 mov eax,[eax*4+0x7f0a40] / call, and
// the callee opens with `test eax,eax`). Ghidra prints FUN_004391b0() with no
// argument at all.
const RVA_GATE_BADGES=0x0042ee00, RVA_GATE_IFACE=0x004391b0;
// Screen id DAT_0067e9fc is DERIVED: FUN_0042f6b0 maps DAT_0067f184 through a
// jump table at 0x0042f724 (f184=3 -> e9fc=6, Challenge Select). Seed the
// producer f184 and call the mapper; do not poke e9fc.
const RVA_MODE=0x0067f184, RVA_SCREENID=0x0067e9fc, RVA_SETSCREEN=0x0042f6b0;
const RVA_SEL=0x0067f17c;             // selected challenge index
const RVA_STATETBL=0x007f0a40;        // 13 rows x 12 dwords cup/unlock table
let nav=null, lookupB=null, lookupI=null, setScreen=null;
let natural=[], hooked=0, rows=[], rowHooked=0;
function abs(r){return ptr(r+DELTA);}

// FUN_004c5c00's list layout, read off the anchored binary (0x004c5c00..72),
// NOT off the port's transcription in Frontend/SpriteCluster.cpp -- that copy
// has two defects and reading the list its way yields garbage names:
//   0x004c5c05  add eax,8         ; eax = head + 8 IS the sentinel ADDRESS
//   0x004c5c0b  mov ebx,[eax]     ; ebx = first node (= *(head+8))
//   0x004c5c19  lea eax,[ebx-8]   ; struct base = node - 8  (the return value)
//   0x004c5c1c  lea ecx,[eax+0x10]; name is the INLINE char array at node + 8
//   0x004c5c27  mov cl,[esi]      ; ...read as chars, not chased as a pointer
//   0x004c5c62  mov ebx,[ebx]     ; next = *node
//   0x004c5c68  cmp ebx,eax / jne ; loop while node != (head + 8)
function walk(headRva){
  const head=abs(headRva).readU32();
  if(head===0) return {head:0, names:null};
  const sentinel=head+8;                       // the ADDRESS, not its contents
  const names=[]; let node=ptr(sentinel).readU32(); let guard=0;
  while(node!==sentinel && node!==0 && guard<4096){
    let nm='<unreadable>';
    try{ nm=ptr(node).add(8).readCString(); }catch(e){}   // inline array
    names.push({node:'0x'+node.toString(16), name:nm});
    node=ptr(node).readU32(); guard++;
  }
  return {head:'0x'+head.toString(16), sentinel:'0x'+sentinel.toString(16),
          count:names.length, names:names};
}

rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    lookupB=new NativeFunction(abs(RVA_LOOKUP_BADGES),'pointer',['pointer']);
    lookupI=new NativeFunction(abs(RVA_LOOKUP_IFACE),'pointer',['pointer']);
    setScreen=new NativeFunction(abs(RVA_SETSCREEN),'void',[]);
    return DELTA;
  },
  // Seed the PRODUCER (DAT_0067f184) and let FUN_0042f6b0 derive the screen id.
  mode:function(m){ abs(RVA_MODE).writeS32(m); setScreen();
                    return abs(RVA_SCREENID).readS32(); },
  screenid:function(){ return abs(RVA_SCREENID).readS32(); },
  sel:function(){ return abs(RVA_SEL).readS32(); },
  // The 13x12 cup table, so measured gate slots can be correlated to a column.
  table:function(){
    const out=[];
    for(let r=0;r<13;r++){
      const row=[];
      for(let c=0;c<12;c++) row.push(abs(RVA_STATETBL+ (r*12+c)*4).readS32());
      out.push(row);
    }
    return JSON.stringify(out);
  },
  // Log every NATURAL gate call: which gate, and the slot it was handed.
  armrows:function(){
    if(rowHooked) return 1;
    Interceptor.attach(abs(RVA_GATE_BADGES), {
      onEnter(args){ rows.push({gate:'badges16', slot:args[0].toInt32()}); }
    });
    Interceptor.attach(abs(RVA_GATE_IFACE), {
      onEnter(){ rows.push({gate:'iface32', slot:this.context.eax.toInt32()}); }
    });
    rowHooked=1; return 1;
  },
  // DISCRIMINATOR. The measured slot is the same for every row on a stock save
  // because rows 0..3 all hold 2 in column 3 -- so "the index carries the row"
  // and "the index is loop-invariant" predict the SAME observation. Poke
  // distinct values into column 3 of rows 0..3 and re-measure: if the badges
  // gate then sees several distinct slots, the index carries the row; if it
  // still sees only row 0's value, it does not. Live-memory poke on a table the
  // renderer only READS, and the process is killed straight after; nothing
  // under original/ is touched.
  poke:function(vals){
    const v=JSON.parse(vals), before=[];
    for(let r=0;r<v.length;r++){
      const a=abs(RVA_STATETBL + (r*12+3)*4);
      before.push(a.readS32()); a.writeS32(v[r]);
    }
    return JSON.stringify(before);
  },
  rowreport:function(){ const c={}; for(const r of rows){
      const k=r.gate+' slot='+r.slot; c[k]=(c[k]||0)+1; }
      return JSON.stringify({counts:c, total:rows.length}); },
  rowclear:function(){ rows=[]; return 1; },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); abs(RVA_CURSCREEN).writeS32(scr);
                      return abs(RVA_DEPTH).readS32(); },
  // Log what the game itself asks the BADGES forwarder for.
  arm:function(){
    if(hooked) return 1;
    Interceptor.attach(abs(RVA_LOOKUP_BADGES), {
      onEnter(args){ this.key=args[0].isNull()?'<null>':args[0].readCString(); },
      onLeave(ret){ natural.push({key:this.key, ret:'0x'+ret.toUInt32().toString(16)}); }
    });
    hooked=1; return 1;
  },
  natural:function(){ const c={}; for(const r of natural){
      const k=r.key+' -> '+r.ret; c[k]=(c[k]||0)+1; } return JSON.stringify(c); },
  dicts:function(){ return JSON.stringify({
      badges_0063b8fc: walk(RVA_DICT_BADGES),
      iface_0063b904:  walk(RVA_DICT_IFACE) }); },
  // Direct control matrix. Pure lookup, no stores (FUN_004c5c00).
  resolve:function(keys){
    const out=[];
    for(const k of keys){
      const buf=Memory.allocUtf8String(k);
      let rb='ERR', ri='ERR';
      try{ rb='0x'+lookupB(buf).toUInt32().toString(16); }catch(e){}
      try{ ri='0x'+lookupI(buf).toUInt32().toString(16); }catch(e){}
      out.push({key:k, badges:rb, iface:ri});
    }
    return JSON.stringify(out);
  }
};
'''


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--screen", type=int, default=6,
                    help="nav screen to push before reading (6 = Challenge Select)")
    ap.add_argument("--settle", type=float, default=3.0)
    ap.add_argument("--poke", default=None, metavar="v0,v1,v2,v3",
                    help="write distinct values into column 3 of cup rows 0..N "
                         "and re-measure, to test whether the gate slot varies "
                         "per row")
    ap.add_argument("--mode", type=int, default=None,
                    help="seed DAT_0067f184 and derive the screen id via "
                         "FUN_0042f6b0 (3 = Challenge Select / screen 6)")
    ap.add_argument("--out", default=str(ROOT / "log" / "chal_icon_probe.json"))
    args = ap.parse_args()

    env = dict(os.environ)
    env.setdefault("MASHED_RE_NO_AUTO_HOOK", "1")   # stock original, none of our hooks
    env.setdefault("MASHED_WIN_POS", "left-bl")

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"spawned MASHED pid={pid} (this session owns ONLY this pid)")
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: print("[js]", m))
        scr.load()
        scr.exports_sync.init()
        dev.resume(pid)
        E = scr.exports_sync

        print("waiting for menu...")
        end = time.time() + 25
        while time.time() < end and E.phase() != 3:
            time.sleep(0.2)
        time.sleep(1.5)

        E.arm()
        E.armrows()
        # Drive the screen id through its producer, not by poking the derived
        # global: FUN_0042f6b0 maps DAT_0067f184 -> DAT_0067e9fc.
        if args.mode is not None:
            print(f"mode DAT_0067f184={args.mode} -> screen id DAT_0067e9fc =",
                  E.mode(args.mode))
        print(f"push {args.screen} -> depth", E.push(args.screen))
        time.sleep(args.settle)
        E.rowclear()                    # drop transition-frame calls
        time.sleep(args.settle)

        dicts = json.loads(E.dicts())
        matrix = json.loads(E.resolve(
            ["check", "lock", "dot", "Lock", "Star", "Tick", "tick", "Button"]))
        nat = json.loads(E.natural())

        for tag in ("badges_0063b8fc", "iface_0063b904"):
            d = dicts[tag]
            names = d.get("names")
            print(f"\n== {tag}  head={d.get('head')} count={d.get('count')}")
            if names is None:
                print("   <null / unwalkable>")
            else:
                # ASCII-safe: a node name can carry bytes the console codepage
                # cannot encode, and a UnicodeEncodeError here would throw away
                # a capture that is already correct.
                print("   " + ", ".join(
                    ascii(n["name"]) for n in names))

        print("\n== direct lookup matrix (retval 0x0 = miss)")
        for r in matrix:
            print(f"   {r['key']:8s}  bb50/badges={r['badges']:12s} bb90/iface={r['iface']}")

        print("\n== natural FUN_0040bb50 calls while screen %d was up" % args.screen)
        for k, v in sorted(nat.items()):
            print(f"   {v:5d}x  {k}")

        # Discriminating pass: give rows 0..3 distinct column-3 values and see
        # whether the gates start receiving distinct slots.
        if args.poke:
            vals = [int(t, 0) for t in args.poke.split(",")]
            print("\npoke cup col3 rows 0..%d -> %s (was %s)"
                  % (len(vals) - 1, vals, E.poke(json.dumps(vals))))
            time.sleep(args.settle)
            E.rowclear()
            time.sleep(args.settle)

        rowrep = json.loads(E.rowreport())
        tbl = json.loads(E.table())
        print(f"\n== row-icon gate calls (screen id {E.screenid()}, "
              f"selected row DAT_0067f17c = {E.sel()}), total {rowrep['total']}")
        if not rowrep["counts"]:
            print("   <none — FUN_00439210's row loop did not run in this window>")
        for k, v in sorted(rowrep["counts"].items()):
            print(f"   {v:5d}x  {k}")
        print("\n== cup table 0x007f0a40, 13 rows x 12 dwords (col 3 = the gate slot)")
        for r, row in enumerate(tbl):
            if any(row):
                print(f"   row {r:2d}: " + " ".join(f"{v:3d}" for v in row))

        out = Path(args.out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(
            {"screen": args.screen, "screen_id": E.screenid(), "selected": E.sel(),
             "dicts": dicts, "matrix": matrix, "natural": nat,
             "rows": rowrep, "cup_table": tbl},
            indent=1))
        print("\n->", out)
    finally:
        # Kill ONLY the pid we spawned (CLAUDE.md multi-session rule).
        try:
            dev.kill(pid)
            print(f"killed pid={pid}")
        except Exception as exc:
            print(f"could not kill pid={pid}: {exc}")


if __name__ == "__main__":
    main()
