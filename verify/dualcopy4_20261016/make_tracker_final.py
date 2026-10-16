"""Derive tracker_final.py from tracker4.py: evidence paths wb* -> wd*, the final asi hash, the extra rows (0x0042d3e0 corrected, 0x004c3ac0 / 0x004c39b0 / 0x004c4d20 camera),
a `tag` override for the changelog label. Run once; the output is committed."""
import pathlib, re

H = pathlib.Path(__file__).resolve().parent
t = (H / "tracker4.py").read_text(encoding="utf-8")
for a, b in [("wb1/", "wd1/"), ("'wb2'", "'wd2'"), ("'wb3'", "'wd2'"), ("wb3/", "wd2/"), ("nb_gctrl.log", "nd_gctrl.log"),
             ("host_tests_new.txt", "host_tests_final.txt"), ("m3r_runs.txt", "m3r_final.txt"), ("final asi f4449212", "final asi d7059579"),
             ('"""hooks.csv + CHANGELOG transaction for dualcopy4', '"""FINAL hooks.csv + CHANGELOG transaction for dualcopy4 (evidence on the final image d)')]:
    assert a in t, a
    t = t.replace(a, b)

# upd(): optional tag
t = t.replace("    def upd(rva, level, name, file, exe, fd, text, cl_line):", "    def upd(rva, level, name, file, exe, fd, text, cl_line, tag=None):")
old = "        log.append(f\"{DATE}  {rva:08x}  {row[ix['name']]}  {level[0]}->{level[1]}{' (RE-EARNED)' if level[0] == level[1] or level[1] > level[0] else ' (DEMOTED)'}  {cl_line} Evidence {EV}/RESULT_dualcopy4.md.\")"
assert old in t
new = ("        lab = tag or (' (RE-EARNED)' if level[0] == level[1] or level[1] > level[0] else ' (DEMOTED)')\n"
       "        log.append(f\"{DATE}  {rva:08x}  {row[ix['name']]}  {level[0]}->{level[1]}{lab}  {cl_line} Evidence {EV}/RESULT_dualcopy4.md.\")")
t = t.replace(old, new)

# evidence readers for the new rows
readers = '''

def mask():
    t = rd("maskcheck_42d3e0.txt")
    assert "0 differing bytes of 1680  -> PASS" in t
    m = re.search(r"cleared but the original keeps \\(entry-relative byte offsets\\):([-\\d ]+)\\n\\s+kept but the original clears:([\\d ]+)", t)
    assert m and m.group(1).split() == "-4 -3 -2 -1 32 33 34 35 48 49 50 51".split() and m.group(2).split() == "13 14 15 28 29 30 31 44 45 46 47".split(), t
    return ("G-MASK host test: the corrected body clears exactly the 14 stores of the original on 30 entries; the old body cleared entry-4 (and so +48 of the previous entry) and +32, "
            "and kept +13..15, +28, +44")


def oldbody():
    t = rd("wm/old_MenuEntryArrayInit_on_base_asi.txt")
    assert "total cases: 2" in t and "mismatches:  2" in t and "RED" in t
    return "the old asi body (base.asi) is RED 2/2 on the same key"


def cam():
    s = rd("pose_scores.txt")
    blocks = dict((m.group(1), m.group(2)) for m in re.finditer(r"=== (\\w+) \\((\\w{8})\\)", s))
    assert blocks["cam_cb"] == blocks["cam1"] == "1e7d2274" and blocks["cam_cd"] == blocks["cam2"] == "87078a7a", blocks
    import hashlib
    h = lambda p: hashlib.sha256((H / p).read_bytes()).hexdigest()[:8]
    assert h("cam_cd/cam_dump.csv") == h("camN0_cd/cam_dump.csv") == "87078a7a" and h("cam1/cam_dump.csv") == h("cam1n0/cam_dump.csv") == "1e7d2274"
    def pos(name):
        i = s.index(f"=== {name} ("); seg = s[i:i + 700]
        m = re.search(r"pos: n=175 median=(\\S+) p95=(\\S+) max=(\\S+)", seg); return m.groups()
    a, b = pos("cam_cb"), pos("cam_cd")
    assert float(b[0]) < float(a[0]) and float(b[1]) < float(a[1]) and float(b[2]) < float(a[2])
    return (f"d1r2 row-indexed replay: Vec3Mag alone is byte-identical (cam_dump sha8 1e7d2274 = base); with LutNormalize cam_dump 87078a7a, N0 identical, "
            f"G-POSE pos median {a[0]} -> {b[0]}, p95 {a[1]} -> {b[1]}, max {a[2]} -> {b[2]}")
'''
t = t.replace("\n\ndef main():", readers + "\n\ndef main():", 1)

rows = '''
    upd(0x0042D3E0, ("C3", "C3"), None, None, None, f"{EV}/wd1/diff_menu_entry_array_init.csv",
        f"{DATE} (dualcopy4) CORRECTED and RE-EARNED: the C3 body did not match the bytes (disasm_42d3e0.txt: 14 stores per entry, dwords entry +0 +4 +8, bytes +12..+15, dwords +16 +20 +24 +28 +36 +40 +44, "
        "entry +32 not written). It wrote the dword before the entry, bytes +9..+12, +32 and missed +13..15, +28, +44; the key observed one dword of entry 0, which both clear. "
        f"Now Frontend/MenuEntryZero.h:MenuEntryZeroBody (header-inline) from the disassembly; MenuInit.cpp:MenuEntryArrayInit (installed) and, for the exe, MenuNavSM.cpp:RecordsZero both run it. "
        f"{mask()}; the registry key is rewritten as cache_setter_observe over entries 0 1 2 28 29 30 + the dword before (79 dwords x 2 fills): {p1('menu_entry_array_init')}; {oldbody()}; "
        f"{p2('menu_entry_array_init', 'wd2')}; {host()}. exe_file stays split on purpose: RecordsZero still clears MORE than the original (pad20 +0x20, which MenuNavSM.cpp writes and reads as row20, pad30 +0x30, "
        f"and entry 30) so the exe reset is unchanged in effect; making it faithful needs a frontend parity capture: open. {common}",
        "Asi body corrected from the bytes (was wrong); one body MenuEntryZero.h; RecordsZero shares it; old body RED on the new key.", tag=" (CORRECTED, RE-EARNED)")
    upd(0x004C3AC0, ("C4", "C4"), None, None, P + "Math/Vec3.cpp", None,
        f"{DATE} (dualcopy4) exe_file == file: Race/RaceCamera.cpp:Vec3Mag (the exe's second body, std::sqrt(x*x+y*y+z*z)) is now a call into Math/Vec3.cpp:Vec3Magnitude (exact-sqrt fallback in the exe, "
        f"the same arithmetic). Witness: {cam()}. C4 evidence is on the .asi body, which is unchanged. {common}",
        "RaceCamera Vec3Mag calls the shared Vec3Magnitude; replay byte-identical.", tag=" (no ladder move)")
    upd(0x004C39B0, ("C2", "C2"), None, None, P + "Math/RwLutTables.cpp|" + P + "Math/RwV3dNormalize.cpp", None,
        f"{DATE} (dualcopy4) Race/RaceCamera.cpp:Vec3Norm (division by an exact sqrt) is now a call into Math::LutNormalize (the real-LUT body of this RVA). {cam()}. The global RwV3dNormalize keeps its exact-sqrt "
        f"fallback in the exe (feeds D2 physics; flipping it is a user decision) and calls the original's error stubs by address on a zero vector, so it is not usable in the exe camera. Level unchanged (C2). {common}",
        "RaceCamera Vec3Norm calls Math::LutNormalize; pose scores improve; global path untouched.", tag=" (no ladder move)")
    upd(0x004C4D20, ("C2", "C2"), None, None, None, None,
        f"{DATE} (dualcopy4) NOT consolidated: Race/RaceCamera.cpp:RotateAboutAxis stays a separate exe body (std::cos/sin Rodrigues matrix + vector apply). The shared RwMatrixRotate is C2, needs a RwMatrix "
        "and the x87 FSIN/FCOS plus a vector transform, and the camera it would replace is 1e-4-accurate against the original (d1r2 G-POSE), so the replay cannot validate the swap. Open. "
        f"Evidence {EV}/RESULT_dualcopy4.md", "RotateAboutAxis left as a separate exe body (documented skip).", tag=" (no ladder move)")
'''
marker = "    out = io.StringIO(newline=\"\")"
assert marker in t
t = t.replace(marker, rows + "\n" + marker, 1)
(H / "tracker_final.py").write_text(t, encoding="utf-8")
print("tracker_final.py written")
