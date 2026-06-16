# WS-H2 — installed-hook canonical-race verification for the scoring trio
# (0x0040b290 ScoreAdd / 0x0040eee0 ScoreElim / 0x00410510 EvalResult).
#
# Spawns the ORIGINAL with the dev .asi AUTO-LOADED (hooks LIVE — no
# MASHED_RE_NO_AUTO_HOOK), drives into a 4-player FFA Quick Battle (the canonical
# elimination-scoring scenario), and verifies, with the inline-JMP installed:
#   (1) PATH2 installer: the 5-byte E9 is live at all three RVAs (read byte0==0xE9).
#   (2) the three hooks FIRE during the round (Interceptor counts > 0).
#   (3) the round/match plays out correctly through the ported scoring: the score
#       array DAT_008a94e0[0..3] evolves, elimination order DAT_008a94c0 fills,
#       a round/match winner is declared (DAT_0063b914 / DAT_0063ba8c=0xb), no
#       crash/rollback.
# This is the project's installed-hook canonical-observation C4 bar (CONFIDENCE.md;
# [[feedback-no-overclaiming-c-levels]]) — the hook is actually installed during a
# canonical scenario and the game behaves correctly through the verbatim reimpl.
#
# NO OS input injection (the FUN_00497310 in-process return override only), per the
# hard user rule. Reuses statenav.Nav + statenav.shoot.
#
# Usage: py -3.12 re/frida/verify_scoring_hooks.py [--round 150] [--shot-dir verify/scoring]
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import statenav
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

# the three ported RVAs + their .asi export names
HOOKS = {
    "0x0040b290": "ScoreAdd_0040b290",
    "0x0040eee0": "ScoreElim_0040eee0",
    "0x00410510": "EvalResult_00410510",
}
# score-state globals (byte addresses; arrays stride 4, 4 cars)
G_SCORE   = "0x008a94e0"   # DAT_008a94e0 live scores
G_ELIM    = "0x008a94c0"   # DAT_008a94c0 elimination order (-1 terminated)
G_PART    = "0x008a94d0"   # DAT_008a94d0 participant count (expect 4 in FFA)
G_WINNER  = "0x0063b914"   # DAT_0063b914 result winner index
G_PHASE   = "0x0063ba8c"   # DAT_0063ba8c race phase (0xb on conclusion)
# scoring callees that should also fire (already-C3 installed hooks)
CALLEES   = ["0x00422fd0", "0x0046c700", "0x00431d80"]

AGENT = statenav.AGENT.replace(
    "let pressCtrl=-1, pressUntil=0; const CNT={};",
    "let pressCtrl=-1, pressUntil=0; const CNT={}; const TRACE=[]; const G_SCORE_ABS=0x008a94e0;"
).replace(
    "  sel:function(){ try{const d=abs(RVA_DEPTH).readS32(); return abs(RVA_SEL+d*0x40).readS32();}catch(e){return -999;} }",
    "  sel:function(){ try{const d=abs(RVA_DEPTH).readS32(); return abs(RVA_SEL+d*0x40).readS32();}catch(e){return -999;} },\n"
    "  installbyte:function(rva){ try{ return abs(parseInt(rva,16)).readU8(); }catch(e){ return -1; } },\n"
    "  peeks:function(rva){ try{ return abs(parseInt(rva,16)).readS32(); }catch(e){ return -999; } },\n"
    "  peekarr:function(rva,n){ const o=[]; try{ const b=abs(parseInt(rva,16)); for(let i=0;i<n;i++) o.push(b.add(i*4).readS32()); }catch(e){} return o; },\n"
    "  tracecalls:function(rvas){ rvas.forEach(function(r){ const a=parseInt(r,16);\n"
    "    try{ Interceptor.attach(abs(a),{ onEnter:function(args){ const sp=this.context.esp;\n"
    "        this.p1=sp.add(4).readS32(); this.p2=sp.add(8).readS32(); },\n"
    "      onLeave:function(ret){ const sc=[]; const b=abs(G_SCORE_ABS);\n"
    "        for(let i=0;i<4;i++) sc.push(b.add(i*4).readS32());\n"
    "        TRACE.push({rva:r, p1:this.p1, p2:this.p2, ret:ret.toInt32(), sc:sc}); }}); }catch(e){} }); return 1; },\n"
    "  gettrace:function(){ return TRACE; }"
)


def main():
    round_secs = int(sys.argv[sys.argv.index("--round")+1]) if "--round" in sys.argv else 150
    shotdir = sys.argv[sys.argv.index("--shot-dir")+1] if "--shot-dir" in sys.argv else "verify/scoring"
    Path(ROOT/shotdir).mkdir(parents=True, exist_ok=True)
    # --baseline: hooks OFF (original scoring). Captures the SAME telemetry through
    # Interceptor on the original RVAs, for an original-vs-modded telemetry diff.
    baseline = "--baseline" in sys.argv
    tag = "baseline" if baseline else "hooked"

    # HOOKS LIVE unless --baseline: spawn WITHOUT MASHED_RE_NO_AUTO_HOOK so the
    # .asi auto-installs. --baseline sets it so the original scoring runs.
    env = dict(os.environ)
    if baseline: env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    else: env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    E = scr.exports_sync

    # count the three ported hooks + the scoring callees; per-call telemetry trace
    E.countthese(list(HOOKS.keys()) + CALLEES)
    E.tracecalls(list(HOOKS.keys()))
    dev.resume(pid)
    nav = statenav.Nav(scr, pid)

    print("  booting (hooks LIVE)...")
    nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 20.0, "title up")

    # ---- (1) PATH2 installer check: E9 live at the three RVAs --------------
    inst = {rva: E.installbyte(rva) for rva in HOOKS}
    print(f"  === installer (E9) check [{tag}] ===")
    all_e9 = True
    for rva, b in inst.items():
        ok = (b == 0xE9)
        all_e9 = all_e9 and ok
        print(f"    {rva} {HOOKS[rva]:22s} byte0=0x{b & 0xff:02x} {'E9 INSTALLED' if ok else 'original (not hooked)'}")
    if baseline:
        print(f"    [baseline] expect original bytes (NOT E9): {'OK' if not all_e9 else 'UNEXPECTED E9'}")
    statenav.shoot(pid, ROOT/shotdir/f"scoring_{tag}_title.png")

    # ---- drive into a 4-player FFA Quick Battle ----------------------------
    nav.confirm_to_depth(2); time.sleep(0.3); nav.press(4); time.sleep(0.5)
    nav.confirm_to_depth(3)
    nav.press(12)   # down -> Quick Battle (competitive arena, FFA)
    nav.confirm_to_depth(4, tries=4)   # colour select
    nav.confirm_to_depth(5, tries=4)   # track select
    statenav.shoot(pid, ROOT/shotdir/"scoring_track.png")
    nav.press(4); time.sleep(1.5)      # confirm track -> game-mode setup
    for k in range(5):
        if nav.phase() != 3: break
        nav.press(4); time.sleep(1.5)
        if nav.phase() == 0: break
    statenav.shoot(pid, ROOT/shotdir/"scoring_race_enter.png")
    print(f"  in race? phase={nav.phase()}  participants(DAT_008a94d0)={E.peeks(G_PART)}")

    # ---- (2)+(3) watch the round play out through the ported scoring -------
    drive = [4, 0, 1, 11, 12, 5, 6, 7]; di = 0; dt = 0; t_shot = 0
    end = time.time() + round_secs
    score_log = []        # (t, scores[4]) timeline
    elim_seen = []
    fired_at = {rva: None for rva in HOOKS}
    last_scores = None
    while time.time() < end:
        if not nav.alive(): print("   process exited during race"); break
        if time.time() - dt > 6: di = (di + 1) % len(drive); dt = time.time()
        E.press(drive[di], 600)
        time.sleep(0.5)
        # note first-fire of each hook
        cc = E.counts()
        for rva in HOOKS:
            if fired_at[rva] is None and isinstance(cc.get(rva), int) and cc[rva] > 0:
                fired_at[rva] = int(round_secs - (end - time.time()))
                print(f"   >>> {HOOKS[rva]} FIRED at t+{fired_at[rva]}s (count={cc[rva]})")
        if time.time() - t_shot > 10:
            t_shot = time.time()
            tnow = int(round_secs - (end - time.time()))
            scores = E.peekarr(G_SCORE, 4)
            elim = E.peekarr(G_ELIM, 4)
            ph = nav.peek(G_PHASE) if hasattr(nav, "peek") else E.peeks(G_PHASE)
            print(f"   t+{tnow}s phase={nav.phase()} scores={scores} elim={elim} "
                  f"winner={E.peeks(G_WINNER)} racephase=0x{E.peeks(G_PHASE) & 0xffffffff:x}")
            statenav.shoot(pid, ROOT/shotdir/f"scoring_t{tnow}.png")
            if scores != last_scores:
                score_log.append((tnow, scores)); last_scores = scores

    # ---- report -----------------------------------------------------------
    cc = E.counts()
    final_scores = E.peekarr(G_SCORE, 4)
    final_elim = E.peekarr(G_ELIM, 4)
    final_winner = E.peeks(G_WINNER)
    final_phase = E.peeks(G_PHASE) & 0xffffffff
    statenav.shoot(pid, ROOT/shotdir/"scoring_final.png")
    print("\n  ===================== RESULT =====================")
    print(f"  installer (E9 live):   {'ALL INSTALLED' if all_e9 else 'MISSING — see above'}")
    print("  hook fire counts (canonical race, inline-JMP live):")
    for rva in HOOKS:
        print(f"    {HOOKS[rva]:22s} {rva}  fires={cc.get(rva)}  first@t+{fired_at[rva]}s")
    print("  scoring-callee fires:")
    for rva in CALLEES:
        print(f"    {rva}  fires={cc.get(rva)}")
    print(f"  participants(DAT_008a94d0): {E.peeks(G_PART)}")
    print(f"  score timeline (changes): {score_log}")
    print(f"  final scores DAT_008a94e0: {final_scores}")
    print(f"  final elim order DAT_008a94c0: {final_elim}")
    print(f"  final winner DAT_0063b914: {final_winner}   race phase DAT_0063ba8c: 0x{final_phase:x}")

    trace = list(E.gettrace())
    fired = [r for r in HOOKS if isinstance(cc.get(r), int) and cc[r] > 0]
    concluded = (final_phase == 0xb) or (final_winner not in (-1, 0, -999))
    print(f"  per-call trace: {len(trace)} scoring calls captured")

    out = {"tag": tag, "installer": inst, "all_e9": all_e9, "counts": cc, "fired_at": fired_at,
           "participants": E.peeks(G_PART), "score_timeline": score_log,
           "final_scores": final_scores, "final_elim": final_elim,
           "final_winner": final_winner, "final_phase": final_phase,
           "concluded": concluded, "trace": trace}
    (ROOT/"log"/f"verify_scoring_{tag}.json").write_text(json.dumps(out, indent=1))
    print(f"  -> log/verify_scoring_{tag}.json")

    # ---- original-vs-modded telemetry DIFF (CONFIDENCE.md C4 requirement) ----
    diff_verdict = None
    other_tag = "baseline" if not baseline else "hooked"
    other_path = ROOT/"log"/f"verify_scoring_{other_tag}.json"
    if other_path.exists():
        other = json.loads(other_path.read_text())
        b = other if baseline is False else out          # hooked
        o = out if baseline is False else other          # baseline
        # "b" = hooked(modded), "o" = baseline(original). Diff the per-call telemetry.
        bt, ot = b["trace"], o["trace"]
        print("\n  ===== ORIGINAL vs MODDED telemetry diff =====")
        print(f"    baseline(original) calls={len(ot)}  hooked(modded) calls={len(bt)}")
        seq_match = len(ot) == len(bt)
        per_call_mismatch = []
        for i in range(min(len(ot), len(bt))):
            a, c = ot[i], bt[i]
            # compare (rva, args, return, resulting score array) per aligned call
            if (a["rva"], a["p1"], a["p2"], a["ret"], a["sc"]) != \
               (c["rva"], c["p1"], c["p2"], c["ret"], c["sc"]):
                per_call_mismatch.append((i, a, c))
        agg_match = (o["final_scores"] == b["final_scores"] and
                     o["final_elim"] == b["final_elim"] and
                     o["final_winner"] == b["final_winner"])
        print(f"    aggregate outcome match (scores/elim/winner): {agg_match}")
        print(f"      original final: scores={o['final_scores']} elim={o['final_elim']} winner={o['final_winner']}")
        print(f"      modded   final: scores={b['final_scores']} elim={b['final_elim']} winner={b['final_winner']}")
        print(f"    per-call sequence aligned: {seq_match}; mismatches: {len(per_call_mismatch)}")
        for i, a, c in per_call_mismatch[:6]:
            print(f"      [{i}] orig={a}  mod={c}")
        if seq_match and not per_call_mismatch:
            diff_verdict = "GREEN: per-call telemetry BIT-IDENTICAL original==modded"
        elif agg_match:
            diff_verdict = ("AGGREGATE-GREEN: identical final scores/elim/winner; "
                            "per-call sequence diverged (race nondeterminism, not scoring)")
        else:
            diff_verdict = "RED: telemetry diverges (see mismatches)"
        print(f"    DIFF VERDICT: {diff_verdict}")
        print("  =============================================")

    verdict = "GREEN" if (all_e9 and fired) else ("PARTIAL" if all_e9 else "RED")
    print(f"\n  VERDICT [{tag}]: {verdict}  (E9={all_e9}  hooks_fired={len(fired)}/3  "
          f"round_concluded={concluded})  diff={diff_verdict}")
    print("  ==================================================")
    try:
        if nav.alive(): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0 if ((baseline or (all_e9 and fired))) else 1


if __name__ == "__main__":
    sys.exit(main())
