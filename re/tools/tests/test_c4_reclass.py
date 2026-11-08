"""Self-test for re/tools/c4_reclass.py (L-93 batch C4). Fake repo root; the real trackers are never touched and the
compiler is never run (evaluate() is replaced by canned match/relocheck results)."""
import importlib.util, pathlib, pytest

TOOL = pathlib.Path(__file__).resolve().parents[1] / "c4_reclass.py"

HOOKS = (
    "# comment line,,,\r\n"
    "rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes,exe_file\r\n"
    "00426070,TrackPhysWorld2Get,render,C3,impl,a.cpp,,,\"over, quoted\",\r\n"
    "00426080,FUN_00426080,frontend,C2,mapped,,,,,\r\n"
    "00426090,FUN_00426090,frontend,C4,impl,b.cpp,,,old,\r\n"
    "004260a0,FUN_004260a0,frontend,C2,mapped,,,,,\r\n"
    "004260b0,FUN_004260b0,frontend,C2,mapped,,,,,\r\n"
    "004b6fc0,lua_gettop,third-party-library[lua-4.0],C2,mapped,,,,,\r\n"
    "004a3090,FUN_004a3090,third-party-library[msvc-crt-main],C2,waived-platform-crt,,,,\"identified, waived: platform CRT (L-67)\",\r\n"
)
CHANGELOG = "# log\r\n\r\n<!-- ENTRIES -->\r\n2026-11-01  old entry\r\n"
LIBTSV = ("library\trva\thooks_name\tsubsystem\tconfidence\tstatus\tlibrary_object\tlib_function\tlib_size\tfirst_diff\tcategory\tnote\n"
          "lua-4.0\t0x004b6fc0\tlua_gettop\tx\tC2\tidentical\tlapi.obj\t_lua_gettop\t15\t\t\t\n"
          "msvc-crt(ddk-libcmt)\t0x004a3090\tF\tx\tC2\tidentical-ambiguous\tlibcmt.lib:ceil.obj\t_ceil\t64\t\t\t\n")
SRC_C = "// FLAGS: /O2 /GS /Ob1\n// MATCH: 0x{a} _FUN_{a}\nint FUN_{a}(int x) {{ return x + 1; }}\n"
SRC_NAKED = "// MATCH: 0x{a} _FUN_{a}\n__declspec(naked) int FUN_{a}(void) {{ __asm {{ mov eax, 1 }} __asm ret }}\n"
SRC_G26 = "// MATCH: 0x{a} _FUN_{a}\nint FUN_{a}(int a) {{ int r; __asm {{ mov eax, a }} return r; }}\n"
SRC_MIXED = "// MATCH: 0x{a} _FUN_{a}\nint FUN_{a}(int a) {{ int r = a * 3; __asm {{ fldz }} g_x = r; return r; }}\n"


def ok_eval(addr, acc=1.0, bad=0, reloc="OK"):
    return {"_toolchain": "ddk", "_flags": "/O2 /GS /Ob1", "_lang": "c",
            addr: {"acc": acc, "n_mine": 3, "n_orig": 3, "bytes_n": 12, "bytes_bad": bad, "flags": "/O2 /GS /Ob1",
                   "toolchain": "ddk", "reloc": (reloc, [] if reloc == "OK" else ["+0x4 g: ours 1 vs orig 2"], ["__ftol"], 2)}}


@pytest.fixture
def mod(tmp_path, monkeypatch):
    spec = importlib.util.spec_from_file_location("c4_reclass", TOOL)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    (tmp_path / "re" / "analysis").mkdir(parents=True)
    (tmp_path / "src").mkdir()
    (tmp_path / "hooks.csv").write_bytes(HOOKS.encode())
    (tmp_path / "re" / "analysis" / "CHANGELOG.md").write_bytes(CHANGELOG.encode())
    (tmp_path / "lib.tsv").write_text(LIBTSV)
    (tmp_path / "ev.md").write_text("result")
    for a, body in [("00426070", SRC_C), ("00426080", SRC_NAKED), ("004260a0", SRC_G26), ("004260b0", SRC_MIXED)]:
        (tmp_path / "src" / f"{a}.c").write_text(body.format(a=a))
    monkeypatch.setattr(m, "ROOT", tmp_path)
    monkeypatch.setattr(m, "HOOKS", tmp_path / "hooks.csv")
    monkeypatch.setattr(m, "CHANGELOG", tmp_path / "re" / "analysis" / "CHANGELOG.md")
    evals = {"src/00426070.c": ok_eval("00426070"), "src/00426080.c": ok_eval("00426080"),
             "src/004260a0.c": ok_eval("004260a0"), "src/004260b0.c": ok_eval("004260b0")}
    monkeypatch.setattr(m, "evaluate", lambda sources, jobs: {s: evals[s] for s in sources})
    m._evals = evals
    return m


def batch(m, tmp_path, rows, *extra):
    t = tmp_path / "batch.tsv"
    t.write_text("rva\troute\tevidence\n" + "".join("\t".join(r) + "\n" for r in rows))
    return m.main(["batch", "--tsv", str(t), "--date", "2026-11-07", *extra])


ROWS = [("0x00426070", "match", "src/00426070.c"), ("0x00426080", "match", "src/00426080.c"),
        ("0x00426090", "match", "src/00426070.c"), ("0x004260a0", "match", "src/004260a0.c"),
        ("0x004260b0", "match", "src/004260b0.c"), ("0x004b6fc0", "libid", "lib.tsv"),
        ("0x004a3090", "libid", "lib.tsv"), ("0x00499999", "match", "src/00426070.c")]


def test_dry_run_writes_nothing(mod, tmp_path, capsys):
    assert batch(mod, tmp_path, ROWS) == 0
    out = capsys.readouterr().out
    assert "[1/8] 0x00426070 match PASS" in out
    assert "[2/8] 0x00426080 match REFUSED naked function" in out
    assert "[3/8] 0x00426090 match SKIP already C4" in out
    assert "[4/8] 0x004260a0 match REFUSED body is 1 inline __asm block" in out
    assert "[5/8] 0x004260b0 match PASS" in out          # __asm inside real C work is allowed
    assert "[6/8] 0x004b6fc0 libid PASS identical _lua_gettop" in out
    assert "[7/8] 0x004a3090 libid REFUSED" in out
    assert "[8/8] 0x00499999 match REFUSED no hooks.csv row" in out
    assert "batch: 3 pass, 4 refused, 1 skipped" in out
    assert (tmp_path / "hooks.csv").read_bytes().decode() == HOOKS


def test_apply_touches_only_promoted_lines(mod, tmp_path):
    assert batch(mod, tmp_path, ROWS, "--apply", "--result", "ev.md") == 0
    new = (tmp_path / "hooks.csv").read_bytes().decode().split("\r\n")
    old = HOOKS.split("\r\n")
    changed = [i for i in range(len(old)) if old[i] != new[i]]
    assert changed == [2, 6, 7]
    assert new[2].startswith("00426070,TrackPhysWorld2Get,render,C4,impl,a.cpp,,,\"over, quoted | 2026-11-07 C4 by route match (L-93): c4=match; match.py src/00426070.c --bytes BYTE-IDENTICAL 12 B")
    assert "relocheck OK checked=2 unchecked=1 [__ftol]" in new[2]
    assert new[7].startswith("004b6fc0,lua_gettop,third-party-library[lua-4.0],C4,mapped")
    assert "c4=libid; libmatch lib.tsv: identical lapi.obj _lua_gettop" in new[7]
    cl = (tmp_path / "re" / "analysis" / "CHANGELOG.md").read_bytes().decode().split("\r\n")
    assert cl[2] == "<!-- ENTRIES -->"
    assert cl[3].startswith("2026-11-07  0x00426070  TrackPhysWorld2Get  C3->C4  route match (L-93)")
    assert cl[4].startswith("2026-11-07  0x004260b0  FUN_004260b0  C2->C4")
    assert cl[5].startswith("2026-11-07  0x004b6fc0  lua_gettop  C2->C4  route libid (L-93)")
    assert cl[6] == "2026-11-01  old entry"


def test_apply_needs_result(mod, tmp_path):
    with pytest.raises(SystemExit):
        batch(mod, tmp_path, ROWS, "--apply")


def test_strict_writes_nothing_on_refusal(mod, tmp_path):
    assert batch(mod, tmp_path, ROWS, "--apply", "--strict", "--result", "ev.md") == 1
    assert (tmp_path / "hooks.csv").read_bytes().decode() == HOOKS


@pytest.mark.parametrize("ev,needle", [
    (dict(bad=3), "not byte-identical"),
    (dict(reloc="BAD"), "relocheck BAD"),
    (dict(acc=0.9), "shorter than the original body"),
])
def test_match_refusals(mod, tmp_path, capsys, ev, needle):
    mod._evals["src/00426070.c"] = ok_eval("00426070", **ev)
    batch(mod, tmp_path, [("0x00426070", "match", "src/00426070.c")])
    assert needle in capsys.readouterr().out


def test_toolchain_must_be_ddk(mod, tmp_path, capsys):
    mod._evals["src/00426070.c"]["_toolchain"] = "vc19"
    batch(mod, tmp_path, [("0x00426070", "match", "src/00426070.c")])
    assert "is not the recorded compiler" in capsys.readouterr().out


def test_second_apply_is_a_noop(mod, tmp_path, capsys):
    batch(mod, tmp_path, ROWS, "--apply", "--result", "ev.md")
    h = (tmp_path / "hooks.csv").read_bytes()
    batch(mod, tmp_path, ROWS, "--apply", "--result", "ev.md")
    assert (tmp_path / "hooks.csv").read_bytes() == h
    assert "batch: 0 pass" in capsys.readouterr().out.splitlines()[-2]
