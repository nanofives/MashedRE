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
//   watch           [va, ...] (at most 2): hardware WRITE watchpoints on every thread from the race anchor on;
//                   each hit sends {kind:'watch', pc, bt, value, n} (pc = the instruction after the store)
//   callLog         log every main-thread clock call from the warp to the race anchor (PROMOTE-DETORIG-LOADHOLD):
//                   64 bytes per call = [N, kind (0 tgt, 1 qpc), esp[0..13]], sent with the race anchor
//   norm, normAt    [[va, width, value], ...] boot-counter pins (re/frida/warp_normalize.tsv, RO-WARP-NORMALIZE),
//                   written at 'warp', 'anchor' or 'both'
'use strict';
const CFG = DETORIG_CFG;
const IMG = 0x400000;
const M = Process.findModuleByName('MASHED.exe');
const ga = (va) => M.base.add(va - IMG);
const keep = [];
const K = (x) => { keep.push(x); return x; };

// ---------------------------------------------------------------- data block
// +0x00 T  +0x04 step  +0x08 due  +0x0c MAIN  +0x10 Qlo  +0x14 Qhi  +0x18 qstep  +0x1c onDue
// +0x20 tgt calls  +0x24 qpc calls  +0x28 forwarded  +0x30 N  +0x34 log on  +0x38 log index
const D = K(Memory.alloc(Process.pageSize));
const LOGCAP = 4096, LOGREC = 64;
const LOG = K(Memory.alloc(LOGCAP * LOGREC));
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

// Call log (cfg.callLog): while [log on] != 0 and the index is below LOGCAP, record N, kind and 14 stack
// dwords from the stub's entry esp (esp[0] = the clock call's return address). Stack reads only.
function logBlock(kind) {
  const b = [0x83, 0x3d, ...d(0x34), 0x00, 0x74, 0x00];                       // cmp [logon],0; je skip
  const jeAt = b.length - 1;
  b.push(0x56, 0x57, 0x51,                                                     // push esi; push edi; push ecx
         0x8b, 0x3d, ...d(0x38), 0x81, 0xff, ...u32(LOGCAP), 0x73, 0x00);      // mov edi,[idx]; cmp edi,CAP; jae done
  const jaeAt = b.length - 1;
  b.push(0xff, 0x05, ...d(0x38), 0xc1, 0xe7, 0x06, 0x81, 0xc7, ...u32(LOG),  // inc [idx]; shl edi,6; add edi,LOG
         0x8b, 0x0d, ...d(0x30), 0x89, 0x0f, 0xc7, 0x47, 0x04, ...u32(kind),   // mov ecx,[N]; mov [edi],ecx; mov [edi+4],kind
         0x8d, 0x74, 0x24, 0x0c, 0x83, 0xc7, 0x08, 0xb9, 14, 0, 0, 0, 0xfc, 0xf3, 0xa5); // lea esi,[esp+12]; add edi,8; mov ecx,14; cld; rep movsd
  b[jaeAt] = b.length - (jaeAt + 1);
  b.push(0x59, 0x5f, 0x5e);                                                    // done: pop ecx; pop edi; pop esi
  b[jeAt] = b.length - (jeAt + 1);
  return b;                                                                    // skip:
}
// main-thread test with a rel32 jne (the log block makes the main-thread path longer than a rel8 reach)
function mainTest() { return [0x64, 0xa1, 0x24, 0, 0, 0, 0x3b, 0x05, ...d(0xc), 0x0f, 0x85, 0, 0, 0, 0]; }
function patchRel32(b, at) { const r = b.length - (at + 4); b[at] = r & 0xff; b[at + 1] = (r >>> 8) & 0xff; b[at + 2] = (r >>> 16) & 0xff; b[at + 3] = (r >>> 24) & 0xff; }

// timeGetTime / GetTickCount: stdcall, no args, value in eax.
function tgtStub(realCell) {
  const b = mainTest();                                                        // mov eax,fs:[24]; cmp eax,[MAIN]; jne real
  const jneAt = b.length - 4;
  b.push(...logBlock(0));
  b.push(0xa1, ...d(0), 0x8b, 0x0d, ...d(4), 0x01, 0x0d, ...d(0),              // mov eax,[T]; mov ecx,[step]; add [T],ecx
         0xff, 0x05, ...d(0x20), 0xff, 0x05, ...d(0x30),                         // inc [tgt]; inc [N]
         0x8b, 0x0d, ...d(0x30), 0x3b, 0x0d, ...d(8), 0x73, 0x01, 0xc3,          // mov ecx,[N]; cmp ecx,[due]; jae slow; ret
         0x50, 0xff, 0x15, ...d(0x1c), 0x58, 0xc3);                              // slow: push eax; call [onDue]; pop eax; ret
  patchRel32(b, jneAt);
  b.push(0xf0, 0xff, 0x05, ...d(0x28), 0xff, 0x25, ...u32(realCell));            // real: lock inc [fwd]; jmp [cell]
  return emit(b);
}
// QueryPerformanceCounter: stdcall (LARGE_INTEGER*), BOOL in eax.
function qpcStub(realCell) {
  const b = mainTest();
  const jneAt = b.length - 4;
  b.push(...logBlock(1));
  b.push(0x8b, 0x44, 0x24, 0x04,                                                 // mov eax,[esp+4]
         0x8b, 0x0d, ...d(0x10), 0x89, 0x08, 0x8b, 0x0d, ...d(0x14), 0x89, 0x48, 0x04, // *eax = Q
         0x8b, 0x0d, ...d(0x18), 0x01, 0x0d, ...d(0x10), 0x83, 0x15, ...d(0x14), 0x00, // Q += qstep
         0xff, 0x05, ...d(0x24), 0xff, 0x05, ...d(0x30),                         // inc [qpc]; inc [N]
         0x8b, 0x0d, ...d(0x30), 0x3b, 0x0d, ...d(8), 0x72, 0x06,                // mov ecx,[N]; cmp ecx,[due]; jb fast
         0xff, 0x15, ...d(0x1c),                                                 // call [onDue]
         0xb8, 1, 0, 0, 0, 0xc2, 0x04, 0x00);                                    // fast: mov eax,1; ret 4
  patchRel32(b, jneAt);
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

// Tick-clock pin (cfg.tick.pin, REPLAY-ORACLE-DESIGN): the frame timer's state was last written from the REAL clock
// before the install. Rewrite it from the virtual clock so the first virtual tick has a delta of one tick:
// DAT_007f1030 last sample and DAT_007f102c 1 s mark (FUN_00493390, stores 0x00493459 / 0x00493478) = the value
// FUN_004950b0 returns now ((Q * 3,000,000) / QPF, QPF = DAT_00771e70, re/match/s16/004950b0.c), DAT_00771984 drift
// = 0 (only FUN_00493390 references it, nondet_scan.txt), and FUN_00492e90's ms marks DAT_00771998 / DAT_0077199c
// = that value / 3000 (0x00492ecd: DAT_007719a0 = FUN_004950b0() / 3000).
function pinTimer() {
  const q = (BigInt(D.add(0x14).readU32()) << 32n) | BigInt(D.add(0x10).readU32());
  const qpf = BigInt(ga(0x00771e70).readU32()) | (BigInt(ga(0x00771e74).readU32()) << 32n);
  const v = Number(((q * 3000000n) / qpf) & 0xffffffffn) >>> 0;
  const before = [0x007f1030, 0x007f102c, 0x00771984, 0x00771998, 0x0077199c].map((a) => ga(a).readU32());
  ga(0x007f1030).writeU32(v); ga(0x007f102c).writeU32(v); ga(0x00771984).writeU32(0);
  ga(0x00771998).writeU32(Math.floor(v / 3000)); ga(0x0077199c).writeU32(Math.floor(v / 3000));
  return { v, before };
}

// Boot-counter normalisation (cfg.norm, RO-WARP-NORMALIZE): the PIN rows of re/frida/warp_normalize.tsv, expanded by
// detorig.py to [[va, width, value], ...]. Each is written on the main thread at the warp and/or at the race anchor
// (cfg.normAt). The writers and readers that justify each row are cited in the TSV.
function pinNorm(where) {
  if (!CFG.norm || !CFG.norm.length || !(CFG.normAt === 'both' || CFG.normAt === where)) return null;
  let changed = 0;
  const sample = [];
  for (const [va, w, v] of CFG.norm) {
    const p = ga(va);
    const old = w === 1 ? p.readU8() : (w === 2 ? p.readU16() : p.readU32());
    if (old !== (v >>> 0)) { changed++; if (sample.length < 8) sample.push([va.toString(16), old]); }
    if (w === 1) p.writeU8(v); else if (w === 2) p.writeU16(v); else p.writeU32(v >>> 0);
  }
  return { at: where, rows: CFG.norm.length, changed, sample };
}

// Audio thread off (cfg.audioOff = 'warp' | 'anchor', RO-AUDIO-ISOLATION). CreateThread (IAT 0x005cc0d0) has one
// caller, CRT __beginthread (0x005c2eca), whose one caller is the audio worker spawner FUN_005aef30 (DESIGN 1.5). So
// every thread whose Win32 start address lies inside MASHED.exe, other than the main thread, is a __beginthread
// worker. They are suspended (SuspendThread) at the chosen point and stay suspended until the process is killed.
// DirectShow / D3D / Frida threads start outside MASHED.exe and are left alone.
function audioOff(where) {
  if (CFG.audioOff !== where) return null;
  const k32 = Process.getModuleByName('kernel32.dll');
  const ntdll = Process.getModuleByName('ntdll.dll');
  const OpenThread = new NativeFunction(k32.getExportByName('OpenThread'), 'pointer', ['uint32', 'int', 'uint32'], 'stdcall');
  const SuspendThread = new NativeFunction(k32.getExportByName('SuspendThread'), 'uint32', ['pointer'], 'stdcall');
  const CloseHandle = new NativeFunction(k32.getExportByName('CloseHandle'), 'int', ['pointer'], 'stdcall');
  const NtQIT = new NativeFunction(ntdll.getExportByName('NtQueryInformationThread'), 'uint32', ['pointer', 'uint32', 'pointer', 'uint32', 'pointer'], 'stdcall');
  const lo = M.base.toUInt32() >>> 0, hi = (lo + M.size) >>> 0;
  const out = { suspended: [], other: [] };
  const buf = Memory.alloc(8);
  for (const t of Process.enumerateThreads()) {
    const h = OpenThread(0x0042, 0, t.id);          // THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION
    if (h.isNull()) { out.other.push([t.id, 'open failed']); continue; }
    buf.writeU32(0);
    NtQIT(h, 9, buf, 4, NULL);                      // ThreadQuerySetWin32StartAddress
    const sa = buf.readU32() >>> 0;
    if (t.id !== CFG.main && sa >= lo && sa < hi) {
      const prev = SuspendThread(h);
      out.suspended.push([t.id, '0x' + (sa - lo + IMG).toString(16), prev]);
    } else out.other.push([t.id, '0x' + sa.toString(16)]);
    CloseHandle(h);
  }
  return out;
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
  // PROV-INSITU (cfg.warpClock = K ticks): the virtual clock at the warp is the start value plus however many ticks the
  // menu ran since the install, and that count is not a function of N (the warp is scheduled on clock calls). Runs one
  // tick apart then pin the frame timer one 50,000-count step apart (0x007f102c / 0x007f1030), and the race reads it:
  // blocks 0x0068f400 / 0x006c2400 differ from tick 2 in every such run. Setting T and Q to fixed values here, before
  // pinTimer, makes the pinned timer the same in every run.
  if (TICK && TICK.pin && CFG.warpClock) {
    const k = BigInt(CFG.warpClock);
    const q = BigInt(CFG.q0) + k * BigInt(TICK.qtick);
    D.add(0x10).writeU32(Number(q & 0xffffffffn)); D.add(0x14).writeU32(Number(q >> 32n));
    D.writeU32((CFG.t0 + CFG.warpClock * TICK.ttick) >>> 0);
    pins.warpClock = { k: CFG.warpClock, q: q.toString() };
  }
  if (TICK && TICK.pin) pins.timer = pinTimer();
  const nw = pinNorm('warp');
  if (nw) pins.norm = nw;
  const ao = audioOff('warp');
  if (ao) pins.audioOff = ao;
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

// ---------------------------------------------------------------- write watch (cfg.watch), port of scenario_launch.py's
// --watch-write: debug-register write watchpoints, armed on every thread at the race anchor. No code patch.
let watchHits = 0;
function watchArm() {
  const addrs = (CFG.watch || []).slice(0, 2).map((va) => ga(va));
  if (!addrs.length) return 'no watch';
  Process.setExceptionHandler((det) => {
    if (det.type !== 'single-step' && det.type !== 'breakpoint') return CR_HANDLER ? CR_HANDLER(det) : false;
    try {
      const c = det.context;
      if (watchHits++ < 2000) {
        let bt = '';
        try { bt = Thread.backtrace(c, Backtracer.ACCURATE).slice(0, 12).map((p) => '0x' + (p.toUInt32() >>> 0).toString(16)).join('|'); } catch (e) { bt = 'bt-err'; }
        send({ kind: 'watch', n: N(), tid: Process.getCurrentThreadId(), pc: '0x' + (c.pc.toUInt32() >>> 0).toString(16), bt,
               values: addrs.map((a) => '0x' + a.readU32().toString(16)) });
      }
    } catch (e) { /* keep running */ }
    return true;
  });
  let n = 0;
  Process.enumerateThreads().forEach((t) => {
    addrs.forEach((a, k) => { try { t.setHardwareWatchpoint(k, a, 4, 'w'); if (k === 0) n++; } catch (e) { } });
  });
  return 'watch armed on ' + n + ' threads: ' + addrs.map((a) => a.toString()).join(',');
}

// ---------------------------------------------------------------- tick clock + per-tick block hashes (REPLAY-ORACLE-DESIGN)
// cfg.tick = {va, qtick, ttick, cap, block}. A native listener (CModule onEnter, no JavaScript) on cfg.tick.va
// (FUN_00493390, the frame timer, entered once per main-loop iteration from FUN_00493480) advances the virtual
// clock by qtick QPC counts / ttick ms. With cfg.qstep = cfg.step = 0 the clock is then a function of the tick
// count only: every main-thread clock read inside one frame returns the same value, so neither the number of
// clock calls per frame nor the number of piz polls during the load can reach a clock-derived value.
// From the race anchor on (tst[0] = 1) the same listener hashes the writable image in cfg.tick.block-byte blocks
// (FNV-1a over dwords, a dword in [0x00995000, 0x30000000) with the low two bits clear is hashed as 0: the
// detorig.is_ptr heap rule), then appends the cfg.tick.raw ranges verbatim and stores [tick, N, hash per block] per tick, up to cfg.tick.cap ticks.
const TICK = CFG.tick || null;
let TK = null;
if (TICK) {
  const nw = (DATA_HI - DATA_LO) >>> 2;
  const bw = TICK.block >>> 2;
  const nblk = Math.ceil(nw / bw);
  const raw = (TICK.raw || []).map(([lo, hi]) => [lo, (hi - lo) >>> 2]);
  const rawW = raw.reduce((a, r) => a + r[1], 0);
  const recw = 2 + nblk + rawW;
  const rawCopy = raw.map(([lo, n]) => `  { const unsigned int *s = (const unsigned int *)0x${ga(lo).toString(16).replace('0x', '')}; for (i = 0; i < ${n}u; i++) rec[o++] = s[i]; }`).join(' ');
  const buf = K(Memory.alloc(TICK.cap * recw * 4));
  const st = K(Memory.alloc(64));
  for (let i = 0; i < 16; i++) st.add(i * 4).writeU32(0);
  const src = `
#include <gum/guminterceptor.h>
extern volatile unsigned int dblk[16];
extern volatile unsigned int tst[16];
extern unsigned int tbuf[];
void onEnter(GumInvocationContext *ic) {
  unsigned long long q = ((unsigned long long)dblk[5] << 32) | dblk[4];
  q += ${TICK.qtick >>> 0}u;
  dblk[4] = (unsigned int)q; dblk[5] = (unsigned int)(q >> 32);
  dblk[0] += ${TICK.ttick >>> 0}u;
  tst[2]++;
  if (!tst[0] || tst[1] >= ${TICK.cap >>> 0}u) return;
  unsigned int *rec = tbuf + tst[1] * ${recw}u;
  rec[0] = tst[2]; rec[1] = dblk[12];
  const unsigned int *p = (const unsigned int *)0x${ga(DATA_LO).toString(16).replace('0x', '')};
  unsigned int b, i, w = 0;
  for (b = 0; b < ${nblk}u; b++) {
    unsigned int h = 2166136261u;
    for (i = 0; i < ${bw}u && w < ${nw}u; i++, w++) {
      unsigned int v = p[w];
      if (v >= 0x00995000u && v < 0x30000000u && !(v & 3u)) v = 0;
      h = (h ^ v) * 16777619u;
    }
    rec[2 + b] = h;
  }
  { unsigned int o = 2 + ${nblk}u;
${rawCopy}
  }
  tst[1]++;
}
`;
  const cmT = K(new CModule(src, { dblk: D, tst: st, tbuf: buf }));
  TK = { nblk, recw, buf, st, cm: cmT };
}
// Stale-field poison (cfg.poison, RO-STACKROW-READERS): the event ring 0x007ec9e4 (256 rows x 0x40, type word
// DAT_007e9de4[row]) is filled from 16 stack dwords per row, and each producer writes only some of them. At the entry
// of every ring consumer (FUN_00422ba0, FUN_0040be50, FUN_004648b0) a native listener overwrites, in every row, the
// fields that row's producer leaves unwritten, so a consumer that read one would see the poison:
//   type 3 (FUN_00469aa0, 0x00469cc8): dwords 10..15 (+0x28..+0x3c)      type 4 (FUN_00469df0): dword 15 (+0x3c)
//   type 8 (FUN_0047def0, 0x0047e5f1 / 0x0047e7b2): dwords 2..8 and 15
// mode 'pos' (positive control) also overwrites type-4 dword 9 (+0x24), which FUN_00469df0 writes and 0x00422e6a reads.
const POISON = CFG.poison || null;
let PZ = null;
if (POISON) {
  const st = K(Memory.alloc(16)); st.writeU32(0); st.add(4).writeU32(0);
  const src = `
#include <gum/guminterceptor.h>
extern volatile unsigned int pst[4];
void onEnter(GumInvocationContext *ic) {
  unsigned int *ring = (unsigned int *)0x${ga(0x007ec9e4).toString(16).replace('0x', '')};
  const unsigned int *typ = (const unsigned int *)0x${ga(0x007e9de4).toString(16).replace('0x', '')};
  unsigned int r, j, v = ${POISON.value >>> 0}u;
  if (!pst[0]) return;
  pst[1]++;
  for (r = 0; r < 256u; r++) {
    unsigned int *row = ring + r * 16u;
    unsigned int t = typ[r];
    if (t == 3u) { for (j = 10; j < 16; j++) row[j] = v; }
    else if (t == 4u) { row[15] = v; ${POISON.mode === 'pos' ? 'row[9] = v;' : ''} }
    else if (t == 8u) { for (j = 2; j < 9; j++) row[j] = v; row[15] = v; }
  }
}
`;
  const cmP = K(new CModule(src, { pst: st }));
  PZ = { st, cm: cmP, at: [0x00422ba0, 0x0040be50, 0x004648b0] };
}

function tickDump() {
  if (!TK) return;
  const n = TK.st.add(4).readU32();
  const recb = TK.recw * 4;
  const per = Math.max(1, Math.floor((4 << 20) / recb));
  for (let i = 0; i < n; i += per) {
    const m = Math.min(per, n - i);
    send({ kind: 'ticks', first: i, count: m, total: n, nblk: TK.nblk, recw: TK.recw, block: TICK.block, raw: TICK.raw || [] },
         TK.buf.add(i * recb).readByteArray(m * recb));
  }
}

// ---------------------------------------------------------------- PROV-INSITU
// cfg.prov.bodies (re/tools/prov/provlink.py): DDK objects of match sources, linked against the original image. At the
// warp each body is copied into executable memory behind a counting stub (`inc dword [cnt_i]` + `jmp body`), and an
// inline JMP rel32 is written at its RVA. cfg.prov.count: RVAs that only get a native call counter (Interceptor +
// CModule, no JS on the hot path), the census of which functions run in this scenario.
const PROV = CFG.prov || null;
let PV = null;
let PV_AT = null;
function provInstall() {
  if (!PROV) return null;
  const bodies = PROV.bodies || [], count = PROV.count || [];
  const nb = bodies.length + count.length;
  // counters and code on separate whole pages: a counter store on a code page costs a self-modifying-code clear per call
  const pg = (n) => Math.ceil(Math.max(n, 1) / Process.pageSize) * Process.pageSize;
  const cnt = K(Memory.alloc(pg(nb * 4 * 2)));   // [0..nb) live counters, [nb..2nb) snapshot at the anchor
  for (let i = 0; i < 2 * nb; i++) cnt.add(i * 4).writeU32(0);
  let total = 0;
  // PROV-WRITESET: cfg.prov.wlog = N makes each stub single-step (TF) the first N calls of its body, see wsInstall
  const WL = PROV.wlog || 0, SZ = WL ? 48 : 16;
  const offs = bodies.map((b) => { const o = total; total += SZ + b.code.length / 2; total = (total + 15) & ~15; return o; });
  const mem = total ? K(Memory.alloc(pg(total))) : null;
  const installed = [];
  bodies.forEach((b, i) => {
    const stub = mem.add(offs[i]);
    const body = stub.add(SZ);
    const code = [];
    for (let k = 0; k < b.code.length; k += 2) code.push(parseInt(b.code.substr(k, 2), 16));
    const dv = new DataView(new ArrayBuffer(code.length)); code.forEach((x, k) => dv.setUint8(k, x));
    const base = body.toUInt32() >>> 0;
    for (const o of b.abs) dv.setUint32(o, (dv.getUint32(o, true) + base) >>> 0, true);
    for (const [o, t] of Object.entries(b.ext)) { const p = base + Number(o) + 4; dv.setInt32(Number(o), (t - p) | 0, true); }
    const ca = cnt.add(i * 4).toUInt32() >>> 0;
    const ent = base + b.entry;
    const le = (v) => [v & 0xff, (v >>> 8) & 0xff, (v >>> 16) & 0xff, (v >>> 24) & 0xff];
    const s = [0xff, 0x05, ...le(ca)];
    if (WL) {   // cmp [cnt], N+1 ; jae skip ; pushfd ; or dword [esp], 0x100 ; popfd ; skip:
      s.push(0x81, 0x3d, ...le(ca), ...le(WL + 1), 0x73, 0x09, 0x9c, 0x81, 0x0c, 0x24, 0x00, 0x01, 0x00, 0x00, 0x9d);
    }
    const jat = s.length;
    s.push(0xe9, 0, 0, 0, 0);
    const rj = (ent - ((stub.toUInt32() >>> 0) + jat + 5)) | 0;
    s.splice(jat + 1, 4, ...le(rj));
    while (s.length < SZ) s.push(0xcc);
    Memory.protect(stub, SZ + code.length, 'rwx');
    stub.writeByteArray(s);
    body.writeByteArray(dv.buffer);
    const site = ga(parseInt(b.rva, 16));
    const before = Array.from(new Uint8Array(site.readByteArray(5)));
    const rel = ((stub.toUInt32() >>> 0) - ((site.toUInt32() >>> 0) + 5)) | 0;
    Memory.patchCode(site, 5, (w) => { w.writeU8(0xe9); w.add(1).writeS32(rel); });
    const after = Array.from(new Uint8Array(site.readByteArray(5)));
    // install check: the site holds E9 to the stub, and the stub's jmp lands on the body entry
    const back = (site.toUInt32() >>> 0) + 5 + site.add(1).readS32();
    const land = (stub.toUInt32() >>> 0) + jat + 5 + stub.add(jat + 1).readS32();
    installed.push({ rva: b.rva, site_before: before.map((x) => x.toString(16)).join(' '), site_after: after.map((x) => x.toString(16)).join(' '),
                     jmp_ok: (back >>> 0) === (stub.toUInt32() >>> 0) && (land >>> 0) === (ent >>> 0), stub: stub.toString(), base: '0x' + (base >>> 0).toString(16), entry: '0x' + (ent >>> 0).toString(16), size: b.code.length / 2 });
  });
  let cm = null;
  if (count.length) {
    cm = K(new CModule(`
#include <gum/guminterceptor.h>
void onEnter(GumInvocationContext *ic) { unsigned int *c = GUM_IC_GET_FUNC_DATA(ic, unsigned int *); (*c)++; }
`));
    count.forEach((rva, j) => {
      const i = bodies.length + j;
      try { Interceptor.attach(ga(parseInt(rva, 16)), cm, cnt.add(i * 4)); installed.push({ rva, count_only: true }); }
      catch (e) { installed.push({ rva, count_only: true, err: '' + e }); }
    });
  }
  PV = { cnt, nb, names: bodies.map((b) => b.rva).concat(count), installed };
  if (WL && mem) wsInstall(mem, total, WL);
  send({ kind: 'provinst', installed });
  return { n: nb, bodies: bodies.length, count: count.length, fail: installed.filter((x) => x.err || x.jmp_ok === false).length };
}
// PROV-WRITESET-114: single-step log of the DDK bodies. A native vectored handler (CModule, no JS per step) logs
// eip + registers before every instruction of a body (blob range [lo, hi)), steps over calls to code outside the blob
// (int3 at the return address, back in the blob) and ends a frame at the body's own return. Records of 10 dwords:
// eip, eax, ecx, edx, ebx, esp, ebp, esi, edi, base_esp | 1 on a frame's first record (base_esp = esp at body entry,
// pointing at the return address). The store targets are computed offline (re/tools/prov/prov_writeset.py).
let WS = null;
function wsInstall(mem, total, WL) {
  const CAP = 1 << 21;   // records
  const buf = K(Memory.alloc(CAP * 40));
  const stm = K(Memory.alloc(Process.pageSize));
  for (let i = 0; i < 192; i++) stm.add(i * 4).writeU32(0);
  const cm = K(new CModule(`
extern unsigned int st[];   /* 0 buf, 1 cap, 2 count, 3 lo, 4 hi, 5 depth, 6 overflow, 7 traps, 8 frames, 9 foreign, 10 bp misses; plain memory from Memory.alloc, a CModule's own globals are not writable */
#define fb (st + 64)   /* frame base esp */
#define fs (st + 96)   /* frame suspended (return address patched) */
#define fa (st + 128)  /* patched address */
#define fo (st + 160)  /* original byte */
static void rec(unsigned char *cx, unsigned int first) {
  unsigned int n = st[2];
  if (n >= st[1]) { st[6]++; return; }
  unsigned int *r = (unsigned int *)(st[0] + n * 40);
  r[0] = *(unsigned int *)(cx + 0xB8); r[1] = *(unsigned int *)(cx + 0xB0); r[2] = *(unsigned int *)(cx + 0xAC);
  r[3] = *(unsigned int *)(cx + 0xA8); r[4] = *(unsigned int *)(cx + 0xA4); r[5] = *(unsigned int *)(cx + 0xC4);
  r[6] = *(unsigned int *)(cx + 0xB4); r[7] = *(unsigned int *)(cx + 0xA0); r[8] = *(unsigned int *)(cx + 0x9C);
  r[9] = fb[st[5] - 1] | first;
  st[2] = n + 1;
}
int __attribute__((stdcall)) veh(unsigned int *ep) {   /* the OS calls a vectored handler stdcall */
  unsigned int *er = (unsigned int *)ep[0];
  unsigned char *cx = (unsigned char *)ep[1];
  unsigned int code = er[0];
  unsigned int *eip = (unsigned int *)(cx + 0xB8), *efl = (unsigned int *)(cx + 0xC0), *esp = (unsigned int *)(cx + 0xC4);
  unsigned int ip = *eip, d = st[5];
  unsigned int tid = ((unsigned int (__attribute__((stdcall)) *)(void))st[16])();
  if (code == 0x80000004u) {
    int inb = ip >= st[3] && ip < st[4];
    st[7]++;
    if (tid != st[17]) { *efl &= ~0x100u; st[15]++; return -1; }   /* only the main thread is logged: the frame state is global */
    if (inb) {
      if (d == 0 || fs[d - 1]) {
        if (d >= 31) { st[9]++; *efl &= ~0x100u; return -1; }
        fb[d] = *esp; fs[d] = 0; st[5] = d + 1; st[8]++;
        rec(cx, 1);
      } else rec(cx, 0);
      *efl |= 0x100u; return -1;
    }
    if (d == 0 || fs[d - 1]) { st[9]++; st[11] = ip; st[12] = d; *efl &= ~0x100u; return -1; }   /* stray step: counted, TF cleared */
    {
      unsigned int top = *esp;
      unsigned int rv = (top >= 0x10000u) ? *(unsigned int *)top : 0;
      if (*esp < fb[d - 1] && rv >= st[3] && rv < st[4]) {
        unsigned char *pa = (unsigned char *)rv;
        fa[d - 1] = rv; fo[d - 1] = *pa; *pa = 0xCC; fs[d - 1] = 1;
        *efl &= ~0x100u; return -1;
      }
      st[5] = d - 1; *efl &= ~0x100u; return -1;
    }
  }
  if (code == 0x80000003u) {
    unsigned int a = ip, i;
    if (tid != st[17]) return 0;   /* Windows reports Eip at the int3 byte itself */
    for (i = st[5]; i > 0; i--) {
      if (fs[i - 1] && fa[i - 1] == a) {
        *(unsigned char *)a = (unsigned char)fo[i - 1]; fs[i - 1] = 0;
        st[5] = i;
        *eip = a; *efl |= 0x100u; return -1;
      }
    }
    st[10]++;
  }
  return 0;
}
`, { st: stm }));
  const lo = mem.toUInt32() >>> 0;
  const st = stm;
  st.writePointer(buf); st.add(4).writeU32(CAP); st.add(8).writeU32(0);
  st.add(12).writeU32(lo); st.add(16).writeU32(lo + total);
  st.add(16 * 4).writeU32(Process.getModuleByName('kernel32.dll').getExportByName('GetCurrentThreadId').toUInt32());
  st.add(17 * 4).writeU32(CFG.main >>> 0);
  const add = new NativeFunction(Process.getModuleByName('kernel32.dll').getExportByName('AddVectoredExceptionHandler'), 'pointer', ['uint', 'pointer']);
  const vk = Object.keys(cm).find((k) => /^_?veh/.test(k));   // stdcall may decorate the symbol (veh@4)
  if (!vk) throw new Error('veh not exported: ' + Object.keys(cm));
  const h = add(1, cm[vk]);
  WS = { cm, buf, st, lo, hi: lo + total, handler: h.toString() };
  if (CFG.prov.wlive) {   // live breadcrumbs: the process can die inside the handler, so a JS timer keeps the last state on disk
    const lf = new File(CFG.prov.wlive, 'w');
    let lastN = -1, lastT = -1;
    setInterval(() => {
      const n = st.add(8).readU32(), tr = st.add(28).readU32();
      if (n === lastN && tr === lastT) return;
      lastN = n; lastT = tr;
      const stats = []; for (let i = 0; i < 18; i++) stats.push(st.add(i * 4).readU32());
      const m = Math.min(n, 12), rows = [];
      for (let i = n - m; i < n; i++) { const r = []; for (let k = 0; k < 10; k++) r.push(buf.add(i * 40 + k * 4).readU32().toString(16)); rows.push(r.join(' ')); }
      lf.write(JSON.stringify({ stats, rows }) + String.fromCharCode(10)); lf.flush();
    }, 25);
  }
  Process.setExceptionHandler((d) => {   // crash report: the fault, our counters and the last 400 records
    const n = st.add(8).readU32(), m = Math.min(n, 400);
    const stats = []; for (let i = 0; i < 18; i++) stats.push(st.add(i * 4).readU32());
    const c = d.context;
    send({ kind: 'wcrash', type: d.type, address: '' + d.address, memory: d.memory ? JSON.stringify(d.memory) : null,
           ctx: { eip: '' + c.eip, esp: '' + c.esp, eax: '' + c.eax, ecx: '' + c.ecx, edx: '' + c.edx, ebx: '' + c.ebx, ebp: '' + c.ebp, esi: '' + c.esi, edi: '' + c.edi },
           stats, blob: [lo, lo + total], n }, buf.add((n - m) * 40).readByteArray(m * 40));
    return false;
  });
}
function wsDump() {
  if (!WS) return;
  const n = WS.st.add(8).readU32();
  const stats = []; for (let i = 0; i < 18; i++) stats.push(WS.st.add(i * 4).readU32());
  const CH = 1 << 16;
  for (let i = 0; i < n; i += CH) {
    const m = Math.min(CH, n - i);
    send({ kind: 'wlog', first: i, count: m, total: n, stats }, WS.buf.add(i * 40).readByteArray(m * 40));
  }
  if (!n) send({ kind: 'wlog', first: 0, count: 0, total: 0, stats }, new ArrayBuffer(0));
}
function provSnap() { if (PV) for (let i = 0; i < PV.nb; i++) PV.cnt.add((PV.nb + i) * 4).writeU32(PV.cnt.add(i * 4).readU32()); }
function provReport() {
  if (!PV) return;
  const rows = PV.names.map((rva, i) => ({ rva, calls_before_warp: PV.atWarp ? PV.atWarp[i] : null, calls_to_anchor: PV.cnt.add((PV.nb + i) * 4).readU32(), calls_total: PV.cnt.add(i * 4).readU32() }));
  send({ kind: 'prov', rows, installed: PV.installed });
}

// ---------------------------------------------------------------- PROV-CRASH-12 first-fault catcher (cfg.crash)
// An exception handler (no Interceptor) records the first real fault: type, faulting address, memory op, all registers,
// 64 stack dwords from esp, 32 code bytes from eip-16, and which --prov blob holds eip. A JS timer samples the main
// thread's context 40 times when N (main-thread clock calls) has not moved for cfg.crash.stall seconds (a hang).
// Each record is written to cfg.crash.file synchronously, then sent: the process can die before a send is delivered.
let CR_FILE = null, CR_HANDLER = null, CR_DONE = 0;
function crWrite(rec) {
  try { if (!CR_FILE) CR_FILE = new File(CFG.crash.file, 'a'); CR_FILE.write(JSON.stringify(rec) + String.fromCharCode(10)); CR_FILE.flush(); } catch (e) { }
  try { send(Object.assign({ kind: 'crash' }, rec)); } catch (e) { }
}
function crHex(p) { return '0x' + (p.toUInt32() >>> 0).toString(16); }
function crWhere(v) {
  v = v >>> 0;
  if (PV && PV.installed) for (const b of PV.installed) {
    if (!b.base) continue;
    const lo = parseInt(b.base, 16), st = parseInt(b.stub, 16);
    if (v >= lo && v < lo + b.size) return { rva: b.rva, off: '0x' + (v - lo).toString(16) };
    if (v >= st && v < lo) return { rva: b.rva, stub: '0x' + (v - st).toString(16) };
  }
  const m = Process.findModuleByAddress(ptr(v));
  return m ? { mod: m.name, off: '0x' + (v - (m.base.toUInt32() >>> 0)).toString(16) } : null;
}
function crCtx(c) {
  const r = {};
  for (const k of ['eip', 'esp', 'ebp', 'eax', 'ecx', 'edx', 'ebx', 'esi', 'edi']) r[k] = crHex(c[k]);
  let stack = [];
  try { const sp = c.esp; for (let i = 0; i < 64; i++) { const v = sp.add(i * 4).readU32() >>> 0; const w = crWhere(v); stack.push(w ? ['0x' + v.toString(16), w] : '0x' + v.toString(16)); } } catch (e) { stack.push('stack-err ' + e); }
  let code = '';
  try { code = Array.from(new Uint8Array(c.eip.sub(16).readByteArray(32))).map((x) => (x < 16 ? '0' : '') + x.toString(16)).join(' '); } catch (e) { code = 'code-err'; }
  return { ctx: r, stack, code, eip: r.eip, eip_in: crWhere(c.eip.toUInt32()) };
}
function crashArm() {
  if (!CFG.crash) return null;
  CR_HANDLER = (d) => {
    if (d.type === 'breakpoint' || d.type === 'single-step' || d.type === 'system') return false;
    if (CR_DONE++ < 4) {
      try { crWrite(Object.assign({ what: 'fault', tid: Process.getCurrentThreadId(), n: N(), type: d.type, address: '' + d.address, memory: d.memory ? { op: d.memory.operation, address: '' + d.memory.address } : null }, crCtx(d.context))); } catch (e) { crWrite({ what: 'fault-err', msg: '' + e }); }
    }
    return false;
  };
  Process.setExceptionHandler(CR_HANDLER);
  let lastN = -1, since = Date.now(), sampled = false;
  setInterval(() => {
    const n = N();
    if (n !== lastN) { lastN = n; since = Date.now(); return; }
    if (sampled || Date.now() - since < CFG.crash.stall * 1000) return;
    sampled = true;
    for (let i = 0; i < 40; i++) {
      try {
        const t = Process.enumerateThreads().find((x) => x.id === CFG.main);
        if (t) crWrite(Object.assign({ what: 'stall', k: i, n, tid: t.id, state: t.state }, crCtx(t.context)));
      } catch (e) { crWrite({ what: 'stall-err', k: i, msg: '' + e }); }
      Thread.sleep(0.05);
    }
  }, 250);
  return 'crash catcher armed, stall ' + CFG.crash.stall + ' s';
}

// ---------------------------------------------------------------- install
const SLOTS =[[0x005cc2b4, 'tgt'], [0x005cc0c8, 'tgt'], [0x005cc0c4, 'qpc']];
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
  if (TK) { Interceptor.attach(ga(TICK.va), TK.cm); res.push(['tick', TICK.va.toString(16)]); }
  // PROV-INSITU bodies / counters go live here, at the menu, not at the warp: work done inside the warp's clock call
  // (14 patchCode calls, or 326 Interceptor attaches) moved the frame-timer phase by one tick in 2 of 4 runs
  PV_AT = provInstall();
  if (PV_AT) res.push(['prov', PV_AT]);
  const CRA = crashArm();
  if (CRA) res.push(['crash', CRA]);
  if (PZ) { for (const va of PZ.at) Interceptor.attach(ga(va), PZ.cm); res.push(['poison', POISON.mode, POISON.value]); }
  schedule(CFG.warpAt, () => {
    warpN = N();
    const pins = warp();
    if (PV_AT) pins.prov = PV_AT;
    if (PV) PV.atWarp = PV.names.map((_, i) => PV.cnt.add(i * 4).readU32());
    if (CFG.callLog) D.add(0x34).writeU32(1);
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
      provSnap();
      if (TK) TK.st.writeU32(1);
      if (PZ) PZ.st.writeU32(1);
      if (CFG.callLog && !CFG.callLogRace) {
        D.add(0x34).writeU32(0);
        const cnt = D.add(0x38).readU32();
        send({ kind: 'calllog', count: cnt, rec: LOGREC }, LOG.readByteArray(cnt * LOGREC));
      }
      const pins3 = pinRngs();
      const na = pinNorm('anchor');
      if (na) pins3.norm = na;
      const ao3 = audioOff('anchor');
      if (ao3) pins3.audioOff = ao3;
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
      send({ kind: 'race', n: raceN, dn_warp: raceN - warpN, tick: TK ? TK.st.add(8).readU32() : null, frames_7f101c: ga(0x007f101c).readU32(),
             clk_7f0ff0: ga(0x007f0ff0).readU32(), clk_7f0ff4: ga(0x007f0ff4).readU32(), pins: pins3, witness: witness() });
      if (CFG.watch && CFG.watch.length) send({ kind: 'info', msg: watchArm() });
      for (const [dn, c, v] of CFG.input) schedule(raceN + dn, () => { CTL.add(c * 4).writeS32(v); });
      CFG.ckpts.forEach((dn, k) => schedule(raceN + dn, () => checkpoint(k)));
      schedule(raceN + CFG.ckpts[CFG.ckpts.length - 1] + 1, () => {
        setDue(0xffffffff);
        if (TK) TK.st.writeU32(0);
        if (CFG.callLog && CFG.callLogRace) {
          D.add(0x34).writeU32(0);
          const cnt = D.add(0x38).readU32();
          send({ kind: 'calllog', count: cnt, rec: LOGREC }, LOG.readByteArray(cnt * LOGREC));
        }
        tickDump();
        provReport();
        wsDump();
        if (PZ) { PZ.st.writeU32(0); send({ kind: 'info', msg: 'poison listener calls ' + PZ.st.add(4).readU32() }); }
        send({ kind: 'done', n: N() });
      });
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
