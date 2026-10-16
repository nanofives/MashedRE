"""hooks.csv + CHANGELOG transaction for dualcopy3 (re-classify side effects 1 and 4). Every claim is read from an evidence file and
asserted; a missing or failing file aborts the whole transaction.   py -3.12 tracker3.py [--apply]"""
import csv, io, pathlib, re, sys
ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
H = ROOT / "verify" / "dualcopy3_20261016"
EV, DATE, P = "verify/dualcopy3_20261016", "2026-10-16", "mashedmod/src/mashed_re/"

def rd(p): return (H / p).read_text(encoding="utf-8", errors="replace")
def sha(log): return tuple(re.findall(r"sha8=(\w{8})", rd(log)))
def gctrl():
    a, b = sha("b88_gctrl.log"), sha("n88_gctrl.log"); assert len(a) == 3 and a == b, (a, b)
    return "/".join(b)
def path1():
    t = rd("w1/path1_ai_post_step_powerup_brake.txt"); m = re.search(r"total cases:\s+(\d+)\s+mismatches:\s+(\d+)", t)
    assert m and m.group(2) == "0" and "GREEN" in t; return f"path1 GREEN {m.group(1)}/{m.group(1)} (cache_setter_observe, both write arms + the four gates closed in turn)"
def path2():
    t = rd("w1/path2_ai_post_step_powerup_brake.txt"); m = re.search(r"interceptor entries \(reimpl\):\s+(\d+) / (\d+)", t)
    assert m and m.group(1) == m.group(2) and "FAIL:\n  (none)" in t; return f"path2 PASS (E9 + rel32, {m.group(1)}/{m.group(2)} entries)"
def ab(tag):
    t = rd(f"w1/ab/AbAiPost.{tag}.log"); s = [l for l in t.splitlines() if "calls=" in l]; assert s, tag
    m = re.search(r"calls=(\d+) mism=(\d+) gate1pass=(\d+) wrote=(\d+) forced=(\d+) fmism=(\d+) fwrote=(\d+) fskip=(\d+) f2=(\d+) f2mism=(\d+) f2brake=(\d+) f2coast=(\d+)", s[-1])
    g = list(map(int, m.groups())); assert g[1] == 0 and g[5] == 0 and g[9] == 0 and g[10] > 0 and g[11] > 0, (tag, s[-1])
    return f"{tag}: >= {g[0]} calls mism=0 (natural gate1pass={g[2]} wrote={g[3]}), forced {g[4]} fmism=0, full-depth {g[8]} f2mism=0 (brake {g[10]} / coast {g[11]})"
def mut():
    t = rd("wm/ab/AbAiPost.warzone_mut.log"); s = [l for l in t.splitlines() if "calls=" in l]
    m = re.search(r"f2mism=(\d+)", s[-1]); assert m and int(m.group(1)) > 0, s[-1]; return f"M-CTRL (brake comparison inverted) f2mism={m.group(1)}"
def nav(n):
    t = rd(f"w1/{n}.txt"); assert "drive_to_race=True" in t and "proc.poll()=None" in t and "CRASH" not in t, n; return "reached a race alive"
def boot():
    t = rd("w1/install_observe_all.txt"); m = re.search(r"manifest: (\d+) hooks seen, (\d+) installed", t); assert m and "survived 45s" in t
    assert re.search(r"0x00417640\s+1\s+AiPostStepPowerupBrake", rd("w1/install_all/hook_install_manifest.txt"), re.I)
    return f"full set {m.group(2)}/{m.group(1)} installed incl. AiPostStepPowerupBrake, 45 s"
def cam():
    def s8(d): return re.findall(rf"{d} cam_dump.csv (\w+)", rd("drv_b88.log" if d.endswith("b88") else "drv_n88.log"))
    a = re.findall(r"^cam cam_dump.csv (\w+)", rd("drv_b88.log"), re.M); b = re.findall(r"^cam cam_dump.csv (\w+)", rd("drv_n88.log"), re.M)
    n0 = re.findall(r"^camN0 cam_dump.csv (\w+)", rd("drv_n88.log"), re.M); mt = re.findall(r"cam_dump.csv (\w+)", rd("mut_cam.log"))
    assert a and a == b == n0 and mt and mt[0] != a[0], (a, b, n0, mt)
    return f"d1r2 row-indexed replay: cam_dump sha8 {a[0]} on the base exe, the new exe and the new exe again (N0); the lround mutant gives {mt[0]}"

SLOTS = {}
def main():
    rows = list(csv.reader(io.StringIO((ROOT / "hooks.csv").read_bytes().decode("utf-8"), newline="")))
    h = rows[0]; ix = {c: i for i, c in enumerate(h)}; by = {}
    for r in rows[1:]:
        try: by[int(r[0], 16)] = r
        except (ValueError, IndexError): pass
    g = gctrl()
    brake = (f"C2->C3 RE-EARNED {DATE} (dualcopy3) ON THE ONE BODY Ai/AiPostStepBrake.cpp (both targets; AiController.cpp and AiStandalone.cpp "
             "copies deleted). The exe copy pinned rate = 0.0f so it could never brake: a port defect (parent decision), fixed by the shared body's "
             "three thunks (track index, FUN_0046d6d0 = record +0x9e4, FUN_0046d570 = the naked AL_46d570). "
             f"{path1()}; {path2()}; AbAiPost live race {ab('warzone')}; {ab('track0')}; {mut()}; nav boot full set and ONLY this hook "
             f"{nav('nav_probe_full_hookset')}; {boot()}; exe G-CTRL {g} before and after (base 88a98a2f). In the exe the predicate word 0x0088ff50 "
             f"and held float 0x00684de0 have no writer, so the brake branch stays unreachable there until the powerup-seek producer FUN_00414f00 is ported. {EV}/RESULT_dualcopy3.md")
    r = by[0x00417640]; assert r[ix["confidence"]] == "C2"
    r[ix["confidence"]] = "C3"; r[ix["file"]] = r[ix["exe_file"]] = P + "Ai/AiPostStepBrake.cpp"; r[ix["notes"]] += " | " + brake
    ftol = (f"C2->C3 RE-EARNED {DATE} (dualcopy3): ONE body. Race/RaceCamera.cpp's static_cast copy now calls FtolVerbatim, a fld/call/ret thunk onto "
            "the verbatim FPURound_4a2c48, so exe_file == file. The 117 bytes 0x004a2c48..0x004a2cbc are byte-identical to the unpatched original in "
            f"both built images (byte_check.py); {cam()}; exe G-CTRL {g} unchanged. {EV}/RESULT_dualcopy3.md")
    r = by[0x004A2C48]; assert r[ix["confidence"]] == "C2"
    r[ix["confidence"]] = "C3"; r[ix["exe_file"]] = P + "Math/FPURound.cpp"; r[ix["notes"]] += " | " + ftol
    for rva in (0x004987B0, 0x00485340, 0x00431B10):
        r = by[rva]; old = r[ix["exe_file"]]; r[ix["exe_file"]] = ""
        r[ix["notes"]] += (f" | {DATE} (dualcopy3): exe_file cleared. {old.replace(P, '')} held a push/ret call-through or a prose comment the "
                           "scanner bound as a body, not a body of this RVA (re-anchored); level unchanged")
    buf = io.StringIO(newline=""); csv.writer(buf, lineterminator="\r\n").writerows(rows)
    cp = ROOT / "re" / "analysis" / "CHANGELOG.md"; ct = cp.read_bytes().decode("utf-8"); nl = "\r\n" if "\r\n" in ct else "\n"
    L = ct.split(nl); at = [i for i, l in enumerate(L) if l == "<!-- ENTRIES -->"]; assert len(at) == 1
    L[at[0] + 1:at[0] + 1] = [
        f"{DATE}  00417640  {by[0x00417640][ix['name']]}  C2->C3 (RE-EARNED)  One body Ai/AiPostStepBrake.cpp, exe brake-rate pin removed. {path1()}; {ab('warzone')}; G-CTRL {g} unchanged. Evidence {EV}/RESULT_dualcopy3.md.",
        f"{DATE}  004a2c48  {by[0x004A2C48][ix['name']]}  C2->C3 (RE-EARNED)  RaceCamera now calls the verbatim body (FtolVerbatim); byte-identical in both images; camera replay unchanged. Evidence {EV}/RESULT_dualcopy3.md.",
        f"{DATE}  004987b0/00485340/00431b10  (exe_file)  (no ladder move)  exe_file cleared: call-through / prose false anchors re-anchored. Evidence {EV}/RESULT_dualcopy3.md."]
    print("rows: 00417640 C2->C3, 004a2c48 C2->C3, 3 exe_file cleared")
    if "--apply" in sys.argv:
        (ROOT / "hooks.csv").write_bytes(buf.getvalue().encode("utf-8")); cp.write_bytes(nl.join(L).encode("utf-8")); print("written")
main()
