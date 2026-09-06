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
let nav=null, lookupB=null, lookupI=null;
let natural=[], hooked=0;
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
    return DELTA;
  },
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
        print(f"push {args.screen} -> depth", E.push(args.screen))
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

        out = Path(args.out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(
            {"screen": args.screen, "dicts": dicts, "matrix": matrix, "natural": nat},
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
