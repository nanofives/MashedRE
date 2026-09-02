// observe_block.js — shared canonical-observation agent block.
//
// Records what the ORIGINAL does, per call, for an arbitrary set of RVAs during a
// real booted scenario: arguments, return value, and per-row observables that are
// either an argument dereference (read after the call) or an ABSOLUTE memory block
// (read BEFORE and AFTER, so the delta is the witness).
//
// Why the pre/post split matters: a post-only read cannot separate "ran and wrote
// this" from "this was already the value". The delta is what distinguishes
// ran-and-worked from never-ran, which is the whole point ([[scratch-field-false-green]]).
//
// APPENDED to a host agent (scenario_launch.py, replay_session.py) rather than
// duplicated into each, so the two drivers cannot drift. It registers itself onto
// the host's rpc.exports at the bottom, so it must be concatenated AFTER the host
// assigns rpc.exports.
//
// The host must provide ga(rva) -> NativePointer (module-base-relative address). If
// it does not, a fallback resolving against the main module is installed below.
//
// Spec shape (JSON from python):
//   [{rva:"0x004d5340", cap:24, nargs:6,
//     obs:[{from:2, off:0, size:4},          // arg-deref, post-only
//          {abs:"0x0063bb04", len:44}]}]     // absolute block, pre+post
'use strict';

if (typeof ga !== 'function') {
  // eslint-disable-next-line no-var
  var ga = function(rva){
    try {
      const m = Process.enumerateModules()[0];
      return m ? m.base.add(rva - 0x00400000) : null;
    } catch(e){ return null; }
  };
}

const TEXOBS = { rows: {}, armed: false };

// Read an absolute-VA block as a hex string. `abs` is an RVA in MASHED.exe, so it
// goes through ga() like every other address (the image is not always at its
// preferred base). Returns a marker rather than throwing - a wild read must not
// disturb the run it is observing.
function readBlock(o){
  try {
    const p = ga(parseInt(o.abs, 16));
    if (!p) return 'NOBASE';
    const b = new Uint8Array(p.readByteArray(o.len || 4));
    let s = '';
    for (let i = 0; i < b.length; i++) s += ('0' + b[i].toString(16)).slice(-2);
    return s;
  } catch(e){ return 'ERR'; }
}

function texObserve(specJson){
  try {
    if (TEXOBS.armed) return 'already armed';
    const spec = JSON.parse(specJson);
    const out = [];
    spec.forEach(function(s){
      const rva = parseInt(s.rva, 16);
      const p = ga(rva);
      if (!p) { out.push(s.rva + '=NOBASE'); return; }
      const row = { rva: s.rva, calls: 0, recs: [], cap: s.cap || 24,
                    nargs: s.nargs || 4, obs: s.obs || [], err: null };
      TEXOBS.rows[s.rva] = row;
      Interceptor.attach(p, {
        onEnter: function(){
          this.rec = null;
          row.calls++;
          // Stop RECORDING at the cap but keep COUNTING, so a hot row does not
          // blow the buffer or the Interceptor budget for the rest of the run.
          if (row.recs.length >= row.cap) return;
          const a = [];
          try {
            const sp = this.context.esp;
            for (let i = 0; i < row.nargs; i++) {
              a.push('0x' + sp.add(4 + i * 4).readU32().toString(16));
            }
          } catch(e){ if (!row.err) row.err = 'args ' + e; }
          this.rec = { n: row.calls, args: a, ret: null, obs: [], pre: [] };
          this.argv = a;
          for (let i = 0; i < row.obs.length; i++) {
            const o = row.obs[i];
            if (!o.abs) { this.rec.pre.push(null); continue; }
            this.rec.pre.push(readBlock(o));
          }
        },
        onLeave: function(rv){
          if (!this.rec) return;
          try { this.rec.ret = '0x' + rv.toUInt32().toString(16); }
          catch(e){ this.rec.ret = 'ERR'; }
          for (let i = 0; i < row.obs.length; i++) {
            const o = row.obs[i];
            if (o.abs) {
              const post = readBlock(o);
              this.rec.obs.push({ abs: o.abs, len: o.len || 4,
                                  pre: this.rec.pre[i], v: post,
                                  moved: this.rec.pre[i] !== post });
              continue;
            }
            // Arg-deref observables are out-params by construction, so they are
            // read AFTER the call; reading on entry would only show what the
            // caller passed in.
            let v = 'ERR';
            try {
              const base = ptr(this.argv[o.from]);
              const at = base.add(o.off || 0);
              const sz = o.size || 4;
              v = '0x' + (sz === 1 ? at.readU8()
                        : sz === 2 ? at.readU16()
                        : at.readU32()).toString(16);
            } catch(e){ v = 'ERR'; }
            this.rec.obs.push({ from: o.from, off: o.off || 0, size: o.size || 4, v: v });
          }
          row.recs.push(this.rec);
        }
      });
      out.push(s.rva + '=observing(cap ' + row.cap + ')');
    });
    TEXOBS.armed = true;
    return out.join(' ');
  } catch(e){ return 'ERR ' + e; }
}

function texResults(){
  try { return JSON.stringify(TEXOBS.rows); }
  catch(e){ return JSON.stringify({ error: '' + e }); }
}

rpc.exports.texObserve = function(specJson){ return texObserve(specJson); };
rpc.exports.texResults = function(){ return texResults(); };
