"""Self-test for re/tools/libid_reclass.py (runs against a fake repo root, never the real trackers)."""
import importlib.util, pathlib, pytest

TOOL = pathlib.Path(__file__).resolve().parents[1] / "libid_reclass.py"

HOOKS = (
    "# comment line,,,\r\n"
    "rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes,exe_file\r\n"
    "004a2b60,sprintf,third-party-library[msvc-crt-main],C2,mapped,,,,old note,\r\n"
    "004a3090,FUN_004a3090,third-party-library[msvc-crt-main],C2,mapped,,,,,\r\n"
    "004a4000,FUN_004a4000,third-party-library[msvc-crt-main],C2,mapped,,,,\"a, b\",\r\n"
    "004a31b1,FUN_004a31b1,boot,C3,impl,x.cpp,,,boot note,x.cpp\r\n"
    "004fcbca,FUN_004fcbca,third-party-library[d3dx9-shader-compiler],C2,mapped,,,,,\r\n"
    "004b7020,FUN_004b7020,render,C3,impl,n.md,,,render note,\r\n"
    "00400000,FUN_00400000,boot,C2,mapped,,,,,\r\n"
)
H = "library\trva\thooks_name\tsubsystem\tconfidence\tstatus\tlibrary_object\tlib_function\tlib_size\tfirst_diff\tcategory\tnote\n"
TSV = H + "".join("\t".join(r) + "\n" for r in [
    ["msvc-crt(ddk-libcmt)", "0x004a2b60", "sprintf", "x", "C2", "identical", "libcmt.lib:obj\\i386\\sprintf.obj", "_sprintf", "88", "", "", "masked 8 B"],
    ["msvc-crt(ddk-libcmt)", "0x004a3090", "F", "x", "C2", "identical-ambiguous", "libcmt.lib:obj\\i386\\ceil.obj", "_ceil", "64", "", "", "masked 12 B; same bytes as _floor,_modf"],
    ["msvc-crt(ddk-libcmt)", "0x004a4000", "F", "x", "C2", "differs", "libcmt.lib:obj\\i386\\a.obj", "_a", "9", "0x5", "register allocation", ""],
    ["msvc-crt(ddk-libcmt)", "0x004a31b1", "(not in hooks.csv)", "", "", "identical-unlisted", "libcmt.lib:obj\\i386\\b.obj", "_b", "20", "", "", ""],
    ["msvc-crt(ddk-libcmt)", "0x004fcbca", "(not in hooks.csv)", "", "", "identical-unlisted", "libcmt.lib:obj\\i386\\c.obj", "_c1", "20", "", "", ""],
    ["msvc-crt(ddk-libcmt)", "0x004fcbca", "(not in hooks.csv)", "", "", "identical-unlisted", "libcmt.lib:obj\\i386\\c.obj", "_c2", "20", "", "", ""],
    ["msvc-crt(ddk-libcmt)", "0x004a2fc0", "(not in hooks.csv)", "", "", "identical-unlisted", "libcmt.lib:obj\\i386\\strchr.obj", "_$$$00001", "16", "", "", ""],
    ["msvc-crt(ddk-libcmt)", "0x005c4080", "(not in hooks.csv)", "", "", "identical-unlisted", "libcmt.lib:obj\\i386\\d.obj", "__d", "16", "", "", ""],
    ["lua-4.0", "0x004b7020", "(not in hooks.csv)", "", "", "identical-unlisted", "lapi.obj", "_lua_typename", "30", "", "", ""],
    ["lua-4.0", "0x004b7450", "(not in hooks.csv)", "", "", "identical-unlisted", "lstate.obj", "_errormessage", "30", "", "", ""],
    ["lua-4.0", "0x004c00b0", "(not in hooks.csv)", "", "", "identical-unlisted", "lmathlib.obj", "_math_mod", "30", "", "", ""],
    ["lua-4.0", "0x004c00b0", "(not in hooks.csv)", "", "", "identical-unlisted", "lmathlib.obj", "_math_pow", "30", "", "", ""],
    ["qhull-2002.1", "0x00591800", "(not in hooks.csv)", "", "", "identical-unlisted", "merge.obj", "_qh_x", "30", "", "", ""],
])
CL = "# Changelog\n\n<!-- ENTRIES -->\n2026-01-01  old entry\n"


@pytest.fixture
def mod(tmp_path, monkeypatch):
    spec = importlib.util.spec_from_file_location("libid_reclass", TOOL)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    (tmp_path / "re" / "analysis").mkdir(parents=True)
    (tmp_path / "ev.md").write_text("evidence", encoding="utf-8")
    (tmp_path / "t.tsv").write_text(TSV, encoding="utf-8")
    (tmp_path / "libcmt.lib").write_bytes(b"x")
    (tmp_path / "hooks.csv").write_bytes(HOOKS.encode())
    (tmp_path / "re" / "analysis" / "CHANGELOG.md").write_bytes(CL.encode())
    monkeypatch.setattr(m, "ROOT", tmp_path)
    monkeypatch.setattr(m, "HOOKS", tmp_path / "hooks.csv")
    monkeypatch.setattr(m, "CHANGELOG", tmp_path / "re" / "analysis" / "CHANGELOG.md")
    monkeypatch.setattr(m, "comp_ids", lambda lib: {"obj\\i386\\sprintf.obj": "28/9178"})
    return m


def run(m, monkeypatch, tmp_path, *argv):
    monkeypatch.setattr("sys.argv", ["libid_reclass.py", *argv, "--tsv", "t.tsv", "--evidence", "ev.md",
                                     "--date", "2026-10-31", "--lib", str(tmp_path / "libcmt.lib")])
    return m.main()


def rows(tmp_path):
    import csv
    return {r[0]: r for r in csv.reader((tmp_path / "hooks.csv").read_text(encoding="utf-8").splitlines()) if r}


def test_crt_waive(mod, monkeypatch, tmp_path):
    log = run(mod, monkeypatch, tmp_path, "crt-waive")
    assert len(log) == 5
    r = rows(tmp_path)
    assert r["004a2b60"][4] == "waived-platform-crt" and r["004a2b60"][3] == "C2"
    assert r["004a2b60"][8].startswith("identified, waived: platform CRT (L-67); libid identical _sprintf")
    assert "@comp.id 28/9178" in r["004a2b60"][8] and r["004a2b60"][8].endswith(" | old note")
    assert "candidates _ceil / _floor / _modf" in r["004a3090"][8]
    assert r["004a4000"][8].startswith("waived: platform CRT (L-67); libid differs") and r["004a4000"][8].endswith("| a, b")
    assert r["004a31b1"][2] == "third-party-library[msvc-crt-main]" and r["004a31b1"][3] == "C3"
    assert r["004fcbca"][2] == "third-party-library[msvc-crt]" and "_c1 / _c2" in r["004fcbca"][8]
    assert r["00400000"][4] == "mapped" and r["004b7020"][2] == "render"
    raw = (tmp_path / "hooks.csv").read_bytes()
    assert raw.startswith(b"# comment line,,,\r\n") and b"\n" not in raw.replace(b"\r\n", b"")
    cl = (tmp_path / "re" / "analysis" / "CHANGELOG.md").read_text(encoding="utf-8")
    assert "2026-10-31  004a31b1  FUN_004a31b1  C3->C3  waived-platform-crt (L-67), refiled boot->" in cl
    assert cl.index("004a2b60") < cl.index("old entry")
    # second run: nothing to do, nothing written
    before = (tmp_path / "hooks.csv").read_bytes()
    assert run(mod, monkeypatch, tmp_path, "crt-waive") == []
    assert (tmp_path / "hooks.csv").read_bytes() == before


def test_add_rows_lua_needs_not_c4(mod, monkeypatch, tmp_path):
    with pytest.raises(SystemExit):
        run(mod, monkeypatch, tmp_path, "add-rows", "--library", "lua-4.0")


def test_add_rows_lua_and_crt(mod, monkeypatch, tmp_path):
    log = run(mod, monkeypatch, tmp_path, "add-rows", "--library", "lua-4.0", "--not-c4", "no vendored Lua")
    assert len(log) == 2           # 0x004b7020 has a row; 0x004b7450 and 0x004c00b0 are added
    r = rows(tmp_path)
    assert r["004b7450"][1] == "errormessage" and r["004b7450"][3] == "C1" and r["004b7450"][4] == "mapped"
    assert r["004c00b0"][1] == "FUN_004c00b0" and "_math_mod / _math_pow" in r["004c00b0"][8]
    assert len(r["004b7450"]) == 10 and r["004b7450"][8].endswith("not C4: no vendored Lua")
    log = run(mod, monkeypatch, tmp_path, "add-rows", "--library", "msvc-crt")
    r = rows(tmp_path)
    assert r["004a2fc0"][1] == "FUN_004a2fc0" and r["004a2fc0"][4] == "waived-platform-crt"
    assert r["005c4080"][1] == "_d" and r["005c4080"][2] == "third-party-library[msvc-crt-tail]"
    assert run(mod, monkeypatch, tmp_path, "add-rows", "--library", "msvc-crt") == []


def test_add_rows_qhull_continuation(mod, monkeypatch, tmp_path):
    run(mod, monkeypatch, tmp_path, "add-rows", "--library", "qhull-2002.1", "--not-c4", "x")
    assert rows(tmp_path)["00591800"][2] == "third-party-library[qhull-2002.1-continuation]"


def test_refile_lib(mod, monkeypatch, tmp_path):
    log = run(mod, monkeypatch, tmp_path, "refile-lib", "--library", "lua-4.0")
    assert len(log) == 1
    r = rows(tmp_path)["004b7020"]
    assert r[2] == "third-party-library[lua-4.0]" and r[3] == "C3" and r[4] == "impl"
    assert r[8].startswith("libid identical lapi.obj _lua_typename") and r[8].endswith("| render note")
    assert run(mod, monkeypatch, tmp_path, "refile-lib", "--library", "lua-4.0") == []


def test_missing_evidence_refused(mod, monkeypatch, tmp_path):
    (tmp_path / "ev.md").unlink()
    with pytest.raises(SystemExit):
        run(mod, monkeypatch, tmp_path, "crt-waive")
