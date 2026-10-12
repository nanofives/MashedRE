# Compare c4_hud_scenario.py runs for one row against setup-matched original references.
# Usage: py -3.12 re/frida/c4_hud_compare.py --dir <run dir> --row 403160|41db80|4295a0|40d440|4464c0 [--csv OUT]
#
# 403160: per-call key = ret | ordered callee events | post cam+0x68/+0x6c (PRE = equal to the
#         pre-call value). A run matches when its set of keys (the one 'first:' call excluded,
#         because the pre-call value is read from the previous call's camera) equals the
#         reference's, and no call carries a key the reference never produced.
# 4295a0: per-call key = the FUN_00427e00 calls made inside the body (6 args each, the y arg
#         checked against [0x0063b910] (+3.0f on the first)) | return == last call's EAX.
# 41db80: per-call key = counter bucket | dispatched slot list. A run matches when every bucket
#         maps to the same slot list as in the reference, and mmOrig (calls whose dispatched list
#         differs from the byte model 0x0041dab0..0x0041db79) is 0.
# 40d440: per-call key = selected/loaded at entry | caller | FUN_0040d270 calls made inside the body
#         (OBJ = [0x005f2770], SEL = [0x0063ba7c]) | [0x0063ba78] after. Same key-set rule as 403160.
# 4464c0: polled key (no per-call hook, see the scenario header) = entry count + per entry type and
#         the class of its three RwMatrix pad dwords. A run matches when its dominant key equals the
#         reference's and every key holding >= 2% of its polls also occurs in the reference.
import json, sys, csv
from pathlib import Path

av = sys.argv
def opt(f, d): return av[av.index(f) + 1] if f in av else d
D = Path(opt("--dir", "log/c4_hud/final")); row = opt("--row", "403160")
EXPORT_OFF = {"403160": "0x141a0", "41db80": "0x142a0", "4295a0": "0x18800",
              "40d440": "0x80760", "4464c0": "0x3d050"}[row]   # the last two: c4race_fix/ctl.asi exports
REF = {"off": "off1", "on": "off1", "stock": "stock1", "late": "stock1", "ctl": "stock1"}

def load(tag):
    p = D / f"c4_hud_{row}_{tag}.json"
    return json.loads(p.read_text()) if p.exists() else None

def keyset(r):
    ks = r.get("dump", {}).get("keys", {})
    if row == "403160":
        return {k: v["n"] for k, v in ks.items() if "post=first:" not in k}
    return {k: v["n"] for k, v in ks.items()}

def bmap(r):
    m = {}
    for k in r.get("dump", {}).get("keys", {}):
        b, act = k.split("|", 1); m.setdefault(b, set()).add(act)
    return m

rows = []
tags = sorted(p.stem.split(f"c4_hud_{row}_", 1)[1] for p in D.glob(f"c4_hud_{row}_*.json"))
for t in tags:
    r = load(t); arm = r["arm"]; ref = load(REF[arm] if t != REF[arm] else ("off2" if arm == "off" else "stock2"))
    w = r.get("pre_witness", {})
    mname = str((w.get("mod") or {}).get("name", ""))
    live = (w.get("b0") == 0xE9 and (mname.startswith("mashed_re_dev") or mname.startswith("c4race_"))
            and (w.get("mod") or {}).get("off") == EXPORT_OFF)
    d = r.get("dump", {})
    res = {"tag": t, "arm": arm, "verdict": r.get("verdict"), "ref": None, "calls": d.get("calls"),
           "b0": w.get("b0"), "hook_live": live, "jmp_module": (w.get("mod") or {}).get("name", ""), "entry": d.get("entry"), "crash": len(r.get("crash", [])),
           "crash_at": ((r.get("crash") or [{}])[0].get("module") or "") + (" " + str((r.get("crash") or [{}])[0].get("addr")) if r.get("crash") else ""),
           "crash_enter": (r.get("crash") or [{}])[0].get("enter") if r.get("crash") else "",
           "crash_inbody": (r.get("crash") or [{}])[0].get("inBody") if r.get("crash") else "",
           "enter": d.get("enter"), "exit_code": hex(r["exit_code"]) if isinstance(r.get("exit_code"), int) else r.get("exit_code"),
           "alive_at_end": r.get("alive_at_end"), "mode": (r.get("first_call_state") or {}).get("mode"),
           "rule": (r.get("first_call_state") or {}).get("rule")}
    if ref is not None:
        res["ref"] = ref["tag"]
        if row == "4464c0":
            a, b = keyset(r), keyset(ref); na = sum(a.values()) or 1
            da = max(a, key=a.get) if a else None; db = max(b, key=b.get) if b else None
            major = {k: n for k, n in a.items() if n / na >= 0.02}
            res["keys"] = len(a); res["ref_keys"] = len(b); res["polls"] = sum(a.values())
            res["dominant_share"] = round(a[da] / na, 4) if da else None
            res["dominant_eq_ref"] = (da == db); res["major_not_in_ref"] = ";".join(k for k in major if k not in b)
            res["dominant"] = da
            res["match"] = bool(a) and da == db and not res["major_not_in_ref"]
        elif row in ("403160", "4295a0", "40d440"):
            a, b = keyset(r), keyset(ref)
            res["keys"] = len(a); res["ref_keys"] = len(b)
            res["foreign_calls"] = sum(n for k, n in a.items() if k not in b)
            res["missing_keys"] = sum(1 for k in b if k not in a)
            res["match"] = bool(a) and res["foreign_calls"] == 0 and res["missing_keys"] == 0
        else:
            a, b = bmap(r), bmap(ref)
            diff = sorted(k for k in set(a) | set(b) if k in a and k in b and a[k] != b[k])
            res["buckets"] = len(a); res["ref_buckets"] = len(b); res["bucket_diffs"] = ";".join(diff)
            res["mmOrig"] = d.get("mmOrig"); res["mmOld"] = d.get("mmOld")
            res["match"] = bool(a) and not diff and d.get("mmOrig") == 0 and set(a) == set(b)
        res["result"] = "GREEN" if (res["match"] and not res["crash"] and r.get("verdict") == "DONE") else "RED"
        if row in ("40d440", "4464c0"): res["seed_calls"] = d.get("seedCalls"); res["tx"] = d.get("tx"); res["outside"] = d.get("outside")
    rows.append(res)

out = opt("--csv", str(D / f"c4_hud_compare_{row}.csv"))
fields = sorted({k for r in rows for k in r}, key=lambda k: (k not in ("tag", "arm", "result", "ref"), k))
with open(out, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=fields); w.writeheader(); [w.writerow(r) for r in rows]
for r in rows: print({k: r.get(k) for k in fields})
print("->", out)
