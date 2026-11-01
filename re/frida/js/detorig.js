// Deterministic-original agent for MASHED.exe (PROMOTE-DETORIG-PILOT, 2026-11-01). Port of SimGolf's
// re/frida/js/scenario_det.js, cut down to what one pilot scenario needs. Driver: re/frida/detorig.py.
//
// Two runs of the same scenario on the original drift apart because the main thread reads real time:
// FUN_004950b0 (QueryPerformanceCounter rescaled to 3,000,000/s, IAT 0x005cc0c4) feeds the frame timer
// FUN_00493390, whose sample DAT_007f1030 (written at 0x00493459) is the race camera's sway input and so
// sets the phase-3 length; FUN_005ab040 reads QPC or timeGetTime (IAT 0x005cc2b4); GetTickCount
// (IAT 0x005cc0c8). Input lands in whichever frame happens to run. This agent makes the main thread's
// view of time a function of its own work:
//
//  1. VIRTUAL CLOCK. The three IAT slots point at native stubs. On the main thread (TEB+0x24 == MAIN)
//     timeGetTime / GetTickCount return T and advance T by cfg.step ms; QPC returns Q and advances Q by
//     cfg.qstep counts. Every main-thread clock call also increments N (the virtual timeline is N).
//     Other threads (audio, DirectShow) jump to the real API without entering JavaScript.
//  2. EVENTS ON N. When a main-thread clock call finds N >= due, the stub calls onDue (JavaScript, on
//     the main thread, which is blocked meanwhile). onDue runs every event whose N has come, in order:
//     the warp (selection globals, FUN_0040e480 slot activation, RNG pins, DAT_00771968 = 2), scripted
//     input changes, and checkpoints. A checkpoint reads .data + _rwdseg of MASHED.exe (the image's
//     whole writable range, 0x005ea000..0x00915008) and the two RNG states on the blocked main thread
//     and sends them to Python.
//  3. SCRIPTED INPUT. FUN_00497310(player, control) (the per-player control resolver, cdecl) is
//     replaced by a CModule that calls the original and, for player 0, returns the scripted value of
//     that control (default 0), so real keystrokes never reach player 0.
//  4. RNG PINS at the warp. CRT rand (0x005c229b) keeps its state at _getptd()+0x14 (_getptd =
//     0x004a8828, called here on the main thread); srand's only caller 0x004c0502 seeds it from
//     0x005c1dac. RenderWare's ring RNG FUN_00534870 is seeded by 0x005349b4..0x00534a74 with the
//     constant 0x9a319039 (0x00534a09), LCG 0x41c64e6d / 0x3039 over 31 dwords, then 0x136 draws
//     (0x00534a46); the menu consumes it a run-dependent number of times. Both are reset to fixed
//     states at the warp so the race starts from the same RNG state in every run.
//
// Config (prefixed by detorig.py as `const DETORIG_CFG = {...};`):
//   main            main thread id (owner of the game window)
//   t0, step        timeGetTime / GetTickCount start (ms) and ms per call
//   q0, qstep       QPC start and counts per call
//   warpAt          N (relative to install) at which the warp runs
//   scen            {track, mode, rule, car, team, cars}
//   input           [[dN, control, value], ...]  dN relative to the warp
//   ckpts           [dN, ...]  checkpoints relative to the warp
//   holdrand        CRT rand state written at the warp
//   witness         RVAs whose first code byte is reported (hook-live check)
//   negctl          negative control at the race anchor: 1 = +1 on the RW RNG word at the cursor, else a VA bumped by one
//   loadSleepMs     real ms slept in each main-thread clock call between the warp and phase 3 (load hold)
//   rebase          set T / Q at the race anchor to the values of an anchor at N = warpAt + 200
'use strict';
const CFG = DETORIG_CFG;
const IMG = 0x400000;
const M = Process.findModuleByName('MASHED.exe');
const ga = (va) => M.base.add(va - IMG);
const keep = [];
const K = (x) => { keep.push(x); return x; };

// ---------------------------------------------------------------- data block
// +0x00 T  +0x04 step  +0x08 due  +0x0c MAIN  +0x10 Qlo  +0x14 Qhi  +0x18 qstep  +0x1c onDue
// +0x20 tgt calls  +0x24 qpc calls  +0x28 forwarded  +0x30 N
const D = K(Memory.alloc(Process.pageSize));
D.writeU32(CFG.t0 >>> 0);
D.add(4).writeU32(CFG.step >>> 0);
D.add(8).writeU32(0xffffffff);
D.add(0xc).writeU32(CFG.main >>> 0);
D.add(0x10).writeU32(CFG.q0 >>> 0);
D.add(0x14).writeU32(0);
D.add(0x18).writeU32(CFG.qstep >>> 0);
const N = () => D.add(0x30).readU32();
const setDue = (v) => D.add(8).writeU32(v >>> 0);

const CODE = K(Memory.alloc(Process.pageSize));
let codeOff = 0;
function emit(bytes) {
  const p = CODE.add(codeOff);
  Memory.patchCode(p, bytes.length, (w) => w.writeByteArray(bytes));
  codeOff = (codeOff + bytes.length + 15) & ~15;
  return p;
}
const u32 = (v) => { v = (typeof v === 'number') ? v >>> 0 : ptr(v).toUInt32(); return [v & 0xff, (v >>> 8) & 0xff, (v >>> 16) & 0xff, (v >>> 24) & 0xff]; };
const d = (off) => u32(D.add(off));
const CELLS = K(Memory.alloc(Process.pageSize));
let cellOff = 0;
function cell(v) { const c = CELLS.add(cellOff); cellOff += 4; c.writePointer(v); return c; }

// timeGetTime / GetTickCount: stdcall, no args, value in eax.
function tgtStub(realCell) {
  const b = [0x64, 0xa1, 0x24, 0, 0, 0, 0x3b, 0x05, ...d(0xc), 0x75, 0x00];   // mov eax,fs:[24]; cmp eax,[MAIN]; jne real
  const jneAt = b.length - 1;
  b.push(0xa1, ...d(0), 0x8b, 0x0d, ...d(4), 0x01, 0x0d, ...d(0),              // mov eax,[T]; mov ecx,[step]; add [T],ecx
         0xff, 0x05, ...d(0x20), 0xff, 0x05, ...d(0x30),                         // inc [tgt]; inc [N]
         0x8b, 0x0d, ...d(0x30), 0x3b, 0x0d, ...d(8), 0x73, 0x01, 0xc3,          // mov ecx,[N]; cmp ecx,[due]; jae slow; ret
         0x50, 0xff, 0x15, ...d(0x1c), 0x58, 0xc3);                              // slow: push eax; call [onDue]; pop eax; ret
  b[jneAt] = b.length - (jneAt + 1);
  b.push(0xf0, 0xff, 0x05, ...d(0x28), 0xff, 0x25, ...u32(realCell));            // real: lock inc [fwd]; jmp [cell]
  return emit(b);
}
// QueryPerformanceCounter: stdcall (LARGE_INTEGER*), BOOL in eax.
function qpcStub(realCell) {
  const b = [0x64, 0xa1, 0x24, 0, 0, 0, 0x3b, 0x05, ...d(0xc), 0x75, 0x00];
  const jneAt = b.length - 1;
  b.push(0x8b, 0x44, 0x24, 0x04,                                                 // mov eax,[esp+4]
         0x8b, 0x0d, ...d(0x10), 0x89, 0x08, 0x8b, 0x0d, ...d(0x14), 0x89, 0x48, 0x04, // *eax = Q
         0x8b, 0x0d, ...d(0x18), 0x01, 0x0d, ...d(0x10), 0x83, 0x15, ...d(0x14), 0x00, // Q += qstep
         0xff, 0x05, ...d(0x24), 0xff, 0x05, ...d(0x30),                         // inc [qpc]; inc [N]
         0x8b, 0x0d, ...d(0x30), 0x3b, 0x0d, ...d(8), 0x72, 0x06,                // mov ecx,[N]; cmp ecx,[due]; jb fast
         0xff, 0x15, ...d(0x1c),                                                 // call [onDue]
         0xb8, 1, 0, 0, 0, 0xc2, 0x04, 0x00);                                    // fast: mov eax,1; ret 4
  b[jneAt] = b.length - (jneAt + 1);
  b.push(0xf0, 0xff, 0x05, ...d(0x28), 0xff, 0x25, ...u32(realCell));
  return emit(b);
}

// ---------------------------------------------------------------- scripted input (CModule over FUN_00497310)
const CTL = K(Memory.alloc(64 * 4));            // scripted value per control for player 0
for (let i = 0; i < 64; i++) CTL.add(i * 4).writeS32(0);
const RESOLVER = ga(0x00497310);
const ORIG_CELL = K(Memory.alloc(8));
const ACTIVE = K(Memory.alloc(4));
ACTIVE.writeU32(0);
const setActive = (v) => ACTIVE.writeU32(v);
const cm = K(new CModule(`
  typedef int (*fn_t)(int, int);
  extern fn_t orig_cell;
  extern volatile int ctl[64];
  extern volatile int active;
  int repl(int p, int c) {
    int r = orig_cell(p, c);
    if (active && p == 0 && c >= 0 && c < 64) return ctl[c];
    return r;
  }
`, { orig_cell: ORIG_CELL, ctl: CTL, active: ACTIVE }));

// ---------------------------------------------------------------- events
const events = [];   // {n, s, fn}
let seq = 0, warpN = null, raceN = null;
function schedule(n, fn) { events.push({ n: n >>> 0, s: seq++, fn }); events.sort((a, b) => a.n - b.n || a.s - b.s); setDue(events.length ? events[0].n : 0xffffffff); }

const DATA_LO = 0x005ea000, DATA_HI = 0x00915008;   // .data (0x32a704) + _rwdseg (8), contiguous with padding
function rwRngPtr() {   // RenderWare RNG plugin block = *(0x007d3ff8) + *(0x007dc578)
  const g = ga(0x007d3ff8).readPointer();
  return g.add(ga(0x007dc578).readU32());
}
const getptd = new NativeFunction(ga(0x004a8828), 'pointer', [], 'mscdecl');
const rwDraw = new NativeFunction(ga(0x00534870), 'uint32', [], 'mscdecl');

function pinRngs() {
  const ptd = getptd();
  const before = ptd.add(0x14).readU32();
  ptd.add(0x14).writeU32(CFG.holdrand >>> 0);
  // reproduce 0x005349cf..0x00534a6d on the existing ring buffer
  const P = rwRngPtr();
  const buf = P.readPointer();
  const ringBefore = buf.readByteArray(0x7c);
  const cursBefore = [P.add(4).readU32(), P.add(8).readU32()];
  let v = 0x9a319039;
  buf.writeU32(v);
  for (let i = 1; i < 31; i++) { v = (Math.imul(v, 0x41c64e6d) + 0x3039) >>> 0; buf.add(i * 4).writeU32(v); }
  P.add(4).writeU32(buf.add(0xc).toUInt32());
  P.add(8).writeU32(buf.toUInt32());
  for (let i = 0; i < 0x136; i++) rwDraw();
  return { ptd: ptd.toString(), holdrand_before: before, rw_ring: buf.toString(), rw_cursors_before: cursBefore };
}

function warp() {
  const s = CFG.scen;
  ga(0x0063ba7c).writeS32(s.track);   // TRACK_ENG
  ga(0x0067f17c).writeS32(s.track);   // TRACK_MENU
  ga(0x0067e9fc).writeS32(s.mode);
  ga(0x007f0fd0).writeS32(s.rule);
  ga(0x0067ea98).writeS32(s.car);
  ga(0x0067ea64).writeS32(s.team);
  const e480 = new NativeFunction(ga(0x0040e480), 'void', ['int', 'int'], 'mscdecl');
  for (let k = 0; k < 4; k++) e480(k, k === 0 ? 1 : (k < s.cars ? 2 : 0));
  const pins = pinRngs();
  ga(0x00771968).writeU8(2);          // PHASE := load + spawn
  return pins;
}

// first code byte at each cfg.witness RVA (0xE9 = the .asi's inline JMP is live there)
function witness() { return (CFG.witness || []).map((va) => [va.toString(16), ga(va).readU8()]); }

function checkpoint(k) {
  const n = N();
  const blob = ga(DATA_LO).readByteArray(DATA_HI - DATA_LO);
  const ptd = getptd();
  const P = rwRngPtr();
  const info = {
    kind: 'ckpt', k, n, dn: n - raceN, dn_warp: n - warpN,
    T: D.readU32(), Qlo: D.add(0x10).readU32(), Qhi: D.add(0x14).readU32(),
    tgt: D.add(0x20).readU32(), qpc: D.add(0x24).readU32(), fwd: D.add(0x28).readU32(),
    phase: ga(0x00771968).readU8(), clk_7f0ff0: ga(0x007f0ff0).readU32(), clk_7f0ff4: ga(0x007f0ff4).readU32(),
    frames_7f101c: ga(0x007f101c).readU32(), witness: witness(),
    holdrand: ptd.add(0x14).readU32(),
    rw_ring: Array.from(new Uint32Array(P.readPointer().readByteArray(0x7c))),
    rw_cur: [P.add(4).readU32() - P.readPointer().toUInt32(), P.add(8).readU32() - P.readPointer().toUInt32()],
  };
  send(info, blob);
}

const onDue = K(new NativeCallback(() => {
  try {
    const n = N();
    while (events.length && events[0].n <= n) { const e = events.shift(); e.fn(); }
    setDue(events.length ? events[0].n : 0xffffffff);
  } catch (e) {
    send({ kind: 'err', msg: 'onDue ' + e + ' ' + e.stack });
    setDue(0xffffffff);
  }
}, 'void', []));
D.add(0x1c).writePointer(onDue);

// ---------------------------------------------------------------- install
const SLOTS = [[0x005cc2b4, 'tgt'], [0x005cc0c8, 'tgt'], [0x005cc0c4, 'qpc']];
function install() {
  // input first (inactive until the warp), then the clock slots
  ORIG_CELL.writePointer(Interceptor.replaceFast(RESOLVER, cm.repl));   // replaceFast returns the original's trampoline
  const res = [];
  for (const [va, kind] of SLOTS) {
    const slot = ga(va);
    const real = slot.readPointer();
    const stub = kind === 'tgt' ? tgtStub(cell(real)) : qpcStub(cell(real));
    Memory.protect(slot, 4, 'rw-');
    slot.writePointer(stub);
    res.push([va.toString(16), real.toString(), stub.toString()]);
  }
  schedule(CFG.warpAt, () => {
    warpN = N();
    const pins = warp();
    send({ kind: 'warp', n: warpN, pins });
    setActive(1);
    // The track load (phase 2) takes a run-dependent number of main-thread clock calls (2 frames apart
    // between two runs, measured 2026-11-01), so the scenario timeline is anchored at the first
    // main-thread clock call that sees DAT_00771968 == 3, where both RNGs are pinned again.
    const watch = () => {
      if (ga(0x00771968).readU8() !== 3) {
        // LOAD HOLD: the load took 113 / 118 / 119 / 122 main-thread clock calls in 7 runs (2026-11-01). Sleeping
        // cfg.loadSleepMs of real time in every main-thread clock call until phase 3 gives other threads time to
        // finish whatever the main thread polls, so the number of polls stops depending on their speed.
        if (CFG.loadSleepMs) Thread.sleep(CFG.loadSleepMs / 1000);
        schedule(N() + 1, watch); return;
      }
      raceN = N();
      const pins3 = pinRngs();
      // Re-base the clock: the load takes 118 or 122 main-thread calls (measured 2026-11-01), so without this the
      // race starts at a run-dependent Q. T / Q become the values of an anchor at N = warpAt + 200.
      if (CFG.rebase) {
        const n0 = CFG.warpAt + 200;
        D.writeU32((CFG.t0 + n0 * CFG.step) >>> 0);
        const q = BigInt(CFG.q0) + BigInt(n0) * BigInt(CFG.qstep);
        D.add(0x10).writeU32(Number(q & 0xffffffffn)); D.add(0x14).writeU32(Number(q >> 32n));
      }
      // negative control: +1 on the RenderWare RNG word at the ring cursor (cfg.negctl = 1), or on a VA
      if (CFG.negctl === 1) { const c = rwRngPtr().add(4).readPointer(); c.writeU32((c.readU32() + 1) >>> 0); }
      else if (CFG.negctl) { const p = ga(CFG.negctl); p.writeU32((p.readU32() + 1) >>> 0); }
      send({ kind: 'race', n: raceN, dn_warp: raceN - warpN, frames_7f101c: ga(0x007f101c).readU32(),
             clk_7f0ff0: ga(0x007f0ff0).readU32(), clk_7f0ff4: ga(0x007f0ff4).readU32(), pins: pins3, witness: witness() });
      for (const [dn, c, v] of CFG.input) schedule(raceN + dn, () => { CTL.add(c * 4).writeS32(v); });
      CFG.ckpts.forEach((dn, k) => schedule(raceN + dn, () => checkpoint(k)));
      schedule(raceN + CFG.ckpts[CFG.ckpts.length - 1] + 1, () => { setDue(0xffffffff); send({ kind: 'done', n: N() }); });
    };
    schedule(warpN + 1, watch);
  });
  return res;
}

rpc.exports = {
  phase() { return ga(0x00771968).readU8(); },
  install() { return JSON.stringify(install()); },
  stats() { return { qpf: ga(0x00771e70).readU32(), N: N(), T: D.readU32(), tgt: D.add(0x20).readU32(), qpc: D.add(0x24).readU32(), fwd: D.add(0x28).readU32(), clk: ga(0x007f0ff0).readU32(), phase: ga(0x00771968).readU8() }; },
};
