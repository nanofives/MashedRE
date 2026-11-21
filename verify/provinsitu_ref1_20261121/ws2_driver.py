# PROV-INSITU-REF1-WS2 driver: stepped runs (--prov-wlog 10, the PROV-INSITU-REF1 write-set recipe) as singles and as
# halves of the three diverging groups ws_g01 / ws_g03 / ws_g05, each scored with prov_compare against ref1 / ref2.
# Resumable: one line per finished run in D:/MashedWT/PROV-INSITU-REF1-WS2_data/ws2_state.jsonl.
#   test-slot py -3.12 ws2_driver.py singles      # 3 covered+image + 28 not-run, one body per run
#   test-slot py -3.12 ws2_driver.py split        # halves of ws_g01/03/05 until each diverging set is one body
#   test-slot py -3.12 ws2_driver.py run <label> <rva,rva,...> [bodies.json]
import csv, json, subprocess, sys, time
from pathlib import Path

H = Path(__file__).resolve().parent
WT = H.parents[1]
sys.path.insert(0, str(WT / "re" / "frida"))
from prov_compare import verdict  # noqa: E402

D = Path("D:/MashedWT/PROV-INSITU-REF1-WS2_data")
SRC = Path("D:/MashedWT/PROV-INSITU-REF1_data")
R = "D:/MashedWT/RO-REF-NAME_data"
B309 = str(SRC / "bodies309.json")
BNW = str(SRC / "bodies_nw.json")
STATE = D / "ws2_state.jsonl"
D.mkdir(parents=True, exist_ok=True)


def done():
    out = {}
    if STATE.exists():
        for ln in STATE.read_text().splitlines():
            if ln.strip():
                x = json.loads(ln); out[x["label"]] = x
    return out


def run(label, rvas, bodies=B309, n=10, extra=()):
    st = done()
    if label in st:
        return st[label]
    print(f"[run] {label} {len(rvas)} bodies {','.join(rvas)[:120]}", flush=True)
    rc = subprocess.call(["py", "-3.12", str(H / "ws2_arm.py"), label, "--prov", bodies, "--prov-rvas", ",".join(rvas),
                          "--prov-wlog", str(n), *extra], cwd=WT)
    d = D / label
    try:
        v = verdict(d, f"{R}/ref1", f"{R}/ref2")
        first = v.get("first") or {}
        rec = {"label": label, "rvas": rvas, "bodies": bodies, "rc": rc, "verdict": v["verdict"],
               "identical_for": v["identical_for"], "first_tick": first.get("tick"),
               "blocks": [b[0] for b in first.get("blocks", [])], "dn_warp": v.get("dn_warp"),
               "segments_match": v.get("segments_match")}
    except Exception as e:  # no ticks: crash or harness failure
        rec = {"label": label, "rvas": rvas, "bodies": bodies, "rc": rc, "verdict": "NO-TICKS", "error": str(e)[:300]}
    try:
        m = json.loads((d / "meta.json").read_text())
        s = m["wlog"]["stats"]
        rec["wlog"] = {"records": s[2], "overflow": s[6], "frames": s[8], "other_thread_steps": s[15]}
    except Exception:
        pass
    with open(STATE, "a") as f:
        f.write(json.dumps(rec) + "\n")
    print(f"   -> {rec['verdict']} {rec.get('identical_for', '')} first {rec.get('first_tick')} {rec.get('blocks')} wlog {rec.get('wlog')}", flush=True)
    return rec


def cut_rows():
    return list(csv.DictReader(open(H / "WS_CUT.tsv"), delimiter="\t"))


def singles():
    rows = [r for r in cut_rows() if r["leg"] in ("covered+image, run diverged", "not-run")]
    print(f"{len(rows)} singles", flush=True)
    for i, r in enumerate(rows):
        rv = r["rva"]
        bodies = BNW if rv == "0x005185a0" else B309
        print(f"[{i + 1}/{len(rows)}] single {rv}", flush=True)
        run(f"s_{rv[2:]}", [rv], bodies)


def split_group(g, members):
    # first repeat the whole group once (is the divergence repeatable?), then halve every diverging set
    top = run(f"{g}_rep", members)
    queue = [(f"{g}", members)] if top["verdict"] != "IDENTICAL-FOR-N" else []
    if not queue:
        print(f"{g}: repeat IDENTICAL, split not needed for a member; halves still run once", flush=True)
        queue = [(f"{g}", members)]
        once = True
    else:
        once = False
    while queue:
        name, ms = queue.pop(0)
        if len(ms) == 1:
            continue
        h = len(ms) // 2
        for tag, part in (("a", ms[:h]), ("b", ms[h:])):
            lab = f"{name}{tag}"
            r = run(lab, part)
            if r["verdict"] != "IDENTICAL-FOR-N" and len(part) > 1 and not once:
                queue.append((lab, part))


def split():
    rows = cut_rows()
    for g in ("ws_g01", "ws_g03", "ws_g05"):
        ms = [r["rva"] for r in rows if r["ws_run"] == g]
        print(f"== {g}: {len(ms)} members", flush=True)
        split_group(g, ms)


PRE_ANCHOR_ONLY = {"0x004ddcb0", "0x005abd30", "0x005ab410", "0x004e51e0", "0x005b2080", "0x005b04b0", "0x00405540"}


def stride():
    # PROV-WRITESET-SCENARIO method on the ref1 recipe: frame-only bodies stepped every K-th call from the race anchor
    # (ws2_wstride.json, ws2_STRIDE_PLAN.tsv), up to 100000 stepped calls per body, groups of 7 or fewer
    rows = [r for r in cut_rows() if r["leg"].startswith("covered, frame only") and r["rva"] not in PRE_ANCHOR_ONLY]
    rvas = [r["rva"] for r in rows]
    groups = [rvas[i:i + 7] for i in range(0, len(rvas), 7)]
    for i, g in enumerate(groups):
        print(f"[{i + 1}/{len(groups)}] stride group", flush=True)
        run(f"st_g{i:02d}", g, n=100000, extra=["--prov-wstride", str(H / "ws2_wstride.json"), "--prov-warm", "anchor"])


if __name__ == "__main__":
    if sys.argv[1] == "stride":
        stride()
    t0 = time.time()
    if sys.argv[1] == "singles":
        singles()
    elif sys.argv[1] == "split":
        split()
    elif sys.argv[1] == "run":
        run(sys.argv[2], sys.argv[3].split(","), sys.argv[4] if len(sys.argv) > 4 else B309)
    elif sys.argv[1] == "nowlog":
        # the same body set, no single-step (--prov-wlog 0): separates a stepping effect from a body interaction
        rows = cut_rows()
        if len(sys.argv) == 2:
            for g in ("ws_g01", "ws_g03", "ws_g05"):
                run(f"nw_{g}", [r["rva"] for r in rows if r["ws_run"] == g], n=0)
        for lab, rv in (x.split("=") for x in sys.argv[2:]):
            run(lab, rv.split(","), n=0)
    print(f"[driver] done {sys.argv[1]} in {time.time() - t0:.0f} s", flush=True)
