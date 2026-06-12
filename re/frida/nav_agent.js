// Shared menu-navigation agent (statenav family). Single copy used by
// scenario_attach_probe.py and run_diff.py's scenario:'race' lane; the
// parity_shots.py / race_refs.py inline copies predate this file.
//
// Input is the in-process FUN_00497310 return-override (player 0 only;
// control codes 4=confirm, 11=up, 12=down). State polling: DAT_0067e9f8
// menu-stack depth, DAT_0067eca4 phase (3=menu; leaves 3 when a race
// loads), per-depth cursor write at DAT_0067ed80 + (depth-1)*0x40.
'use strict';
const IMG = 0x00400000; let DELTA = 0;
const RVA_RES = 0x00497310, RVA_DEPTH = 0x0067e9f8, RVA_PHASE = 0x0067eca4;
let pressCtrl = -1, pressUntil = 0;
function abs(r) { return ptr(r + DELTA); }
rpc.exports = {
  init: function () {
    const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMG;
    Interceptor.attach(abs(RVA_RES), {
      onEnter(a) { const sp = this.context.esp; this.p = sp.add(4).readS32(); this.c = sp.add(8).readS32(); },
      onLeave(ret) { if (this.p === 0 && this.c === pressCtrl && Date.now() < pressUntil) ret.replace(ptr(0xff)); }
    });
    return DELTA;
  },
  press: function (c, ms) { pressCtrl = c; pressUntil = Date.now() + ms; return 1; },
  depth: function () { try { return abs(RVA_DEPTH).readS32(); } catch (e) { return -999; } },
  phase: function () { try { return abs(RVA_PHASE).readS32(); } catch (e) { return -999; } },
  setsel: function (v) { try { const d = abs(RVA_DEPTH).readS32(); abs(0x0067ed80 + (d - 1) * 0x40).writeS32(v); return 1; } catch (e) { return 0; } },
  snap: function (addrs) { return addrs.map(a => { try { return abs(a).readU32(); } catch (e) { return null; } }); }
};
send({ kind: 'ready' });
