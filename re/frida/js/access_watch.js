'use strict';
// --access-watch (RO-PIZFS-READERS-RUN, 2026-11-14): sampled READ/WRITE accessors of one image address range on the
// ORIGINAL, by guard pages (MemoryAccessMonitor). Read-only: no Interceptor, no code patch, no memory write.
// A guard page fires once per arm, so the range is re-armed every PERIOD ms: each cycle records at most the FIRST
// access to each page, filtered to [lo, hi). Accessors that never come first on a page inside a cycle are missed.
// Each hit: window label, pc (image rva or module!off), op, address rva, and the first 3 image .text dwords found
// on the stack (candidate return addresses, stale ones possible) when the details carry a context.
const IMG = 0x00400000;
const M = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
const D = M.base.toUInt32() - IMG;
const S = { rs: [], lo: 0, hi: 0, period: 25, label: 'boot', armed: false, cycles: 0, cb: 0, inr: 0, outr: 0,
            hits: {}, err: null, timer: null, firstSeen: {} };
function hx(v) { return '0x' + (v >>> 0).toString(16).padStart(8, '0'); }
function pcName(p) {
  const u = p.toUInt32() >>> 0;
  if (u >= M.base.toUInt32() && u < M.base.toUInt32() + M.size) return hx(u - D);
  const m = Process.findModuleByAddress(p);
  return m ? m.name + '!' + hx(u - m.base.toUInt32()) : hx(u);
}
function stackScan(c) {
  const out = [];
  try {
    for (let k = 0; k < 64 && out.length < 3; k++) {
      const x = c.esp.add(k * 4).readU32() >>> 0;
      const r = x - D;
      if (r >= 0x401000 && r < 0x5c0000) out.push(hx(r));
    }
  } catch (e) { out.push('scan-err'); }
  return out.join('|');
}
function onAccess(d) {
  S.cb++;
  const a = (d.address.toUInt32() >>> 0) - D;
  if (!S.rs.some(function (r) { return a >= r[0] && a < r[1]; })) { S.outr++; return; }
  S.inr++;
  let bt = '';
  if (d.context) bt = stackScan(d.context);
  const key = S.label + '\t' + pcName(d.from) + '\t' + d.operation + '\t' + hx(a) + '\t' + bt;
  S.hits[key] = (S.hits[key] || 0) + 1; if (S.f && S.hits[key] === 1) { try { S.f.write(key + String.fromCharCode(10)); S.f.flush(); } catch (e) {} }
  const fk = pcName(d.from) + '\t' + d.operation;
  if (!(fk in S.firstSeen)) S.firstSeen[fk] = S.label + '@cycle' + S.cycles;
}
function arm() {
  const pr = S.rs.map(function (r) { const pl = (r[0] + D) & ~0xfff, ph = ((r[1] + D) + 0xfff) & ~0xfff;
                                      return { base: ptr(pl), size: ph - pl }; });
  try { MemoryAccessMonitor.disable(); } catch (e) {}
  try {
    MemoryAccessMonitor.enable(pr, { onAccess: onAccess });
    S.cycles++;
  } catch (e) { if (!S.err) S.err = 'arm ' + e; }
}
// Hardware mode (lo = 'hw', hi = comma list of up to 4 rvas): debug-register READ/WRITE watchpoints, 4 bytes each,
// on every thread enumerated at arm time (re-armed on every label change). Catches every access on armed threads.
// The trap fires after the instruction, so pc is the address of the NEXT instruction (column pc_after).
const HW = { on: false, addrs: [], rvas: [], err: null, threads: 0 };
function hwArm() {
  let n = 0;
  Process.enumerateThreads().forEach(function (t) {
    for (let k = 0; k < HW.addrs.length; k++) {
      try { t.setHardwareWatchpoint(k, HW.addrs[k], 4, 'rw'); if (k === 0) n++; }
      catch (e) { if (!HW.err) HW.err = 'arm tid ' + t.id + ' ' + e.message; }
    }
  });
  HW.threads = n; return n;
}
function hwStart(csv) {
  HW.rvas = csv.split(',').filter(x => x).map(x => parseInt(x, 16)).slice(0, 4);
  HW.addrs = HW.rvas.map(r => ptr(r + D));
  Process.setExceptionHandler(function (d) {
    if (d.type !== 'single-step' && d.type !== 'breakpoint') return false;
    try {
      const c = d.context;
      // DR6 is not exposed: report every armed address whose value context is ambiguous as the watch set
      const key = S.label + '\t' + pcName(c.pc) + '\tpc_after\t' + HW.rvas.map(hx).join(',') + '\t' + stackScan(c);
      S.hits[key] = (S.hits[key] || 0) + 1; if (S.f && S.hits[key] === 1) { try { S.f.write(key + String.fromCharCode(10)); S.f.flush(); } catch (e) {} } S.inr++;
      const fk = pcName(c.pc) + '\tpc_after';
      if (!(fk in S.firstSeen)) S.firstSeen[fk] = S.label;
    } catch (e) { if (!S.err) S.err = 'hit ' + e; }
    return true;
  });
  HW.on = true;
  S.timer = setInterval(hwArm, 1000);   // cover threads created after the first arm
  return 'hw watch rw on ' + hwArm() + ' threads: ' + HW.rvas.map(hx).join(',');
}
rpc.exports = {
  start: function (lo, hi, period, stream) {
    if (stream) { try { S.f = new File(stream, 'w'); } catch (e) { S.err = 'stream ' + e; } }
    if (lo === 'hw') return hwStart(hi);
    // lo:hi, or several ranges joined by '+' inside hi's string ('lo:hi+lo2:hi2' arrives as lo, 'hi+lo2:hi2')
    const spec = (lo + ':' + hi).split('+');
    S.rs = spec.map(function (x) { const q = x.split(':'); return [parseInt(q[0], 16), parseInt(q[1], 16)]; });
    S.lo = S.rs[0][0]; S.hi = S.rs[S.rs.length - 1][1]; S.period = period;
    arm(); S.armed = true;
    S.timer = setInterval(arm, S.period);
    return 'access watch ' + hx(S.lo) + '..' + hx(S.hi) + ' period ' + period + ' ms, pages ' +
           ((((S.hi + 0xfff) & ~0xfff) - (S.lo & ~0xfff)) >>> 12);
  },
  label: function (l) { S.label = l; if (HW.on) return l + ' (hw re-armed on ' + hwArm() + ' threads, err=' + HW.err + ')'; return l; },
  stop: function () {
    if (S.timer) clearInterval(S.timer); S.timer = null;
    try { MemoryAccessMonitor.disable(); } catch (e) {}
    return 'stopped';
  },
  stats: function () { return JSON.stringify({ cycles: S.cycles, callbacks: S.cb, in_range: S.inr, out_of_range: S.outr,
                                              keys: Object.keys(S.hits).length, err: S.err, label: S.label }); },
  drain: function () { return { hits: S.hits, first: S.firstSeen }; },
};
