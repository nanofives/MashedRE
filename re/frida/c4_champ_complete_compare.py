# Compare c4_champ_complete_scenario.py runs against a reference run (normally the first
# original-side 'off' run). Channels, per body call:
#   grid   : every dword of 0x007f0a40..0x007f0cc0 that the call changed (entry -> return)
#   f2c    : 0x007f0f2c at return (written only on the mode-5 all-done path, 0x00430584)
#   ev/pri : PostTrophyEvent cells 0x00899140 / 0x00898ab0 at return (last post wins)
#   posts  : PostTrophyEvent ids posted while the body was on the stack (orig 0x0042a920 or
#            the .asi export; supplementary: an inlined port call would not be seen)
#   saves  : AutosaveTrigger 0x004099a0 entries while the body was on the stack
# Consumer: SHA-256 of the copy's gamesave.bin after the run (the AutosaveTrigger tail writes it).
#
# Usage: py -3.12 re/frida/c4_champ_complete_compare.py REF.json RUN.json [RUN.json ...]
import json, sys


def calls(rec):
    out = []
    for c in rec.get("dump", {}).get("calls", []):
        pre, post = c["pre"], c["post"]
        g = [(hex(0x7f0a40 + 4 * i), a, b) for i, (a, b) in enumerate(zip(pre["g"], post["g"])) if a != b]
        out.append({"ra": c["ra"], "forced": c["forced"], "mode": pre["mode"], "idx": pre["idx"],
                    "concl": pre["concl"], "won_pre": pre["won"], "grid": g, "f2c": post["f2c"],
                    "ev": post["ev"], "pri": post["pri"], "posts": [p["id"] for p in c["posts"]],
                    "saves": len(c["saves"])})
    return out


def main():
    ref = json.load(open(sys.argv[1], encoding="utf-8"))
    rc = calls(ref)
    print(f"REF {ref['tag']} arm={ref['arm']} calls={len(rc)} save_after={str(ref.get('save_sha256_after'))[:16]}")
    for c in rc: print("   ", c)
    for p in sys.argv[2:]:
        r = json.load(open(p, encoding="utf-8"))
        cc = calls(r)
        diffs = []
        if len(cc) != len(rc): diffs.append(f"call count {len(cc)} vs {len(rc)}")
        for i, (a, b) in enumerate(zip(cc, rc)):
            for k in ("ra", "forced", "mode", "idx", "concl", "won_pre", "grid", "f2c", "ev", "pri", "saves"):
                if a[k] != b[k]: diffs.append(f"call{i}.{k}: {a[k]} vs {b[k]}")
            if a["posts"] and b["posts"] and a["posts"] != b["posts"]:
                diffs.append(f"call{i}.posts: {a['posts']} vs {b['posts']}")
        sv = "same" if r.get("save_sha256_after") == ref.get("save_sha256_after") else "DIFF"
        crash = len(r.get("crash", []))
        print(f"{r['tag']:>14} arm={r['arm']:<5} verdict={r['verdict']} calls={len(cc)} crash={crash} "
              f"alive_at_end={r.get('alive_at_end')} save_after={sv} -> {'GREEN' if not diffs else 'RED'}")
        for c in cc: print("      posts", c["posts"], "saves", c["saves"])
        for d in diffs: print("      ", d)


if __name__ == "__main__":
    main()
